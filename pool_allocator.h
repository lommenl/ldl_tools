#pragma once
#ifndef LDL_POOL_ALLOCATOR_H_
#define LDL_POOL_ALLOCATOR_H_

#include "pool.h" // StaticPoolList
#include "shared_pointer.h"

#include <functional> // std::function
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

        typedef std::size_t size_type;

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
        std::size_t max_size() const;

        /// Construct an object of type T in memory pointed to by ptr.
        void construct(T* ptr, const T& val) const;

        /// Call destructor ~T() on memory pointed to by ptr.
        void destroy(T* ptr) const;

        /// Return a pointer to block of memory of size sizeof(T[numel])
        /// If growth_step for the matching pool is nonzero, the function will allocate memory from the heap if necessary.
        /// (which will will be returned to the pool when deallocated.)
        /// if growth_step==0, it will throw an exception.
        T* allocate(std::size_t numel, const void* hint = 0);

        /// return a block of memory (assumed to be of size sizeof(T[numel])) to the appropriate pool.
        void deallocate(T* ptr, std::size_t numel);

        //-----------------
        // PoolList static functions
        //-----------------

        // Increase the size of pool[numel] by num_blocks.
        static void IncreasePoolSize(std::size_t numel, std::size_t num_blocks);
        
        // set the growth_step for the pool that holds buffers of size sizeof(T[numel])
        static void SetPoolGrowthStep(std::size_t numel, int growth_step);

        // get the growth_step for the pool that holds buffers of size sizeof(T[numel])
        static int GetPoolGrowthStep(std::size_t numel);

        // Get the number of unallocated blocks in the pool that holds buffers of size sizeof(T[numel])
        static std::size_t GetPoolFree(std::size_t numel);

        // Get the total number of blocks (allocated and unallocat3ed) in the pool that holds buffers of size sizeof(T[numel])
        static std::size_t GetPoolSize(std::size_t numel);

        // static version of allocate() for use by NewArray()
        static T* Allocate(std::size_t numel);

        // static version of deallocate() for use by DeleteArray()
        static void Deallocate(T* ptr, std::size_t numel);

        //-----------------
        // SharedPtr construction functions
        //-----------------

        /// return a SharedPointer that points to an array of T[numel] allocated from pooled memory
        /// and construted by T()
        static typename SharedPointer<T> NewArray(std::size_t numel);

        /// return a SharedPointer that points to an object of type T allocated from pooled memory.
        /// and construted by T()
        static typename SharedPointer<T> New();

        /// return a SharedPointer that points to an array of T[numel] allocated from pooled memory
        /// and construted by T(a1)
        template<typename A1>
        static typename SharedPointer<T> NewArray(std::size_t numel, A1 a1);

        /// return a SharedPointer that points to an object of type T allocated from pooled memory
        /// and construted by T(a1)
        template<typename A1>
        static typename SharedPointer<T> New(A1 a1);

        /// return a SharedPointer that points to an array of T[numel] allocated from pooled memory
        /// and construted by T(a1,a2)
        template<typename A1, typename A2>
        static typename SharedPointer<T> NewArray(std::size_t numel, A1 a1, A2 a2);

        /// return a SharedPointer that points to an object of type T allocated from pooled memory
        /// and construted by T(a1,a2)
        template<typename A1, typename A2>
        static typename SharedPointer<T> New(A1 a1, A2 a2);

        /// return a SharedPointer that points to an array of T[numel] allocated from pooled memory
        /// and construted by T(a1,a2,a3)
        template<typename A1, typename A2, typename A3>
        static SharedPointer<T> NewArray(std::size_t numel, A1 a1, A2 a2, A3 a3);

        /// return a SharedPointer that points to an object of type T allocated from pooled memory
        /// and construted by T(a1,a2,a3)
        template<typename A1, typename A2, typename A3>
        static SharedPointer<T> New(A1 a1, A2 a2, A3 a3);

    private:

        // Destroy and deallocate a block allocated from pool that contains an array of T[numel]
        // should not be necessary, since all normally allocated objects are wrapped in SharedPointers
        // Note: specifying a value of numel different than the value used in NewArray() will cause things to catch on fire.
        static void DeleteArray(std::size_t numel, T* ptr);

        // Destroy and deallocate a block allocated from pool that contains a single object of type T
        // This static function can be used as a Deleter argument in a shared_ptr constructor.
        // specify as 'PoolAllocator<T>::Delete';
        static void Delete(T* ptr);

        // Return a function object that will call this->Delete(numel,ptr);
        // the returned object can be used as a Deleter argument in a shared_ptr constructor.
        static c11::function<void(T*)> GetArrayDeleter(std::size_t numel);

        // return a pointer to Delete(T* ptr);
        // The returned object can be used as a Deleter argument in a shared_ptr constructor.
        static c11::function<void(T*)> GetDeleter();

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