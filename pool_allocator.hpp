#include "pool_allocator.h"

#include <exception>
#include <memory> //std::allocator
#include <cstdint> //int8_t

namespace ldl {

    //--------------
    template<typename T>
    template<typename U>
    PoolAllocator<T>::PoolAllocator(const PoolAllocator<U>& other)
    {
        // nothing to do
    }

    //--------------
    template<typename T>
    template<typename U>
    PoolAllocator<T>& PoolAllocator<T>::operator=(const PoolAllocator<U>& other)
    {
        // nothing to do
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
        return MAX_N;
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::construct(T* ptr, const T& val) const
    {
        if (ptr) {
            new((void*)ptr) T(val);
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

    //--------------
    template<typename T>
    T* PoolAllocator<T>::allocate(size_t n, const void* hint = 0)
    {
        if (n == 0) {
            throw std::runtime_error("error");
        }
        return GetPool(n*sizeof(T)).allocate();
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::deallocate(T* ptr, size_t n)
    {
        if (ptr && n) {
            GetPool(n * sizeof(T)).deallocate(ptr);
        }
    }

    //--------------
    template<typename T>
    T* PoolAllocator<T>::New(size_t n = 1)
    {
        T* ptr = allocate(n);
        for ( size_t ix = 0; ix<n; ++ix ) {
            new((void*)(ptr+ix)) T();
        }
        return ptr;
    }

    //--------------
    template<typename T>
    template<typename A1>
    T* PoolAllocator<T,A1>::New(A1 a1, size_t n = 1)
    {
        T* ptr = allocate(n);
        for ( size_t ix = 0; ix<n; ++ix ) {
            new((void*)(ptr+ix)) T(a1);
        }
        return ptr;
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2>
    T* PoolAllocator<T,A1,A2>::New(A1 a1, A2 a2, size_t n = 1)
    {
        T* ptr = allocate(n);
        for ( size_t ix = 0; ix<n; ++ix ) {
            new((void*)(ptr+ix)) T(a1,a2);
        }
        return ptr;
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2, typename A3>
    T* PoolAllocator<T,A1,A2,A3>::New(A1 a1, A2 a2, A3 a3, size_t n = 1)
    {
        T* ptr = allocate(n);
        for ( size_t ix = 0; ix<n; ++ix ) {
            new((void*)(ptr+ix)) T(a1,a2,a3);
        }
        construct(ptr,FIXME);
        return ptr;
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::Delete(T* ptr, size_t n = 1)
    {
        if (ptr) {
            for (size_t ix = 0; ix < n; ++ix) {
                ptr[ix].~T();
            }
            deallocate(ptr, n)
        }
    }

    //--------------
    template<typename T>
    c11::function<void(T*)> PoolAllocator<T>::GetDeleter(size_t n = 1)
    {
        return c11::bind(&PoolAllocator::Delete, this, _1, n);
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
