#include <stdlib.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "TcpServer.h"
#include "tinyxml.h"
#include "XMLParser.h"
#include "DataStruct.h"
#include "mysql.h"



int main(int argc, char const *argv[])
{
	/*
	std::cout << std::endl;
	std::string fname = "test.xml";

/* Load file to parse 
	XMLParser xml(fname.c_str());
	if(!xml.loadxml())
		return 0;


/* create Struct for result 
	DataStruct result;
/* parse doc 
	result = xml.Parse_to_struct();
/* print result 
	result.get_data();

/* SQL SELECT ALL 

  	mysql_insert(result);


  	std::vector<DataStruct> sql_request;
	sql_request = mysql_select(); // Запрос
	
	std::cout << "MySQL info: " << std::endl;
	for (int i = 0; i < sql_request.size(); ++i)
	{
		std::cout << sql_request[i].No << "\t|\t" << sql_request[i].Id << "\t|\t" << sql_request[i].Account 
					<< "\t|\t" << sql_request[i].UtcTime << "\t|\t" << sql_request[i].Picture << std::endl;
	}
	std::cout << std::endl;
	
*/
	TcpServer serv;
	serv.start();
	std::cout << "----------------------end of prog server failed----------------------" << std::endl;




	return 1;
}

