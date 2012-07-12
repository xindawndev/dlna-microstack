#ifndef _CXL_YETI_LOCK_H_
#define _CXL_YETI_LOCK_H_

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

class CriticalSection
{
public:
    CriticalSection() {
#if defined(_WIN32)
        InitializeCriticalSection(&m_cs_);
#else
        pthread_mutex_init(&m_cs_, NULL);
#endif
    }

    ~CriticalSection() {
#if defined(_WIN32)
        DeleteCriticalSection(&m_cs_);
#else
        pthread_mutex_destroy(&m_cs_);
#endif
    }

    void lock() {
#if defined(_WIN32)
        EnterCriticalSection(&m_cs_);
#else
        pthread_mutex_lock(&m_cs_);
#endif
    }
    
    void unlock() {
#if defined(_WIN32)
        LeaveCriticalSection(&m_cs_);
#else
        pthread_mutex_unlock(&m_cs_);
#endif
    }
private:
#if defined(_WIN32)
    CRITICAL_SECTION m_cs_;
#else
    pthread_mutex_t m_cs_;
#endif
};

class AutoLock : public CriticalSection
{
public:
    AutoLock() {
        lock();
    }

    ~AutoLock() {
        unlock();
    }
};

class SingleLock
{
public:
    SingleLock(CriticalSection & sc)
        : m_sc_(sc)
    {
        m_sc_.lock();
    }

    ~SingleLock() {
        m_sc_.unlock();
    }

private:
    CriticalSection & m_sc_;
};
#endif // _CXL_YETI_LOCK_H_
