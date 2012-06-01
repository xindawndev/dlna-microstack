#include "airplay/Zeroconf.h"
#include "airplay/JobManager.h"

#ifdef _LINUX
#ifndef __APPLE__
#include "linux/ZeroconfAvahi.h"
#else
#include "osx/ZeroconfOSX.h"
#endif
#elif defined(_WIN32)
#include "airplay/ZeroconfWin.h"
#endif

#include "airplay/Lock.h"

#ifndef HAS_ZEROCONF
class CZeroconfDummy : public Zeroconf
{
    virtual bool do_publish_service(const std::string&, const std::string&, const std::string&, unsigned int, std::map<std::string, std::string>)
    {
        return false;
    }
    virtual bool do_remove_service(const std::string& fcr_ident){return false;}
    virtual void do_stop(){}
};
#endif

long Zeroconf::sm_singleton_guard = 0;
Zeroconf * Zeroconf::smp_instance = 0;

Zeroconf::Zeroconf():mp_crit_sec(new CriticalSection),m_started(false)
{
}

Zeroconf::~Zeroconf()
{
    delete mp_crit_sec;
}

bool Zeroconf::publish_service(const std::string& fcr_identifier,
                              const std::string& fcr_type,
                              const std::string& fcr_name,
                              unsigned int f_port,
                              std::map<std::string, std::string> txt)
{
    SingleLock lock(*mp_crit_sec);
    Zeroconf::PublishInfo info = {fcr_type, fcr_name, f_port, txt};
    std::pair<tServiceMap::const_iterator, bool> ret = m_service_map.insert(std::make_pair(fcr_identifier, info));
    if(!ret.second) // identifier exists
        return false;
    if(m_started)
        JobManager::get_instance().add_job(new Publish(fcr_identifier, info), NULL);

    return true;
}

bool Zeroconf::remove_service(const std::string& fcr_identifier)
{
    SingleLock lock(*mp_crit_sec);

    tServiceMap::iterator it = m_service_map.find(fcr_identifier);
    if(it == m_service_map.end())
        return false;
    m_service_map.erase(it);
    if(m_started)
        return do_remove_service(fcr_identifier);
    else
        return true;
}

bool Zeroconf::has_service(const std::string& fcr_identifier) const
{
    return (m_service_map.find(fcr_identifier) != m_service_map.end());
}

void Zeroconf::start()
{
    SingleLock lock(*mp_crit_sec);
    if(!is_zc_daemon_running())
    {
        //g_guiSettings.SetBool("services.zeroconf", false);
        //if (g_guiSettings.GetBool("services.airplay"))
        //    g_guiSettings.SetBool("services.airplay", false);
        return;
    }
    if(m_started)
        return;
    m_started = true;

    JobManager::get_instance().add_job(new Publish(m_service_map), NULL);
}

void Zeroconf::stop()
{
    SingleLock lock(*mp_crit_sec);
    if(!m_started)
        return;
    do_stop();
    m_started = false;
}

Zeroconf*  Zeroconf::get_instance()
{
    CAtomicSpinLock lock(sm_singleton_guard);
    if(!smp_instance)
    {
#ifndef HAS_ZEROCONF
        smp_instance = new CZeroconfDummy;
#else
#ifdef __APPLE__
        smp_instance = new CZeroconfOSX;
#elif defined(_LINUX)
        smp_instance  = new CZeroconfAvahi;
#elif defined(_WIN32)
        smp_instance  = new CZeroconfWIN;
#endif
#endif
    }
    assert(smp_instance);
    return smp_instance;
}

void Zeroconf::release_instance()
{
    CAtomicSpinLock lock(sm_singleton_guard);
    delete smp_instance;
    smp_instance = 0;
}

Zeroconf::Publish::Publish(const std::string& fcr_identifier, const PublishInfo& pubinfo)
{
    m_servmap.insert(std::make_pair(fcr_identifier, pubinfo));
}

Zeroconf::Publish::Publish(const tServiceMap& servmap) 
: m_servmap(servmap)
{
}

bool Zeroconf::Publish::do_work()
{
    for(tServiceMap::const_iterator it = m_servmap.begin(); it != m_servmap.end(); ++it)
        Zeroconf::get_instance()->do_publish_service(it->first, it->second.type, it->second.name, it->second.port, it->second.txt);

    return true;
}