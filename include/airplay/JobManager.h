#ifndef _CXL_YETI_JOBMANAGER_H_
#define _CXL_YETI_JOBMANAGER_H_

#include <queue>
#include <vector>
#include <string>
#include "airplay/Lock.h"
#include "airplay/Job.h"

class JobManager;

class JobWorker
{
public:
    JobWorker(JobManager *manager);
    virtual ~JobWorker();

    void process();

private:
    JobManager  *m_jobManager;
};

class JobQueue: public IJobCallback
{
    class JobPointer
    {
    public:
        JobPointer(Job *job) {
            m_job = job;
            m_id = 0;
        };
        void CancelJob();
        void FreeJob()
        {
            delete m_job;
            m_job = NULL;
        };
        bool operator==(const Job *job) const
        {
            if (m_job)
                return *m_job == job;
            return false;
        };
        Job *m_job;
        unsigned int m_id;
    };
public:

    JobQueue(bool lifo = false, unsigned int jobsAtOnce = 1, Job::PRIORITY priority = Job::PRIORITY_LOW);

    virtual ~JobQueue();

    void AddJob(Job *job);

    void CancelJobs();

    virtual void OnJobComplete(unsigned int jobID, bool success, Job *job);

private:
    void QueueNextJob();

    typedef std::deque<JobPointer> Queue;
    typedef std::vector<JobPointer> Processing;
    Queue m_jobQueue;
    Processing m_processing;

    unsigned int m_jobsAtOnce;
    Job::PRIORITY m_priority;
    CriticalSection m_section;
    bool m_lifo;
};

class JobManager
{
    class WorkItem
    {
    public:
        WorkItem(Job *job, unsigned int id, IJobCallback *callback)
        {
            m_job = job;
            m_id = id;
            m_callback = callback;
        }
        bool operator==(unsigned int jobID) const
        {
            return m_id == jobID;
        };
        bool operator==(const Job *job) const
        {
            return m_job == job;
        };
        void FreeJob()
        {
            delete m_job;
            m_job = NULL;
        };
        void Cancel()
        {
            m_callback = NULL;
        };

        Job         *m_job;
        unsigned int  m_id;
        IJobCallback *m_callback;
    };

public:
    static JobManager & get_instance();

    unsigned int add_job(Job *job, IJobCallback *callback, Job::PRIORITY priority = Job::PRIORITY_LOW);

    void cancel_job(unsigned int jobID);

    void cancel_jobs();

protected:
    friend class JobWorker;
    friend class Job;

    Job * get_next_job(const JobWorker *worker);

    void  on_job_complete(bool success, Job *job);

    bool  on_job_progress(unsigned int progress, unsigned int total, const Job *job) const;

private:
    JobManager();
    JobManager(const JobManager&);
    JobManager const& operator=(JobManager const&);
    virtual ~JobManager();

    Job * pop_job();

    void start_workers(Job::PRIORITY priority);
    void remove_worker(const JobWorker *worker);
    unsigned int get_max_workers(Job::PRIORITY priority) const;

    unsigned int m_jobCounter;

    typedef std::deque<WorkItem>    JobQueue;
    typedef std::vector<WorkItem>   Processing;
    typedef std::vector<JobWorker*> Workers;

    JobQueue   m_jobQueue[Job::PRIORITY_HIGH+1];
    Processing m_processing;
    Workers    m_workers;

    CriticalSection  m_section;
    CEvent           m_jobEvent;
    bool             m_running;
};

#endif // _CXL_YETI_JOBMANAGER_H_
