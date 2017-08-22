#pragma once
#ifndef LDL_POOLED_ARRAY_H_
#define LDL_POOLED_ARRAY_H_

#include "pool.h"
#include "pooled_new.h"

#include <cstdint>
namespace c11 {
    using namespace std;
}

namespace ldl {

    /// Class that wraps an array of the specified type allocated from pooled memory.
    /// for classes, inheriting from PooledNew<T> adds new[] and delete[] operators that allow pooled arrays to be constructed
    /// "normally".
    /// However this doesn't work for non-class types that can't inherit from a base class. (e.g., uint8_t)
    /// Instead, this class tries to duplicate the c11::array class, except with a constructor that sets the array size when
    /// (or after) the object is constructed.
    template<typename T>
    class PooledArray {
    public:

        typedef T value_type;

        typedef value_type* iterator;

        typedef value_type const* const_iterator;

        //------

        // default constructor (empty array)
        PooledArray();

        // copy constructor
        PooledArray(const PooledArray& other);

        // set size constructor
        PooledArray(std::size_t numel);

        // destructor
        ~PooledArray();

        // copy assignment operator
        PooledArray& operator=(const PooledArray& other);

        // set the size of an uninitialized object.
        void SetSize(std::size_t numel);

        // swap contents with another object
        void swap(PooledArray& other);

        //------

        iterator begin();

        const_iterator begin() const;

        const_iterator cbegin() const;

        iterator end();

        const_iterator end() const;

        const_iterator cend() const;

        //---

        // return size of array
        std::size_t size() const;

        // return size of array
        std::size_t max_size() const;

        // return true if array is initialized
        operator bool() const;

        // set all array elements to val.
        void fill(const value_type& val);

        // return reference to element n
        value_type& operator[](std::size_t n);

        // return const reference to element n
        value_type const& operator[](std::size_t n) const;

        // return reference to element n
        value_type& at(std::size_t n);

        // return const reference to element n
        value_type const& at(std::size_t n) const;

        // return reference to element 0.
        value_type& front();

        // return const reference to element 0.
        value_type const& front() const;

        // return reference to element size-1.
        value_type& back();

        // return reference to element size-1.
        value_type const& back() const;

        // return pointer to buffer
        value_type* data();

        // return const pointer to buffer
        value_type const* data() const;

    private:

        // allocate and construct buffer
        void ConstructData(size_t numel);

        // destroy and deallocate buffer
        void DestroyData();

        //----

        std::size_t numel_;

        value_type* data_;

        //---------------

    public:

        // Increase the size of pool[numel] by num_blocks.
        static void IncreasePoolSize(std::size_t numel, std::size_t num_blocks);

        // Set the growth_step for the pool that holds buffers of size sizeof(T[numel])
        static void SetPoolGrowthStep(std::size_t numel, int growth_step);

        // Get the growth_step for the pool that holds buffers of size sizeof(T[numel])
        static int GetPoolGrowthStep(std::size_t numel);

        // Get the number of unallocated blocks in the pool that holds buffers of size sizeof(T[numel])
        static std::size_t GetPoolFree(std::size_t numel);

        // Get the total number of blocks (allocated and unallocat3ed) in the pool that holds buffers of size sizeof(T[numel])
        static std::size_t GetPoolSize(std::size_t numel);

    private:

        static PoolList pool_list_;

    }; //class PooledArray

       // return true if two objects have identical contents
    template<typename T>
    bool operator==(const PooledArray<T>& lhs, const PooledArray<T>& rhs);

    // return true if two objects do not have identical contents
    template<typename T>
    bool operator!=(const PooledArray<T>& lhs, const PooledArray<T>& rhs);

    // return true if lexicographical_compare(lhs,rhs) returns true
    template<typename T>
    bool operator<(const PooledArray<T>& lhs, const PooledArray<T>& rhs);

    // return true if lexicographical_compare(rhs,lhs) returns true
    template<typename T>
    bool operator>(const PooledArray<T>& lhs, const PooledArray<T>& rhs);

    // return true if lexicographical_compare(lhs,rhs) returns false
    template<typename T>
    bool operator>=(const PooledArray<T>& lhs, const PooledArray<T>& rhs);

    // return true if lexicographical_compare(rhs,lhs) returns false
    template<typename T>
    bool operator<=(const PooledArray<T>& lhs, const PooledArray<T>& rhs);

} //namespace ldl

#include "pooled_array.hpp"

#endif //!LDL_POOLED_ARRAY_H_