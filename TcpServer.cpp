#include "TcpServer.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <list>
#include <iterator>
#include <cstdio>

//-----------------------
#include <string.h>

TcpServer::TcpServer() {

this->addr.sin_family = AF_INET;
this->addr.sin_port = htons(3425);
this->addr.sin_addr.s_addr = htonl(INADDR_ANY);
}


int TcpServer::start(void) // --------------work true--------------------
{
    this->listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(this->listener < 0)
        {
            std::cout << "listen socket not created" << std::endl;
            return 0;
        }
        if(bind(this->listener, (struct sockaddr *)&this->addr, sizeof(this->addr)) < 0)
        {
            std::cout << "listener bind error" << std::endl;
            return 0;
        }


    listen(this->listener, 10);
    while(1)
    {
        char id_buffer[3];
        int bytes_read = 0;

        int sock = accept(this->listener, NULL, NULL);
        if(sock < 0)
        {
            std::cout << "connection accept error" << std::endl;
            return 0;
        }

        std::cout << "someone connected: "<< sock << std::endl;

        bytes_read = recv(sock, id_buffer, 3, 0);

        std::cout << "Id: " << id_buffer << std::endl;
        /* Проверка налличия данных, если данных нет то поток не создаем и слушаем дальше */
        if(bytes_read <= 0)
            continue;

        std::string id = id_buffer;
        this->socket_map.insert( std::pair<std::string,int>(id, sock) );
        it = socket_map.find(id);
        std::thread data_recv_thread(&TcpServer::OpiSocket, this, it);
        data_recv_thread.detach();
    }

    return 1;
}



void TcpServer::OpiSocket(std::map <std::string, int> :: iterator it) // --------------work true--------------------
{

    char * recv_buffer;
    char len_buffer[6];

    int sock = it->second;
    int len_recvd = 0;
    int len_to_recv = 0;
    int to_get = this->BUFFERSIZE;
    int bytes_read = 0;

    std::ostringstream ss;

    std::cout << "thread for Id " << it->first << " started" << std::endl;

    while(1)
    {
        ss.flush();
        bytes_read = 0;
        len_to_recv = 0;
        len_recvd = 0;
        char rdata[1024];

        bytes_read = recv(sock, len_buffer, 6, 0);
        if(bytes_read <= 0)
        {
            this->socket_map.erase(it);
            break;
        }

        sscanf(len_buffer, "%d", &len_to_recv);
        std::cout << "parsel size: " << len_to_recv << " bytes" << std::endl;

        while(len_recvd < len_to_recv)
        {
            len_recvd += BUFFERSIZE;
            if(len_recvd >= len_to_recv)
            {
                to_get = this->BUFFERSIZE - (len_recvd - len_to_recv);
            }
            else to_get = this->BUFFERSIZE;

            recv_buffer = new char[to_get];

            std::cout << "to get: " << to_get << " bytes" << std::endl;
            
            bytes_read = recv(sock, recv_buffer, to_get, 0);
            if(bytes_read <= 0)
            {
                std::cout << "recv failed" << std::endl;
                this->socket_map.erase(it);
                break;
            }

            ss << recv_buffer;
            delete recv_buffer;
        }

        std::cout << "recv: "<< ss.str().size() << " bytes: " << ss.str() << std::endl;
    }





    std::cout << "socket " << it->first << " closed" << std::endl;
    close(it->second);
    return;
}


TcpServer::~TcpServer() {

}

/*
void TcpServer::socket_send(char * data, int len)
{
    char buff[6];

    sprintf(buff, "%06d", len);
    std::cout << "len to send: " << buff << std::endl;

    this->it = this->socket_list.begin();
    while(this->it != this->socket_list.end())
    {
        int transfer_len = 0;
        int sended = 0;

        std::cout << "send to: " << *it << std::endl;

        if(send(*it, buff, 6, 0) <= 0)
        {
            std::cout << "send len error" << std::endl;
            return;
        }
/*
        int len_send = 0;
        while(len_send < len)
        {
            len_send += 1024;
            if(len_send > len)
            {
                len_send = 1024 - (len_send - len);
            }
            if(send(*it, data, len_send, 0) <= 0)
            {
                std::cout << "send data error" << std::endl;
                return;
            }
            data += len_send;
        }


        while (sended < len)
        {
            if(len >= this->BUFFERSIZE)
                transfer_len = len;

            else
                transfer_len = len - sended;

            if(send(*it, data, transfer_len, 0) <= 0)
            {
                std::cout << "send data error" << std::endl;
                return;
            }
            sended += transfer_len;
            data += transfer_len;
        }


        std::cout << len << " bytes sended successfully" << std::endl;
        this->it++;
    }
}
*/