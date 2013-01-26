#include "daytime_server.h"

DaytimeServer::DaytimeServer(){
    this->port = 0;
    this->request = 0;
}

char* DaytimeServer::getTimeInString(){
    time(&seconds);
    timeinfo = localtime(&seconds);
    char daytime[42];

    strftime(daytime, 42, "%A, %B %d, %Y %H:%M:%S-%Z", timeinfo);
    //9, 9 2, 4 2:2:2-3 = 42znaki
    return daytime;
}

bool DaytimeServer::start(char * port){
    #ifdef WIN32
      WORD wVersionRequested;
      WSADATA wsaData;
      wVersionRequested = MAKEWORD(1,1);
      int ver = WSAStartup(wVersionRequested, &wsaData);
      int hib = HIBYTE(wsaData.wHighVersion);
      int lob = LOBYTE(wsaData.wHighVersion);
    #endif

    struct sockaddr_in sin;
    struct servent *pse;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(sock < 0) return false;

    sin.sin_family = AF_INET;
    if((pse = getservbyname(port, "udp")) == NULL){
        sin.sin_port = htons(atoi(port));
    } else sin.sin_port = pse->s_port;

    sin.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr*)&sin, sizeof(sin)) != 0){
        return false;
    }

    this->port = htons(sin.sin_port);

    return true;
}

void DaytimeServer::listen(){
    this->go_offline = false;
    char buff[50];

    char endline[3] = {13, 10, 0};
    struct sockaddr client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    timeval t = {0, 100};
    fd_set read;

    while(!go_offline){

        FD_ZERO(&read);
        FD_SET(sock, &read);

        if(select(sock+1, &read, NULL, NULL, &t)){
                client_addr_len = sizeof(client_addr);
                recvfrom(sock, NULL, 0, 0, &client_addr, &client_addr_len);

                sprintf(buff, "%s%s", getTimeInString(), endline);

                sendto(sock, buff, strlen(buff), 0, &client_addr, client_addr_len);
                request++;
        }
    }
}

void DaytimeServer::stopListen(){
    this->go_offline = true;
}

bool DaytimeServer::stop(){
    #ifdef WIN32
       WSACleanup();
    #endif
    close(this->sock);
    this->port = 0;
    return true;
}
