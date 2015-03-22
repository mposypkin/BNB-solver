#pragma once

#include <assert.h>
#include <string.h>
#include <vector>

namespace dmc
{

//BinarySerializer for compsuters with same architectures
class BinarySerializer
{
public:
  BinarySerializer(size_t buffer_size = 1024): buffer_(new char[buffer_size]), buffer_size_(buffer_size), buffer_pos_(0)
  {
  }

  BinarySerializer(const char *buffer, size_t buffer_size): buffer_(new char[buffer_size]), buffer_size_(buffer_size), buffer_pos_(0)
  {
    memcpy(buffer_, buffer, buffer_size_);
  }

  BinarySerializer(const BinarySerializer &rhs): buffer_(new char[rhs.buffer_size_]), buffer_size_(rhs.buffer_size_), buffer_pos_(rhs.buffer_pos_)
  {
    memcpy(buffer_, rhs.buffer_, buffer_size_);
  }

  BinarySerializer &operator=(const BinarySerializer &rhs)
  {
    char *buffer = new char[rhs.buffer_size_];
    memcpy(buffer, rhs.buffer_, rhs.buffer_size_);
    delete [] buffer_;
    buffer_ = buffer;
    buffer_size_ = rhs.buffer_size_;
    buffer_pos_ = rhs.buffer_pos_;
    return *this;
  }

  ~BinarySerializer()
  {
    delete [] buffer_;
  }

  void reset()
  {
    buffer_pos_ = 0;
  }

  char *get()
  {
    return buffer_;
  }

  const char *get() const
  {
    return buffer_;
  }

  size_t size() const
  {
    return buffer_size_;
  }

  void resize(size_t new_size)
  {
    if(new_size > buffer_size_)
    {
      char *buffer = new char[new_size];
      memcpy(buffer, buffer_, buffer_pos_);
      delete [] buffer_;
      buffer_ = buffer;
      buffer_size_ = new_size;
    }
  }

  void pack(const char *data, size_t size)
  {
    if(buffer_pos_ + size > buffer_size_)
    {
      size_t new_size = 2*buffer_size_;
      resize(new_size);
    }
    memcpy(buffer_+buffer_pos_, data, size);
    buffer_pos_ += size;
  }

  void unpack(char *data, size_t size)
  {
    assert(buffer_pos_ + size <= buffer_size_);
    memcpy(data, buffer_+buffer_pos_, size);
    buffer_pos_ += size;
  }

  template <typename T> void packArray(const T *array, size_t size)
  {
    pack(size);
    for(size_t i = 0; i < size; ++i)
    {
      pack(array[i]);
    }
  }

  template <typename T> void unpackArray(T *&array, size_t &size)
  {
    unpack(size);
    array = new T[size];
    for(size_t i = 0; i < size; ++i)
    {
      unpack(array[i]);
    }
  }

  //defalt realization for integrated types and plane c structures
  template <typename T> void pack(const T &data)
  {
    pack((const char *)&data, sizeof(T));
  }

  //defalt realization for integrated types and plane c structures
  template <typename T> void unpack(T &data)
  {
    unpack((char *)&data, sizeof(T));
  }

  template <typename T> void pack(const std::vector<T> &val)
  {
    size_t size = val.size();
    pack(size);
    for(size_t i = 0; i < size; ++i)
    {
      pack(val[i]);
    }
  }

  template <typename T> void unpack(std::vector<T> &val)
  {
    size_t size;
    unpack(size);
    val.reserve(size);
    for(size_t i = 0; i < size; ++i)
    {
      T element;
      unpack(element);
      val.push_back(element);
    }
  }

  template <typename T> BinarySerializer & operator << (const T & data)
  {
    pack (data);
    return *this;
  }

  template <typename T> BinarySerializer & operator >> (T & data)
  {
    unpack (data);
    return *this;
  }
private:
  char *buffer_;
  size_t buffer_size_;
  size_t buffer_pos_;
};

}
