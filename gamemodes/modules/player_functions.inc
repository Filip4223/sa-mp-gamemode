/* ------------- *
 * -- PUBLICS -- * 
 * ------------- */

public OnPlayerConnect(playerid)
{
	if(!strcmp(GetPlayerCountryName(playerid), "Poland")) pPlayer[playerid][player_language] = LAN_PL;
	else pPlayer[playerid][player_language] = LAN_ENG;

	if(dev_mode) pPlayer[playerid][player_language] = LAN_PL; //FOR DEVELOPERS

	new Cache:result;
	result = mysql_query(m_sql, sprintf("SELECT `player_uid` FROM samp_players WHERE player_name = '%s'", PlayerName(playerid)));

	if(cache_get_rows())
		ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_LOGIN, playerid), GetStringValue(STRING_LOGIN_BTN_1, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));		
	else
		ShowPlayerDialog(playerid, DIALOG_LOGIN_ERROR, DIALOG_STYLE_INPUT, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_LOGIN_NO_ACC, playerid), GetStringValue(STRING_LOGIN_BTN_CHANGE, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));
	

	cache_delete(result);
	
	PlayAudioStreamForPlayer(playerid, "https://archive.org/download/honest-rp/honest-rp_vbr.m3u");
	for(new i = 0; i<50; i++) SendClientMessage(playerid, -1, "");

	TogglePlayerSpectating(playerid, 1);

	defer InitialCamera[1](playerid, 0);

	pPlayer[playerid][player_logged] = false;

	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	SavePlayer(playerid, SAVE_TYPE_BASIC);
	SavePlayer(playerid, SAVE_TYPE_POS);

	pPlayer[playerid][player_uid] = 0;
	pPlayer[playerid][player_language] = 0;
	format(pPlayer[playerid][player_pass], 24, "");
	format(pPlayer[playerid][player_name], 128, "");

	mysql_pquery(m_sql, sprintf("UPDATE `samp_sessions` SET `session_end` = %d, `session_reason` = %d WHERE `session_uid` = %d", gettime(), reason, pPlayer[playerid][player_session_uid]));
	return 1;
}

public OnPlayerText(playerid, text[])
{
	LocalMessage(playerid, text, 0);

	return 0;
}

public OnPlayerCommandReceived(playerid, cmd[], params[], flags) 
{ 

    return 1; 
} 

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags) 
{ 
    if(result == -1) 
    { 
    	Debug(sprintf("%d", pPlayer[playerid][player_area]));
    	//DODAC INFO ZE NIE ZNALEZIONO INFORMACJI



        return 0; 
    } 

    return 1; 
} 

/* ------------- *
 * --- STOCKS -- * 
 * ------------- */

stock OnPlayerLoginHere(playerid)
{
	new Cache:result;
	result = mysql_query(m_sql, sprintf("SELECT * FROM samp_players WHERE player_name = '%s'", PlayerName(playerid)));

	pPlayer[playerid][player_uid] = cache_get_int(0, "player_uid");
	pPlayer[playerid][player_language] = cache_get_int(0, "player_language");
	pPlayer[playerid][player_skin] = cache_get_int(0, "player_skin");
	pPlayer[playerid][player_level] = cache_get_int(0, "player_level");
	pPlayer[playerid][player_cash] = cache_get_int(0, "player_cash");
	pPlayer[playerid][player_bank_cash] = cache_get_int(0, "player_bank_cash");
	pPlayer[playerid][player_pos][0] = cache_get_float(0, "player_pos_x");
	pPlayer[playerid][player_pos][1] = cache_get_float(0, "player_pos_y");
	pPlayer[playerid][player_pos][2] = cache_get_float(0, "player_pos_z");
	pPlayer[playerid][player_pos][3] = cache_get_float(0, "player_pos_a");
	cache_get(0, "player_pass", pPlayer[playerid][player_pass]);
	cache_get(0, "player_name", pPlayer[playerid][player_name]);

	cache_delete(result);

	StopAudioStreamForPlayer(playerid);

	SendClientMessage(playerid, 0xFFFFFFFF, GetStringValue(STRING_WELCOME, playerid));

	TogglePlayerSpectating(playerid, 0);
	SetSpawnInfo(playerid, 0, pPlayer[playerid][player_skin], pPlayer[playerid][player_pos][0], pPlayer[playerid][player_pos][1], pPlayer[playerid][player_pos][2], pPlayer[playerid][player_pos][3], 0, 0, 0, 0, 0, 0);
	SpawnPlayer(playerid);

	PrintLog(playerid, 0, "has been logged in.");

	pPlayer[playerid][player_logged] = true;

	CreatePlayerLabel(playerid);
	CreateSession(playerid);
}

