#include "XMLParser.h"


/* init filename to parse */
XMLParser::XMLParser(const char * fname)
{
	this->XMLdoc = new TiXmlDocument(fname);
}




/* Load file */
int XMLParser::loadxml(void)
{
	if(!this->XMLdoc->LoadFile())
		return 0;
	return 1;
}

/*Parse xml file and push result to std::map */
std::map<std::string, std::string> XMLParser::Parse_to_map(void)
{
	TiXmlElement *pSource, *pSimpleItem, *pSimpleItem2, *pData ;

	/* Read 
	<tt:Message UtcTime=" 2019-Jul-02T10-09-50"	PropertyOperation="Initialized" xmlns:tt="http://www.onvif.org/ver10/schema"> */

	TiXmlElement *pMessage = XMLdoc->FirstChildElement( "tt:Message" ); // get field

		this->Data.insert(std::pair<std::string, std::string>("UtcTime", pMessage->Attribute("UtcTime")));
		this->Data.insert(std::pair<std::string, std::string>("PropertyOperation", pMessage->Attribute("PropertyOperation")));
		this->Data.insert(std::pair<std::string, std::string>("xmlns:tt", pMessage->Attribute("xmlns:tt")));


	/* Read 
		<tt:Source>
			<tt:SimpleItem Name="Id" Value="66011" />
		</tt:Source> 
	*/
	pSource = pMessage->FirstChildElement( "tt:Source" ); // it's the child of tt:Message
	pSimpleItem = pSource->FirstChildElement( "tt:SimpleItem" ); // it's the child of tt:Source
	while(pSimpleItem)
	{
		this->Data.insert(std::pair<std::string, std::string>(pSimpleItem->Attribute("Name"), pSimpleItem->Attribute("Value")));


		pSimpleItem = pSimpleItem->NextSiblingElement("tt:SimpleItem"); // go to next field with name tt:message (it onse)

	}

	/*	Read
		<tt:Data>
			<tt:SimpleItem Name="Account" Value="admin" />
			<tt:SimpleItem Name="Picture" Value="2019-Jul-02T10-09-50.jpg" />
			<tt:SimpleItem Name="Result" Value="detected" />
		</tt:Data>
	*/
	pData = pMessage->FirstChildElement( "tt:Data" ); 
	pSimpleItem2 = pData->FirstChildElement( "tt:SimpleItem" ); // it's the child of tt:Source
	while(pSimpleItem2)
	{
		this->Data.insert(std::pair<std::string, std::string>(pSimpleItem2->Attribute("Name"), pSimpleItem2->Attribute("Value")));
		pSimpleItem2 = pSimpleItem2->NextSiblingElement("tt:SimpleItem"); // go to next field with name tt:message (it onse)
	}

	return this->Data;
}


DataStruct XMLParser::Parse_to_struct(void)
{
		TiXmlElement *pSource, *pSimpleItem, *pSimpleItem2, *pData ;

		DataStruct result;
	/* Read 
	<tt:Message UtcTime=" 2019-Jul-02T10-09-50"	PropertyOperation="Initialized" xmlns:tt="http://www.onvif.org/ver10/schema"> */

	TiXmlElement *pMessage = XMLdoc->FirstChildElement( "tt:Message" ); // get field

		result.UtcTime =  pMessage->Attribute("UtcTime");
		result.PropertyOperation = pMessage->Attribute("PropertyOperation");
		result.xmlns = pMessage->Attribute("xmlns:tt");


	/* Read 
		<tt:Source>
			<tt:SimpleItem Name="Id" Value="66011" />
		</tt:Source> 
	*/
	pSource = pMessage->FirstChildElement( "tt:Source" ); // it's the child of tt:Message
	pSimpleItem = pSource->FirstChildElement( "tt:SimpleItem" ); // it's the child of tt:Source
	while(pSimpleItem)
	{
		result.Id = pSimpleItem->Attribute("Value");
		pSimpleItem = pSimpleItem->NextSiblingElement("tt:SimpleItem"); // go to next field with name tt:message (it onse)

	}

	/*	Read
		<tt:Data>
			<tt:SimpleItem Name="Account" Value="admin" />
			<tt:SimpleItem Name="Picture" Value="2019-Jul-02T10-09-50.jpg" />
			<tt:SimpleItem Name="Result" Value="detected" />
		</tt:Data>
	*/
	pData = pMessage->FirstChildElement( "tt:Data" ); 
	pSimpleItem2 = pData->FirstChildElement( "tt:SimpleItem" ); // it's the child of tt:Source

		result.Account = pSimpleItem2->Attribute("Value");
		pSimpleItem2 = pSimpleItem2->NextSiblingElement("tt:SimpleItem"); // go to next field with name tt:message (it onse)
		result.Picture = pSimpleItem2->Attribute("Value");
		pSimpleItem2 = pSimpleItem2->NextSiblingElement("tt:SimpleItem"); // go to next field with name tt:message (it onse)
		result.Result = pSimpleItem2->Attribute("Value");
		pSimpleItem2 = pSimpleItem2->NextSiblingElement("tt:SimpleItem"); // go to next field with name tt:message (it onse)


	return result;
}