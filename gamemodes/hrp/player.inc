stock OutputLoginForm(playerid, bool:do_query=false)
{
	if( do_query )
	{
		SetPlayerVirtualWorld(playerid, 0);
		SetPlayerInterior(playerid, 0);

		pInfo[playerid][player_login_camera_crc] = 0;
		LoginCameraMove(playerid, pInfo[playerid][player_login_camera_crc]);

    	TextDrawShowForPlayer(playerid, Panorama[0]);
    	TextDrawShowForPlayer(playerid, Panorama[1]);

    	PlayAudioStreamForPlayer(playerid, "https://archive.org/download/honest-rp/honest-rp_vbr.m3u");
    	for(new i;i<50;i++) SendClientMessage(playerid, -1, " ");

		new name_escaped[MAX_PLAYER_NAME+1];
		strcopy(name_escaped, pInfo[playerid][player_name], MAX_PLAYER_NAME+1);
		strreplace_char(name_escaped, ' ', '_');

		new Cache:result;

		if( strfind(name_escaped, "_") == -1 )
		{
			// global login
			result = mysql_query(g_sql, sprintf("SELECT m.name, m.member_id, m.member_premium_time, m.member_game_points FROM core_members m WHERE m.name = '%s'", name_escaped));

			pInfo[playerid][player_login_method] = LoginMethod::GLOBAL;
		}
		else
		{
			// character login
			result = mysql_query(g_sql, sprintf("SELECT ch.char_uid, ch.char_gid, m.name, m.member_id, m.member_premium_time, m.member_game_points FROM crp_characters ch INNER JOIN core_members m ON ch.char_gid = m.member_id WHERE ch.char_name = '%s' AND ch.char_gid > 0", name_escaped));
		
			pInfo[playerid][player_login_method] = LoginMethod::CHARACTER;
		}
	
		if( cache_get_rows() )
		{
			// Getting needed data
			cache_get(0, "name", gInfo[playerid][global_name]);
			
			if( pInfo[playerid][player_login_method] == LoginMethod::CHARACTER ) {
				pInfo[playerid][player_id] = cache_get_int(0, "char_uid");	
			}

			gInfo[playerid][global_id] = cache_get_int(0, "member_id");
			gInfo[playerid][global_premium_time] = cache_get_int(0, "member_premium_time");
			gInfo[playerid][global_game_points] = cache_get_int(0, "member_game_points");
			
			gInfo[playerid][global_registered] = true;
		}
		
		cache_delete(result);

		
		// Session start in here
		if( gInfo[playerid][global_registered] )
		{
			if( pInfo[playerid][player_called_incoming] && !pInfo[playerid][player_changed_nick]  ) RemoveBuildingsForPlayer(playerid);

			pInfo[playerid][player_changed_nick] = false;

			// Sprawdzamy czy ma bana
			result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_bans` WHERE `ban_owner` = %d", gInfo[playerid][global_id]));
			
			if( cache_get_rows() )
			{
				new reason[130];
				cache_get(0, "ban_reason", reason);

				SendFormattedClientMessage(playerid, COLOR_RED, "Twoje konto jest zbanowane, powód: %s.", reason);
				SendClientMessage(playerid, COLOR_RED, "Je¿eli kara zosta³a nadana nies³usznie - mo¿esz apelowaæ na naszym forum.");
				SendClientMessage(playerid, COLOR_RED, "Aby dezaktywowaæ karê lub z³o¿yæ apelacje udaj siê na nasze forum.");
				SendClientMessage(playerid, COLOR_RED, "Adres naszej strony: "PAGE_URL". Pamiêtaj, by nie za³atwiaæ takich spraw w grze!");
				
				Kick(playerid);
				
				cache_delete(result);
				return 1;
			}

			cache_delete(result);

			// tworzymy sesje
			sInfo[playerid][session_start_time] = gettime();
			sInfo[playerid][session_state] = SESSION_STATE_NONE;
			
		    new ip[16];
		    GetPlayerIp(playerid, ip, sizeof(ip));

			result = mysql_query(g_sql, sprintf("INSERT INTO `crp_sessions` (session_uid, session_type, session_extraid, session_owner, session_start, session_ip) VALUES (null, %d, %d, %d, %d, '%s')", SESSION_TYPE_LOGIN, sInfo[playerid][session_state], pInfo[playerid][player_id], sInfo[playerid][session_start_time], ip));
			sInfo[playerid][session_id] = cache_insert_id();

			cache_delete(result);
		}
		else
		{
			TextDrawShowForPlayer(playerid, HonestLogoLogin);

			new ip[16], string[512];
			GetPlayerIp(playerid, ip, sizeof(ip));

			SendClientMessage(playerid, -1, "aa");

			mysql_pquery(g_sql, sprintf("SELECT session_owner, char_name, char_hours FROM crp_sessions LEFT JOIN crp_characters ON char_uid = session_owner WHERE session_ip = '%s' AND session_type = 1 AND session_start > %d GROUP BY session_owner LIMIT 5", ip, (gettime() - 86400)), "OnSuggestedCharactersLoaded", "i", playerid);

			return 1;
		}
	}

	new bad_pass_info[60];

	if( gInfo[playerid][global_bad_pass] >= 3 )
	{
		sInfo[playerid][session_state] = SESSION_STATE_BAD_PASS;
		SendClientMessage(playerid, COLOR_LIGHTER_RED, "Zosta³eœ wyrzucony poniewa¿ zbyt du¿o razy wpisa³eœ b³êdne has³o.");
		Kick(playerid);
		
		return 1;
	}
	else if( gInfo[playerid][global_bad_pass] > 0 )
	{
		format(bad_pass_info, sizeof(bad_pass_info), "\n\n"HEX_COLOR_LIGHTER_RED"Poda³eœ b³êdne has³o. Pozosta³o prób: %d/3", 3-gInfo[playerid][global_bad_pass]);
	}
	
	TextDrawShowForPlayer(playerid, HonestLogoLogin);
	
	// show login dialog
	new string[400];
	if( pInfo[playerid][player_login_method] == LoginMethod::CHARACTER )
	{
		strcat(string, ""HEX_COLOR_SMOOTH_RED"Witaj na "PAGE_NAME", polskim serwerze Role Play.\n\n");
		strcat(string, sprintf(""HEX_COLOR_SAMP"Twoja postaæ "HEX_COLOR_WHITE"%s"HEX_COLOR_SAMP" zosta³a odnaleziona w bazie danych.\n", pInfo[playerid][player_name]));
		strcat(string, "1.\tPostaæ jest Twoja? WprowadŸ has³o w celu zalogowania siê.\n");
    }
    else
	{
		strcat(string, ""HEX_COLOR_SMOOTH_RED"Witaj na "PAGE_NAME", polskim serwerze Role Play.\n\n");
		strcat(string, sprintf(""HEX_COLOR_SAMP"Twoje konto globalne "HEX_COLOR_WHITE"%s"HEX_COLOR_SAMP" zosta³o odnalezione w bazie danych.\n", pInfo[playerid][player_name]));
		strcat(string, "1.\tKonto jest Twoje? WprowadŸ has³o w celu zalogowania siê.\n");
    }

    strcat(string, sprintf("2.\tPomyli³eœ nick? WprowadŸ go jeszcze raz klikaj¹c "HEX_COLOR_WHITE"zmieñ nick"HEX_COLOR_SAMP".%s", bad_pass_info));
    
    ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Logowanie", string, "Zaloguj", "Zmieñ nick");
	return 1;
}

stock CheckPasswordCorrectness(playerid, text[])
{
	if( Setting[setting_run_mode] == RunMode::DEV )
	{
		gInfo[playerid][global_logged] = true;
		OnPlayerLoggedIn(playerid);

		SendClientMessage(playerid, -1, ""HEX_COLOR_HONEST"DEV MODE: "HEX_COLOR_WHITE"Zosta³eœ zalogowany bez sprawdzania poprawnoœci has³a.");
	}
	else
	{
		HTTP(playerid, HTTP_POST, LOGIN_AUTH_URL, sprintf("%d|%s", gInfo[playerid][global_id], text), "OnPasswordCheckResponse");
	}
}

function OnSuggestedCharactersLoaded(playerid)
{
	SendClientMessage(playerid, -1, "a");

	new rows = cache_get_rows(), string[512];

	SendClientMessage(playerid, -1, "a");

	if(rows > 0)
	{
		SendClientMessage(playerid, -1, "e");

		DynamicGui_Init(playerid);

		for(new i=0;i<rows;i++)
		{
			new uid = cache_get_int(i, "session_owner");
			new name[MAX_PLAYER_NAME];
			cache_get(i, "char_name", name);

			if(strContains(name, "_")) UnderscoreToSpace(name);

			strins(string, sprintf("{A9C4E4}• %d   %s (%dh)\n", uid, name, cache_get_int(i, "char_hours")), strlen(string));
			DynamicGui_AddRow(playerid, DIALOG_LOGIN_NO_ACCOUNT_SUGGESTED, uid);
		}

		strcat(string, "{FFFFFF}-\n");
		DynamicGui_AddBlankRow(playerid);

		strcat(string, sprintf("{AFAFAF}Niestety nie znaleŸliœmy postaci %s, lecz\n", pInfo[playerid][player_name]));
		DynamicGui_AddBlankRow(playerid);

		strcat(string, "{AFAFAF}powy¿ej przedstawiono postaci, które\n");
		DynamicGui_AddBlankRow(playerid);

		strcat(string, "{AFAFAF}mog¹ nale¿eæ do Ciebie. Zasugerowaliœmy je\n\n");
		DynamicGui_AddBlankRow(playerid);

		strcat(string, "{AFAFAF}na podstawie poprzednich logowañ z\n");
		DynamicGui_AddBlankRow(playerid);

		strcat(string, "{AFAFAF}tego adresu IP. Je¿eli któraœ z nich\n");
		DynamicGui_AddBlankRow(playerid);

		strcat(string, "{AFAFAF}nale¿y do Ciebie wybierz j¹ klikaj¹c\n");
		DynamicGui_AddBlankRow(playerid);

		strcat(string, "{AFAFAF}na ni¹ dwukrotnie.");
		DynamicGui_AddBlankRow(playerid);

		ShowPlayerDialog(playerid, DIALOG_LOGIN_NO_ACCOUNT_SUGGESTED, DIALOG_STYLE_TABLIST, "Logowanie", string, "Zmieñ nick", "Inna postaæ");
	}
	else
	{
		format(string, sizeof(string), HEX_COLOR_SMOOTH_RED"Witaj na "PAGE_NAME", polskim serwerze Role Play.\n\n");
		format(string, sizeof(string), "%s"HEX_COLOR_SAMP"Niestety pod nickiem "HEX_COLOR_WHITE"%s"HEX_COLOR_SAMP" nie mo¿esz kontynuowaæ logowania.\n", string, pInfo[playerid][player_name]);
		format(string, sizeof(string), "%s1.\tPomyli³eœ nick? WprowadŸ go jeszcze raz klikaj¹c "HEX_COLOR_WHITE"zmieñ nick"HEX_COLOR_SAMP".\n", string);
		format(string, sizeof(string), "%s2.\tNie masz jeszcze u nas konta? Za³ó¿ je na "HEX_COLOR_WHITE""PAGE_URL""HEX_COLOR_SAMP"!", string);
		ShowPlayerDialog(playerid, DIALOG_LOGIN_NO_ACCOUNT, DIALOG_STYLE_MSGBOX, "Logowanie", string, "Zmieñ nick", "Zamknij" );
	}
}

function OnPasswordCheckResponse(playerid, response_code, data[])
{
	if( response_code == 200 && !strcmp(data, "OK") )
	{
		gInfo[playerid][global_logged] = true;
		OnPlayerLoggedIn(playerid);
	}
	else
	{
		SendClientMessage(playerid, -1, sprintf("%d|%s", response_code, data));

		gInfo[playerid][global_bad_pass] += 1;
		OutputLoginForm(playerid);
	}
	return 1;
}

stock OnPlayerLoggedIn(playerid)
{
	if( pInfo[playerid][player_login_method] == LoginMethod::CHARACTER ) 
	{
		OnCharacterLoggedIn(playerid);
	}
	else
	{
		mysql_pquery(g_sql, sprintf("SELECT char_uid, char_name, penalty_uid AS is_blocked FROM crp_characters LEFT JOIN crp_penalties ON penalty_owner = char_uid AND penalty_deactivate = 0 AND penalty_type = %d AND penalty_extraid = %d WHERE char_gid = %d", PENALTY_TYPE_BLOCK, BLOCK_CHAR, gInfo[playerid][global_id]), "OnPlayerCharactersLoaded", "d", playerid);
	}
}

function OnPlayerCharactersLoaded(playerid)
{
	if( cache_get_rows() )
	{
		new string[1024], name[32];
		format(string, sizeof(string), ""HEX_COLOR_HONEST"UID\tNick\n");

		DynamicGui_Init(playerid);

		for(new i;i<cache_get_rows();i++)
		{
			if( !cache_is_null(i, "is_blocked") ) continue;

			cache_get(i, "char_name", name);
			UnderscoreToSpace(name);

			format(string, sizeof(string), "%s%d\t%s\n", string, cache_get_int(i, "char_uid"), name);
			DynamicGui_AddRow(playerid, cache_get_int(i, "char_uid"));
		}

		ShowPlayerDialog(playerid, DIALOG_GLOBAL_CHOOSE_CHAR, DIALOG_STYLE_TABLIST_HEADERS, "Wybór postaci", string, "Wybierz", "");
	}
	else
	{
		SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Do tego konta nie jest przypisana ¿adna aktywna postaæ.");
		Kick(playerid);
	}
}

stock OnCharacterLoggedIn(playerid)
{	
	TextDrawHideForPlayer(playerid, HonestLogoLogin);
	TextDrawShowForPlayer(playerid, HonestLogoAboveHud);

	TextDrawHideForPlayer(playerid, Panorama[0]);
	TextDrawHideForPlayer(playerid, Panorama[1]);

	// wczytujemy blocki
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_penalties` WHERE penalty_deactivate = 0 AND penalty_type = %d AND penalty_owner = %d", PENALTY_TYPE_BLOCK, pInfo[playerid][player_id]));

	for(new i;i<cache_get_rows();i++)
	{
		new block_type, endtime;
		block_type = cache_get_int(i, "penalty_extraid");
		endtime = cache_get_int(i, "penalty_end");

		pInfo[playerid][player_block] += block_type;
		setPlayerBlockTime(playerid, block_type, endtime);
	}

	cache_delete(result);

	if( ((pInfo[playerid][player_block] & (BLOCK_CHAR)) == (BLOCK_CHAR)) )
	{
		result = mysql_query(g_sql, sprintf("SELECT * FROM crp_penalties WHERE penalty_type = %d AND penalty_extraid = %d AND penalty_owner = %d LIMIT 1", PENALTY_TYPE_BLOCK, BLOCK_CHAR, pInfo[playerid][player_id]));
		
		new reason[130];
		cache_get(0, "penalty_reason", reason);
		
		SendFormattedClientMessage(playerid, COLOR_RED, "> Ta postaæ jest zablokowana, powód: %s.", reason);
		SendClientMessage(playerid, COLOR_RED, "Je¿eli kara zosta³a nadana nies³usznie - mo¿esz apelowaæ na naszym forum.");
		SendClientMessage(playerid, COLOR_RED, "Aby dezaktywowaæ karê lub z³o¿yæ apelacje udaj siê na nasze forum.");
		SendClientMessage(playerid, COLOR_RED, "Adres naszej strony: www.honest-rp.pl. Pamiêtaj, by nie za³atwiaæ takich spraw w grze!");
		
		Kick(playerid);
		
		cache_delete(result);
		return;
	}

	result = mysql_query(g_sql, sprintf("SELECT * FROM crp_characters WHERE char_uid = %d AND char_gid = %d", pInfo[playerid][player_id], gInfo[playerid][global_id]));
	// test
	if( cache_get_rows() == 1 )
	{
		cache_get(0, "char_name", pInfo[playerid][player_name], MAX_PLAYER_NAME+1);
		
		SetPlayerName(playerid, pInfo[playerid][player_name]);
		UnderscoreToSpace(pInfo[playerid][player_name]);

		// Setting some player data		
		pInfo[playerid][player_id] = cache_get_int(0, "char_uid");
		pInfo[playerid][player_online_time] = cache_get_int(0, "char_hours") * 3600 + cache_get_int(0, "char_minutes") * 60;
		pInfo[playerid][player_last_hour_cpoints] = cache_get_int(0, "char_hours");
		pInfo[playerid][player_skin] = cache_get_int(0, "char_skin");
		pInfo[playerid][player_last_skin] = pInfo[playerid][player_skin];
		
		pInfo[playerid][player_health] = cache_get_float(0, "char_health");
		pInfo[playerid][player_bw] = cache_get_int(0, "char_bw");		
		if( pInfo[playerid][player_bw] > 0 ) pInfo[playerid][player_bw_end_time] = pInfo[playerid][player_bw] + gettime();  
		
		pInfo[playerid][player_aj] = cache_get_int(0, "char_aj");
		if( pInfo[playerid][player_aj] > 0 ) pInfo[playerid][player_aj_end_time] = pInfo[playerid][player_aj] + gettime();  		
		
		pInfo[playerid][player_drunk_time] = cache_get_int(0, "char_drunk");
		
		pInfo[playerid][player_sex] = cache_get_int(0, "char_sex");
		pInfo[playerid][player_age] = 2019 - cache_get_int(0, "char_birth");
		pInfo[playerid][player_strength] = cache_get_int(0, "char_strength");
		pInfo[playerid][player_job] = cache_get_int(0, "char_job");
		pInfo[playerid][player_documents] = cache_get_int(0, "char_documents");
		pInfo[playerid][player_talk_style] = cache_get_int(0, "char_talkstyle");
		pInfo[playerid][player_walk_style] = cache_get_int(0, "char_walkstyle");
		pInfo[playerid][player_door] = cache_get_int(0, "char_door");
		pInfo[playerid][player_keep] = cache_get_int(0, "char_arrest");
		
		pInfo[playerid][player_quit_pos][0] = cache_get_float(0, "char_posx");
		pInfo[playerid][player_quit_pos][1] = cache_get_float(0, "char_posy");
		pInfo[playerid][player_quit_pos][2] = cache_get_float(0, "char_posz");
		pInfo[playerid][player_quit_pos][3] = cache_get_float(0, "char_posa");
		pInfo[playerid][player_quit_vw] = cache_get_int(0, "char_world");
		pInfo[playerid][player_quit_int] = cache_get_int(0, "char_interior");
		pInfo[playerid][player_quit_time] = cache_get_int(0, "char_quittime");
		
		//pInfo[playerid][player_spawn] = mysql_fetch_field_int("spawn");
	
		pInfo[playerid][player_money] = cache_get_int(0, "char_cash");
		pInfo[playerid][player_bank_number] = cache_get_int(0, "char_banknumb");
		pInfo[playerid][player_bank_money] = cache_get_int(0, "char_bankcash");

		pInfo[playerid][player_last_train] = cache_get_int(0, "char_last_train");
		pInfo[playerid][player_freeze_door] = cache_get_bool(0, "char_freeze_door");
		pInfo[playerid][player_show_hints] = cache_get_bool(0, "char_show_hints");

		pInfo[playerid][player_interface] = cache_get_bool(0, "char_interface");
		pInfo[playerid][player_editor] = cache_get_bool(0, "char_editor");
		pInfo[playerid][player_list_favorite_items] = cache_get_bool(0, "char_favorite_items");

		pInfo[playerid][player_level] = cache_get_int(0, "char_level");

		pInfo[playerid][player_debit] = cache_get_int(0, "char_debit");
		pInfo[playerid][player_insurance] = cache_get_int(0, "char_insurance");

		pInfo[playerid][player_damage] = cache_get_int(0, "char_damage");

		pInfo[playerid][player_hospitalization_costs] = cache_get_int(0, "char_hospitalization_costs");

		pInfo[playerid][player_phone_call_started] = false;
		pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
		pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;

		pInfo[playerid][player_edited_object] = -1;
		pInfo[playerid][player_has_animation] = false;

		LoadPlayerGroupData(playerid);
		LoadItem(sprintf("WHERE `item_ownertype` = %d AND `item_owner` = %d", ITEM_OWNER_TYPE_PLAYER, pInfo[playerid][player_id]));
		LoadPlayerBags(playerid);

		new ip[16];
		GetPlayerIp(playerid, ip, sizeof(ip));

		new time = GetPlayerOnlineTime(playerid), hour = floatround(time/3600, floatround_floor);
		time -= hour * 3600;
		new minute = floatround(time/60, floatround_floor);

		PlayerLog(sprintf("Logged in {ID:%d,CASH:$%d,BANK:$%d,PLAYTIME:%dh %dm,IP:%s}", playerid, pInfo[playerid][player_money], pInfo[playerid][player_bank_money], hour, minute, ip), pInfo[playerid][player_id], "basic");

		// Szukamy ubran
		new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_CLOTH);
		if( itemid > -1 )
		{
			pInfo[playerid][player_last_skin] = Item[itemid][item_value1];
		}
		else
		{
			pInfo[playerid][player_last_skin] = pInfo[playerid][player_skin];
		}
		
		itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_ZEGAREK);
		if( itemid > -1 )
		{
			ShowZegarekTd(playerid);
		}
		
		// Set some player's data
		SetPlayerMoney(playerid, pInfo[playerid][player_money]);
		
		SetPlayerSkillLevel(playerid, WEAPONSKILL_PISTOL, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_PISTOL_SILENCED, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_DESERT_EAGLE, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_SHOTGUN, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_SAWNOFF_SHOTGUN, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_SPAS12_SHOTGUN, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_MICRO_UZI, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_MP5, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_AK47, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_M4, 0);
		SetPlayerSkillLevel(playerid, WEAPONSKILL_SNIPERRIFLE, 0);
		
		// Ustawiamy mu label
		Attach3DTextLabelToPlayer(pInfo[playerid][player_label], playerid, 0.0, 0.0, 0.15);
		Attach3DTextLabelToPlayer(pInfo[playerid][player_description_label], playerid, 0.0, 0.0, -0.7);
		UpdatePlayerLabel(playerid);
		
		SetPlayerScore(playerid, gInfo[playerid][global_game_points]);
		
		TextDrawShowForPlayer(playerid, LSNtd);
		TextDrawShowForPlayer(playerid, LSNtd2);
		
		// Ustawiamy ostatnie logowanie jako aktualne
		pInfo[playerid][player_last_join] = gettime();
		mysql_query(g_sql, sprintf("INSERT INTO `crp_logged_players` (char_uid, user_id, seen) VALUES (%d, %d, %d)", pInfo[playerid][player_id], gInfo[playerid][global_id], pInfo[playerid][player_last_join]), false);
		
		// Welcome message for player
		SendFormattedClientMessage(playerid, -1, "** Witaj, "HEX_COLOR_HONEST"%s (GID: %d)"HEX_COLOR_WHITE". Zosta³eœ zalogowany na postaæ "HEX_COLOR_HONEST"%s (UID: %d)"HEX_COLOR_WHITE". ¯yczymy mi³ej gry!", gInfo[playerid][global_name], gInfo[playerid][global_id], pInfo[playerid][player_name], pInfo[playerid][player_id], playerid);
		if(pInfo[playerid][player_level]) SendFormattedClientMessage(playerid, -1, "Posiadasz "HEX_COLOR_HONEST"%d "HEX_COLOR_WHITE"poziom - "HEX_COLOR_HONEST"%s.", pInfo[playerid][player_level], AdminLevels[pInfo[playerid][player_level]]);

		scrp_SpawnPlayer(playerid, false);
		TogglePlayerSpectating(playerid, 0);

		/*GetPlayerPos(playerid, x, y, z);
		SetPlayerVirtualWorld(playerid, 0);
		SetPlayerPos(playerid, x+0.5, y+0.5, z+0.5);
		SetPlayerVirtualWorld(playerid, vw);*/
		
		pInfo[playerid][player_spawn_time] = gettime();
		
		if( IsPlayerVip(playerid) ) TextDrawShowForPlayer(playerid, PlayerVip);
		else TextDrawHideForPlayer(playerid, PlayerVip);
		
		if(!GetPlayerPermissions(playerid, 5))
		{
			foreach(new gs : GlobalSpawns)
			{
				Streamer_AppendArrayData(STREAMER_TYPE_3D_TEXT_LABEL, GlobalSpawn[gs][gspawn_label], E_STREAMER_PLAYER_ID, playerid);
			}
		}
	
		pInfo[playerid][player_logged] = true;
		sInfo[playerid][session_state] = SESSION_STATE_NORMAL;
		
		UpdatePlayerColor(playerid);
	}
	else
	{
		Kick(playerid);
		cache_delete(result);

		return;
	}
	
	cache_delete(result);
	
	result = mysql_query(g_sql, sprintf("SELECT session_start FROM `crp_sessions` WHERE session_extraid = 4 AND session_owner = %d ORDER BY session_end DESC LIMIT 1", pInfo[playerid][player_id]));
	
	new last_logged = 0;
	
	if( cache_get_rows() > 0 ) last_logged = cache_get_int(0, "session_start");
	
	cache_delete(result);
	
	mysql_query(g_sql, sprintf("UPDATE crp_characters SET char_last_join = %d WHERE char_uid = %d", gettime(), pInfo[playerid][player_id]));

	result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_changelog` WHERE change_date >= %d AND change_date <= %d ORDER BY change_date DESC LIMIT 20", last_logged, gettime()));
	
	if( cache_get_rows() > 0 )
	{
		new str[2048], last_change_date = 0;
		
		new 
			year, month, day, hour, minute, second,
			t_year, t_month, t_day, t_hour, t_minute, t_second;
		
		format(str, sizeof(str), "Poni¿ej widzisz listê zmian, które zosta³y wprowadzone od Twojej ostatniej wizyty:\n\n");
		
		for(new i;i<cache_get_rows();i++)
		{
			TimestampToDate(last_change_date, t_year, t_month, t_day, t_hour, t_minute, t_second, 1, 0);	
			TimestampToDate(cache_get_int(i, "change_date"), year, month, day, hour, minute, second, 1, 0);
			
			if( t_year != year || t_month != month || t_day != day )
			{
				format(str, sizeof(str), "%s{3DA876}[%02d.%02d.%d]"HEX_COLOR_SAMP"\n", str, day, month, year);
			}
			
			new desc[80];
			cache_get(i, "change_desc", desc);
			
			format(str, sizeof(str), "%s    - %s\n", str, desc);
			
			last_change_date = cache_get_int(i, "change_date");
		}
		
		SendGuiInformation(playerid, "Changelog", str);
	}
	
	cache_delete(result);
}

stock ShowZegarekTd(playerid)
{
	new hour, minute;
	gettime(hour, minute);
	
	TextDrawSetString(ZegarekTD, sprintf("%02d:%02d", hour, minute));
	
	TextDrawShowForPlayer(playerid, ZegarekTD);
}

stock GetPlayerByUid(p_uid)
{
	foreach(new p : Player)
	{
		if( pInfo[p][player_id] == p_uid ) return p;
	}
	
	return -1;
}

stock ShowPlayerPayment(playerid)
{	
	ShowPlayerDialog(playerid, DIALOG_PAYMENT, DIALOG_STYLE_LIST, "Sposób p³atnoœci", "P³atnoœæ gotówk¹\nP³atnoœæ kart¹ kredytow¹", "Wybierz", "Anuluj");
}

stock OnPlayerPaymentResponse(playerid, type, response)
{
	OnPlayerOfferFinish(playerid, type, response);
	return 1;
}

stock AddPlayerDebit(playerid, amount, desc[] = "undefined")
{
	replacePolishChars(desc);

	if(amount < 0)
	{
		SendClientMessage(playerid, 0x94c684FF, sprintf("[Bank] {AFAFAF}Na Twoim koncie zanotowano sp³atê debetu na kwotê {94c684}$%d{AFAFAF}!", abs(amount)));
		mysql_query(g_sql, sprintf("INSERT INTO crp_transactions VALUES (null, '%d', 2, '%s', '%d', '%d')", pInfo[playerid][player_id], desc, amount, gettime()));
	}
	else
	{
		SendClientMessage(playerid, 0xed6161FF, sprintf("[Bank] {AFAFAF}Na Twoim koncie naliczno debet w kwocie {ed6161}$%d{AFAFAF}!", amount));
		mysql_query(g_sql, sprintf("INSERT INTO crp_transactions VALUES (null, '%d', 3, '%s', '%d', '%d')", pInfo[playerid][player_id], desc, amount, gettime()));		
	}

	pInfo[playerid][player_debit] += amount;
	mysql_query(g_sql, sprintf("UPDATE crp_characters SET char_debit = %d WHERE char_uid = %d", pInfo[playerid][player_debit], pInfo[playerid][player_id]));
}

stock AddPlayerBankMoney(playerid, amount, desc[] = "undefined")
{
	replacePolishChars(desc);
	
	if(amount > 0)
	{
		SendClientMessage(playerid, 0x94c684FF, sprintf("[Bank] {AFAFAF}Na Twoim koncie zanotowano now¹ transakcjê na kwotê {94c684}$%d{AFAFAF}!", amount));
		mysql_query(g_sql, sprintf("INSERT INTO crp_transactions VALUES (null, '%d', 0, '%s', '%d', '%d')", pInfo[playerid][player_id], desc, amount, gettime()));
	}
	else
	{
		SendClientMessage(playerid, 0xed6161FF, sprintf("[Bank] {AFAFAF}Na Twoim koncie zanotowano now¹ transakcjê na kwotê {ed6161}$%d{AFAFAF}!", amount));
		mysql_query(g_sql, sprintf("INSERT INTO crp_transactions VALUES (null, '%d', 1, '%s', '%d', '%d')", pInfo[playerid][player_id], desc, amount, gettime()));
	}

	pInfo[playerid][player_bank_money] += amount;
	
	mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_bankcash` = %d WHERE `char_uid` = %d", pInfo[playerid][player_bank_money], pInfo[playerid][player_id]));
}

