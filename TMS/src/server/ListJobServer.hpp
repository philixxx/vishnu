/**
 * \file ListJobServer.hpp
 * \brief This file contains the VISHNU ListJobServer class.
 * \author Daouda Traore (daouda.traore@sysfera.com) and
 *   Eugène PAMBA CAPO-CHICHI (eugene.capochichi@sysfera.com)
 * \date February 2011
 */
#ifndef _LIST_JOB_SERVER_
#define _LIST_JOB_SERVER_

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "QueryServer.hpp"
#include "SessionServer.hpp"
#include "TMS_Data.hpp"

/**
 * \class ListJobServer
 * \brief ListJobServer class implementation
 */
class ListJobServer: public QueryServer<TMS_Data::ListJobsOptions, TMS_Data::ListJobs> {

public:

  /**
   * \fn ListJobServer(const SessionServer session)
   * \param session The object which encapsulates the session information (ex: identifier of the session)
   * \param machineId The identifier of the machine in which the jobs will be listed
   * \brief Constructor, raises an exception on error
   */
  ListJobServer(const SessionServer session, std::string machineId):
    QueryServer<TMS_Data::ListJobsOptions, TMS_Data::ListJobs>(session) {
   mcommandName = "vishnu_list_jobs";
   mmachineId = machineId;
  }
  /**
   * \fn ListJobServer(const UMS_Data::ListSessionOptions_ptr params,
   *                        const SessionServer& session)
   * \param params The object which encapsulates the information of ListJobServer options
   * \param session The object which encapsulates the session information (ex: identifier of the session)
   * \param machineId The identifier of the machine in which the jobs will be listed
   * \brief Constructor, raises an exception on error
   */
  ListJobServer(TMS_Data::ListJobsOptions_ptr params, const SessionServer& session, std::string machineId):
    QueryServer<TMS_Data::ListJobsOptions, TMS_Data::ListJobs>(params, session) {
    mcommandName = "vishnu_list_jobs";
    mmachineId = machineId;
  }

  /**
   * \brief Function to treat the listJobServer options
   * \fn void processOptions(const UMS_Data::ListSessionOptions_ptr& options,
   *                         std::string& sqlRequest)
   * \param options the object which contains the ListJobServer options values
   * \param sqlRequest the sql data base request
   * \return raises an exception on error
   */
  void
  processOptions(const TMS_Data::ListJobsOptions_ptr& options, std::string& sqlRequest) {

    //To check if the jobId is defined
    if (options->getJobId().size() != 0) {
      //To check if the jobIf exists
      checkJobId(options->getJobId());
      //To add the jobId on the request
      addOptionRequest("jobId", options->getJobId(), sqlRequest);
    }

    //To check if the number of cpu is defined and positive
    if (options->getNbCpu() > 0) {
      //To add the number of the cpu to the request
      addOptionRequest("nbCpus", convertToString(options->getNbCpu()), sqlRequest);
    } else {
      //If the options is not the default value -1
      if (options->getNbCpu() != -1) {
        throw UserException(ERRCODE_INVALID_PARAM, "The number of cpu is incorrect");
      }
    }

    time_t fromSubmitDate = static_cast<time_t>(options->getFromSubmitDate());
    if(fromSubmitDate != -1) {
      std::string submitDateStr =  boost::posix_time::to_simple_string(boost::posix_time::from_time_t(fromSubmitDate));
      addTimeRequest("submitDate", submitDateStr, sqlRequest, ">=");
    }

    time_t toSubmitDate = static_cast<time_t>(options->getToSubmitDate());
    if(toSubmitDate != -1) {
      std::string submitDateStr =  boost::posix_time::to_simple_string(boost::posix_time::from_time_t(toSubmitDate));
      addTimeRequest("submitDate", submitDateStr, sqlRequest, "<=");
    }

    //To check the job status
    if (options->getStatus() != -1) {
      //To check the job status options
      checkJobStatus(options->getStatus());
      //To add the number of the cpu to the request
      addOptionRequest("status", convertToString(options->getStatus()), sqlRequest);
    }

    //To check the job priority
    if (options->getPriority() != -1) {
      //To check the job priority options
      checkJobPriority(options->getPriority());
      //To add the number of the cpu to the request
      addOptionRequest("jobPrio", convertToString(options->getPriority()), sqlRequest);
    }

    if (options->getOwner().size() != 0) {
      addOptionRequest("owner", options->getOwner(), sqlRequest);
    }

    //To check if the queue is defined
    if (options->getQueue().size() != 0) {
      //To check if the queue exists
      checkQueueName(options->getQueue());
      //To add the jobId on the request
      addOptionRequest("jobQueue", options->getQueue(), sqlRequest);
    }
  }

