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

namespace ldl {

    ///--------------
    // Class defining a stack of pointers to memory blocks of  size - n
    class Pool {
    public:
        // default constructor
        Pool();

        // construct object that holds blocks of nbytes bytes.
        Pool(size_t nbytes);

        // destructor
        ~Pool();

        // swap state with other (needed to insert a non-copyable object into a standard container)
        void swap(Pool& other);

        /// increase size of stack_ to new_capacity elements of nbytes each.
        void resize(size_t new_capacity);

        /// set number of nbyte blocks to add to stack_ if it becomes empty.
        /// setting this to 0 disables growth of this pool.
        void SetGrowthStep(size_t blocks);

        /// Return the current value of growth_step. (in blocks)
        size_t GetGrowthStep();

        // Pop a pointer to a single nbytes_ sized block off of stack_. 
        // If stack is empty and growth_step>0, call resize() to add growth_step more blocks.
        // otherwise throw an exception.
        void* allocate();

        // Push a block pointer onto stack_.
        // If stack is full and growth_step == 0, throw an exception.
        void deallocate(void* ptr);

    private:
        // no copies allowed
        Pool(const Pool&); // = delete;
        Pool& operator=(const Pool&); // = delete;

        // resize stack_ without locking mutex (assume it's already locked)
        void NoLockResize(size_t new_cap);

        //----

        // thread safety mutex.
        // declaring as a pointer allows swap/move/copy of the mutex.
        c11::shared_ptr<c11::mutex> mutex_ptr_;

        // number of bytes in each block
        const size_t nbytes_;

        // number of valid unallocated pointers in stack_;
        size_t size_;

        // number of blocks to add to stack_ if it becomes empty.
        size_t growth_step_;

        // stack of pointers to allocated nbytes_ size memory blocks
        std::vector<void*> stack_;

    }; // Pool

    //---------------------------------
    // all static base class for PoolAllocator objects
    class PoolList {
    public:
        /// Return a reference to pool_list[nbytes], which contains nbytes sized blocks
        static Pool& GetPool(size_t nbytes);

        // Set the number of blocks to add to pool_list[nbytes] if it becomes empty.
        // nbytes = 0 sets the growth_step value for all current and future pools to the specified value.
        // Setting nblocks = 0 disables pool growth.
        static void SetGrowthStep(size_t nblocks, size_t nbytes = 0);

        // Return the number of blocks that will be added to pool_list[nbytes] if it becomes empty.
        // nbytes=0 returns the default value assigned to new pools when they are created.
        static size_t GetGrowthStep(size_t nbytes = 0 );

        /// Set the size of the pool that holds nbytes sized blocks to new_capacity blocks.
        static void ResizePool(size_t nbytes, size_t new_capacity);

    private:

        //maximum value of nbytes allowed in pool_list
        static const size_t MAX_NBYTES = static_cast<size_t>(1E9);

        // struct declaring static member variables.
        // Declaring these inside a struct lets us also declare a constructor function to initialize them at program start,
        // and a destructor function to destroy them at program shutdown, which the regular class destructor can't do.
        struct Statics {
            // mutex for thread safety
            c11::mutex mutex;

            // default value of growth_step_ for all pools (in in blocks)
            static size_t default_growth_step;

            // Type defining a map of multiple Pool objects keyed by their nbytes_ value. 
            typedef std::map<size_t, Pool> PoolList;

            // A map of multiple Pool objects keyed by their nbytes_ value.
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
    class PoolAllocator : protected PoolList {
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

        //PoolAllocator() = default;

        // PoolAllocator(const PoolAllocator& other) = default;

        //~PoolAllocator() = default;

        // PoolAllocator& operator=(const PoolAllocator& other) = default;

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

        // duplicate "new T()" and "new T[n]();"
        T* New(size_t n = 1);

        // duplicate "new T(a1)" and simulate something equivalent to "new T[n](a1)"
        template<typename A1>
        T* New(A1 a1, size_t n = 1);

        // duplicate "new T(a1,a2)" and simulate something equivalent to "new T[n](a1,a2)"
        template<typename A1, typename A2>
        T* New(A1 a1, A2 a2, size_t n = 1);

        // duplicate "new T(a1,a2,a3)" and simulate something equivalent to "new T[n](a1,a2,a3)"
        template<typename A1, typename A2, typename A3>
        T* New(A1 a1, A2 a2, A3 a3, size_t n = 1);

        // duplicate delete ptr; and sort of duplicate delete[] ptr;
        void Delete(T* ptr, size_t n = 1);

        // Return a function object that will call this->Delete(ptr,n);
        // This object can be used as a Deleter argument in a shared_ptr constructor.
        c11::function<void(T*)> GetDeleter(size_t n = 1);

    };

    //-----------------------
    template<typename T, typename U>
    bool operator==(const PoolAllocator<T>& lhs, const PoolAllocator<U>& rhs);

    //-----------------------
    template<typename T, typename U>
    bool operator!=(const PoolAllocator<T>& lhs, const PoolAllocator<U>& rhs);

    //-----------------------
    // instantiation for PoolAllocator<void>
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