/* Copyright (C) 2016-2018 NSEIT Limited, Mumbai, India. All rights
 * reserved.
 *
 * This program and the accompanying materials are made available
 * under the terms described in the LICENSE file which accompanies
 * this distribution. If the LICENSE file was not attached to this
 * distribution or for further clarifications, please contact
 * legal@nseit.com. */

#include <stdlib.h>

#include "buffer.h"

NAMESPACE__NSEIT__START

Buffer::Buffer(size_t length)
{
  m_pointer = malloc(length);
  m_length = length;
}

Buffer::Buffer(Buffer&& other)
{
  m_pointer = other.m_pointer;
  m_length = other.m_length;
  other.m_pointer = nullptr;
  other.m_length = 0;
}

Buffer&
Buffer::operator=(Buffer&& other) noexcept
{
  if (this != &other) {
    if (m_pointer != nullptr) {
      free(m_pointer);
    }

    m_pointer = other.m_pointer;
    m_length = other.m_length;

    other.m_pointer = nullptr;
    other.m_length = 0;
  }

  return *this;
}

Buffer::~Buffer()
{
  if (m_pointer != nullptr) {
    free(m_pointer);
  }
}

NAMESPACE__NSEIT__END

/*
  Local Variables:
  mode: c++
  End:
*/
