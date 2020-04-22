#include "mysql.h"

std::vector<DataStruct> mysql_select(void)
{
	
  std::vector<DataStruct> result;
  DataStruct data;

try {
  sql::Driver *driver;
  sql::Connection *con;
  sql::Statement *stmt;
  sql::ResultSet *res;



  /* Create a connection */
  driver = get_driver_instance();
  con = driver->connect("tcp://127.0.0.1:3306", "alarmdaemon", "66062pass");
  /* Connect to the MySQL test database */
  con->setSchema("alarm_base");

  stmt = con->createStatement();
  res = stmt->executeQuery("SELECT * FROM data");
  while (res->next()) {

    /* Access column data by alias or column name */
 	data.No = res->getString("No");
 	data.Id = res->getString("Id");
 	data.Account = res->getString("Account");
 	data.UtcTime = res->getString("UtcTime");
 	data.Picture = res->getString("Picture");

	result.push_back(data);   
    /* Access column data by numeric offset, 1 is the first column 
    cout << res->getString(3) << endl; */
  }

  delete res;
  delete stmt;
  delete con;

} catch (sql::SQLException &e) {
  std::cout << "# ERR: SQLException in " << __FILE__;
  std::cout << "(" << __FUNCTION__ << ") on line "
     << __LINE__ << std::endl;
  std::cout << "# ERR: " << e.what();
  std::cout << " (MySQL error code: " << e.getErrorCode();
  std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
}

	return result;
}


int mysql_insert(DataStruct Data)
{
  sql::Driver *driver;
  sql::Connection *con;
  sql::PreparedStatement  *prep_stmt;

  /* Create a connection */
  driver = get_driver_instance();
  con = driver->connect("tcp://127.0.0.1:3306", "alarmdaemon", "66062pass");
  /* Connect to the MySQL test database */
  con->setSchema("alarm_base");

  prep_stmt = con->prepareStatement("INSERT INTO data (Id, Account, UtcTime, Picture) VALUES (?, ?, ?, ?)");

  prep_stmt->setString(1, Data.Id);
  prep_stmt->setString(2, Data.Account);
  prep_stmt->setString(3, Data.UtcTime);
  prep_stmt->setString(4, Data.Picture);
  prep_stmt->execute();


  delete prep_stmt;
  delete con;
  
  return 1;
}
