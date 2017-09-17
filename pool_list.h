#pragma once
#ifndef LDL_POOL_LIST_H_
#define LDL_POOL_LIST_H_

#include "pool.h"

#include <map>

namespace ldl {

    /// Class that manages multiple Pool objects of different sizes.
    class PoolList {

        //maximum value of block_size allowed in pool_list_
        static const size_t MAX_BLOCK_SIZE_ = static_cast<size_t>(1E9);

    public:

        // Default constructor
        PoolList();

        void swap(PoolList& other);

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

#endif //! LDL_POOL_LIST_H_
