#include "AirPlayServer.h"

#if defined(_WIN32)
#define close closesocket
#endif

#define RECEIVEBUFFER 1024

#define AIRPLAY_STATUS_OK                  200
#define AIRPLAY_STATUS_SWITCHING_PROTOCOLS 101
#define AIRPLAY_STATUS_NEED_AUTH           401
#define AIRPLAY_STATUS_NOT_FOUND           404
#define AIRPLAY_STATUS_METHOD_NOT_ALLOWED  405
#define AIRPLAY_STATUS_NOT_IMPLEMENTED     501
#define AIRPLAY_STATUS_NO_RESPONSE_NEEDED  1000

AirplayServer * AirplayServer::server_instance_ = NULL;
int AirplayServer::m_is_playing_                = 0;

#define EVENT_NONE     -1
#define EVENT_PLAYING   0
#define EVENT_PAUSED    1
#define EVENT_LOADING   2
#define EVENT_STOPPED   3
const char *eventStrings[] = {"playing", "paused", "loading", "stopped"};

#define PLAYBACK_INFO  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"\
    "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\r\n"\
    "<plist version=\"1.0\">\r\n"\
    "<dict>\r\n"\
    "<key>duration</key>\r\n"\
    "<real>%f</real>\r\n"\
    "<key>loadedTimeRanges</key>\r\n"\
    "<array>\r\n"\
    "\t\t<dict>\r\n"\
    "\t\t\t<key>duration</key>\r\n"\
    "\t\t\t<real>%f</real>\r\n"\
    "\t\t\t<key>start</key>\r\n"\
    "\t\t\t<real>0.0</real>\r\n"\
    "\t\t</dict>\r\n"\
    "</array>\r\n"\
    "<key>playbackBufferEmpty</key>\r\n"\
    "<true/>\r\n"\
    "<key>playbackBufferFull</key>\r\n"\
    "<false/>\r\n"\
    "<key>playbackLikelyToKeepUp</key>\r\n"\
    "<true/>\r\n"\
    "<key>position</key>\r\n"\
    "<real>%f</real>\r\n"\
    "<key>rate</key>\r\n"\
    "<real>%d</real>\r\n"\
    "<key>readyToPlay</key>\r\n"\
    "<true/>\r\n"\
    "<key>seekableTimeRanges</key>\r\n"\
    "<array>\r\n"\
    "\t\t<dict>\r\n"\
    "\t\t\t<key>duration</key>\r\n"\
    "\t\t\t<real>%f</real>\r\n"\
    "\t\t\t<key>start</key>\r\n"\
    "\t\t\t<real>0.0</real>\r\n"\
    "\t\t</dict>\r\n"\
    "</array>\r\n"\
    "</dict>\r\n"\
    "</plist>\r\n"

#define PLAYBACK_INFO_NOT_READY  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"\
    "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\r\n"\
    "<plist version=\"1.0\">\r\n"\
    "<dict>\r\n"\
    "<key>readyToPlay</key>\r\n"\
    "<false/>\r\n"\
    "</dict>\r\n"\
    "</plist>\r\n"

#define SERVER_INFO  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"\
    "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\r\n"\
    "<plist version=\"1.0\">\r\n"\
    "<dict>\r\n"\
    "<key>deviceid</key>\r\n"\
    "<string>%s</string>\r\n"\
    "<key>features</key>\r\n"\
    "<integer>119</integer>\r\n"\
    "<key>model</key>\r\n"\
    "<string>AppleTV2,1</string>\r\n"\
    "<key>protovers</key>\r\n"\
    "<string>1.0</string>\r\n"\
    "<key>srcvers</key>\r\n"\
    "<string>"AIRPLAY_SERVER_VERSION_STR"</string>\r\n"\
    "</dict>\r\n"\
    "</plist>\r\n"

#define EVENT_INFO "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\r\n"\
    "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n\r\n"\
    "<plist version=\"1.0\">\r\n"\
    "<dict>\r\n"\
    "<key>category</key>\r\n"\
    "<string>video</string>\r\n"\
    "<key>state</key>\r\n"\
    "<string>%s</string>\r\n"\
    "</dict>\r\n"\
    "</plist>\r\n"\

#define AUTH_REALM "AirPlay"
#define AUTH_REQUIRED "WWW-Authenticate: Digest realm=\""  AUTH_REALM  "\", nonce=\"%s\"\r\n"

bool AirplayServer::start_server(int port, bool nonlocal)
{
    stop_server(true);
    server_instance_ = new AirplayServer(port, nonlocal);
    if (server_instance_->_initialize()) {
        server_instance_->create();
        return true;
    }

    return false;
}

