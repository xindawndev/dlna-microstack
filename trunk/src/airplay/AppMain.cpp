#include "airplay/AirplayServer.h"

int main(int argc, char **argv)
{
    AirplayServer::start_server(36667, true);
    return 0;
}