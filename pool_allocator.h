#pragma once
#ifndef POOL_ALLOCATOR_H_
#define POOL_ALLOCATOR_H_

#include "shared_pointer.h"

#include <vector>
#include <map>

#include <functional> // std::function
#include <memory>       // std::shared_ptr
#include <mutex>    // std::mutex
namespace c11 {
    using namespace std;
}

using namespace std::placeholders; // to match "using namespace std::placeholders;" in <functional>

namespace ldl {

    ///--------------
    // Class defining a stack of pointers to memory blocks.
    class Pool {
    public:
        // default constructor
        Pool();

        // construct and initialize object that holds num_blocks block_size byte blocks.
        Pool(std::size_t block_size, std::size_t num_blocks = 0, int growth_step = 0);

        // destructor
        ~Pool();

        // swap state with other.
        void swap(Pool& other);

        // initialize a default constructed object 
        void Initialize(std::size_t block_size, std::size_t num_blocks, int growth_step);

        /// resize stack_ to hold num_blocks blocks.
        void Resize(std::size_t num_blocks);

        /// Set number of blocks to automatically add to stack_ if it becomes empty.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step blocks when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        void SetGrowthStep(int growth_step);

        /// Return the current value of growth_step.
        int GetGrowthStep();

        /// return number of bytes in a block
        std::size_t GetBlockSize() const;

        // return number of unallocated blocks in pool.
        std::size_t GetFree();

        // return total number (allocated+ and unallocated) blocks in pool.
        std::size_t GetCapacity();

        // Pop a pointer to a single block off of stack_. 
        // If stack is empty and growth_step!=0, call resize() to add more blocks according to the value of growth_step.
        // If stack is empty and growth_step==0, throw an exception.
        void* Pop();

        // Push a block pointer onto stack_.
        // If stack is full and growth_step != 0, increase the size of stack_ by 1.
        // If stack is full and growth_step==0, throw an exception.
        void Push(void* ptr);

    private:
        // no copies allowed
        Pool(const Pool&) = delete;
        Pool& operator=(const Pool&) = delete;

        // Resize stack_ without locking mutex (assume it's already locked)
        void NoLockResize(std::size_t new_cap);

        //----

        // thread safety mutex.
        // declaring as a pointer allows swap/move/copy of the mutex.
        c11::shared_ptr<c11::mutex> mutex_ptr_;

        // number of bytes in each block
        std::size_t block_size_;

        // top of stack
        std::size_t tos_;

        // Description of number of blocks to add to stack_ if it becomes empty.
        int growth_step_;

        // stack of pointers to allocated block_size_ byte memory blocks
        std::vector<void*> stack_;

    }; // class Pool

    //---------------------------------
    // class that manages multiple pools of different sizes.
    class PoolList {
    public:
        // Default constructor
        PoolList() = default;

        // copy constructor
        PoolList(const PoolList&) = default;

        // copy assignment
        PoolList& operator=(const PoolList&) = default;

        // destructor
        ~PoolList() = default;

        /// Return a reference to pool_list[block_size]
        /// Will create an empty pool with default_growth_step if it doesn't already exist.
        Pool& GetPool(std::size_t block_size);

        /// Set the size of pool_list[block_size] to hold num_blocks blocks.
        void ResizePool(std::size_t block_size, std::size_t num_blocks);

        // Set the number of blocks to add to pool_list[block_size] if it becomes empty.
        // using block_size = 0 sets the growth_step value for all current and future pools.
        // Otherwise only the value of pool_list[block_size] is set.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step blocks when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of a pool.
        void SetGrowthStep(std::size_t block_size, int growth_step);

        // Return the number of blocks that will be added to pool_list[block_size] if it becomes empty.
        // setting block_size=0 returns the default value that will be assigned to new pools when they are created.
        int GetGrowthStep(std::size_t block_size);

        // return current number of unallocated blocks in pool_list[block_size]
        std::size_t GetPoolFree(std::size_t block_size);

        // return total number of blocks (unallocated and allocated) in pool_list[block_size]
        std::size_t GetPoolCapacity(std::size_t block_size);

        // pop a block from pool_list[block_size]
        void* Pop(std::size_t block_size);

        // push a block onto pool_list[block_size]
        void Push(std::size_t block_size, void* ptr);

        //maximum value of block_size allowed in pool_list
        const std::size_t MAX_BLOCK_SIZE = static_cast<std::size_t>(1E9);

