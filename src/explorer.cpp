/* Protomodel - MIT License */

#include "protomodel/protomodel.h"
#include "protomodel/context.h"
#include "protomodel/error.h"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <unordered_set>

namespace protomodel {

static Status
_explore(std::shared_ptr<Context> &context,
         const flatbuffers::StructDef *obj);

static Status
_explore_type(std::shared_ptr<Context> &context,
              std::shared_ptr<Table> &table,
              const flatbuffers::StructDef *source,
              const flatbuffers::FieldDef *field)
{
  const flatbuffers::Type &type = field->value.type;
  const flatbuffers::BaseType base_type =
    (type.base_type == flatbuffers::BASE_TYPE_VECTOR)
    ? type.element : type.base_type;

  /* We are in a repeated type IFF the base type is a vector type */
  const bool repeated = type.base_type == flatbuffers::BASE_TYPE_VECTOR;


//  const flatbuffers::BaseType base_type = type.base_type;
  switch (base_type)
  {
    case flatbuffers::BASE_TYPE_BOOL:
      table->add_boolean_value(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_CHAR:
      table->add_numerical_value<int8_t>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_UCHAR:
      table->add_numerical_value<uint8_t>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_SHORT:
      table->add_numerical_value<int16_t>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_USHORT:
      table->add_numerical_value<uint16_t>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_INT:
      table->add_numerical_value<int32_t>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_UINT:
      table->add_numerical_value<uint32_t>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_LONG:
      table->add_numerical_value<int64_t>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_ULONG:
      table->add_numerical_value<uint64_t>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_FLOAT:
      table->add_numerical_value<float>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_DOUBLE:
      table->add_numerical_value<double>(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_STRING:
      table->add_string_value(field, repeated);
      break;
    case flatbuffers::BASE_TYPE_VECTOR:
      /* We shouldn't be able to repeat a vector type */
      assert(! repeated && "Repeated vector type is weird");
      ECHECK(_explore_type(context, table, source, field));
      /* FIXME handle map values */
      break;

    case flatbuffers::BASE_TYPE_UTYPE:
      break; // FIXME TODO Enum

    case flatbuffers::BASE_TYPE_STRUCT:
      if (field->attributes.Lookup("map"))
      {
        assert(repeated && "Cannot apply a map on non-repeated elements");
        table->add_map(field);
      }
      else
      { table->add_object(field, repeated); }
      ECHECK(_explore(context, type.struct_def));
      break;

    default:
      return error("Unhandled flatbuffers type '" +
                   std::string(flatbuffers::kTypeNames[base_type]) +
                   std::string("'"));
  }
  return Ok();
}

static Status
_explore(std::shared_ptr<Context> &context,
         const flatbuffers::StructDef *obj)
{
  /* Don't visit the same object twice */
  static std::unordered_set<const flatbuffers::StructDef *> objects;
  if (objects.find(obj) != objects.end())
  { return Ok(); }
  objects.insert(obj);

  /* We have one more table :) */
  std::shared_ptr<Table> table = context->add_table(obj);

  for (const flatbuffers::FieldDef *field : obj->fields.vec)
  {
     ECHECK(_explore_type(context, table, obj, field));
  }
  return Ok();
}

std::shared_ptr<Context>
load(const std::string &filename)
{
  /* Create an IDL parsing context */
  auto opts = flatbuffers::IDLOptions();
  opts.lang = flatbuffers::IDLOptions::kCpp;
  const auto model_name = get_model_name(filename);
  auto context = std::make_shared<Context>(model_name, opts);
  if (! context)
  { return nullptr; }

  /* Load the IDL file into memory */
  using namespace boost::interprocess;
  const file_mapping file(filename.c_str(), read_only);
  const mapped_region region(file, read_only);
  const char *const data = static_cast<const char*>(region.get_address());

  /* Load the context */
  if (! context->load(filename, data).check())
  { return nullptr; }

  /* We will always include the <model>_generated.h header */
  const std::string model_include{ model_name + "_generated.h" };
  context->add_include(model_include);

  /* And explore the IDL graph... */
  const auto &p = context->parser();
  auto err = _explore(context, p.root_struct_def_);
  if (! err.check())
  { return nullptr; }

  /* Return a valid context */
  return context;
}

} /* namespace protomodel */