stock CreateSession(playerid)
{
	new Cache:result;
	result = mysql_query(m_sql, sprintf("INSERT INTO `samp_sessions` VALUES (null, %d, %d, 0, 0, '%s')", pPlayer[playerid][player_uid], gettime(), GetPlayerAddress(playerid)));

	pPlayer[playerid][player_session_uid] = cache_insert_id();

	cache_delete(result);
}

stock SavePlayer(playerid, type)
{
	if(type & SAVE_TYPE_BASIC)
	{
		mysql_query(m_sql, sprintf("UPDATE samp_players SET player_cash = '%d', player_bank_cash = '%d', player_skin = '%d' WHERE player_uid = '%d'", pPlayer[playerid][player_cash], pPlayer[playerid][player_bank_cash], pPlayer[playerid][player_skin], pPlayer[playerid][player_uid]));
	}

	if(type & SAVE_TYPE_POS)
	{
		new
			Float:x,
			Float:y,
			Float:z,
			Float:a;

		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);

		mysql_query(m_sql, sprintf("UPDATE samp_players SET player_pos_x = '%f', player_pos_y = '%f', player_pos_z = '%f', player_pos_a = '%f' WHERE player_uid = '%d'", x, y, z, a, pPlayer[playerid][player_uid]));
	}
}

stock LocalMessage(playerid, text[], type)
{
	static Float:lm_distances[][5] = 
	{
		{ 10.0, 8.0, 6.0, 4.0, 2.0 },		//Local
		{ 30.0, 24.0, 18.0, 12.0, 6.0 },	//Shout
		{ 5.0, 4.0, 3.0, 2.0, 1.0 },		//Whisper
		{ 3.0, 2.0, 1.0, 0.0, 0.0 }			//Silent whisper
	};

	static lm_colors[][5] =
	{
		{ 0xFFFFFFFF, 0xB2B2B2FF, 0x9E9E9EFF, 0x8C8C8CFF, 0x737373FF },
		{ 0xFFFFFFFF, 0xB2B2B2FF, 0x9E9E9EFF, 0x8C8C8CFF, 0x737373FF },
		{ 0xFFFFFFFF, 0xB2B2B2FF, 0x9E9E9EFF, 0x8C8C8CFF, 0x737373FF },
		{ 0xFFFFFFFF, 0xB2B2B2FF, 0x9E9E9EFF, 0x8C8C8CFF, 0x737373FF }
	};

	static lm_texts[][16] =
	{
		"mówi",
		"krzyczy",
		"szepcze",
		"cicho szepcze"
	};

	new
		Float:x,
		Float:y,
		Float:z,
		vw = GetPlayerVirtualWorld(playerid),
		interior = GetPlayerInterior(playerid);

	GetPlayerPos(playerid, x, y, z);

	new string[256];
	format(string, sizeof(string), text);

	string[0] = toupper(string[0]);

	if(isletter(string[strlen(string)-1])) strcat(string, ".");

	new final_text[256];
	format(final_text, sizeof(final_text), "%s %s: %s", BeautifyName(playerid), lm_texts[type], string);

	foreach(new p : Player)
	{
		if( !pPlayer[p][player_logged] ) continue;
		if( GetPlayerVirtualWorld(p) != vw || GetPlayerInterior(p) != interior ) continue;
		new Float:distance = GetPlayerDistanceFromPoint(p, x, y, z);

		if( distance > lm_distances[type][0] ) continue;

		new color;

		if( distance <= lm_distances[type][0] ) color = lm_colors[type][4];
		if( distance <= lm_distances[type][1] ) color = lm_colors[type][3];
		if( distance <= lm_distances[type][2] ) color = lm_colors[type][2];
		if( distance <= lm_distances[type][3] ) color = lm_colors[type][1];
		if( distance <= lm_distances[type][4] ) color = lm_colors[type][0];

		new area[8];
		if(pPlayer[playerid][player_area] != -1) format(area, sizeof(area), pArea[pPlayer[playerid][player_area]][area_name]);
		else format(area, sizeof(area), "NULL");

		PrintLog(playerid, 0, sprintf("[chat] [a:%s] %s", area, text));

		extended_SendClientMessage(p, color, final_text);
	}

	return 0;
}

/* ------------- *
 * - FUNCTIONS - * 
 * -------------- */

forward AuthPlayer(playerid, const text[]);
public AuthPlayer(playerid, const text[])
{
	if(!strcmp(text, cache_get_data(0, "player_pass"), false)) {
		OnPlayerLoginHere(playerid);
	}			
	else {
		GameTextForPlayer(playerid, GetStringValue(STRING_BAD_PASSWORD, playerid), 1000, 4);
		ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_LOGIN, playerid), GetStringValue(STRING_LOGIN_BTN_1, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));
	}
	return 1;
}
