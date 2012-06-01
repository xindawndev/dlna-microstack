#include "airplay/AirplayServer.h"
#include "airplay/ZeroconfWin.h"

int main(int argc, char **argv)
{
    int listenPort = 36667;
    std::string password = "";
    bool usePassword = false;
    ZeroconfWin zfw;

    if (AirplayServer::start_server(listenPort, true)) {
        AirplayServer::set_credentials(usePassword, password);
        std::map<std::string, std::string> txt;
        if (true) {
            txt["deviceid"] = "74:E5:0B:10:74:72";
        } else {
            txt["deviceid"] = "FF:FF:FF:FF:FF:F2";
        }
        txt["features"] = "0x77";
        txt["model"] = "AppleTV2,1";
        txt["srcvers"] = AIRPLAY_SERVER_VERSION_STR;
        zfw.do_publish_service("servers.airplay", "_airplay._tcp", "my_airplay", listenPort, txt);
    }

    while(1);
    return 0;
}