stock RefreshPlayer(playerid)
{
	new asd = random(4);
	
	new Float:pos[3];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	
	switch( asd )
	{
		case 0: SetPlayerPos(playerid, pos[0]+0.05, pos[1]+0.05, pos[2]);
		case 1: SetPlayerPos(playerid, pos[0]+0.05, pos[1]-0.05, pos[2]);
		case 2: SetPlayerPos(playerid, pos[0]-0.05, pos[1]-0.05, pos[2]);
		case 3: SetPlayerPos(playerid, pos[0]-0.05, pos[1]+0.05, pos[2]);
	}	
}

stock Float:GetDistanceBetweenPlayers(playerid, targetplayerid)
{
    new 
		Float:x,
		Float:y,
		Float:z;
		
    if( !IsPlayerConnected(playerid) || !IsPlayerConnected(targetplayerid) ) return 1000.00;
	
    GetPlayerPos(playerid, x, y, z);
    return GetPlayerDistanceFromPoint(targetplayerid, x, y, z);
}


stock LoadPlayerGroupData(playerid)
{
	// Pobieramy jego grupê
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT * FROM crp_char_groups WHERE `char_uid` = %d LIMIT 5", pInfo[playerid][player_id]));
	
	new count = cache_get_rows();
	
	for(new i;i<count;i++)
	{		
		pGroup[playerid][i][pg_id] = GetGroupByUid(cache_get_int(i, "group_belongs"));
		
		cache_get(i, "group_title", pGroup[playerid][i][pg_rank_title]);
		pGroup[playerid][i][pg_rank_skin] = cache_get_int(i, "group_skin");
		pGroup[playerid][i][pg_rank_payment] = cache_get_int(i, "group_payment");
		pGroup[playerid][i][pg_rank_perm] = cache_get_int(i, "group_perm");		
		pGroup[playerid][i][pg_last_payday] = cache_get_int(i, "group_payday");
	}
	
	cache_delete(result);
	
	return count;
}

