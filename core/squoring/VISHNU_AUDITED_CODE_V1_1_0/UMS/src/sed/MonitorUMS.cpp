/**
* \file MonitorUMS.cpp
* \brief This file presents the implementation of the UMS Monitor
* \author Eugène PAMBA CAPO-CHICHI (eugene.capochichi@sysfera.com)
* \date 31/01/2001
*/

#include <boost/scoped_ptr.hpp>
#include "MonitorUMS.hpp"

/**
* \brief Constructor, raises an exception on error
* \fn ServerUMS(std::string cfg)
* \param interval The interval to check the database
*/
MonitorUMS::MonitorUMS(int interval) {
minterval = interval;
mdatabaseVishnu = NULL;
}

/**
* \brief Destructor
* \fn ~MonitorUMS()
*/

MonitorUMS::~MonitorUMS() {
  if (mdatabaseVishnu != NULL) {
    delete mdatabaseVishnu;
  }
}


/**
* \brief To initialize the UMS monitor with individual parameters instead of configuration file
* \fn int init(int vishnuId, int dbType, std::string dbHost, std::string dbUsername, std::string dbPassword)
* \param vishnuId The password of the root user vishnu_user for the connection with the database
* \param dbConfig The configuration of the database
* \return raises an execption
*/
void
MonitorUMS::init(int vishnuId, DbConfiguration dbConfig) {

  DbFactory factory;

  mdatabaseVishnu = factory.createDatabaseInstance(dbConfig);

  std::string sqlCommand("SELECT * FROM vishnu where vishnuid="+convertToString(vishnuId));

  try {
    /*connection to the database*/
    mdatabaseVishnu->connect();

    /* Checking of vishnuid on the database */
    boost::scoped_ptr<DatabaseResult> result(mdatabaseVishnu->getResult(sqlCommand.c_str()));
    if (result->getResults().size() == 0) {
      throw SystemException(ERRCODE_DBERR, "The vishnuid is unrecognized");
    }
  } catch (VishnuException& e) {
    exit(0);
  }

}

/**
* \brief To launch the UMS Monitor
* \fn int run()
* \return raises an exception
*/
int
MonitorUMS::run() {

  std::vector<std::string>::iterator ii;
  std::vector<std::string> tmp;
  SessionServer closer;

  try {
    boost::scoped_ptr<DatabaseResult> result(closer.getSessionToclosebyTimeout());

    if (result->getNbTuples() != 0) {
      for (size_t i = 0; i < result->getNbTuples(); ++i) {
        tmp.clear();
        tmp = result->get(i);

        ii = tmp.begin();
        SessionServer sessionServer (*ii);

        try {
          //closure of the session
          sessionServer.close();
        }
        catch (VishnuException& e) {
          string errorInfo =  e.buildExceptionString();
        }
      }
    }
    sleep(minterval);

  } catch (VishnuException& e) {
    string errorInfo =  e.buildExceptionString();
  }
  return 0;
}