#ifndef  REDJAGUAR_JS_MACROS_H_		// Check to see if this .h is defined
#define  REDJAGUAR_JS_MACROS_H_		//  if not, define it

namespace RJ_Macro
{
	#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = NULL; } 
};

#endif //REDJAGUAR_JS_MACROS_H_