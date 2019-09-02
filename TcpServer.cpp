#include "TcpServer.h"


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
        //char id_buffer[3] = "";
        //char len_buffer[6] = "";
        //int bytes_read = 0;
        std::string init_data = "";

        int sock = accept(this->listener, NULL, NULL);
        if(sock < 0)
        {
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
            std::cout << "Opi connected in socket: "<< sock << std::endl;
            std::string id = recv_data(sock);
            if(id.empty())
                continue;
            std::cout << "Id: " << id << std::endl;
            this->socket_map.insert( std::pair<std::string,int>(id, sock) );
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

            /* SQL SELECT ALL */
            /*
            std::vector<DataStruct> sql_request;
            sql_request = mysql_select(); // Запрос
    
            std::cout << "MySQL info: " << std::endl;
            for (int i = 0; i < static_cast<int>(sql_request.size()); ++i)
            {
                std::cout << sql_request[i].No << "\t|\t" << sql_request[i].Id << "\t|\t" << sql_request[i].Account 
                    << "\t|\t" << sql_request[i].UtcTime << "\t|\t" << sql_request[i].Picture << std::endl;
            }
            std::cout << std::endl;
            */
        }

        /* collect and save image */
        else if(data_str == "image")
        {
            std::string str = this->recv_data(it->second);
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

void TcpServer::ClientSocket(std::list<int>::iterator it)
{
    std::string data_str;
    std::cout << "thread started" << std::endl;
    while(1)
    {
        data_str = this->recv_data(*it);
        if(data_str.empty())
            break;

        std::cout << data_str << std::endl;

        if(data_str == "update")
        {
            for( std::map <std::string, int> :: const_iterator itt = this->socket_map.begin(); itt != this->socket_map.end(); ++itt )
            {
                std::cout <<itt->first <<":"<< itt->second <<";"<< std::endl;
            }
        }

    }

    close(*it);
    this->client_list.erase(it);
    std::cout << "socket: " << *it << " closed" << std::endl;


}


/*
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

*/

std::string TcpServer::recv_data(int sock)
{
        char len_buffer[6]; // buffer to collect data size
        char * recv_buffer;
        int len_to_recv = 0;
        int bytes_read = 0;

        bytes_read = recv(sock, len_buffer, 6, MSG_WAITALL);
        if(bytes_read <= 0)
        {
            std::cout << "recv failed 1" << std::endl;
            return "";
        }
        sscanf(len_buffer, "%d", &len_to_recv); // translate char* to int


        recv_buffer = new char[len_to_recv];
            
        bytes_read = recv(sock, recv_buffer, len_to_recv, MSG_WAITALL);
        if(bytes_read <= 0)
        {
            std::cout << "recv failed 1" << std::endl;
            return "";
        }

        std::string str_data = std::string(recv_buffer, len_to_recv);
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