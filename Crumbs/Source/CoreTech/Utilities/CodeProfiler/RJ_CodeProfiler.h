/**
 * 0===========================================================================0
 * | RJ_CodeProfiler.h                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Joseph Stuhr                                            |
 * |   E-mail        : JpDeathBlade@gmail.com                                  |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : ??:?? pm (Eastern) on 09/13/2009                        |
 * |   Date Edited   : 10:15 pm (Eastern) on 06/09/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   A class used to profile code to view the time it takes a fucntion or    |
 * |    chunk of code to run. Using this a programmer can find spikes in       |
 * |    his/her code, allowing for optimization.                               |
 * 0===========================================================================0
 */
 
#ifndef REDJAGUAR_JS_CCODEPROFILER_H_
#define REDJAGUAR_JS_CCODEPROFILER_H_

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include <windows.h>  // For QueryPerformanceCounter()
#include <vector>     // Used to store the Profiles till the end of the program.
using std::vector;	  //  as well as the error strings

// Profile MACRO to return the Profiler Instance
#define RJ_CODEPROFILER		RJ_CodeProfiler::GetInstance()

// Start and Stop MACROS for the user(s) of the code
#define Start()				Start_t(__FUNCTION__,__LINE__,__FILE__);
#define Stop()				Stop_t(__FUNCTION__,__LINE__,__FILE__);

class RJ_CodeProfiler
{
	// 0=======================================================================0
	// | ~ VARIABLES ~                                                         |
	// 0=======================================================================0
	private:
		// A basic layout of a Code Profile
		struct tCodeProfile
		{
			int			   m_nLineStarted;   // Line the profiler started at.
			int			   m_nLineStopped;   // Line the profiler stopped at.
			char*		   m_szFunctionName; // Name of the Function being 
											 //   profiled.
			char*		   m_szFileName;     // The file the function is in.
			LARGE_INTEGER  m_liStart;		 // Log the run time of the 
										     //   Code Profile.
			vector<float>  m_fTimeElapsed;   // The total time elapsed.
			bool		   m_bRunning;	     // Is there a Profile being logged?

		}; // end of struct tCodeProfile

		// A basic layout of a Code Profile
		struct tError
		{
			char		   m_szError[1024];  // Error String

		}; // end of struct tError

		// Store the frequeny to get accurate time for that machine.
		LARGE_INTEGER		 m_liFrequeny;

		// Log the run time of the Code Profiler
		LARGE_INTEGER		 m_liProfilerStart;

		// Log the stop time of a profile to save space
		LARGE_INTEGER		 m_liEnd;

		// Holds all the Profiles to save at the end of the program.
		vector<tCodeProfile> m_CodeProfiles;

		// Holds all the Erros to save at the end of the program.
		vector<tError>		 m_Errors;

		// Holds the profile that is Already Started
		tCodeProfile*		 m_pRunningProfile;

		// Instance of the CodeProfiler
		static RJ_CodeProfiler m_Instance;

	private:
		// 0===================================================================0
		// | ~ FUNCTIONS ~                                                     |
		// 0=====0=======================================================0=====0
		//       |                                                       |      
		// 0=====0=======================================================0=====0
		// | Function  :	CCodeProfiler()                           09/15/09 |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Input   :	void                                               |
		// |                                                                   |
		// |   Return  :	N/A                                                |
		// |                                                                   |
		// |   Purpose :	Constructor.                                       |
		// 0===================================================================0
		RJ_CodeProfiler(void);
		RJ_CodeProfiler(RJ_CodeProfiler &ref);
		RJ_CodeProfiler &operator= (RJ_CodeProfiler &ref);

		// 0===================================================================0
		// | Function  :	AlreadyStarted()                          09/15/09 |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Input   :	char*: Name of the Profile                         |
		// |                                                                   |
		// |   Return  :	True if the function exists.                       |
		// |                                                                   |
		// |   Purpose :	Locate the function to see if it exists.           |
		// 0===================================================================0
		bool AlreadyStarted(char* _function);

		// 0===================================================================0
		// | Function  :	LogError()                                09/15/09 |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Input   :	char*: Name of the Error                           |
		// |                char*: Name of the Profile                         |
		// |                char*: The file the error happened in              |
		// |                int  : The line the error happened on              |
		// |                                                                   |
		// |   Return  :	Adds the error to the error log.                   |
		// |                                                                   |
		// |   Purpose :	Logging Errors.                                    |
		// 0===================================================================0
		void LogError(char* _error, char* _function, char* _file, int _line);

	public:
		// 0===================================================================0
		// | ~ FUNCTIONS ~                                                     |
		// 0=====0=======================================================0=====0
		//       |                                                       |      
		// 0=====0=======================================================0=====0
		// | Function  :	~CCodeProfiler()                          09/15/09 |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Input   :	void                                               |
		// |                                                                   |
		// |   Return  :	N/A                                                |
		// |                                                                   |
		// |   Purpose :	Destructor.                                        |
		// 0===================================================================0
		~RJ_CodeProfiler(void);
 
		// 0===================================================================0
		// | Function  :	GetInstance()                             09/15/09 |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Input   :	void                                               |
		// |                                                                   |
		// |   Return  :	Instance to this class.                            |
		// |                                                                   |
		// |   Purpose :	Make the class a singleton and allow access to it. |
		// 0===================================================================0
		static RJ_CodeProfiler* GetInstance(void);

		// 0===================================================================0
		// | Function  :	InitCodeProfiler()                        09/15/09 |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Input   :	_D3DXVECTOR2: D3DXVECTOR2 of the Profile vector (Default: 128).  |
		// |                                                                   |
		// |   Return  :	True if the function was successful.               |
		// |                                                                   |
		// |   Purpose :	To initialize the Code Profiler.                   |
		// 0===================================================================0
		bool InitCodeProfiler(int _Size = 128);

		// 0===================================================================0
		// | Function  :	ShutdownCodeProfiler()                    09/15/09 |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Input   :	void                                               |
		// |                                                                   |
		// |   Return  :	void                                               |
		// |                                                                   |
		// |   Purpose :	To shutdown the Code Profiler.                     |
		// 0===================================================================0
		void ShutdownCodeProfiler(void);

		// 0===================================================================0
		// | Function  :	Start()                                   09/15/09 |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Input   :	char*: Name of the Profile                         |
		// |                                                                   |
		// |   Return  :	void                                               |
		// |                                                                   |
		// |   Purpose :	Tell the Profiler to begin logging this Profile.   |
		// 0===================================================================0
		void Start_t(char* _function, int _line, char* _file);

		// 0===================================================================0
		// | Function  :	Stop()                                    09/15/09 |
		// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
		// |   Input   :	char*: Name of the Profile                         |
		// |                                                                   |
		// |   Return  :	void                                               |
		// |                                                                   |
		// |   Purpose :	Tell the Profiler to stop logging this Profile.    |
		// 0===================================================================0
		void Stop_t(char* _function, int _line, char* _file);

}; // end of class RJ_CodeProfiler

#endif // end of REDJAGUAR_JS_CCODEPROFILER_H_