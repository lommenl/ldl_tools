#include "pool_allocator.h"

#include <exception>
#include <functional> // std:: bind
namespace c11 {
    using namespace std;
}
using namespace std::placeholders; // to match using in <functional>

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
        return pool_list_.MAX_BLOCK_SIZE / sizeof(T);
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
        return Allocate(numel); // call static member function
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::deallocate(T* ptr, size_t numel)
    {
        Deallocate(ptr, numel); // call static member function
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::SetPoolSize(size_t numel, size_t num_blocks)
    {
        pool_list_.SetPoolSize(numel * sizeof(T), num_blocks);
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::IncreasePoolSize(size_t numel, size_t num_blocks)
    {
        pool_list_.IncreasePoolSize(numel * sizeof(T), num_blocks);
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::SetGrowthStep(size_t numel, int growth_step)
    {
        pool_list_.SetGrowthStep(numel * sizeof(T), growth_step);
    }

    //--------------
    template<typename T>
    int PoolAllocator<T>::GetGrowthStep(size_t numel)
    {
        return pool_list_.GetGrowthStep(numel * sizeof(T));
    }

    //--------------
    template<typename T>
    size_t PoolAllocator<T>::GetPoolFree(size_t numel)
    {
        return pool_list_.GetPoolFree(numel * sizeof(T));
    }
    //--------------
    template<typename T>
    size_t PoolAllocator<T>::GetPoolSize(size_t numel)
    {
        return pool_list_.GetPoolSize(numel * sizeof(T));
    }

    //--------------
    template<typename T>
    T* PoolAllocator<T>::Allocate(size_t numel)
    {
        if (numel == 0) { throw std::bad_alloc(); }
        return static_cast<T*>(pool_list_.Pop(numel * sizeof(T)));
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::Deallocate(T* ptr, size_t numel)
    {
        if (ptr && numel) {
            pool_list_.Push(numel * sizeof(T), static_cast<void*>(ptr));
        }
    }

    //--------------
    template<typename T>
    SharedPointer<T> PoolAllocator<T>::NewArray(size_t numel)
    {
        T* ptr = Allocate(numel);
        for (size_t ix = 0; ix < numel; ++ix) {
            new(static_cast<void*>(ptr + ix)) T();
        }
        return SharedPointer<T>(ptr, GetArrayDeleter(numel));
    }

    //--------------
    template<typename T>
    SharedPointer<T> PoolAllocator<T>::New()
    {
        return NewArray(1);
    }

    //--------------
    template<typename T>
    template<typename A1>
    SharedPointer<T> PoolAllocator<T>::NewArray(size_t numel, A1 a1)
    {
        T* ptr = Allocate(numel);
        for (size_t ix = 0; ix < numel; ++ix) {
            new(static_cast<void*>(ptr + ix)) T(a1);
        }
        return SharedPointer<T>(ptr, GetArrayDeleter(numel));
    }

    //--------------
    template<typename T>
    template<typename A1>
    SharedPointer<T> PoolAllocator<T>::New(A1 a1)
    {
        return NewArray(1, a1);
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2>
    SharedPointer<T> PoolAllocator<T>::NewArray(size_t numel, A1 a1, A2 a2)
    {
        T* ptr = Allocate(numel);
        for (size_t ix = 0; ix < numel; ++ix) {
            new(static_cast<void*>(ptr + ix)) T(a1, a2);
        }
        return SharedPointer<T>(ptr, GetArrayDeleter(numel));
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2>
    SharedPointer<T> PoolAllocator<T>::New(A1 a1, A2 a2)
    {
        return NewArray(1, a1, a2);
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2, typename A3>
    SharedPointer<T> PoolAllocator<T>::NewArray(size_t numel, A1 a1, A2 a2, A3 a3)
    {
        T* ptr = Allocate(numel);
        for (size_t ix = 0; ix < numel; ++ix) {
            new(static_cast<void*>(ptr + ix)) T(a1, a2, a3);
        }
        return SharedPointer<T>(ptr, GetArrayDeleter(numel));
    }

    //--------------
    template<typename T>
    template<typename A1, typename A2, typename A3>
    SharedPointer<T> PoolAllocator<T>::New(A1 a1, A2 a2, A3 a3)
    {
        return NewArray(1, a1, a2, a3);
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::DeleteArray(size_t numel, T* ptr)
    {
        if (ptr) {
            for (size_t ix = 0; ix < numel; ++ix) {
                ptr[ix].~T();
            }
            Deallocate(ptr, numel);
        }
    }

    //--------------
    template<typename T>
    void PoolAllocator<T>::Delete(T* ptr)
    {
        DeleteArray(1, ptr);
    }

    //--------------
    template<typename T>
    c11::function<void(T*)> PoolAllocator<T>::GetArrayDeleter(size_t numel)
    {
        return c11::bind(DeleteArray, numel, _1);
    }

    //--------------
    template<typename T>
    c11::function<void(T*)>  PoolAllocator<T>::GetDeleter()
    {
        return Delete;
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
