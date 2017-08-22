#pragma once
#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

namespace ldl {

    //===============
    /// Class defining a pointer that allows it to be used in a LinkedList object.
    template<typename T>
    class Linkable {
    public:
        Linkable() : next_ptr_(0) {}
    private:
        /// allow LinkedList to access next_ptr_
        template<typename U>
        friend class LinkedList;

        /// Pointer to next element in list.
        T* next_ptr_;
    };

    //===============
    /// Class defining a linked list of pointers to objects which inherit from class Linkabke<>.
    template<typename T>
    class LinkedList {
    public:

        /// Template class for defining iterator and const_iterator.
        template<typename U>
        class Iterator {
        public:
            // default constructor
            Iterator();
            // construct from pointer
            Iterator(U* ptr);
            // copy constructor
            Iterator(const Iterator& other);
            // copy assignment operator
            Iterator& operator=(const Iterator& other);
            // dereference operator
            U& operator*();
            // derefernce operator
            U* operator->();
            // preincrement operator
            Iterator& operator++();
            // postincrement operator
            Iterator& operator++(int);
            // equality operator
            bool operator==(const Iterator& other) const;
            // inequality operator
            bool operator!=(const Iterator& other) const;
        private:
            /// pointer to current Listable element in list
            U* ptr_;
        };

        // type defining iterator
        typedef Iterator<T> iterator;

        /// Type defining const_iterator
        typedef Iterator<const T> const_iterator;

        //---

        // default constructor (empty list)
        LinkedList();

        // copy constructor
        LinkedList(const LinkedList& other);

        // copy assignment operator
        LinkedList& operator=(const LinkedList& other);

        // destructor
        ~LinkedList();

        // swap contents of two objects
        void swap(LinkedList& other);

        // clear list contents
        void clear();

        // return true if list is empty
        bool empty() const;

        // return number of elements in list
        std::size_t size() const;

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

        // Insert a copy of val at the front of the list
        void push_front(const T& val);

        // insert the object referenced by ptr at the front of the list.
        // The list takes ownership of ptr, and deletes it when its element is removed from the list.
        void push_front(T* ptr);

        // remove the first element from the list.
        void pop_front();

    private:

        // pointer to first element in list
        T* begin_;

    };

} //namespace ldl

#include "linked_list.hpp"

#endif //! LINKED_LIST_H_