stock SendPlayerInformation(playerid, text[], showTime = 2000)
{
	pInfo[playerid][player_info_td_id]++;

	PlayerTextDrawHide(playerid, pInfo[playerid][informationTd]);

	if( showTime > 0 )
	{
		new str[300];
		format(str, sizeof(str), "~w~%s", text);
		
		PlayerTextDrawSetString(playerid, pInfo[playerid][informationTd], str);
		PlayerTextDrawShow(playerid, pInfo[playerid][informationTd]);

		defer HideInformationTextdraw[showTime](playerid, pInfo[playerid][player_info_td_id]);
	}
	else HideInformationTextdraw(playerid, pInfo[playerid][player_info_td_id]);
	
	return 1;
}

stock IsPlayerVip(playerid)
{
	if( gInfo[playerid][global_premium_time] - gettime() > 0 ) return 1;
	
	return 0;
}

stock GetPlayerNameByUid(uid, string[], length = sizeof(string))
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT `char_name` FROM `crp_characters` WHERE `char_uid` = %d", uid));
	
	if( cache_get_rows() ) cache_get(0, "char_name", string, length);
	
	cache_delete(result);

	return true;
}

stock OnPlayerVirtualWorldChange(playerid, vw)
{
	new d_id = GetDoorByUid(vw);
	
	if( d_id == -1 ) StopAudioStreamForPlayer(playerid);
	else
	{
		if( isnull(Door[d_id][door_audio]) ) StopAudioStreamForPlayer(playerid);
		else
		{
			PlayAudioStreamForPlayer(playerid, Door[d_id][door_audio], 0);
		}
	}

	if( vw > 0 && pInfo[playerid][player_last_vw] != vw && !pInfo[playerid][player_admin_spec] )
	{
		if( pInfo[playerid][player_freeze_door] ) FreezePlayer(playerid, 2500);
	}
	
	pInfo[playerid][player_last_vw] = vw;

	SetPlayerWeather(playerid, Setting[setting_server_weather]);
	
	// Check if was spectated
	foreach(new p : Spectators)
	{
		if( pInfo[p][player_admin_spec_id] == playerid && pInfo[p][player_admin_spec] )
		{
			SetPlayerVirtualWorld(p, vw);
		}
	}
}

