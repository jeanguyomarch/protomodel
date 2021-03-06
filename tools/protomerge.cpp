/* Protomodel - MIT License */

#include "protomodel/error.h"

#include <cxxopts.hpp>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
# define unlikely(X) __builtin_expect((X), 0)
# else
# define unlikely(X) (X)
#endif

using namespace protomodel;

namespace protomerge {

class Error : public std::exception
{
public:
  Error(const char *motive) : _motive{ motive } {}
  const char *what() const noexcept
  { return _motive; }

private:
  const char *const _motive;
};

struct Table
{
  const std::string name;
  const char *const table_start;
  const char *const table_end;
  const char *const inner_start;
  const char *const inner_end;
  const bool extends;
  mutable bool used;

  Table() = delete;

  explicit
  Table(const char *start, const char *end,
        const char *nstart, const char *nend,
        const char *tstart, const char *tend,
        bool does_extend) :
    name{ nstart, static_cast<size_t>(nend - nstart) },
    table_start{ start }, table_end{ end },
    inner_start{ tstart }, inner_end{ tend },
    extends{ does_extend }, used{ false }
  {}
};

class Parser
{
public:
  Parser(const std::string &file) :
    _mapping{ file.c_str(), boost::interprocess::read_only },
    _region{ _mapping, boost::interprocess::read_only },
    _start{ static_cast<const char *>(_region.get_address()) },
    _end{ _start + _region.get_size() },
    _p{ _start },
    _in_comments{ false }
  {}

private:
  void next(char token)
  {
    while (((*_p != token) && (! _in_comments)) || (_in_comments))
    {
      check_comments();
      _p++;
      check_eof();
    }
  }

  void skip_whitespaces()
  {
    while (isspace(*(_p++)))
    { check_eof(); }
  }

  bool check_comments()
  {
    if ((*_p == '/') && (*(++_p) == '/'))
    { _in_comments = true; }
    else if (*_p == '\n')
    { _in_comments = false; }
    return _in_comments;
  }

  void check_eof() const
  {
    if (unlikely(_p >= _end))
    { throw Error("Unexpected end of file"); }
  }

public:
  void parse()
  {
    for (;;)
    {
      if (! check_comments()) /* Returns true in comments */
      {
        if (0 == strncmp("table ", _p, 6)) /* Found 'table ' somewhere */
        {
          const bool extends =
            ((_p >= _start + 8u) && (0 == strncmp("@extend ", _p - 8u, 8u)));

          const char *const start = _p;

          _p += sizeof("table ") - 1; /* Go after table + space */

          skip_whitespaces();
          const char *const name_start = _p - 1; /* Start of the table name */
          next(' '); /* XXX Whitespace or start of comment */
          const char *const name_end = _p; /* End of the table name */
          _p++; /* Go after the whitespace */
          next('{'); _p++; /* Go after the initial { */
          const char *const inner_start = _p; /* Start of the table contents */
          next('}'); /* Go at the end of the table */
          const char *const inner_end = _p; /* Before the end of the table */
          _p += 2u; /* Go after the } */
          const char *const end = _p;

          /* Create the table */
          const auto table =
            std::make_shared<Table>(start, end, name_start, name_end,
                                    inner_start, inner_end, extends);
          _tables.push_back(table);
          _map.emplace(table->name, table);
        }
      }

      /* Move on... Maybe stop parsing as well. */
      _p++;
      if (unlikely(_p >= _end))
      { break; }
    }
  }

public:
  const std::vector<std::shared_ptr<Table>> tables() const noexcept
  { return _tables; }

  std::shared_ptr<Table> get_table(const std::string &name) const noexcept
  {
    const auto it = _map.find(name);
    if (unlikely(it == _map.end()))
    { return nullptr; }
    else
    { return it->second; }
  }

  const char *start() const noexcept
  { return _start; }

  const char *end() const noexcept
  { return _end; }

  size_t size() const noexcept
  { return static_cast<size_t>(_end - _start); }

private:
  boost::interprocess::file_mapping _mapping;
  boost::interprocess::mapped_region _region;

