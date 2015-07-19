// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include "RJ_CodeProfiler.h"
#include <fstream>		// Allows us to open a file and write out data
using std::ofstream;	//   to be veiwed later.

// 0===========================================================================0
// | ~ STATICS ~                                                               |
// 0===========================================================================0
RJ_CodeProfiler RJ_CodeProfiler::m_Instance;

// 0===========================================================================0
// | Function  :	CCodeProfiler()                                   09/15/09 |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Input   :	void                                                       |
// |                                                                           |
// |   Return  :	N/A                                                        |
// |                                                                           |
// |   Purpose :	Constructor.                                               |
// 0===========================================================================0
RJ_CodeProfiler::RJ_CodeProfiler(void)
{
	#if _DEBUG
		// Initialize the Frequeny
		//  - This can fail on older machines!
		if(!QueryPerformanceFrequency(&m_liFrequeny))
			exit(-10); // m_liFrequeny is now 0, fix that (quit for now)

	#endif
}

// 0===========================================================================0
// | Function  :	~CCodeProfiler()                                  09/15/09 |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Input   :	void                                                       |
// |                                                                           |
// |   Return  :	N/A                                                        |
// |                                                                           |
// |   Purpose :	Destructor.                                                |
// 0===========================================================================0
RJ_CodeProfiler::~RJ_CodeProfiler(void)
{
	#if _DEBUG
	#endif
}

// 0===========================================================================0
// | Function  :	GetInstance()                                     09/15/09 |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Input   :	void                                                       |
// |                                                                           |
// |   Return  :	Instance to this class.                                    |
// |                                                                           |
// |   Purpose :	Make the class a singleton and allow access to it.         |
// 0===========================================================================0
RJ_CodeProfiler* RJ_CodeProfiler::GetInstance(void)
{
	#if _DEBUG
		return &m_Instance;
	#else
		return NULL;
	#endif
}

// 0===========================================================================0
// | Function  :	InitCodeProfiler()                                09/15/09 |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Input   :	_D3DXVECTOR2: D3DXVECTOR2 of the Profile vector (Default: 128).          |
// |                                                                           |
// |   Return  :	True if the function was successful.                       |
// |                                                                           |
// |   Purpose :	To initialize the Code Profiler.                           |
// 0===========================================================================0
bool RJ_CodeProfiler::InitCodeProfiler(int _D3DXVECTOR2)
{
	#if _DEBUG
		// Reserve slots for the Profiles and Errors, Default: 128
		m_CodeProfiles.reserve(_D3DXVECTOR2);
		m_Errors.reserve(_D3DXVECTOR2);

		// Start logging the Profilers run time
		QueryPerformanceCounter(&m_liProfilerStart);
		return true;
	#else
		return false;
	#endif
}

// 0===========================================================================0
// | Function  :	ShutdownCodeProfiler()                            09/15/09 |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Input   :	void                                                       |
// |                                                                           |
// |   Return  :	void                                                       |
// |                                                                           |
// |   Purpose :	To shutdown the Code Profiler.                             |
// 0===========================================================================0
void RJ_CodeProfiler::ShutdownCodeProfiler(void)
{
	#if _DEBUG
		// The Profiler is done logging, log the time stopped
		QueryPerformanceCounter(&m_liEnd);

		// Calculate the elapsed time of the Code Profiler
		//  (End - Start * 1000) / CPU Frequeny = Milliseconds Ran
		float timeRan = 
			(float)(((m_liEnd.QuadPart - m_liProfilerStart.QuadPart) * 1000.0f)
			  / m_liFrequeny.QuadPart);

		float	totalTime;			// Store the total time that each Profile ran
		bool	stillRunning;		// Store if the Profile is still running

		// Open the txt file to save the data
		char buffer[64];
		sprintf_s(buffer,sizeof(buffer),"Resources\\Data\\CodeProfiler\\Profile - %s %d.txt",__DATE__,__TIME__);
		ofstream FileOut(buffer);

		// If the file is opened, write the data
		if(FileOut.is_open())
		{
			// Print the Profilers total Runtime
			FileOut << "Code Profiler Runtime: " << timeRan << " millisecond.\n"
					<< "====================================================\n\n";

			// Iterate through the Profiles
			for(vector<tCodeProfile>::iterator iter = m_CodeProfiles.begin();
				iter != m_CodeProfiles.end(); iter++)
			{
				// Reset for the new Profile
				stillRunning = false;
				totalTime = 0.0f;

				// If that function is still running, stop it!
				if((*iter).m_bRunning)
				{
					Stop();	// Call stop on the Profile
					(*iter).m_nLineStopped = -1;	// Set its line stopped to -1
					stillRunning = true;			// It was still running

					LogError("Error on Shutdown: %s NEVER STOPPED!! - Called at %s Line %d",
						(*iter).m_szFunctionName,(*iter).m_szFileName,(*iter).m_nLineStarted);
				}

				// Print the details of the Profile
				FileOut << (*iter).m_szFunctionName << " in " << (*iter).m_szFileName << "\n"
						<< " - Line Started: " << (*iter).m_nLineStarted << "\n"
						<< " - Line Stopped: " << (*iter).m_nLineStopped << "\n";

				// If it was still running print that we stopped it
				if(stillRunning)
					FileOut << "    !! STOPPED ON SHUTDOWN !!\n";

				// Iterate through the times of the Profile
				for(vector<float>::iterator iterF = (*iter).m_fTimeElapsed.begin();
				iterF != (*iter).m_fTimeElapsed.end(); iterF++)
				{
					FileOut << " - Time        : " << (*iterF) << " ms\n"; // Print the time
					totalTime += (*iterF); // Update the total time
				}

				// Print the total as well as the average
				FileOut << " Total: " << totalTime << " / " << (*iter).m_fTimeElapsed.size() << " run(s)\n"
					    << "  - " << (totalTime / (*iter).m_fTimeElapsed.size()) << " average ms\n"
						<< "===================================================\n\n";
			}

			// Print the Errors the user had
			FileOut << "Errors:\n"
					<< "===================================================\n\n";

			// If there are no errors, print that
			if(m_Errors.size() == 0)
				FileOut << " - There were no errors!";
			else
			{
				// Iterate through the error list
				for(vector<tError>::iterator iter = m_Errors.begin();
					iter != m_Errors.end(); iter++)
				{
					// Print the details
					FileOut << (*iter).m_szError << "\n\n";
				}
			}

			// Close the file when we are done
			FileOut.close();
		}
	#endif
}

