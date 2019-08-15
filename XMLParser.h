#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "tinyxml.h"
#include "DataStruct.h"

#ifndef __XMLPARSERH__
#define __XMLPARSERH__

class XMLParser
{
public:
	//XMLParser();
	XMLParser(const char * fname);
	//~XMLParser();

	int loadxml(void);

	std::map<std::string, std::string> Parse_to_map(void);
	DataStruct Parse_to_struct(void);

private:
	TiXmlDocument * XMLdoc;
	std::map<std::string, std::string> Data;
};

#endif