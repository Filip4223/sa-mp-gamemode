stock PlayerName(playerid) {
	new name[MAX_PLAYER_NAME+1];
	GetPlayerName(playerid, name, sizeof(name));
	return _:name;
}

stock BeautifyName(playerid) {
	new name[MAX_PLAYER_NAME], dname[MAX_PLAYER_NAME], dsurname[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));

	sscanf(name, "p<_>s[24]s[24]", dname, dsurname);

	dname[0] = toupper(dname[0]);
	dsurname[0] = toupper(dsurname[0]);

	format(name, sizeof(name), "%s %s", dname, dsurname);
	return _:name;
}

stock GetPlayerAddress(playerid)
{
	new ip[16];
	GetPlayerIp(playerid, ip, sizeof(ip));

	return _:ip;
}

stock GetPlayerCountryName(playerid)
{
    new country[16];
    GetPlayerCountry(playerid, country, sizeof(country));

    return _:country;
}

stock Debug(str[])
{
	if(dev_mode) {
		SendClientMessageToAll(0x7F0000FF, sprintf("[Debug] %s", str));
		print(sprintf("[Debug] %s", str));
	}
	else print(sprintf("[Debug] %s", str)); 

	return true;
}

stock extended_SendClientMessage(playerid, color, message[], length = sizeof message)
{	
	if( strlen(message) > 100 )
	{
		new last_whitespace = -1, line[100];
		for(new i;i < strlen(message); i++)
		{
			if( i > 60 && message[i] == ' ' ) last_whitespace = i;
			
			if( i > 80 )
			{
				if( last_whitespace == -1 ) strmid(line, message, 0, 80);
				else strmid(line, message, 0, last_whitespace);
				
				SendClientMessage(playerid, color, sprintf("%s ...", line));
				
				if( last_whitespace == -1 ) strmid(line, message, 80, length);
				else strmid(line, message, last_whitespace, length);
				
				SendClientMessage(playerid, color, sprintf("... %s", line));
				
				break;
			}			
		}
	}
	else SendClientMessage(playerid, color, message);

	return 1;
}

stock CommandError(playerid)
{
	SendClientMessage(playerid, 0x9E2F2FFF, GetStringValue(STRING_COMMAND_ERROR, playerid));

	return 1;
}

stock ShowPlayerInformation(playerid, str[])
{
	ShowPlayerDialog(playerid, 0, DIALOG_STYLE_MSGBOX, "Powiadomienie", str, "Zamknij", "");
	return 1;
}