  /**
   * \brief Function to list sessions information
   * \fn UMS_Data::ListSessions* list()
   * \return The pointer to the UMS_Data::ListSessions containing sessions information
   * \return raises an exception on error
   */
  TMS_Data::ListJobs*
  list() {
    std::string sqlListOfJobs = "SELECT submitMachineId, submitMachineName, jobId, jobName, jobPath, errorPath,"
                                "outputPath, jobPrio, nbCpus, jobWorkingDir, status, submitDate, endDate, owner,"
                                "jobQueue,wallClockLimit, groupName, jobDescription, memLimit, nbNodes, "
                                "nbNodesAndCpuPerNode from job, vsession "
                                "where vsession.numsessionid=job.vsession_numsessionid"
                                " and job.submitMachineId='"+mmachineId+"'";

    std::vector<std::string>::iterator ii;
    std::vector<std::string> results;

    TMS_Data::TMS_DataFactory_ptr ecoreFactory = TMS_Data::TMS_DataFactory::_instance();
    mlistObject = ecoreFactory->createListJobs();

    msessionServer.check();

    processOptions(mparameters, sqlListOfJobs);

    boost::scoped_ptr<DatabaseResult> ListOfJobs (mdatabaseVishnu->getResult(sqlListOfJobs.c_str()));

    if (ListOfJobs->getNbTuples() != 0){
      for (size_t i = 0; i < ListOfJobs->getNbTuples(); ++i) {
        results.clear();
        results = ListOfJobs->get(i);
        ii = results.begin();

        TMS_Data::Job_ptr job = ecoreFactory->createJob();
        job->setSubmitMachineId(*ii);
        job->setSubmitMachineName(*(++ii));
        job->setJobId(*(++ii));
        job->setJobName(*(++ii));
        job->setJobPath(*(++ii));
        job->setOutputPath(*(++ii));
        job->setErrorPath(*(++ii));
        job->setJobPrio(convertToInt(*(++ii)));
        job->setNbCpus(convertToInt(*(++ii)));
        job->setJobWorkingDir(*(++ii));
        job->setStatus(convertToInt(*(++ii)));
        job->setSubmitDate(string_to_time_t(*(++ii)));
        job->setEndDate(string_to_time_t(*(++ii)));
        job->setOwner(*(++ii));
        job->setJobQueue(*(++ii));
        job->setWallClockLimit(convertToInt(*(++ii)));
        job->setGroupName(*(++ii));
        job->setJobDescription(*(++ii));
        job->setMemLimit(convertToInt(*(++ii)));
        job->setNbNodes(convertToInt(*(++ii)));
        job->setNbNodes(convertToInt(*(++ii)));
        job->setNbNodesAndCpuPerNode(1); // TODO

        mlistObject->getJobs().push_back(job);
      }
    }
    return mlistObject;
  }


  /**
   * \brief Function to get the name of the ListJobServer command line
   * \fn std::string getCommandName()
   * \return The the name of the ListJobServer command line
   */
  std::string getCommandName()
  {
    return mcommandName;
  }

  /**
   * \fn ~ListJobServer()
   * \brief Destructor, raises an exception on error
   */
  ~ListJobServer()
  {
  }

  private:

  /////////////////////////////////
  // Attributes
  /////////////////////////////////


  /**
  * \brief The name of the ListJobServer command line
  */
  std::string mcommandName;
  /**
  * \brief The name of the machine in which the jobs whill be listed
  */
  std::string mmachineId;
};

#endif
