#ifndef TCPSERVER_H
#define TCPSERVER_H

/* socket include*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/*stl include*/
#include <string>
#include <fstream>
#include <cstdio>
#include <map>
#include <iterator>
#include <sstream>
#include <list>
#include <fstream>
#include <iostream>
#include <thread>

/*my libs include*/
#include "XMLParser.h"
#include "DataStruct.h"
#include "mysql.h"
#include "Opi_data.h"

#define DATAPATH "/data/" // path to save collected data

class TcpServer
{
public:
    TcpServer();
    ~TcpServer();

    int start(void);
    void get_connections();

    void socket_send(char * data, int len);

    void OpiSocket(std::map <std::string, Opi_data> :: iterator it);
    void ClientSocket(std::list<int>::iterator it);

    std::string recv_data(int sock);
    void send_data(int sock, const char * data, int len);
private:

    int listener;
    struct sockaddr_in addr;

    static const int BUFFERSIZE = 256;

    std::map <std::string, Opi_data> socket_map;
    std::map <std::string, Opi_data> :: iterator it = socket_map.begin();
    std::list <int> client_list;
    std::list <int> :: iterator client_it = client_list.begin();


};





#endif