    private:

        // mutex for thread safe access to list
        c11::mutex mutex_;

        // default value of growth_step_ for all pools
         int default_growth_step_;

        // A map of multiple Pool objects keyed by their block_size value.
         typedef std::map<std::size_t, Pool> PoolMap;
         PoolMap pool_map_;

    }; // class PoolList


    //-----------------
    // base class that will allocate objects of a class from a static memory pool.
    // does not allow arrays of objects to be allocated from a pool.
    template<typename T>
    class PooledNew {
    public:

        // Resize pool to hold num_blocks buffers
        static void ResizePool(std::size_t num_blocks);

        /// Set number of elements to add to Pool if it becomes empty.

        // setting growth_step > 0 automatically increases the pool size by  +growth_step elements when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        static void SetPoolGrowthStep(int growth_step);

        /// Return the current value of growth_step.
        static int GetPoolGrowthStep();

        static std::size_t GetPoolBlockSize();

        // return number of unallocated blocks in pool.
        static std::size_t GetPoolFree();

        // return total number (allocated & unallocated) of blocks in pool.
        static std::size_t GetPoolCapacity();

        //get a pointer to a block containing of the specified size
        // this function is called to allocate memory for an object.
        static void* operator new(std::size_t);

        // return a block of the specified size back to its pool.
        // his function is called to deallocate memory for an object.
        static void operator delete(void* ptr);

    private:

        static Pool pool_;

    }; //class PooledNew



    //-----------------
    // Class declaring a static PoolList shared by all PoolAllocator<T> classes.
    class PoolAllocatorBase {
    protected:
        static PoolList pool_list_;
    };

    //-----------------
    /// template class for an allocator that will minimize allocations of heap memory
    /// by recycling the allocated memory of deleted objects for reuse.
    /// Can be used in place of std::allocator as an allocator for STL container classes and other
    /// classes (like shared_ptr), which allocate memory internally.
    template<typename T>
    class PoolAllocator : public PoolAllocatorBase {
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
        // PoolList functions
        //-----------------

        // Resize a pool to hold num_blocks buffers of size sizeof(T[numel])
        static void ResizePool(std::size_t numel, std::size_t num_blocks);
        
        // set the growth_step for the pool that holds buffers of size sizeof(T[numel])
        static void SetGrowthStep(std::size_t numel, int growth_step);

        // get the growth_step for the pool that holds buffers of size sizeof(T[numel])
        static int GetGrowthStep(std::size_t numel);

        // Get the number of unallocated blocks in the pool that holds buffers of size sizeof(T[numel])
        static std::size_t GetPoolFree(std::size_t numel);

        // Get the total number of blocks (allocated and unallocat3ed) in the pool that holds buffers of size sizeof(T[numel])
        static std::size_t GetPoolCapacity(std::size_t numel);

        // static version of allocate() for use by NewArray()
        static T* Allocate(std::size_t numel);

        // static version of deallocate() for use by DeleteArray()
        static void Deallocate(T* ptr, std::size_t numel);

        //-----------------
        // SharedPtr construction functions
        //-----------------

        /// return a SharedPointer that points to an array of T[numel] allocated from pooled memory
        /// and construted by T()
        static SharedPointer<T> NewArray(std::size_t numel);

        /// return a SharedPointer that points to an object of type T allocated from pooled memory.
        /// and construted by T()
        static SharedPointer<T> New();

        /// return a SharedPointer that points to an array of T[numel] allocated from pooled memory
        /// and construted by T(a1)
        template<typename A1>
        static SharedPointer<T> NewArray(std::size_t numel, A1 a1);

        /// return a SharedPointer that points to an object of type T allocated from pooled memory
        /// and construted by T(a1)
        template<typename A1>
        static SharedPointer<T> New(A1 a1);

        /// return a SharedPointer that points to an array of T[numel] allocated from pooled memory
        /// and construted by T(a1,a2)
        template<typename A1, typename A2>
        static SharedPointer<T> NewArray(std::size_t numel, A1 a1, A2 a2);

        /// return a SharedPointer that points to an object of type T allocated from pooled memory
        /// and construted by T(a1,a2)
        template<typename A1, typename A2>
        static SharedPointer<T> New(A1 a1, A2 a2);

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

#endif //! POOL_ALLOCATOR_H_