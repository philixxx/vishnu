#ifndef __SED__H__
#define __SED__H__

#include "DIET_client.h"
#include "sslhelpers.hpp"
#include <map>
#include <string>
#include <vector>
#include <boost/function.hpp>

#define UNKNOWN_SERVICE 1;
#define INTERNAL_ERROR 2;

typedef boost::function1<int, diet_profile_t*> CallbackFn;
typedef std::map<std::string, CallbackFn> CallbackMap;

int
heartbeat(diet_profile_t* pb);
/**
 * \class SeD
 * \brief base class to Server*MS classes
 */
class SeD {
public:
  /**
   * \brief Constructor
   */
  SeD();

  /**
   * \brief To call a function upon receiving a request
   * \param prof The profile of the service
   * \return the error code of the function
   */
  virtual int
  call(diet_profile_t* prof);

  /**
   * \brief To get the services offered by the server
   * \return the services offered by the server
   */
  virtual std::vector<std::string>
  getServices();

protected:
  /**
   * \brief map with function ptr for callback
   */
  CallbackMap mcb;
};

/**
 * @brief ZMQServerStart
 * @param server
 * @param uri
 * @param useSsl
 * @param cafile
 * @return
 */
int
ZMQServerStart(boost::shared_ptr<SeD> server,
               const std::string& uri,
               bool useSsl,
               const std::string& cafile);


#endif // __SED__H__
