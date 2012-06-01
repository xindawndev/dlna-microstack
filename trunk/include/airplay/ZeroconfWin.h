#include <memory>

//#include "airplay/Zeroconf.h"
#include "airplay/Lock.h"
#include <dns_sd.h>
#include <string>
#include <map>

class ZeroconfWin //: public Zeroconf
{
public:
    ZeroconfWin();
    ~ZeroconfWin();

protected:
public:
    bool do_publish_service(const std::string& fcr_identifier,
        const std::string& fcr_type,
        const std::string& fcr_name,
        unsigned int f_port,
        std::map<std::string, std::string> txt);

    bool do_remove_service(const std::string& fcr_ident);

    virtual void do_stop();

    bool is_zc_daemon_running();

private:
    static void DNSSD_API registerCallback(DNSServiceRef sdref, const DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context);

    CriticalSection m_data_guard;
    typedef std::map<std::string, DNSServiceRef> tServiceMap;
    tServiceMap m_services;
};
