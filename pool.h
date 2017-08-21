#pragma once
#ifndef LDL_POOL_H_
#define LDL_POOL_H_

#include <vector>
#include <map>

#include <mutex>    // std::mutex
namespace c11 {
    using namespace std;
}

namespace ldl {

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

        /// increase stack_ size by num_blocks blocks.
        void IncreaseSizeBy(std::size_t num_blocks);

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

        /// Set the size of pool_list[block_size] to hold num_blocks blocks.
        void IncreasePoolSize(std::size_t block_size, std::size_t num_blocks);

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
    /// Class declaring a static PoolList
    class StaticPoolList {
    protected:
        static PoolList pool_list_;
    };

} //namespace ldl

#endif //! LDL_POOL_H_