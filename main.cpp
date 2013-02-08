#include <iostream>
#include "daytime_server.h"

#ifdef WIN32
//ta definicja normalnie jest zadeklarowana w standardzie POSIX
//w przypadku windowsa musimy zadeklarowac recznie jej wartosc na 0
#define STDIN_FILENO 0
#endif

/** Funkcja sluzy do sprawdzania czy klawisz zostal wcisniety
 *  zrodlo: http://cboard.cprogramming.com/
 */
bool typed_exit (void)
{
  //obiekt struktury okreslajacej czas oczekiwania na zdarzenie przez metode select
  struct timeval tv;
  //zbior socketow
  fd_set rdfs;

  //wypelnienie obiektu tv zerami - oznacza brak czekania na wcisniecie klawisza
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  //wyzerowanie tablicy
  FD_ZERO(&rdfs);
  //wrzucenie deskryptora strumienia wejscia do zbioru rdfs
  FD_SET(STDIN_FILENO, &rdfs);

  //sprawdzanie czy nie wcisnieto klawisza
  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);

  //sprawdzenie czy w zbiorze deskryptorow gdzie nastapilo zdarzenie jest nasz strumien wejscia
  if(!FD_ISSET(STDIN_FILENO, &rdfs)) return false;

  char word[256];
  //pobranie zawartosci oczekujacej na strumieniu wejsciowym
  std::cin.get(word, 256);
  //wyczeszczenie buforu
  std::cin.ignore();
  //std::cout << "DEBUG >> " << word << std::endl;
  //czy w buforze lancuch znakow zawieralo sie slowo "exit"
  if(word[0]=='e' && word[1]=='x' && word[2]=='i' && word[3]=='t') return true;

  return false;
}

using namespace std;

//obiekt serwera
DaytimeServer server;

int main(){

    cout << "Trwa uruchamianie serwera..." << endl;
    if(!server.start()){
		return EXIT_FAILURE;
	}

    cout << "Trwa nasluchiwanie... Wpisz slowo 'exit' aby zakonczyc..." << endl;
    while(!typed_exit()){
        //jesli nastapilo zadanie o date i czas
        if(server.isRequest()){
            //wyslanie daty i czasu do klienta
            server.answerToClient();
        }
    }

    cout << "Zakonczono nasluchiwanie, trwa wylaczanie serwera..." << endl;
    //zamykanie socketu oraz oddelegowanie biblioteki winsock w przypadku windows
    server.stop();

    return EXIT_SUCCESS;
}