stock scrp_SpawnPlayer(playerid, bool:withSpawn = true)
{
	new Cache:result;
	if( pInfo[playerid][player_aj] > 0 )
	{
		SetSpawnInfo(playerid, 255, pInfo[playerid][player_last_skin], 154.011, -1952.09, 47.875, 323.23, 0, 0, 0, 0, 0, 0 );
		SetPlayerVirtualWorld(playerid, playerid);
		
		SetCameraBehindPlayer(playerid);
		if( withSpawn ) SpawnPlayer(playerid);
		
		return 1;
	}
	
	if( pInfo[playerid][player_bw] > 0 ) return scrp_SpawnPlayerWithBW(playerid, withSpawn);
	
	if( pInfo[playerid][player_keep] > gettime() )
	{
		SetSpawnInfo(playerid, 1, pInfo[playerid][player_last_skin], pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2], pInfo[playerid][player_quit_pos][3], 0, 0, 0, 0, 0, 0);
		SetPlayerVirtualWorld(playerid, pInfo[playerid][player_quit_vw]);
		SetPlayerInterior(playerid, pInfo[playerid][player_quit_int]);
		
		new time = pInfo[playerid][player_keep] - gettime(), hour = floatround(time/3600, floatround_floor);
		time -= hour * 3600;
		new minute = floatround(time/60, floatround_floor);
		
		SendGuiInformation(playerid, "Informacja", sprintf("Nadal jesteœ przetrzymywany w tym budynku przez: %dh %dmin", hour, minute));
		
		SetCameraBehindPlayer(playerid);
		if( withSpawn ) SpawnPlayer(playerid);
		
		return 1;
	}
	
	if( pInfo[playerid][player_is_selecting_bus] )
	{
		new Float:pos[3];
		GetBusPos(pInfo[playerid][player_start_bus_id], pos[0], pos[1], pos[2]);
		SetSpawnInfo(playerid, 1, pInfo[playerid][player_last_skin], pos[0], pos[1], pos[2], 0.0, 0, 0, 0, 0, 0, 0);
		SetPlayerVirtualWorld(playerid, 0);
		SetPlayerInterior(playerid, 0);
		
		SetCameraBehindPlayer(playerid);
		if( withSpawn ) SpawnPlayer(playerid);
		
		return 1;
	}
	
	if( pInfo[playerid][player_admin_spec] )
	{
		SetSpawnInfo(playerid, 1, pInfo[playerid][player_last_skin], pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2], pInfo[playerid][player_quit_pos][3], 0, 0, 0, 0, 0, 0);
		SetPlayerVirtualWorld(playerid, pInfo[playerid][player_quit_vw]);
		SetPlayerInterior(playerid, pInfo[playerid][player_quit_int]);
		
		SetCameraBehindPlayer(playerid);
		if( withSpawn ) SpawnPlayer(playerid);
		
		return 1;
	}
		
	new time_away = gettime() - pInfo[playerid][player_quit_time];
	if( time_away < QS_TIME )
	{
		SetSpawnInfo(playerid, 1, pInfo[playerid][player_last_skin], pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2], pInfo[playerid][player_quit_pos][3], 0, 0, 0, 0, 0, 0);
		SetPlayerVirtualWorld(playerid, pInfo[playerid][player_quit_vw]);
		SetPlayerInterior(playerid, pInfo[playerid][player_quit_int]);

		if(pInfo[playerid][player_admin_duty_died])
		{
			SendPlayerInformation(playerid, "Twoja pozycja zostala przywrocona.", 3000);
			pInfo[playerid][player_admin_duty_died] = false;
		}
		else
		{
			mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_posx`='0.0', `char_posy`='0.0', `char_posz`='0.0', `char_posa`='0.0', `char_world`=0, `char_interior`=0, `char_quittime`=0 WHERE `char_uid`=%d", pInfo[playerid][player_id]));
			InfoboxRight(playerid, 5, "Twoja poprzednia pozycja zosta³a przywrocona poniewaz wrociles po czasie krotszym niz 10 minut");

			PlayerLog(sprintf("Spawned in saved position {%.1f, %.1f, %.1f, %.1f}", pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2], pInfo[playerid][player_quit_pos][3]), pInfo[playerid][player_id], "basic");
		}
				
		pInfo[playerid][player_quit_time] = 0;
	}
	else
	{
		new bool:is_spawned;
		if( pInfo[playerid][player_door] > 0 )
		{
			new did = GetDoorByUid(pInfo[playerid][player_door]);
			if( did > -1 )
			{
				if( Door[did][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
				{
					new g_id = GetGroupByUid(Door[did][door_owner]);
					if( Group[g_id][group_type] == GROUP_TYPE_HOTEL )
					{
						// Spawn w h³otelu ;d	
						result = mysql_query(g_sql, sprintf("SELECT COUNT(*) as havepaid FROM crp_sessions WHERE session_extraid = 4 AND session_type = 1 AND session_owner = %d AND from_unixtime(session_start, '%%Y-%%m-%%d') = CURDATE()", pInfo[playerid][player_id]));
						
						new bool:havemoney = true;
						
						if( cache_get_int(0, "havepaid") == 0 )
						{
							if( pInfo[playerid][player_bank_money] < 20 || pInfo[playerid][player_debit] > 0)
							{
								havemoney = false;
							}
							else
							{
								AddPlayerBankMoney(playerid, -20, "Op³ata za hotel");
								SendClientMessage(playerid, COLOR_RED, "Z Twojego konta zosta³o pobrane $20 pieniêdzy w celu op³acenia hotelu.");
							}
						}
						
						cache_delete(result);
						
						new time = GetPlayerOnlineTime(playerid), hour = floatround(time/3600, floatround_floor);
						if( hour > 40 ) 
						{
							SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Przekroczy³eœ próg 20h online i nie mo¿esz ju¿ d³u¿ej mieszkaæ w hotelu. Poszukaj sobie jakiegoœ przytulnego mieszkanka.");
						}
						else
						{
							if( !havemoney )
							{
								is_spawned = false;
								
								SendClientMessage(playerid, COLOR_RED, "Na Twoim koncie bankowym nie by³o odpowiedniej iloœci pieniêdzy do op³acenia hotelu.");
								
								pInfo[playerid][player_door] = 0;
								mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_door = %d WHERE char_uid = %d", pInfo[playerid][player_door], pInfo[playerid][player_id]));
							}
							else
							{
								is_spawned = true;
								
								if( Group[g_id][group_value1] == 0 )
								{
									SetPlayerInterior(playerid, 2);
									new vw[7];
									format(vw, sizeof(vw), "20%04d", pInfo[playerid][player_id]);
									SetPlayerVirtualWorld(playerid, strval(vw));				
									
									SetSpawnInfo(playerid, 255, pInfo[playerid][player_last_skin], 271.884979, 306.631988, 999.148437, 0.0, 0, 0, 0, 0, 0, 0 );
									
									defer BuggedHotel[100](playerid);
								}
								else
								{
									SetPlayerInterior(playerid, 15);
									new vw[7];
									format(vw, sizeof(vw), "20%04d", pInfo[playerid][player_id]);
									SetPlayerVirtualWorld(playerid, strval(vw));				
									
									SetSpawnInfo(playerid, 255, pInfo[playerid][player_last_skin], 235.6946, -1168.8226, 1029.8043, 273.1100, 0, 0, 0, 0, 0, 0 );
									
									defer BuggedHotel[100](playerid);
								}

								PlayerLog(sprintf("Spawned in hotel, doors: %s", DoorLogLink(Door[did][door_uid])), pInfo[playerid][player_id], "basic");
							}
						}
					}
				}
				else if( Door[did][door_owner_type] == DOOR_OWNER_TYPE_GLOBAL )
				{
					new parent_did = GetDoorByUid(Door[did][door_vw]);
					if( parent_did > -1 )
					{
						if( Door[parent_did][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
						{
							new gid = GetGroupByUid(Door[parent_did][door_owner]);
							if( gid > -1 )
							{
								if( Group[gid][group_type] == GROUP_TYPE_SOCIAL_HOUSE )
								{
									// Spawn w mieszkaniu socjalnym ;d	
									result = mysql_query(g_sql, sprintf("SELECT COUNT(*) as havepaid FROM crp_sessions WHERE session_extraid = 4 AND session_type = 1 AND session_owner = %d AND from_unixtime(session_start, '%%Y-%%m') = from_unixtime((%d-604800), '%%Y-%%m')", pInfo[playerid][player_id], gettime()));
									
									new bool:havemoney = true;
									
									if( cache_get_int(0, "havepaid") == 0 )
									{
										if( pInfo[playerid][player_bank_money] < Door[parent_did][door_value1] || pInfo[playerid][player_debit] > 0 )
										{
											havemoney = false;
										}
										else
										{
											AddPlayerBankMoney(playerid, -Door[parent_did][door_value1], "Op³ata czynszu");
											SendClientMessage(playerid, COLOR_RED, sprintf("Z Twojego konta zosta³o pobrane $%d pieniêdzy w celu op³acenia mieszkania socjalnego (op³ata tygodniowa).", Door[parent_did][door_value1]));
										}
									}
									
									cache_delete(result);
									
									if( !havemoney )
									{
										is_spawned = false;
										
										SendClientMessage(playerid, COLOR_RED, "Na Twoim koncie bankowym nie by³o odpowiedniej iloœci pieniêdzy do op³acenia mieszkania socjalnego lub masz do sp³acenie debet.");
										
										pInfo[playerid][player_door] = 0;
										mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_door = %d WHERE char_uid = %d", pInfo[playerid][player_door], pInfo[playerid][player_id]));
									}
									else
									{
										is_spawned = true;
										
										SetPlayerInterior(playerid, Door[did][door_spawn_int]);
										SetPlayerVirtualWorld(playerid, Door[did][door_spawn_vw]);				
										
										SetSpawnInfo(playerid, 255, pInfo[playerid][player_last_skin], Door[did][door_spawn_pos][0], Door[did][door_spawn_pos][1], Door[did][door_spawn_pos][2], Door[did][door_spawn_pos][3], 0, 0, 0, 0, 0, 0 );
									}

									PlayerLog(sprintf("Spawned in doors %s", DoorLogLink(Door[did][door_uid])), pInfo[playerid][player_id], "basic");

								}
							}
						}	
					}
				}
				
				if( !is_spawned )
				{
					SetPlayerInterior(playerid, Door[did][door_spawn_int]);
					SetPlayerVirtualWorld(playerid, Door[did][door_spawn_vw]);				
					
					SetSpawnInfo(playerid, 255, pInfo[playerid][player_last_skin], Door[did][door_spawn_pos][0], Door[did][door_spawn_pos][1], Door[did][door_spawn_pos][2], Door[did][door_spawn_pos][3], 0, 0, 0, 0, 0, 0 );
					is_spawned = true;
				}
			}
		}
	
		if( !is_spawned )
		{
			// Gdyby jakis imbecyl nie ustawil spawnow...
			if( !Iter_Count(GlobalSpawns) )
			{
				SetSpawnInfo(playerid, 255, pInfo[playerid][player_last_skin], 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0 );
			}
			else
			{
				SetSpawnInfo(playerid, 255, pInfo[playerid][player_last_skin], GlobalSpawn[0][gspawn_pos][0], GlobalSpawn[0][gspawn_pos][1], GlobalSpawn[0][gspawn_pos][2], GlobalSpawn[0][gspawn_pos][3], 0, 0, 0, 0, 0, 0 );
				
				SetPlayerVirtualWorld(playerid, 0);
				SetPlayerInterior(playerid, 0);
			}

			PlayerLog("Spawned on global spawn", pInfo[playerid][player_id], "basic");
		}		
	}
	
	SetCameraBehindPlayer(playerid);
	if( withSpawn ) SpawnPlayer(playerid);
	
	return 1;
}

stock scrp_SpawnPlayerWithBW(playerid, bool:withSpawn = true)
{
	AddPlayerStatus(playerid, PLAYER_STATUS_BW);
	// Spawn player
	SetSpawnInfo(playerid, 1, pInfo[playerid][player_last_skin], pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2]-0.2, pInfo[playerid][player_quit_pos][3], 0, 0, 0, 0, 0, 0);
	SetPlayerHealth(playerid, 1);
	
	SendClientMessage(playerid, COLOR_LIGHTER_RED, "Aby zaakceptowaæ smieræ i zablokowaæ swoj¹ postaæ wpisz /akceptujsmierc.");
	
	if( withSpawn ) SpawnPlayer(playerid);	
	return 1;
}

stock SavePlayer(playerid)
{
	if( !pInfo[playerid][player_logged] ) return;
	
	if( pInfo[playerid][player_bw] > 0 && pInfo[playerid][player_bw_end_time] > gettime() )
	{
		pInfo[playerid][player_bw] = pInfo[playerid][player_bw_end_time] - gettime();
	}
	
	if( pInfo[playerid][player_aj] > 0 && pInfo[playerid][player_aj_end_time] > gettime() )
	{
		pInfo[playerid][player_aj] = pInfo[playerid][player_aj_end_time] - gettime();
	}
	
	new time = GetPlayerOnlineTime(playerid), hour = floatround(time/3600, floatround_floor);
	time -= hour * 3600;
	new minute = floatround(time/60, floatround_floor);
	
	mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_hours` = %d, `char_minutes` = %d, `char_health` = %f, `char_bw` = %d, `char_aj` = %d, `char_drunk` = %d WHERE `char_uid` = %d", hour, minute, pInfo[playerid][player_health], pInfo[playerid][player_bw], pInfo[playerid][player_aj], pInfo[playerid][player_drunk_time], pInfo[playerid][player_id]));
	mysql_pquery(g_sql, sprintf("DELETE FROM `crp_logged_players` WHERE `char_uid` = %d", pInfo[playerid][player_id]));
}

