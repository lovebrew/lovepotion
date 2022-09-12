#ifndef THREAD3DS_INC
#define THREAD3DS_INC

/*
 * Public domain C++ <thread>-ish implementation for libctru
 * VER 20220902060500
 */

#include <3ds.h>
#include <utility>
#include <tuple>
#include <atomic>
#include <memory>
#include <chrono>
#include <bits/unique_lock.h>

namespace ctr {

struct thread_id {
    friend class thread;
    friend class this_thread;

    thread_id() = default;

    bool operator==(thread_id rhs ) const noexcept
    {
        return m_id == rhs.m_id;
    }
    bool operator!=(thread_id rhs ) const noexcept
    {
        return m_id != rhs.m_id;
    }
    
    bool operator<(thread_id rhs ) const noexcept
    {
        return m_id < rhs.m_id;
    }
    bool operator<=(thread_id rhs ) const noexcept
    {
        return m_id <= rhs.m_id;
    }
    bool operator>(thread_id rhs ) const noexcept
    {
        return m_id > rhs.m_id;
    }
    bool operator>=(thread_id rhs ) const noexcept
    {
        return m_id >= rhs.m_id;
    }

private:
    Thread m_id{};
};

struct thread_base {
    template<class Rep, class Period>
    static void sleep_for(const std::chrono::duration<Rep, Period>& length)
    {
        svcSleepThread(std::chrono::duration_cast<std::chrono::nanoseconds>(length).count());
    }
};
struct this_thread : public thread_base {
    using id = thread_id;

    static thread_id get_id()
    {
        thread_id out;
        out.m_id = native_handle();
        return out;
    }
    static Thread native_handle()
    {
        return threadGetCurrent();
    }
    static void yield() noexcept
    {
        svcSleepThread(1);
    }
};

class thread {
public:
    struct meta {
        std::size_t stack_size;
        int prio;
        int core_id;
    };

private:
    Thread m_th{};

    template<class P_t>
    static void trampoline(void* v_arg)
    {
        auto const thread_arg = std::unique_ptr<P_t>(static_cast<P_t*>(v_arg));
        std::apply(thread_arg->first, std::forward<typename P_t::second_type>(thread_arg->second));
    }

public:
    using id = thread_id;

    static constexpr inline meta basic_meta{
        64 * 1024,
        0x30,
        0,
    };

    template<class F, typename... Args>
    thread(const meta& info, F&& func, Args&&... args)
    {
        static_assert(std::is_invocable_v<F, std::unwrap_reference_t<Args>...>, "Can not call this function with these argument.");
        using P_t = std::pair<F&&, std::tuple<std::unwrap_reference_t<Args>...>>;
        auto arg = std::make_unique<P_t>(
            std::forward<F>(func),
            std::forward_as_tuple(std::forward<Args>(args)...)
        );
        m_th = threadCreate(&thread::trampoline<P_t>, arg.get(), info.stack_size, info.prio, info.core_id, false);
        if(m_th)
        {
            svcSleepThread(1000 * 1000);
            arg.release();
        }
    }

    template<class F, typename... Args, typename = std::enable_if_t<!std::is_same_v<meta, std::remove_cvref_t<F>>>>
    thread(F&& func, Args&&... args)
        : thread(basic_meta, std::forward<F>(func), std::forward<Args>(args)...)
    { }

    thread() = default;
    thread(thread&& other)
        : m_th(std::exchange(other.m_th, nullptr))
    {

    }
    thread(const thread&) = delete;

    thread& operator=(thread&& other)
    {
        if(this != &other)
        {
            if(joinable()) throw std::runtime_error("Moving to a thread already running.");
            m_th = std::exchange(other.m_th, nullptr);
        }
        return *this;
    }
    thread& operator=(const thread&) = delete;

    ~thread()
    {
        if(m_th)
        {
            svcBreak(USERBREAK_ASSERT);
        }
    }

    operator bool() const
    {
        return m_th != nullptr;
    }

    bool joinable() const noexcept
    {
        return m_th != threadGetCurrent();
    }

