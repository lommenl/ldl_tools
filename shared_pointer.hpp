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
    template<typename U>
    SharedPointer<T>::SharedPointer(U* ptr)
        : mutex_ptr_(new PooledMutex)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(static_cast<T*>(ptr))
    {
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(nullptr_t)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
    {
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(U* ptr, Deleter deleter)
        : mutex_ptr_(new PooledMutex)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(static_cast<T*>(ptr))
        , deleter_(deleter)
    {
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(nullptr_t, Deleter deleter)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
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
        // if not the same object, or managing the same object
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            // remove *this from its current list.
            reset();
            if (other.mutex_ptr_) {
                // insert *this after other
                ListLock lock(*other.mutex_ptr_);
                obj_ptr_ = static_cast<T*>(other.obj_ptr_);
                deleter_ = other.deleter_;
                mutex_ptr_ = other.mutex_ptr_;
                prev_ptr_ = &other;
                next_ptr_ = other.next_ptr_;
                other.next_ptr_ = this;
            }
        }
        return *this;
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>& SharedPointer<T>::operator=(SharedPointer<U>& other)
    {
        // if not the same object, or managing the same object
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            // remove *this from its current list
            reset();
            if (other.mutex_ptr_) {
                // insert *this after other
                ListLock lock(*other.mutex_ptr_);
                mutex_ptr_ = other.mutex_ptr_;
                obj_ptr_ = static_cast<T*>(other.obj_ptr_);
                if (other.deleter_) {  // if a deleter was specified for other.
                    deleter_ = CastDeleter<U>(other.deleter_);
                }
                prev_ptr_ = &other;
                next_ptr_ = other.next_ptr_;
                other.next_ptr_ = this;
            }
        }
        return *this;
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::swap(SharedPointer& other)
    {
        // if not the same object, or managing the same object
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            // lock both this objects list and others list
            if (mutex_ptr_ && !other.mutex_ptr_) {
                ListLock lock(*mutex_ptr_);
                std::swap(mutex_ptr_, other.mutex_ptr_);
                std::swap(prev_ptr_, other.prev_ptr_);
                std::swap(next_ptr_, other.next_ptr_);
                std::swap(obj_ptr_, other.obj_ptr_);
                std::swap(deleter_, other.deleter_);
            }
            else if (!mutex_ptr_ && other.mutex_ptr_) {
                ListLock lock(*other.mutex_ptr_);
                std::swap(mutex_ptr_, other.mutex_ptr_);
                std::swap(prev_ptr_, other.prev_ptr_);
                std::swap(next_ptr_, other.next_ptr_);
                std::swap(obj_ptr_, other.obj_ptr_);
                std::swap(deleter_, other.deleter_);
            }
            else if (mutex_ptr_ && other.mutex_ptr_) {
                ListLock lock(*mutex_ptr_);
                ListLock lock2(*other.mutex_ptr_);
                std::swap(mutex_ptr_, other.mutex_ptr_);
                std::swap(prev_ptr_, other.prev_ptr_);
                std::swap(next_ptr_, other.next_ptr_);
                std::swap(obj_ptr_, other.obj_ptr_);
                std::swap(deleter_, other.deleter_);
            }
        }
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::reset()
    {
        if ( mutex_ptr_ ) {
            ListLock lock(*mutex_ptr_);
            // if this is the only object in the list, delete obj_ptr_
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
        } // unlock mutex before deleting mutex_ptr_
        if (mutex_ptr_) {
            delete mutex_ptr_;
            mutex_ptr_ = 0;
        }
    }

    //-----------------
    template<typename T>
    template<typename U>
    void SharedPointer<T>::reset(U* ptr)
    {
        reset();
        swap(SharedPointer(ptr));
    }

    //-----------------
    template<typename T>
    template<typename U>
    void SharedPointer<T>::reset(U* ptr, Deleter del)
    {
        reset();
        swap(SharedPointer(ptr, del));
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
        if (mutex_ptr_) {
            ListLock lock(*mutex_ptr_);
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
        bool retval = false;
        if (mutex_ptr_) {
            ListLock lock(*mutex_ptr_);
            retval = (prev_ptr_ == 0 && next_ptr_ == 0);
        } 
        return retval;
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::operator bool() const
    {
        return (obj_ptr_ != 0);
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
