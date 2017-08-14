#pragma once
#ifndef POOL_ALLOCATOR_H_
#define POOL_ALLOCATOR_H_

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
    // Class defining a stack of pointers to memory blocks of  size - n
    class Pool {
    public:
        // default constructor
        Pool();

        // construct object that holds nbyte sized blocks.
        Pool(size_t nbytes, int growth_step);

        // destructor
        ~Pool();

        // swap state with other.
        // (needed to insert a non-copyable object into a standard container)
        void swap(Pool& other);

        // return the maximum capacity of the pool (NOT the number of blocks it currently contains.)
        size_t size();

        /// increase size of stack_ to hold new_capacity blocks of nbytes each.
        void resize(size_t new_capacity);

        /// Set number of nbyte blocks to add to stack_ if it becomes empty.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step blocks when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        void SetGrowthStep(int growth_step);

        /// Return the current value of growth_step.
        int GetGrowthStep();

        // Pop a pointer to a single nbytes sized block off of stack_. 
        // If stack is empty and growth_step!=0, call resize() to add more blocks according to the value of growth_step.
        // If stack is empty and growth_step==0, throw an exception.
        void* allocate();

        // Push a block pointer onto stack_.
        // If stack is full and growth_step != 0, increase the size of stack_ by 1.
        // If stack is full and growth_step==0, throw an exception.
        void deallocate(void* ptr);

    private:
        // no copies allowed
        Pool(const Pool&); // = delete;
        Pool& operator=(const Pool&); // = delete;

        // Resize stack_ without locking mutex (assume it's already locked)
        void NoLockResize(size_t new_cap);

        //----

        // thread safety mutex.
        // declaring as a pointer allows swap/move/copy of the mutex.
        c11::shared_ptr<c11::mutex> mutex_ptr_;

        // number of bytes in each block
        size_t nbytes_;

        // number of unallocated (valid) block pointers in stack_;
        size_t size_;

        // description of number of blocks to add to stack_ if it becomes empty.
        int growth_step_;

        // stack of pointers to allocated nbytes_ sized memory blocks
        std::vector<void*> stack_;

    }; // Pool

    //---------------------------------
    // all static base class for PoolAllocator objects
    class PoolList {
    protected:
        // Set the number of blocks to add to pool_list[pool_num] if it becomes empty.
        // using pool_num = 0 sets the growth_step value for all current and future pools.
        // Otherwise only the value fo pool_list[pool_num] is set.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step blocks when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of a pool.
        static void SetGrowthStep(size_t pool_num, int growth_step);

        // Return the number of blocks that will be added to pool_list[pool_num] if it becomes empty.
        // setting pool_num=0 returns the default value that will be assigned to new pools when they are created.
        static int GetGrowthStep(size_t pool_num);

        /// Set the size of pool_list[pool_num] to hold new_capacity blocks.
        static void ResizePool(size_t pool_num, size_t new_capacity);

        // return size of pool_list(n)
        static size_t GetPoolSize(size_t pool_num);

        /// Return a reference to pool_list[pool_num]
        static Pool& GetPool(size_t pool_num);

    private:

        // struct declaring static member variables.
        // Declaring these inside a struct lets us also declare a constructor function to initialize them at program start,
        // and a destructor function to destroy them at program shutdown, which the regular class destructor can't do.
        struct Statics {

            //maximum value of pool_num allowed in pool_list
            const size_t MAX_POOLS = static_cast<size_t>(1E9);

            // mutex for thread safety
            c11::mutex mutex;

            // default value of growth_step_ for all pools
             int default_growth_step;

            // Type defining a map of multiple Pool objects keyed by their pool_num value. 
            typedef std::map<size_t, Pool> PoolList;

            // A map of multiple Pool objects keyed by their pool_num value.
            PoolList pool_list;

            // default constructor (called at program start)
            Statics();

            // destructor (called at program exit)
            ~Statics();
        };
        static Statics s;
    };


    //-----------------
    /// template class for an allocator object that will minimize allocations of heap memory by recycling the allocated memory of deleted
    /// objects for other objects.
    /// Can be used in place of std::allocator as an allocator for STL container classes and other classes like shared_ptr,
    /// which allocate memory internally.
    template<typename T>
    class PoolAllocator : public PoolList {
    public:
        typedef T value_type;

        typedef T* pointer;

        typedef T& reference;

        typedef const T* const_pointer;

        typedef const T& const_reference;

        typedef size_t size_type;

        typedef ptrdiff_t difference_type;

        template <typename U> struct rebind { typedef PoolAllocator<U> other; };

        //-----

        // Default Constructor
        PoolAllocator() {} // = default;

        // Copy Constructor
        PoolAllocator(const PoolAllocator&) {} // = default;

        // Destructor
        ~PoolAllocator() {} // = default;

        // Copy asignment operator
        PoolAllocator& operator=(const PoolAllocator&) {} // = default;

        // Copy constructor from PoolAllocator<U>
        template<typename U>
        PoolAllocator(const PoolAllocator<U>& other);

        // Copy assignment from PoolAllocator<U>
        template<typename U>
        PoolAllocator& operator=(const PoolAllocator<U>& other);

        //---

        // return pointer to object x;
        T* address(T& x) const;

        // return const pointer to object x;
        const T* address(const T& x) const;

        // return maximum number of elements of type T that can be allocated in a contiguous block.
        size_t max_size() const;

        /// construct an object of type T in memory pointed to by ptr.
        void construct(T* ptr, const T& val) const;

        /// Call destructor ~T() on memory pointed to by ptr.
        void destroy(T* ptr) const;

        /// Return a pointer to block of memory of size sizeof(T[n])
        /// If growth_step for the matching pool is nonzero, the function will allocate memory from the heap if necessary.
        /// (which will will be returned to the pool when deallocated.)
        /// if growth_step==0, it will throw an exception.
        T* allocate(size_t n, const void* hint = 0);

        /// return a block of memory assumed to be of size sizeof(T[n]) to the appropriate pool
        void deallocate(T* ptr, size_t n);

        //----

        static void SetGrowthStep(int growth_step, size_t n = 0 );

        static int GetGrowthStep(size_t n = 0);

        static void ResizePool(size_t new_capacity, size_t n = 1);
        
        static size_t GetPoolSize(size_t n = 1);

        // static version of allocate()
        static T* Allocate(size_t n);

        // static version of deallocate()
        static void Deallocate(T* ptr, size_t n);

        // duplicate "new T()"
        static T* New();

        // duplicate "new T[n]();"
        static T* NewArray(size_t n);

        // duplicate "new T(a1)"
        template<typename A1>
        static T* New(A1 a1);

        // duplicate "new T[n](a1)" (which doesn't really exist)
        template<typename A1>
        static T* NewArray(size_t n, A1 a1);

        // duplicate "new T(a1,a2)" 
        template<typename A1, typename A2>
        static T* New(A1 a1, A2 a2);

        // duplicate "new T[n](a1,a2)"  (which doesn't really exist)
        template<typename A1, typename A2>
        static T* NewArray(size_t n, A1 a1, A2 a2);

        // duplicate "new T(a1,a2,a3)"
        template<typename A1, typename A2, typename A3>
        static T* New(A1 a1, A2 a2, A3 a3);

        // duplicate "new T[n](a1,a2,a3)" (which doesn't really exist)
        template<typename A1, typename A2, typename A3>
        static T* NewArray(size_t n, A1 a1, A2 a2, A3 a3);

        // duplicate "delete ptr"
        // This static function can be used as a Deleter argument in a shared_ptr constructor.
        // specify as 'ldl::PoolAllocator<T>::Delete';
        static void Delete(T* ptr);

        // return a pointer to Delete(T* ptr);
        // The returned object can be used as a Deleter argument in a shared_ptr constructor.
        static c11::function<void(T*)> GetDeleter();

        // sort of duplicate "delete[] ptr"
        // specifying a value of n different than the value used in
        // NewArray() will cause things to catch on fire.
        static void DeleteArray(size_t n, T* ptr);

        // Return a function object that will call this->Delete(n,ptr);
        // the returned object can be used as a Deleter argument in a shared_ptr constructor.
        static c11::function<void(T*)> GetArrayDeleter(size_t n = 1);

    };

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