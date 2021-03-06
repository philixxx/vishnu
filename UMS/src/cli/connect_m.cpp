/**
 * \file connect_m.cpp
 * This file defines the VISHNU connect command
 * \author Ibrahima Cisse (ibrahima.cisse@sysfera.com)
 */
#include <stdlib.h>                     // for getenv
#include <unistd.h>                     // for getppid
#include <boost/bind/arg.hpp>           // for arg
#include <boost/bind.hpp>          // for bind_t, bind
#include <boost/function.hpp>  // for function1
#include <boost/lexical_cast.hpp>       // for lexical_cast
#include <boost/ref.hpp>                // for reference_wrapper, ref
#include <boost/smart_ptr/shared_ptr.hpp>  // for shared_ptr
#include <exception>                    // for exception
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <sstream>                      // for basic_stringbuf<>::int_type, etc
#include <stdexcept>                    // for out_of_range
#include <string>                       // for string, allocator, etc

#include "ConnectOptions.hpp"           // for ConnectOptions
#include "ListUsers.hpp"                // for ListUsers
#include "Options.hpp"                  // for ::CONFIG, Options, ::ENV
#include "Session.hpp"                  // for Session
#include "UMSVishnuException.hpp"       // for ERRCODE_TEMPORARY_PASSWORD
#include "UMS_DataFactory.hpp"          // for UMS_DataFactory
#include "User.hpp"                     // for User
#include "UserException.hpp"            // for ERRCODE_CLI_ERROR_DIET, etc
#include "VishnuException.hpp"          // for VishnuException
#include "api_ums.hpp"                  // for connect, vishnuInitialize
#include "cliUtil.hpp"                  // for errorUsage, helpUsage, etc
#include "connectUtils.hpp"             // for makeConnectOptions
#include "daemon_cleaner.hpp"           // for cleaner
#include "ecore_forward.hpp"            // for EInt
#include "ecorecpp/mapping/EList.hpp"   // for EList
#include "sessionUtils.hpp"             // for storeLastSession

using namespace std;
using namespace vishnu;


bool isHelp(string s){
  return (s.size()>1 && s.at(0)=='-' && s.at(1)=='h');
}

bool isSubstitute(string s){
  return (s.size()>1 && s.at(0)=='-' && s.at(1)=='s');
}

bool isPolicy(string s){
  return (s.size()>1 && s.at(0)=='-' && s.at(1)=='p');
}

bool isDelay(string s){
  return (s.size()>1 && s.at(0)=='-' && s.at(1)=='d');
}



