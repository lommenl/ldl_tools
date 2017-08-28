#pragma once
#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include "pooled_new.h"
#include "shared_pointer.h"

namespace ldl {

    //===============
    class Linkable {
    private:
        template<typename T>
        friend class LinkedList;

        SharedPointer<Linkable> next_;
    };

    //===============
    /// base class for LinkedList that hold member variables.
    class LinkedListBase : public PooledNew<LinkedListBase> {

    protected:

        typedef SharedPointer<Linkable> LinkablePointer;

        // Shared pointer to first element in list
        LinkablePointer begin_;

    };

    //===============
    /// Class defining a linked list of pointers to objects which inherit from class Linkabke<>.
    template<typename T>
    class LinkedList : public LinkedListBase {
    public:

        typedef SharedPointer<T> Pointer;

        class const_iterator {
        public:
            const_iterator() : ptr_(0) {}
            const_iterator(LinkablePointer ptr) : ptr_(ptr) {}
            const_iterator(const const_iterator& other) : ptr_(other.ptr) {}
            const_iterator& operator=(const const_iterator& other) { if (this != &other) { (ptr_ = other.ptr_); } return *this; }
            const T& operator*() const { return *ptr_; }
            const T* operator->() const { return ptr_->get(); }
            const_iterator& operator++() { if (ptr_) { ptr_ = ptr->ptr_next_; } return *this; }
            const_iterator operator++(int) { const_iterator retval = *this; operator++(); return this; }
            bool operator==(const const_iterator& other) const { return (ptr_ = other.ptr_); }
            bool operator!=(const const_iterator& other) const { return (ptr_ != other.ptr_); }
        private:
            const LinkablePointer ptr_;
        };

        class iterator {
        public:
            iterator() : ptr_(0) {}
            iterator(LinkablePointer ptr) : ptr_(ptr) {}
            iterator(const iterator& other) : ptr_(other.ptr) {}
            iterator& operator=(const iterator& other) { if (this != &other) { (ptr_ = other.ptr_); } return *this; }
            T& operator*() const { return *ptr_; }
            T* operator->() const { ptr_.get(); }
            iterator& operator++() { if (ptr_) { ptr_ = ptr->ptr_next_; } return *this; }
            iterator operator++(int) { iterator retval = *this; operator++(); return this; }
            bool operator==(const iterator& other) const { return (ptr_ = other.ptr_); }
            bool operator!=(const iterator& other) const { return (ptr_ != other.ptr_); }
        private:
            LinkablePointer ptr_;
        };

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
        T const& front() const;

        // return shared pointer to element at front of list
        Pointer front_ptr();

        // insert the object referenced by ptr at the front of the list.
        // The list takes ownership of ptr, and deletes it when its element is removed from the list.
        void push_front(Pointer ptr);

        // remove and delete the first element in the list.
        void pop_front();

    };

} //namespace ldl

#include "linked_list.hpp"

#endif //! LINKED_LIST_H_