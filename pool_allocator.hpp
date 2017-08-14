#include "pool_allocator.h"

#include <exception>
#include <memory> //std::allocator
#include<functional> //std::bind()
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
        return s.MAX_NBYTES / sizeof(T);
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

    template<typename T>
    T* PoolAllocator<T>::allocate(size_t n, const void* hint = 0)
    {
        return Allocate(n);
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::deallocate(T* ptr, size_t n)
    {
        Deallocate(ptr, n);
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::SetGrowthStep(int growth_step, size_t n)
    {
        PoolList::SetGrowthStep(n * sizeof(T), growth_step);
    }

    //--------------
    template<typename T>
    int PoolAllocator<T>::GetGrowthStep(size_t n)
    {
        return PoolList::GetGrowthStep(n * sizeof(T));
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::ResizePool(size_t new_capacity, size_t n)
    {
        PoolList::ResizePool(n * sizeof(T), new_capacity);
    }

    //--------------
    template<typename T>
    size_t PoolAllocator<T>::GetPoolSize(size_t n)
    {
        return PoolList::GetPoolSize(n * sizeof(T));
    }

    //--------------
    template<typename T>
    T* PoolAllocator<T>::Allocate(size_t n)
    {
        if (n == 0) {
            throw std::runtime_error("error");
        }
        return static_cast<T*>(GetPool(n * sizeof(T)).allocate());
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::Deallocate(T* ptr, size_t n)
    {
        if (ptr && n) {
            size_t foo = sizeof(T);
            GetPool(n * sizeof(T)).deallocate(ptr);
        }
    }

    //--------------
    template<typename T>
    T* PoolAllocator<T>::New()
    {
        return NewArray(1);
    }

    //--------------
    template<typename T>
    T* PoolAllocator<T>::NewArray(size_t n)
    {
        T* ptr = Allocate(n);
        for (size_t ix = 0; ix < n; ++ix) {
            new((void*)(ptr + ix)) T();
        }
        return ptr;
    }

    //--------------
    template<typename T>
    template<typename A1>
    T* PoolAllocator<T>::New(A1 a1)
    {
        return NewArray(1, a1);
    }

    //--------------
    template<typename T>
    template<typename A1>
    T* PoolAllocator<T>::NewArray(size_t n, A1 a1)
    {
        T* ptr = Allocate(n);
        for (size_t ix = 0; ix < n; ++ix) {
            new((void*)(ptr + ix)) T(a1);
        }
        return ptr;
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2>
    T* PoolAllocator<T>::New(A1 a1, A2 a2)
    {
        return NewArray(1, a1, a2);
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2>
    T* PoolAllocator<T>::NewArray(size_t n, A1 a1, A2 a2)
    {
        T* ptr = Allocate(n);
        for (size_t ix = 0; ix < n; ++ix) {
            new((void*)(ptr + ix)) T(a1, a2);
        }
        return ptr;
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2, typename A3>
    T* PoolAllocator<T>::New(A1 a1, A2 a2, A3 a3)
    {
        return NewArray(1, a1, a2, a3);
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2, typename A3>
    T* PoolAllocator<T>::NewArray(size_t n, A1 a1, A2 a2, A3 a3)
    {
        T* ptr = Allocate(n);
        for (size_t ix = 0; ix < n; ++ix) {
            new((void*)(ptr + ix)) T(a1, a2, a3);
        }
        return ptr;
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::Delete(T* ptr)
    {
        DeleteArray(1, ptr);
    }

    //--------------
    template<typename T>
    c11::function<void(T*)>  PoolAllocator<T>::GetDeleter()
    {
        return Delete;
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::DeleteArray(size_t n, T* ptr)
    {
        if (ptr) {
            for (size_t ix = 0; ix < n; ++ix) {
                ptr[ix].~T();
            }
            Deallocate(ptr, n);
        }
    }

    //--------------
    template<typename T>
    c11::function<void(T*)> PoolAllocator<T>::GetArrayDeleter(size_t n = 1)
    {
        return c11::bind(DeleteArray, n, _1);
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
