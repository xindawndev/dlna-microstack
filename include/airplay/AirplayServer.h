#ifndef _CXL_YETI_AIRPLAY_SERVER_H_
#define _CXL_YETI_AIRPLAY_SERVER_H_

#include <string>
#include <map>
#include <vector>
#include "airplay/Socket.h"
#include "airplay/Lock.h"
#include <windows.h>

class HttpParser;
class DllLibPlist;

#define AIRPLAY_SERVER_VERSION_STR "101.28"

class AirplayServer
{
public:
    static void create();
    static AirplayServer * get_instance() { return server_instance_; }

public:
    static bool start_server(int port, bool nonlocal);
    static void stop_server(bool wait);
    static bool set_credentials(bool use_pwd, const std::string & pwd);
    static bool is_playing() { return m_is_playing_ > 0; }
    static int m_is_playing_;

    void process();

private:
    AirplayServer(int port, bool nonlocal);
    bool _set_internal_credentials(bool usepwd, const std::string & pwd);
    bool _initialize();
    void _deinitialize();

    class TcpClient
    {
    public:
        TcpClient();
        ~TcpClient();

        TcpClient(const TcpClient & copy);
        TcpClient & operator =(const TcpClient & other);

        void push_buffer(AirplayServer * host, const char * buffer,
            int length, std::string & session_id,
            std::map<std::string, int> & reverse_sockets);
        void disconnect();

        int             m_socket_;
        struct sockaddr m_cliaddr_;
        socklen_t       m_addrlen_;
        CriticalSection m_crit_section_;

    private:
        int _process_request(std::string & resp_header,
            std::string & resp_body,
            std::string & reverse_header,
            std::string & reverse_body,
            std::string & session_id);
        void _compose_reverse_event(std::string & reverse_header,
            std::string & reverse_body,
            std::string & session_id,
            int state);
        void _compose_auth_request_answer(std::string & resp_header, std::string & resp_body);
        bool _check_authorization(const std::string & auth_str,
            const std::string & method,
            const std::string & uri);
        void _copy(const TcpClient & client);

        HttpParser *    m_http_parser_;
        DllLibPlist *   m_plib_plist_;
        bool            m_authenticated_;
        int             m_last_event_;
        std::string     m_auth_nonce_;
    };

    std::vector<TcpClient>      m_connection_;
    std::map<std::string, int>  m_reverse_sockets_;
    int                         m_server_socket_;
    int                         m_port_;
    bool                        m_nonlocal_;
    bool                        m_use_pwd_;
    std::string                 m_pwd_;

    bool                        m_stop_;

    static AirplayServer * server_instance_;
};

#endif // _CXL_YETI_AIRPLAY_SERVER_H_
