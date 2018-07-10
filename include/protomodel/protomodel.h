/* Protomodel - MIT License */

#ifndef PROTOMODEL_PROTMODEL_H__
#define PROTOMODEL_PROTMODEL_H__

#include <memory>
#include <string>

namespace protomodel {

class Context;

/**
 * Load a flatbuffer interface file @p filename.
 *
 * @param[in] filename Path to the flatbuffer interface
 * @return A shared pointer to a protomodel context. Will return nullptr on
 *   failure.
 */
std::shared_ptr<Context> load(const std::string &filename);

/**
 * Retrieve the name of the model from its definition filename
 *
 * @param[in] filename Path to the model definition
 * @return The name of the model
 */
std::string get_model_name(const std::string &filename);

/**
 * Utility template that allows to retrieve a textual description of a native
 * type.
 *
 * @code
 * std::cout << "String for type 'int' is " << TypeName<int>::name;
 * @endcode
 */
template<typename T>
struct TypeName
{ static const std::string name; /**< Type name */ };

} /* namespace protomodel */

#endif /* ! PROTOMODEL_PROTMODEL_H__ */