// 0===========================================================================0
// | Function  :	Start()                                           09/15/09 |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Input   :	char*: Name of the Profile                                 |
// |                                                                           |
// |   Return  :	void                                                       |
// |                                                                           |
// |   Purpose :	Tell the Profiler to begin logging this Profile.           |
// 0===========================================================================0
void RJ_CodeProfiler::Start_t(char* _function, int _line, char* _file)
{
	#if _DEBUG
		if(!AlreadyStarted(_function)) //If the function doesn't exist we want to make it
		{							   //  - AlreadyStarted (if true) saves the Profile to m_pRunningProfile
			tCodeProfile newProfile;   // Create a Profile for the new function

			// Initialize the profile
			newProfile.m_szFunctionName = _function;  // Save the function name
			newProfile.m_nLineStarted = _line;		  // Save the Starting Line
			newProfile.m_szFileName = _file;		  // Save the File name
			newProfile.m_bRunning = true;			  // This function is now running

			// Start the timer
			QueryPerformanceCounter(&newProfile.m_liStart);

			// Add the profile to the vector
			m_CodeProfiles.push_back(newProfile);
		}
		else // If the Profile exists, check if its running
		{

			if(!(*m_pRunningProfile).m_bRunning) // If its not running
			{
				// Start it up again
				(*m_pRunningProfile).m_bRunning = true;
				QueryPerformanceCounter(&(*m_pRunningProfile).m_liStart);
			}
			else
			{
				LogError("Error: %s already running - Called at %s Line %d",
						_function, _file, _line);
			}
		}
	#endif
}

// 0===========================================================================0
// | Function  :	Stop()                                            09/15/09 |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Input   :	char*: Name of the Profile                                 |
// |                                                                           |
// |   Return  :	void                                                       |
// |                                                                           |
// |   Purpose :	Tell the Profiler to stop logging this Profile.            |
// 0===========================================================================0
void RJ_CodeProfiler::Stop_t(char* _function, int _line, char* _file)
{
	#if _DEBUG
		if(AlreadyStarted(_function)) //If it exists
		{							  //  - AlreadyStarted (if true) saves the Profile to m_pRunningProfile
			if((*m_pRunningProfile).m_bRunning) // If its running
			{
				QueryPerformanceCounter(&m_liEnd);           // Log the stop time of the Profile
				(*m_pRunningProfile).m_bRunning = false;	 // Stop it so we can use this Profile again
				(*m_pRunningProfile).m_nLineStopped = _line; // Log the line it was stopped

				// Calculate the elapsed time
				float time = 
					(float)(((m_liEnd.QuadPart - (*m_pRunningProfile).m_liStart.QuadPart) * 1000.0f)
						/ m_liFrequeny.QuadPart);

				// Save the elapsed time
				(*m_pRunningProfile).m_fTimeElapsed.push_back(time);
			}
			else
			{
				LogError("Error: %s is NOT running - Called at %s Line %d",
						_function, _file, _line);
			}
		}
		else // It does not exists, error
		{
			LogError("Error: %s does NOT exist - Called at %s Line %d",
						_function, _file, _line);
		}
	#endif
}

// 0===========================================================================0
// | Function  :	AlreadyStarted()                                  09/15/09 |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Input   :	char*: Name of the Profile                                 |
// |                                                                           |
// |   Return  :	True if the function exists.                               |
// |                                                                           |
// |   Purpose :	Locate the function to see if it exists.                   |
// 0===========================================================================0
bool RJ_CodeProfiler::AlreadyStarted(char* _function)
{	
	// Iterate through the Profiles
	for(vector<tCodeProfile>::iterator iter = m_CodeProfiles.begin(); iter != m_CodeProfiles.end(); iter++)
	{
		// If the function already exists...
		if( (*iter).m_szFunctionName == _function)
		{
			// Save it so we can edit it...
			m_pRunningProfile = &(*iter);
			return true; // Break and return true
		}
	}

	return false;
}

// 0===========================================================================0
// | Function  :	LogError()                                        09/15/09 |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Input   :	char*: Name of the Error                                   |
// |                char*: Name of the Profile                                 |
// |                char*: The file the error happened in                      |
// |                int  : The line the error happened on                      |
// |                                                                           |
// |   Return  :	Adds the error to the error log.                           |
// |                                                                           |
// |   Purpose :	Logging Errors.                                            |
// 0===========================================================================0
void RJ_CodeProfiler::LogError(char* _error, char* _function, char* _file, int _line)
{
	tError newError;  // Create an error
	char buffer[1024]; // Create a buffer for the error

	// Create the Error message
	sprintf_s(buffer,_countof(buffer),_error,
		_function,_file,_line);

	// Save the error
	strcpy_s(newError.m_szError,sizeof(buffer),buffer);

	// Push the error back
	m_Errors.push_back(newError);
}