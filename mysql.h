#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <vector>
#include <string>
#include "DataStruct.h"

#ifndef __MYSQLH__
#define __MYSQLH__

std::vector<DataStruct> mysql_select(void);
int mysql_insert(DataStruct data);

#endif