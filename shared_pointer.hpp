#include "shared_pointer.h"

namespace ldl {

    //-----------------
    template<typename T, typename M>
    SharedPointer<T, M>::SharedPointer()
        : mutex_ptr_(0)
        , prev_sp_(0)
        , next_sp_(0)
        , obj_ptr_(0)
    {
    }

    //-----------------
    template<typename T, typename M>
    template<typename U>
    SharedPointer<T, M>::SharedPointer(U* ptr, M& mx)
        : mutex_ptr_(&mx)
        , prev_sp_(0)
        , next_sp_(0)
        , obj_ptr_(static_cast<T*>(ptr))
    {
    }

    //-----------------
    template<typename T, typename M>
    SharedPointer<T, M>::SharedPointer(nullptr_t, M& mx)
        : mutex_ptr_(&mx)
        , prev_sp_(0)
        , next_sp_(0)
        , obj_ptr_(0)
    {
    }

    //-----------------
    template<typename T, typename M>
    template<typename U>
    SharedPointer<T, M>::SharedPointer(U* ptr, Deleter del, M& mx)
        : mutex_ptr_(&mx)
        , prev_sp_(0)
        , next_sp_(0)
        , obj_ptr_(static_cast<T*>(ptr))
        , deleter_(del)
    {
    }

    //-----------------
    template<typename T, typename M>
    SharedPointer<T, M>::SharedPointer(nullptr_t, Deleter del, M& mx)
        : mutex_ptr_(&mx)
        , prev_sp_(0)
        , next_sp_(0)
        , obj_ptr_(0)
        , deleter_(del)
    {
    }

    //-----------------
    template<typename T, typename M>
    SharedPointer<T, M>::SharedPointer(SharedPointer& other)
        : mutex_ptr_(0)
        , prev_sp_(0)
        , next_sp_(0)
        , obj_ptr_(0)
    {
        operator=(other);
    }

    //-----------------
    template<typename T, typename M>
    template<typename U>
    SharedPointer<T, M>::SharedPointer(SharedPointer<U, M>& other)
        : mutex_ptr_(0)
        , prev_sp_(0)
        , next_sp_(0)
        , obj_ptr_(0)
    {
        operator=(other);
    }

    //-----------------
    template<typename T, typename M>
    SharedPointer<T, M>::~SharedPointer()
    {
        reset();
    }

