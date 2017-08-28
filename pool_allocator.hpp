#include "pool_allocator.h"

#include <exception>

namespace ldl {

    //--------------
    template<typename T>
    template<typename U>
    PoolAllocator<T>::PoolAllocator(const PoolAllocator<U>& other)
    {
        // nothing to do (all member variables are static)
    }

    //--------------
    template<typename T>
    template<typename U>
    PoolAllocator<T>& PoolAllocator<T>::operator=(const PoolAllocator<U>& other)
    {
        // nothing to do (all member variables are static)
    }

    //--------------
    template<typename T>
    T* PoolAllocator<T>::address(T& x) const
    {
        return &x;
    }

    //--------------
    template<typename T>
    const T* PoolAllocator<T>::address(const T& x) const
    {
        return &x;
    }

    //--------------
    template<typename T>
    size_t PoolAllocator<T>::max_size() const
    {
        return PoolAllocator<void>::GetMaxPoolBlockSize() / sizeof(T);
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::construct(T* ptr, const T& val) const
    {
        if (ptr) {
            new(static_cast<void*>(ptr)) T(val);
        }
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::destroy(T* ptr) const
    {
        if (ptr) {
            ptr->~T();
        }
    }

    template<typename T>
    T* PoolAllocator<T>::allocate(size_t numel, const void* hint)
    {
        if (numel == 0) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(PoolAllocator<void>::Pop(numel * sizeof(T)));
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::deallocate(T* ptr, size_t numel)
    {
        if (ptr && numel) {
            PoolAllocator<void>::Push(numel * sizeof(T), static_cast<void*>(ptr));
        }
    }

    //-----------------------
    template<typename T, typename U>
    bool operator==(const PoolAllocator<T>& lhs, const PoolAllocator<U>& rhs)
    {
        return (typeid(T) == typeid(U));
    }

    //-----------------------
    template<typename T, typename U>
    bool operator!=(const PoolAllocator<T>& lhs, const PoolAllocator<U>& rhs)
    {
        return !(lhs == rhs);
    }

} //namespace ldl