int main (int ac, char* av[]){

  int counter;

  /******* Parsed value containers ****************/

  string configFile;

  /********** EMF Data ****************************/

  UMS_Data::ConnectOptions connectOpt;
  UMS_Data::UMS_DataFactory_ptr ecoreFactory = UMS_Data::UMS_DataFactory::_instance();

  /*********** In parameters **********************/

  UMS_Data::ListUsers listUsers;

  /*********** Out parameters *********************/
  std::string userId;
  boost::function1<void,UMS_Data::SessionCloseType> fClosePolicy( boost::bind(&UMS_Data::ConnectOptions::setClosePolicy,boost::ref(connectOpt),_1));
  boost::function1<void,int> fSessionInactivityDelay( boost::bind(&UMS_Data::ConnectOptions::setSessionInactivityDelay,boost::ref(connectOpt),_1));
  boost::function1<void,string> fSubstituteUserId( boost::bind(&UMS_Data::ConnectOptions::setSubstituteUserId,boost::ref(connectOpt),_1));

  boost::shared_ptr<Options> opt=makeConnectOptions(av[0],userId,0,configFile,CONFIG);
  opt->add("sessionInactivityDelay,d", "The session inactivity delay",CONFIG,fSessionInactivityDelay);


  opt->add("substituteUserId,s","The substitute user identifier",CONFIG,fSubstituteUserId);

  opt->add("closePolicy,p","for closing session automatically",ENV,fClosePolicy );


  UMS_Data::Session session;

  configFile = string(getenv("VISHNU_CONFIG_FILE"));


  /********* Get all the couples user/pwd ******************/
  counter = 1;
  while (counter<ac) {
    string tmp(av[counter]);
    try {
// Checking token
      if (isPolicy(tmp)) {
        if (tmp.size()>2) {
          counter++;
          UMS_Data::SessionCloseType type(boost::lexical_cast<int>(tmp.substr(2)));
          connectOpt.setClosePolicy(type);
        } else {
          UMS_Data::SessionCloseType type(boost::lexical_cast<int>(av[counter+1]));
          connectOpt.setClosePolicy(type);
          counter +=2;
        }
      } // end if close policy
      else if (isDelay(tmp)) {
        if (tmp.size()>2) {
          counter++;
          connectOpt.setSessionInactivityDelay(boost::lexical_cast<int>(tmp.substr(2)));
        } else {
          connectOpt.setSessionInactivityDelay(boost::lexical_cast<int>(av[counter+1]));
          counter +=2;
        }
      } // end if delay
      else if (isSubstitute(tmp)) {
        if (tmp.size()>2) {
          counter++;
          connectOpt.setSubstituteUserId(tmp.substr(2));
        } else {
          connectOpt.setSubstituteUserId(string(av[counter+1]));
          counter +=2;
        }
      } // end if substitute
      else if (isHelp(tmp)){
        counter++;
        helpUsage(*opt);
        return 0;
      } else { // is a couple userid followed by password
        // If the second component isn't present
        if (counter+1>=ac) {
          helpUsage(*opt);
          return ERRCODE_CLI_ERROR_MISSING_PARAMETER;
        }
        UMS_Data::User_ptr user = ecoreFactory->createUser();
        user->setUserId(tmp);
        user->setPassword(string(av[counter+1]));
        listUsers.getUsers().push_back(user);
        counter +=2 ;
      }
    } catch (std::out_of_range &e){
    }


  }// End while


    /************** Call UMS connect service *******************************/
  try{

    cleaner(const_cast<char*>(configFile.c_str()), ac, av);// lauch the daemon cleaner if it is not already running

    // initializing vishnu
    if (vishnuInitialize(const_cast<char*>(configFile.c_str()), ac, av)) {

      errorUsage(av[0],communicationErrorMsg,EXECERROR);

      return  ERRCODE_CLI_ERROR_COMMUNICATION ;
    }

    connect(listUsers, session, connectOpt);// call the api extern connect service to get a session key

    storeLastSession(session,getppid()); // store sessionKey into $HOME/.vishnu/sessions
    std::cout << "sessionId: " << session.getSessionId() << "\n";
  }  // End of try bloc


//  catch(po::required_option& e){// a required parameter is missing
//
//    usage(*opt,"[options] ",REQUIREDPARAMMSG);
//
//    return ERRCODE_CLI_ERROR_MISSING_PARAMETER;
//  }
//  // {{RELAX<CODEREDUCER> The error handling is the same in all command
//
//  catch(po::error& e){ // catch all other bad parameter errors
//    errorUsage(av[0], e.what());
//
//    return ERRCODE_INVALID_PARAM;
//  }

  catch(VishnuException& e){// catch all Vishnu runtime error

    // handle the temporary password
    if(ERRCODE_TEMPORARY_PASSWORD==e.getMsgI()){

      errorUsage(av[0],"Your password is temporary, use vishnu_change_password command to change it",EXECERROR);
    }

    else{

      std::string  msg = e.getMsg()+" ["+e.getMsgComp()+"]";

      errorUsage(av[0],msg,EXECERROR);
    }

    return e.getMsgI() ;

  }

  catch(std::exception& e){ // catch all std runtime error

    errorUsage(av[0], e.what());

    return ERRCODE_CLI_ERROR_RUNTIME;
  }

  return 0;

// }}RELAX<CODEREDUCER>

}// end of main
