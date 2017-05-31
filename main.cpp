#include <iostream>
#include <progbase-cpp/net.h>
#include <vector>
#include "film.h"
#include "requestHandler.h"
using namespace std;
using namespace progbase::net;
int main() {
    vector <Film> films;
    films.push_back(Film("Mummy",1999,8.2));
    films.push_back(Film("Pulp Fiction",1994,8.9));
    films.push_back(Film("Harry Potter 1",2001,8.7));
    films.push_back(Film("Martian",2015,7.7));
    const int serverPort = 3000;
    TcpListener listener;
    NetMessage message(10000);
    try {
        listener.bind(IpAddress("127.0.0.1", serverPort));
        listener.start();
        while (true) {
            cout << ">> Waiting for clients at " << serverPort << "..." << endl;
            TcpClient * client = listener.accept();
            client->receive(message);
            cout << ">> Received: " << endl << message.dataAsString() << endl;
            auto data = message.dataAsString();
            //func to make response
            auto separated =parseRequest(data);
            //string responseMessage = makeResponse(some parameters
            int status = analyzeRequest(separated);
            auto r = response(films, status,separated);
            message.setDataString(r);
            cout<< r <<"/n/n";
            client->send(message);
            cout << ">> Response sent." << endl;
            delete client;
        }
    } catch(NetException const & exc) {
        cerr << exc.what() << endl;
    }
    return 0;

}
