/* Copyright (C) 2016-2018 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#ifndef NSEIT_CPP_BASE__ef6de134_c590_11e9_95c0_d43b04f4acea
#define NSEIT_CPP_BASE__ef6de134_c590_11e9_95c0_d43b04f4acea

#include <stddef.h>

#include "_namespace.h"

NAMESPACE__NSEIT__START

// Buffer: Move-only buffer
//
// This class is intentionally simple. Its simplicity introduces
// contraints that limit its use for many things. This was done
// intentionally. The properties of this Buffer are:
//
//   1. The object can either be empty or contain a buffer of given
//   length.
//
//   2. The object shall not be copyable. It can only be moved.
//
//   3. After the object has been constructed, the internally held
//   pointer should not be changeable. We dont want it to
//   increase/decrease size, etc.
//
// It is wrong to take a reference or a pointer to this object. In
// that case, almost always you are doing something it was not
// intended for.
//
// This class and its constraints allows us to efficiently move
// buffers across layers of network code without copying. By not
// allowing copying, we'll be easily avoiding a lot of mistakes that
// developers do. Further, we'd also be able to catch wrong use of the
// Buffer in code-reviews, which is usually a symptom of a larger
// problem.

class Buffer final
{
public:
  ~Buffer();
  Buffer() {}
  Buffer(size_t length);
  Buffer(Buffer&& buffer);

  Buffer& operator=(Buffer&& other) noexcept;
  explicit operator bool() const
  {
    return m_pointer != nullptr;
  }

  void* pointer(void) const
  {
    return m_pointer;
  }

  size_t length(void) const
  {
    return m_length;
  }

private:
  void* m_pointer = nullptr;
  size_t m_length = 0;
};

NAMESPACE__NSEIT__END

#endif // NSEIT_CPP_BASE__ef6de134_c590_11e9_95c0_d43b04f4acea

/*
  Local Variables:
  mode: c++
  End:
*/
