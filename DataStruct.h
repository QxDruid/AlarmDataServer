#include <string>
#include <iostream>

#ifndef __DATASTRUCT__
#define __DATASTRUCT__


struct DataStruct
{
	std::string No;
	std::string Id;
	std::string Account;
	std::string UtcTime;
	std::string Picture;
	std::string PropertyOperation;
	std::string Result;
	std::string xmlns;

	void get_data(void)
	{
		std::cout << "DataStruct info: " << std::endl << Id << "\t|\t" <<  Account << "\t|\t" << UtcTime << "\t|\t" << Picture << std::endl << std::endl;
	}
};

#endif