#include "pool.h"

#include <exception>
#include <algorithm> // std::max

#include <cstdint>
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
        if (num_blocks) {
            IncreaseSize(num_blocks);
        }
    }

    //-----------------
    void Pool::IncreaseSize(size_t num_blocks)
    {
        stack_.resize(stack_.size() + num_blocks);
        // Round block_size up to next multiple of 8 bytes.
        size_t padded_block_size = (block_size_ + 7) >> 3;
        for (size_t ix = 0; ix < num_blocks; ++ix) {
            // for each new element allocate raw memory from the heap and push it onto the stack
            // allocate uint64_t to get 64-bit alignment for all blocks (round up)
            Push(new c11::uint64_t[padded_block_size]);
        }
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
        if (tos_ >= stack_.size() && growth_step_ == 0) {
            throw std::bad_alloc();
        }
        if (ptr) {
            stack_[tos_++] = ptr;
        }
    }

} //namespace ldl
