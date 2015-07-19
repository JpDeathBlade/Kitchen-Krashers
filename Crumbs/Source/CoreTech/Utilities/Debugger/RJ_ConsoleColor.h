/**
 * 0===========================================================================0
 * | RJ_ConsoleColor.h                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Joseph Stuhr                                            |
 * |   E-mail        : JpDeathBlade@gmail.com                                  |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : ??:?? pm (Eastern) on 03/06/2009                        |
 * |   Date Edited   : 10:15 pm (Eastern) on 06/09/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   A class to control and change the display colors in a Win32 console.    |
 * |    16 colors for both foreground and background, 256 total combinations.  |
 * 0===========================================================================0
 */

#ifndef REDJAGUAR_JS_CONSOLECOLOR_H_  // Check to see if this .h is defined
#define REDJAGUAR_JS_CONSOLECOLOR_H_  //  if not, define it

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#include <windows.h>  // Used to edit the colors of the console window.
#include <iostream>
using namespace std;

// 0===========================================================================0
// | namespace Console_Color                                                   |
// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
// |   Namespace used to edit the color of the Win32 Console.                  |
// 0===========================================================================0
namespace ConsoleColor
{
	// 0=======================================================================0
    // | enum Mask                                                             |
	// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
	// |   Masks used when setting colors. DO NOT EDIT!!                       |
    // 0=======================================================================0
	enum Mask{ 
			   FG_Mask = ( FOREGROUND_BLUE | FOREGROUND_GREEN | 
                           FOREGROUND_RED  | FOREGROUND_INTENSITY ),
			   BG_Mask = ( BACKGROUND_BLUE | BACKGROUND_GREEN | 
						   BACKGROUND_RED  | BACKGROUND_INTENSITY )
	};

	// 0=======================================================================0
    // | enum Color_FG                                                         |
	// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
	// |   All of the available foreground colors.                             |
    // 0=======================================================================0
	enum Color_FG { Black     = (0),
		            D_Blue    = ( FOREGROUND_BLUE  ),
					D_Green   = ( FOREGROUND_GREEN ),
					D_Cyan    = ( D_Green | D_Blue ),
					D_Red     = ( FOREGROUND_RED   ),
					D_Magenta = ( D_Red | D_Blue   ),
					D_Yellow  = ( D_Red | D_Green  ),
					D_White   = ( D_Red | D_Green | D_Blue ),
					Gray      = ( Black     | FOREGROUND_INTENSITY ),
					Blue      = ( D_Blue    | FOREGROUND_INTENSITY ),
					Green     = ( D_Green   | FOREGROUND_INTENSITY ),
					Cyan      = ( D_Cyan    | FOREGROUND_INTENSITY ),
					Red       = ( D_Red     | FOREGROUND_INTENSITY ),
					Magenta   = ( D_Magenta | FOREGROUND_INTENSITY ),
					Yellow    = ( D_Yellow  | FOREGROUND_INTENSITY ),
					White     = ( D_White   | FOREGROUND_INTENSITY ),
					Default   =   D_White,

		// Support for all capital colors
		BLACK    = Black    , D_BLUE  = D_Blue  , D_GREEN   = D_Green,
		D_CYAN   = D_Cyan   , D_RED   = D_Red   , D_MAGENTA = D_Magenta,
		D_YELLOW = D_Yellow , D_WHITE = D_White ,
		GRAY     = Gray     , BLUE    = Blue    , GREEN     = Green,
		CYAN     = Cyan     , RED     = Red     , MAGENTA   = Magenta,
		YELLOW   = Yellow   , WHITE   = White   , DEFAULT   = Default
	};

	// 0=======================================================================0
    // | enum Color_BG                                                         |
	// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
	// |   All of the available background colors.                             |
    // 0=======================================================================0
	enum Color_BG { BG_Black     = (0),
		            BG_D_Blue    = ( BACKGROUND_BLUE  ),
					BG_D_Green   = ( BACKGROUND_GREEN ),
					BG_D_Cyan    = ( BG_D_Green | BG_D_Blue ),
					BG_D_Red     = ( BACKGROUND_RED   ),
					BG_D_Magenta = ( BG_D_Red | BG_D_Blue   ),
					BG_D_Yellow  = ( BG_D_Red | BG_D_Green  ),
					BG_D_White   = ( BG_D_Red | BG_D_Green | BG_D_Blue ),
					BG_Gray      = ( Black        | BACKGROUND_INTENSITY ),
					BG_Blue      = ( BG_D_Blue    | BACKGROUND_INTENSITY ),
					BG_Green     = ( BG_D_Green   | BACKGROUND_INTENSITY ),
					BG_Cyan      = ( BG_D_Cyan    | BACKGROUND_INTENSITY ),
					BG_Red       = ( BG_D_Red     | BACKGROUND_INTENSITY ),
					BG_Magenta   = ( BG_D_Magenta | BACKGROUND_INTENSITY ),
					BG_Yellow    = ( BG_D_Yellow  | BACKGROUND_INTENSITY ),
					BG_White     = ( BG_D_White   | BACKGROUND_INTENSITY ),
					BG_Default   =   BG_Black,