stock Float:GetPlayerDistanceFromVehicle(playerid, vehicleid)
{
    new 
		Float:x,
		Float:y,
		Float:z;
		
    if( !IsPlayerConnected(playerid) || !Iter_Contains(Vehicles, vehicleid) ) return 1000.00;
	
    GetPlayerPos(playerid, x, y, z);
    return GetVehicleDistanceFromPoint(vehicleid, x, y, z);
}

stock GetPlayerOnlineTime(playerid)
{
	new time = pInfo[playerid][player_online_time]; 
	
	return time;
}

stock UpdatePlayerColor(playerid)
{	
	if( !pInfo[playerid][player_logged] )
	{
		SetPlayerColor(playerid, 0x00000000);
		return;
	}
	
	if( IsPlayerVip(playerid) ) SetPlayerColor(playerid, 0xFFFFFF80);
	else SetPlayerColor(playerid, 0xADADAD80);
}

stock SetPlayerRealTime(playerid)
{
	new hour, minute;
	gettime(hour, minute);
	
	if( Setting[setting_server_hour] == -1 )
	{
		hour += 1;
		if( hour > 24 ) hour = hour-24;
		SetPlayerTime(playerid, hour, minute);
	}
	else
	{
		if( !pInfo[playerid][player_logged] ) SetPlayerTime(playerid, Setting[setting_server_hour], 0);
	}
}

stock PreloadActorAnimLib(actorid, animlib[])
{
	ApplyDynamicActorAnimation(actorid,animlib,"null",0.0,0,0,0,0,0);
}

stock PreloadAnimLib(playerid, animlib[])
{
	ApplyAnimation(playerid,animlib,"null",0.0,0,0,0,0,0);
}

stock UpdatePlayerLabel(playerid)
{
	new nick_part[40], status_part[140], tag_part[15], f_label[220];
	
	
	if( IsPlayerNPC(playerid) ) format(nick_part, sizeof(nick_part), "%s", pInfo[playerid][player_name]);
	else 
	{
		if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASK) > -1 ) format(nick_part, sizeof(nick_part), "%s", pInfo[playerid][player_name]);	
		else format(nick_part, sizeof(nick_part), "%s (%d)", pInfo[playerid][player_name], playerid);	
	}
	
	// Format status
	sortIntegers( pInfo[playerid][player_status], 0, 5 );
	
	new comma[5], status_count, last_status;
	for(new s; s < 5; s++)
	{
		if( pInfo[playerid][player_status][s] > 0 )
		{
			status_count += 1;
			last_status = s;
		}
	}
	for(new i; i < 5; i++)
	{
		if( pInfo[playerid][player_status][i] > 0 )
		{
			format(comma, 5, "");
			if( status_count > 1 && last_status != i ) format(comma, 5, ", ");
						
			switch( pInfo[playerid][player_status][i] )
			{
				case PLAYER_STATUS_AFK:
				{
					new afkHour, afkMinute, temp;
					TimestampToDate(pInfo[playerid][player_last_activity], temp, temp, temp, afkHour, afkMinute, temp, 1);
					
					if( afkHour == 24 ) afkHour = 0;
					format(status_part, sizeof(status_part), "%sAFK od %02d:%02d%s", status_part, afkHour, afkMinute, comma);
				}
				
				case PLAYER_STATUS_BELT:
				{
					format(status_part, sizeof(status_part), "%spasy%s", status_part, comma);
				}
				
				case PLAYER_STATUS_HURT:
				{
					format(status_part, sizeof(status_part), "%sranny%s", status_part, comma);
				}
				
				case PLAYER_STATUS_BW:
				{
					format(status_part, sizeof(status_part), "%snieprzytomny%s", status_part, comma);
				}
				
				case PLAYER_STATUS_CERPEK:
				{
					format(status_part, sizeof(status_part), "%ssta³y bywalec%s", status_part, comma);
				}
				
				case PLAYER_STATUS_TIPSY:
				{
					format(status_part, sizeof(status_part), "%spodpity%s", status_part, comma);
				}
				
				case PLAYER_STATUS_DRUNK:
				{
					format(status_part, sizeof(status_part), "%spijany%s", status_part, comma);
				}
				
				case PLAYER_STATUS_GLOVES:
				{
					format(status_part, sizeof(status_part), "%rêkawiczki%s", status_part, comma);
				}
			}
		}
	}
	
	if( status_count > 0 ) format(status_part, sizeof(status_part), "(%s)", status_part);
	
	new slot = GetPlayerDutySlot(playerid);
	if( slot > -1 )
	{
		new gid = pGroup[playerid][slot][pg_id];
		if( GroupHasFlag(gid, GROUP_FLAG_DUTY_TAG) )
		{
			format(tag_part, sizeof(tag_part), "[%s]", GetGroupTag(gid));
		}
	}

	if( isnull(status_part) ) format(f_label, sizeof(f_label), "%s\n%s", nick_part, tag_part);
	else format(f_label, sizeof(f_label), "%s\n%s\n%s", nick_part, status_part, tag_part);

	new nick_color = LABEL_NICK_NORMAL_COLOR;
	if( pInfo[playerid][player_admin_duty] )
	{
		nick_color = LABEL_NICK_CREW_COLOR;
	 	if(!GetPlayerPermissions(playerid, 1)) format(f_label, sizeof(f_label), "{89afd3}Support\n%s", f_label);
	 	if(!GetPlayerPermissions(playerid, 5)) format(f_label, sizeof(f_label), "{ed6161}Administrator\n%s", f_label);			
	}


	
	if( pInfo[playerid][player_taking_damage] ) Update3DTextLabelText(pInfo[playerid][player_label], LABEL_NICK_DAMAGED_COLOR, f_label);
	else
	{
		if( slot > -1 )
		{
			new gid = pGroup[playerid][slot][pg_id];
			if( GroupHasFlag(gid, GROUP_FLAG_COLORED) )
			{
				new gcolor = GetGroupColor(gid), r, g, b, a;
				HexToRGBA(gcolor, r, g, b, a);
				
				gcolor = RGBAToHex(r, g, b, 112);
				Update3DTextLabelText(pInfo[playerid][player_label], gcolor, f_label);
			}
			else Update3DTextLabelText(pInfo[playerid][player_label], nick_color, f_label);
		}
		else Update3DTextLabelText(pInfo[playerid][player_label], nick_color, f_label);
	}
}

