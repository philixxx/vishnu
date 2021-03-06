/**
 * \file NetrcReader.hpp
 * \brief This file contains the netrc file reader class.
 * \author Daouda Traore (daouda.traore@sysfera.com)
 * and Eugene PAMBA CAPO-CHICHI (eugene.capochichi@sysfera.com)
 * \date December 2011
 */

#include "NetrcReader.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sys/stat.h>

#include "UserException.hpp"

using namespace std;


/**
  * \brief Constructor
  */
NetrcReader::NetrcReader() {
  boost::filesystem::path p(getenv("HOME"));
  p /= ".netrc";
  mpath = p.string();
}


/**
  * \param path  The path until the netrc file
  * \brief Constructor
  */
NetrcReader::NetrcReader(const string& path) : mpath(path) {}

/**
  * \brief Function to read the netrc file for getting the login and the password for
  * a specific machine
  * \param auth all the couples login/password returned which is associated to the machine
  * \param machineName the name of the machine defined on the netrc file
  * \return the corresponding couple (login, password)
  */
void
NetrcReader::read(map<size_t, pair<string,string> > &auth, const string& machineName) {
  this->check();
  auth = this->getNetrcInfo(machineName);
}


/**
 * \brief Destructor
 */
NetrcReader::~NetrcReader() {}

/**
  * \brief Function to get value associated to a key
  * \param key the key associated to a value on the file
  * \param line the line where the value of the key is searched
  * \param tab a table which allows to register the couple key value
  * \param machine_pos
  * \return true if the value has found else found
  */
bool
NetrcReader::searchValueFromKey(const string& key,
                                string& value,
                                vector<string>& tokens,
                                map<size_t, pair<string, string> >& tab,
                                const size_t machine_pos) {
  vector<string>::iterator beg = tokens.begin();
  vector<string>::iterator end = tokens.end();
  size_t size = end - beg;

  if (machine_pos < size) {
    vector<string>::iterator iter = find(beg + machine_pos, end, key);
    value = "";

    if (iter != end) {
      vector<string>::iterator keyIter = iter;
      size_t keyPos = keyIter - beg;
      ++iter;
      if (iter != end) {
        value = *iter;
      }
      tab[keyPos] = make_pair<string, string>(key, value);
      return true;
    }
  }
  return false;
}


/**
  * \brief Function to analyze the couple key and value table
  * \param tab a table in which the couple key and value are registered
  * \param machineName the machine name used for searching the couple
  * \return The analyszed tab
  */
map<size_t, pair<string,string> >&
NetrcReader::analyze(map<size_t, pair<string,string> >& tab,
                     const string& machineName) {
  static map<size_t, pair<string,string> > analyzedTab;
  map<size_t, pair<string,string> >::iterator iter;
  map<size_t, pair<string,string> >::iterator end = tab.end();
  bool machineNameIsNotFound = true;

  for(iter = tab.begin(); iter != tab.end(); ++iter) {
    if (iter->second == pair<string, string> ("machine", machineName)) {
      if (machineNameIsNotFound) {
        machineNameIsNotFound = false;
      }
      ++iter; //to go to the next element
      if (iter == end  || (iter->second).first.compare("login") != 0) {
        throw UserException(ERRCODE_INVALID_PARAM,
            "The login is undefined. The password must follow the login");
      }
      string login = (iter->second).second;
      analyzedTab[iter->first] = pair<string, string>("login", login);
      ++iter; //go to to the next element
      if (iter == end  || (iter->second).first.compare("password") != 0) {
        throw UserException(ERRCODE_INVALID_PARAM, "The password is undefined");
      }
      string password = (iter->second).second;
      analyzedTab[iter->first] = pair<string, string>("password", password);
    }
  }
  //if the machine name is not found
  if (machineNameIsNotFound) {
     throw UserException(ERRCODE_INVALID_PARAM, "The machine " + machineName + " is undefined");
  }

  return analyzedTab;
}

/**
  * \brief Function to check the validity of the netrc file
  * \return raises an exception on error
  */
void
NetrcReader::check() {
  struct stat fileStat;
  boost::filesystem::path file(mpath);

  // Check the existence of file
  if (!boost::filesystem::exists(file)) {
    throw UserException(ERRCODE_INVALID_PARAM, "The file " + mpath + " does not exist");
  }

  // Check the regularity of the file
  if (!boost::filesystem::is_regular_file(file)) {
    throw UserException(ERRCODE_INVALID_PARAM, "The file " + mpath + " is not a regular file");
  }

  // Check if the file is empty
  if (boost::filesystem::is_empty(file)) {
    throw UserException(ERRCODE_INVALID_PARAM, "The file " + mpath + " is empty");
  }

  // Check permissions
  if (stat(mpath.c_str(), &fileStat)) {
    throw UserException(ERRCODE_INVALID_PARAM,
                        "There is problems to get the permissions of the file: " + mpath);
  }

  ostringstream out;
  out << oct << fileStat.st_mode;

  if (out.str() != "100600") {
      throw UserException(ERRCODE_INVALID_PARAM,
                        "The permissions of the file: " + mpath + " should be 600");
  }
}

/**
  * \brief Function to get the login and the password associated to a specific machine
  * by parsing the netrc file
  * \param machineName the name of the machine
  * \return the corresponding couple (login, password)
  */
map<size_t, pair<string,string> >&
NetrcReader::getNetrcInfo(const string& machineName) {
  ifstream infile;
  string line;
  string machineValue;
  string loginValue;
  string passwordValue;

  infile.open (mpath.c_str());

  static map<size_t, pair<string,string> > tab;
  string fileContent;

  if (infile.is_open()) {
    while (!infile.eof()) {
      getline(infile, line);
      fileContent = fileContent +  " " + line;
    }
    infile.close();

    vector<string> tokens;
    istringstream iss(fileContent);
    copy(istream_iterator<string>(iss),
              istream_iterator<string>(),
              back_inserter<vector<string> >(tokens));

    bool machineTest = false;
    bool loginTest = false;
    bool passwordTest = false;
    size_t tokensize = tokens.size();
    size_t machinePos = 0;
    vector<string>::iterator iter;
    vector<string>::iterator beg = tokens.begin();
    vector<string>::iterator end = tokens.end();

    while (machinePos < tokensize) {
      machineTest = searchValueFromKey("machine", machineValue, tokens, tab, machinePos);
      iter = find(beg + machinePos, end, "machine");
      if (machineValue.empty()) {
        machinePos = (iter - beg) + 1;
      } else {
        machinePos = (iter - beg) + 2;
      }

      loginTest = searchValueFromKey("login", loginValue, tokens, tab, machinePos);
      iter = find(beg + machinePos, end, "login");
      size_t tmp = iter - beg;
      if (tmp <= machinePos) {
        if (loginValue.empty()) {
          machinePos += 1;
        } else {
          machinePos += 2;
        }
      }

      passwordTest = searchValueFromKey("password", passwordValue, tokens, tab, machinePos);
      if (!machineTest && !loginTest && !passwordTest) {
        break;
      }
    }

    return analyze(tab, machineName);
  } else {
    throw UserException(ERRCODE_INVALID_PARAM,
                        "The file: " + mpath + " can not be opened");
  }
}