    thread_id get_id()
    {
        thread_id out;
        out.m_id = m_th;
        return out;
    }
    Thread native_handle() const
    {
        return m_th;
    }
    void join_timeout(const u64 timeout)
    {
        if(R_SUCCEEDED(threadJoin(m_th, timeout)))
        {
            threadFree(m_th);
            m_th = nullptr;
        }
    }
    
    void join()
    {
        join_timeout(U64_MAX);
    }
    void swap(thread& other) noexcept
    {
        std::swap(m_th, other.m_th);
    }

    static unsigned int hardware_concurrency() noexcept
    {
        return 1;
    }
};

class mutex {
    LightLock m_lock;

public:
    mutex() noexcept
    {
        LightLock_Init(&m_lock);
    }
    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;
    mutex(mutex&&) = delete;
    mutex& operator=(mutex&&) = delete;

    void lock() noexcept
    {
        LightLock_Lock(&m_lock);
    }
    bool try_lock() noexcept
    {
        return LightLock_TryLock(&m_lock) == 0;
    }
    void unlock() noexcept
    {
        LightLock_Unlock(&m_lock);
    }

    LightLock* native_handle()
    {
        return &m_lock;
    }
};

class recursive_mutex {
    RecursiveLock m_lock;

public:
    recursive_mutex() noexcept
    {
        RecursiveLock_Init(&m_lock);
    }
    recursive_mutex(const recursive_mutex&) = delete;
    recursive_mutex& operator=(const recursive_mutex&) = delete;
    recursive_mutex(recursive_mutex&&) = delete;
    recursive_mutex& operator=(recursive_mutex&&) = delete;

    void lock() noexcept
    {
        RecursiveLock_Lock(&m_lock);
    }
    bool try_lock() noexcept
    {
        return RecursiveLock_TryLock(&m_lock) == 0;
    }
    void unlock() noexcept
    {
        RecursiveLock_Unlock(&m_lock);
    }

    RecursiveLock* native_handle()
    {
        return &m_lock;
    }
};

enum class cv_status { no_timeout, timeout };

class condition_variable {
    CondVar m_cv;

public:
    condition_variable()
    {
        CondVar_Init(&m_cv);
    }
    condition_variable(const condition_variable&) = delete;

    void notify_one() noexcept
    {
        CondVar_Signal(&m_cv);
    }
    void notify_all() noexcept
    {
        CondVar_Broadcast(&m_cv);
    }

    void wait(std::unique_lock<mutex>& lock)
    {
        CondVar_Wait(&m_cv, lock.mutex()->native_handle());
    }
    template< class Predicate >
    void wait( std::unique_lock<mutex>& lock, Predicate stop_waiting)
    {
        while (!stop_waiting()) {
            wait(lock);
        }
    }

    template<class Rep, class Period>
    cv_status wait_for(std::unique_lock<mutex>& lock, const std::chrono::duration<Rep, Period>& rel_time)
    {
        const int r = CondVar_WaitTimeout(&m_cv, lock.mutex()->native_handle(), std::chrono::duration_cast<std::chrono::nanoseconds>(rel_time).count());
        return r == 0 ? cv_status::no_timeout : cv_status::timeout;
    }

    template< class Rep, class Period, class Predicate >
    bool wait_for( std::unique_lock<mutex>& lock,
                const std::chrono::duration<Rep, Period>& rel_time,
                Predicate stop_waiting)
    {
        return wait_until(lock, std::chrono::steady_clock::now() + rel_time, std::move(stop_waiting));
    }

    template<class Rep, class Period>
    cv_status wait_until(std::unique_lock<mutex>& lock, const std::chrono::duration<Rep, Period>& abs_time)
    {
        return wait_for(lock, abs_time - std::chrono::steady_clock::now());
    }

    template< class Clock, class Duration, class Predicate >
    bool wait_until( std::unique_lock<mutex>& lock,
                    const std::chrono::time_point<Clock, Duration>& timeout_time,
                    Predicate stop_waiting )
    {
        while (!stop_waiting())
        {
            if (wait_until(lock, timeout_time) == cv_status::timeout)
            {
                return stop_waiting();
            }
        }
        return true;
    }

    CondVar* native_handle()
    {
        return &m_cv;
    }
};

}

#endif
