#include <string.h>

struct CTempString
{
  CTempString()
      : _buffer{nullptr}
  {}

  template<typename... __arg_pkg>
  CTempString(const char* format, __arg_pkg... args)
      : _buffer{nullptr}
  {
    size_t size = snprintf(nullptr, 0, format, args...);
    _buffer     = new char[size + 1];

    assert(_buffer != nullptr);
    snprintf(_buffer, size + 1, format, args...);

    _buffer[size] = 0;
  }

  CTempString(const CTempString& right)
      : _buffer{nullptr}
  {
    if(right._buffer != nullptr) {
      _buffer = strdup(right._buffer);
    }
  }

  CTempString(CTempString&& right_move)
  {
    _buffer            = right_move._buffer;
    right_move._buffer = nullptr;
  }

  CTempString& operator=(const CTempString& right)
  {
    delete _buffer;
    _buffer = nullptr;
    if(right._buffer != nullptr) {
      _buffer = strdup(right._buffer);
    }
    return *this;
  }

  CTempString& operator=(CTempString&& right_move)
  {
    _buffer            = right_move._buffer;
    right_move._buffer = nullptr;
    return *this;
  }

  ~CTempString()
  {
    delete _buffer;
    _buffer = nullptr;
  }

  inline operator char*() const { return _buffer; }

private:
  char* _buffer;
};
