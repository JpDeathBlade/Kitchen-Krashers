#ifndef  REDJAGUAR_JS_DEBUGGER_H_
#define  REDJAGUAR_JS_DEBUGGER_H_

// 0===========================================================================0
// | ~ Defines ~                                                               |
// 0===========================================================================0
#define RJ_DEBUGGER RJ_Debugger::GetInstance()	// Allows for easy access to 
												//  this classes Instance

#define Error(_error, _message) Error_t(_error, _message, __FILE__, __FUNCTION__, __LINE__)

class RJ_Debugger
{
	public:
		enum RJ_ERROR_TYPE { RJ_FATAL = 0, RJ_ALERT, RJ_CRIT, RJ_ERROR, RJ_WARN, 
			RJ_NOTICE, RJ_INFO, RJ_DEBUG, RJ_NOTSET, RJ_UNKNOWN
		};

	private:
		static RJ_Debugger				m_Instance;
		RJ_ERROR_TYPE					m_Threshold;

		RJ_Debugger(void);
		RJ_Debugger(RJ_Debugger& _ref);
		RJ_Debugger& operator= (RJ_Debugger& _ref);

		char* GetErrorType(const RJ_ERROR_TYPE _error);
		void SetErrorColor(const RJ_ERROR_TYPE _error);

		void ConsoleError_t(const RJ_ERROR_TYPE _error, const char* _message,
			const char* _file, const char* _function, const int _line);

	public:
		static RJ_Debugger* GetInstance(void)	{ return &m_Instance; }
		virtual ~RJ_Debugger() { }

		void Error_t(const RJ_ERROR_TYPE _error, const char* _message, 
			const char* _file, const char* _function, const int _line);
		void Log(const char* _message);

		void SetThreshold(const RJ_ERROR_TYPE _error) { m_Threshold = _error; }
};

#endif // end of REDJAGUAR_JS_DEBUGGER_H_