#include "airplay/AirplayServer.h"

int main(int argc, char **argv)
{
    int listenPort = 36667;
    std::string password = "";
    bool usePassword = false;

    if (AirplayServer::start_server(listenPort, true))
    {
        AirplayServer::set_credentials(usePassword, password);
        std::map<std::string, std::string> txt;
        if (false)
        {
            txt["deviceid"] = "My_MAC_Addr";
        }
        else
        {
            txt["deviceid"] = "FF:FF:FF:FF:FF:F2";
        }
        txt["features"] = "0x77";
        txt["model"] = "AppleTV2,1";
        txt["srcvers"] = AIRPLAY_SERVER_VERSION_STR;
        Zeroconf::get_instance()->publish_service("servers.airplay", "_airplay._tcp", "my_airplay", listenPort, txt);
    }

    return 0;
}