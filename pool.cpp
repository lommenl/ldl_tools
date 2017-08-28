#include "pool.h"

#include <iostream>
#include <exception>
#include <memory> //std::allocator
#include <algorithm> // std::max

#include <mutex>
namespace c11 {
    using namespace std;
}

namespace ldl {

    //--------------
    Pool::Pool()
        : block_size_(0)
        , growth_step_(0)
        , tos_(0)
    {}

    //--------------
    Pool::Pool(size_t block_size, size_t num_blocks, int growth_step)
    {
        Initialize(block_size, num_blocks, growth_step);
    }

    //--------------
    Pool::~Pool()
    {
        while (!IsEmpty()) {
            delete[] Pop();
        }
    }

    //-----------------
    void Pool::swap(Pool& other)
    {
        if (this != &other) {
            std::swap(block_size_, other.block_size_);
            std::swap(growth_step_, other.growth_step_);
            std::swap(tos_, other.tos_);
            std::swap(stack_, other.stack_);
        }
    }

    //--------------
    void Pool::Initialize(size_t block_size, size_t num_blocks, int growth_step)
    {
        if (block_size == 0) {
            throw std::runtime_error("invalid block_size argument");
        }
        block_size_ = block_size;
        growth_step_ = growth_step;
        tos_ = 0;
    }

    //-----------------
    void Pool::IncreaseSize(size_t num_blocks)
    {
        stack_.resize(stack_.size() + num_blocks);
        size_t padded_block_size = (block_size_ + 7) >> 3;
        for (size_t ix = 0; ix < num_blocks; ++ix) {
            // for each new element allocate raw memory from the heap and push it onto the stack
            // allocate uint64_t to get 64-bit alignment for all blocks (round up)
            Push(new uint64_t[padded_block_size]);
        }
#ifdef _DEBUG
        std::cout << "Called new uint64_t[" << padded_block_size << "] ("
            << block_size_ << " bytes) " << num_blocks << " times" << std::endl;
#endif
    }

    //-----------------
    void Pool::SetGrowthStep(int growth_step)
    {
        growth_step_ = growth_step;
    }

    //-----------------
    int Pool::GetGrowthStep() const
    {
        return growth_step_;
    }

    //-----------------
    size_t Pool::GetBlockSize() const
    {
        return block_size_;
    }

    //-----------------
    size_t Pool::GetFree() const
    {
        return tos_;
    }

    //-----------------
    size_t Pool::GetSize() const
    {
        return stack_.size();
    }

    //-----------------
    bool Pool::IsEmpty() const
    {
        return (tos_ == 0);
    }

    //-----------------
    void* Pool::Pop()
    {
#ifdef _DEBUG
        std::cout << "-->Popping a(n) " << block_size_ << " byte block" << std::endl;
#endif //_DEBUG
        void* retval = 0;
        if (IsEmpty()) { //if stack is empty
            if (growth_step_ > 0) { // growth_step is an increment
                //  add growth_step_ elements to stack_
                IncreaseSize(static_cast<size_t>(growth_step_));
            }
            else if (growth_step_ < 0) { // growth step is negative inverse of a scale factor. (e.g. -3 = a scale factor of 1/3
                // always grow by at least 1.
                IncreaseSize(std::max<size_t>(1, stack_.size() / static_cast<size_t>(-growth_step_)));
            }
            else { // growth_step == 0 // no growth allowed
                throw std::bad_alloc();
            }
        }
        retval = stack_[--tos_]; // get ptr from front of stack
        return retval;
    }

    //-----------------
    void Pool::Push(void* ptr)
    {
#ifdef _DEBUG
            std::cout << "<--Pushing a(n) " << block_size_ << " byte block" << std::endl;
#endif //_DEBUG
        if (tos_ >= stack_.size() && growth_step_ == 0) {
            throw std::bad_alloc();
        }
        if (ptr) {
            stack_[tos_++] = ptr;
        }
    }

    //================================
    PoolList::PoolList()
        : default_growth_step_(0)
    {}

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
        // round block_size up to nearest multple of 8. (multiple block sizes will map to one pool)
        block_size = (block_size + 7) >> 3;
        if (!HasPool(block_size)) { // pool doesn't exist
            // construct a new Pool object
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
        // round block_size up to nearest multple of 8. (multiple block sizes will map to one pool)
        block_size = (block_size + 7) >> 3;
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