stock AddPlayerStatus(playerid, type)
{
	new free_slot = -1;
	for(new s; s < 5; s++)
	{
		if( pInfo[playerid][player_status][s] == 0 && free_slot == -1 ) free_slot = s;
		if( pInfo[playerid][player_status][s] == type ) return;
	}
	
	if( free_slot == -1 ) return;
	
	pInfo[playerid][player_status][free_slot] = type;
	UpdatePlayerLabel(playerid);
}

stock RemovePlayerStatus(playerid, type)
{
	new bool:changes;
	for(new s; s < 5; s++)
	{
		if( pInfo[playerid][player_status][s] == 0 ) continue;	
		if( pInfo[playerid][player_status][s] == type ) 
		{
			pInfo[playerid][player_status][s] = 0;
			changes = true;
		}
	}
	
	if( changes ) UpdatePlayerLabel(playerid);
}

stock ApplyCommandAnim(playerid, query[])
{	
	if( pInfo[playerid][player_hurted] ) return;
	
	mysql_tquery(g_sql, sprintf("SELECT * FROM crp_anim %s", query), "OnApplyCommandAnimResult", "i", playerid);
}

function OnApplyCommandAnimResult(playerid)
{
	if( cache_get_rows() == 1 )
	{
		new anim_action = cache_get_int(0, "anim_action");
		
		if( anim_action > 0 )
		{
			SetPlayerSpecialAction(playerid, anim_action);
		}
		else
		{
			new anim_lib[20], anim_name[20];
			cache_get(0, "anim_lib", anim_lib);
			cache_get(0, "anim_name", anim_name);
			
			if( cache_get_int(0, "anim_opt1") == 1 || cache_get_int(0, "anim_opt4") == 1 ) pInfo[playerid][player_looped_anim] = true;
			
			ApplyAnimation(playerid, anim_lib, anim_name, cache_get_float(0, "anim_speed"), cache_get_int(0, "anim_opt1"), cache_get_int(0, "anim_opt2"), cache_get_int(0, "anim_opt3"), cache_get_int(0, "anim_opt4"), cache_get_int(0, "anim_opt5"), 1);	
		}
	}
	else
	{
		PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	}

	return 1;
}

stock SetPlayerCellPhoneVisuals(playerid, bool:set)
{
	if( !set )
	{
		if( GetPlayerSpecialAction(playerid) != SPECIAL_ACTION_USECELLPHONE ) return 1;
		
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
		if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
	}
	else
	{
		if( GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USECELLPHONE ) return 1;
			
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
		pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
		if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);
	}

	return 1;
}

stock FreezePlayer(playerid, time = 1000)
{
	pInfo[playerid][player_freeze_id]++;

	TogglePlayerControllable(playerid, 0);
	defer UnfreezePlayer[time](playerid, pInfo[playerid][player_freeze_id]);
}


stock CleanGlobalData(playerid)
{
	for(new z=0; e_global_account_info:z != e_global_account_info; z++)
    {
		gInfo[playerid][e_global_account_info:z] = 0;
    }
	
	for(new z=0; e_session_info:z != e_session_info; z++)
    {
		sInfo[playerid][e_session_info:z] = 0;
    }
}

stock GetPlayerFreeAttachSlot(playerid)
{
    for(new i;i<MAX_PLAYER_ATTACHED_OBJECTS;i++)
    {
		if( !IsPlayerAttachedObjectSlotUsed(playerid, i) ) return i;
    }
	
    return -1;
}

stock CleanPlayerData(playerid)
{
	new Text3D:tmp_label = pInfo[playerid][player_label];
	new Text3D:tmp_label2 = pInfo[playerid][player_description_label];
	
	if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
	if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);

	for(new z=0; e_players_info:z != e_players_info; z++)
    {
		pInfo[playerid][e_players_info:z] = 0;
    }
	
	for(new x=0; e_player_offer:x != e_player_offer; x++)
	{
		pOffer[playerid][e_player_offer:x] = 0;
	}
	
	for(new y;y<13;y++)
	{
		pWeapon[playerid][y][pw_itemid] = -1;
		pWeapon[playerid][y][pw_id] = -1;
		pWeapon[playerid][y][pw_ammo] = -1;
		pWeapon[playerid][y][pw_object_index] = -1;
	}
	
	pInfo[playerid][player_label] = tmp_label;
	pInfo[playerid][player_description_label] = tmp_label2;
	Update3DTextLabelText(pInfo[playerid][player_description_label], LABEL_DESCRIPTION, "");
	
	pInfo[playerid][player_entering_vehicle] = -1;
	pInfo[playerid][player_occupied_vehicle] = -1;
	pInfo[playerid][player_vehicle_target] = -1;
	pInfo[playerid][player_last_pm_playerid] = -1;
	pInfo[playerid][player_last_group_slot_chat] = -1;
	pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
	pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;
	pInfo[playerid][player_phone_object_index] = -1;
	pInfo[playerid][player_lsn_wywiad_starter] = INVALID_PLAYER_ID;
	pInfo[playerid][player_lsn_wywiad_with] = INVALID_PLAYER_ID;
	pInfo[playerid][player_bw_vehicle] = INVALID_VEHICLE_ID;
	pInfo[playerid][player_last_vw] = -1;
	
	for(new i;i<5;i++) CleanPlayerGroup(playerid, i, false);
}

stock CleanPlayerGroup(playerid, slot, bool:do_reload = true)
{
	for(new z=0; e_player_groups:z != e_player_groups; z++)
    {
		pGroup[playerid][slot][e_player_groups:z] = 0;
    }
	
	pGroup[playerid][slot][pg_id] = -1;
	pGroup[playerid][slot][pg_rank_skin] = -1;
	
	if( do_reload )
	{
		ReloadPlayerGroupData(playerid);
	}
}

stock ReloadPlayerGroupData(playerid)
{
	for(new i;i<4;i++)
	{
		if( pGroup[playerid][i][pg_id] == -1 )
		{
			if( pGroup[playerid][i+1][pg_id] != -1 )
			{
				pGroup[playerid][i][pg_id] = pGroup[playerid][i+1][pg_id];
				
				pGroup[playerid][i][pg_rank_skin] = pGroup[playerid][i+1][pg_rank_skin];
				pGroup[playerid][i][pg_rank_payment] = pGroup[playerid][i+1][pg_rank_payment];
				pGroup[playerid][i][pg_rank_perm] = pGroup[playerid][i+1][pg_rank_perm];
				pGroup[playerid][i][pg_duty_start] = pGroup[playerid][i+1][pg_duty_start];
				pGroup[playerid][i][pg_duty] = pGroup[playerid][i+1][pg_duty];
				
				strcopy(pGroup[playerid][i][pg_rank_title], pGroup[playerid][i+1][pg_rank_title], 32);
				
				CleanPlayerGroup(playerid, i+1, false);
			}
		}
	}
}

stock HidePlayerDialog(playerid)
{
	ShowPlayerDialog(playerid,-1,0,"","","","");
}

stock _GetPlayerSkin(playerid)
{
	new cskin = GetPlayerCustomSkin(playerid);
	new skin = GetPlayerSkin(playerid);

	if(cskin <= 0) return skin;
	return cskin;
}
#undef GetPlayerSkin
#define GetPlayerSkin(%0) _GetPlayerSkin(%0)

stock UpdatePlayerBWTextdraw(playerid)
{
	if(pInfo[playerid][player_bw_end_time] == 9999)
	{
		PlayerTextDrawHide(playerid, pInfo[playerid][leftTime]);
		PlayerTextDrawSetString(playerid, pInfo[playerid][leftTime], "~r~~h~~h~stan nieprzytomnosci~n~~w~stan krytyczny");
		PlayerTextDrawShow(playerid, pInfo[playerid][leftTime]);
		return;
	}
	else
	{
		new bstring[50], Float:TimeInMin = floatdiv((pInfo[playerid][player_bw_end_time] - gettime()), 60);
		format(bstring, sizeof(bstring), "~r~~h~~h~stan nieprzytomnosci~n~~w~przez %d min", floatround(TimeInMin, floatround_ceil));	
		PlayerTextDrawHide(playerid, pInfo[playerid][leftTime]);
		PlayerTextDrawSetString(playerid, pInfo[playerid][leftTime], bstring);
		PlayerTextDrawShow(playerid, pInfo[playerid][leftTime]);
	}
}

