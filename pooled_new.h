#pragma once
#ifndef LDL_POOLED_NEW_H_
#define LDL_POOLED_NEW_H_

#include "Pool.h" // Pool

namespace ldl {

    // Base class that will allocate buffers for an object from a Pool of memory.
    // classes that inherit from this class will share a common set of memory pools for allocating objects.
    template<typename T>
    class PooledNew {
    public:

        // increase size of pool holding single objects by num_blocks.
        static void IncreasePoolSize(std::size_t num_blocks);

        /// Set number of elements to add to single object Pool if it becomes empty.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step elements when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        static void SetPoolGrowthStep(int growth_step);

        /// Return the current value of growth_step for the single object pool.
        static int GetPoolGrowthStep();

        // return number of unallocated blocks in the single object pool
        static std::size_t GetPoolFree();

        // return total number (allocated & unallocated) of blocks in the single pool object
        static std::size_t GetPoolSize();


        //get a pointer to a block containing n bytes
        // This function is called by new to allocate memory for a single object.
        static void* operator new(std::size_t n);

        // Return a block containing a single element of type T to the pool
        // his function is called by delete to deallocate memory for a single object.
        static void operator delete(void* ptr);

        //----------------------

        // increase size of pool for T[numel] arrays by num_blocks.
        static void IncreaseArrayPoolSize(size_t numel, std::size_t num_blocks);

        /// Set number of elements to add to Pool for T[numel] if it becomes empty.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step elements when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        static void SetArrayPoolGrowthStep(std::size_t numel, int growth_step);

        /// Return the current value of growth_step for pool for T[numel]
        static int GetArrayPoolGrowthStep(std::size_t numel );

        // return number of unallocated blocks in pool for T[numel]
        static std::size_t GetArrayPoolFree(std::size_t numel);

        // return total number (allocated & unallocated) of blocks for pool T[numel]
        static std::size_t GetArrayPoolSize(std::size_t numel);

        //Get a pointer to a block for storing T[numel]
        // This function is called to allocate memory for an array of objects.
        static void* operator new[](std::size_t n);

        // Return a block containing a T[numel] array to its pool.
        // block_size is stored in ptr[-1];
        // This function is called to deallocate memory for an object.
        static void operator delete[](void* ptr);

    private:

        static PoolList pool_list_;

    }; //class PooledNew

} //namespace ldl

#include "pooled_new.hpp"

#endif //! LDL_POOLED_NEW_H_