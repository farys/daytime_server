#ifndef DAYTIME_SERVER_H
#define DAYTIME_SERVER_H
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <memory.h>
#include <signal.h>
#include <cstdio>
#include <unistd.h>

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
    struct tm *timeinfo;
    int port;
    unsigned int request;

    char* getTimeInString();

};

#endif // DAYTIME_SERVER_H
