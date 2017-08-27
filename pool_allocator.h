#pragma once
#ifndef LDL_POOL_ALLOCATOR_H_
#define LDL_POOL_ALLOCATOR_H_

#include "pool.h" // StaticPoolList

namespace c11 {
    using namespace std;
}

namespace ldl {

    /// template class for an allocator that will minimize allocations of heap memory
    /// by recycling the allocated memory of deleted objects for reuse.
    /// Can be used in place of std::allocator as an allocator for STL container classes and other
    /// classes (like shared_ptr), which allocate memory internally.
    template<typename T>
    class PoolAllocator : public StaticPoolList {
    public:
        typedef T value_type;

        typedef T* pointer;

        typedef T& reference;

        typedef const T* const_pointer;

        typedef const T& const_reference;

        typedef size_t size_type;

        typedef ptrdiff_t difference_type;

        template <typename U> struct rebind { typedef PoolAllocator<U> other; };

        //-----------------
        // std::allocator functions
        //-----------------

        // Default Constructor
        PoolAllocator() = default;

        // Copy Constructor
        PoolAllocator(const PoolAllocator&) = default;

        // Copy asignment operator
        PoolAllocator& operator=(const PoolAllocator&) = default;

        // Destructor
        ~PoolAllocator() = default;

        // Copy constructor, from PoolAllocator<U>
        template<typename U>
        PoolAllocator(const PoolAllocator<U>& other);

        // Copy assignment, from PoolAllocator<U>
        template<typename U>
        PoolAllocator& operator=(const PoolAllocator<U>& other);

        //---

        // return a pointer to object x;
        T* address(T& x) const;

        // return a const pointer to object x;
        const T* address(const T& x) const;

        // Return the maximum number of elements of type T that can be allocated in a contiguous block.
        size_t max_size() const;

        /// Construct an object of type T in memory pointed to by ptr.
        void construct(T* ptr, const T& val) const;

        /// Call destructor ~T() on memory pointed to by ptr.
        void destroy(T* ptr) const;

        /// Return a pointer to block of memory of size sizeof(T[numel])
        /// If growth_step for the matching pool is nonzero, the function will allocate memory from the heap if necessary.
        /// (which will will be returned to the pool when deallocated.)
        /// if growth_step==0, it will throw an exception.
        T* allocate(size_t numel, const void* hint = 0);

        /// return a block of memory (assumed to be of size sizeof(T[numel])) to the appropriate pool.
        void deallocate(T* ptr, size_t numel);

    }; //class PoolAllocator<T>

    //-----------------------
    template<typename T, typename U>
    bool operator==(const PoolAllocator<T>& lhs, const PoolAllocator<U>& rhs);

    //-----------------------
    template<typename T, typename U>
    bool operator!=(const PoolAllocator<T>& lhs, const PoolAllocator<U>& rhs);

    //-----------------------
    // instantiation of class PoolAllocator<void>
    template <> class PoolAllocator<void> {
    public:
        typedef void* pointer;
        typedef const void* const_pointer;
        typedef void value_type;
        template <typename U> struct rebind { typedef PoolAllocator<U> other; };
    };

} //namespace ldl

#include "pool_allocator.hpp"

#endif //! LDL_POOL_ALLOCATOR_H_