#pragma once
#ifndef LDL_LINKED_LIST_H_
#define LDL_LINKED_LIST_H_

#include "pooled_new.h"
#include "linkable.h"

namespace ldl {

    /// Class defining a linked list of pointers to objects which inherit from class Linkabke<>.
    template<typename T>
    class LinkedList : public Linkable {
    public:

        template<typename U>
        class Iterator {
        public:
            Iterator() : ptr_(0) {}
            Iterator(Linkable* const ptr) : ptr_(ptr) {}
            Iterator(const Iterator& other) : ptr_(other.ptr) {}
            Iterator& operator=(const Iterator& other) { if (this != &other) { (ptr_ = other.ptr_); } return *this; }
            T& operator*() const { return *static_cast<T*>(ptr_); }
            T* operator->() const { return static_cast<T*>(ptr_); }
            Iterator& operator++() { if (ptr_) { ptr_ = ptr_->next_linkable_; } return *this; }
            Iterator operator++(int) { Iterator retval = *this; operator++(); return this; }
            bool operator==(const Iterator& other) const { return (ptr_ = other.ptr_); }
            bool operator!=(const Iterator& other) const { return (ptr_ != other.ptr_); }
        private:
            Linkable* ptr_;
        };

        typedef Iterator<T> iterator;

        typedef Iterator<const T> const_iterator;

        //---

        // default constructor (empty list)
        LinkedList();

        // copy constructor
        LinkedList(const LinkedList& other);

        // destructor
        ~LinkedList();

        // copy assignment
        LinkedList& operator=(const LinkedList& other);

        // swap contents of two objects
        void swap(LinkedList& other);

        // clear list contents
        void clear();

        // return true if list is empty
        bool empty() const;

        // return number of elements in list
        size_t size() const;

        //---

        // return iterator to beginning of list
        iterator begin();

        // return const_iterator to beginning of list
        const_iterator begin() const;

        // return const_iterator to beginning of list
        const_iterator cbegin();

        // return iterator to end of list
        iterator end();

        // return const_iterator to end of list
        const_iterator end() const;

        // return const_iterator to end of list
        const_iterator cend();

        // return reference to element at front of list
        T& front();

        // return const reference to element at front of list
        const T& front() const;

        // insert the object referenced by ptr at the front of the list.
        // The list takes ownership of ptr, and deletes it when its
        // element is removed from the list.
        void push_front(T* ptr);

        // insert a copy of val at the front of the list.
        void push_front(const T& val);

        // remove and delete the first element in the list.
        void pop_front();

    private:

        // pointer to first element in list
        Linkable* begin_;

        //---

        static const size_t element_size_;

    public:
#include "pooled_new.inc"

    };

} //namespace ldl

#include "linked_list.hpp"

#endif //! LDL_LINKED_LIST_H_
