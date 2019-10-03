#include "TcpServer.h"
#include <errno.h>
#include <syslog.h>

int log_(std::string data)
{
    std::ofstream log;
    log.open("logServer.txt",std::ios_base::app);
    log << data << std::endl;
    log.close();
}


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
            log_("listen socket not created");
            std::cout << "listen socket not created" << std::endl;
            return 0;
        }
        int opt = 1;
        if (setsockopt (this->listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) == -1) 
            perror("setsockopt");
        if(bind(this->listener, (struct sockaddr *)&this->addr, sizeof(this->addr)) < 0)
        {
            log_("listener bind error");
            std::cout << "listener bind error" << strerror(errno) << std::endl;
            return 0;
        }


    listen(this->listener, 10);
    while(1)
    {
        //char id_buffer[3] = "";
        //char len_buffer[6] = "";
        //int bytes_read = 0;
        std::string init_data = "";

        int sock = accept(this->listener, NULL, NULL);
        if(sock < 0)
        {
            log_("connection accept error");
            std::cout << "connection accept error" << std::endl;
            return 0;
        }

        init_data = recv_data(sock);
        if(init_data.empty())
                continue;
        
        //bytes_read = recv(sock, len_buffer, 6, 0);
        //if(bytes_read <= 0)
            //continue;
        //bytes_read = recv(sock, id_buffer, 3, 0);
        //if(bytes_read <= 0)
            //continue;
        if(init_data == "Opi")
        {
            log_("Opi connected in socket: ");
            std::cout << "Opi connected in socket: "<< sock << std::endl;
            std::string id = recv_data(sock);
            if(id.empty())
                continue;
            std::cout << "Id: " << id << std::endl;

            std::string acc = recv_data(sock);
            if(id.empty())
                continue;
            std::cout << "account: " << acc << std::endl;

            this->socket_map.insert( std::pair<std::string,Opi_data>(id, Opi_data(sock,acc)) );

            it = socket_map.find(id);
            std::thread data_recv_thread(&TcpServer::OpiSocket, this, it);
            data_recv_thread.detach();
        }
        else if(init_data == "Client")
        {
            std::cout << "Client connected: " << sock << std::endl;
            this->client_list.push_back(sock);
            std::thread data_recv_thread(&TcpServer::ClientSocket, this, --client_list.end());
            std::cout << *(--this->client_list.end()) << std::endl;
            data_recv_thread.detach();
        }
        else
        {
            std::cout << "Someone connected in socket: "<< sock << std::endl;
            std::cout << "False init packet" << std::endl;
        }
        
        /* Проверка налличия данных, если данных нет то поток не создаем и слушаем дальше */
        
/*
        std::string id = std::string(id_buffer,3);
        std::cout << "Id: " << id << std::endl;
        this->socket_map.insert( std::pair<std::string,int>(id, sock) );
        it = socket_map.find(id);
        std::thread data_recv_thread(&TcpServer::OpiSocket, this, it);
        data_recv_thread.detach();
        */
    }

    return 1;
}


