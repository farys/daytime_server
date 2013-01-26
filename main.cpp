#include <iostream>
#include "daytime_server.h"

/*
    Wersja beta
*/

using namespace std;

DaytimeServer server;

int main(int argc, char *argv[]){
    char *port;
    if(argc == 2){
        port = argv[1];
    }else{
        port = "3000";
    }

    if(!server.start((char*)port)){
        cout << "Wystapił błąd przy startowaniu.";
		return EXIT_FAILURE;
	}

    cout << "Nasluchiwanie..." << endl;
    server.listen();

    server.stop();

    return EXIT_SUCCESS;
}
