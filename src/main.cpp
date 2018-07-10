/* Protomodel - MIT License */

#include "protomodel/protomodel.h"
#include "protomodel/context.h"
#include "protomodel/error.h"

#include <cxxopts.hpp>
#include <mstch/mstch.hpp>

#include <sstream>
#include <istream>
#include <fstream>
#include <ostream>
#include <iostream>
#include <exception>
#include <cstdlib>

/*****************************************************************************/

namespace protomodel {

static Status
run(int argc, char **argv)
{
  /***************************************************************************/
  /* Getopts */
  std::string file;
  std::string name_space;
  std::vector<std::string> templates;
  std::vector<std::string> outputs;
  std::vector<std::string> include_dirs;

  cxxopts::Options options(
    "protomodel", "Data Model generator from a flatbuffers interface\n");
  options.show_positional_help();
  options
    .positional_help("file")
    .add_options()
    ("h,help", "Display this message")
    ("V,version", "Display protomodel's version")
    ("o,output", "Output file in which the generated sources will be stored",
      cxxopts::value<std::vector<std::string>>(outputs))
    ("t,template", "Template to be used to generate the data model loader",
      cxxopts::value<std::vector<std::string>>(templates))
    ("I", "Flatbuffers include directories",
      cxxopts::value<std::vector<std::string>>(include_dirs))
    ("file", "Flatbuffers interface",
      cxxopts::value<std::string>(file))
  ;
  options.parse_positional({"file"});
  auto result = options.parse(argc, argv);
  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    return Ok();
  }
  else if (result.count("version"))
  {
    std::cout << "protomodel v" VERSION << std::endl;
    return Ok();
  }
  else if (file.empty())
  { return error("protomodel requires a positional argument (see --help)"); }

  if (! result.count("template"))
  { return error("protomodel requires the --template (-t) option"); }
  if (result.count("template") != result.count("output"))
  { return error("you must provide one --output (-o) per --template (-t)"); }


  /***************************************************************************/
  /* Templating */
  const auto context = load(file, include_dirs);
  if (! context)
  { return error("protomodel context creation failed"); }

  assert(templates.size() == outputs.size());
  for (size_t i = 0u; i < templates.size(); i++)
  {
    std::ifstream in_file{ templates[i] };
    std::stringstream buffer;
    buffer << in_file.rdbuf();

    std::ofstream outfile{ outputs[i] };
    outfile << mstch::render(buffer.str(), context);
    outfile.close();
  }

  return Ok();
}

} /* namespace protomodel */

int
main(int argc,
     char **argv)
{
  try
  {
    return (protomodel::run(argc, argv).check()) ? EXIT_SUCCESS : EXIT_FAILURE;
  }
  catch (const cxxopts::missing_argument_exception &e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return EXIT_FAILURE;
}
