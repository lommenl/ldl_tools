#include "shared_pointer.h"

namespace ldl {

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer()
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
    {
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(Mutex& mx)
        : mutex_ptr_(&mx)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
    {
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(U* ptr, Mutex& mx)
        : mutex_ptr_(&mx)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(static_cast<T*>(ptr))
    {
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(nullptr_t, Mutex& mx)
        : mutex_ptr_(&mx)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
    {
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(U* ptr, Deleter del, Mutex& mx)
        : mutex_ptr_(&mx)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(static_cast<T*>(ptr))
        , deleter_(del)
    {
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(nullptr_t, Deleter del, Mutex& mx)
        : mutex_ptr_(&mx)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
        , deleter_(del)
    {
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(SharedPointer& other)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
    {
        operator=(other);
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(SharedPointer<U>& other)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
    {
        operator=(other);
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::~SharedPointer()
    {
        reset();
    }

    //-----------------
    template<typename T>
    SharedPointer<T>&  SharedPointer<T>::operator=(SharedPointer<T>& other)
    {
        // if not same object, or same managed object
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            // remove *this from its current list.
            reset();
            // insert *this into others list
            ListLock lock(other); // lock other list
            mutex_ptr_ = other.mutex_ptr_;
            prev_ptr_ = &other;
            next_ptr_ = other.next_ptr_;
            other.next_ptr_ = this;
            obj_ptr_ = static_cast<T*>(other.obj_ptr_);
            deleter_ = other.deleter_;
        }
        return *this;
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>& SharedPointer<T>::operator=(SharedPointer<U>& other)
    {
        // if not same object, or same managed object
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            // remove *this from its current list
            reset();
            // insert *this into others list
            ListLock lock(other); // lock other list
            mutex_ptr_ = other.mutex_ptr_;
            prev_ptr_ = &other;
            next_ptr_ = other.next_ptr_;
            other.next_ptr_ = this;
            obj_ptr_ = static_cast<T*>(other.obj_ptr_);
            if (other.deleter_) {  // if a deleter was specified for other.
                // set deleter_(ptr) to call other.deleter_(static_cast<U*>(ptr));
                deleter_ = CastDeleter<U>(other.deleter_);
            }
        }
        return *this;
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::swap(SharedPointer& other)
    {
        // if not same object, or same managed object
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            // lock both this objects list and others list
            ListLock lock(*this);
            ListLock lock2(other);
            std::swap(mutex_ptr_, other.mutex_ptr_);
            std::swap(prev_ptr_, other.prev_ptr_);
            std::swap(next_ptr_, other.next_ptr_);
            std::swap(obj_ptr_, other.obj_ptr_);
            std::swap(deleter_, other.deleter_);
        }
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::reset()
    {
        { // mutex block
            ListLock lock(*this);
            // if this is the only item in the list, delete obj_ptr_
            if (obj_ptr_ != 0 && prev_ptr_ == 0 && next_ptr_ == 0) {
                if (deleter_) {
                    deleter_(obj_ptr_);
                }
                else {
                    delete obj_ptr_;
                }
            }
            // if necessary, fix pointer in *prev_ptr_
            if (prev_ptr_) {
                prev_ptr_->next_ptr_ = next_ptr_;
            }
            // if necessary, fix pointer in *next_ptr_
            if (next_ptr_) {
                next_ptr_->prev_ptr_ = prev_ptr_;
            }
            // reset this to empty state
            prev_ptr_ = 0;
            next_ptr_ = 0;
            obj_ptr_ = 0;
            deleter_ = Deleter();
        } // unlock mutex before resetting mutex_ptr_
        mutex_ptr_ = 0;
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::reset(Mutex& mx)
    {
        reset();
        swap(SharedPointer(mx));
    }

    //-----------------
    template<typename T>
    template<typename U>
    void SharedPointer<T>::reset(U* ptr, Mutex& mx)
    {
        reset();
        swap(SharedPointer(ptr, mx));
    }

    //-----------------
    template<typename T>
    template<typename U>
    void SharedPointer<T>::reset(U* ptr, Deleter del, Mutex& mx)
    {
        reset();
        swap(SharedPointer(ptr, del, mx));
    }

    //-----------------
    template<typename T>
    typename SharedPointer<T>::element_type* SharedPointer<T>::get() const
    {
        return obj_ptr_;
    }

    //-----------------
    template<typename T>
    typename SharedPointer<T>::element_type& SharedPointer<T>::operator*() const
    {
        return *obj_ptr_;
    }

    //-----------------
    template<typename T>
    typename SharedPointer<T>::element_type* SharedPointer<T>::operator->() const
    {
        return obj_ptr_;
    }

    //-----------------
    template<typename T>
    long int SharedPointer<T>::use_count()
    {
        long int retval = 0;
        if (obj_ptr_ != 0) {
            ListLock lock(*this);
            for (SharedPointer* ptr = this; ptr != 0; ptr = ptr->prev_ptr_) {
                ++retval;
            }
            for (SharedPointer* ptr = next_ptr_; ptr != 0; ptr = ptr->next_ptr_) {
                ++retval;
            }
        }
        return retval;
    }

    //-----------------
    template<typename T>
    bool SharedPointer<T>::unique()
    {
        ListLock lock(*this);
        return (obj_ptr_ != 0 && prev_ptr_ == 0 && next_ptr_ == 0);
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::operator bool() const
    {
        return (obj_ptr_ != 0);
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::lock() {
        if (mutex_ptr_) {
            mutex_ptr_->lock();
        }
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::unlock() {
        if (mutex_ptr_) {
            mutex_ptr_->unlock();
        }
    }

    //-----------------
    template<typename T>
    void swap(SharedPointer<T>& lhs, SharedPointer<T>& rhs)
    {
        lhs.swap(rhs);
    }

    //-----------------
    template<typename T>
    bool operator==(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() == rhs.get()); }
    template<typename T>
    bool operator==(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() == nullptr); }
    template<typename T>
    bool operator==(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() == nullptr); }

    //-----------------
    template<typename T>
    bool operator!=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() != rhs.get()); }
    template<typename T>
    bool operator!=(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() != nullptr); }
    template<typename T>
    bool operator!=(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() != nullptr); }

    //-----------------
    template<typename T>
    bool operator<(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() < rhs.get()); }
    template<typename T>
    bool operator<(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() < nullptr); }
    template<typename T>
    bool operator<(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() < nullptr); }

    //-----------------
    template<typename T>
    bool operator>(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() > rhs.get()); }
    template<typename T>
    bool operator>(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() > nullptr); }
    template<typename T>
    bool operator>(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() > nullptr); }

    //-----------------
    template<typename T>
    bool operator>=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() >= rhs.get()); }
    template<typename T>
    bool operator>=(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() >= nullptr); }
    template<typename T>
    bool operator>=(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() >= nullptr); }

    //-----------------
    template<typename T>
    bool operator<=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs) { return (lhs.get() <= rhs.get()); }
    template<typename T>
    bool operator<=(nullptr_t, const SharedPointer<T>& rhs) { return (lhs.get() <= nullptr); }
    template<typename T>
    bool operator<=(const SharedPointer<T>& lhs, nullptr_t) { return (lhs.get() <= nullptr); }

    //-----------------
    template<typename charT, typename traits, typename T>
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const SharedPointer<T>& rhs)
    {
        return (os << rhs.get());
    }

} //namespace ldl
