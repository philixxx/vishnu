/** \file api_fms.hpp
 * \brief This file contains the VISHNU api functions for FMS package.
 * \author Ibrahima Cisse (ibrahima.cisse@sysfera.com)
 * \date MAY 2011
 */

#ifndef API_FMS_HPP
#define API_FMS_HPP

// C++ Headers
#include <string>

#include "UserException.hpp"
#include "SystemException.hpp"
#include "UMSVishnuException.hpp"
#include "FMSVishnuException.hpp"
#include <sys/types.h>
#ifdef __WIN32__
#include "OSIndependance.hpp"
#endif
//FMS data  declarations
#include <FMS_Data.hpp>
#ifdef __WIN32__
#define VISHNU_API_LIB __declspec(dllexport)
#else
#define VISHNU_API_LIB
#endif

// Namespaces area
using namespace std;
using namespace FMS_Data;

namespace vishnu{

  /**
   * \brief create a new remote file
   * \param sessionKey the session key
   * \param path  the file path using host:path format
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int
    createFile(const string& sessionKey,const string& path)
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief change the group of a file
   * \param sessionKey the session key
   * \param group the name of the new group to use for this file
   * \param path  the file path using host:path format
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int
    chGrp(const string& sessionKey, const string& group, const string& path)
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief  change the permissions of a file
   * \param sessionKey the session key
   * \param mode the path new mode
   * \param path  the file path using host:path format
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int chMod(const string& sessionKey, const mode_t& mode, const string& path)
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief  copy the file
   * \param sessionKey the session key
   * \param src  the "source" file path using host:path format
   * \param dest  the "destination" file path using host:path format
   * \param options contains the options 
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int copyFile(const string& sessionKey,const string& src, const string& dest, const CpFileOptions& options= CpFileOptions())
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief copy the file in a asynchronous mode
   * \param sessionKey the session key
   * \param src   the "source" file path using host:path format
   * \param dest  the "destination" file path using host:path format
   * \param transferInfo contains different information about the submitted file
   * transfer (like the transfer identifier)
   * \param options contains options used to perform the file transfer
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int copyAsyncFile(const string& sessionKey,const string& src, const string& dest,
      FileTransfer& transferInfo, const CpFileOptions& options = CpFileOptions())
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /** 
   * \brief get the first lines of a file
   * \param sessionKey the session key
   * \param path   the file path using host:path format
   * \param contentOfFile  the first "nline" lines of the file
   * \param options   contains the options used to perform the service (like the maximum number of lines to get)
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int headOfFile(const string& sessionKey,const string& path, string& contentOfFile, const HeadOfFileOptions& options = HeadOfFileOptions())
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);
  
  /** 
   * \brief get the content of a file
   * \param sessionKey the session key
   * \param path   the file path using host:path format
   * \param contentOfFile  the content of specified the file
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int contentOfFile(const string& sessionKey,const string& path, string& contentOfFile)
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);


  /**
   * \brief get the list of files and subdirectories of a directory
   * \param sessionKey the session key
   * \param path  the directory path using host:path format
   * \param dirContent  the directory content
   * \param options   contains the options used to perform the service (like the long format list)
   * \return 0 if everything is OK, another value otherwise
   */
 
  VISHNU_API_LIB int listDir(const string& sessionKey,const string& path, DirEntryList& dirContent,const LsDirOptions& options=LsDirOptions())
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief create a directory
   * \param sessionKey the session key
   * \param path: the directory path using host:path format
   * \param options   contains the options used to perform the service (like the recursive creation)
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int createDir(const string& sessionKey,const string& path,const CreateDirOptions& options= CreateDirOptions())
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief move a file
   * \param sessionKey the session key
   * \param src   the "source" file path using host:path format
   * \param dest  the "destination" file path using host:path format
   * \param options   contains the options used to perform the service (like the transfer command :scp or rsync)
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int moveFile(const string& sessionKey,const string& src, const string& dest, const CpFileOptions& options= CpFileOptions() )
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief move a file in a asynchronous mode
   * \param sessionKey the session key
   * \param src:   the "source" file path using host:path format
   * \param dest:  the "destination" file path using host:path format
   * \param transferInfo contains different information about the submitted file
   * transfer (like the transfer identifier) 
   * \param options   contains the options used to perform the service (like the transfer command :scp or rsync)
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int moveAsyncFile(const string& sessionKey,const string& src, const string& dest,
      FileTransfer& transferInfo, const CpFileOptions& options= CpFileOptions())
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /** remove a file
   * \param sessionKey the session key
   * \param path    the file path using host:path format
   * \param options contains options used to perform the remove file function
   \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int removeFile(const string& sessionKey,const string& path,const RmFileOptions& options= RmFileOptions())
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief  remove a directory
   * \param sessionKey the session key
   * \param path    the directory path using host:path format
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int removeDir(const string& sessionKey,const string& path)
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief get the last lines of a file
   * \param sessionKey the session key
   * \param path    the file path using host:path format
   * \param contentOfFile  the last "nline" lines of the file
   * \param options  the options used to perform the service
   * \param contentOfFile  an user-allocated buffer to store the result
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int tailOfFile(const string& sessionKey,const string& path, string& contentOfFile,const TailOfFileOptions& options = TailOfFileOptions())
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);

  /**
   * \brief  obtain informations about a file
   * \param sessionKey the session key
   * \param path the file path using host:path format
   * \param filesInfo  the file informations
   * \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int getFileInfo(const string& sessionKey,const string& path, FileStat& filesInfo)
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);


  /**
   * \brief cancel a file transfer
   * \param sessionKey the session key
   * \param options   contains the options used to perform the service (like the transfer id obtained after a call to copyAsyncFile or
   *          moveAsyncFile )
   \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int stopFileTransfer(const string& sessionKey,const StopTransferOptions& options = StopTransferOptions() )
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);


  /**
   * \brief List  file transfers
   * \param sessionKey the session key
   * \param fileTransferList the file transfer list
   * \param options contains the options used to perform the service (like the transfer id obtained after a call to copyAsyncFile or
   moveAsyncFile)
   \return 0 if everything is OK, another value otherwise
   */
  VISHNU_API_LIB int listFileTransfers(const string& sessionKey,FileTransferList& fileTransferList, const LsTransferOptions& options = LsTransferOptions())
    throw (UMSVishnuException, FMSVishnuException, UserException, SystemException);




}

#endif
