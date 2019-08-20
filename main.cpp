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

	TcpServer serv;
	serv.start();
	std::cout << "----------------------end of prog server failed----------------------" << std::endl;

	return 1;
}

