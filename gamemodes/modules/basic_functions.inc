public OnGameModeInit()
{
	Timer_Start(OnGameModeInit);
	print("["SCRIPT_TAG"] "SCRIPT_NAME" v"SCRIPT_VERSION" has been turned on.\n["SCRIPT_TAG"] Configuration loading started.\n-");
	
	SetGameModeText("["SCRIPT_TAG"]" "v"SCRIPT_VERSION);

	if(!LoadMySQL()) return 1;
	LoadSettings();
	LoadConfiguration();
	LoadAreas();

	printf("["SCRIPT_TAG"] "SCRIPT_NAME" v"SCRIPT_VERSION" has been fully loaded in %d milliseconds.", Timer_End(OnGameModeInit));

	return 1;
}

public OnGameModeExit()
{
	print("["SCRIPT_TAG"] "SCRIPT_NAME" v"SCRIPT_VERSION" has been turned off.");

	return 1;
}

//Global setting's;
stock LoadSettings()
{
	Setting[setting_weather] = 1;
	Setting[setting_time] = 12;
	Setting[setting_show_nametags] = false;
	Setting[setting_markers] = PLAYER_MARKERS_MODE_OFF;
	Setting[setting_nametags_drawdistance] = 10.0;
	Setting[setting_stunts] = false;
	Setting[setting_default_interiors] = false;
}

stock LoadConfiguration()
{
	ShowPlayerMarkers(Setting[setting_markers]);
	ShowNameTags(Setting[setting_show_nametags]);
	SetWeather(Setting[setting_weather]);
	SetWorldTime(Setting[setting_time]);
	SetNameTagDrawDistance(Setting[setting_nametags_drawdistance]);
	EnableStuntBonusForAll(Setting[setting_stunts]);
	if(!Setting[setting_default_interiors]) DisableInteriorEnterExits();
}

stock LoadMySQL()
{
	mysql_log(ALL);
	m_sql = mysql_connect_file("mysql.ini"); //PATH: root server folder

	if(m_sql == MYSQL_INVALID_HANDLE || mysql_errno(m_sql) != 0) {
		printf("["SCRIPT_TAG"] "SCRIPT_NAME" v"SCRIPT_VERSION" - [MySQL] - Failed to connect to database. [%d]", mysql_errno(m_sql));
		return false;
	}
	else print("["SCRIPT_TAG"] "SCRIPT_NAME" v"SCRIPT_VERSION" - [MySQL] - Connected successfully to the database.\n-");

	return true;
}