bool AirplayServer::set_credentials(bool use_pwd, const std::string & pwd)
{
    bool ret = false;
    if (server_instance_) {
        ret = server_instance_->_set_internal_credentials(use_pwd, pwd);
    }
    return ret;
}

bool AirplayServer::_set_internal_credentials(bool usepwd, const std::string & pwd)
{
    m_use_pwd_  = usepwd;
    m_pwd_      = pwd;
    return true;
}

void AirplayServer::stop_server(bool wait)
{
    if (server_instance_) {
        server_instance_->stop_thread(wait);
        if (wait) {
            delete server_instance_;
            server_instance_ = NULL;
        }
    }
}

AirplayServer::AirplayServer(int port, bool nonlocal)
{
    m_port_             = port;
    m_nonlocal_         = nonlocal;
    m_server_socket_    = INVALID_SOCKET;
    m_use_pwd_          = false;
}

void AirplayServer::process()
{
    m_stop_ = false;

    while (!m_stop_) {
        int             max_fd = 0;
        fd_set          rfds;
        struct timeval  to = {1, 0};
        FD_ZERO(&rfds);
        FD_SET(m_server_socket_, &rfds);
        max_fd = m_server_socket_;

        for (unsigned int i = 0; i < m_connection_.size(); ++i) {
            FD_SET(m_connection_[i].m_socket_, &rfds);
            if (m_connection_[i].m_socket_ > max_fd)
                max_fd = m_connection_[i].m_socket_;
        }

        int res = select(max_fd + 1, &rfds, NULL, NULL, &to);
        if (res < 0) {
            perror("select");
            Sleep(1000);
            _initialize();
        } else if (res > 0) {
            for (unsigned int i = m_connection_.size() - 1; i >= 0; --i) {
                int sock = m_connection_[i].m_socket_;
                if (FD_ISSET(sock, &rfds)) {
                    char buffer[RECEIVEBUFFER] = {0};
                    int nread = 0;
                    nread = recv(sock, (char *)&buffer, RECEIVEBUFFER, 0);
                    if (nread > 0) {
                        std::string sid;
                        m_connection_[i].push_buffer(this, buffer, nread, sid, m_reverse_sockets_);
                    } else {
                        perror("disconnection");
                        m_connection_[i].disconnect();
                        m_connection_.erase(m_connection_.begin() + i);
                    }
                }
            }

            if (FD_ISSET(m_server_socket_, &rfds)) {
                printf("new connection\n");
                TcpClient new_connection;
                new_connection.m_socket_ = accept(m_server_socket_, &new_connection.m_cliaddr_, &new_connection.m_addrlen_);
                if (new_connection.m_socket_ == INVALID_SOCKET) {
                    perror("accept");
                } else {
                    printf("new connection added\n");
                    m_connection_.push_back(new_connection);
                }
            }
        }
    }
    _deinitialize();
}

bool AirplayServer::_initialize()
{
    _deinitialize();

    struct sockaddr_in myaddr;

    myaddr.sin_family   = AF_INET;
    myaddr.sin_port     = htons(m_port_);

    if (m_nonlocal_) {
        myaddr.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, "127.0.0.1", &myaddr.sin_addr.s_addr);
    }

    m_server_socket_ = socket(PF_INET, SOCK_STREAM, 0);

    if (m_server_socket_ == INVALID_SOCKET) {
        perror("create server socket");
        return false;
    }

    if (bind(m_server_socket_, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind server socket");
        close(m_server_socket_);
        return false;
    }

    if (listen(m_server_socket_, 10) < 0) {
        perror("server set listen");
        close(m_server_socket_);
        return false;
    }

    printf("initialized successfully!\n");
    return true;
}

void AirplayServer::_deinitialize()
{
    for (unsigned int i = 0; i < m_connection_.size(); ++i) {
        m_connection_[i].disconnect();
    }

    m_connection_.clear();
    m_reverse_sockets_.clear();

    if (m_server_socket_ != INVALID_SOCKET) {
        shutdown(m_server_socket_, SHUT_RDWR);
        close(m_server_socket_);
        m_server_socket_ = INVALID_SOCKET;
    }
}

AirplayServer::TcpClient::TcpClient()
{
    m_socket_           = INVALID_SOCKET;
    m_http_parser_      = new HttpParser();

    m_addrlen_          = sizeof(struct sockaddr);
    m_plib_plist_       = new DllLibPlist();

    m_authenticated_    = false;
    m_last_event_       = EVENT_NONE;
}

AirplayServer::TcpClient::TcpClient(const TcpClient & copy)
{
    _copy(copy);
    m_plib_plist_       = new DllLibPlist();
}

