#pragma once
#ifndef LDL_STATIC_POOL_LIST_H_
#define LDL_STATIC_POOL_LIST_H_

#include "pool_list.h" // PoolList

#include <mutex>
namespace c11 {
    using namespace std;
}

namespace ldl {

    class StaticPoolList {
    public:

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

        static c11::mutex mutex_;

        static PoolList pool_list_;
    };

} //namespace ldl


#endif //!LDL_STATIC_POOL_LIST_H_
