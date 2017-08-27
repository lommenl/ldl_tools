#pragma once
#ifndef LDL_POOLED_NEW_H_
#define LDL_POOLED_NEW_H_

#include "Pool.h" // StaticPoolList

#include <functional>
namespace c11 {
    using namespace std;
}

namespace ldl {

    class UnknownElementSize {};

    //-----------------------
    // Base class that will allocate buffers for an object from a Pool of memory.
    template<typename T>
    class PooledNew {
    public:

        //get a pointer to a block containing n bytes
        // This function is called by new to allocate memory for a single object.
        static void* operator new(size_t n);

        // Return a block containing a single element of type T to the pool
        // his function is called by delete to deallocate memory for a single object.
        static void operator delete(void* ptr);

        //----

        // Set the size of a single element.
        // Needed in cases when it can't be specified by a template parameter.
        static void SetElementSize(size_t element_size);

        // Increase size of pool holding single objects by num_blocks.
        static void IncreasePoolSize(size_t num_blocks);

        /// Set number of elements to automatically add to single object Pool if it becomes empty.
        // setting growth_step > 0 increases the pool size by  +growth_step elements when it is empty
        // setting growth_step < 0 increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        static void SetPoolGrowthStep(int growth_step);

        /// Return the current value of growth_step for the single object pool.
        static int GetPoolGrowthStep();

        // return number of unallocated blocks in the single object pool
        static size_t GetPoolFree();

        // return total number (allocated & unallocated) of blocks in the single object pool.
        static size_t GetPoolSize();

        // return true if the single object pool is empty
        static bool PoolIsEmpty();

        //----------------------

        // Get a pointer to a block for storing T[numel]
        // This function is called to allocate memory for an array of objects.
        static void* operator new[](size_t n);

        // Return a block containing a T[numel] array to its pool.
        // block_size is stored in ptr[-1];
        // This function is called to deallocate memory for an object.
        static void operator delete[](void* ptr);

        // increase size of pool for T[numel] arrays by num_blocks.
        static void IncreaseArrayPoolSize(size_t numel, size_t num_blocks);

        /// Set number of elements to add to Pool for T[numel] if it becomes empty.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step elements when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        static void SetArrayPoolGrowthStep(size_t numel, int growth_step);

        /// Return the current value of growth_step for pool for T[numel]
        static int GetArrayPoolGrowthStep(size_t numel);

        // return number of unallocated blocks in pool for T[numel]
        static size_t GetArrayPoolFree(size_t numel);

        // return total number (allocated & unallocated) of blocks for pool T[numel]
        static size_t GetArrayPoolSize(size_t numel);

        // return true if pool T[numel] is empty
        static bool GetArrayPoolIsEmpty(size_t numel);

    private:

        static size_t element_size_;

    }; //class PooledNew

} //namespace ldl

#include "pooled_new.hpp"

#endif //! LDL_POOLED_NEW_H_