AirplayServer::TcpClient::~TcpClient()
{
    if (m_pLm_plib_plist_ibPlist->IsLoaded())
    {
        m_plib_plist_->Unload();
    }
    delete m_plib_plist_;
}

AirplayServer::TcpClient & AirplayServer::TcpClient::operator =(const TcpClient & other)
{
    _copy(other);
    return *this;
}

void AirplayServer::TcpClient::push_buffer(AirplayServer * host, const char * buffer, int length, std::string & session_id, std::map<std::string, int> & reverse_sockets)
{
    HttpParser::status_t status = m_http_parser_->addBytes(buffer, length);

    if (status == HttpParser::Done) {
        std::string resp_header, resp_body;
        std::string reverse_header, reverse_body;

        int status = ;
        std::string status_msg = "OK";
        int reverse_socket = INVALID_SOCKET;

        switch (status) {
            case AIRPLAY_STATUS_NOT_IMPLEMENTED:
                status_msg = "Not Implemented";
                break;
            case AIRPLAY_STATUS_SWITCHING_PROTOCOLS:
                status_msg = "Switching Protocols";
                reverse_sockets[session_id] = m_socket_;
                break;
            case AIRPLAY_STATUS_NEED_AUTH:
                status_msg = "Unauthorized";
                break;
            case AIRPLAY_STATUS_NOT_FOUND:
                status_msg = "Not Found";
                break;
            case AIRPLAY_STATUS_METHOD_NOT_ALLOWED:
                status_msg = "Method Not Allowed";
                break;
        }

        std::string resp;
        char buf[512] = {0};
        const time_t ltime = time(NULL);
        char * date = asctime(gmtime(&ltime));
        date[strlen(date) - 1] = '\0';
        sprintf(buf, "HTTP/1.1 %d %s\nDate: %s\r\n", status, status_msg.c_str(), date);
        resp = buf;

        if (resp_header.size() > 0) {
            resp += resp_header;
        }
        if (resp_body.size() > 0) {

        }
    }
}

void AirplayServer::TcpClient::disconnect()
{
    if (m_socket_ != INVALID_SOCKET) {
        Lock(m_cri_section_);
        shutdown(m_socket_);
        m_socket_ = INVALID_SOCKET;
        delete m_http_parser_;
    }
}

void AirplayServer::TcpClient::_copy(const TcpClient & client)
{
    m_socket_           = client.m_socket_;
    m_cliaddr_          = client.m_cliaddr_;
    m_addrlen_          = client.m_addrlen_;
    m_http_parser_      = client.m_http_parser_;
    m_auth_nonce_       = client.m_auth_nonce_;
    m_authenticated_    = client.m_authenticated_;
}

void AirplayServer::TcpClient::_compose_reverse_event(std::string & reverse_header, std::string & reverse_body, std::string & session_id, int state)
{
    if (m_last_event_ != state) {
        switch (state) {
            case EVENT_PLAYING:
            case EVENT_LOADING:
            case EVENT_PAUSED:
            case EVENT_STOPPED:
                char buf[512] = {0};
                sprintf(buf, EVENT_INFO, eventStrings[state]);
                reverse_body = buf;
                printf("sending event: %s\n", eventStrings[state]);
                break;
        }
        char buf[512] = {0};
        reverse_header = "Content-Type: text/x-apple-plist+xml\r\n";
        sprintf(buf, "%sContent-Length: %d", reverse_header.c_str(), reverse_header.size());
        reverse_header = buf;
        sprintf(buf, "%sx-apple-session-id: %s\r\n", reverse_header.c_str(), session_id.c_str());
        reverse_header = buf;
        m_last_event_ = state;
    }
}

void AirplayServer::TcpClient::_compose_auth_request_answer(std::string & resp_header, std::string & resp_body)
{
    std::string random_str;
    short random = rand();
    char buf[256] = {0};
    sprintf(buf, "%i", random);
    random_str = buf;
    m_auth_nonce_ = Md5(random_str);
    sprintf(buf, AUTH_REQUIRED, m_auth_nonce_);
    resp_header = buf;
    resp_body.clear();
}

std::string calc_response(const std::string& username,
                          const std::string& password,
                          const std::string& realm,
                          const std::string& method,
                          const std::string& digestUri,
                          const std::string& nonce)
{
    std::string resp;
    std::string ha1;
    std::string ha2;

    ha1 = Md5(username + ":" + realm + ":" + password);
    ha2 = Md5(method + ":" + digestUri);

    resp = Md5(ha1.tolower() + ":" + nonce + ":" + ha2.tolower());
    return resp.tolower();
}

std::string get_fild_from_string(const std::string & str, const char * field)
{
    std::string tmp_str;
    std::tmp_ar1, tmp_ar2;
}
