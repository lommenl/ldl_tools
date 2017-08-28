#pragma once
#ifndef LDL_POOL_H_
#define LDL_POOL_H_

#include <vector>
#include <map>

#include <mutex>
namespace c11 {
    using namespace std;
}

namespace ldl {

    // Class defining a stack of pointers to memory blocks allocated from the heap.
    class Pool {
    public:
        // default constructor
        Pool();

        // construct and initialize object that holds num_blocks block_size byte blocks.
        Pool(size_t block_size, size_t num_blocks = 0, int growth_step = 0);

        // destructor
        ~Pool();

        // swap state with other.
        void swap(Pool& other);

        // initialize a default constructed object 
        void Initialize(size_t block_size, size_t num_blocks, int growth_step);

        /// Increase stack_ size by num_blocks blocks.
        void IncreaseSize(size_t num_blocks);

        /// Set number of blocks to automatically add to stack_ if it becomes empty.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step blocks when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        void SetGrowthStep(int growth_step);

        /// Return the current value of growth_step.
        int GetGrowthStep() const;

        /// return number of bytes in a block
        size_t GetBlockSize() const;

        // return number of unallocated blocks in pool.
        size_t GetFree() const;

        // return total number (allocated+ and unallocated) blocks in pool.
        size_t GetSize() const;

        // return true if pool has no free elements
        bool IsEmpty() const;

        // Pop a pointer to a single block off of stack_. 
        // If stack is empty and growth_step!=0, call increaseSize() to add more blocks according to the value of growth_step.
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

        //----

        // number of bytes in each block
        size_t block_size_;

        // Number of blocks to automatically add to stack_ if it becomes empty.
        int growth_step_;

        // top of stack
        size_t tos_;

        // Stack of pointers to allocated block_size_ byte memory blocks
        std::vector<void*> stack_;

    }; // class Pool

    //---------------------------------
    /// Class that manages multiple Pool objects of different sizes.
    class PoolList {

        //maximum value of block_size allowed in pool_list_
        static const size_t MAX_BLOCK_SIZE_ = static_cast<size_t>(1E9);

    public:
        // Default constructor
        PoolList();

        // Return largest possible block_size that can be created in pool_list.
        size_t GetMaxPoolBlockSize() const;

        /// Return true if pool of specified block size exists in pool_map
        bool HasPool(size_t block_size) const;

        /// Return a reference to pool_list[block_size]
        /// Will create an empty pool with default_growth_step if it doesn't already exist.
        Pool& GetPool(size_t block_size);

        /// Return a const reference to pool_list[block_size]. Throws if the pool doesn't exist.
        Pool const& GetPool(size_t block_size) const;

        /// increase the size of pool_list[block_size] by num_blocks blocks.
        void IncreasePoolSize(size_t block_size, size_t num_blocks);

        // Set the number of blocks to add to pool_list[block_size] if it becomes empty.
        // using block_size = 0 sets the growth_step value for all current and future pools.
        // Otherwise only the value of pool_list[block_size] is set.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step blocks when it is empty.
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of a pool.
        void SetPoolGrowthStep(size_t block_size, int growth_step);

        // Return the number of blocks that will be added to pool_list[block_size] if it becomes empty.
        // setting block_size=0 returns the default value that will be assigned to new pools when they are created.
        int GetPoolGrowthStep(size_t block_size) const;

        // return the current number of unallocated blocks in pool_list[block_size]
        size_t GetPoolFree(size_t block_size) const;

        // return total number of blocks (unallocated and allocated) in pool_list[block_size]
        size_t GetPoolSize(size_t block_size) const;

        // return true if there are no free blocks in pool_list[block_size]
        bool PoolIsEmpty(size_t block_size) const;

        // pop a block from pool_list[block_size]
        void* Pop(size_t block_size);

        // push a block onto pool_list[block_size]
        void Push(size_t block_size, void* ptr);

    private:
        // no copies
        PoolList(const PoolList&); //= delete;
        PoolList& operator=(const PoolList&); //= delete;

        // default value of growth_step_ for all pools
         int default_growth_step_;

        // type defining a map of multiple Pool objects keyed by their block_size.
         typedef std::map<size_t, Pool> PoolMap;

        // A map of multiple Pool objects keyed by their block_size.
         PoolMap pool_map_;

    }; // class PoolList

} //namespace ldl

#endif //! LDL_POOL_H_
