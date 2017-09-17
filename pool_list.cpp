#include "pool_list.h"

#include <exception>

namespace ldl {

    //--------------
    PoolList::PoolList()
        : default_growth_step_(0)
    {}

    //--------------
    void PoolList::swap(PoolList& other)
    {
        if (this != &other) {
            std::swap(default_growth_step_, other.default_growth_step_);
            pool_map_.swap(other.pool_map_);
        }
    }

    //--------------
    void PoolList::Reset()
    {
        pool_map_.clear();
        default_growth_step_ = 0;
    }

    //--------------
    size_t PoolList::GetMaxPoolBlockSize() const
    {
        return MAX_BLOCK_SIZE_;
    }

    //--------------
    bool PoolList::HasPool(size_t block_size) const
    {
        return (pool_map_.count(block_size) != 0);
    }

    //--------------
    Pool& PoolList::GetPool(size_t block_size)
    {
        if (block_size == 0 || block_size > MAX_BLOCK_SIZE_) {
            throw std::runtime_error("Invalid block_size argument");
        }
        if (!HasPool(block_size)) { // pool doesn't exist
            // construct a new (empty) Pool object
            pool_map_[block_size].Initialize(block_size, 0, default_growth_step_); // empty pool
        }
        return pool_map_.at(block_size);
    }

    //--------------
    Pool const& PoolList::GetPool(size_t block_size) const
    {
        if (block_size == 0 || block_size > MAX_BLOCK_SIZE_ || !HasPool(block_size)) {
            throw std::runtime_error("Invalid block_size argument");
        }
        return pool_map_.at(block_size);
    }

    //--------------
    void PoolList::IncreasePoolSize(size_t block_size, size_t num_blocks)
    {
        GetPool(block_size).IncreaseSize(num_blocks); // GetPool() may create the pool
    }

    //--------------
    void PoolList::SetPoolGrowthStep(size_t block_size, int growth_step)
    {
        if (block_size == 0) { // set default, and all pools
            // set default growth_step for new pools.
            default_growth_step_ = growth_step;
            // set size of all existing pools to growth_step
            for (PoolMap::iterator it = pool_map_.begin(); it != pool_map_.end(); ++it) {
                it->second.SetGrowthStep(growth_step);
            }
        }
        else { //set only pool_list[block_size]
            GetPool(block_size).SetGrowthStep(growth_step); // GetPool() may create the pool
        }
    }

    //--------------
    int PoolList::GetPoolGrowthStep(size_t block_size) const
    {
        int retval = default_growth_step_;
        if (HasPool(block_size)) {
            retval = GetPool(block_size).GetGrowthStep();
        }
        return retval;
    }

    //--------------
    size_t PoolList::GetPoolFree(size_t block_size) const
    {
        size_t retval = 0;
        if (HasPool(block_size)) {
            retval = GetPool(block_size).GetFree();
        }
        return retval;
    }

    //--------------
    size_t PoolList::GetPoolSize(size_t block_size) const
    {
        size_t retval = 0;
        if (HasPool(block_size)) {
            retval = GetPool(block_size).GetSize();
        }
        return retval;
    }

    //--------------
    bool PoolList::PoolIsEmpty(size_t block_size) const
    {
        bool retval = true;
        if (HasPool(block_size)) {
            retval = GetPool(block_size).IsEmpty();
        }
        return retval;
    }

    //--------------
    void* PoolList::Pop(size_t block_size)
    {
        return GetPool(block_size).Pop(); // GetPool() may create the pool
    }

    //--------------
    void PoolList::Push(size_t block_size, void* ptr)
    {
        GetPool(block_size).Push(ptr); // GetPool() may create the pool
    }

} //namespace ldl
