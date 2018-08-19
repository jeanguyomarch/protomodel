/* Protomodel - MIT License */

#ifndef PROTOMODEL_CONTEXT_H__
#define PROTOMODEL_CONTEXT_H__

#include "protomodel/protomodel.h"
#include "protomodel/error.h"
#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <mstch/mstch.hpp>
#include <exception>

namespace protomodel {

inline std::string get_object_typename(const flatbuffers::StructDef *obj)
{
  std::string type_str;
  if (obj->defined_namespace)
  {
    for (const auto &ns : obj->defined_namespace->components)
    { type_str += ns + "::"; }
  }
  type_str += obj->name;
  if (! obj->fixed)
  { type_str += 'T'; } /* FIXME 'T' may change */

  return type_str;
}


template<typename T>
class TableValue : public mstch::object
{
public:
  TableValue(const flatbuffers::FieldDef *field) :
    _field{ field }
  {
     mstch::object::register_methods(this, {
      {"name", &TableValue<T>::name},
      {"py_name", &TableValue<T>::py_name},
      {"type", &TableValue<T>::type},
      {"required", &TableValue<T>::required},
      {"at_least", &TableValue<T>::at_least},
      {"at_most", &TableValue<T>::at_most},
      {"has_range", &TableValue<T>::has_range},
    });
  }

private:
  mstch::node name()
  { return _field->name; }

  mstch::node py_name()
  { return get_pascal_case(_field->name); }

  mstch::node required()
  { return (_field->attributes.Lookup("key")) ? false : _field->required; }

  mstch::node at_least()
  { return 1; } /* TODO */

  mstch::node at_most()
  { return INT_MAX; } /* TODO */

  mstch::node has_range()
  { return std::is_integral<T>::value; }

  mstch::node type()
  { return TypeName<T>::name; }

private:
  const flatbuffers::FieldDef *const _field;
};

/*****************************************************************************/

class TableMap : public mstch::object
{
public:
  TableMap(const flatbuffers::FieldDef *obj) :
    _base{ obj },
    _obj{ obj->value.type.struct_def }
  {
    register_methods(this, {
      {"name", &TableMap::name},
      {"py_name", &TableMap::py_name},
      {"key_name", &TableMap::key_name},
      {"key_type", &TableMap::key_type},
      {"value_type", &TableMap::value_type},
      {"value_obj_type", &TableMap::value_obj_type},
      {"at_least", &TableMap::at_least},
      {"at_most", &TableMap::at_most},
    });
  }

private:
  mstch::node name()
  { return _base->name; }

  mstch::node py_name()
  { return get_pascal_case(_base->name); }

  mstch::node key_type()
  { return TypeName<std::string>::name; } // FIXME scalar

  mstch::node key_name()
  {
    for (const flatbuffers::FieldDef *field : _obj->fields.vec)
    {
      if (field->attributes.Lookup("key"))
      { return field->name; }
    }
    throw std::invalid_argument("Failed to find a 'key' attribute");
  }

  mstch::node value_obj_type()
  { return get_object_typename(_obj); }

  mstch::node value_type()
  { return _obj->name; }

  mstch::node at_least()
  { return 1; } // TODO

  mstch::node at_most()
  { return INT_MAX; } // TODO

private:
  const flatbuffers::FieldDef *const _base;
  const flatbuffers::StructDef *const _obj;
};

/*****************************************************************************/

class TableObject : public mstch::object
{
public:
  TableObject(const flatbuffers::FieldDef *field) :
    _base{ field },
    _obj{ field->value.type.struct_def }
  {
    register_methods(this, {
      {"name", &TableObject::name},
      {"py_name", &TableObject::py_name},
      {"type", &TableObject::type},
      {"obj_type", &TableObject::obj_type},
      {"required", &TableObject::required},
      {"at_least", &TableObject::at_least},
      {"at_most", &TableObject::at_most},
    });
  }

private:
  mstch::node name()
  { return _base->name; }

  mstch::node py_name()
  { return get_pascal_case(_base->name); }

  mstch::node type()
  { return _obj->name; }

  mstch::node obj_type()
  { return get_object_typename(_obj); }

  mstch::node required()
  {
    if (_obj->fixed) /* 'struct' in the IDL */
    { return true; }

    for (const auto *val : _obj->attributes.vec)
   { std::cerr << "attr: " << val->constant << std::endl;
     if (val->constant == "required")
     { return true; }
   }

    return false; // FIXME
  }

  mstch::node at_least()
  { return 1; } // TODO

