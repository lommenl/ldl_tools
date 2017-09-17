#pragma once
#ifndef LDL_POOLED_NEW_H_
#define LDL_POOLED_NEW_H_


namespace ldl {

    //-----------------------
    // Base class that will allocate buffers for an object from a Pool of memory.
    template<typename T>
    class PooledNew {
        static const size_t element_size_;
    public:
#include "pooled_new.inc"
    }; //class PooledNew

    template<typename T>
    const size_t PooledNew<T>::element_size_ = sizeof(T);

} //namespace ldl

#endif //! LDL_POOLED_NEW_H_