    //-----------------
    template<typename T, typename M>
    SharedPointer<T, M>&  SharedPointer<T, M>::operator=(SharedPointer<T, M>& other)
    {
        // if not same object, or same list
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            // remove object from current list
            reset();
            ListLock lock(other);
            // insert object into other list
            mutex_ptr_ = other.mutex_ptr_;
            prev_sp_ = &other;
            next_sp_ = other.next_sp_;
            other.next_sp_ = this;
            obj_ptr_ = static_cast<T*>(other.obj_ptr_);
            deleter_ = other.deleter_;
        }
        return *this;
    }

    //-----------------
    template<typename T, typename M>
    template<typename U>
    SharedPointer<T, M>& SharedPointer<T, M>::operator=(SharedPointer<U, M>& other)
    {
        // if not same object, or same list
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            // remove object from current list
            reset();
            ListLock lock(other);
            // insert object into other list
            mutex_ptr_ = other.mutex_ptr_;
            prev_sp_ = &other;
            next_sp_ = other.next_sp_;
            other.next_sp_ = this;
            obj_ptr_ = static_cast<T*>(other.obj_ptr_);
            if (other.deleter_) {  // if a deleter was specified for other.
                // set deleter_(ptr) to call other.deleter_(static_cast<U*>(ptr));
                deleter_ = CastDeleter<U>(other.deleter_);
            }
        }
        return *this;
    }

    //-----------------
    template<typename T, typename M>
    void SharedPointer<T, M>::swap(SharedPointer& other)
    {
        // if not same object, or same list
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            ListLock lock(*this);
            ListLock lock2(other);
            std::swap(mutex_ptr_, other.mutex_ptr_);
            std::swap(prev_sp_, other.prev_sp_);
            std::swap(next_sp_, other.next_sp_);
            std::swap(obj_ptr_, other.obj_ptr_);
            std::swap(deleter_, other.deleter_);
        }
    }

    //-----------------
    template<typename T, typename M>
    void SharedPointer<T, M>::reset()
    {
        { // mutex block
            ListLock lock(*this);

            // if this is the only item in the list, delete obj_ptr_
            if (obj_ptr_ != 0 && prev_sp_ == 0 && next_sp_ == 0) {
                if (deleter_) {
                    deleter_(obj_ptr_);
                }
                else {
                    delete obj_ptr_;
                }
            }
            // if necessary, fix pointer in *prev_sp_
            if (prev_sp_) {
                prev_sp_->next_sp_ = next_sp_;
            }
            // if necessary, fix pointer in *next_sp_
            if (next_sp_) {
                next_sp_->prev_sp_ = prev_sp_;
            }
            // reset this to empty state
            prev_sp_ = 0;
            next_sp_ = 0;
            obj_ptr_ = 0;
            deleter_ = Deleter();
        } // unlock mutex before clearing mutex_ptr_
        mutex_ptr_ = 0;
    }

    //-----------------
    template<typename T, typename M>
    template<typename U>
    void SharedPointer<T, M>::reset(U* ptr, M& mx)
    {
        reset();
        swap(SharedPointer(ptr, mx));
    }

    //-----------------
    template<typename T, typename M>
    template<typename U>
    void SharedPointer<T, M>::reset(U* ptr, Deleter del, M& mx)
    {
        reset();
        swap(SharedPointer(ptr, del, mx));
    }

    //-----------------
    template<typename T, typename M>
    T* SharedPointer<T, M>::get() const
    {
        return obj_ptr_;
    }

    //-----------------
    template<typename T, typename M>
    T& SharedPointer<T, M>::operator*() const
    {
        return *obj_ptr_;
    }

    //-----------------
    template<typename T, typename M>
    T* SharedPointer<T, M>::operator->() const
    {
        return obj_ptr_;
    }

    //-----------------
    template<typename T, typename M>
    long int SharedPointer<T, M>::use_count()
    {
        long int retval = 0;
        if (obj_ptr_ != 0) {
            ListLock lock(*this);
            for (SharedPointer* ptr = this; ptr != 0; ptr = ptr->prev_sp_) {
                ++retval;
            }
            for (SharedPointer* ptr = next_sp_; ptr != 0; ptr = ptr->next_sp_) {
                ++retval;
            }
        }
        return retval;
    }

    //-----------------
    template<typename T, typename M>
    bool SharedPointer<T, M>::unique()
    {
        ListLock lock(*this);
        return (obj_ptr_ != 0 && prev_sp_ == 0 && next_sp_ == 0);
    }

    //-----------------
    template<typename T, typename M>
    SharedPointer<T, M>::operator bool() const
    {
        return (obj_ptr_ != 0);
    }

    //-----------------
    template<typename T, typename M>
    void SharedPointer<T, M>::lock() {
        if (mutex_ptr_) {
            mutex_ptr_->lock();
        }
    }

    //-----------------
    template<typename T, typename M>
    void SharedPointer<T, M>::unlock() {
        if (mutex_ptr_) {
            mutex_ptr_->unlock();
        }
    }

    //-----------------
    template<typename T, typename M>
    void swap(SharedPointer<T, M>& lhs, SharedPointer<T, M>& rhs)
    {
        lhs.swap(rhs);
    }

    //-----------------
    template<typename T, typename M>
    bool operator==(const SharedPointer<T, M>& lhs, const SharedPointer<T, M>& rhs) { return (lhs.get() == rhs.get()); }
    template<typename T, typename M>
    bool operator==(nullptr_t, const SharedPointer<T, M>& rhs) { return (lhs.get() == nullptr); }
    template<typename T, typename M>
    bool operator==(const SharedPointer<T, M>& lhs, nullptr_t) { return (lhs.get() == nullptr); }

    //-----------------
    template<typename T, typename M>
    bool operator!=(const SharedPointer<T, M>& lhs, const SharedPointer<T, M>& rhs) { return (lhs.get() != rhs.get()); }
    template<typename T, typename M>
    bool operator!=(nullptr_t, const SharedPointer<T, M>& rhs) { return (lhs.get() != nullptr); }
    template<typename T, typename M>
    bool operator!=(const SharedPointer<T, M>& lhs, nullptr_t) { return (lhs.get() != nullptr); }

    //-----------------
    template<typename T, typename M>
    bool operator<(const SharedPointer<T, M>& lhs, const SharedPointer<T, M>& rhs) { return (lhs.get() < rhs.get()); }
    template<typename T, typename M>
    bool operator<(nullptr_t, const SharedPointer<T, M>& rhs) { return (lhs.get() < nullptr); }
    template<typename T, typename M>
    bool operator<(const SharedPointer<T, M>& lhs, nullptr_t) { return (lhs.get() < nullptr); }

    //-----------------
    template<typename T, typename M>
    bool operator>(const SharedPointer<T, M>& lhs, const SharedPointer<T, M>& rhs) { return (lhs.get() > rhs.get()); }
    template<typename T, typename M>
    bool operator>(nullptr_t, const SharedPointer<T, M>& rhs) { return (lhs.get() > nullptr); }
    template<typename T, typename M>
    bool operator>(const SharedPointer<T, M>& lhs, nullptr_t) { return (lhs.get() > nullptr); }

    //-----------------
    template<typename T, typename M>
    bool operator>=(const SharedPointer<T, M>& lhs, const SharedPointer<T, M>& rhs) { return (lhs.get() >= rhs.get()); }
    template<typename T, typename M>
    bool operator>=(nullptr_t, const SharedPointer<T, M>& rhs) { return (lhs.get() >= nullptr); }
    template<typename T, typename M>
    bool operator>=(const SharedPointer<T, M>& lhs, nullptr_t) { return (lhs.get() >= nullptr); }

    //-----------------
    template<typename T, typename M>
    bool operator<=(const SharedPointer<T, M>& lhs, const SharedPointer<T, M>& rhs) { return (lhs.get() <= rhs.get()); }
    template<typename T, typename M>
    bool operator<=(nullptr_t, const SharedPointer<T, M>& rhs) { return (lhs.get() <= nullptr); }
    template<typename T, typename M>
    bool operator<=(const SharedPointer<T, M>& lhs, nullptr_t) { return (lhs.get() <= nullptr); }

    //-----------------
    template<typename charT, typename traits, typename T, typename M>
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const SharedPointer<T, M>& rhs)
    {
        return (os << rhs.get());
    }

} //namespace ldl
