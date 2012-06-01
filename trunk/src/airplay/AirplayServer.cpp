#include "airplay/AirPlayServer.h"
#include "airplay/Md5.h"
#include "airplay/HttpParser.h"
#include "airplay/DllLibPlist.h"

#include <time.h>
#include <stdlib.h>

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

DWORD WINAPI WorkThread(void *args)
{
    AirplayServer::get_instance()->process();
    return 0;
}

void AirplayServer::create()
{
    DWORD ptid = 0;
    CreateThread(NULL, 0, &WorkThread, NULL, 0, &ptid);
}

bool AirplayServer::start_server(int port, bool nonlocal)
{
    stop_server(true);

    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 2, 0 );
    if (WSAStartup( wVersionRequested, &wsaData ) != 0) {
        return false;
    }

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
        //server_instance_->stop_thread(wait);
        if (wait) {
            delete server_instance_;
            server_instance_ = NULL;
        }
        WSACleanup();
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
            perror("AIRPLAY Server: Select failed");
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
                        printf("recv data: %dbytes\n", nread);
                        m_connection_[i].push_buffer(this, buffer, nread, sid, m_reverse_sockets_);
                    } else {
                        printf("AIRPLAY Server: Disconnection detected\n");
                        m_connection_[i].disconnect();
                        m_connection_.erase(m_connection_.begin() + i);
                    }
                }
            }

            if (FD_ISSET(m_server_socket_, &rfds)) {
                printf("AIRPLAY Server: New connection detected\n");
                TcpClient new_connection;
                new_connection.m_socket_ = accept(m_server_socket_, &new_connection.m_cliaddr_, &new_connection.m_addrlen_);
                if (new_connection.m_socket_ == INVALID_SOCKET) {
                    perror("AIRPLAY Server: Accept of new connection failed");
                } else {
                    printf("AIRPLAY Server: New connection added\n");
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
        perror("AIRPLAY Server: Failed to create server socket!");
        return false;
    }

    if (bind(m_server_socket_, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("AIRPLAY Server: Failed to bind server socket!");
        close(m_server_socket_);
        return false;
    }

    if (listen(m_server_socket_, 10) < 0) {
        perror("AIRPLAY Server: Failed to set listen!");
        close(m_server_socket_);
        return false;
    }

    printf("AIRPLAY Server: Successfully initialized\n");
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
    if (m_plib_plist_->is_loaded())
    {
        m_plib_plist_->unload();
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

        int status = _process_request(resp_header, resp_body, reverse_header, reverse_body, session_id);
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
            sprintf(buf, "%sContent-Length: %d\r\n", resp.c_str(), resp_body.size());
            resp = buf;
        }
        resp += "\r\n";

        if (resp_body.size() > 0) {
            resp + reverse_body;
        }

        if (status != AIRPLAY_STATUS_NO_RESPONSE_NEEDED) {
            send(m_socket_, resp.c_str(), resp.size(), 0);
        }

        if (reverse_header.size() > 0 && reverse_sockets.find(session_id) != reverse_sockets.end())
        {
            //search the reverse socket to this sessionid
            sprintf(buf, "POST /event HTTP/1.1\r\n");
            resp = buf;
            reverse_socket = reverse_sockets[session_id]; //that is our reverse socket
            resp += reverse_header;
        }
        resp += "\r\n";

        if (reverse_body.size() > 0) {
            resp += reverse_body;
        }

        if (reverse_socket != INVALID_SOCKET) {
            send(reverse_socket, resp.c_str(), resp.size(), 0);//send the event status on the eventSocket
        }

        // We need a new parser...
        delete m_http_parser_;
        m_http_parser_ = new HttpParser;
    }
}

