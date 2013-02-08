#ifndef DAYTIME_SERVER_H
#define DAYTIME_SERVER_H
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <cstdio>

//biblioteki dla systemu Windows
#ifdef WIN32
#include <winsock2.h>
typedef int socklen_t;
#endif
//biblioteki dla systemu LINUX
#ifdef __linux
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
/**
 * Głowna klasa serwera
 */
class DaytimeServer{
public:
    /**
     * Konstruktor klasy, ustawawia domyślne wartosci atrybutow klasy.
     */
    DaytimeServer();

    /**
     * Metoda inicjuje gniazdo sieciowe oraz podpina pod odpowiednie port w systemie.
     * Zwraca true jesli wszystko sie powiedzie
     */
    bool start();

    /**
     * Metoda sprawdza czy nastapilo zadanie o date i czas
     * Zwraca true jesli nastapilo zadanie
     */
    bool isRequest();

    /**
     * Odbiera zadanie po czym wysyla daytime
     */
    void answerToClient();

    /**
     * Zwalnia zarezerwowane przez serwer zasoby
     */
    void stop();

private:
    /** atrybut klasy do identyfikacji utworzonego gniazda sieciowego */
    int sock;

    /** obiekt potrzebny do obrobki czasu */
    time_t seconds;

    /** ostatni lancuch znakow zawierajacy czas w formacie daytime */
    char last_daytime[21];

    /** obiekt przechowujacy czas */
    struct tm *timeinfo;
    /** zmienna przechowuje ilosc zadan po czas */
    unsigned int request;

    /**
     * Metoda zwraca wskaznik na czas w formacie daytime
     * @return czas w formacie daytime
     */
    char* getDaytimeInString();

};

#endif // DAYTIME_SERVER_H