		// Support for all capital colors
		BG_BLACK    = BG_Black    , BG_D_BLUE    = BG_D_Blue,
		BG_D_GREEN  = BG_D_Green  , BG_D_CYAN    = BG_D_Cyan,
		BG_D_RED    = BG_D_Red    , BG_D_MAGENTA = BG_D_Magenta,
		BG_D_YELLOW = BG_D_Yellow , BG_D_WHITE   = BG_D_White,
		BG_GRAY     = BG_Gray     , BG_BLUE      = BG_Blue,
		BG_GREEN    = BG_Green    , BG_CYAN      = BG_Cyan,
		BG_RED      = BG_Red      , BG_MAGENTA   = BG_Magenta,
		BG_YELLOW   = BG_Yellow   , BG_WHITE     = BG_White,
		BG_DEFAULT  = BG_Default
	};

	// 0=======================================================================0
    // | static class Color_Functions : Color                                  |
	// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
	// |   A class that handles all of the main functions for the color class. |
	// |    Color is also available outside of Console_Color.h so you can call |
	// |    these functions on your own.                                       |
    // 0=======================================================================0
    static class Color_Functions
    {
        private:
			// 0===============================================================0
			// | ~ VARIABLES ~                                                 |
			// 0=====0===================================================0=====0
			//       |                                                   |      
			// 0=====0===================================================0=====0
			// | HANDLE hStdOut                                                |
			// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
			// |   Stores the STD_OUTPUT_HANDLE.                               |
			// 0===============================================================0
			HANDLE                      hStdOut;

			// 0===============================================================0
			// | CONSOLE_SCREEN_BUFFER_INFO  csbi                              |
			// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
			// |   Stores the Screens Buffer Info.                             |
			// 0===============================================================0
			CONSOLE_SCREEN_BUFFER_INFO  csbi; 

        public:
			// 0===============================================================0
			// | ~ FUNCTIONS ~                                                 |
			// 0=====0===================================================0=====0
			//       |                                                   |      
			// 0=====0===================================================0=====0
			// | Constructor : Color_Function()                                |
			// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
			// |   Retrieves the Output Handle and Buffer Info.                |
			// 0===============================================================0
			Color_Functions() 
			{ hStdOut  = GetStdHandle( STD_OUTPUT_HANDLE );
			  GetConsoleScreenBufferInfo( hStdOut, &csbi ); }

			// 0===============================================================0
			// | SetColor(Color,Mask)                                          |
			// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
			// |   Sets a single color, foreground or background.              |
			// 0===============================================================0
			void SetColor( WORD wRGBI, WORD Mask )
			{ csbi.wAttributes &= Mask; csbi.wAttributes |= wRGBI; 
			  SetConsoleTextAttribute( hStdOut, csbi.wAttributes ); }

			// 0===============================================================0
			// | SetColors(Color_FG,Color_BG)                                  |
			// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
			// |   Sets both foreground and background.                        |
			// 0===============================================================0
			void SetColors( WORD FG_wRGBI, WORD BG_wRGBI )
			{ SetColor( FG_wRGBI, BG_Mask ); SetColor( BG_wRGBI, FG_Mask ); }

			// 0===============================================================0
			// | SetColor_Default()                                            |
			// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
			// |   Resets back to the original colors.                         |
			// 0===============================================================0
			void SetColor_Default()
			{ SetColor( D_WHITE, BG_Mask ); SetColor( BG_BLACK, FG_Mask ); }

    } Color;  // end of class Color_Functions

	// 0=======================================================================0
    // | Operator Overload: <<                                                 |
	// |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
	// |   Overload the << operator to take in a Color_FG or Color_BG object   |
	// |    With the new color object we then set the color appropriately.     |
    // 0=======================================================================0
	ostream& operator<<(ostream& os, Color_FG Cfg)
	{ os.flush(); Color.SetColor(Cfg,BG_Mask); return os; }

	ostream& operator<<(ostream& os, Color_BG Cbg)
	{ os.flush(); Color.SetColor(Cbg,FG_Mask); return os; }

}  // end of namespace Console_Color

// 0===========================================================================0
// | Include the namespace so the user doesn't have too.                       |
// 0===========================================================================0
using namespace ConsoleColor;

#endif  // end of REDJAGUAR_JS_CONSOLECOLOR_H_