/* 
 * File:   buffer.hpp
 * Author: posypkin
 *
 * Created on April 16, 2011, 11:54 AM
 *
 * Basic interface to the message buffer
 */

#ifndef BUFFER_HPP
#define	BUFFER_HPP


class Buffer {
public:
    
    /**
     * Returns buffer size in bytes
     * @return buffer size
     */
    virtual int size() const = 0;
    
    
    /**
     * Returns the pointer to the first byte of the buffer
     * @return data poiner
     */
    virtual void* data() const = 0;
    
    
    /**
     * Changes the size of the buffer to the new size 
     * @param newsize the new size of the buffer
     * @returns 0 for the successful reallocation and error code otherwise
     */
    virtual int resize(int newsize) = 0;
};



#endif	/* BUFFER_HPP */

