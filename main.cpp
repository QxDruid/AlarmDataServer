/* подключение типов данных и тп */
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iomanip> 
#include <iostream>
#include <map>
#include <vector>

/* Подключение библиотек для демонизации */
#include <fcntl.h> 
#include <errno.h> 
#include <unistd.h> 
#include <syslog.h> 
#include <sys/types.h> 
#include <sys/stat.h> 

/* подключение моих библиотек */
#include "TcpServer.h"
#include "tinyxml.h"
#include "XMLParser.h"
#include "DataStruct.h"
#include "mysql.h"

#define PIDFILE "/var/run/alarmserver.pid"

void savepid()
{
	std::ofstream pidf;
	pidf.open(PIDFILE,std::ios_base::out);
	pidf << getpid();
	pidf.close();
}

int log_it(std::string data)
{
	std::ofstream log;
	log.open("log.txt",std::ios_base::out);
	log << data << std::endl;
	log.close();
}

int WorkProc(void)
{
	TcpServer serv;
	serv.start();
}


int main(int argc, char const *argv[])
{
	pid_t pid, sid;

/* Ответвляемся от родительского процесса */
pid = fork();
if(pid < 0)
{
	log_it("fork failed");
	exit(EXIT_FAILURE);
}
if (pid > 0)
{
	log_it("fork success");
	exit(EXIT_SUCCESS);
}


/* Изменяем файловую маску */
umask(0);
/* Создание нового SID для дочернего процесса */
sid = setsid();
if (sid < 0) {
  log_it("sid failure");
  exit(EXIT_FAILURE);
}

// сохраняем pid нашего процесса чтоб потом управлять им
savepid();

/* Изменяем текущий рабочий каталог */
if ((chdir("/")) < 0) {
  log_it("chdir failure");
  exit(EXIT_FAILURE);
}

/* Закрываем стандартные файловые дескрипторы 
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);
*/

	/* запуск рабочепй программы */
	WorkProc();

	exit(EXIT_SUCCESS);
}