void AirplayServer::TcpClient::disconnect()
{
    if (m_socket_ != INVALID_SOCKET) {
        m_crit_section_.lock();
        shutdown(m_socket_, SHUT_RDWR);
        close(m_socket_);
        m_socket_ = INVALID_SOCKET;
        delete m_http_parser_;
        m_crit_section_.unlock();
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
                printf("AIRPLAY: sending event: %s\n", eventStrings[state]);
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
    m_auth_nonce_ = Md5::GetMD5(random_str);
    sprintf(buf, AUTH_REQUIRED, m_auth_nonce_);
    resp_header = buf;
    resp_body.clear();
}

std::string str_to_lower(std::string & str)
{
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] |= 0x20;
        }
    }
    return str;
}

std::string str_to_upper(std::string & str)
{
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] &= 0xdf;
        }
    }
    return str;
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

    ha1 = Md5::GetMD5(username + ":" + realm + ":" + password);
    ha2 = Md5::GetMD5(method + ":" + digestUri);

    resp = Md5::GetMD5(str_to_lower(ha1) + ":" + nonce + ":" + str_to_lower(ha2));
    return str_to_lower(resp);
}

int split_string(const std::string & s, std::string delim, std::vector<std::string> * ret)
{
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != std::string::npos) {
        ret->push_back(s.substr(last, index - last));
        last = index + 1;
        index = s.find_first_of(delim, last);
    }
    if (index - last > 0) {
        ret->push_back(s.substr(last, index - last));
    }
    return ret->size();
}

std::string get_fild_from_string(const std::string & str, const char * field)
{
    std::string tmp_str;
    std::vector<std::string> tmp_ar1, tmp_ar2;
    split_string(str, ",", &tmp_ar1);

    for (unsigned int i = 0; i < tmp_ar1.size(); ++i) {
        if (tmp_ar1[i].find_first_of(field) != std::string::npos) {
            if (split_string(tmp_ar1[i], "=", &tmp_ar2) == 2) {
                //tmp_ar2[1].remove('\"');// 删除 "
                return tmp_ar2[1];
            }
        }
    }

    return "";
}

bool AirplayServer::TcpClient::_check_authorization(const std::string & auth_str, const std::string & method, const std::string & uri)
{
    bool auth_valid = true;
    std::string username;

    if (auth_str.empty()) return false;

    username = get_fild_from_string(auth_str, "username");
    if (username.empty())
        auth_valid = false;

    if (auth_valid)
        if (get_fild_from_string(auth_str, "realm") != AUTH_REALM)
            auth_valid = false;

    if (auth_valid)
        if (get_fild_from_string(auth_str, "nonce") != m_auth_nonce_)
            auth_valid = false;

    if (auth_valid)
        if (get_fild_from_string(auth_str, "uri") != uri)
            auth_valid = false;

    if (auth_valid) {
        std::string  realm = AUTH_REALM;
        std::string our_resp = calc_response(username, server_instance_->m_pwd_, realm, method, uri, m_auth_nonce_);
        std::string their_resp = get_fild_from_string(auth_str, "response");

        std::string tmp1 = their_resp, tmp2 = our_resp;
        if (str_to_lower(tmp1) != str_to_lower(tmp2)) { // 需要大小写不敏感比较
            auth_valid = false;
            printf("AirAuth: response mismatch - our: %s theirs: %s\n", our_resp.c_str(), their_resp.c_str());
        } else {
            printf("AirAuth: successfull authentication from AirPlay client\n");
        }
    }

    m_authenticated_ = auth_valid;

    return m_authenticated_;
}

