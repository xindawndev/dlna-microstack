#ifndef _CXL_YETI_JOB_H_
#define _CXL_YETI_JOB_H_

class Job;

class IJobCallback
{
public:
  virtual ~IJobCallback() {};

  virtual void on_job_complete(unsigned int jobID, bool success, Job *job)=0;
  virtual void on_job_progress(unsigned int jobID, unsigned int progress, unsigned int total, const Job *job) {};
};

class JobManager;

class Job
{
public:
  enum PRIORITY {
    PRIORITY_LOW = 0,
    PRIORITY_NORMAL,
    PRIORITY_HIGH
  };
  Job() { m_callback = NULL; };

  virtual ~Job() {};

  virtual bool do_work() = 0;  // function to do the work

  virtual const char * get_type() const { return ""; };

  virtual bool operator==(const Job* job) const
  {
    return false;
  }

  bool should_cancel(unsigned int progress, unsigned int total) const;
private:
  friend class JobManager;
  JobManager *m_callback;
};

#endif // _CXL_YETI_JOB_H_
