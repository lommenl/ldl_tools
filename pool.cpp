#include "pool.h"

#include <iostream>
#include <exception>
#include <memory> //std::allocator
#include <algorithm> // std::max

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
                std::allocator<c11::uint64_t>().deallocate(static_cast<c11::uint64_t*>(stack_[ix]), (block_size_+7)/8);
            }
#ifdef _DEBUG
        std::cout << "Called std::allocator<uint64_t>().deallocate(ptr," << (block_size_ + 7) / 8 << ") ("
            << block_size_ << " bytes) " << tos_ << " times" << std::endl;
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
    void Pool::IncreaseSize(std::size_t num_blocks)
    {
        if (mutex_ptr_) { //object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            NoLockIncreaseSize( stack_.size() + num_blocks);
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
    std::size_t Pool::GetSize()
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
                    NoLockIncreaseSize(static_cast<std::size_t>(growth_step_));
                }
                else if (growth_step_ < 0) { // growth step is negative inverse of a scale factor. (e.g. -3 = a scale factor of 1/3
                    NoLockIncreaseSize(std::max<std::size_t>(1, stack_.size()/static_cast<std::size_t>(-growth_step_)));
                }
                else { // growth_step == 0 // no growth allowed
                    throw std::bad_alloc();
                }
            }
            retval = stack_[--tos_]; // get ptr from front of stack
#ifdef _DEBUG
            std::cout << "-->Popped a(n) " << block_size_ << " byte block" << std::endl;
#endif //_DEBUG
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
#ifdef _DEBUG
                std::cout << "<--Pushed a(n) " << block_size_ << " byte block" << std::endl;
#endif //_DEBUG
            }
        }
    }

    //-----------------
    void Pool::NoLockIncreaseSize(std::size_t num_blocks)
    {
        stack_.resize(stack_.size() + num_blocks );
        for (std::size_t ix = 0; ix < num_blocks; ++ix) {
            // for each new element allocate raw memory from the heap and push it onto the stack
            // allocate uint64_t to get 64-bit alignment for all blocks (round up)
            stack_[tos_++] = static_cast<void*>(std::allocator<c11::uint64_t*>().allocate((block_size_+7)/8));
        }
#ifdef _DEBUG
        std::cout << "Called std::allocator<uint64_t>().allocate(" << (block_size_ + 7) / 8 << ") ("
            << block_size_ << " bytes) " << num_blocks << " times" << std::endl;
#endif
    }

    //================================

    //--------------
    Pool& PoolList::GetPool(std::size_t block_size)
    {
        if (block_size == 0 || block_size > MAX_BLOCK_SIZE ) {
            throw std::runtime_error("Invalid block_size argument");
        }
        // round block_size up to nearest multple of 8. (multiple block sizes will map to one pool)
        std::size_t rounded_block_size = (block_size + 7) & (~0x7UL);
        //lock while accessing pool_map_
        c11::lock_guard<c11::mutex> lock(mutex_);
        if (pool_map_.count(rounded_block_size) == 0) { // pool doesn't exist
            // construct a new Pool object
            pool_map_[rounded_block_size].Initialize(rounded_block_size, 0, default_growth_step_); // empty pool
        }
        return pool_map_.at(rounded_block_size);
    }

    //--------------
    void PoolList::IncreasePoolSize(std::size_t block_size, std::size_t num_blocks)
    {
        std::size_t rounded_block_size = (block_size + 7) & (~0x7UL);
        GetPool(rounded_block_size).IncreaseSize(num_blocks); // GetPool() may create the pool
    }

    //--------------
    void PoolList::SetPoolGrowthStep(std::size_t block_size, int growth_step)
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
            std::size_t rounded_block_size = (block_size + 7) & (~0x7UL);
            GetPool(rounded_block_size).SetGrowthStep(growth_step); // GetPool() may create the pool
        }
    }

    //--------------
    int PoolList::GetPoolGrowthStep(std::size_t block_size)
    {
        //lock while accessing growth_step_
        c11::lock_guard<c11::mutex> lock(mutex_);
        int retval = default_growth_step_;
        std::size_t rounded_block_size = (block_size + 7) & (~0x7UL);
        if (pool_map_.count(rounded_block_size)) { // don't create a pool if it doesn't already exist
            retval = GetPool(rounded_block_size).GetGrowthStep();
        }
        return retval;
    }

    //--------------
    std::size_t PoolList::GetPoolFree(std::size_t block_size)
    {
        // Pool will ensure its own thread safety.
        std::size_t retval = 0;
        std::size_t rounded_block_size = (block_size + 7) & (~0x7UL);
        if (pool_map_.count(rounded_block_size)) { // dont create a pool if it doesn't already exist
            retval = GetPool(rounded_block_size).GetFree();
        }
        return retval;
    }

    //--------------
    std::size_t PoolList::GetPoolSize(std::size_t block_size)
    {
        std::size_t retval = 0;
        std::size_t rounded_block_size = (block_size + 7) & (~0x7UL);
        if (pool_map_.count(rounded_block_size)) { // dont create a pool if it doesn't already exist
            retval = GetPool(rounded_block_size).GetSize();
        }
        return retval;
    }

    //--------------
    void* PoolList::Pop(std::size_t block_size)
    {
        return GetPool(block_size).Pop(); // GetPool() may create the pool
    }

    //--------------
    void PoolList::Push(std::size_t block_size, void* ptr)
    {
        GetPool(block_size).Push(ptr); // GetPool() may create the pool
    }

    //==================================================

    //--------------
    PoolList StaticPoolList::pool_list_;

} //namespace ldl
