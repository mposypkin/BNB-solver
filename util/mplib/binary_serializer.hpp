/*
 * File:   binary_serializer.hpp
 * Author: Alexander Ignatyev
 *
 * Created on April 21, 2011
 *
 * binary serializer
 */


#ifndef BINARY_SERIALIZER_HPP
#define	BINARY_SERIALIZER_HPP

#include <assert.h>
#include <string.h>
#include <string>
#include <vector>
#include "buffer.hpp"

/**
 * binary serializer for computers with same architectures
 */
class BinarySerializer : public Buffer {
public:

    /**
     * constructor
     * @param buffer_size the size in bytes of a serilizer buffer
     */
    BinarySerializer(size_t buffer_size = 1024) : buffer_(new char[buffer_size]), buffer_size_(buffer_size), buffer_pos_(0) {
    }

    /**
     * constructor
     * @param buffer the pointer to data buffer
     * @param buffer_size the size in bytes of the buffer
     */
    BinarySerializer(const char *buffer, size_t buffer_size) : buffer_(new char[buffer_size]), buffer_size_(buffer_size), buffer_pos_(0) {
        memcpy(buffer_, buffer, buffer_size_);
    }

    /**
     * copy constructor
     */
    BinarySerializer(const BinarySerializer &rhs) : buffer_(new char[rhs.buffer_size_]), buffer_size_(rhs.buffer_size_), buffer_pos_(rhs.buffer_pos_) {
        memcpy(buffer_, rhs.buffer_, buffer_size_);
    }

    /**
     * operator =
     */
    BinarySerializer & operator=(const BinarySerializer &rhs) {
        char *buffer = new char[rhs.buffer_size_];
        memcpy(buffer, rhs.buffer_, rhs.buffer_size_);
        delete [] buffer_;
        buffer_ = buffer;
        buffer_size_ = rhs.buffer_size_;
        buffer_pos_ = rhs.buffer_pos_;
        return *this;
    }

    /**
     * destructor
     */
    ~BinarySerializer() {
        delete [] buffer_;
    }

    /**
     * remove all data from buffer
     */
    void reset() {
        buffer_pos_ = 0;
    }

    /**
     * Returns the pointer to the first byte of the buffer
     * @return data poiner
     */
    virtual void *data() const {
        return (void *) buffer_;
    }

    /**
     * Returns buffer size in bytes
     * @return buffer size
     */
    virtual int size() const {
        //return buffer_size_;
        return buffer_pos_ + 1;
    }

    /**
     * Changes the size of the buffer to the new size
     * @param new_size the new size of the buffer
     * @returns 0 for the successful reallocation and error code otherwise
     */
    virtual int resize(int new_size) {
        if (new_size > buffer_size_) {
            char *buffer = new char[new_size];
            memcpy(buffer, buffer_, buffer_pos_);
            delete [] buffer_;
            buffer_ = buffer;
            buffer_size_ = new_size;
        }
        return 0;
    }

    /**
     * Serialize from buffer @param data with size @param @size
     */
    void pack(const char *data, int size) {
        if (buffer_pos_ + size > buffer_size_) {
            int new_size = 2 * buffer_size_;
            resize(new_size);
        }
        memcpy(buffer_ + buffer_pos_, data, size);
        buffer_pos_ += size;
    }

    /**
     * Deserialize data size @param size too buffer @param data
     */
    void unpack(char *data, int size) {
        assert(buffer_pos_ + size <= buffer_size_);
        memcpy(data, buffer_ + buffer_pos_, size);
        buffer_pos_ += size;
    }

    /**
     * serialize the array @param array with the size @param size
     */
    template <typename T> void packArray(const T *array, size_t size) {
        pack(size);
        for (size_t i = 0; i < size; ++i) {
            pack(array[i]);
        }
    }

    /**
     * deserialize array
     */
    template <typename T> void unpackArray(T *&array, size_t &size) {
        unpack(size);
        array = new T[size];
        for (size_t i = 0; i < size; ++i) {
            unpack(array[i]);
        }
    }

    /**
     * defalt realization of serialization for integrated types and plane c structures
     */
    template <typename T> void pack(const T &data) {
        pack((const char *) &data, sizeof (T));
    }

    /**
     * defalt realization of deserialization for integrated types and plane c structures
     */
    template <typename T> void unpack(T &data) {
        unpack((char *) &data, sizeof (T));
    }

    /**
     * Serialize string
     * @param s string to serialize
     */
    void pack(const std::string &s) {
        int n = s.length();
        pack(n);
        for(int i = 0; i < n; i ++) {
            char c = s[i];
            pack(c);
        }
    }

    /**
     * Unpacks string
     * @param s
     */
    void unpack(std::string &s) {
        int n;
        unpack(n);
        for(int i = 0; i < n; i ++) {
            char c;
            unpack(c);
            s += c;
        }
    }

    /**
     * serialize std::vector<T>
     */
    template <typename T> void pack(const std::vector<T> &val) {
        size_t size = val.size();
        pack(size);
        for (size_t i = 0; i < size; ++i) {
            pack(val[i]);
        }
    }

    /**
     * deserialize std::vector<T>
     */
    template <typename T> void unpack(std::vector<T> &val) {
        size_t size;
        unpack(size);
        val.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            T element;
            unpack(element);
            val.push_back(element);
        }
    }

    template <typename T> BinarySerializer & operator <<(const T & data) {
        pack(data);
        return *this;
    }

    template <typename T> BinarySerializer & operator >>(T & data) {
        unpack(data);
        return *this;
    }
private:
    char *buffer_;
    int buffer_size_;
    int buffer_pos_;
};

#endif //BINARY_SERIALIZER_HPP
