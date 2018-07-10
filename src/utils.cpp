/* Protomodel - MIT License */

#include "protomodel/protomodel.h"

#include <string>
#include <cstdint>

namespace protomodel {

template<> const std::string TypeName<int8_t>::name = "int8_t";
template<> const std::string TypeName<uint8_t>::name = "uint8_t";
template<> const std::string TypeName<int16_t>::name = "int16_t";
template<> const std::string TypeName<uint16_t>::name = "uint16_t";
template<> const std::string TypeName<uint32_t>::name = "uint32_t";
template<> const std::string TypeName<int32_t>::name = "int32_t";
template<> const std::string TypeName<uint64_t>::name = "uint64_t";
template<> const std::string TypeName<int64_t>::name = "int64_t";
template<> const std::string TypeName<float>::name = "float";
template<> const std::string TypeName<double>::name = "double";
template<> const std::string TypeName<bool>::name = "bool";
template<> const std::string TypeName<std::string>::name = "std::string";


std::string
get_model_name(const std::string &filename)
{
  /* XXX Use std::filesystem instead: this method will not work on systems
   * where the path separator can be something else than a forward slash
   *
   * If we have:
   *   - /file.proto
   *   - file.proto
   *   - a/path/with/file.meaningless_extension
   *   - ./file.pro
   *   - ...
   *
   * we want to always end up with 'file'.
   */

  /* Determine the start of the protobuf name */
  std::size_t start = filename.rfind("/"); /* For a basename */
  if (std::string::npos == start)
  { start = 0u; }
  else
  { start += 1u; }

  /* Set end to always be at the expected end of the filename */
  std::size_t end = filename.rfind(".");  /* To strip extension */
  if ((std::string::npos == end) || (end < start))
  { end = filename.size(); }

  return filename.substr(start, end - start);
}



} /* namespace protomodel */