int AirplayServer::TcpClient::_process_request(std::string & resp_header, std::string & resp_body, std::string & reverse_header, std::string & reverse_body, std::string & session_id)
{
    std::string method          = m_http_parser_->getMethod();
    std::string uri             = m_http_parser_->getUri();
    std::string query_str       = m_http_parser_->getQueryString();
    std::string body            = m_http_parser_->getBody();
    std::string content_type    = m_http_parser_->getValue("content-type");
    std::string authorization   = m_http_parser_->getValue("authorization");
    session_id                  = m_http_parser_->getValue("x-apple-session-id");
    int status                  = AIRPLAY_STATUS_OK;
    bool need_auth              = false;

    if (server_instance_->m_use_pwd_ && !m_authenticated_)
        need_auth = true;

    int start_qs = uri.find('?');
    if (start_qs != std::string::npos)
        uri = uri.substr(0, start_qs);

    if (uri == "/reverse") {
        status = AIRPLAY_STATUS_SWITCHING_PROTOCOLS;
        resp_header = "Upgrade: PTTH/1.0\r\nConnection: Upgrade\r\n";
    } else if (uri == "/rate") {
        const char* found = strstr(query_str.c_str(), "value=");
        int rate = found ? (int)(atof(found + strlen("value=")) + 0.5f) : 0;

        printf("AIRPLAY: got request %s with rate %i\n", uri.c_str(), rate);

        //if (need_auth && !_check_authorization(authorization, method, uri)) {
        //    status = AIRPLAY_STATUS_NEED_AUTH;
        //} else if (rate == 0) {
        //    if (g_application.m_pPlayer && g_application.m_pPlayer->IsPlaying() && !g_application.m_pPlayer->IsPaused()) {
        //        g_application.getApplicationMessenger().MediaPause();
        //        _compose_reverse_event(reverse_header, reverse_body, session_id, EVENT_PAUSED);
        //    }
        //} else {
        //    if (g_application.m_pPlayer && g_application.m_pPlayer->IsPlaying() && g_application.m_pPlayer->IsPaused()) {
        //        g_application.getApplicationMessenger().MediaPause();
        //        _compose_reverse_event(reverse_header, reverse_body, session_id, EVENT_PLAYING);
        //    }
        //}
    } else if (uri == "/volume") {
        const char* found = strstr(query_str.c_str(), "volume=");
        double volume = found ? (double)(atof(found + strlen("volume="))) : 0;

        printf("AIRPLAY: got request %s with volume %f\n", uri.c_str(), volume);

        if (need_auth && !_check_authorization(authorization, method, uri)) {
            status = AIRPLAY_STATUS_NEED_AUTH;
        } else if (volume >= 0 && volume <= 1) {
            //int oldVolume = g_application.GetVolume();
            //volume *= 100;
            //if(oldVolume != (int)volume) {
            //    g_application.SetVolume(volume);          
            //    g_application.getApplicationMessenger().ShowVolumeBar(oldVolume < volume);
            //}
        }
    } else if (uri == "/play") {
        std::string location;
        float position = 0.0;
        m_last_event_ = EVENT_NONE;

        printf("AIRPLAY: got request %s\n", uri.c_str());

        if (need_auth && !_check_authorization(authorization, method, uri)) {
            status = AIRPLAY_STATUS_NEED_AUTH;
        } else if (content_type == "application/x-apple-binary-plist") {
            AirplayServer::m_is_playing_++;

            if (m_plib_plist_->load()) {
                m_plib_plist_->enable_delayed_unload(false);

                const char* bodyChr = m_http_parser_->getBody();

                plist_t dict = NULL;
                m_plib_plist_->plist_from_bin(bodyChr, m_http_parser_->getContentLength(), &dict);

                if (m_plib_plist_->plist_dict_get_size(dict)) {
                    plist_t tmpNode = m_plib_plist_->plist_dict_get_item(dict, "Start-Position");
                    if (tmpNode) {
                        double tmpDouble = 0;
                        m_plib_plist_->plist_get_real_val(tmpNode, &tmpDouble);
                        position = (float)tmpDouble;
                    }

                    tmpNode = m_plib_plist_->plist_dict_get_item(dict, "Content-Location");
                    if (tmpNode) {
                        char *tmpStr = NULL;
                        m_plib_plist_->plist_get_string_val(tmpNode, &tmpStr);
                        location=tmpStr;
#ifdef _WIN32
                        m_plib_plist_->plist_free_string_val(tmpStr);
#else
                        free(tmpStr);
#endif
                    }

                    if (dict) {
                        m_plib_plist_->plist_free(dict);
                    }
                } else {
                   perror("Error parsing plist");
                }
                m_plib_plist_->unload();
            }
        } else {
            AirplayServer::m_is_playing_++;
            // Get URL to play
            //int start = body.Find("Content-Location: ");
            //if (start == -1)
            //    return AIRPLAY_STATUS_NOT_IMPLEMENTED;
            //start += strlen("Content-Location: ");
            //int end = body.Find('\n', start);
            //location = body.Mid(start, end - start);

            //start = body.Find("Start-Position");
            //if (start != -1) {
            //    start += strlen("Start-Position: ");
            //    int end = body.Find('\n', start);
            //    std::string positionStr = body.Mid(start, end - start);
            //    position = (float)atof(positionStr.c_str());
            //}
        }

        if (status != AIRPLAY_STATUS_NEED_AUTH) {
            //std::string userAgent="AppleCoreMedia/1.0.0.8F455 (AppleTV; U; CPU OS 4_3 like Mac OS X; de_de)";
            //CURL::Encode(userAgent);
            //location += "|User-Agent=" + userAgent;

            //CFileItem fileToPlay(location, false);
            //fileToPlay.SetProperty("StartPercent", position*100.0f);
            //g_application.getApplicationMessenger().MediaPlay(fileToPlay);
            //_compose_reverse_event(reverse_header, reverse_body, session_id, EVENT_PLAYING);
        }
    } else if (uri == "/scrub") {
        if (need_auth && !_check_authorization(authorization, method, uri)) {
            status = AIRPLAY_STATUS_NEED_AUTH;
        } else if (method == "GET") {
            printf("AIRPLAY: got GET request %s\n", uri.c_str());

            //if (g_application.m_pPlayer && g_application.m_pPlayer->GetTotalTime()) {
            //    float position = ((float) g_application.m_pPlayer->GetTime()) / 1000;
            //    resp_body.Format("duration: %d\r\nposition: %f", g_application.m_pPlayer->GetTotalTime(), position);

            //    //unpause media on GET scrub when paused
            //    if (g_application.m_pPlayer->IsPlaying() && g_application.m_pPlayer->IsPaused()) {
            //        g_application.getApplicationMessenger().MediaPause();
            //        _compose_reverse_event(reverse_header, reverse_body, session_id, EVENT_PLAYING);
            //    }
            //} else {
            //    status = AIRPLAY_STATUS_METHOD_NOT_ALLOWED;
            //}
        } else {
            const char* found = strstr(query_str.c_str(), "position=");

            //if (found && g_application.m_pPlayer) {
            //    __int64 position = (__int64) (atof(found + strlen("position=")) * 1000.0);
            //    g_application.m_pPlayer->SeekTime(position);
            //    printf("AIRPLAY: got POST request %s with pos %\n"PRId64, uri.c_str(), position);
            //}
        }
    } else if (uri == "/stop") {
        printf("AIRPLAY: got request %s\n", uri.c_str());
        if (need_auth && !_check_authorization(authorization, method, uri)) {
            status = AIRPLAY_STATUS_NEED_AUTH;
        } else {
            if (is_playing()) {//only stop player if we started him
                //g_application.getApplicationMessenger().MediaStop();
                AirplayServer::m_is_playing_--;
            } else {//if we are not playing and get the stop request - we just wanna stop picture streaming
                //g_windowManager.PreviousWindow();
            }
            _compose_reverse_event(reverse_header, reverse_body, session_id, EVENT_STOPPED);
        }
    } else if (uri == "/photo") {
        printf("AIRPLAY: got request %s\n", uri.c_str());
        if (need_auth && !_check_authorization(authorization, method, uri)) {
            status = AIRPLAY_STATUS_NEED_AUTH;
        } else if (m_http_parser_->getContentLength() > 0) {
            // 将图片写入缓存，然后输出显示
            //XFILE::CFile tmpFile;
            //CStdString tmpFileName = "special://temp/airplay_photo.jpg";

            //if( m_http_parser_->getContentLength() > 3 &&
            //    m_http_parser_->getBody()[1] == 'P' &&
            //    m_http_parser_->getBody()[2] == 'N' &&
            //    m_http_parser_->getBody()[3] == 'G') {
            //    tmpFileName = "special://temp/airplay_photo.png";
            //}

            //if (tmpFile.OpenForWrite(tmpFileName, true)) {
            //    int writtenBytes = 0;
            //    writtenBytes = tmpFile.Write(m_http_parser_->getBody(), m_http_parser_->getContentLength());
            //    tmpFile.Close();

            //    if (writtenBytes > 0 && (unsigned int)writtenBytes == m_http_parser_->getContentLength()) {
            //        g_application.getApplicationMessenger().PictureShow(tmpFileName);
            //    } else {
            //        perror("AirPlayServer: Error writing tmpFile!");
            //    }
            //}
        }
    } else if (uri == "/playback-info") {
        float position      = 0.0f;
        float duration      = 0.0f;
        float cacheDuration = 0.0f;
        bool playing        = false;

        printf("AIRPLAY: got request %s\n", uri.c_str());

        // 获取当前播放状态
        //if (need_auth && !_check_authorization(authorization, method, uri)) {
        //    status = AIRPLAY_STATUS_NEED_AUTH;
        //} else if (g_application.m_pPlayer) {
        //    if (g_application.m_pPlayer->GetTotalTime()) {
        //        position = ((float) g_application.m_pPlayer->GetTime()) / 1000;
        //        duration = (float) g_application.m_pPlayer->GetTotalTime();
        //        playing = g_application.m_pPlayer ? !g_application.m_pPlayer->IsPaused() : false;
        //        cacheDuration = (float) g_application.m_pPlayer->GetTotalTime() * g_application.GetCachePercentage()/100.0f;
        //    }

        //    resp_body.Format(PLAYBACK_INFO, duration, cacheDuration, position, (playing ? 1 : 0), duration);
        //    resp_header = "Content-Type: text/x-apple-plist+xml\r\n";

        //    if (g_application.m_pPlayer->IsCaching()) {
        //        _compose_reverse_event(reverse_header, reverse_body, session_id, EVENT_LOADING);
        //    } else if (playing) {
        //        _compose_reverse_event(reverse_header, reverse_body, session_id, EVENT_PLAYING);
        //    } else {
        //        _compose_reverse_event(reverse_header, reverse_body, session_id, EVENT_PAUSED);
        //    }
        //} else {
        //    resp_body.Format(PLAYBACK_INFO_NOT_READY, duration, cacheDuration, position, (playing ? 1 : 0), duration);
        //    resp_header = "Content-Type: text/x-apple-plist+xml\r\n";     
        //    _compose_reverse_event(reverse_header, reverse_body, session_id, EVENT_STOPPED);
        //}
    } else if (uri == "/server-info") {
        printf("AIRPLAY: got request %s", uri.c_str());
        char tmpbuf[256] = {0};
        sprintf(SERVER_INFO, "74:E5:0B:10:74:72"); // warning: need local mac address
        resp_body = tmpbuf;
        resp_header = "Content-Type: text/x-apple-plist+xml\r\n";
    } else if (uri == "/slideshow-features") {

    } else if (uri == "/authorize") {

    } else if (uri == "/setProperty") {
        status = AIRPLAY_STATUS_NOT_FOUND;
    } else if (uri == "/getProperty") {
        status = AIRPLAY_STATUS_NOT_FOUND;
    } else if (uri == "200") {
        status = AIRPLAY_STATUS_NO_RESPONSE_NEEDED;
    } else {
        printf("AIRPLAY Server: unhandled request [%s]\n", uri.c_str());
        status = AIRPLAY_STATUS_NOT_IMPLEMENTED;
    }

    if (status == AIRPLAY_STATUS_NEED_AUTH) {
        _compose_auth_request_answer(resp_header, resp_body);
    }

    return status;
}
