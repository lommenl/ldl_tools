#include "future.h"

#include <exception>
#include <algorithm> //swap
#include <mutex> //unique_lock

namespace ldl {

    //---------------
    FutureState::FutureState()
        : promise_error(false)
        , value_ptr(0)
    {
    }

    //---------------
    FutureBase::FutureBase()
        : state_ptr_(0)
    {
    }

    //---------------
    template<typename T>
    Future<T>::Future()
        : FutureBase()
    {
    }

    //---------------
    template<typename T>
    Future<T>::Future(Future& other)
        : FutureBase()
    {
        swap(other); // move ShredPointer
    }

    //---------------
    template<typename T>
    Future<T>::~Future()
    {
        reset();
    }

    //---------------
    template<typename T>
    Future<T>& Future<T>::operator=(Future& other)
    {
        if (this != &other) {
            reset();
            swap(other); // move SharedPointer
        }
        return *this;
    }

    //---------------
    template<typename T>
    void Future<T>::swap(Future& other) {
        if (this != &other) {
            state_ptr_.swap(other.state_ptr_); // swap SharedPointer
        }
    }

    //---------------
    template<typename T>
    void Future<T>::reset()
    {
        // if necessary, destroy state_ptr_->value_ptr
        if (state_ptr_ && state_ptr_->value_ptr) {
            delete static_cast<T*>(state_ptr_->value_ptr);
            state_ptr_->value_ptr = 0;
        }
        state_ptr_.reset();
    }

    //---------------
    template<typename T>
    bool Future<T>::valid() const {
        return  (bool)state_ptr_;
    }

    //---------------
    template<typename T>
    T Future<T>::get()
    {
        if (!state_ptr_) {
            throw std::runtime_error("Future is not valid");
        }
        wait(); // block until promise notifies future
        if (state_ptr_->promise_error) { // if woken because Promise was destroyed
            throw std::runtime_error("promise was destroyed before setting value,");
        }
        return *static_cast<T*>(state_ptr_->value_ptr);
    }

    //---------------
    template<typename T>
    void Future<T>::wait()
    {
        if (!state_ptr_) {
            throw std::runtime_error("future is not valid");
        }
        c11::unique_lock<c11::mutex> lock(state_ptr_->mutex);
        while (!state_ptr_->value_ptr) { //ignore spurious wake-ups
            state_ptr_->cv.wait(lock);
        }
    }

    //---------------
    template<typename T>
    template<typename Clock, typename Duration>
    FutureStatus::type Future<T>::wait_until(const c11::chrono::time_point<Clock, Duration >& abs_time)
    {
        FutureStatus::type retval = FutureStatus::ready;
        if (!state_ptr_) {
            throw std::runtime_error("future is not valid");
        }
        c11::unique_lock<c11::mutex> lock(state_ptr_->mutex);
        while (!state_ptr_->value_ptr) { //ignore spurious wake-ups
            if (state_ptr_->cv.wait_until(lock, abs_time) == c11::cv_status::timeout) {
                retval = FutureStatus::timeout;
                break; //stop waiting
            }
        }
        return retval;
    }

    //---------------
    template<typename T>
    template< typename Rep, typename Period>
    FutureStatus::type Future<T>::wait_for(const c11::chrono::duration<Rep, Period>& rel_time)
    {
        return wait_until(c11::chrono::steady_clock::now() + rel_time);
    }

    //---------------
    template<typename T>
    Future<T>::Future(SharedPointer<FutureState>& state_ptr)
        : FutureBase()
    {
        state_ptr_ = state_ptr; // copy sharedPointer
    }

    //==========================

    //---------------
    PromiseBase::PromiseBase()
        : future_constructed_(false)
    {
    }

    //---------------
    template<typename T>
    Promise<T>::Promise()
        : PromiseBase()
    {
    }
    //---------------
    template<typename T>
    Promise<T>::Promise(Promise& other)
        : PromiseBase()
    {
        swap(other); // move SharedPointer
    }

    //---------------
    template<typename T>
    Promise<T>::~Promise()
    {
        reset();
    }

    //---------------
    template<typename T>
    Promise<T>& Promise<T>::operator=(Promise& other)
    {
        if (this != &other) {
            reset();
            swap(other); // move SharedPointer
        }
        return *this;
    }

    //---------------
    template<typename T>
    void Promise<T>::swap(Promise& other)
    {
        if (this != &other) {
            state_ptr_.swap(other.state_ptr_);
            std::swap(future_constructed_, other.future_constructed_);
        }
    }

    //---------------
    template<typename T>
    void Promise<T>::reset()
    {
        if (state_ptr_) {
            c11::lock_guard<c11::mutex> lock(state_ptr_->mutex);
            state_ptr_->promise_error = true;
        } // unlock before calling set_value
        // if necessary, wake promise
        if (future_constructed_ && state_ptr_ && !state_ptr_->value_ptr) {
            set_value(T());
        }
        state_ptr_.reset();
        future_constructed_ = false;
    }

    //---------------
    template<typename T>
    Future<T> Promise<T>::get_future()
    {
        if (future_constructed_) {
            throw std::runtime_error("Future already constructed");
        }
        if (!state_ptr_) {
            state_ptr_.reset(new FutureState());
        }
        return Future<T>(state_ptr_); // copy SharedPointer to future
    }

    //---------------
    template<typename T>
    void Promise<T>::set_value(const T& value) {
        if (!state_ptr_) {
            state_ptr_.reset(new FutureState());
        }
        c11::lock_guard<c11::mutex> lock(state_ptr_->mutex);
        if (state_ptr_->value_ptr) {
            throw std::runtime_error("value already set");
        }
        state_ptr_->value_ptr = new T(value);
        state_ptr_->cv.notify_one();
    }

} //namespace ldl