  mstch::node at_most()
  { return INT_MAX; } // TODO

private:
  const flatbuffers::FieldDef *const _base;
  const flatbuffers::StructDef *const _obj;
};

/*****************************************************************************/

class Table : public mstch::object
{
public:
  Table(const flatbuffers::StructDef *obj) :
    _obj{ obj }
  {
    register_methods(this, {
      {"table_name", &Table::name},
      {"table_type", &Table::type},
      {"values", &Table::values},
      {"repeated_values", &Table::repeated_values},
      {"objects", &Table::objects},
      {"repeated_objects", &Table::repeated_objects},
      {"maps", &Table::maps},
    });
  }

  template<typename T>
  void add_value(const flatbuffers::FieldDef *field,
                 bool repeated)
  {
    auto &array = (repeated) ? _repeated_values : _values;
    array.emplace_back(std::make_shared<TableValue<T>>(field));
  }

  void add_object(const flatbuffers::FieldDef *field,
                  bool repeated)
  {
    auto &array = (repeated) ? _repeated_objects : _objects;
    array.emplace_back(std::make_shared<TableObject>(field));
  }

  void add_map(const flatbuffers::FieldDef *field)
  { _maps.emplace_back(std::make_shared<TableMap>(field)); }

private:
  mstch::node name()
  { return _obj->name; }

  mstch::node type()
  { return get_object_typename(_obj); }

  mstch::node values()
  { return _values; }

  mstch::node repeated_values()
  { return _repeated_values; }

  mstch::node objects()
  { return _objects; }

  mstch::node repeated_objects()
  { return _repeated_objects; }

  mstch::node maps()
  { return _maps; }

private:
  mstch::array _values;
  mstch::array _repeated_values;
  mstch::array _objects;
  mstch::array _repeated_objects;
  mstch::array _maps;
  const flatbuffers::StructDef *const _obj;
};

/*****************************************************************************/

class Include : public mstch::object
{
public:
  Include(const std::string &name) :
    _name{ name }
  {
    register_methods(this, {
      {"name", &Include::name},
      {"name_uppercase", &Include::name_uppercase},
    });
  }

  mstch::node name()
  { return _name; }

  mstch::node name_uppercase()
  {
    std::string str{ _name };
    std::transform(str.begin(), str.end(),str.begin(), ::toupper);
    return str;
  }

private:
  const std::string _name;
};

/*****************************************************************************/

class Context : public mstch::object
{
public:
  Context(const std::string &model_name,
          const flatbuffers::IDLOptions &options) :
    _model_name{ model_name },
    _parser(options)
  {
    register_methods(this, {
      {"includes", &Context::includes},
      {"tables", &Context::tables},
      {"root_type", &Context::root_type},
      {"root_table", &Context::root_table},
      {"model_name", &Context::model_name},
      {"magic", &Context::magic},
    });
  }

public:
  void add_include(const std::string &name)
  { _includes.emplace_back(std::make_shared<Include>(name)); }

  std::shared_ptr<Table> add_table(const flatbuffers::StructDef *obj)
  {
    auto msg = std::make_shared<Table>(obj);
    _tables.push_back(msg);
    return msg;
  }

  Status load(const std::string &filename,
              const char *data,
              const std::vector<std::string> &include_dirs)
  {
    assert(nullptr != data);

    /* Create a char** holding the include directories */
    std::vector<const char *> dirs; /* FIXME pre-alloc size */
    for (const auto &include_dir : include_dirs)
    { dirs.push_back(include_dir.c_str()); }
    dirs.push_back(nullptr); /* Must be nullptr-terminated */

    if (! _parser.Parse(data, dirs.data()))
    { return error("Failed to parse '" + filename + "': " + _parser.error_); }

    if (! _parser.SetRootType("Config"))
    { return error("Failed to set 'Config' as the root type"); }

    if (_parser.namespaces_.size() > 2)
    { return error("Only one namespace is supported"); }

    return Ok();
  }

  const flatbuffers::Parser &parser() const noexcept
  { return _parser; }

private:
  mstch::node model_name()
  { return _model_name; }

  mstch::node magic()
  { return _parser.file_identifier_; }

  mstch::node root_table()
  { return _parser.root_struct_def_->name; }

  mstch::node root_type()
  { return get_object_typename(_parser.root_struct_def_); }

  mstch::node includes()
  { return _includes; }

  mstch::node tables()
  { return _tables; }

private:
  const std::string _model_name;
  flatbuffers::Parser _parser;
  mstch::array _includes;
  mstch::array _tables;
};

} /* namespace protomodel */

#endif /* ! PROTOMODEL_CONTEXT_H__ */
