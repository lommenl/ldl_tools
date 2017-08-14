#include "pool_allocator.h"
#include <exception>
#include <memory> //std::allocator
#include <cstdint> //int8_t

namespace ldl {

    //--------------
    Pool::Pool()
        : mutex_ptr_(nullptr)
        , nbytes_(0)
        , size_(0)
        , growth_step_(0)
    {}

    //--------------
    Pool::Pool(size_t n)
        : mutex_ptr_(new c11::mutex())
        , nbytes_(n)
        , size_(0)
        , growth_step_(0)
    {}

    //--------------
    Pool::~Pool()
    {
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            for (size_t ix = 0; ix < size_; ++ix) {
                std::allocator<std::int8_t>().deallocate(stack_[ix], nbytes_);
            }
        }
    }

    //-----------------
    void Pool::swap(Pool& other)
    {
        //FIXME
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
    void Pool::resize(size_t new_capacity)
    {
        if (mutex_ptr_) { //object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            NoLockResize(new_capacity);
        }
    }

    //-----------------
    void Pool::SetGrowthStep(size_t nblocks)
    {
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            growth_step_ = nblocks;
        }
    }

    //-----------------
    size_t Pool::GetGrowthStep()
    {
        size_t retval = 0;
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            retval = growth_step_;
        }
        return retval;
    }

    //-----------------
    void* Pool::allocate()
    {
        void* retval = 0;
        if (mutex_ptr_) { // object is valid
            c11::lock_guard<c11::mutex> lock(*mutex_ptr_);
            if (size_ == 0) { //if stack is empty
                if (growth_step_ > 0) {
                    //  add growth_step_ elements to stack_
                    NoLockResize(stack_.size() + growth_step_);
                }
                else { // no growth allowed
                    throw std::runtime_error("stack_ is empty and growth_step_ = 0");
                }
            }
            retval = stack_[--size_]; // get ptr from front of stack
        }
        return retval;
    }

    //-----------------
    void Pool::deallocate(void* ptr)
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
            // allocate raw memory from the heap for each new element.
            stack_[ix] = std::allocator<std::int8_t>(nbytes_);
        }
#ifdef _DEBUG
        std::cout << "called std::allocator<int8_t>().allocate(" << nbytes_
            << ") " << (new_capacity - old_capacity) << " times" << std::endl;
#endif
    }

    //--------------
    Pool& PoolList::GetPool(size_t nbytes)
    {
        //lock while accessing s
        c11::lock_guard<c11::mutex> lock(s.mutex);
        if (s.pool_list.count(nbytes) == 0) {
            // construct a correctly initialized Pool object and swap it with the empty object in s.pool_list
            Pool(nbytes).swap(s.pool_list[n]);
            // set initial growth step to default.
            s.pool_list[n].SetGrowthStep(s.default_growth_step);
        }
        return s.pool_list[nbytes];
    }

    //--------------
    void PoolList::SetGrowthStep(size_t nblocks, size_t nbytes = 0)
    {
        //lock while accessing s
        c11::lock_guard<c11::mutex> lock(s.mutex);
        if (n == 0) { // set default, and all pools
            s.default_growth_step = nblocks;
            for (Statics::PoolList::iterator it = s.pool_list.begin(); it != s.pool_list.end(); ++it) {
                it->second.SetGrowthStep(nblocks);
            }
        }
        else { //set only pool n
            GetPool(n).SetGrowthStep(nblocks);
        }
    }

    //--------------
    size_t PoolList::GetGrowthStep(size_t nbytes = 0 )
    {
        //lock while accessing s
        c11::lock_guard<c11::mutex> lock(s.mutex);
        size_t retval = s.default_growth_step;
        if (nbytes != 0 && s.pool_list.count(nbytes)) { // only get value if pool already exists
            retval = GetPool(nbytes).GetGrowthStep();
        }
        return retval;
    }

    //--------------
    void PoolList::ResizePool(size_t nbytes, size_t new_capacity)
    {
        GetPool(nbytes).resize(new_capacity);
    }

    //--------------
    PoolList::Statics s;

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