  const char *const _start;
  const char *const _end;
  const char *_p; /**< Pointer iterator */
  bool _in_comments;

  std::unordered_map<std::string, std::shared_ptr<Table>> _map;
  std::vector<std::shared_ptr<Table>> _tables;
};


static Status
merge(const std::vector<std::string> &files,
      FILE *output)
{
  assert(files.size() == 2);

  const std::string &base_file = files.at(0);
  const std::string &ext_file = files.at(1);

  Parser p_base{ base_file };
  p_base.parse();
  Parser p_ext{ ext_file };
  p_ext.parse();

  fprintf(output,
          "// Flatbuffers model generated by protomerge\n"
          "// Base Model.......: %s\n"
          "// Extension Model..: %s\n\n",
          base_file.c_str(), ext_file.c_str());

  const char *base_ptr = p_base.start();

#define WRITE(Ptr, Size) \
  ({ \
    const size_t size___ = static_cast<size_t>(Size); \
    fwrite((Ptr), sizeof(char), size___, output); \
    (Ptr) + size___; \
  })

  for (const std::shared_ptr<Table> &t : p_base.tables())
  {
    /* Write to the end of the base table */
    base_ptr = WRITE(base_ptr, t->inner_end - base_ptr);

    std::shared_ptr<Table> ext = p_ext.get_table(t->name);
    if (! ext) /* No extension table: stop for this table */
    { continue; }
    else if (! ext->extends)
    { return error("Table '" + ext->name + "' overrides an existing table"); }
    ext->used = true;

    fprintf(output, "\n  // Declared in %s:\n", ext_file.c_str());
    WRITE(ext->inner_start, ext->inner_end - ext->inner_start);
  }
  WRITE(base_ptr, p_base.end() - base_ptr);

  fprintf(output, "\n// Tables declared in %s:\n\n", ext_file.c_str());
  for (const std::shared_ptr<Table> &ext : p_ext.tables())
  {
    if (ext->extends)
    {
      if (! ext->used)
      {
        return error("Table '" + ext->name + "' is tagged '@extend' "
                     "but extends nothing");
      }
      continue;
    }
    WRITE(ext->table_start, ext->table_end - ext->table_start);
  }
  return Ok();
#undef WRITE
}

static Status
run(int argc,
    char **argv)
{
  std::vector<std::string> files;
  std::string output;

  cxxopts::Options options(
    "protomerge", "Merge two flatbuffers models by aggregating them");
  options.show_positional_help();
  options
    .positional_help("files...")
    .add_options()
    ("h,help", "Display this message")
    ("V,version", "Display protomerge's version")
    ("o,output", "Output of the merged model",
     cxxopts::value<std::string>(output))
    ("files", "Flatbuffers Interface Files",
     cxxopts::value<std::vector<std::string>>(files))
  ;
  options.parse_positional({"files"});

  auto result = options.parse(argc, argv);
  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    return Ok();
  }
  else if (result.count("version"))
  {
    std::cout << "protomerge v" VERSION << std::endl;
    return Ok();
  }
  else if (output.empty())
  { return error("protomerge required the --output,-o option"); }
  else if (files.size() <= 1u)
  { return error("protomerge requires positional arguments (see --help)"); }
  else if (files.size() != 2u)
  { return error("for now, protomerge can only merge two files together"); }

  /* Open the output FILE descriptor. As a unique_ptr, to handle exceptions */
  std::unique_ptr<FILE, decltype(&fclose)> file{
    fopen(output.c_str(), "w"), fclose
  };
  if (! file)
  { return error("Failed to open file '" + output + "'"); }

  if (! merge(files, file.get()).check())
  { return error("Failed to merge flatbuffers interfaces"); }

  return Ok();
}

} /* namespace protomerge */

int
main(int argc,
     char **argv)
{
  try
  {
    return (protomerge::run(argc, argv).check()) ? EXIT_SUCCESS : EXIT_FAILURE;
  }
  catch (const boost::interprocess::interprocess_exception &e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  catch (const cxxopts::missing_argument_exception &e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return EXIT_FAILURE;
}
