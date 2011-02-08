#ifndef OPTIONS_HH
#define OPTIONS_HH


/**
 * \file Option.hh
 * \brief This file defines a class to handle command line options.
 * \author Ibrahima Cisse (ibrahima.cisse@sysfera.com)
 */

#include <boost/program_options.hpp>
#include <boost/function.hpp>
#include <string>
#include <list>
#include <map>
#include <typeinfo>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <fstream>

namespace po = boost::program_options;
class Options;

/**
 * \class Configuration
 * \brief Standard configuration class. 
 */
 
class Configuration {
private:
  std::string pgName;/*!< Program name     */
  std::string configFile;/*!< configuration file   */
public:
  /**
   * \brief Default constructor
   */
   Configuration();

  /**
   * \brief Constructor by variable
   * \param pgName: The program name
   */
  Configuration(const std::string& pgName);

  /**
   * \brief get the program name
   * \return the program name as std::string
   */
  const std::string& getPgName() const;

  /**
   * \brief Get the configuration file
   * \return the configuration file as std::string
   */
  const std::string& getConfigFile() const;
  

  /**
   * \brief Set a new configuration file
   * \param configFile: the new configuration file to set
   */
  void setConfigFile(const std::string& configFile);
};

 /**
  * \brief function type used to map environnement variable name
  */
	typedef boost::function1<std::string, std::string> func1;



	/**
	 * \enum Group_type
	 * \brief A convenient type allowing to group options
	 */
	typedef enum{
		GENERIC=0,/*!< a generic group of options allowed for all commands  */
		CONFIG,/*!<  configuration options   */
		ENV,/*!< environment variables  */
		HIDDEN /*!< options that will not shown to the user */
	}Group_type;


	/**
	 * \class Options
	 * \brief Used to process the users command line parameters
	 */
	class Options {
		private:
			Configuration* conf;/*!< store a command configuration*/
			po::options_description generic_options;/*!< Generic option description*/
			po::options_description config_options;/*!< Configuration option description */ 
			po::options_description hidden_options ;/*!<Hidden option description*/
			po::options_description env_options; /*!< environment variable option description*/
			mutable po::variables_map vm;/*!<  map storing all parsed options  */
			po::positional_options_description position; /*!< option position in a command line  */

			void setGroup (const po::options_description&, const Group_type&);/*!< to set the option group    */

		public:
			/**
			 * \brief Constructor by variable taking a command configuration in parameter
			 * \param 
			 */
			Options(Configuration* );

			/**
			 * \brief function allowing to add a new non-typed option 
			 */
			virtual  void add(const std::string&,const std::string&, const Group_type &);
			
			/**
			 * \brief function offering a way to add a typed option
			 * \param name: the name of the option
			 * \param desc: brief description of the option
			 * \param group: a group which option belongs to
			 */

			template<class T>
				void add(const std::string& name,
                           const std::string& desc,
						   const Group_type& group,
						   T& value){

					po::options_description tmp_options;

					if( typeid(T)== typeid(std::vector<std::string>)){
						tmp_options.add_options()(name.c_str(),po::value<T>(&value)->multitoken(),desc.c_str());
					}
					else{
						tmp_options.add_options()(name.c_str(),po::value<T>(&value),desc.c_str());
					}

					// Set the group

					setGroup(tmp_options,group);

				}
			
			/**
			 * \brief To set position of options 
			 */
			
			virtual void setPosition(const std::string &, int); 

			/**
			 * \brief To parse command line
			 */

			virtual void parse_cli(int, char*[]);

			/**
			 * \brief To parse config file
			 */

			 virtual void parse_cfile();
			 
			 /**
			  * \brief To parse environement variable
			  */

			 void parse_env (const func1 &);
			 
			 /**
			  * \brief To notify all  user-defined functions
			  */

			 void notify ();
			 
			 /**
			  * \brief To check the existence of options
			  */

			 int count (const std::string&)const;
			 
			 /**
			  * \brief To get the value of options
			  */
			 
			 template<class T>
				 const T & get(const std::string key)const{
					 return(vm[key.c_str()].template as<T>());
				 }
			 
			 /**
			  * \brief to print options
			  */
	
			 friend std::ostream& operator<<(std::ostream &,const Options & );
			 
			 /**
			  * \brief Default destructor
			  */

			 virtual ~Options();
	};

/*
 * \brief A helper function to simplify the display of vector
 * \param os: an ostream to write data in
 * \param v: a vector to print
 */

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v){
	    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
		    
		return os;
}


#endif
