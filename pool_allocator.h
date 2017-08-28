#pragma once
#ifndef LDL_POOL_ALLOCATOR_H_
#define LDL_POOL_ALLOCATOR_H_

#include "Pool.h" // PoolList

#include <mutex>
namespace c11 {
    using namespace std;
}

namespace ldl {

    /// template class for an allocator that will minimize allocations of heap memory
    /// by recycling the allocated memory of deleted objects for reuse.
    /// Can be used in place of std::allocator as an allocator for STL container classes and other
    /// classes (like shared_ptr), which allocate memory internally.
    template<typename T>
    class PoolAllocator {
    public:
        typedef T value_type;

        typedef T* pointer;

        typedef T& reference;

        typedef const T* const_pointer;

        typedef const T& const_reference;

        typedef size_t size_type;

        typedef ptrdiff_t difference_type;

        template <typename U> struct rebind { typedef PoolAllocator<U> other; };

        //---

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
    /// Instantiation of class PoolAllocator<void>
    /// Also provides a global static PoolList object that can be shared by other classes.
    template <> class PoolAllocator<void> {
    public:
        typedef void* pointer;
        typedef const void* const_pointer;
        typedef void value_type;
        template <typename U> struct rebind { typedef PoolAllocator<U> other; };

        //----------

        /// Return a reference to pool_list[block_size]
        /// Will create an empty pool with default_growth_step if it doesn't already exist.
        static Pool& GetPool(size_t block_size);

        /// increase the size of pool_list[block_size] by num_blocks blocks.
        static void IncreasePoolSize(size_t block_size, size_t num_blocks);

        // Set the number of blocks to automatically add to pool_list[block_size] if it becomes empty.
        // Using block_size = 0 sets the growth_step value for all current and future pools.
        // Otherwise only the value of pool_list[block_size] is set.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step blocks when it is empty.
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of a pool.
        static void SetPoolGrowthStep(size_t block_size, int growth_step);

        // Return the number of blocks that will be automatically added to pool_list[block_size] if it becomes empty.
        // setting block_size=0 returns the default value that will be assigned to new pools when they are created.
        static int GetPoolGrowthStep(size_t block_size);

        // return current number of unallocated blocks in pool_list[block_size]
        static size_t GetPoolFree(size_t block_size);

        // return total number of blocks (unallocated and allocated) in pool_list[block_size]
        static size_t GetPoolSize(size_t block_size);

        // return true if pool_list[block_size] has no unallocated blocks.
        static bool PoolIsEmpty(size_t block_size);

        // Return the maximum possible value of block_size.
        static size_t GetMaxPoolBlockSize();

        // pop a block from pool_list[block_size]
        static void* Pop(size_t block_size);

        // push a block onto pool_list[block_size]
        static void Push(size_t block_size, void* ptr);

    private:
        /// return true if pool_list[block_size] size exists.
        static bool HasPool(size_t block_size);

        //---

        static c11::mutex mutex_;

        static PoolList pool_list_;
    };

} //namespace ldl

#include "pool_allocator.hpp"

#endif //! LDL_POOL_ALLOCATOR_H_