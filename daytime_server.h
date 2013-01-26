#ifndef DAYTIME_SERVER_H
#define DAYTIME_SERVER_H
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <cstdio>
#include <unistd.h>

#ifdef WIN32
#include <winsock2.h>
typedef int socklen_t;
#endif

#ifdef __linux
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

class DaytimeServer{
public:
    DaytimeServer();
    bool start(char * port);
    void listen();
    void stopListen();
    bool stop();

private:
    int sock;
    bool go_offline;
    time_t seconds;
    char last_daytime[50];
    struct tm *timeinfo;
    int port;
    unsigned int request;

    char* getTimeInString();

};

#endif // DAYTIME_SERVER_H
