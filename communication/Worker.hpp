/**
 * \file Worker.hpp
 * \brief This file contains the definition of the base class for the workers
 * \author Haikel Guemar (haikel.guemar@sysfera.com)
 * \date January 2013
 */
#ifndef _WORKER_HPP_
#define _WORKER_HPP_

#include <iostream>

#include "zhelpers.hpp"
#include "utils.hpp"
#include "sslhelpers.hpp"
#include "VishnuException.hpp"
#include "Logger.hpp"

/**
 * \class Worker
 * \brief Base class for workers
 */
class Worker {
public:
  /**
   * \brief Constructor
   * \param ctx zmq context
   * \param uriInproc URI to be used to connect the socket
   * \param id worker's identifier
   */
  explicit Worker(boost::shared_ptr<zmq::context_t> ctx,
                  const std::string& uriInproc,
                  int id)
    : ctx_(ctx), uriInproc_(uriInproc), id_(id) {}


  /**
   * \brief Main loop. receives data and deal with it if it isn't empty
   * Uses protected method doCall to provide implementation specific
   * behavior when recieving data.
   */
  void
  operator()() {
    Socket socket(*ctx_, ZMQ_REP);
    socket.connect(uriInproc_.c_str());
    std::string data;

    while (true) {
      data.clear();
      try {
        data = socket.get();
      } catch (zmq::error_t &error) {
        LOG(boost::str(boost::format("[ERROR] %1%\n") % error.what()), LogErr);
        continue;
      }

      // Deserialize and call Method
      if (! data.empty()) {
        try {
          std::string resultSerialized = doCall(data);
          socket.send(resultSerialized);
        } catch (const VishnuException& ex) {
          socket.send(ex.what());
          LOG(boost::str(boost::format("[ERROR] %1%\n")%ex.what()), LogErr);
        }
      }
    }
  }


protected:
  /**
   * \brief method to provide implementation specific behavior
   * to handle received data.
   * \param data a string containing the data
   * \return the updated data
   */
  virtual std::string
  doCall(std::string& data) = 0;

  /**
   * \brief zmq context
   */
  boost::shared_ptr<zmq::context_t> ctx_;
  /**
   * \brief The worker inproc uri
   */
  std::string uriInproc_;
  /**
   * \brief Worker id
   */
  int id_;
};



/**
 * \brief templated method to create two sockets: a router and dealer
 * and, creates a pool of threads of workers to handle the requests
 * \param serverUri URI of the server socket (ROUTER)
 * \param workerUri URI of the worker socket (DEALER)
 * \param nbThreads number of threads in the pool
 * \param params Worker specific parameter
 * \return 0 on success, an error code otherwize
 */
template<typename WorkerType,
         typename WorkerParam>
int
serverWorkerSockets(const std::string& serverUri,
                    const std::string& workerUri,
                    int nbThreads,
                    WorkerParam params,
                    bool useSsl,
                    const std::string& cafile) {
  boost::shared_ptr<zmq::context_t> context(new zmq::context_t(1));
  zmq::socket_t socket_server(*context, ZMQ_ROUTER);
  zmq::socket_t socket_workers(*context, ZMQ_DEALER);

  // bind the sockets
  try {
    socket_server.bind(serverUri.c_str());
    LOG(boost::str(boost::format("[INFO] ZMQ socket bound (%1%)") % serverUri), LogInfo);
  } catch (const zmq::error_t& e) {
    LOG(boost::str(boost::format("[ERROR] zmq socket_server (%1%) binding failed (%2%)")
                   % serverUri % e.what()), LogErr);
    return 1;
  }

  try {
    socket_workers.bind(workerUri.c_str());
  } catch (const zmq::error_t& e) {
    LOG(boost::str(boost::format("[ERROR] zmq socket_worker (%1%) binding failed (%2%)")
                   % workerUri % e.what()), LogErr);
    return 1;
  }

  // Create our pool of threads
  ThreadPool pool(nbThreads);
  for (int i = 0; i < nbThreads; ++i) {
    if (useSsl) {
      pool.submit(WorkerType(context, workerUri, i, params, useSsl, cafile));
    } else {
      pool.submit(WorkerType(context, workerUri, i, params, false, ""));
    }
  }

  // connect our workers threads to our server via a queue
  do {
    try {
      zmq::device(ZMQ_QUEUE, socket_server, socket_workers);
      break;
    } catch (const zmq::error_t& e) {
      if (EINTR == e.num()) {
        continue;
      } else {
        LOG(boost::str(boost::format("[ERROR] zmq device creation failed (%1%)\n")
                       % e.what()), LogErr);
        return 1;
      }
    }
  } while(true);

  return 0;
}


#endif /* _WORKER_HPP_ */
