#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <fstream>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <map>
#include <iterator>
#include <sstream>


#include "XMLParser.h"
#include "DataStruct.h"
#include "mysql.h"

class TcpServer
{
public:
    TcpServer();
    ~TcpServer();

    int start(void);
    void get_connections();

    void socket_send(char * data, int len);
    void OpiSocket(std::map <std::string, int> :: iterator it);
    std::string recv_data(int sock);
private:

    int listener;
    struct sockaddr_in addr;

    static const int BUFFERSIZE = 1024;

    std::map <std::string, int> socket_map;
    std::map <std::string, int> :: iterator it = socket_map.begin();

};





#endif