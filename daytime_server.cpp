#include "daytime_server.h"
#include <iostream>
#include <exception>

DaytimeServer::DaytimeServer(){
    //ilosc zadan ustawiamy na 0
    this->request = 0;
}

char* DaytimeServer::getDaytimeInString(){
    //funkcja ustawia wartosc atrybutu seconds iloscia sekund jaka uplynela od 1 stycznia 1970 roku
    time(&this->seconds);
    //wypelnia strukture timeinfo informacjami zwiazanymi z czasem
    //dzieki strukturze mozemy pobrac np. aktualny: rok, miesiac, dzien.
    timeinfo = localtime(&this->seconds);
    //przetwarzamy dane z timeinfo do formatu ISO8601
    strftime(last_daytime, 21, "%FT%TZ", timeinfo);
    //zwracamy przetworzony czas
    return last_daytime;
}

bool DaytimeServer::start(){
    //jesli kompilowany plik binarny jest pod system Windows
    #ifdef WIN32
        //zmienna przechowuje wersje biblioteki, ktora chcielibysmy wykorzystac
        WORD wVersionRequested;
        //struktura zawierajaca informacje o implementacji Windows Socket w systemie,
        //wypelniana przy pomocy funkcji WSAStartup
        WSADATA wsaData;
        //wykorzystujemy makro zdefiniowane w Windef.h
        //przy pomocy makra deklarujemy wersje biblioteki, ktora chcemy wykorzystac
        wVersionRequested = MAKEWORD(1, 1);
        //funkcja podpina pod proces klienta windowsowe biblioteki Winsock
        //gdy biblioteki zostana pomyslnie dolaczone do procesu, funkcja zwroci 0
        if(WSAStartup(wVersionRequested, &wsaData) != 0){
            std::cerr << "Blad podczas dolaczenia Winsock DLL do procesu." << std::endl;
            return false;
        }
    #endif
    //struktura adresowa przystosowana dla protokołu IP w wersji 4,
    //odpowiednio spreparowana wersja sockaddr, ktora ulatwia dostep do czesci struktury
    //zwiazanej z rodzina protokolu, adresu oraz portu
    struct sockaddr_in sin;
    //struktura do przechowywania informacji o usludze,
    //zawiera oficjalna nazwe uslugi, liste innych nazw uslugi, port uslugi, protokol na ktorym usluga dziala
    struct servent *pse;
    //tworzymy gniazdo dla UDP
    //funkcja zwraca identyfikator utworzonego gniazda sieciowego
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //jesli socket zostal utworzony
    if(sock < 0){
        std::cerr << "Blad przy tworzeniu gniazda sieciowego." << std::endl;
        return false;
    }
    //ustawiamy rodzine adresu
    sin.sin_family = AF_INET;
    //szukamy uslugi po nazwie,
    //jesli jest, to ustawiamy jej port,
    //jesli nie ma to ustawiamy nasz port
    if((pse = getservbyname("daytime", "udp")) == NULL){
        sin.sin_port = htons(13);
    } else sin.sin_port = pse->s_port;

    //deklarujemy chec odbierania pakietow na wszystkich adresach IP, ktore uzywa nasz host
    sin.sin_addr.s_addr = INADDR_ANY;
    //zaczynamy nasluch na naszym gniezdzie sieciowym, z parametrami ustawionymi w naszym obiekcie sin
    if(bind(sock, (struct sockaddr*)&sin, sizeof(sin)) != 0){
        std::cerr << "Nie mozna rozpoczac sluchania na wybranym porcie." << std::endl;
        return false;
    }

    //jesli wszytko przebieglo pomyslnie zwracamy true
    return true;
}

bool DaytimeServer::isRequest(){
    //obiekt, ktory sluzy okresleniu czasu oczekiwania na zdarzenie
    timeval t = {1, 0};
    //obiekt okreslajacy zbior gniazd na ktorych bedziemy wyczekiwac na pakiety
    fd_set read;
    //zerujemy zbior 'read'
    FD_ZERO(&read);
    //dodajemy gniazdo sieciowe do zbioru gniazd 'read'
    FD_SET(sock, &read);
    //zwracamy true jesli nastapilo otrzymanie pakietu na gniezdzie, na ktorym nasluchujemy
    return select(sock+1, &read, NULL, NULL, &t);
}

void DaytimeServer::answerToClient(){
    //struktura adresowa klienta, ktory zazadal czasu
    struct sockaddr client_addr;
    //wielkosc struktury client_addr
    socklen_t client_addr_len;

    //ustawiamy wielkosc struktury adresu klienta
    client_addr_len = sizeof(sockaddr_in);
    //odbieramy pakiet
    //nie odbieramy zawartosci pakietu, poniewaz zawartosc nie jest potrzebna
    recvfrom(sock, NULL, 0, 0, &client_addr, &client_addr_len);
    //pobieramy aktualny czas w formacie ISO8601
    char *timeBuf = getDaytimeInString();
    //ilosc wyslanych bajtow do tej pory
    unsigned int sent = 0;

    //wysylamy czas do klienta, sprawdzajac czy wszystkie bajty timeBuf zostaly wyslane
    do{
        //wysylamy pakiet z offsetem jesli pozostala jakas czesc do wyslania,
        //funkcja sendto zwraca ilosc wyslanych bajtow
        int s = sendto(sock, timeBuf+sent, strlen(timeBuf)-sent, 0, &client_addr, client_addr_len);
        //aktualizujemy licznik wyslanych bajtow
        sent += s;
        //powtarzamy operacje jesli nie wszystkie bajty zostaly wyslane
    }while(strlen(timeBuf) > sent);
    //zwiekszamy licznik zadan o 1
    request++;
}

void DaytimeServer::stop(){
    //jesli plik binarny jest dla systemu Windows to odlaczamy biblioteke Winsock
    #ifdef WIN32
       WSACleanup();
    #endif
    //zamykamy gniazdo sieciowe
    close(this->sock);
}
