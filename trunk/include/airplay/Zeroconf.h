#ifndef _CXL_YETI_ZEROCONF_H_
#define _CXL_YETI_ZEROCONF_H_

#include <string>
#include <map>
#include "airplay/Job.h"

class CriticalSection;
class Zeroconf
{
public:
    bool publish_service(const std::string& fcr_identifier,
        const std::string& fcr_type,
        const std::string& fcr_name,
        unsigned int f_port,
        std::map<std::string, std::string> txt);

    bool remove_service(const std::string& fcr_identifier);

    bool has_service(const std::string& fcr_identifier) const;

    void start();

    void stop();

    static Zeroconf * get_instance();
    static void   release_instance();
    static bool   is_instantiated() { return  smp_instance != 0; }

protected:
    virtual bool do_publish_service(const std::string& fcr_identifier,
        const std::string& fcr_type,
        const std::string& fcr_name,
        unsigned int f_port,
        std::map<std::string, std::string> txt) = 0;
    virtual bool do_remove_service(const std::string& fcr_ident) = 0;

    virtual void do_stop() = 0;

    virtual bool is_zc_daemon_running() { return  true; }

protected:
    Zeroconf();
    Zeroconf(const Zeroconf&);
    virtual ~Zeroconf();

private:
    struct PublishInfo{
        std::string type;
        std::string name;
        unsigned int port;
        std::map<std::string, std::string> txt;
    };

    CriticalSection* mp_crit_sec;
    typedef std::map<std::string, PublishInfo> tServiceMap;
    tServiceMap m_service_map;
    bool m_started;

    static long sm_singleton_guard;
    static Zeroconf* smp_instance;

    class Publish : public Job
    {
    public:
        Publish(const std::string& fcr_identifier, const PublishInfo& pubinfo);
        Publish(const tServiceMap& servmap);

        bool do_work();

    private:
        tServiceMap m_servmap;
    };
};

#endif
