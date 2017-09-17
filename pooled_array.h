#pragma once
#ifndef LDL_POOLED_ARRAY_H_
#define LDL_POOLED_ARRAY_H_

#include "pooled_new.h"
#include "linkable.h"

#include <iterator>

namespace ldl {

    template<typename T, size_t N>
    class PooledArray : public Linkable {
    public:

        typedef T value_type;

        typedef T* iterator;

        typedef const T* const_iterator;

        typedef std::reverse_iterator<iterator> reverse_iterator;

        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        //---

        void fill(const T& val);

        // swap contents of two objects
        void swap(PooledArray& other);

        // return true if array is empty (size_>0)
        bool empty() const;

        // return number of elements in array
        size_t size() const;

        // return number of elements in array
        size_t max_size() const;

        T& operator[](size_t n);

        const T& operator[](size_t n) const;

        T& at(size_t n);

        const T& at(size_t n) const;

        // return reference to element at front of array
        T& front();

        // return const reference to element at front of array
        const T& front() const;

        // return reference to element at back of array
        T& back();

        // return const reference to element at back of array  
        const T& back() const;

        // return pointer to array
        T* data();

        // return pointer to array
        const T* data() const;

        //---

        // return iterator to beginning of array
        iterator begin();

        // return iterator to end of array
        iterator end();

        // return const_iterator to beginning of array
        const_iterator begin() const;

        // return const_iterator to end of array
        const_iterator end() const;

        // return const_iterator to beginning of array
        const_iterator cbegin();

        // return const_iterator to end of array
        const_iterator cend();

        // return reverse_iterator to beginning of array
        reverse_iterator rbegin();

        // return reverse_iterator to end of array
        reverse_iterator rend();

        // return const_reverse_iterator to beginning of array
        const_reverse_iterator rbegin() const;

        // return const_reverse_iterator to end of array
        const_reverse_iterator rend() const;

        // return const_reverse_iterator to beginning of array
        const_reverse_iterator crbegin();

        // return const_reverse_iterator to end of array
        const_reverse_iterator crend();

    private:

        T array_[N];

        //----

        static const size_t element_size_;

    public:

#include "pooled_new.inc"

    };

} //namespace ldl

#include "pooled_array.hpp"

#endif //! LDL_POOLED_ARRAY_H_