stock UpdatePlayerAJTextdraw(playerid)
{
	new bstring[50], Float:TimeInMin = floatdiv((pInfo[playerid][player_aj_end_time] - gettime()), 60);
	format(bstring, sizeof(bstring), "~r~~h~~h~admin jail~n~~w~przez %d min", floatround(TimeInMin, floatround_ceil));	
	PlayerTextDrawHide(playerid, pInfo[playerid][leftTime]);
	PlayerTextDrawSetString(playerid, pInfo[playerid][leftTime], bstring);
	PlayerTextDrawShow(playerid, pInfo[playerid][leftTime]);
}

stock GivePlayercPoints(playerid, amount)
{
	gInfo[playerid][global_game_points] += amount;
	mysql_pquery(g_sql, sprintf("UPDATE core_members SET member_game_points = %d WHERE member_id = %d", gInfo[playerid][global_game_points], gInfo[playerid][global_id]));
	
	SetPlayerScore(playerid, gInfo[playerid][global_game_points]);
}

ptask PlayersTask[1000](playerid)
{	
	if( !pInfo[playerid][player_logged] && pInfo[playerid][player_connected] )
	{
		if(gettime() - gInfo[playerid][global_join_time] > 120) Kick(playerid);
	}

	if( pInfo[playerid][player_logged] )
	{
		if( gettime() - pInfo[playerid][player_last_activity] >= AFK_TIME && !pInfo[playerid][player_afk] )
		{
			AddPlayerStatus(playerid, PLAYER_STATUS_AFK);
			pInfo[playerid][player_afk] = true;
		}
		
		if( !pInfo[playerid][player_afk] ) pInfo[playerid][player_online_time]++;
		
		new time = GetPlayerOnlineTime(playerid), hour = floatround(time/3600, floatround_floor);
		time -= hour * 3600;
		
		if( hour > 0 && pInfo[playerid][player_last_hour_cpoints] != hour )
		{
			pInfo[playerid][player_last_hour_cpoints] = hour;
			
			if( !IsPlayerVip(playerid) ) GivePlayercPoints(playerid, 10);
			else GivePlayercPoints(playerid, 15);
		}
		
		new slot = GetPlayerDutySlot(playerid);
		if( slot > -1 )
		{
			if( !pInfo[playerid][player_afk] && IsPlayerInGroupPlace(playerid, pGroup[playerid][slot][pg_id], true) ) 
			{
				pGroup[playerid][slot][pg_duty_time]++;
				
				if( (pGroup[playerid][slot][pg_duty_time] % 60) == 0 ) GiveGroupPoints(pGroup[playerid][slot][pg_id], 3);
			}
		}
		
		if( pInfo[playerid][player_admin_duty] )
		{
			if( !pInfo[playerid][player_afk] ) pInfo[playerid][player_admin_duty_time]++;
		}

		if( pInfo[playerid][player_damage] != DAMAGE_LEVEL_NONE)
		{
			if(pInfo[playerid][player_damage] == DAMAGE_LEVEL_LOW && GetPlayerSpeed(playerid) > 10)
			{
				//ranny i biega
				Alert(playerid, ALERT_TYPE_INFO, "Nie mozesz biegac bedac ~r~rannym~w~! Wezwij ~g~karetke ~w~lub zglos sie do ~g~szpitala~w~.");
				TogglePlayerControllable(playerid, 0);

				defer PlayerDamaged[3000](playerid);
			}
			if(pInfo[playerid][player_damage] == DAMAGE_LEVEL_BAD && GetPlayerSpeed(playerid) > 0)
			{
				if(GetPlayerSpeed(playerid) > 10)
				{
					Alert(playerid, ALERT_TYPE_INFO, "Nie mozesz biegac bedac ~r~rannym~w~! Wezwij ~g~karetke ~w~lub zglos sie do ~g~szpitala~w~.");
					TogglePlayerControllable(playerid, 0);

					defer PlayerDamaged[3000](playerid);					
				}
				else if(GetPlayerSpeed(playerid) > 3)
				{
					switch(random(5))
					{
						case 0:
						{
							if(!pInfo[playerid][player_overturned])
							{
								Alert(playerid, ALERT_TYPE_INFO, "Twoja postac z powodu zlego stanu zdrowia ~r~wywrocila sie~w~! Zglos sie do ~g~szpitala~w~!");
								
								ProxMessage(playerid, "wywraca siê.", PROX_AME);

								TogglePlayerControllable(playerid, 0);

								defer OverturnPlayer[10](playerid);
								defer StopPlayerAnim[3000](playerid);
							}
						}
					}
				}
			}		
		}

		if( pInfo[playerid][player_is_cuffed] )
		{
			if( GetDistanceBetweenPlayers(playerid, pInfo[playerid][player_cuff_targetid]) > 10.0 )
			{
				new Float:pos[4];
				GetPointInAngleOfPlayer(pInfo[playerid][player_cuff_targetid], pos[0], pos[1], pos[2], 1.0, 180.0);
				GetPlayerFacingAngle(pInfo[playerid][player_cuff_targetid], pos[3]);

				SetPlayerPos(playerid, pos[0], pos[1], pos[2]);
				SetPlayerFacingAngle(playerid, pos[3]);
				SetCameraBehindPlayer(playerid);
			}
		}
			
		// Busy
		if( pInfo[playerid][player_is_waiting_for_bus] && gettime() - pInfo[playerid][player_bus_waiting_start] >= 30 )
		{
			if( IsPlayerInDynamicArea(playerid, Object[Bus[pInfo[playerid][player_start_bus_id]][bus_objectid]][object_area]) )
			{
				new Float:pos[3];
				GetBusPos(pInfo[playerid][player_start_bus_id], pos[0], pos[1], pos[2]);
				new Float:distance;
				Streamer_GetDistanceToItem(pos[0], pos[1], pos[2], STREAMER_TYPE_OBJECT, Bus[pInfo[playerid][player_selected_bus_id]][bus_objectid], distance, 2);
				
				new price = floatround(distance*Bus[pInfo[playerid][player_start_bus_id]][bus_ratio]*0.04);
				
				if( pInfo[playerid][player_money] < price ) 
				{
					pInfo[playerid][player_is_waiting_for_bus] = false;
					pInfo[playerid][player_bus_waiting_start] = 0;
					pInfo[playerid][player_selected_bus_id] = -1;
					pInfo[playerid][player_start_bus_id] = -1;
					
					SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mog³eœ wsi¹œæ do autobusu poniewa¿ nie masz wystarczaj¹cej iloœci pieniêdzy na przejazd.");
				}
				else
				{
					GivePlayerMoney(playerid, -price);
					ProxMessage(playerid, sprintf("%s wsiad³ do autobusu w kierunku %s.", pInfo[playerid][player_name], Bus[pInfo[playerid][player_selected_bus_id]][bus_name]), PROX_DO);
								
					pInfo[playerid][player_is_waiting_for_bus] = false;
					pInfo[playerid][player_bus_waiting_start] = 0;
					pInfo[playerid][player_is_riding_bus] = true;
					
					new travel_time = floatround(distance*Bus[pInfo[playerid][player_start_bus_id]][bus_ratio]*0.08);
					
					pInfo[playerid][player_bus_ride_ends] = gettime() + travel_time;
					
					SetPlayerPos(playerid, 1.808619, 32.384357, 1199.593750);
					SetPlayerInterior(playerid, 1);
					SetPlayerVirtualWorld(playerid, playerid);
					
					TogglePlayerControllable(playerid, 0);
					
					TextDrawShowForPlayer(playerid, BlackScreen);
					
					PlayerPlaySound(playerid, 1062, 0, 0, 0);
				}
			}
			else
			{
				SendGuiInformation(playerid, "Informacja", "Twój autobus odjecha³ poniewa¿ znajdowa³eœ siê zbyt daleko od przystanku.");
				
				pInfo[playerid][player_is_waiting_for_bus] = false;
				pInfo[playerid][player_bus_waiting_start] = 0;
				pInfo[playerid][player_selected_bus_id] = -1;
				pInfo[playerid][player_start_bus_id] = -1;
			}
		}
		
		if(GetPlayerVirtualWorld(playerid) > 0)
		{
			new d_uid = GetPlayerVirtualWorld(playerid);
			new d_id = GetDoorByUid(d_uid);

			if(d_id != -1)
			{
				if(!CanPlayerEditDoor(playerid, d_id))
				{
					if(!IsPlayerInAnyDynamicArea(playerid, Door[d_id][door_meters_area]))
					{
						playerTeleportedByServer[playerid] = true;
						InfoboxRight(playerid, 5, "Z powodu wyjscia poza metraz budynku przywrocono Cie do pozycji wejscia.");
						SetPlayerPos(playerid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);						
					}
				}
			}
		}

		if( pInfo[playerid][player_edit_meters] )
		{
			if(GetPlayerVirtualWorld(playerid) != pInfo[playerid][player_edit_meters_vw])
			{
				pInfo[playerid][player_edit_meters] = false;
				pInfo[playerid][player_meters_pos][0] = 0.0;
				pInfo[playerid][player_meters_pos][1] = 0.0;
				pInfo[playerid][player_meters_pos][2] = 0.0;

				Alert(playerid, ALERT_TYPE_NEGATIVE, "~r~Przerwano ~w~wyznaczania metrazu z powodu opuszczenia budynku!");
			}
			else
			{
				new
					Float:x = floatabs(pInfo[playerid][player_meters_pos][0]),
					Float:y = floatabs(pInfo[playerid][player_meters_pos][1]),
					Float:z = floatabs(pInfo[playerid][player_meters_pos][2]),
					Float:nx,
					Float:ny,
					Float:nz;

				GetPlayerPos(playerid, nx, ny, nz);

				new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));

				nx = floatabs(nx);
				ny = floatabs(ny);
				nz = floatabs(nz);

				new diffx, diffy, diffz;

				if(nx > x) diffx = floatround(nx-x);  
				else diffx = floatround(x-nx);

				if(ny > y) diffy = floatround(ny-y);
				else diffy = floatround(y-ny);

				if(nz > z) diffz = floatround(nz-z);
				else diffz = floatround(z-nz);

				if(diffx == 0) diffx = 1;
				if(diffy == 0) diffy = 1;
				if(diffz == 0) diffz = 1;

				new diffsum = (diffx*diffy*diffz);

				new meters_string[256];
				strcat(meters_string, "~y~Wyznaczanie metrazu~n~~n~");
				strcat(meters_string, sprintf("~p~Metraz: ~w~%d~y~x~w~%d~y~x~w~%d ", diffx, diffy, diffz));
				strcat(meters_string, sprintf("(%s%d~w~)~n~", (Door[d_id][door_meters] < diffsum) ? ("~r~") : ("~g~"), diffsum));
				strcat(meters_string, sprintf("Aby ~g~zakonczyc i zapisac ~w~metraz wcisnij ~g~LPM~w~!"));
				InfoboxRight(playerid, 2, meters_string);
			}
		}

		if( pInfo[playerid][player_is_riding_bus] && pInfo[playerid][player_bus_ride_ends] <= gettime() )
		{
			new Float:pos[3];
			GetBusPos(pInfo[playerid][player_selected_bus_id], pos[0], pos[1], pos[2]);
			
			SetPlayerPos(playerid, pos[0], pos[1], pos[2]);
			SetPlayerInterior(playerid, 0);
			SetPlayerVirtualWorld(playerid, 0);
			
			TogglePlayerControllable(playerid, 1);
					
			TextDrawHideForPlayer(playerid, BlackScreen);
			
			PlayerPlaySound(playerid,1063,0.0,0.0,0.0);
			
			new Float:rotz;
			GetDynamicObjectRot(Bus[pInfo[playerid][player_selected_bus_id]][bus_objectid], rotz, rotz, rotz);
			
			SetPlayerFacingAngle(playerid, rotz+90.0);
			
			ProxMessage(playerid, sprintf("%s wysiad³ z autobusu.", pInfo[playerid][player_name]), PROX_DO);
			
			pInfo[playerid][player_is_riding_bus] = false;
			pInfo[playerid][player_bus_ride_ends] = 0;
			pInfo[playerid][player_selected_bus_id] = -1;
			pInfo[playerid][player_start_bus_id] = -1;
		}
		
		if( pOffer[playerid][offer_type] > 0 )
		{
			if( pOffer[playerid][offer_sellerid] == INVALID_PLAYER_ID )
			{
				if( gettime() - pOffer[playerid][offer_start] >= 20 )
				{
					OnPlayerOfferResponse(pOffer[playerid][offer_buyerid], 0);
				}
			}
		}
		
		if( !IsPlayerVip(playerid) ) TextDrawHideForPlayer(playerid, PlayerVip);
		
		if( pInfo[playerid][player_taking_damage] && gettime() - pInfo[playerid][player_taken_damage] >= 2 )
		{	
			pInfo[playerid][player_taking_damage] = false;			
			UpdatePlayerLabel(playerid);
		}
		
		if( pInfo[playerid][player_aj] > 0 )
		{
			if( pInfo[playerid][player_aj_end_time] <= gettime() )
			{
				// koniec bw
				pInfo[playerid][player_aj] = 0;
				pInfo[playerid][player_aj_end_time] = 0;
				mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_aj` = 0 WHERE `char_uid` = %d", pInfo[playerid][player_id]));
				
				PlayerTextDrawHide(playerid, pInfo[playerid][leftTime]);
				scrp_SpawnPlayer(playerid, true);
			}
			else
			{
				// update textdrawu
				UpdatePlayerAJTextdraw(playerid);
			}
		}
		
		if( pInfo[playerid][player_bw] > 0 && pInfo[playerid][player_aj] == 0 )
		{
			if( pInfo[playerid][player_bw_end_time] <= gettime() && pInfo[playerid][player_bw_end_time] != 9999)
			{
				// koniec bw
				pInfo[playerid][player_bw] = 0;
				pInfo[playerid][player_bw_end_time] = 0;
				mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_bw` = 0 WHERE `char_uid` = %d", pInfo[playerid][player_id]));
				
				PlayerTextDrawHide(playerid, pInfo[playerid][leftTime]);
				RemovePlayerStatus(playerid, PLAYER_STATUS_BW);
				SetPlayerHealth(playerid, 20);
				TogglePlayerControllable(playerid, 1);
				SetCameraBehindPlayer(playerid);
				ClearAnimations(playerid);
				
				PlayerLog("BW ended naturally.", pInfo[playerid][player_id], "damage");

				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
			}
			else
			{
				// update textdrawu
				UpdatePlayerBWTextdraw(playerid);
			}
		}

		if( pInfo[playerid][player_drunk_time] > 0 ) pInfo[playerid][player_drunk_time]--;
		
		if( pInfo[playerid][player_drunk_time] > 0 && pInfo[playerid][player_drunk_time] < 600 )
		{
			RemovePlayerStatus(playerid, PLAYER_STATUS_DRUNK);
			AddPlayerStatus(playerid, PLAYER_STATUS_TIPSY);
			
			SetPlayerDrunkLevel(playerid, 2200);
		}
		else if( pInfo[playerid][player_drunk_time] > 0 && pInfo[playerid][player_drunk_time] >= 600 )
		{
			RemovePlayerStatus(playerid, PLAYER_STATUS_TIPSY);
			AddPlayerStatus(playerid, PLAYER_STATUS_DRUNK);
			
			SetPlayerDrunkLevel(playerid, 2600);
		}
		else
		{
			RemovePlayerStatus(playerid, PLAYER_STATUS_TIPSY);
			RemovePlayerStatus(playerid, PLAYER_STATUS_DRUNK);

			SetPlayerDrunkLevel(playerid, 0);
		}

		action_Process(playerid);

		PlayerAntyCheat(playerid);
		
		SetPlayerRealTime(playerid);
	}
}

