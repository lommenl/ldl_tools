#include "shared_pointer.h"

#include <algorithm> //std::swap

namespace ldl {

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer()
        : SharedPointerBase()
    {
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(U* ptr)
        : SharedPointerBase()
    {
        reset(ptr);
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(nullptr_t)
        : SharedPointerBase()
    {
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(U* ptr, Deleter deleter)
        : SharedPointerBase()
    {
        reset(ptr, deleter);
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(nullptr_t, Deleter deleter)
        : SharedPointerBase()
    {
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(SharedPointer& other)
        : SharedPointerBase()
    {
        other.AddToList(*this);
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(SharedPointer<U>& other)
        : SharedPointerBase()
    {
        other.AddToList(*this);
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::~SharedPointer()
    {
        reset();
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>& SharedPointer<T>::operator=(SharedPointer<U>& other)
    {
        reset();
        other.AddToList(*this);
        return *this;
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::swap(SharedPointer& other)
    {
        // if not the same object, and not already in the same list
        if (this != &other && void_ptr_ != other.void_ptr_) {
            if (mutex_ptr_) {
                ListLock lock(*mutex_ptr_); // lock current list during swap
                SharedPointer temp; // (temp = 0);
                // (temp = *this; *this = 0)
                std::swap(temp.mutex_ptr_, mutex_ptr_);
                std::swap(temp.prev_ptr_, prev_ptr_);
                std::swap(temp.next_ptr_, next_ptr_);
                std::swap(temp.void_ptr_, void_ptr_);
                std::swap(temp.deleter_ptr_, deleter_ptr_);
                other.AddToList(*this); // (*this = other)
                other.reset(); // (other = 0)
                // (other = temp; temp = 0)
                std::swap(other.mutex_ptr_, temp.mutex_ptr_);
                std::swap(other.prev_ptr_, temp.prev_ptr_);
                std::swap(other.next_ptr_, temp.next_ptr_);
                std::swap(other.void_ptr_, temp.void_ptr_);
                std::swap(other.deleter_ptr_, temp.deleter_ptr_);
            }
            else { // no current list (*this == 0)
                other.AddToList(*this);// (*this = other)
                other.reset(); // (other = 0)
            }
        }
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::reset()
    {
        T* ptr = static_cast<T*>(RemoveFromList());
        if (ptr) { // need to delete ptr
            if (deleter_ptr_) {
                reinterpret_cast<void(*)(T*)>(deleter_ptr_)(ptr);
                deleter_ptr_ = 0;
            }
            else {
                delete ptr;
            }
            void_ptr_ = 0;
        }
    }

    //-----------------
    template<typename U>
    void std_deleter(U* ptr) {
        delete ptr;
    }

    //-----------------
    template<typename T>
    template<typename U>
    void SharedPointer<T>::reset(U* ptr)
    {
        reset(ptr, std_deleter<U>);
    }

    //-----------------
    template<typename T>
    template<typename U>
    void SharedPointer<T>::reset(U* ptr, Deleter deleter)
    {
        reset();
        if (ptr) {
            mutex_ptr_ = new PooledMutex();
            void_ptr_ = static_cast<void*>(ptr);
            deleter_ptr_ = reinterpret_cast<VoidDeleter>(deleter);
        }
    }

    //-----------------
    template<typename T>
    typename SharedPointer<T>::element_type* SharedPointer<T>::get() const
    {
        return static_cast<T*>(void_ptr_);
    }

    //-----------------
    template<typename T>
    typename SharedPointer<T>::element_type& SharedPointer<T>::operator*() const
    {
        return *static_cast<T*>(void_ptr_);
    }

    //-----------------
    template<typename T>
    typename SharedPointer<T>::element_type* SharedPointer<T>::operator->() const
    {
        return static_cast<T*>(void_ptr_);
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::operator bool() const
    {
        return (void_ptr_ != 0);
    }

    //-----------------
    template<typename T>
    long int SharedPointer<T>::use_count()
    {
        long int retval = 0;
        if (mutex_ptr_) {
            ListLock lock(*mutex_ptr_);
            for (SharedPointerBase* ptr = this; ptr!=nullptr; ptr = ptr->prev_ptr_) {
                ++retval;
            }
            for (SharedPointerBase* ptr = next_ptr_; ptr!=nullptr; ptr = ptr->next_ptr_) {
                ++retval;
            }
        }
        return retval;
    }

    //-----------------
    template<typename T>
    bool SharedPointer<T>::unique()
    {
        bool retval = false;
        if (mutex_ptr_) {
            ListLock lock(*mutex_ptr_);
            retval = (prev_ptr_ == 0 && next_ptr_ == 0);
        }
        return retval;
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::IncreasePoolSize(size_t numel)
    {
        PooledNew::IncreasePoolSize(numel);
        // also increase size of PooledMutex
        PooledMutex::IncreasePoolSize(numel);
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::AddToList(SharedPointer& other)
    {
        // assumes other is not in any list
        if (mutex_ptr_) { // if *this is in a list
            ListLock lock(*mutex_ptr_);
            // insert other after *this
            other.void_ptr_ = void_ptr_;
            other.deleter_ptr_ = deleter_ptr_;
            other.mutex_ptr_ = mutex_ptr_;
            other.next_ptr_ = next_ptr_;
            other.prev_ptr_ = this;
            next_ptr_ = &other;
        }
    }

    //-----------------
    template<typename T>
    void* SharedPointer<T>::RemoveFromList()
    {
        void* retval = 0;
        if (mutex_ptr_) { // if in a list
            ListLock lock(*mutex_ptr_);
            // if this is the only object in the list, return void_ptr_
            // to indicate it needs to be deleted
            if (void_ptr_ && !prev_ptr_ && !next_ptr_) {
                retval = void_ptr_;
            }
            // if necessary, update *prev_ptr_
            if (prev_ptr_) {
                prev_ptr_->next_ptr_ = next_ptr_;
            }
            // if necessary, update *next_ptr_
            if (next_ptr_) {
                next_ptr_->prev_ptr_ = prev_ptr_;
            }
        } // unlock mutex before deleting it
        // if list is empty, delete mutex_ptr_
        if (mutex_ptr_ && retval) {
            delete mutex_ptr_;
        }
        mutex_ptr_ = 0;
        prev_ptr_ = 0;
        next_ptr_ = 0;
        if (!retval) {
            void_ptr_ = 0;
            deleter_ptr_ = 0;
        }
        return retval;
    }


    //-----------------
    // lhs==rhs
    template<typename T>
    bool operator==(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() == rhs.get()); }
    // lhs==0
    template<typename T>
    bool operator==(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() == nullptr); }
    // 0==rhs
    template<typename T>
    bool operator==(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() == nullptr); }

    //-----------------
    // 0!=rhs
    template<typename T>
    bool operator!=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() != rhs.get()); }
    // lhs!=0
    template<typename T>
    bool operator!=(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() != nullptr); }
    // 0!=rhs
    template<typename T>
    bool operator!=(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() != nullptr); }

    //-----------------
    // lhs<rhs
    template<typename T>
    bool operator<(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() < rhs.get()); }
    // lhs<0
    template<typename T>
    bool operator<(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() < nullptr); }
    // 0<rhs
    template<typename T>
    bool operator<(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() < nullptr); }

    //-----------------
    // lhs>rhs
    template<typename T>
    bool operator>(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() > rhs.get()); }
    // lhs>0
    template<typename T>
    bool operator>(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() > nullptr); }
    // 0>rhs
    template<typename T>
    bool operator>(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() > nullptr); }

    //-----------------
    // lhs>=rhs
    template<typename T>
    bool operator>=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() >= rhs.get()); }
    // lhs>=0
    template<typename T>
    bool operator>=(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() >= nullptr); }
    // 0>=rhs
    template<typename T>
    bool operator>=(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() >= nullptr); }

    //-----------------
    // lhs<=rhs
    template<typename T>
    bool operator<=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() <= rhs.get()); }
    // lhs<=0
    template<typename T>
    bool operator<=(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() <= nullptr); }
    // 0<=rhs
    template<typename T>
    bool operator<=(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() <= nullptr); }

    //-----------------
    template<typename charT, typename traits, typename T>
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const SharedPointer<T>& rhs)
    {
        return (os << rhs.get());
    }

} //namespace ldl
