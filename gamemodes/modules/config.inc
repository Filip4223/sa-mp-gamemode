new bool:dev_mode = true;

#define SCRIPT_TAG 				"HRP"
#define SCRIPT_VERSION			"1.0"
#define SCRIPT_NAME				"Honest RolePlay"
#define SCRIPT_NAME_SMALL		"honest-rp.pl"
#define SCRIPT_MAIN_COLOR		"{E5DA67}"

//Language's
#define LAN_ENG		0
#define LAN_PL		1

//String definitions
#define STRING_WELCOME				0
#define STRING_LOGIN_HEADER			1
#define STRING_LOGIN 				2
#define STRING_LOGIN_BTN_1			3
#define STRING_LOGIN_BTN_2			4
#define STRING_BAD_PASSWORD 		5
#define STRING_LOGIN_BTN_CHANGE		6
#define STRING_LOGIN_NO_ACC			7
#define STRING_CHANGENAME			8
#define STRING_NICK_ERROR			9
#define STRING_COMMAND_ERROR		10

//Main MySQL handler
new MySQL:m_sql;

//Dialogs
#define DIALOG_NULL				0
#define DIALOG_LOGIN 			1
#define DIALOG_LOGIN_ERROR  	2
#define DIALOG_CHANGENAME		3

//Saves types
#define SAVE_TYPE_BASIC			1
#define SAVE_TYPE_POS			2

//Iterators
#define MAX_AREAS 4000
new Iterator:Areas<MAX_AREAS>;

//Owner types
#define OWNER_TYPE_PLAYER	0
#define OWNER_TYPE_GROUP	1