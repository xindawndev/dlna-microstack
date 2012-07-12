#include "airplay/ZeroconfWin.h"

#include <string>
#include <sstream>

ZeroconfWin::ZeroconfWin()
{
}

ZeroconfWin::~ZeroconfWin()
{
    do_stop();
}

bool ZeroconfWin::is_zc_daemon_running()
{
    uint32_t version;
    uint32_t size = sizeof(version);
    DNSServiceErrorType err = DNSServiceGetProperty(kDNSServiceProperty_DaemonVersion, &version, &size);
    if(err != kDNSServiceErr_NoError)
    {
        printf("ZeroconfWIN: Zeroconf can't be started probably because Apple's Bonjour Service isn't installed. You can get it by either installing Itunes or Apple's Bonjour Print Service for Windows (http://support.apple.com/kb/DL999)\n");
        //CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Error, g_localizeStrings.Get(34300), g_localizeStrings.Get(34301), 10000, true);
        return false;
    }
    printf("ZeroconfWIN:Bonjour version is %d.%d\n", version / 10000, version / 100 % 100);
    return true;
}

bool ZeroconfWin::do_publish_service(const std::string& fcr_identifier,
                                    const std::string& fcr_type,
                                    const std::string& fcr_name,
                                    unsigned int f_port,
                                    std::map<std::string, std::string> txt)
{
    DNSServiceRef netService = NULL;
    TXTRecordRef txtRecord;
    TXTRecordCreate(&txtRecord, 0, NULL);

   printf("ZeroconfWIN: identifier: %s type: %s name:%s port:%i\n", fcr_identifier.c_str(), fcr_type.c_str(), fcr_name.c_str(), f_port);

    //add txt records
    if(!txt.empty())
    {
        for(std::map<std::string, std::string>::const_iterator it = txt.begin(); it != txt.end(); ++it)
        {
            printf("ZeroconfWIN: key:%s, value:%s\n",it->first.c_str(),it->second.c_str());
            uint8_t txtLen = (uint8_t)strlen(it->second.c_str());
            TXTRecordSetValue(&txtRecord, it->first.c_str(), txtLen, it->second.c_str());
        }
    }

    DNSServiceErrorType err = DNSServiceRegister(&netService, 0, 0, fcr_name.c_str(), fcr_type.c_str(), NULL, NULL, htons(f_port), TXTRecordGetLength(&txtRecord), TXTRecordGetBytesPtr(&txtRecord), registerCallback, NULL);

    if (err != kDNSServiceErr_NoError)
    {
        if (netService)
            DNSServiceRefDeallocate(netService);

        printf("ZeroconfWIN: DNSServiceRegister returned (error = %ld)\n", (int) err);
    } 
    else
    {
        err = DNSServiceProcessResult(netService);

        if (err != kDNSServiceErr_NoError)
            printf("ZeroconfWIN: DNSServiceProcessResult returned (error = %ld)\n", (int) err);

        SingleLock lock(m_data_guard);
        m_services.insert(make_pair(fcr_identifier, netService));
    }

    TXTRecordDeallocate(&txtRecord);

    return err == kDNSServiceErr_NoError;
}

bool ZeroconfWin::do_remove_service(const std::string& fcr_ident)
{
    SingleLock lock(m_data_guard);
    tServiceMap::iterator it = m_services.find(fcr_ident);
    if(it != m_services.end())
    {
        DNSServiceRefDeallocate(it->second);
        m_services.erase(it);
        return true;
    } 
    else
        return false;
}

void ZeroconfWin::do_stop()
{
    SingleLock lock(m_data_guard);
    printf("ZeroconfWIN: Shutdown services\n");
    for(tServiceMap::iterator it = m_services.begin(); it != m_services.end(); ++it)
        DNSServiceRefDeallocate(it->second);
    m_services.clear();
}

void DNSSD_API ZeroconfWin::registerCallback(DNSServiceRef sdref, const DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context)
{
    (void)sdref;    // Unused
    (void)flags;    // Unused
    (void)context;  // Unused

    if (errorCode == kDNSServiceErr_NoError)
    {
        if (flags & kDNSServiceFlagsAdd)
            printf("ZeroconfWIN: %s.%s%s now registered and active\n", name, regtype, domain);
        else
            printf("ZeroconfWIN: %s.%s%s registration removed\n", name, regtype, domain);
    }
    else if (errorCode == kDNSServiceErr_NameConflict)
        printf("ZeroconfWIN: %s.%s%s Name in use, please choose another\n", name, regtype, domain);
    else
        printf("ZeroconfWIN: %s.%s%s error code %d\n", name, regtype, domain, errorCode);

}
