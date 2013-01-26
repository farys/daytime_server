#include <iostream>
#include "daytime_server.h"

/*
    Wersja beta
*/

using namespace std;

DaytimeServer server;

int main(){

    if(!server.start((char*)"3000")){
        cout << "Wystapił błąd przy startowaniu.";
		return EXIT_FAILURE;
	}

    cout << "Nasluchiwanie..." << endl;
    server.listen();

    server.stop();

    return EXIT_SUCCESS;
}
