/**
 * 0===========================================================================0
 * | RJ_Config.h                                                               |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Joseph Stuhr                                            |
 * |   E-mail        : JpDeathBlade@gmail.com                                  |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 09:30 pm (Eastern) on 05/21/2010                        |
 * |   Date Edited   : 03:38 pm (Eastern) on 05/25/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Internal Config file for default variables.                             |
 * 0===========================================================================0
 */

#ifndef  REDJAGUAR_JS_CONFIG_H_		// Check to see if this .h is defined
#define  REDJAGUAR_JS_CONFIG_H_		//  if not, define it

// 0===========================================================================0
// | ~ Includes ~                                                              |
// 0===========================================================================0
#if defined(DEBUG) | defined(_DEBUG)
	#include <iostream>
#endif

namespace RJ_Config
{
	#define RJ_PACKAGE							"RedJaguar"
	#define RJ_VERSION							1.0

	#define RJ_CONFIG_GAME_NAME					"Kitchen Krashers"
	#define RJ_CONFIG_GAME_VERSION				1.0

	enum	RJ_Supported_GraphicsAPI			{ RJ_API_DIRECT3D = 0, RJ_API_MAX };

	#define RJ_CONFIG_API_DEFAULT				RJ_API_DIRECT3D
	#define RJ_CONFIG_API_DIRECT3D_VERSION		D3D_SDK_VERSION

	//Client Defaults
	#define RJ_CONFIG_CLIENT_CLASSNAME			"RedJaguar:WindowClass_KitchenKrashers"
	#define RJ_CONFIG_CLIENT_TITLE				"Atmosphere Games - Kitchen Krashers"
	#define RJ_CONFIG_CLIENT_WIDTH				1024
	#define RJ_CONFIG_CLIENT_HEIGHT				768
	#define RJ_CONFIG_CLIENT_XPOSITION			20
	#define RJ_CONFIG_CLIENT_YPOSITION			20
	#define RJ_CONFIG_CLIENT_FULLSCREEN			true
	#define RJ_CONFIG_CLIENT_ANIMATED			true
	#define RJ_CONFIG_CLIENT_VSYNC				false

	enum	RJ_Supported_Textures			{RJ_TEXTURE_NONE = 0, RJ_TEXTURE_BMP, RJ_TEXTURE_MAX };
	enum	RJ_Mesh_Types					{RJ_MT_UNNAMED, RJ_MT_FISHBLUE, RJ_MT_ICECREAMTUB};
	enum	RJ_Object_Types					{RJ_BASE = -1, RJ_PLAYER = 0, RJ_STONE, RJ_DYNAMIC, RJ_STATIC,
												RJ_SOLIDSPHEREVOLUME, RJ_PLANEVOLUME, RJ_HAZARDSPHEREVOLUME,
												RJ_OBBVOLUME, RJ_GOAL, RJ_OBJECT_MAX };

	enum	RJ_Effect_Types					{RJ_UNDEFINED = 0, RJ_COLLISION, RJ_COLLECTION, RJ_SMOKE, RJ_DRIP, RJ_JELLO,
											RJ_MOTION, RJ_SPLAT, RJ_WAKE, RJ_HAZARD_WATER, RJ_HAZARD_ORANGE, RJ_HAZARD_OIL,
											RJ_HAT, RJ_SCRAPE, RJ_SKID, RJ_BURNER};

	enum    RJ_Effect_Hat					{RJ_HAT_NONE = 0, RJ_HAT_FC, RJ_HAT_COW, RJ_HAT_CHERRY, RJ_HAT_TURKEY,
												RJ_HAT_GUM};
	enum    RJ_Effect_Position				{RJ_POS_NONE = 0, RJ_POS_UP, RJ_POS_DOWN, RJ_POS_LEFT, RJ_POS_RIGHT};

	enum	ActionsPressed					{ AP_UP = 1,
											AP_DOWN = 2, 
											AP_LEFT = 4,
											AP_RIGHT = 8,
											AP_CONFIRM = 16,
											AP_CANCEL = 32,
											AP_PAUSE = 64,
											CAM_FORWARD = 128,
											CAM_BACKWARD = 256,
											CAM_LEFT = 512,
											CAM_RIGHT = 1024,
											CAM_ROTLEFT = 2048,
											CAM_ROTRIGHT = 4096,
											CAM_UP = 8192,
											CAM_DOWN = 16384,
											CAM_PITCHUP = 32768,
											CAM_PITCHDOWN = 65536,
											AP_CAMLOCK = 131072,
											AP_OTHER2 = 262144,
											AP_OTHER3 = 524288,
											AP_OTHER4 = 1048576,
											AP_SPACE = 2097152,
											/*AP_KILLTURN = 4194304,*/
											AP_F1 = 4194304,
											AP_TYPE = 8388608,
											AP_ALT = 16777216,
											AP_TAB = 33554432};

	enum	RJ_INPUT {RJ_INPUT_KEYBOARD = 1, RJ_INPUT_MOUSE = 2, RJ_INPUT_CONTROLLER = 4}; // DO NOT remove the 'RJ' lol
	enum	CONTROLLER_Type {CONTROLLER_KEYBOARD_MOUSE = 0, CONTROLLER_LOGITECH, CONTROLLER_OTHER};

	enum	CAM_TYPE {RJ_CAM_MAIN = 0, RJ_CAM_GOAL};

