#include "daytime_server.h"
#include <iostream>
#include <exception>

DaytimeServer::DaytimeServer(){
    this->request = 0;
}

char* DaytimeServer::getDaytimeInString(){
    time(&this->seconds);
    timeinfo = localtime(&this->seconds);

    strftime(last_daytime, 21, "%FT%TZ", timeinfo);

    return last_daytime;
}

bool DaytimeServer::start(){
    #ifdef WIN32
      WORD wVersionRequested;
      WSADATA wsaData;
      wVersionRequested = MAKEWORD(1, 1);
      WSAStartup(wVersionRequested, &wsaData);
    #endif

    struct sockaddr_in sin;
    struct servent *pse;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(sock < 0){
        std::cerr << "Blad przy tworzeniu gniazda sieciowego." << std::endl;
        return false;
    }

    sin.sin_family = AF_INET;
    if((pse = getservbyname("daytime", "udp")) == NULL){
        sin.sin_port = htons(13);
    } else sin.sin_port = pse->s_port;

    sin.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr*)&sin, sizeof(sin)) != 0){
        std::cerr << "Nie mozna rozpoczac sluchania na wybranym porcie." << std::endl;
        return false;
    }

    return true;
}

bool DaytimeServer::isRequest(){
    timeval t = {1, 0};
    fd_set read;
    FD_ZERO(&read);
    FD_SET(sock, &read);

    return select(sock+1, &read, NULL, NULL, &t);
}

void DaytimeServer::answerToClient(){
    struct sockaddr client_addr;
    socklen_t client_addr_len;

    client_addr_len = sizeof(sockaddr_in);
    recvfrom(sock, NULL, 0, 0, &client_addr, &client_addr_len);
    char *timeBuf = getDaytimeInString();

    sendto(sock, timeBuf, strlen(timeBuf), 0, &client_addr, client_addr_len);
    request++;
}

void DaytimeServer::stop(){
    #ifdef WIN32
       WSACleanup();
    #endif
    close(this->sock);
}
