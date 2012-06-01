#ifndef _CXL_YETI_LOCK_H_
#define _CXL_YETI_LOCK_H_

#include <windows.h>

class CriticalSection
{
public:
    CriticalSection() {
        InitializeCriticalSection(&m_cs_);
    }

    ~CriticalSection() {
        DeleteCriticalSection(&m_cs_);
    }

    void lock() {
        EnterCriticalSection(&m_cs_);
    }
    
    void unlock() {
        LeaveCriticalSection(&m_cs_);
    }
private:
    CRITICAL_SECTION m_cs_;
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