	enum	SOUND_FILE {RJ_SOUND_NA = 0, RJ_MUSIC_CHASE, RJ_SOUND_POWERSELECT, RJ_SOUND_POWERRELEASE,
		RJ_AMBIENT_LVL1FREEZER, RJ_COLLISION_ANTHONY,RJ_SWEEP_ANTHONY, RJ_POWERUP_ANTHONY,
		RJ_ANTHONY_RIDEIDLE, RJ_UI_SELECT, RJ_UI_NAVIGATE, RJ_UI_INCREASE, RJ_UI_DECREASE,
		RJ_BULLSEYE, RJ_SUGAR_1, RJ_SUGAR_2, RJ_SUGAR_3, RJ_COLLIDE_ICECREAM, RJ_COLLIDE_FISH,
		RJ_COLLIDE_FREEZERSEAL, RJ_COLLIDE_ICECUBE,	RJ_COLLIDE_ICETRAY, RJ_COLLIDE_PEAS, RJ_COLLIDE_BITTEN,
		RJ_COLLIDE_WHOLE, RJ_COLLIDE_PIZZA,	RJ_COLLIDE_POPSICLE, RJ_COLLIDE_SODA, RJ_POWERUP_WHIPCREAM,
		RJ_POWERUP_CHERRIES, RJ_POWERUP_GUM, RJ_POWERUP_FORTUNECOOKIE, RJ_COLLIDE_CARROT, RJ_COLLIDE_GELATIN,
		RJ_COLLIDE_FRUIT, RJ_COLLIDE_KNIFE, RJ_COLLIDE_KITCHENSINK, RJ_COLLIDE_LIGHTSWITCH, RJ_COLLIDE_JUICECUP,
		RJ_COLLIDE_CEREALBOX, RJ_COLLIDE_COFFEEPOT, RJ_ANTHONY_BROWNIEPOINT, RJ_ANTHONY_ANGLE, RJ_ANTHONY_MOUNT,
		RJ_ANTHONY_LAUNCH, RJ_ANTHONY_HAZARD, RJ_ANTHONY_SCRAPE, RJ_AMBIENT_LVL2KITCHEN, RJ_GUI_SCORETALLY,
		RJ_GUI_SCORETOTAL, RJ_POWERUP_BUTTER, RJ_POWERUP_CARROT, RJ_POWERUP_PEANUT, RJ_POWERUP_COLDTURKEYUSE,
		RJ_ANTHONY_RIDEWATER, RJ_ANTHONY_CHEER1, RJ_ANTHONY_CHEER2, RJ_ANTHONY_CHEER3, RJ_AMBIENT_SINKDRIP,
		RJ_GUI_FANFARE3, RJ_COLLIDE_DUCK, RJ_COLLIDE_STONE, RJ_GUI_TOASTUP, RJ_GUI_TOASTDOWN,
		RJ_POWERUP_COLDTURKEY, RJ_ANTHONY_CHEER4};

	enum	ANIM_TYPE{RJ_ANIM_NA = 0, RJ_ANIM_ANTHONY, RJ_ANIM_GUMMY, RJ_ANIM_TOOTHPICK, RJ_ANIM_BROOM, RJ_SPECTATOR};
	enum	ANTHONY_ANIM_STATES{ANTH_ANIM_NA = 0, ANTH_ANIM_IDLE, ANTH_ANIM_IDLERIDE, ANTH_ANIM_LAUNCH,
			ANTH_ANIM_PUSHBACK, ANTH_ANIM_IDLETOGUMMY, ANTH_ANIM_SWEEPFORWARD, ANTH_ANIM_SWEEPLEFT, 
			ANTH_ANIM_SWEEPRIGHT, ANTH_ANIM_ENDSWEEP, ANTH_ANIM_COLLITION, ANTH_ANIM_WIN, ANTH_ANIM_FOUL,
			ANTH_ANIM_SCRAPE};
	enum	GUMMY_ANIM_STATES{GUMMY_ANIM_NA, GUMMY_ANIM_LAUNCH, GUMMY_ANIM_IDLE, GUMMY_ANIM_RELEASE};
	enum	BROOM_ANIM_STATES{BROOM_ANIM_NA = 0, BROOM_ANIM_IDLE, BROOM_ANIM_IDLERIDE, BROOM_ANIM_LAUNCH,
			BROOM_ANIM_PUSHBACK, BROOM_ANIM_IDLETOGUMMY, BROOM_ANIM_SWEEPFORWARD, BROOM_ANIM_SWEEPLEFT, 
			BROOM_ANIM_SWEEPRIGHT, BROOM_ANIM_ENDSWEEP, BROOM_ANIM_COLLITION, BROOM_ANIM_WIN, BROOM_ANIM_FOUL,
			BROOM_ANIM_SCRAPE};
	enum	TOOTH_ANIM_STATES{TOOTH_ANIM_NA, TOOTH_ANIM_LAUNCH, TOOTH_ANIM_IDLE, TOOTH_ANIM_RELEASE};
	enum	SPECTATOR_ANIM_STATES{SPEC_ANIM_NA, SPEC_ANIM_IDLE, SPEC_ANIM_CHEER1, SPEC_ANIM_CHEER2, SPEC_ANIM_CHEER3, SPEC_ANIM_CHEER4};
	
	//enable DLL Exports
	//#define RJ_DLL

}

#endif //REDJAGUAR_JS_CONFIG_H_