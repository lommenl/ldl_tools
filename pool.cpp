#include "pool.h"

#include <iostream>
#include <exception>
#include <memory> //std::allocator
#include <algorithm> // std::max

#include <cstdint> //std::uint8_t
namespace c11 {
    using namespace std;
}

namespace ldl {

    //--------------
    Pool::Pool()
        : mutex_ptr_(nullptr)
        , block_size_(0)
        , tos_(0)
        , growth_step_(0)
    {}

    //--------------
    Pool::Pool(std::size_t block_size, std::size_t num_blocks, int growth_step)
    {
        Initialize(block_size, num_blocks, growth_step);
    }

    //--------------
    Pool::~Pool()
    {
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            for (std::size_t ix = 0; ix < tos_; ++ix) {
                std::allocator<c11::uint8_t>().deallocate(static_cast<c11::uint8_t*>(stack_[ix]), block_size_);
            }
#ifdef _DEBUG
        std::cout << "called std::allocator<uint8_t>().deallocate(ptr," << block_size_ << ") "
            << tos_ << " times" << std::endl;
#endif
        }
    }

    //-----------------
    void Pool::swap(Pool& other)
    {
        if (this != &other) {
            if (mutex_ptr_) {
                mutex_ptr_->lock();
            }
            if (other.mutex_ptr_) {
                other.mutex_ptr_->lock();
            }
            std::swap(mutex_ptr_, other.mutex_ptr_);
            std::swap(block_size_, other.block_size_);
            std::swap(tos_, other.tos_);
            std::swap(growth_step_, other.growth_step_);
            std::swap(stack_, other.stack_);
            if (mutex_ptr_) {
                mutex_ptr_->unlock();
            }
            if (other.mutex_ptr_) {
                other.mutex_ptr_->unlock();
            }
        }
    }

    //--------------
    void Pool::Initialize(std::size_t block_size, std::size_t num_blocks, int growth_step)
    {
        if (block_size == 0 ) {
            throw std::runtime_error("invalid block_size argument");
        }
        mutex_ptr_.reset(new c11::mutex());
        block_size_ = block_size;
        tos_ = 0;
        growth_step_ = growth_step;
    }

    //-----------------
    void Pool::Resize(std::size_t num_blocks)
    {
        if (mutex_ptr_) { //object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            NoLockResize(num_blocks);
        }
    }

    //-----------------
    void Pool::IncreaseSizeBy(std::size_t num_blocks)
    {
        if (mutex_ptr_) { //object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            NoLockResize( stack_.size() + num_blocks);
        }
    }

    //-----------------
    void Pool::SetGrowthStep(int growth_step)
    {
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            growth_step_ = growth_step;
        }
    }

    //-----------------
    int Pool::GetGrowthStep()
    {
        int retval = 0;
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            retval = growth_step_;
        }
        return retval;
    }

    //-----------------
    std::size_t Pool::GetBlockSize() const
    {
        return block_size_;
    }

    //-----------------
    std::size_t Pool::GetFree()
    {
        std::size_t retval = 0;
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            retval = tos_;
        }
        return retval;
    }

    //-----------------
    std::size_t Pool::GetCapacity()
    {
        std::size_t retval = 0;
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            retval = stack_.size();
        }
        return retval;
    }

    //-----------------
    void* Pool::Pop()
    {
        void* retval = 0;
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            if (tos_ == 0) { //if stack is empty
                if (growth_step_ > 0) { // growth_step is an increment
                    //  add growth_step_ elements to stack_
                    NoLockResize(stack_.size() + static_cast<std::size_t>(growth_step_));
                }
                else if (growth_step_ < 0) { // growth step is negative inverse of a scale factor. (e.g. -3 = a scale factor of 1/3
                    NoLockResize(stack_.size() + std::max<std::size_t>(1, stack_.size()/static_cast<std::size_t>(-growth_step_)));
                }
                else { // growth_step == 0 // no growth allowed
                    throw std::bad_alloc();
                }
            }
            retval = stack_[--tos_]; // get ptr from front of stack
        }
        return retval;
    }

    //-----------------
    void Pool::Push(void* ptr)
    {
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            if (tos_ >= stack_.size() && growth_step_ == 0) {
                throw std::bad_alloc();
            }
            if (ptr) {
                stack_[tos_++] = ptr;
            }
        }
    }

    //-----------------
    void Pool::NoLockResize(std::size_t num_blocks)
    {
        std::size_t old_capacity = stack_.size();
        if (num_blocks <= old_capacity) {
            return;
        }
        stack_.resize(num_blocks);
        for (std::size_t ix = old_capacity; ix < num_blocks; ++ix) {
            // for each new element allocate raw memory from the heap and push it onto the stack
            stack_[tos_++] = static_cast<void*>(std::allocator<c11::uint8_t*>().allocate(block_size_));
        }
#ifdef _DEBUG
        std::cout << "called std::allocator<uint8_t>().allocate(" << block_size_ << ") "
            << (num_blocks - old_capacity) << " times" << std::endl;
#endif
    }

    //================================

    //--------------
    Pool& PoolList::GetPool(std::size_t block_size)
    {
        if (block_size == 0 || block_size > MAX_BLOCK_SIZE ) {
            throw std::runtime_error("Invalid block_size argument");
        }
        //lock while accessing pool_map_
        c11::lock_guard<c11::mutex> lock(mutex_);
        if (pool_map_.count(block_size) == 0) {
            // construct a new Pool object
            pool_map_[block_size].Initialize(block_size, 0, default_growth_step_); // empty pool
        }
        return pool_map_.at(block_size);
    }

    //--------------
    void PoolList::ResizePool(std::size_t block_size, std::size_t num_blocks)
    {
        GetPool(block_size).Resize(num_blocks); // may create the pool
    }

    //--------------
    void PoolList::IncreasePoolSize(std::size_t block_size, std::size_t num_blocks)
    {
        GetPool(block_size).IncreaseSizeBy(num_blocks); // may create the pool
    }

    //--------------
    void PoolList::SetGrowthStep(std::size_t block_size, int growth_step)
    {
        if (block_size == 0) { // set default, and all pools
            //lock while accessing default_growth_step_ and pool_map_
            c11::lock_guard<c11::mutex> lock(mutex_);
            // set default growth_step for new pools.
            default_growth_step_ = growth_step;
            // set size of all existing pools to growth_step
            for (PoolMap::iterator it = pool_map_.begin(); it != pool_map_.end(); ++it) {
                it->second.SetGrowthStep(growth_step);
            }
        }
        else { //set only pool_list[block_size]
            GetPool(block_size).SetGrowthStep(growth_step); // may create the pool
        }
    }

    //--------------
    int PoolList::GetGrowthStep(std::size_t block_size)
    {
        //lock while accessing growth_step_
        c11::lock_guard<c11::mutex> lock(mutex_);
        int retval = default_growth_step_;
        if (pool_map_.count(block_size)) { // don't create a pool if it doesn't exist
            retval = GetPool(block_size).GetGrowthStep();
        }
        return retval;
    }

    //--------------
    std::size_t PoolList::GetPoolFree(std::size_t block_size)
    {
        // Pool will ensure its own thread safety.
        std::size_t retval = 0;
        if (pool_map_.count(block_size)) { // dont create a pool if it doesn't exist
            retval = GetPool(block_size).GetFree();
        }
        return retval;
    }

    //--------------
    std::size_t PoolList::GetPoolCapacity(std::size_t block_size)
    {
        std::size_t retval = 0;
        if (pool_map_.count(block_size)) { // dont create a pool if it doesn't exist
            retval = GetPool(block_size).GetCapacity();
        }
        return retval;
    }

    //--------------
    void* PoolList::Pop(std::size_t block_size)
    {
        return GetPool(block_size).Pop();
    }

    //--------------
    void PoolList::Push(std::size_t block_size, void* ptr)
    {
        GetPool(block_size).Push(ptr);
    }

    //==================================================

    //--------------
    PoolList StaticPoolList::pool_list_;

} //namespace ldl
