#include "ProcessServer.hpp"
#include "DbFactory.hpp"
#include "DatabaseResult.hpp"
#include "utilServer.hpp"
#include "utilIMS.hpp"

using namespace vishnu;

ProcessServer::ProcessServer(const SessionServer session):msession(session){
  DbFactory factory;
  mcommandName = "vishnu_list_processes";
  mdatabase = factory.getDatabaseInstance();
}

ProcessServer::ProcessServer(IMS_Data::ProcessOp_ptr op, const SessionServer session):mop(op), msession(session){
  DbFactory factory;
  mcommandName = "vishnu_list_processes";
  mdatabase = factory.getDatabaseInstance();
}

ProcessServer::~ProcessServer(){
}

IMS_Data::ListProcesses*
ProcessServer::list(){
  string request = "SELECT * from process WHERE \"pstatus\"='"+convertToString(PRUNNING)+"' ";
  vector<string> results;
  vector<string>::iterator iter;
  
  if (mop->getMachineId().compare("") != 0){
    string machine = "SELECT machineid from machine where machineid='"+mop->getMachineId()+"'";
    DatabaseResult *res = mdatabase->getResult(machine.c_str());
    if(res->getNbTuples()==0) {
       throw UMSVishnuException(ERRCODE_UNKNOWN_MACHINE);
    }
    request += "AND \"machineid\"='"+mop->getMachineId()+"'";
  }

  IMS_Data::IMS_DataFactory_ptr ecoreFactory = IMS_Data::IMS_DataFactory::_instance();
  IMS_Data::ListProcesses_ptr mlistObject = ecoreFactory->createListProcesses();

  try{
    boost::scoped_ptr<DatabaseResult> listOfProcess (mdatabase->getResult(request.c_str()));
    for (size_t i = 0; i < listOfProcess->getNbTuples(); i++){
      results.clear();
      results = listOfProcess->get(i);
      iter  = results.begin();
      IMS_Data::Process_ptr proc = ecoreFactory->createProcess();
      proc->setState(convertToInt(*(iter)));
      proc->setProcessName(*(++iter));
      proc->setDietId(*(++iter));
      proc->setMachineId(*(++iter));
      proc->setTimestamp(convertToInt(*(++iter)));
      proc->setScript(*(++iter));
      mlistObject->getProcess().push_back(proc);
    }
  }catch (SystemException& e){
    throw (e);
  }


  return mlistObject;
}

string
ProcessServer::getCommandName(){
  return mcommandName;
}

int
ProcessServer::connectProcess(IMS_Data::Process_ptr proc){
  string request = "UPDATE \"process\" SET \"pstatus\"='"+convertToString(PRUNNING)+"', \"uptime\"=CURRENT_TIMESTAMP WHERE \"vishnuname\"='"+proc->getProcessName()+"' AND \"machineid\"='"+proc->getMachineId()+"'";
  try{
    mdatabase->process(request.c_str());
  }catch(SystemException& e){
    e.appendMsgComp(" Error connecting the process "+proc->getProcessName()+" on the machine "+proc->getMachineId());
    throw(e);
  }
  return IMS_SUCCESS;
}

int
ProcessServer::disconnectProcess(IMS_Data::Process_ptr proc){
  string request = "UPDATE \"process\" SET \"pstatus\"='"+convertToString(PDOWN)+"', \"uptime\"=CURRENT_TIMESTAMP WHERE \"dietname\"='"+proc->getDietId()+"'";
  try{
    mdatabase->process(request.c_str());
  }catch(SystemException& e){
    e.appendMsgComp(" Error disconnecting the process "+proc->getDietId());
    throw(e);
  }
  return IMS_SUCCESS;
}
int
ProcessServer::authentifyProcess(IMS_Data::Process_ptr proc){
  string request = "UPDATE \"process\" SET \"dietname\"='"+proc->getDietId()+"', \"uptime\"=CURRENT_TIMESTAMP WHERE \"vishnuname\"='"+proc->getProcessName()+"' AND \"machineid\"='"+proc->getMachineId()+"'";
  try{
    mdatabase->process(request.c_str());
  }catch(SystemException& e){
    e.appendMsgComp(" Error authentificating the process"+proc->getProcessName()+" on the machine "+proc->getMachineId());
    throw(e);
  }
  return IMS_SUCCESS;
}

int
ProcessServer::stopProcess(IMS_Data::Process_ptr proc){
  string request = "UPDATE \"process\" SET \"pstatus\"='"+convertToString(PDELETED)+"', \"uptime\"=CURRENT_TIMESTAMP WHERE \"dietname\"='"+proc->getDietId()+"'";
  try{
    mdatabase->process(request.c_str());
  }catch(SystemException& e){
    e.appendMsgComp(" Error stopping the process "+proc->getDietId());
    throw(e);
  }
  return IMS_SUCCESS;
}