stock setPlayerBlockTime(playerid, block_type, time)
{
	switch( block_type )
	{
		case BLOCK_CHAR:
		{
			pInfo[playerid][player_block_time][0] = time;
		}
		
		case BLOCK_OOC:
		{
			pInfo[playerid][player_block_time][1] = time;
		}
		
		case BLOCK_VEHICLES:
		{
			pInfo[playerid][player_block_time][2] = time;
		}
		
		case BLOCK_RUN:
		{
			pInfo[playerid][player_block_time][3] = time;
		}
	}
}

stock getPlayerBlockTime(playerid, block_type)
{
	switch( block_type )
	{
		case BLOCK_CHAR:
		{
			return pInfo[playerid][player_block_time][0];
		}
		
		case BLOCK_OOC:
		{
			return pInfo[playerid][player_block_time][1];
		}
		
		case BLOCK_VEHICLES:
		{
			return pInfo[playerid][player_block_time][2];
		}
		
		case BLOCK_RUN:
		{
			return pInfo[playerid][player_block_time][3];
		}
	}

	return 0;
}


stock PlayerHasBlock(playerid, block_type)
{
	if( pInfo[playerid][player_block] & block_type == block_type )
	{
		if(getPlayerBlockTime(playerid, block_type) < gettime() && getPlayerBlockTime(playerid, block_type) != 0)
		{
			pInfo[playerid][player_block] -= block_type;
			setPlayerBlockTime(playerid, block_type, 0);
			return false;
		}
		return true;
	}

	return false;
}

ptask PlayerSpeedyTask[215](playerid)
{
	if( pInfo[playerid][player_logged] )
	{
		if( pInfo[playerid][player_holding_fire] && GetPlayerWeapon(playerid) == 41 )
		{
			new Float:pos[3];
			GetPointInAngleOfPlayer(playerid, pos[0], pos[1], pos[2], 2.0, 0.0);
			
			new veh = INVALID_VEHICLE_ID;
			foreach(new vid : Vehicles)
			{
				if( Vehicle[vid][vehicle_state] != VEHICLE_STATE_PAINT ) continue;
				if( GetVehicleDistanceFromPoint(vid, pos[0], pos[1], pos[2]) < 4.0 )
				{
					veh = vid;
					break;
				}
			}
			
			if( veh != INVALID_VEHICLE_ID ) 
			{
				Vehicle[veh][vehicle_state_end] -= 8;
			}
		}

		// firedep
		ProcessFireExtinguish(playerid);
	}
}

function OnBincoGetNextLoaded(playerid)
{
	if( cache_get_rows() > 0 ) 
	{
		new Float:pos[3];
		GetDynamicObjectPos(pInfo[playerid][player_binco_access_object], pos[0], pos[1], pos[2]);
		
		new Float:p_pos[4];
		GetPlayerFacingAngle(playerid, p_pos[3]);
		
		if( p_pos[3] <= 90.0 ) p_pos[3] = 180.0 + p_pos[3];
		else if( p_pos[3] > 270.0 ) p_pos[3] = 90.0 + (p_pos[3] - 270.0);
		else if( p_pos[3] > 90.0 && p_pos[3] <= 180.0 ) p_pos[3] = 270.0 + (p_pos[3] - 90.0);
		else if( p_pos[3] > 180.0 && p_pos[3] <= 270.0 ) p_pos[3] = 0.0 + (p_pos[3] - 180.0);

		DestroyDynamicObject(pInfo[playerid][player_binco_access_object]);
		
		pInfo[playerid][player_binco_access_uid] = cache_get_int(0, "access_uid");
		pInfo[playerid][player_binco_access_object] = CreateDynamicObject(cache_get_int(0, "access_model"), pos[0], pos[1], pos[2], 0.0 + cache_get_float(0, "access_rotx"), 0.0 + cache_get_float(0, "access_roty"), p_pos[3] + cache_get_float(0, "access_rotz"), -1, -1, playerid);
		
		new accessname[50];
		cache_get(0, "access_name", accessname);
		
		PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], sprintf("~g~%s (%d)   ~p~CENA: $%d~n~~n~~w~Uzyj ~y~LMB~w~ i ~y~RMB~w~ aby zmieniac dostepne dodatki.~n~~y~~k~~VEHICLE_ENTER_EXIT~~w~ Aby kupic wybrany dodatek lub ~y~~k~~PED_JUMPING~ ~w~aby anulowac.", accessname, cache_get_int(0, "access_model"), cache_get_int(0, "access_price")));
		PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);
		
		RefreshPlayer(playerid);
	}
	
	return 1;
}

function OnCdUrlLoaded(playerid)
{
	new url[120];
	cache_get(0, "audio_url", url);

	PlayAudioStreamForPlayer(playerid, url);

	return 1;
}

function OnCdUrlLoaded_vehid(vid)
{
	new url[120];	
	cache_get(0, "audio_url", url);
	
	foreach(new p : Player)
	{
		if( GetPlayerVehicleID(p) == vid )
		{
			PlayAudioStreamForPlayer(p, url);
		}
	}

	return 1;
}

