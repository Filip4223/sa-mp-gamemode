/*  SA:MP RolePlay GameMode
*   -
*   Author: 	  Filip Szymanski (Discord: Filip#0996/aventez#5327)
*   Published: 	  21.12.2018
*   Version:      1.0
*	Credits:	  Shelby, promsters 
* 	-
*	If you delete this footer, add author to credits. Respect others work.

//%s zabezpieczenie w dialogach
//sugestie po sesjach
//nazwy rang przy nickach, /a, globalne spawny
*/

//Loading includes
#include <a_samp>
#include <a_mysql>
#include <streamer>
#include <sscanf2>

#include <YSI\y_iterate>
#include <YSI\y_timers>

#include <md5>
#include <Pawn.CMD>
#include <geolocation>

//Macros
new sprintfstr[1024];
#define sprintf(%0,%1) (format(sprintfstr, 1024, %0, %1), sprintfstr) //Shorter format function, possible to use in argument

#define Timer_Start(%0) new timer_%0 = GetTickCount()
#define Timer_End(%0) (GetTickCount() - timer_%0) 		//For use in functions

#define toupper(%0) \
    (((%0) >= 'a' && (%0) <= 'z') ? ((%0) & ~0x20) : (%0)) //Letter to upper

#define tolower(%0) \
    (((%0) >= 'A' && (%0) <= 'Z') ? ((%0) | 0x20) : (%0)) //Letter to lower

#define isletter(%0) \
    (((%0) >= 0x41 && (%0) <= 0x5A) || (%0) >= 0x61 && (%0) <= 0x7A) ? (true) : (false)

//Loading modules
#include "modules/config.inc"			//All configuration and definitions etc.
#include "modules/enums.inc"			//All enumerators
#include "modules/mysql_other.inc"		//Useful functions for MySQL
#include "modules/basic_functions.inc"	//Basic script functions, mainly relating to gamemode
#include "modules/labels.inc"			//3D texts 
#include "modules/areas.inc"			//Areas management
#include "modules/player_functions.inc" //All player functions, mostly main player's publics
#include "modules/misc.inc"				//Miscellaneous things
#include "modules/multilingualism.inc" 	//Support for many languages
#include "modules/dialogs.inc"			//Public OnDialogResponse
#include "modules/timers.inc"			//Timers
#include "modules/logs.inc"				//Logs management
#include "modules/cmd.inc"				//Everything about commands
#include "modules/tests.inc"			//Automatic tests

main() {}
