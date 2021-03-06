#pragma once
#ifndef SHARED_POINTER_H
#define SHARED_POINTER_H

#include "pooled_new.h"
#include "pooled_mutex.h"
#include "linkable.h"

#include<mutex> // lock_guard
namespace c11 {
    using namespace std;
}

namespace ldl {

    /// Almost-duplicate of c11::shared_ptr that does not perform internal
    /// memory allocations. Instead it uses a linked list to identify the owners of a given pointer.
    template<typename T>
    class SharedPointer : public Linkable {
    public:

        /// Type of the managed pointer.
        typedef T element_type;

        /// Type of function pointer to a delete function.
        /// Not an std::function<> object because std::function<> may do internal memory allocations.
        typedef void(*DeleteFunction)(T*);

        //---

        //Default constructor
        SharedPointer();

        // Construct from pointer
        template<typename U>
        SharedPointer(U* ptr);

        // construct from null pointer
        SharedPointer(nullptr_t);

        // construct from pointer and delete_func
        template<typename U>
        SharedPointer(U* ptr, DeleteFunction delete_func);

        // construct from null pointer and delete_func
        SharedPointer(nullptr_t, DeleteFunction delete_func);

        // copy constructor
        SharedPointer(SharedPointer& other);

        // copy constructor from a different SharedPointer specialization
        template<typename U>
        SharedPointer(SharedPointer<U>& other);

        // destructor
        ~SharedPointer();

        // Copy assignment operator
        template<typename U>
        SharedPointer& operator=(SharedPointer<U>& other);

        // swap state of two objects.
        void swap(SharedPointer<T>& other);

        // Reset object to an empty state.
        // Releases ownership of any currently owned object.
        void reset();

        // reset object and then reinitialize it as if constructed by SharedPointer(ptr)
        template<typename U>
        void reset(U* ptr);

        // reset object and then reinitialize it as if constructed by SharedPointer(ptr, delete_func)
        template<typename U>
        void reset(U* ptr, DeleteFunction delete_func);

        // return the managed pointer.
        element_type* get() const;

        // dereference the managed pointer
        element_type& operator*() const;

        // dereference the managed pointer for accessing its members.
        element_type* operator->() const;

        // return true if this object is managing a pointer.
        operator bool() const;

        // return the number of objects holding the same pointer as *this. (0 if this object is empty)
        long int use_count();

        // return true if this object is the only one holding its pointer.
        bool unique();

    private:

        // Add specified object after *this in its list
        // (other must be empty)
        // if replace = true reset *this after adding other
        void AddToList(SharedPointer& other, bool replace = false);

        //---

        // type of lock guard used to control mutex.
        typedef c11::lock_guard<PooledMutex> ListLock;

        //----

        // pointer to mutex (pointer makes it poolable and copyable/swapable.)
        PooledMutex* mutex_ptr_;

        // pointer to previous object in linked list of owners.
        SharedPointer* prev_ptr_;

        // pointer to next object in linked list of owners.
        SharedPointer* next_ptr_;

        // managed pointer
        T* obj_ptr_;

        // pointer to static function called to delete obj_ptr_
        DeleteFunction delete_func_;

        static const size_t element_size_;
    public:
#include "pooled_new.inc"

    }; //SharedPointer<T>

    // Equality comparison operators
    template<typename T>
    bool operator==(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator==(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator==(const SharedPointer<T>& lhs, nullptr_t);

    // inequality comparison operators
    template<typename T>
    bool operator!=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator!=(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator!=(const SharedPointer<T>& lhs, nullptr_t);

    // less than comparison operators
    template<typename T>
    bool operator<(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator<(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator<(const SharedPointer<T>& lhs, nullptr_t);

    // greater than comparison operators
    template<typename T>
    bool operator>(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator>(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator>(const SharedPointer<T>& lhs, nullptr_t);

    // greater than or equal to comparison operators.
    template<typename T>
    bool operator>=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator>=(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator>=(const SharedPointer<T>& lhs, nullptr_t);

    // less than or equal to comparison operators.
    template<typename T>
    bool operator<=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator<=(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator<=(const SharedPointer<T>& lhs, nullptr_t);

    // ostream operator
    template<typename charT, typename traits, typename T>
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const SharedPointer<T>& rhs);

} //namespace ldl

#include "shared_pointer.hpp"

#endif //! SHARED_POINTER_H
