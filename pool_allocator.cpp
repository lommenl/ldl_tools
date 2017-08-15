#include "pool_allocator.h"

#include <iostream>
#include <exception>
#include <memory> //std::allocator
#include <cstdint> //std::uint8_t
#include <algorithm> // std::max

namespace ldl {

    //--------------
    Pool::Pool()
        : mutex_ptr_(nullptr)
        , nbytes_(0)
        , size_(0)
        , growth_step_(0)
    {}

    //--------------
    Pool::Pool(size_t nbytes, int growth_step)
        : mutex_ptr_(new c11::mutex())
        , nbytes_(nbytes)
        , size_(0)
        , growth_step_(growth_step)
    {
        if (nbytes == 0 ) {
            throw std::runtime_error("invalid nbytes argument");
        }
    }

    //--------------
    Pool::~Pool()
    {
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            for (size_t ix = 0; ix < size_; ++ix) {
                std::allocator<c11::uint8_t>().deallocate(static_cast<c11::uint8_t*>(stack_[ix]), nbytes_);
            }
#ifdef _DEBUG
        std::cout << "called std::allocator<uint8_t>().deallocate(ptr," << nbytes_ << ") "
            << size_ << " times" << std::endl;
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
            std::swap(nbytes_, other.nbytes_);
            std::swap(size_, other.size_);
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

    //-----------------
    size_t Pool::size()
    {
        size_t retval = 0;
        if (mutex_ptr_) { //object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            retval = stack_.size();
        }
        return retval;
    }

    //-----------------
    void Pool::Resize(size_t new_capacity)
    {
        if (mutex_ptr_) { //object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            NoLockResize(new_capacity);
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
    void* Pool::Pop()
    {
        void* retval = 0;
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            if (size_ == 0) { //if stack is empty
                if (growth_step_ > 0) { // growth_step is an increment
                    //  add growth_step_ elements to stack_
                    NoLockResize(stack_.size() + static_cast<size_t>(growth_step_));
                }
                else if (growth_step_ < 0) { // growth step is negative inverse of a scale factor. (e.g. -3 = a scale factor of 1/3
                    NoLockResize(stack_.size() + std::max<size_t>(1, stack_.size()/static_cast<size_t>(-growth_step_)));
                }
                else { // growth_step == 0 // no growth allowed
                    throw std::runtime_error("pool is empty and growth_step_ = 0");
                }
            }
            retval = stack_[--size_]; // get ptr from front of stack
        }
        return retval;
    }

    //-----------------
    void Pool::Push(void* ptr)
    {
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            if (size_ >= stack_.size() && growth_step_ == 0) {
                throw std::runtime_error("stack_ is full and growth_step_ = 0");
            }
            if (ptr) {
                stack_[size_++] = ptr;
            }
        }
    }

    //-----------------
    void Pool::NoLockResize(size_t new_capacity)
    {
        size_t old_capacity = stack_.size();
        if (new_capacity <= old_capacity) {
            return;
        }
        stack_.resize(new_capacity);
        for (size_t ix = old_capacity; ix < new_capacity; ++ix) {
            // for each new element allocate raw memory from the heap and push it onto the stack
            stack_[size_++] = static_cast<void*>(std::allocator<c11::uint8_t*>().allocate(nbytes_));
        }
#ifdef _DEBUG
        std::cout << "called std::allocator<uint8_t>().allocate(" << nbytes_ << ") "
            << (new_capacity - old_capacity) << " times" << std::endl;
#endif
    }

    //--------------
    Pool& PoolList::GetPool(size_t pool_num)
    {
        if (pool_num == 0 || pool_num > s.MAX_POOLS ) {
            throw std::runtime_error("Invalid pool_num argument");
        }
        //lock while accessing s
        c11::lock_guard<c11::mutex> lock(s.mutex);
        if (s.pool_list.count(pool_num) == 0) {
            // construct a Pool object with nbytes=pool_num and swap it into s.pool_list
            s.pool_list[pool_num].swap(Pool(pool_num,s.default_growth_step));
        }
        return s.pool_list[pool_num];
    }

    //--------------
    void PoolList::SetGrowthStep(size_t pool_num, int growth_step)
    {
        //lock while accessing s
        c11::lock_guard<c11::mutex> lock(s.mutex);
        if (pool_num == 0) { // set default, and all pools
            s.default_growth_step = growth_step;
            for (Statics::PoolList::iterator it = s.pool_list.begin(); it != s.pool_list.end(); ++it) {
                it->second.SetGrowthStep(growth_step);
            }
        }
        else { //set only pool_list[pool_num]
            GetPool(pool_num).SetGrowthStep(growth_step); // may create the pool
        }
    }

    //--------------
    int PoolList::GetGrowthStep(size_t pool_num)
    {
        //lock while accessing s
        c11::lock_guard<c11::mutex> lock(s.mutex);
        int retval = s.default_growth_step;
        if (s.pool_list.count(pool_num)) { // don't create a pool if it doesn't exist
            retval = GetPool(pool_num).GetGrowthStep();
        }
        return retval;
    }

    //--------------
    void PoolList::ResizePool(size_t pool_num, size_t new_capacity)
    {
        GetPool(pool_num).Resize(new_capacity); // may create the pool
    }

    //--------------
    size_t PoolList::GetPoolSize(size_t pool_num)
    {
        size_t retval = 0;
        if (s.pool_list.count(pool_num)) { // dont create a pool if it doesn't exist
            retval = GetPool(pool_num).size();
        }
        return retval;
    }

    //--------------
    void* PoolList::Pop(size_t pool_num)
    {
        return GetPool(pool_num).Pop();
    }

    //--------------
    void PoolList::Push(size_t pool_num, void* ptr)
    {
        GetPool(pool_num).Push(ptr);
    }

    //--------------
    PoolList::Statics PoolList::s;

    //---------------
    PoolList::Statics::Statics()
        : default_growth_step(10)
    {
    }

    //---------------
    PoolList::Statics::~Statics()
    {
    }

} //namespace ldl
