#include "shared_pointer.h"

namespace ldl {

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer()
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
        , delete_func_(0)
    {
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(U* ptr)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
        , delete_func_(0)
    {
        reset(ptr);
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(nullptr_t)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
        , delete_func_(0)
    {
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(U* ptr, DeleteFunction delete_func)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
        , delete_func_(0)
    {
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(nullptr_t, DeleteFunction delete_func)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
        , delete_func_(0)
    {
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::SharedPointer(SharedPointer& other)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
        , delete_func_(0)
    {
        other.AddToList(*this);
    }

    //-----------------
    template<typename T>
    template<typename U>
    SharedPointer<T>::SharedPointer(SharedPointer<U>& other)
        : mutex_ptr_(0)
        , prev_ptr_(0)
        , next_ptr_(0)
        , obj_ptr_(0)
        , delete_func_(0)
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
        if (this != &other && obj_ptr_ != other.obj_ptr_) {
            if (mutex_ptr_) {
                ListLock lock(*mutex_ptr_); // lock current list during swap
                SharedPointer tmp;
                // tmp = *this;
                // *this = 0;
                std::swap(tmp.mutex_ptr_, mutex_ptr_);
                std::swap(tmp.prev_ptr_, prev_ptr_);
                std::swap(tmp.next_ptr_, next_ptr_);
                std::swap(tmp.obj_ptr_, obj_ptr_);
                std::swap(tmp.delete_func_, delete_func_);
                if (tmp.prev_ptr_) {
                    tmp.prev_ptr_->next_ptr_ = &tmp;
                }
                if (tmp.next_ptr_) {
                    tmp.next_ptr_->prev_ptr_ = &tmp;
                }
                //*this = other;
                // other = 0;
                other.AddToList(*this, true);
                // other = tmp;
                // tmp = 0;
                std::swap(tmp.mutex_ptr_, other.mutex_ptr_);
                std::swap(tmp.prev_ptr_, other.prev_ptr_);
                std::swap(tmp.next_ptr_, other.next_ptr_);
                std::swap(tmp.obj_ptr_, other.obj_ptr_);
                std::swap(tmp.delete_func_, other.delete_func_);
                if (other.prev_ptr_) {
                    other.prev_ptr_->next_ptr_ = &other;
                }
                if (other.next_ptr_) {
                    other.next_ptr_->prev_ptr_ = &other;
                }
            }
            else { // *this == 0
                // *this = other
                // other = 0;
                other.AddToList(*this, true);
                other.reset();
            }
        }
    }

    //-----------------
    template<typename T>
    void SharedPointer<T>::reset()
    {
        if (mutex_ptr_) { // if in a list
            ListLock lock(*mutex_ptr_);
            if (obj_ptr_ && !prev_ptr_ && !next_ptr_) {
                if (delete_func_) {
                    delete_func_(obj_ptr_);
                    delete_func_ = 0;
                }
                else {
                    delete obj_ptr_;
                }
                obj_ptr_ = 0;
            }
            // if necessary, update *prev_ptr_
            if (prev_ptr_) {
                prev_ptr_->next_ptr_ = next_ptr_;
            }
            // if necessary, update *next_ptr_
            if (next_ptr_) {
                next_ptr_->prev_ptr_ = prev_ptr_;
            }
            prev_ptr_ = 0;
            next_ptr_ = 0;
        } // unlock mutex before deleting it
        // if obj_ptr_ was deleted, also delete mutex_ptr_
        if (mutex_ptr_ && !obj_ptr_) {
            delete mutex_ptr_;
            mutex_ptr_ = 0;
        }
    }

    //-----------------
    template<typename T>
    template<typename U>
    void SharedPointer<T>::reset(U* ptr)
    {
        if (ptr) {
            reset();
            mutex_ptr_ = new PooledMutex();
            obj_ptr_ = ptr;
        }
    }

    //-----------------
    template<typename T>
    template<typename U>
    void SharedPointer<T>::reset(U* ptr, DeleteFunction delete_func)
    {
        if (ptr) {
            reset();
            mutex_ptr_ = new PooledMutex();
            obj_ptr_ = ptr;
            delete_func_ = delete_func;
        }
    }

    //-----------------
    template<typename T>
    T* SharedPointer<T>::get() const
    {
        return  obj_ptr_;
    }

    //-----------------
    template<typename T>
    T& SharedPointer<T>::operator*() const
    {
        return *obj_ptr_;
    }

    //-----------------
    template<typename T>
    T* SharedPointer<T>::operator->() const
    {
        return obj_ptr_;
    }

    //-----------------
    template<typename T>
    SharedPointer<T>::operator bool() const
    {
        return (obj_ptr_ != 0);
    }

    //-----------------
    template<typename T>
    long int SharedPointer<T>::use_count()
    {
        long int retval = 0;
        if (mutex_ptr_) {
            ListLock lock(*mutex_ptr_);
            for (SharedPointer* ptr = this; ptr != nullptr; ptr = ptr->prev_ptr_) {
                ++retval;
            }
            for (SharedPointer* ptr = next_ptr_; ptr != nullptr; ptr = ptr->next_ptr_) {
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
    void SharedPointer<T>::AddToList(SharedPointer& other, bool replace)
    {
        if (other.mutex_ptr_) {
            throw std::runtime_error("argument is already in a list");
        }
        if (mutex_ptr_) { // if *this is in a list
            ListLock lock(*mutex_ptr_);
            // insert other after *this
            other.obj_ptr_ = obj_ptr_;
            other.delete_func_ = delete_func_;
            other.mutex_ptr_ = mutex_ptr_;
            other.next_ptr_ = next_ptr_;
            other.prev_ptr_ = this;
            next_ptr_ = &other;
            if (replace) {
                // while still locked, reset *this
                if (next_ptr_) {
                    next_ptr_->prev_ptr_ = prev_ptr_;
                }
                if (prev_ptr_) {
                    prev_ptr_->next_ptr_ = next_ptr_;
                }
                mutex_ptr_ = 0;
                next_ptr_ = 0;
                prev_ptr_ = 0;
                obj_ptr_ = 0;
                delete_func_ = 0;
            }
        }
    }

    //-----------------
    template<typename T>
    const size_t SharedPointer<T>::element_size_ = sizeof(SharedPointer<T>);

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
