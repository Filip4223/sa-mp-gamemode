/*
 * Log types
 * -
 * 0 - player
 * 1 - vehicles
 * 2 - damage
 * 3 - commands
*/

stock PrintLog(playerid, type, content[])
{
	new day, month, year, hours, minutes, seconds;
	getdate(year, month, day);
	gettime(hours, minutes, seconds);

	new path[64];
	format(path, sizeof(path), "logs/%s-%d.%d.%d.txt", PlayerName(playerid), day, month, year);
	new File:handle = fopen(path, io_readwrite);

	if(handle) {
		fseek(handle, 0, seek_end);

		new string[512];
		format(string, sizeof(string), "[%02d:%02d:%02d] [%s] [%s] %s\n", hours, minutes, seconds, GetTypeString(type), PlayerName(playerid), content);

		fwrite(handle, string);
		fclose(handle);
	}
	else {
		printf("[ERROR] [%s] The file [%s] can not be opened.", PlayerName(playerid), sprintf("logs/%s-%d.%d.%d.txt", PlayerName(playerid), day, month, year));
	}
}

stock GetTypeString(type)
{
	new str[8];

	switch(type)
	{
		case 0: format(str, sizeof(str), "player");
		case 1: format(str, sizeof(str), "veh");
		case 2: format(str, sizeof(str), "dmg");
		case 3: format(str, sizeof(str), "cmdl");
	}

	return _:str;
}