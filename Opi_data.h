#ifndef ___OPIDATASTRUCT___
#define ___OPIDATASTRUCT___


struct Opi_data
{
	Opi_data(int newsocket, std::string newaccount)
         : socket(newsocket), account(newaccount) {}
	int socket;
	std::string account;
};










#endif