#pragma once
#ifndef POOLED_MUTEX_H_
#define POOLED_MUTEX_H_

#include "pooled_new.h"

#include <mutex>
namespace c11 {
    using namespace std;
}

namespace ldl {

    class PooledMutex : public PooledNew<PooledMutex> {
    public:
        typedef c11::mutex::native_handle_type native_handle_type;
        PooledMutex() {}
        void lock() { mutex_.lock(); }
        bool try_lock() { return mutex_.try_lock(); }
        void unlock() { mutex_.unlock(); }
        native_handle_type native_handle() { return mutex_.native_handle(); }
    private:
        PooledMutex(const PooledMutex&); //= delete;
        c11::mutex mutex_;
    };

} //namespace ldl

#endif //! POOLED_MUTEX_
