/* Protomodel - MIT License */

#ifndef PROTOMODEL_ERRORS_H__
#define PROTOMODEL_ERRORS_H__

#include <string>
#include <cassert>
#include <iostream>

namespace protomodel {

class Status
{
public:
  explicit Status(bool is_error) :
    is_error_{ is_error },
    has_been_checked_{ false }
  {}

  ~Status()
  { assert(has_been_checked_ && "Unchecked Status detected"); }

  bool check() noexcept
  {
    has_been_checked_ = true;
    return ! is_error_;
  }

  Status &operator=(const Status &other) {
    is_error_ = other.is_error_;
    has_been_checked_ = false;
    other.has_been_checked_ = true;
    return *this;
  }

  Status(const Status &other)
  { *this = other; }

private:
  bool is_error_;
  mutable bool has_been_checked_;
};

class Ok : public Status
{
public:
  Ok() : Status(false) {}
};

inline Status error(const std::string &message)
{
  std::cerr << "ERROR: " << message << std::endl;
  return Status(true);
}

#define ECHECK(Call_)                                           \
  do {                                                          \
    auto check_error___ = (Call_);                              \
    if (! check_error___.check()) { return check_error___; }    \
  } while (0)

} /* namespace protomodel */

#endif /* PROTOMODEL_ERRORS_H__ */