void TcpServer::OpiSocket(std::map <std::string, Opi_data> :: iterator it) // --------------work true--------------------
{

    std::string data_str;
    std::string fname;
    std::string path = DATAPATH;
    std::cout << "thread for Id " << it->first << " started" << std::endl;
    DataStruct parsed_data;

    while(1)
    {
        /* recv name of data */
       data_str = this->recv_data(it->second.socket);
       if(data_str.empty())
            break;

        /* if get xml recv file name and xml data
            save file
            parce file and save data to database
        */
        if(data_str == "xml")
        {
            data_str = this->recv_data(it->second.socket);
            if(data_str.empty())
                break;
            fname = data_str;

            data_str = this->recv_data(it->second.socket);
            if(data_str.empty())
                break;

            std::ofstream xml_file(path + fname);
            if (!xml_file.is_open()) // если файл небыл открыт
            {
                std::cout << "xml_file не может быть открыт или создан" << std::endl; // напечатать соответствующее сообщение
                return; // выполнить выход из программы
            }
            xml_file << data_str;
            xml_file.close();


            XMLParser xml((path + fname).c_str());
            if(!xml.loadxml())
            {
                std::cout << "XML open fail" << std::endl;
                break;
            }
            parsed_data = xml.Parse_to_struct();
            
            /* SQL INSERT XML DATA */
            mysql_insert(parsed_data);

        }

        /* collect and save image */
        else if(data_str == "image")
        {
            std::string send_str;
            std::stringstream ss;
            std::string str = this->recv_data(it->second.socket);
            if(str.empty())
                break;

            std::ofstream img_file(path + parsed_data.Picture);
            if (!img_file.is_open()) // если файл небыл открыт
            {
                std::cout << "img_file не может быть открыт или создан" << std::endl; // напечатать соответствующее сообщение
                return; // выполнить выход из программы
            }
            img_file << str;
            img_file.close();


            ss << "alarm" << ";" << "1" << ":" << parsed_data.Id << ":" <<  parsed_data.Account << ":" <<  parsed_data.UtcTime << ":" <<  parsed_data.Picture;
            send_str = ss.str();
            std::cout << "ALARM: " << send_str << std::endl;
            ss.str("");
            for( std::list <int> :: const_iterator it_ = this->client_list.begin(); it_ != this->client_list.end(); ++it_ )
            {
               this->send_data(*it_, send_str.c_str(), send_str.length());
            }


        }

        /* Ping */
        if(data_str == "ping")
        {
            /* send ping accept messege */
        }

        else
            std::cout << "recv: "<< data_str.size() << " bytes: " << data_str << std::endl;

    }

    /* if connection failed close socket and remove it from Map */
    /* thread is detach -> after end of func its closed automaticaly */
    close(it->second.socket);
    std::cout << "socket " << it->first << " closed" << std::endl;
    
    
    std::cout << "socket " << it->first << " erised from map" << std::endl;
    this->socket_map.erase(it);
    return;
}

void TcpServer::ClientSocket(std::list<int>::iterator it)
{
    std::string data_str;
    std::string output_str;
    std::stringstream output_ss;
    std::cout << "thread started" << std::endl;
    char len_buffer[6];
    while(1)
    {
        data_str = this->recv_data(*it);
        std::cout << data_str << std::endl;

        if(data_str.empty())
            break;

        if(data_str == "update")
        {
            output_ss << "update" << ";" << this->socket_map.size() << ";";
            for( std::map <std::string, Opi_data>:: const_iterator itt = this->socket_map.begin(); itt != this->socket_map.end(); ++itt )
            {
               output_ss << itt->first << ":" <<  itt->second.account << ":" <<  "Online" << ";";
            }

            output_str = output_ss.str();

            this->send_data(*it,output_str.c_str(),output_str.length());
            std::cout << "UPDATE: " << output_str << std::endl;
            output_ss.str("");
        }

    }

    close(*it);
    this->client_list.erase(it);
    std::cout << "socket: " << *it << " closed" << std::endl;


}


std::string TcpServer::recv_data(int sock)
{
        char len_buffer[6]; // buffer to collect data size
        char * recv_buffer;
        int len_to_recv = 0;
        int bytes_read = 0;

        bytes_read = recv(sock, len_buffer, 6, MSG_WAITALL);
        if(bytes_read <= 0)
        {
            std::cout << "data size recv failed" << std::endl;
            return "";
        }
        sscanf(len_buffer, "%d", &len_to_recv); // translate char* to int

        recv_buffer = new char[len_to_recv];
            
        bytes_read = recv(sock, recv_buffer, len_to_recv, MSG_WAITALL);
        if(bytes_read <= 0)
        {
            std::cout << "data recv failed" << std::endl;
            return "";
        }

        std::string str_data = std::string(recv_buffer, len_to_recv);

        this->send_data(sock, "Ok", 2);
        return str_data;
}


TcpServer::~TcpServer() {

}


void TcpServer::send_data(int sock, const char * data, int len)
{
    char buff[6];

    sprintf(buff, "%06d", len);
    if(send(sock, buff, 6, 0) <= 0)
    {
        std::cout << "send len error" << std::endl;
        return;
    }
    if(send(sock, data, len, 0) <= 0)
    {
        std::cout << "send data error" << std::endl;
        return;
    }
    return;
}
