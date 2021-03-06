/**
 * \file BatchFactory.cpp
 * \brief This file implements the batch factory
 * \author Daouda Traore (daouda.traore@sysfera.com)
 * \date April
 */

#include "BatchFactory.hpp"
#include <string>
#include <boost/format.hpp>
#include <iostream>
#include "tmsUtils.hpp"
#include "SharedLibrary.hh"


static int created = 0;

/**
 * \brief Constructor
 */
BatchFactory::BatchFactory() {
  mbatchServer = NULL;
}


BatchServer*
loadPluginBatch(const char *name) {
  dadi::SharedLibrary *plugin(NULL);
  BatchServer *instance(NULL);

  std::string libname = boost::str(boost::format("%1%%2%%3%")
                                   % dadi::SharedLibrary::prefix()
                                   % name
                                   % dadi::SharedLibrary::suffix());

  plugin = new dadi::SharedLibrary(libname);
  if (plugin->isLoaded()) {
    void *factory = plugin->symbol("create_plugin_instance");
    reinterpret_cast<factory_function>(factory)(reinterpret_cast<void**>(&instance));
  }
  return instance;
}

/**
 * \brief Function to create a batchServer.
 * \param batchType The type of batchServer to create
 * \param batchVersion The version of batchServer to create
 * \return an instance of BatchServer, or NULL
 */
BatchServer*
BatchFactory::getBatchServerInstance(int batchType,
                                     const std::string &batchVersion) {
  BatchServer *instance(NULL);
  std::string libname = "vishnu-tms-";
  // batchVersion is set to n/a when not applicable but MUST be not be taken into account when loading the lib
  std::string realBatchVersion = (batchVersion!="n/a")? batchVersion : "";

  switch(batchType){
  case TORQUE:
    libname += "torque";
    break;
  case LOADLEVELER:
    libname += "loadleveler";
    break;
  case SLURM:
    libname += "slurm";
    break;
  case LSF:
    libname += "lsf";
    break;
  case SGE:
    libname += "sge";
    break;
  case PBSPRO:
    libname += "pbspro";
    break;
  case DELTACLOUD:
    libname += "deltacloud";
    realBatchVersion = "";
    break;
  case OPENNEBULA:
    libname += "opennebula";
    realBatchVersion = "";
    break;
  case POSIX: // POSIX IS THE DEFAULT CASE
  default : // Always compile tmp-posix
    libname += "posix";
    realBatchVersion = "";
    break;
  }

  libname += realBatchVersion;
  instance = loadPluginBatch(libname.c_str());
  return static_cast<BatchServer *>(instance);
}


/**
 * \brief Function to delete a batchServer.
 */
void BatchFactory::deleteBatchServerInstance() {
  delete mbatchServer;
}
/**
 * \brief Destructor
 */
BatchFactory::~BatchFactory() {
}
