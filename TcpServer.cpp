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

#include <vector>
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


/* --------------------------------------------------------------------------------прошлая версия--------------------------------------------------------------------------------
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

            if(ss.str() == "image")
            {

            }
        }

        std::cout << "recv: "<< ss.str().size() << " bytes: " << ss.str() << std::endl;
    }

    std::cout << "socket " << it->first << " closed" << std::endl;
    close(it->second);
    return;
}
*/



void TcpServer::OpiSocket(std::map <std::string, int> :: iterator it) // --------------work true--------------------
{

    std::string data_str;
    std::string fname;
    std::string path = "data/";
    std::cout << "thread for Id " << it->first << " started" << std::endl;
    DataStruct parsed_data;
    while(1)
    {
        /* recv name of data */
       data_str = this->recv_data(it->second);
       if(data_str.empty())
            break;

        /* if get xml recv file name and xml data
            save file
            parce file and save data to database
        */
        if(data_str == "xml")
        {
            data_str = this->recv_data(it->second);
            if(data_str.empty())
                break;
            fname = data_str;

            data_str = this->recv_data(it->second);
            if(data_str.empty())
                break;

            std::ofstream xml_file(path + fname);
            xml_file << data_str;
            xml_file.close();

/*
            XMLParser xml(fname.c_str());
            if(!xml.loadxml())
                break;
            parsed_data = xml.Parse_to_struct();
            */
        }

        /* collect and save image */
        else if(data_str == "image")
        {
            std::string str = this->recv_data(it->second);
            if(str.empty())
                break;

            std::ofstream img_file(path + "pic.jpg");
            img_file << str;
            img_file.close();
        }
        else
            std::cout << "recv: "<< data_str.size() << " bytes: " << data_str << std::endl;

    }

    /* if connection failed close socket and remove it from Map */
    /* thread is detach -> after end of func its closed automaticaly */
    close(it->second);
    std::cout << "socket " << it->first << " closed" << std::endl;
    
    this->socket_map.erase(it);
    std::cout << "socket " << it->first << " erised from map" << std::endl;
    return;
}


std::string TcpServer::recv_data(int sock)
{

    std::vector<char> data; // char vector to collect data

    char * recv_buffer; // dynamic buffer to socket func recv()
    char len_buffer[6]; // buffer to collect data size

    int len_recvd = 0; // length of recievd data
    int len_to_recv = 0; // total length to reciev
    int to_get = this->BUFFERSIZE; // data length to reciev for one iteration
    int bytes_read = 0; // return of socket func recv() -> show length of recievd data (if 0 -> socket connection failed)

        /* recv length of parcel. 
            if 0 -> connection failed -> return zero string
        */
        bytes_read = recv(sock, len_buffer, 6, 0);
        if(bytes_read <= 0)
        {
            std::cout << "recv failed 1" << std::endl;
            return "";
        }
        sscanf(len_buffer, "%d", &len_to_recv); // translate char* to int

        data.reserve(len_to_recv); // reserve vector size equal parcel size (to optimize memory operations)

        /* recv parcel by parts of 1024 bytes
            if size < 1024 reciev all parcel
        */
        while(len_recvd < len_to_recv)
        {
            len_recvd += BUFFERSIZE;
            if(len_recvd >= len_to_recv)
            {
                to_get = this->BUFFERSIZE - (len_recvd - len_to_recv);
            }
            else to_get = this->BUFFERSIZE;

            recv_buffer = new char[to_get];
            
            bytes_read = recv(sock, recv_buffer, to_get, 0);
            if(bytes_read <= 0)
            {
                std::cout << "recv failed 2" << std::endl;
                return "";
            }
            
            data.insert(data.end(), recv_buffer, recv_buffer+to_get); // add recvd piece to data

            delete[] recv_buffer;  // clear buffer
        }

        std::string str_data = std::string(data.begin(), data.end()); // converce std::vector to std::string
        return str_data;    
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