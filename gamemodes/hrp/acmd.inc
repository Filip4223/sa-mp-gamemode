COMMAND:apomoc(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;

	pInfo[playerid][player_dialog_tmp1] = 1;
	ShowPlayerDialog(playerid, DIALOG_APOMOC, DIALOG_STYLE_TABLIST, "Pomoc admina", "01.\tKomendy\n02.\tTypy i w³aœciciele przedmiotów\n03.\tTypy grup\n04.\tInne typy", "Wybierz", "Zamknij");
	
	AdminLog("apomoc", pInfo[playerid][player_id], "cmd");

	return 1;
}

COMMAND:alevel(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new targetid, level;
	if( sscanf(params, "dd", targetid, level) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /alevel [id gracza] [poziom 0-6]");
	
	if( !IsPlayerConnected(targetid) || !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if(level > 6) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Poziom, który chcesz przypisaæ jest zbyt wysoki.");

	pInfo[targetid][player_level] = level;

	SendGuiInformation(playerid, "Ustawiono", sprintf("Ustawi³eœ graczowi %s poziom %d.", pInfo[playerid][player_name], level));

	AdminLog(sprintf("Set admin level for player %s to %d", PlayerLogLink(pInfo[targetid][player_id]), level), pInfo[playerid][player_id], "cmd");
	return 1;
}

COMMAND:aduty(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	
	if( pInfo[playerid][player_admin_duty] )
	{
		strcopy(pInfo[playerid][player_name], pInfo[playerid][player_duty_tmp], MAX_PLAYER_NAME+1);
		pInfo[playerid][player_duty_tmp][0] = EOS;
		
		
		SetPlayerHealth(playerid, 100);
		pInfo[playerid][player_admin_duty] = false;

		UpdatePlayerLabel(playerid);
		
		AdminLog(sprintf("Left admin duty (%dh %dmin)", floatround(pInfo[playerid][player_admin_duty_time]/60, floatround_floor), pInfo[playerid][player_admin_duty_time]%60), pInfo[playerid][player_id], "cmd");

		SendClientMessage(playerid, 0xD7A064FF, sprintf("Zszed³eœ ze s³u¿by administratora. Czas na s³u¿bie: %d min %d sek.", floatround(pInfo[playerid][player_admin_duty_time]/60, floatround_floor), pInfo[playerid][player_admin_duty_time]%60));
		mysql_pquery(g_sql, sprintf("INSERT INTO `crp_sessions` (session_uid, session_type, session_extraid, session_owner, session_start, session_end) VALUES (null, %d, %d, %d, %d, %d)", SESSION_TYPE_ADMIN_DUTY, 0, pInfo[playerid][player_id], pInfo[playerid][player_admin_duty_start], (pInfo[playerid][player_admin_duty_start] + pInfo[playerid][player_admin_duty_time])));
	}
	else
	{
		if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASK) > -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Aby wejœc na s³u¿bê musisz wczeœniej zdj¹æ maskê.");
		
		strcopy(pInfo[playerid][player_duty_tmp], pInfo[playerid][player_name], MAX_PLAYER_NAME+1);
		strcopy(pInfo[playerid][player_name], gInfo[playerid][global_name], MAX_PLAYER_NAME+1);
		
		pInfo[playerid][player_admin_duty] = true;
		pInfo[playerid][player_admin_duty_time] = 0;
		pInfo[playerid][player_admin_duty_start] = gettime();
		SetPlayerHealth(playerid, 9999999);

		UpdatePlayerLabel(playerid);
		
		SendClientMessage(playerid, 0xD7A064FF, "Wszed³eœ na slu¿bê administratora. Aby zejœæ, u¿yj ponownie tej komendy.");

		AdminLog("Entered admin duty", pInfo[playerid][player_id], "cmd");
	}
	
	return 1;
}

COMMAND:spec(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;

	new targetid;
	if( sscanf(params, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /spec [id gracza]");
	if( playerid == targetid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz spectowaæ samego siebie.");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	if( !pInfo[playerid][player_admin_spec] )
	{
		pInfo[playerid][player_admin_spec] = true;
		
		GetPlayerPos(playerid, pInfo[playerid][player_quit_pos][0], pInfo[playerid][player_quit_pos][1], pInfo[playerid][player_quit_pos][2]);
		GetPlayerFacingAngle(playerid, pInfo[playerid][player_quit_pos][3]);
		
		pInfo[playerid][player_quit_vw] = GetPlayerVirtualWorld(playerid);
		pInfo[playerid][player_quit_int] = GetPlayerInterior(playerid);
		
		TogglePlayerSpectating(playerid, 1);
		
		PlayerSetSpectate(playerid, targetid);
	}
	
	PlayerSpectatePlayer(playerid, targetid);
	
	Iter_Add(Spectators, playerid);
	
	AdminLog(sprintf("Started spectating player %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");

	return 1;
}

COMMAND:specoff(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;

	if( !pInfo[playerid][player_admin_spec] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie spectujesz nikogo.");
	
	scrp_SpawnPlayer(playerid, false);
	TogglePlayerSpectating(playerid, 0);
	
	Iter_Remove(Spectators, playerid);
	
	PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
	
	pInfo[playerid][player_admin_spec] = false;
	pInfo[playerid][player_admin_spec_id] = INVALID_PLAYER_ID;

	AdminLog("Turns off spectating %s", pInfo[playerid][player_id], "cmd");
	return 1;
}

COMMAND:apos(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	
	new Float:pos[3];
	if( sscanf(params, "a<f>[3]", pos) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /apos [x] [y] [z]");
	
	SetPlayerPos(playerid, pos[0], pos[1], pos[2]);

	AdminLog(sprintf("Changed self position {POS:%.1f,%.1f,%.1f}", pos[0], pos[1], pos[2]), pInfo[playerid][player_id], "cmd");
	return 1;
}

COMMAND:gp(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;

	new Float:pos[3];

	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);

	SendClientMessage(playerid, COLOR_GREY, sprintf("Twoja pozycja: X%f Y%f Z%f", pos[0], pos[1], pos[2]));
	return 1;
}

COMMAND:ptp(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 2)) return 1;
	
	new cmdParam[2];
	if( sscanf(params, "a<u>[2]", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ptp [kogo] [do kogo]");
	if( (!IsPlayerConnected(cmdParam[0]) || !pInfo[cmdParam[0]][player_logged] || IsPlayerNPC(cmdParam[0])) || (!IsPlayerConnected(cmdParam[1]) || !pInfo[cmdParam[1]][player_logged] || IsPlayerNPC(cmdParam[1])) ) return SendClientMessage(playerid, COLOR_GREY, "Jeden z graczy nie jest zalogowany.");
	
	if(GetPlayerPermissions(playerid, 2)) return SendClientMessage(playerid, COLOR_GREY, "Nie posiadasz uprawnieñ do teleportowania innych osób.");
	if( pInfo[cmdParam[0]][player_aj] > 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz teleportowaæ tego gracza poniewa¿ jest w admin jailu.");
	if( pInfo[cmdParam[0]][player_keep] > gettime() ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz teleportowaæ tego gracza poniewa¿ jest przetrzymywany w budynku.");
	new
		Float:x,
		Float:y,
		Float:z;
	GetPlayerPos(cmdParam[1], x, y, z);
	
	SetPlayerPos(cmdParam[0], x, y, z);
	SetPlayerVirtualWorld(cmdParam[0], GetPlayerVirtualWorld(cmdParam[1]));
	SetPlayerInterior(cmdParam[0], GetPlayerInterior(cmdParam[1]));
	
	FreezePlayer(cmdParam[0], 2500);
	
	if( cmdParam[0] == playerid )
	{
		SendGuiInformation(cmdParam[0], "Informacja", sprintf("Przeteleportowa³eœ siê do gracza %s (ID: %d, UID: %d).", pInfo[cmdParam[1]][player_name], cmdParam[1], pInfo[cmdParam[1]][player_id]));
		AdminLog(sprintf("Teleported to %s", PlayerLogLink(pInfo[cmdParam[1]][player_id])), pInfo[playerid][player_id], "cmd");
		AdminLog(sprintf("%s teleported here", PlayerLogLink(pInfo[playerid][player_id])), pInfo[cmdParam[1]][player_id], "cmd");
	}				
	else
	{
		AdminLog(sprintf("Teleported %s to %s", PlayerLogLink(pInfo[cmdParam[0]][player_id]), PlayerLogLink(pInfo[cmdParam[1]][player_id])), pInfo[playerid][player_id], "cmd");
		AdminLog(sprintf("%s teleported you to %s", PlayerLogLink(pInfo[playerid][player_id]), PlayerLogLink(pInfo[cmdParam[1]][player_id])), pInfo[cmdParam[0]][player_id], "cmd");
		AdminLog(sprintf("%s teleported %s to you", PlayerLogLink(pInfo[playerid][player_id]), PlayerLogLink(pInfo[cmdParam[0]][player_id])), pInfo[cmdParam[1]][player_id], "cmd");

		SendGuiInformation(cmdParam[0], "Informacja", sprintf("%s przeteleportowa³ Ciê do gracza %s (ID: %d, UID: %d).", pInfo[playerid][player_name], pInfo[cmdParam[1]][player_name], cmdParam[1], pInfo[cmdParam[1]][player_id]));
	}

	return 1;
}

COMMAND:to(playerid, params[])
{
	return cmd_goto(playerid, params);
}

COMMAND:goto(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;

	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /goto [id gracza]");
	if( !IsPlayerConnected(targetid) || IsPlayerNPC(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( pInfo[targetid][player_aj] > 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz teleportowaæ tego gracza poniewa¿ jest w admin jailu.");
	
	new Float:pos[4];
	GetPointInAngleOfPlayer(targetid, pos[0], pos[1], pos[2], 1.0, 0.0);
	GetPlayerFacingAngle(targetid, pos[3]);
	
	SetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));
	SetPlayerInterior(playerid, GetPlayerInterior(targetid));
	
	pos[3] = 360.0 - pos[3];
	SetPlayerFacingAngle(playerid, pos[3]);

	AdminLog(sprintf("Teleported to player %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");

	return 1;
}

COMMAND:tm(playerid, params[])
{
	return cmd_gethere(playerid, params);
}

COMMAND:tphere(playerid, params[])
{
	return cmd_gethere(playerid, params);
}

COMMAND:gethere(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;

	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /gethere [id gracza]");
	if( !IsPlayerConnected(targetid) || IsPlayerNPC(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( pInfo[targetid][player_aj] > 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz teleportowaæ tego gracza poniewa¿ jest w admin jailu.");
	if( pInfo[targetid][player_keep] > gettime() ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz teleportowaæ tego gracza poniewa¿ jest przetrzymywany w budynku.");
	
	new Float:pos[4];
	GetPointInAngleOfPlayer(playerid, pos[0], pos[1], pos[2], 1.0, 0.0);
	GetPlayerFacingAngle(playerid, pos[3]);
	
	SetPlayerPos(targetid, pos[0], pos[1], pos[2]);
	SetPlayerVirtualWorld(targetid, GetPlayerVirtualWorld(playerid));
	SetPlayerInterior(targetid, GetPlayerInterior(playerid));
	
	pos[3] = 360.0 - pos[3];
	SetPlayerFacingAngle(targetid, pos[3]);
	
	AdminLog(sprintf("Teleported player %s to himself", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");

	return 1;
}

COMMAND:givecash(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 6)) return 1;
	
	new targetid, amount;
	if( sscanf(params, "dd", targetid, amount) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /givecash [id gracza] [ilosc]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	GivePlayerMoney(targetid, amount);
	
	if( playerid != targetid ) SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s da³ Ci $%d pieniêdzy.", pInfo[playerid][player_name], amount);
	SendGuiInformation(playerid, "Informacja", sprintf("Da³eœ $%d pieniêdzy graczowi %s (%d).", amount, pInfo[targetid][player_name], targetid));	
	
	AdminLog(sprintf("Give cash to player %s {CASH:%d}", PlayerLogLink(pInfo[targetid][player_id]), amount), pInfo[playerid][player_id], "cmd");
	
	return 1;
}

COMMAND:achangelog(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /achangelog [dodaj] (...)");
	
	if( !strcmp(sub_cmd, "dodaj", false) )
	{
		new hours, minutes, desc[80];
		if( sscanf(param, "dds[80]", hours, minutes, desc) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /achangelog dodaj [+godzin] [+minut] [opis]");
		
		mysql_pquery(g_sql, sprintf("INSERT INTO crp_changelog (change_date, change_desc) VALUES (%d, '%s')", gettime() + hours*60*60 + minutes*60, desc));
		
		new 
			year, month, day, hour, minute, second;
		
		TimestampToDate(gettime() + hours*60*60 + minutes*60, year, month, day, hour, minute, second, 1, 0);
		
		SendGuiInformation(playerid, "Informacja", sprintf("Doda³eœ wpis do changeloga: %s (%02d.%02d.%d, %02d:%02d).", desc, day, month, year, hour, minute));
	}

	return 1;
}

COMMAND:aspawn(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn [dodaj/usun/lista/goto] (...)");
	
	if( !strcmp(sub_cmd, "dodaj", false) )
	{
		new name[150];
		if( sscanf(param, "s[75]", name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn dodaj [nazwa]");
		if( strlen(name) == 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn dodaj [nazwa]");
		if( strlen(name) < 4 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nazwa spawnu musi byæ d³u¿sza ni¿ 3 znaki.");

		new Float:pPos[4];
		
		GetPlayerPos(playerid, pPos[0], pPos[1], pPos[2]);
		GetPlayerFacingAngle(playerid, pPos[3]);

		mysql_escape_string(name, name);
		new Cache:result;
		result = mysql_query(g_sql, sprintf("INSERT INTO `crp_gspawns` (gspawn_uid, gspawn_name, gspawn_posx, gspawn_posy, gspawn_posz, gspawn_posa) VALUES (null, '%s', %f, %f, %f, %f)", name, pPos[0], pPos[1], pPos[2], pPos[3]));
		
		new id = cache_insert_id(), i = Iter_Free(GlobalSpawns);

		cache_delete(result);

		GlobalSpawn[i][gspawn_id] = id;
		strcopy(GlobalSpawn[i][gspawn_name], name, 100);
		GlobalSpawn[i][gspawn_pos][0] = pPos[0];
		GlobalSpawn[i][gspawn_pos][1] = pPos[1];
		GlobalSpawn[i][gspawn_pos][2] = pPos[2];
		GlobalSpawn[i][gspawn_pos][3] = pPos[3];
		
		Iter_Add(GlobalSpawns, i);
		
		// Textdraw na spawnie
		GlobalSpawn[i][gspawn_label] = CreateDynamic3DTextLabel("", COLOR_WHITE, GlobalSpawn[i][gspawn_pos][0], GlobalSpawn[i][gspawn_pos][1], GlobalSpawn[i][gspawn_pos][2], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, 0, 0, 0);
		Streamer_RemoveArrayData(STREAMER_TYPE_3D_TEXT_LABEL, GlobalSpawn[i][gspawn_label], E_STREAMER_PLAYER_ID, 0);
		
		foreach(new p : Player)
		{
			if( !pInfo[p][player_logged] ) continue;
			
			if(GetPlayerPermissions(playerid, 5))
			{
				Streamer_AppendArrayData(STREAMER_TYPE_3D_TEXT_LABEL, GlobalSpawn[i][gspawn_label], E_STREAMER_PLAYER_ID, playerid);
			}
		}
		
		new str[50];
		format(str, sizeof(str), "Globalny spawn\n%s(ID: %d)", GlobalSpawn[i][gspawn_name], i);
		UpdateDynamic3DTextLabelText(GlobalSpawn[i][gspawn_label], 0xFFFFFF80, str);
		
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie doda³eœ nowy globalny spawn %s (ID: %d).", GlobalSpawn[i][gspawn_name], i));

		AdminLog(sprintf("Created global spawn {NAME:%s,ID:%d}", GlobalSpawn[i][gspawn_name], GlobalSpawn[i][gspawn_id]), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new id;
		if( sscanf(param, "d", id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn usun [id spawnu]");
		if( id < 0 || id >= MAX_GLOBAL_SPAWNS ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Spawn o podanym id nie istnieje.");
		if( !Iter_Contains(GlobalSpawns, id) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Spawn o podanym id nie istnieje.");
		
		mysql_pquery(g_sql, sprintf("DELETE FROM `crp_gspawns` WHERE `gspawn_uid` = %d", GlobalSpawn[id][gspawn_id]));
		
		DestroyDynamic3DTextLabel(GlobalSpawn[id][gspawn_label]);

		AdminLog(sprintf("Removed global spawn {NAME:%s,ID:%d}", GlobalSpawn[id][gspawn_name], GlobalSpawn[id][gspawn_id]), pInfo[playerid][player_id], "cmd");
		
		for(new z=0; e_global_spawns:z != e_global_spawns; z++)
		{
			GlobalSpawn[id][e_global_spawns:z] = 0;
		}
		
		Iter_Remove(GlobalSpawns, id);
		
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie usun¹³eœ globalny spawn (ID: %d).", id));

	}
	else if( !strcmp(sub_cmd, "lista") )
	{
		if( !Iter_Count(GlobalSpawns) ) return SendGuiInformation(playerid, "Informacja", "Nie ma ¿adnych spawnów globalnych");
		new str[3000];
		format(str, sizeof(str), "ID\tUID\tNazwa\n");

		foreach(new gsid : GlobalSpawns)
		{
			format(str, sizeof(str), "%s%d\t%d\t%s\n", str, gsid, GlobalSpawn[gsid][gspawn_id], GlobalSpawn[gsid][gspawn_name]);
		}

		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST_HEADERS, "Lista spawnów globalnych", str, "Zamknij", "");
	}
	else if( !strcmp(sub_cmd, "goto") )
	{
		new gsid;
		if( sscanf(param, "d", gsid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn goto [id spawnu]");
		if( !Iter_Contains(GlobalSpawns, gsid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Spawn o podanym id nie istnieje.");

		SetPlayerPos(playerid, GlobalSpawn[gsid][gspawn_pos][0], GlobalSpawn[gsid][gspawn_pos][1], GlobalSpawn[gsid][gspawn_pos][2]);
		SetPlayerFacingAngle(playerid, GlobalSpawn[gsid][gspawn_pos][3]);

		AdminLog(sprintf("Teleported to global spawn {NAME:%s,ID:%d}", GlobalSpawn[gsid][gspawn_name], GlobalSpawn[gsid][gspawn_id]), pInfo[playerid][player_id], "cmd");
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn [dodaj/usun/lista/goto] (...)");
	
	return 1;
}

COMMAND:i(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new text[290];
	if( sscanf(params, "s[290]", text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /i [treœæ]");
	
	AdminLog(sprintf("[ooc:global] %s", text), pInfo[playerid][player_id], "chat");

	format(text, sizeof(text), "[[ %s: %s ]]", pInfo[playerid][player_name], BeautifyString(text));
	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		SendClientMessage(p, COLOR_WHITE, text);
	}

	return 1;
}

COMMAND:ac(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ac [tekst]");
	
	new text[200];
	format(text, sizeof(text), "<< [%s] [%s] %s", AdminLevels[pInfo[playerid][player_level]], pInfo[playerid][player_name], params);

	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		if( GetPlayerPermissions(p, 1) ) continue;
		
		SendClientMessage(p, 0xEFE7ADFF, text);
	}
	
	AdminLog(sprintf("[ooc:admin_chat] %s", text), pInfo[playerid][player_id], "chat");

	return 1;
}

COMMAND:hp(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 2)) return 1;
	new targetid, health;
	if( sscanf(params, "dd", targetid, health) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /hp [id gracza] [hp]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( health < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /hp [id gracza] [hp]");
	
	SetPlayerHealth(targetid, health);
	SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s ustawi³ Ci %d hp.", pInfo[playerid][player_name], health);
	if( playerid != targetid ) SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "Ustawi³eœ graczowi %s (UID: %d, ID: %d) %d ¿ycia.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, health);

	AdminLog(sprintf("Set player %s health to %d", PlayerLogLink(pInfo[targetid][player_id]), health), pInfo[playerid][player_id], "cmd");

	return 1;
}

COMMAND:ad(playerid, params[])
{
	return cmd_adrzwi(playerid, params);
}

COMMAND:adrzwi(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi [stworz/usun/lista/id/pickup/mapicon/goto/przypisz/interior/wejscie/wyjscie/exitvw/objects/przeladuj/value1] (...)");
	
	
	if( !strcmp(sub_cmd, "stworz", false) )
	{
		new name[50];
		if( sscanf(param, "s[50]", name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi stworz [nazwa]");
	
		if( GetPlayerArea(playerid, AREA_TYPE_DOOR_INNER) != -1 || GetPlayerArea(playerid, AREA_TYPE_DOOR_OUTER) != -1 ) return SendClientMessage(playerid, COLOR_GREY, "W tym miejscu nie mo¿esz stworzyæ drzwi.");
		
		new Float:pPos[4];
		
		GetPlayerPos(playerid, pPos[0], pPos[1], pPos[2]);
		GetPlayerFacingAngle(playerid, pPos[3]);
		
		new str[400], Cache:result;
		strcat(str, "INSERT INTO crp_doors (door_uid, door_owner, door_ownertype, door_name, door_type, door_pickupid, door_enterx, door_entery, door_enterz, door_entera, door_entervw, door_enterint, door_exitx, door_exity, door_exitz, door_exita) ");
		strcat(str, sprintf("VALUES (null, 0, 0, '%s', 1, 1239, %f, %f, %f, %f, %d, %d, %f, %f, %f, %f)", name, pPos[0], pPos[1], pPos[2], pPos[3], GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), pPos[0], pPos[1], pPos[2], pPos[3]));
		
		result = mysql_query(g_sql, str);
		new uid = cache_insert_id();

		cache_delete(result);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);

		AdminLog(sprintf("Created door {NAME:%s,ID:%d}", Door[did][door_name], uid), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Drzwi zosta³y pomyœlnie utworzone! [UID: %d, ID: %d]", uid, did));
	}
	else if( !strcmp(sub_cmd, "usun", false) )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi usun [id drzwi]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi usun [id drzwi]");
		
		new p_count;
		
		if( Door[d_id][door_spawn_vw] > 0 )
		{
			foreach(new p : Player)
			{
				if( GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] )
				{
					SetPlayerVirtualWorld(p, Door[d_id][door_vw]);
					SetPlayerInterior(p, Door[d_id][door_int]);

					SetPlayerPos(p, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
					SetPlayerFacingAngle(p, Door[d_id][door_pos][3]);
					
					SendClientMessage(p, COLOR_LIGHTER_RED, "Drzwi, w których siê znajdowa³eœ zosta³y usuniête przez administratora. Zosta³eœ przeniesiony do ich wejœcia.");
					
					p_count++;
				}
			}
		}
		
		AdminLog(sprintf("Removed door {NAME:%s,ID:%d}", Door[d_id][door_name], Door[d_id][door_uid]), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Drzwi o UID %d zosta³y usuniête, a %d graczy znajduj¹cych siê w œrodku zosta³o przeniesionych do wejœcia.", Door[d_id][door_uid], p_count));
		
		DeleteDoor(d_id);
	}
	else if( !strcmp(sub_cmd, "pickup") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi pickup [id drzwi]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi pickup [id drzwi]");
		
		new str[250];
		
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, d_id);
		
		format(str, sizeof(str), "%s01\tSerce (1240)\n", str);
		DynamicGui_AddRow(playerid, 0, 1240);
		
		format(str, sizeof(str), "%s02\tKamizelka (1242)\n", str);
		DynamicGui_AddRow(playerid, 0, 1242);
		
		format(str, sizeof(str), "%s03\tLiterka 'i' (1239)\n", str);
		DynamicGui_AddRow(playerid, 0, 1239);
		
		format(str, sizeof(str), "%s04\tNiebieski domek (1272)\n", str);
		DynamicGui_AddRow(playerid, 0, 1272);
		
		format(str, sizeof(str), "%s05\tZielony domek (1273)\n", str);
		DynamicGui_AddRow(playerid, 0, 1273);
		
		format(str, sizeof(str), "%s06\tPlik gotówki (1212)\n", str);
		DynamicGui_AddRow(playerid, 0, 1212);
		
		format(str, sizeof(str), "%s07\tPigu³ka (1241)\n", str);
		DynamicGui_AddRow(playerid, 0, 1241);
		
		format(str, sizeof(str), "%s08\tOdznaka (1247)\n", str);
		DynamicGui_AddRow(playerid, 0, 1247);
		
		format(str, sizeof(str), "%s09\tDolar (1274)\n", str);
		DynamicGui_AddRow(playerid, 0, 1274);
		
		format(str, sizeof(str), "%s10\tNiebieska koszulka (1275)\n", str);
		DynamicGui_AddRow(playerid, 0, 1275);
		
		ShowPlayerDialog(playerid, DIALOG_ADRZWI_PICKUP, DIALOG_STYLE_LIST, sprintf("Drzwi %d (%s; SampID: %d)", Door[d_id][door_uid], Door[d_id][door_name], d_id), str, "Wybierz", "Zamknij");
	}
	else if( !strcmp(sub_cmd, "mapicon") )
	{
		new d_id, model;
		if( sscanf(param, "dd", d_id, model) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi mapicon [id drzwi] [model map ikony(-1 aby usunac)]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi pickup [model map ikony(-1 aby usunac)]");
		
		mysql_query(g_sql, sprintf("UPDATE `crp_doors` SET `door_mapmodel` = %d WHERE `door_uid` = %d", model, Door[d_id][door_uid]), false);
			
		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);

		AdminLog(sprintf("Set door map icon {NAME:%s,ID:%d,ICON:%d}", Door[d_id][door_name], Door[d_id][door_uid], model), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Ikona mapy drzwi zosta³a pomyœlnie zmieniona! [ICON: %d, UID: %d, ID: %d]", model, uid, did));
	}
	else if( !strcmp(sub_cmd, "metraz") )
	{
		new d_id, meters;
		if( sscanf(param, "dd", d_id, meters) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi [id] [metry]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi metraz [id] [metry]");
		
		mysql_query(g_sql, sprintf("UPDATE `crp_doors` SET `door_meters` = %d WHERE `door_uid` = %d", meters, Door[d_id][door_uid]), false);
			
		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);

		AdminLog(sprintf("Set door %s meters {METERS:%d}", DoorLogLink(Door[d_id][door_uid]), meters), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Metry drzwi zosta³y pomyœlnie zmienione! [ICON: %d, UID: %d, ID: %d]", meters, uid, did));		
	}
	else if( !strcmp(sub_cmd, "goto") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi goto [id drzwi]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi goto [id drzwi]");
		
		FreezePlayer(playerid, 2500);
				
		SetPlayerPos(playerid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
		SetPlayerFacingAngle(playerid, Door[d_id][door_pos][3]);
		
		SetCameraBehindPlayer(playerid);
		
		SetPlayerVirtualWorld(playerid, Door[d_id][door_vw]);
		SetPlayerInterior(playerid, Door[d_id][door_int]);
		
		AdminLog(sprintf("Teleported to door {NAME:%s,ID:%d}", Door[d_id][door_name], Door[d_id][door_uid]), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Przenios³eœ siê do wejœcia drzwi [UID: %d, ID: %d].", Door[d_id][door_uid], d_id));
	}
	else if( !strcmp(sub_cmd, "przypisz") )
	{
		new d_id, action[32], who;
		if( sscanf(param, "ds[32]d", d_id, action, who) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi przypisz [id drzwi] [grupa/postac/globalne] [uid grupy/id gracza(nie uid)]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi przypisz [id drzwi] [grupa/postac] [uid grupy/id gracza(nie uid)]");
		
		if( !strcmp(action, "grupa") )
		{
			new gid = GetGroupByUid(who);
			if( gid == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Grupa o podanym uid nie istnieje.");
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_ownertype` = %d, `door_owner` = %d WHERE `door_uid` = %d", DOOR_OWNER_TYPE_GROUP, who, Door[d_id][door_uid]));
			
			Door[d_id][door_owner_type] = DOOR_OWNER_TYPE_GROUP;
			Door[d_id][door_owner] = who;
			
			AdminLog(sprintf("Assigned door {NAME:%s,ID:%d} to group %s", Door[d_id][door_name], Door[d_id][door_uid], GroupLogLink(who)), pInfo[playerid][player_id], "cmd");

			SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ w³aœciciela drzwi [UID: %d, ID: %d] na grupê %s [UID: %d, ID: %d].", Door[d_id][door_uid], d_id, Group[gid][group_name], Group[gid][group_uid], gid));
		}
		else if( !strcmp(action, "postac") )
		{
			if( !IsPlayerConnected(who) ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie jest pod³¹czony.");
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_ownertype` = %d, `door_owner` = %d WHERE `door_uid` = %d", DOOR_OWNER_TYPE_PLAYER, pInfo[who][player_id], Door[d_id][door_uid]));
			
			Door[d_id][door_owner_type] = DOOR_OWNER_TYPE_PLAYER;
			Door[d_id][door_owner] = pInfo[who][player_id];
			
			AdminLog(sprintf("Assigned door {NAME:%s,ID:%d} to player %s", Door[d_id][door_name], Door[d_id][door_uid], PlayerLogLink(pInfo[who][player_id])), pInfo[playerid][player_id], "cmd");

			SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ w³aœciciela drzwi [UID: %d, ID: %d] na gracza %s [UID: %d, ID: %d].", Door[d_id][door_uid], d_id, pInfo[who][player_name], pInfo[who][player_id], who));
		}
		else if( !strcmp(action, "globalne") )
		{
			mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_ownertype` = %d, `door_owner` = 0 WHERE `door_uid` = %d", DOOR_OWNER_TYPE_GLOBAL, Door[d_id][door_uid]));
			
			Door[d_id][door_owner_type] = DOOR_OWNER_TYPE_GLOBAL;
			Door[d_id][door_owner] = 0;
			
			AdminLog(sprintf("Assigned door {NAME:%s,ID:%d} to global", Door[d_id][door_name], Door[d_id][door_uid]), pInfo[playerid][player_id], "cmd");

			SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ w³aœciciela drzwi [UID: %d, ID: %d] na globalnego.", Door[d_id][door_uid], d_id));
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi przypisz [id drzwi] [grupa/postac] [uid grupy/id gracza(nie uid)]");	
	}
	else if( !strcmp(sub_cmd, "interior") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi interior [id drzwi]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi interior [id drzwi]");
		
		DoorsDefaultInteriorsList(playerid, d_id);
	}
	else if( !strcmp(sub_cmd, "wejscie") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi wejscie [id drzwi]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi wejscie [id drzwi]");
		
		GetPlayerPos(playerid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
		GetPlayerFacingAngle(playerid, Door[d_id][door_pos][3]);
		
		mysql_query(g_sql, sprintf("UPDATE `crp_doors` SET `door_enterx` = %f, `door_entery` = %f, `door_enterz` = %f, `door_entera` = %f, `door_entervw` = %d WHERE `door_uid` = %d", Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2], Door[d_id][door_pos][3], GetPlayerVirtualWorld(playerid), Door[d_id][door_uid]), false);		
		
		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);
		
		AdminLog(sprintf("Set outer door position {NAME:%s,ID:%d} to {POS:%.1f,%.1f,%.1f}", Door[d_id][door_name], Door[d_id][door_uid], Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Pozycja zewnetrznych drzwi zosta³a pomyœlnie zmieniona [UID: %d, ID: %d].", Door[d_id][door_uid], did));
	}
	else if( !strcmp(sub_cmd, "objects") )
	{
		new d_id, limit;
		if( sscanf(param, "dd", d_id, limit) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi objects [id drzwi] [limit obiektów]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi objects [id drzwi] [limit obiektów]");
		if( limit < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi objects [id drzwi] [limit obiektów]");
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_objects` = %d WHERE `door_uid` = %d", limit, Door[d_id][door_uid]));
			
		Door[d_id][door_objects_limit] = limit;
		
		AdminLog(sprintf("Set door object limit {NAME:%s,ID:%d} to {LIMIT:%d}", Door[d_id][door_name], Door[d_id][door_uid], limit), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ limit obiektów drzwi [LIMIT: %d, UID: %d, ID: %d].", limit, Door[d_id][door_uid], d_id));	
	}
	else if( !strcmp(sub_cmd, "przeladuj") )
	{
		Code_ExTimer_Begin(DoorsReload);
		for(new d_id;d_id<MAX_DOORS;d_id++)
		{
			if( Iter_Contains(Doors, d_id) ) DeleteDoor(d_id, false);
		}
		
		new count = LoadDoor();

		AdminLog("Reloaded doors", pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie prze³adowa³eœ wszystkie drzwi [ILOŒÆ: %d, CZAS: %d ms].", count, Code_ExTimer_End(DoorsReload)));	
	}
	else if( !strcmp(sub_cmd, "exitvw") )
	{
		new d_id, vw;
		if( sscanf(param, "dd", d_id, vw) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi exitvw [id drzwi] [vw]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi exitvw [id drzwi] [vw]");
		if( vw < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi exitvw [id drzwi] [vw]");
		
		mysql_query(g_sql, sprintf("UPDATE `crp_doors` SET `door_exitvw` = %d WHERE `door_uid` = %d", vw, Door[d_id][door_uid]), false);
		
		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);
		
		AdminLog(sprintf("Set door inner vw {NAME:%s,ID:%d} to {VW:%d}", Door[d_id][door_name], Door[d_id][door_uid], vw), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ wewnêtrzny virtual world drzwi [EXITVW: %d, UID: %d, ID: %d]", vw, Door[did][door_uid], did));
	}
	else if( !strcmp(sub_cmd, "wyjscie") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi wejscie [id drzwi]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi wejscie [id drzwi]");
		
		GetPlayerPos(playerid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
		GetPlayerFacingAngle(playerid, Door[d_id][door_spawn_pos][3]);
		
		mysql_query(g_sql, sprintf("UPDATE `crp_doors` SET `door_exitx` = %f, `door_exity` = %f, `door_exitz` = %f, `door_exita` = %f WHERE `door_uid` = %d", Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3], Door[d_id][door_uid]), false);		
		
		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);
		
		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);
		
		AdminLog(sprintf("Set inner door position {NAME:%s,ID:%d} to {POS:%.1f,%.1f,%.1f}", Door[d_id][door_name], Door[d_id][door_uid], Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Pozycja wewnetrznych drzwi zosta³a pomyœlnie zmieniona [UID: %d, ID: %d].", Door[d_id][door_uid], did));
	}
	
	else if( !strcmp(sub_cmd, "value1") )
	{
		new d_id, value1;
		if( sscanf(param, "dd", d_id, value1) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi value1 [id drzwi] [wartoœæ]");	
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi value1 [id drzwi] [wartoœæ]");
		
		Door[d_id][door_value1] = value1;
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_value1` = %d WHERE `door_uid` = %d", Door[d_id][door_value1], Door[d_id][door_uid]));		
		
		AdminLog(sprintf("Set door value1 {NAME:%s,ID:%d} to {V1:%d}", Door[d_id][door_name], Door[d_id][door_uid], Door[d_id][door_value1]), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Wartoœæ 1 drzwi zosta³a pomyœlnie zmieniona na %d [UID: %d, ID: %d].", value1, Door[d_id][door_uid], d_id));
	}
	else if( !strcmp(sub_cmd, "lista") )
	{
		new page, keyword[50];
		sscanf(param, "D(1)S()[50]", page, keyword);
		if(page == 1 && isnull(keyword)) SendPlayerInformation(playerid, "Mozesz rowniez:~n~/ad lista (strona) (fraza)", 4000);

		if( strlen(keyword) > 0 ) {
			format(keyword, sizeof(keyword), "WHERE d.door_name LIKE '%%%s%%' ", keyword);
		}

		mysql_tquery(g_sql, sprintf("SELECT d.door_uid, d.door_name, d.door_ownertype, d.door_owner, g.group_name, p.char_name FROM crp_doors d LEFT JOIN crp_groups g ON g.group_uid = d.door_owner AND d.door_ownertype = 3 LEFT JOIN crp_characters p ON p.char_uid = d.door_owner AND d.door_ownertype = 2 %sLIMIT %d, 20", keyword, (page-1)*20), "OnAdoorsListLoaded", "i", playerid);
	}
	else if( !strcmp(sub_cmd, "id") )
	{
		new duid;
		if( sscanf(param, "d", duid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi id [uid drzwi]");	
		
		new d_id = GetDoorByUid(duid);


		if(d_id == -1) return SendPlayerInformation(playerid, sprintf("Drzwi o uid %d nie istnieja.", duid));
		else return SendPlayerInformation(playerid, sprintf("Drzwi o uid %d maja id %d.", duid, d_id));
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi [stworz/usun/lista/id/pickup/mapicon/goto/przypisz/interior/wejscie/wyjscie/exitvw/objects/przeladuj/value1] (...)");
	return 1;
}

function OnAdoorsListLoaded(playerid)
{
	if(!cache_get_rows()) return SendPlayerInformation(playerid, "Brak wynikow spelniajacych kryteria.", 4000);

	new list_str[3000], dname[50], oname[50], ouid[20], otype[30];

	format(list_str, sizeof(list_str), "UID drzwi\tNazwa drzwi\tW³aœciciel\t  ");

	for(new i;i<cache_get_rows();i++)
	{
		cache_get(i, "door_name", dname);

		strcopy(oname, "------");
		strcopy(otype, "brak  ");

		if( cache_get_int(i, "door_owner") > 0 ) cache_get(i, "door_owner", ouid);
		else strcopy(ouid, "--");

		switch(cache_get_int(i, "door_ownertype"))
		{
			case DOOR_OWNER_TYPE_GLOBAL:
			{
				strcopy(otype, "global");
			}

			case DOOR_OWNER_TYPE_GROUP:
			{
				strcopy(otype, "grupa ");

				if( !cache_is_null(i, "group_name") ) cache_get(i, "group_name", oname);
			}

			case DOOR_OWNER_TYPE_PLAYER:
			{
				strcopy(otype, "gracz ");

				if( !cache_is_null(i, "char_name") ) cache_get(i, "char_name", oname);
			}
		}

		format(list_str, sizeof(list_str), "%s\n%d\t%s\t[%s, UID: %s]\t%s", list_str, cache_get_int(i, "door_uid"), dname, otype, ouid, oname);
	}

	ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST_HEADERS, "Lista drzwi", list_str, "Zamknij", "");

	return 1;
}

COMMAND:abus(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /abus [stworz/usun/lista/goto/ratio/nazwa] (...)");
	
	if( !strcmp(sub_cmd, "stworz") )
	{
		if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
		
		new name[60], Float:ratio;
		if( sscanf(param, "f s[60]", ratio, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /abus stworz [ratio] [nazwa]");
		
		new Cache:result;
		result = mysql_query(g_sql, sprintf("INSERT INTO crp_buses VALUES (null, '%s', %f)", name, ratio));	
		
		new bid = Iter_Free(Buses);
		Iter_Add(Buses, bid);
		
		Bus[bid][bus_uid] = cache_insert_id();
		strcopy(Bus[bid][bus_name], name, 60);
		Bus[bid][bus_ratio] = ratio;

		cache_delete(result);
		
		new o_id = pInfo[playerid][player_edited_object];
		mysql_query(g_sql, sprintf("UPDATE `crp_objects` SET `object_type` = %d, `object_extraid` = %d, `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", OBJECT_TYPE_BUS, Bus[bid][bus_uid], Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5], Object[pInfo[playerid][player_edited_object]][object_uid]), false);
		
		pInfo[playerid][player_edited_object_no_action] = true;
		CancelEdit(playerid);
		
		new uid = Object[o_id][object_uid];
		
		pInfo[playerid][player_edited_object] = -1;
		Object[o_id][object_is_edited] = false;
		
		PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
		
		DeleteObject(o_id, false);
		new oid = LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
		
		RefreshPlayer(playerid);
		
		Bus[bid][bus_objectid] = oid;
		
		AdminLog(sprintf("Created bus stop {NAME:%s,ID:%d,RATIO:%.1f}", Bus[bid][bus_name], Bus[bid][bus_uid], Bus[bid][bus_ratio]), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie utworzy³eœ przystanek autobusowy %s [UID: %d].", name, Bus[bid][bus_uid]));
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new buid;
		if( sscanf(param, "d", buid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /abus usun [uid przystanku]");
		
		new bid = GetBusByUid(buid);
		if( bid == -1 ) return SendGuiInformation(playerid, "Wystapi³ b³¹d", "Przystanek o podanym uid nie istnieje.");
		
		new o_id = Bus[bid][bus_objectid];
		mysql_query(g_sql, sprintf("UPDATE `crp_objects` SET `object_type` = %d, `object_extraid` = 0, `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", OBJECT_TYPE_NORMAL, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5], Object[pInfo[playerid][player_edited_object]][object_uid]), false);
		
		new uid = Object[o_id][object_uid];
		
		DeleteObject(o_id, false);
		LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
		
		AdminLog(sprintf("Removed bus stop {NAME:%s,ID:%d}", Bus[bid][bus_name], Bus[bid][bus_uid]), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie usun¹³eœ przystanek autobusowy %s [UID: %d].", Bus[bid][bus_name], Bus[bid][bus_uid]));
		
		mysql_pquery(g_sql, sprintf("DELETE FROM crp_buses WHERE bus_uid = %d", Bus[bid][bus_uid]));
		
		Bus[bid][bus_name][0] = EOS;
		Bus[bid][bus_uid] = 0;
		Bus[bid][bus_objectid] = INVALID_OBJECT_ID;
		Bus[bid][bus_ratio] = 0;

		Iter_Remove(Buses, bid);
	}
	else if( !strcmp(sub_cmd, "lista") )
	{
		if( !Iter_Count(Buses) ) return SendGuiInformation(playerid, "Informacja", "Nie ma ¿adnych przystanków autobusowych");
		
		new str[3000];
		format(str, sizeof(str), "ID\tUID\tNazwa\tRatio\n");

		foreach(new bid : Buses)
		{
			format(str, sizeof(str), "%s%d\t%d\t%s\t%.2f\n", str, bid, Bus[bid][bus_uid], Bus[bid][bus_name], Bus[bid][bus_ratio]);
		}

		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST_HEADERS, "Lista przystanków autobusowych", str, "Zamknij", "");
	}
	else if( !strcmp(sub_cmd, "goto") )
	{
		new bid;
		if( sscanf(param, "d", bid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /abus goto [id przystanku]");

		if( !Iter_Contains(Buses, bid) ) return SendGuiInformation(playerid, "Wystapi³ b³¹d", "Przystanek o podanym id nie istnieje.");
		
		new Float:x, Float:y, Float:z;

		Streamer_GetFloatData(STREAMER_TYPE_OBJECT, Bus[bid][bus_objectid], E_STREAMER_X, x);
		Streamer_GetFloatData(STREAMER_TYPE_OBJECT, Bus[bid][bus_objectid], E_STREAMER_Y, y);
		Streamer_GetFloatData(STREAMER_TYPE_OBJECT, Bus[bid][bus_objectid], E_STREAMER_Z, z);

		AdminLog(sprintf("Teleported to bus stop {NAME:%s,ID:%d}", Bus[bid][bus_name], Bus[bid][bus_uid]), pInfo[playerid][player_id], "cmd");
		SetPlayerPos(playerid, x, y, z);
	}
	else if( !strcmp(sub_cmd, "ratio") )
	{
		new bid, Float:ratio;
		if( sscanf(param, "df", bid, ratio) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /abus ratio [id przystanku] [ratio]");

		if( !Iter_Contains(Buses, bid) ) return SendGuiInformation(playerid, "Wystapi³ b³¹d", "Przystanek o podanym id nie istnieje.");
		if( ratio < 0 ) return SendGuiInformation(playerid, "Wystapi³ b³¹d", "Ratio musi byæ wiêksze od 0.");

		Bus[bid][bus_ratio] = ratio;
		mysql_pquery(g_sql, sprintf("UPDATE crp_buses SET bus_ratio = %f WHERE bus_uid = %d", ratio, Bus[bid][bus_uid]));

		AdminLog(sprintf("Set bus stop ratio {NAME:%s,ID:%d} to {RATIO:%.1f}", Bus[bid][bus_name], Bus[bid][bus_uid], ratio), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ ratio przystanku %s (UID: %D;ID: %d) na %.2f", Bus[bid][bus_name], Bus[bid][bus_uid], bid, ratio));
	}
	else if( !strcmp(sub_cmd, "nazwa") )
	{
		new bid, nazwa[60];
		if( sscanf(param, "ds[60]", bid, nazwa) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /abus nazwa [id przystanku] [nazwa]");

		if( !Iter_Contains(Buses, bid) ) return SendGuiInformation(playerid, "Wystapi³ b³¹d", "Przystanek o podanym id nie istnieje.");
		if( strlen(nazwa) < 4 ) return SendGuiInformation(playerid, "Wystapi³ b³¹d", "Nazwa musi zawieraæ minimum 4 znaki.");

		AdminLog(sprintf("Set bus stop name {NAME:%s,ID:%d} to ", Bus[bid][bus_name], Bus[bid][bus_uid], nazwa), pInfo[playerid][player_id], "cmd");

		mysql_pquery(g_sql, sprintf("UPDATE crp_buses SET bus_name = '%s' WHERE bus_uid = %d", nazwa, Bus[bid][bus_uid]));
		
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ nazwê przystanku %s (UID: %D;ID: %d) na %s", Bus[bid][bus_name], Bus[bid][bus_uid], bid, nazwa));
		format(Bus[bid][bus_name], 60, "%s", nazwa);

		UpdateDynamic3DTextLabelText(Object[Bus[bid][bus_objectid]][object_label], COLOR_WHITE, sprintf("{FFD000}#%d %s\n"HEX_COLOR_WHITE"(( Wpisz /bus aby wybraæ miejsce podró¿y. ))", Bus[bid][bus_uid], Bus[bid][bus_name]));
	}
	return 1;
}

COMMAND:atime(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new hour;
	if( sscanf(params, "d", hour) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /atime [0-24 lub -1]");
	if( hour < -1 || hour > 24 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /atime [0-24 lub -1]");
	
	Setting[setting_server_hour] = hour;
	SetWorldTime(hour);
	
	foreach(new p : Player)
	{
		SetPlayerTime(p, hour, 0);
	}
	
	AdminLog(sprintf("Set global time to {TIME:%d}", hour), pInfo[playerid][player_id], "cmd");
	SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ godzine serwera na %d.", hour));
	
	return 1;
}

COMMAND:aweather(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new weather;
	if( sscanf(params, "d", weather) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aweather [pogoda]");
	
	AdminLog(sprintf("Set global weather to {WEATHER:%d}", weather), pInfo[playerid][player_id], "cmd");
	SetWeather(weather);
	
	return 1;
}

COMMAND:as(playerid, params[])
{
	return cmd_astrefa(playerid, params);
}

COMMAND:astrefa(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa [stworz/usun/przypisz/podglad/objects/typ] (...)");
	
	if( !strcmp(sub_cmd, "stworz") )
	{
		if( pInfo[playerid][player_lookup_area] ) return SendClientMessage(playerid, COLOR_GREY, "Masz w³¹czony podgl¹d jakiejœ strefy, musisz go najpierw wy³¹czyæ.");
		if( pInfo[playerid][player_creating_area] )
		{
			if( pInfo[playerid][player_carea_type] == AREA_SHAPE_SQUARE )
			{
				if( pInfo[playerid][player_carea_point2][0] == 0.0 && pInfo[playerid][player_carea_point2][1] == 0.0 && pInfo[playerid][player_carea_point2][2] == 0.0 ) return ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Aby stworzyæ prostok¹tn¹ strefê musisz wyznaczyæ jej dwa punkty tworz¹ce przek¹tn¹.", "OK", "");	
			
				new str[400], Cache:result;
				strcat(str, "INSERT INTO `crp_areas` (area_uid, area_shape, area_type, area_ownertype, area_owner, area_objects, area_vw, area_size, area_point1, area_point2) ");
				strcat(str, sprintf("VALUES(null, %d, %d, %d, 0, 0, %d, 0.0, '%f|%f|%f', '%f|%f|%f')", AREA_SHAPE_SQUARE, AREA_TYPE_NORMAL, AREA_OWNER_TYPE_GLOBAL, GetPlayerVirtualWorld(playerid), pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2], pInfo[playerid][player_carea_point2][0], pInfo[playerid][player_carea_point2][1], pInfo[playerid][player_carea_point2][2]));
				result = mysql_query(g_sql, str);
				
				new uid = cache_insert_id();
				cache_delete(result);
				new a_id = LoadArea(sprintf("WHERE `area_uid` = %d", uid), true);
				
				pInfo[playerid][player_carea_point1][0] = 0.0;
				pInfo[playerid][player_carea_point1][1] = 0.0;
				pInfo[playerid][player_carea_point1][2] = 0.0;
				
				pInfo[playerid][player_carea_point2][0] = 0.0;
				pInfo[playerid][player_carea_point2][1] = 0.0;
				pInfo[playerid][player_carea_point2][2] = 0.0;
				
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
				
				GangZoneDestroy(pInfo[playerid][player_carea_zone]);
				
				pInfo[playerid][player_creating_area] = false;
				SendPlayerInformation(playerid, "", 0);
				
				AdminLog(sprintf("Created area {SHAPE:rectangular,ID:%d}", uid), pInfo[playerid][player_id], "cmd");

				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie stworzy³eœ prostok¹tn¹ strefê [UID: %d, ID: %d].", uid, a_id), "OK", "");
			}
			else if( pInfo[playerid][player_carea_type] == AREA_SHAPE_CIRCLE )
			{
				if( pInfo[playerid][player_carea_point1][0] == 0.0 && pInfo[playerid][player_carea_point1][1] == 0.0 && pInfo[playerid][player_carea_point1][2] == 0.0 ) return ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Aby stworzyæ sferyczn¹ strefê musisz wyznaczyæ punkt œrodka sfery.", "OK", "");	
			
				new str[400], Cache:result;
				strcat(str, "INSERT INTO `crp_areas` (area_uid, area_shape, area_type, area_ownertype, area_owner, area_objects, area_vw, area_size, area_point1, area_point2) ");
				strcat(str, sprintf("VALUES(null, %d, %d, %d, 0, 0, %d, %f, '%f|%f|%f', '')", AREA_SHAPE_CIRCLE, AREA_TYPE_NORMAL, AREA_OWNER_TYPE_GLOBAL, GetPlayerVirtualWorld(playerid), pInfo[playerid][player_carea_size], pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2]));
				result = mysql_query(g_sql, str);
			
				new uid = cache_insert_id();
				cache_delete(result);
				new a_id = LoadArea(sprintf("WHERE `area_uid` = %d", uid), true);
				
				pInfo[playerid][player_carea_point1][0] = 0.0;
				pInfo[playerid][player_carea_point1][1] = 0.0;
				pInfo[playerid][player_carea_point1][2] = 0.0;
				
				pInfo[playerid][player_carea_point2][0] = 0.0;
				pInfo[playerid][player_carea_point2][1] = 0.0;
				pInfo[playerid][player_carea_point2][2] = 0.0;
				
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
				
				GangZoneDestroy(pInfo[playerid][player_carea_zone]);
				
				pInfo[playerid][player_creating_area] = false;
				SendPlayerInformation(playerid, "", 0);
				
				AdminLog(sprintf("Created area {SHAPE:spherical,ID:%d}", uid), pInfo[playerid][player_id], "cmd");
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie stworzy³eœ sferyczn¹ strefê [UID: %d, ID: %d].", uid, a_id), "OK", "");
			}
			
			
			return 1;
		}
		
		new type[32], Float:size;
		if( sscanf(param, "s[32]F(0.0)", type, size) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa stworz [sfera/prostokat] (...)");
		
		if( !strcmp(type, "sfera") )
		{
			if( size == 0.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa stworz sfera [wielkosc]");
			
			pInfo[playerid][player_carea_size] = size;
			pInfo[playerid][player_carea_type] = AREA_SHAPE_CIRCLE;
			pInfo[playerid][player_creating_area] = true;
			SendPlayerInformation(playerid, "Aby dodac punkt strefy wcisnij ~r~RMB~w~ lub ~r~LMB ~w~aby usunac ostatnio dodany punkt. ~n~Aby anulowac tworzenie strefy wcisnij ~g~LALT + SPACE~w~.~n~~n~~y~Aby stworzyc strefe uzyj /astrefa stworz.", 30000);
		}
		else if( !strcmp(type, "prostokat") )
		{
			pInfo[playerid][player_carea_type] = AREA_SHAPE_SQUARE;
			pInfo[playerid][player_creating_area] = true;
			SendPlayerInformation(playerid, "Aby dodac punkt strefy wcisnij ~r~RMB~w~ lub ~r~LMB ~w~aby usunac ostatnio dodany punkt. ~n~Aby anulowac tworzenie strefy wcisnij ~g~LALT + SPACE~w~.~n~~n~~y~Aby stworzyc strefe uzyj /astrefa stworz.", 30000);
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa stworz [sfera/prostokat]");
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new a_id;
		if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa usun [id strefy]");	
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa usun [id strefy]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL && Area[a_id][area_type] != AREA_TYPE_PETROL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa usun [id strefy]");
		
		foreach(new p : Player)
		{
			if( IsPlayerInDynamicArea(playerid, a_id) )
			{
				PlayerTextDrawHide(playerid, pInfo[playerid][AreaInfo]);
			}
		}

		AdminLog(sprintf("Removed area {ID:%d}", Area[a_id][area_uid]), pInfo[playerid][player_id], "cmd");
		
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie usun¹³ês strefê [UID: %d, ID: %d].", Area[a_id][area_uid], a_id), "OK", "");
		
		DeleteArea(a_id);
	}
	else if( !strcmp(sub_cmd, "przypisz") )
	{
		new a_id, action[32], who;
		if( sscanf(param, "ds[32]d", a_id, action, who) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa przypisz [id strefy] [grupa/postac/postac_uid/globalna] [uid grupy/id gracza(nie uid)/uid gracza]");
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa przypisz [id strefy] [grupa/postac/postac_uid/globalna] [uid grupy/id gracza(nie uid)/uid gracza]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL && Area[a_id][area_type] != AREA_TYPE_PETROL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa przypisz [id strefy] [grupa/postac/postac_uid/globalna] [uid grupy/id gracza(nie uid)/uid gracza]");
		
		if( !strcmp(action, "grupa") )
		{
			new gid = GetGroupByUid(who);
			if( gid == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Grupa o podanym uid nie istnieje.");
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_areas` SET `area_ownertype` = %d, `area_owner` = %d WHERE `area_uid` = %d", AREA_OWNER_TYPE_GROUP, who, Area[a_id][area_uid]));
			
			Area[a_id][area_owner_type] = AREA_OWNER_TYPE_GROUP;
			Area[a_id][area_owner] = who;
			
			AdminLog(sprintf("Assigned area {ID:%d} to group %s", Area[a_id][area_uid], GroupLogLink(who)), pInfo[playerid][player_id], "cmd");

			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie zmieni³eœ w³aœciciela strefy [UID: %d, ID: %d] na grupê %s [UID: %d, ID: %d].", Area[a_id][area_uid], a_id, Group[gid][group_name], Group[gid][group_uid], gid), "OK", "");
		}
		else if( !strcmp(action, "postac") )
		{
			if( !IsPlayerConnected(who) ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie jest pod³¹czony.");
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_areas` SET `area_ownertype` = %d, `area_owner` = %d WHERE `area_uid` = %d", AREA_OWNER_TYPE_PLAYER, pInfo[who][player_id], Area[a_id][area_uid]));
			
			Area[a_id][area_owner_type] = AREA_OWNER_TYPE_PLAYER;
			Area[a_id][area_owner] = pInfo[who][player_id];
			
			AdminLog(sprintf("Assigned area {ID:%d} to player %s", Area[a_id][area_uid], PlayerLogLink(pInfo[who][player_id])), pInfo[playerid][player_id], "cmd");

			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie zmieni³eœ w³aœciciela strefy [UID: %d, ID: %d] na gracza %s [UID: %d, ID: %d].", Area[a_id][area_uid], a_id, pInfo[who][player_name], pInfo[who][player_id], who), "OK", "");
		}
		else if( !strcmp(action, "postac_uid") )
		{
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT COUNT(*) as exist, char_name FROM `crp_characters` WHERE `char_uid` = %d", who));
			
			new exist = 0, name[MAX_PLAYER_NAME+1];
			
			if( cache_get_rows() )
			{
				exist = cache_get_int(0, "exist");
				cache_get(0, "char_name", name);
			}
			
			cache_delete(result);

			if( !exist ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym uid nie istnieje.");
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_areas` SET `area_ownertype` = %d, `area_owner` = %d WHERE `area_uid` = %d", AREA_OWNER_TYPE_PLAYER, who, Area[a_id][area_uid]));
			
			Area[a_id][area_owner_type] = AREA_OWNER_TYPE_PLAYER;
			Area[a_id][area_owner] = who;
			
			AdminLog(sprintf("Assigned area {ID:%d} to player %s", Area[a_id][area_uid], PlayerLogLink(who)), pInfo[playerid][player_id], "cmd");

			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie zmieni³eœ w³aœciciela strefy [UID: %d, ID: %d] na gracza %s [UID: %d].", Area[a_id][area_uid], a_id, name, who), "OK", "");
		}
		else if( !strcmp(action, "globalne") )
		{
			mysql_pquery(g_sql, sprintf("UPDATE `crp_areas` SET `area_ownertype` = %d, `area_owner` = 0 WHERE `area_uid` = %d", AREA_OWNER_TYPE_GLOBAL, Area[a_id][area_uid]));
			
			Area[a_id][area_owner_type] = AREA_OWNER_TYPE_GLOBAL;
			Area[a_id][area_owner] = 0;
			
			AdminLog(sprintf("Assigned area {ID:%d} to global", Area[a_id][area_uid]), pInfo[playerid][player_id], "cmd");

			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie zmieni³eœ w³aœciciela strefy [UID: %d, ID: %d] na globalnego.", Area[a_id][area_uid], a_id), "OK", "");
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi przypisz [id drzwi] [grupa/postac] [uid grupy/id gracza(nie uid)]");	
	}
	else if( !strcmp(sub_cmd, "objects") )
	{
		new a_id, limit;
		if( sscanf(param, "dd", a_id, limit) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa objects [id strefy] [limit obiektów]");	
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa objects [id strefy] [limit obiektów]");	
		if( limit < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa objects [id strefy] [limit obiektów]");	
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL && Area[a_id][area_type] != AREA_TYPE_PETROL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa objects [id strefy] [limit obiektów]");	
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_areas` SET `area_objects` = %d WHERE `area_uid` = %d", limit, Area[a_id][area_uid]));
			
		Area[a_id][area_objects_limit] = limit;
		
		AdminLog(sprintf("Set area object limit {ID:%d} to {LIMIT:%d}", Area[a_id][area_uid], limit), pInfo[playerid][player_id], "cmd");

		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie zmieni³eœ limit obiektów strefy [LIMIT: %d, UID: %d, ID: %d].", limit, Area[a_id][area_uid], a_id), "OK", "");	
	}
	else if( !strcmp(sub_cmd, "typ") )
	{
		new a_id, type;
		if( sscanf(param, "dd", a_id, type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa typ [id strefy] [typ]");	
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa typ [id strefy] [typ]");	
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL && Area[a_id][area_type] != AREA_TYPE_PETROL ) return SendClientMessage(playerid, COLOR_GREY, "Nie mo¿esz edytowaæ strefy tego typu.");
		
		mysql_query(g_sql, sprintf("UPDATE `crp_areas` SET `area_type` = %d WHERE `area_uid` = %d", type, Area[a_id][area_uid]), false);
		
		new auid = Area[a_id][area_uid];
		DeleteArea(a_id, false);
		
		new aid = LoadArea(sprintf("WHERE area_uid = %d", auid), true);
		
		AdminLog(sprintf("Set area type {ID:%d} to {TYPE:%d}", Area[a_id][area_uid], type), pInfo[playerid][player_id], "cmd");

		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie zmieni³eœ typ strefy [TYP: %d, UID: %d, ID: %d].", type, Area[aid][area_uid], aid), "OK", "");	
	}
	else if( !strcmp(sub_cmd, "podglad") )
	{
		if( pInfo[playerid][player_lookup_area] )
		{
			pInfo[playerid][player_lookup_area] = false;
			GangZoneDestroy(pInfo[playerid][player_carea_zone]);
			
			if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
			if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
			
			PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
		}
		else
		{
			new a_id;
			if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa podglad [id strefy]");	
			if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Strefa o podanym id nie istnieje.");
			if( pInfo[playerid][player_creating_area] ) return SendClientMessage(playerid, COLOR_GREY, "Nie mo¿esz korzystaæ z podgl¹du strefy w trybie tworzenia strefy.");
			if( Area[a_id][area_type] != AREA_TYPE_NORMAL && Area[a_id][area_type] != AREA_TYPE_PETROL ) return SendClientMessage(playerid, COLOR_GREY, "Nie mo¿esz edytowaæ strefy tego typu.");
			
			pInfo[playerid][player_lookup_area] = true;
			new string[140];
		
			new owner_str[80];
			switch( Area[a_id][area_owner_type] )
			{
				case AREA_OWNER_TYPE_GLOBAL:
				{
					format(owner_str, sizeof(owner_str), "globalne; brak");
				}
				
				case AREA_OWNER_TYPE_PLAYER:
				{			
					new name[MAX_PLAYER_NAME+1];
					GetPlayerNameByUid(Area[a_id][area_owner], name);
					
					format(owner_str, sizeof(owner_str), "prywatne; %s", name);
				}
				
				case AREA_OWNER_TYPE_GROUP:
				{			
					new gid = GetGroupByUid(Area[a_id][area_owner]);
					format(owner_str, sizeof(owner_str), "grupowe; %s", Group[gid][group_name]);
				}
			}
			
			format(string, sizeof(string), "%d. Strefa (SampID: %d)~n~~n~~p~Owner: ~w~%d:%d (%s)    ~w~Obiektow: %d/%d", Area[a_id][area_uid], a_id, Area[a_id][area_owner_type], Area[a_id][area_owner], owner_str, CountAreaObjects(a_id), Area[a_id][area_objects_limit]);
				
			PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], string);
			PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);
			
			new Float:point1[3], Float:point2[3], shape, Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT area_shape, area_point1, area_point2 FROM `crp_areas` WHERE `area_uid` = %d", Area[a_id][area_uid]));
			
			new str[100];
			cache_get(0, "area_point1", str);
			sscanf(str, "p<|>a<f>[3]", point1);
			
			new str2[100];
			cache_get(0, "area_point2", str2);
			sscanf(str2, "p<|>a<f>[3]", point2);
			
			shape = cache_get_int(0, "area_shape");
			
			cache_delete(result);
			
			if( shape == AREA_SHAPE_SQUARE  )
			{
				pInfo[playerid][player_carea_zone] = GangZoneCreate(Min(point1[0], point2[0]), Min(point1[1], point2[1]), Max(point1[0], point2[0]), Max(point1[1], point2[1]));
				GangZoneShowForPlayer(playerid, pInfo[playerid][player_carea_zone], 0xFF3C3C80);

				pInfo[playerid][player_carea_label][0] = CreateDynamic3DTextLabel(sprintf("Punkt pierwszy\n(%f, %f, %f)", point1[0], point1[1], point1[2]), COLOR_LIGHTER_RED, point1[0], point1[1], point1[2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
				pInfo[playerid][player_carea_label][1] = CreateDynamic3DTextLabel(sprintf("Punkt drugi\n(%f, %f, %f)", point2[0], point2[1], point2[2]), COLOR_LIGHTER_RED, point2[0], point2[1], point2[2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
			}
			else if( shape == AREA_SHAPE_CIRCLE )
			{
				pInfo[playerid][player_carea_label][0] = CreateDynamic3DTextLabel(sprintf("Punkt pierwszy\n(%f, %f, %f)", point1[0], point1[1], point1[2]), COLOR_LIGHTER_RED, point1[0], point1[1], point1[2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
			}
		}
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(as)trefa [stworz/usun/przypisz/podglad/objects] (...)");
	return 1;
}

COMMAND:ado(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 4)) return 1;
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ado [tekst]");
	
	new str[200];
	
	format(str, sizeof(str), "** %s", BeautifyString(params, true, true), pInfo[playerid][player_name]);

	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		
		SendClientMessage(p, 0x9B91ECFF, str);
	}

	AdminLog(sprintf("[ic:ado] ** %s **", params), pInfo[playerid][player_id], "chat");
	
	return 1;
}

COMMAND:unbw(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 3)) return 1;
	
	new targetid;
	if( sscanf(params, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /unbw [id gracza]");
	if( !IsPlayerConnected(targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie jest zalogowany.");
	if( !pInfo[targetid][player_bw] ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie ma bw, wiêc nie mo¿esz mu go zdj¹æ.");

	return cmd_bw(playerid, sprintf("%d 0", targetid));
}	

COMMAND:bw(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 2)) return 1;
	
	new time, targetid;
	if( sscanf(params, "dd", targetid, time) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /bw [id gracza] [czas]");
	if( time < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /bw [id gracza] [czas]");
	if( !IsPlayerConnected(targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie jest zalogowany.");
		
	if( time == 0 && pInfo[targetid][player_bw] > 0 )
	{
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Nada³eœ graczowi %s [UID: %d, ID: %d] bw na czas: %d minut.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, time), "OK", "");
		// koniec bw
		pInfo[targetid][player_bw] = 0;
		pInfo[targetid][player_bw_end_time] = 0;
		mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_bw` = 0 WHERE `char_uid` = %d", pInfo[targetid][player_id]));
		
		PlayerTextDrawHide(targetid, pInfo[targetid][leftTime]);
		RemovePlayerStatus(targetid, PLAYER_STATUS_BW);
		SetPlayerHealth(targetid, 20);
		TogglePlayerControllable(targetid, 1);
		SetCameraBehindPlayer(targetid);
		ClearAnimations(targetid);
		
		SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);
		
		AdminLog(sprintf("Removed player %s bw status", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");
		AdminLog(sprintf("Player %s removed your bw status", PlayerLogLink(pInfo[playerid][player_id])), pInfo[targetid][player_id], "cmd");

		return 1;
	}
	else if( time == 0 && pInfo[targetid][player_hurted] )
	{
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Zdj¹³eœ graczowi %s [UID: %d, ID: %d] status rannego.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid), "OK", "");
		
		StopPlayerHurted(targetid);
		
		AdminLog(sprintf("Removed player %s hurted status", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");
		AdminLog(sprintf("Player %s removed your hurted status", PlayerLogLink(pInfo[playerid][player_id])), pInfo[targetid][player_id], "cmd");

		return 1;
	}
	else if( time == 0 )
	{
		return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie ma bw, wiêc nie mo¿esz mu go zdj¹æ.");
	}
	
	ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Nada³eœ graczowi %s [UID: %d, ID: %d] bw na czas: %d minut.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, time), "OK", "");

	AdminLog(sprintf("Set player %s bw status {MINUTES:%d}", PlayerLogLink(pInfo[targetid][player_id]), time), pInfo[playerid][player_id], "cmd");
	AdminLog(sprintf("Player %s set your bw status {MINUTES:%d}", PlayerLogLink(pInfo[playerid][player_id]), time), pInfo[targetid][player_id], "cmd");

	pInfo[targetid][player_bw] = 60 * time;
	pInfo[targetid][player_bw_end_time] = pInfo[targetid][player_bw] + gettime();
		
	// Setup bw pos
	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(targetid, x, y, z);
	GetPlayerFacingAngle(targetid, a);
			
	mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_bw`=%d, `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d WHERE `char_uid`=%d", pInfo[targetid][player_bw], x, y, z, a, GetPlayerVirtualWorld(targetid), GetPlayerInterior(targetid), pInfo[targetid][player_id]));
	
	pInfo[targetid][player_quit_pos][0] = x;
	pInfo[targetid][player_quit_pos][1] = y;
	pInfo[targetid][player_quit_pos][2] = z;
	pInfo[targetid][player_quit_pos][3] = a;
	pInfo[targetid][player_quit_vw] = GetPlayerVirtualWorld(targetid);
	pInfo[targetid][player_quit_int] = GetPlayerInterior(targetid);
	
	scrp_SpawnPlayer(targetid);
	return 1;
}

COMMAND:setvw(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	
	new targetid, vw;
	if( sscanf(params, "dd", targetid, vw) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setvw [id gracza] [vw]");
	if( vw < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setvw [id gracza] [vw]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	SetPlayerVirtualWorld(targetid, vw);
	
	if( targetid != playerid ) SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s zmieni³ Twój vw na %d.", pInfo[playerid][player_name], vw);
	SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ vw graczowi %s na %d.", pInfo[targetid][player_name], vw));
	
	AdminLog(sprintf("Set player %s vw {VW:%d}", PlayerLogLink(pInfo[targetid][player_id]), vw), pInfo[playerid][player_id], "cmd");
	if(targetid != playerid) AdminLog(sprintf("Player %s set your vw {VW:%d}", PlayerLogLink(pInfo[playerid][player_id]), vw), pInfo[targetid][player_id], "cmd");

	return 1;
}

COMMAND:setskin(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;

	new targetid, skin;
	if( sscanf(params, "dd", targetid, skin) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setskin [id gracza] [skin]");
	if( !IsValidSkin(skin) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setskin [id gracza] [skin]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	pInfo[targetid][player_skin] = skin;
	mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_skin = %d WHERE char_uid = %d", pInfo[targetid][player_skin], pInfo[targetid][player_id]));
	
	SetPlayerSkin(targetid, skin);
	
	if( targetid != playerid ) SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s zmieni³ Twój skin na %d.", pInfo[playerid][player_name], skin);
	SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ skin graczowi %s na %d.", pInfo[targetid][player_name], skin));

	AdminLog(sprintf("Set player %s skin {SKIN:%d}", PlayerLogLink(pInfo[targetid][player_id]), skin), pInfo[playerid][player_id], "cmd");
	if(targetid != playerid) AdminLog(sprintf("Player %s set your skin {SKIN:%d}", PlayerLogLink(pInfo[playerid][player_id]), skin), pInfo[targetid][player_id], "cmd");
	
	return 1;
}

COMMAND:arestart(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
		
	aRestartExecuting = true;

	AdminLog("Forced game server to restart", pInfo[playerid][player_id], "cmd");

	SendRconCommand("gmx");
	
	return 1;
}

COMMAND:freeze(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	
	new targetid;
	
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /freeze [id gracza]"); 
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	AdminLog(sprintf("Freezed player %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");
	if(targetid != playerid) AdminLog(sprintf("Player %s freezed you", PlayerLogLink(pInfo[playerid][player_id])), pInfo[targetid][player_id], "cmd");
	
	TogglePlayerControllable(targetid, 0);
	
	return 1;
}

COMMAND:anick(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 3)) return 1;
	
	new nick[MAX_PLAYER_NAME+1];
	if( sscanf(params, "s[24]", nick) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /anick [tymczasowy nick]");
	
	strreplace_char(nick, ' ', '_');
	strcopy(pInfo[playerid][player_name], nick, MAX_PLAYER_NAME+1);
		
	UpdatePlayerLabel(playerid);
	
	SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ tymczasowo swój nick na: %s.", nick));
	
	AdminLog(sprintf("Set his temporary nick to %s", nick), pInfo[playerid][player_id], "cmd");

	return 1;
}

COMMAND:unfreeze(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	
	new targetid;
	
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /freeze [id gracza]"); 
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	AdminLog(sprintf("Unfreezed player %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");
	if(targetid != playerid) AdminLog(sprintf("Player %s unfreezed you", PlayerLogLink(pInfo[playerid][player_id])), pInfo[targetid][player_id], "cmd");
	
	TogglePlayerControllable(targetid, 1);
	return 1;
}

COMMAND:ags(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 2)) return 1;

	new targetid, valuee, reason[100];
	
	if( sscanf(params, "ud s[100]", targetid, valuee, reason) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ags [id gracza] [ilosc] [powód]"); 
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( valuee == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Niepoprawna iloœæ punktów gamescore.");
	if(pInfo[playerid][player_level] <= 3 && -1000 > valuee > 1000) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Dla supporterów zakres GameScore wynosi od -1000 do 1000.");
	
	AddPlayerPenalty(targetid, PENALTY_TYPE_GS, playerid, 0, reason, valuee);
	return 1;
}

COMMAND:ag(playerid, params[])
{
	return cmd_agrupa(playerid, params);
}

COMMAND:agrupa(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa [stworz/usun/lista/nazwa/lider/info/typ/wypros/zapros/value1/value2/parent/flagi] (...)");
	
	if( !strcmp(sub_cmd, "stworz") )
	{
		new name[50];
		if( sscanf(param, "s[50]", name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa stworz [nazwa]");
		
		new Cache:result;
		result = mysql_query(g_sql, sprintf("INSERT INTO `crp_groups` (group_uid, group_name, group_tag, group_color) VALUES (null, '%s', 'TAG', 0)", name));
		
		if( cache_insert_id() )
		{
			new gid = Iter_Free(Groups);
			Iter_Add(Groups, gid);
			
			Group[gid][group_uid] = cache_insert_id();
			Group[gid][group_bank_money] = 0;
			
			strcopy(Group[gid][group_name], name);
			
			AdminLog(sprintf("Created group {NAME:%s,ID:%d}", Group[gid][group_name], Group[gid][group_uid]), pInfo[playerid][player_id], "cmd");

			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Stworzy³eœ grupê %s (UID: %d, ID: %d).", Group[gid][group_name], Group[gid][group_uid], gid), "OK", "");
		}

		cache_delete(result);
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa usun [uid grupy]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym id nie istnieje");
		
		foreach(new d_id : Doors)
		{
			if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP && Door[d_id][door_owner] == Group[gid][group_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz usun¹æ grupy, która posiada przypisane drzwi.");
		}
		
		foreach(new a_id : Areas)
		{
			if( Area[a_id][area_owner_type] == AREA_OWNER_TYPE_GROUP && Area[a_id][area_owner] == Group[gid][group_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz usun¹c grupy, która posiada przypisan¹ strefê.");
		}
		
		AdminLog(sprintf("Removed group {NAME:%s,ID:%d}", Group[gid][group_name], Group[gid][group_uid]), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Usun¹³eœ grupê %s [UID: %d, ID: %d].", Group[gid][group_name], Group[gid][group_uid], gid));
		
		// Na poczatku usuwamy z grupy wszystkich graczy online
		foreach(new p : Player)
		{
			new slot = GetPlayerGroupSlot(p, gid);
			if( slot > -1 ) 
			{
				if( pGroup[p][slot][pg_duty] ) PlayerTextDrawHide(p, pInfo[p][GroupDutyTag]);
				CleanPlayerGroup(playerid, slot);
			}
		}
		
		// Teraz usuwamy czlonkow z bazy danych
		mysql_pquery(g_sql, sprintf("DELETE FROM `crp_char_groups` WHERE `group_belongs` = %d", Group[gid][group_uid]));
		
		// Kolej na usuniecie grupy z bazy danych
		mysql_pquery(g_sql, sprintf("DELETE FROM `crp_groups` WHERE `group_uid` = %d", Group[gid][group_uid]));
		
		// Na koncu usuwamy grupe z serwera
		for(new z=0; e_groups:z != e_groups; z++)
		{
			Group[gid][e_groups:z] = 0;
		}
		
		Iter_Remove(Groups, gid);
	}
	else if( !strcmp(sub_cmd, "info") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa info [uid grupy]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym id nie istnieje");
		
		new str[300];
		
		format(str, sizeof(str), "%sTyp grupy:\t\t%s\n", str, GroupTypes[Group[gid][group_type]]);
		format(str, sizeof(str), "%sBud¿et:\t\t\t$%d\n", str, Group[gid][group_bank_money]);
		format(str, sizeof(str), "%sPunkty aktywnoœci:\t%s%d\n", str, ((Group[gid][group_activity_points] >= 1000) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)), Group[gid][group_activity_points]);
		format(str, sizeof(str), "%sDotacja:\t\t$%d\n", str, GetGroupDotation(gid));
		format(str, sizeof(str), "%sTag:\t\t\t%s\n", str, GetGroupTag(gid));

		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) » Informacje", Group[gid][group_name], Group[gid][group_uid]), str, "OK", "");
	}
	else if( !strcmp(sub_cmd, "nazwa") )
	{
		new g_uid, name[64];
		if( sscanf(param, "ds[64]", g_uid, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa nazwa [uid grupy] [nazwa]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym id nie istnieje");
		
		AdminLog(sprintf("Set group {NAME:%s,ID:%d} name to {NAME:%s}", Group[gid][group_name], Group[gid][group_uid], name), pInfo[playerid][player_id], "cmd");

		strcopy(Group[gid][group_name], name);
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_groups` SET `group_name` = '%s' WHERE `group_uid` = %d", Group[gid][group_name], Group[gid][group_uid]));

		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ nazwê grupy [UID: %d, ID: %d] na %s.", Group[gid][group_uid], gid, Group[gid][group_name]));
	}
	else if( !strcmp(sub_cmd, "typ") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa typ [uid grupy]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym id nie istnieje");
		
		new str[400];
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, gid);
		
		for(new i;i<sizeof(GroupTypes);i++)
		{
			format(str, sizeof(str), "%s%02d\t%s\n", str, i+1, GroupTypes[i]);
			DynamicGui_AddRow(playerid, 0, i);
		}

		ShowPlayerDialog(playerid, DIALOG_AGRUPA_TYP, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) » Zmiana typu", Group[gid][group_name], Group[gid][group_uid]), str, "Wybierz", "Zamknij");
	}
	else if( !strcmp(sub_cmd, "flagi") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa flagi [uid grupy]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym uid nie istnieje");
		
		new str[2048];
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, gid);
		
		for(new i;i<sizeof(GroupFlagsBit);i++)
		{
			if( GroupHasFlag(gid, GroupFlagsBit[i]) ) format(str, sizeof(str), "%s"HEX_COLOR_LIGHTER_GREEN"%s\n", str, GroupFlags[i]);
			else format(str, sizeof(str), "%s"HEX_COLOR_LIGHTER_RED"%s\n", str, GroupFlags[i]);
			DynamicGui_AddRow(playerid, i);
		}
		
		ShowPlayerDialog(playerid, DIALOG_AGRUPA_FLAGI, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) » Flagi", Group[gid][group_name], Group[gid][group_uid]), str, "Wybierz", "Zamknij");
	}
	else if( !strcmp(sub_cmd, "parent") )
	{
		new g_uid, g_parent;
		if( sscanf(param, "dd", g_uid, g_parent) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa parent [uid grupy] [uid grupy nadrzednej]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym uid nie istnieje");
		
		new gid_parent = GetGroupByUid(g_parent);
		if( gid_parent == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa nadrzedna o podanym uid nie istnieje");
		
		Group[gid][group_parent_uid] = Group[gid_parent][group_uid];
		mysql_pquery(g_sql, sprintf("UPDATE crp_groups SET group_owner = %d WHERE group_uid = %d", Group[gid_parent][group_uid], Group[gid][group_uid]));
		
		AdminLog(sprintf("Set group %s as parent of %s", GroupLogLink(Group[gid][group_uid]), GroupLogLink(Group[gid_parent][group_uid])), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ grupê nadrzêdn¹ grupy %s (UID: %d) na %s (UID: %d).", Group[gid][group_name], Group[gid][group_uid], Group[gid_parent][group_name], Group[gid_parent][group_uid]));
	}
	else if( !strcmp(sub_cmd, "lider") )
	{
		new g_uid, targetid;
		if( sscanf(param, "dd", g_uid, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa lider [uid grupy] [id gracza]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym uid nie istnieje");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
		
		new slot = GetPlayerGroupSlot(targetid, gid);
		if( slot > -1 )
		{
			mysql_pquery(g_sql, sprintf("UPDATE `crp_char_groups` SET `group_perm` = %d WHERE `char_uid` = %d AND `group_belongs` = %d", 63, pInfo[targetid][player_id], Group[gid][group_uid]));
			
			pGroup[targetid][slot][pg_rank_perm] = 63;
			
			SendGuiInformation(playerid, "Informacja", sprintf("Nada³eœ graczowi %s (UID: %d, ID: %d) uprawnienia lidera w grupie %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
			if( targetid != playerid ) SendGuiInformation(targetid, "Informacja", sprintf("Administrator %s nada³ Ci uprawnienia lidera w grupie %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}
		else
		{
			new free_slot = GetPlayerGroupFreeSlot(targetid);
			if( free_slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie ma ju¿ wolnego slotu grupy.");
			
			mysql_pquery(g_sql, sprintf("INSERT INTO `crp_char_groups` (char_uid, group_belongs, group_perm) VALUES (%d, %d, %d)", pInfo[targetid][player_id], Group[gid][group_uid], 63));
			
			pGroup[targetid][free_slot][pg_id] = gid;
			pGroup[targetid][free_slot][pg_rank_perm] = 63;
			SendGuiInformation(playerid, "Informacja", sprintf("Zaprosi³eœ gracza %s (UID: %d, ID: %d) i nada³eœ mu uprawnienia lidera w grupie %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
			if( targetid != playerid ) SendGuiInformation(targetid, "Informacja", sprintf("Administrator %s zaprosi³ Ciê do grupy %s (UID: %d) nadaj¹c uprawnienia lidera.", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}

		AdminLog(sprintf("Set group %s leader to %s", GroupLogLink(Group[gid][group_uid]), PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "zapros") )
	{
		new g_uid, targetid;
		if( sscanf(param, "dd", g_uid, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa zapros [uid grupy] [id gracza]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym id nie istnieje");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
		if( GetPlayerGroupSlot(targetid, gid) > -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest ju¿ w tej grupie.");
		
		new free_slot = GetPlayerGroupFreeSlot(targetid);
		if( free_slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie ma ju¿ wolnego slotu grupy.");
		
		mysql_pquery(g_sql, sprintf("INSERT INTO `crp_char_groups` (char_uid, group_belongs, group_perm) VALUES (%d, %d, %d)", pInfo[targetid][player_id], Group[gid][group_uid], 31));
			
		pGroup[targetid][free_slot][pg_id] = gid;
		pGroup[targetid][free_slot][pg_rank_perm] = 31;
		SendGuiInformation(playerid, "Informacja", sprintf("Zaprosi³eœ gracza %s (UID: %d, ID: %d) do grupy %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
		if( targetid != playerid ) SendGuiInformation(targetid, "Informacja", sprintf("Administrator %s zaprosi³ Ciê do grupy %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));

		AdminLog(sprintf("Invited %s to group %s", PlayerLogLink(pInfo[targetid][player_id]), GroupLogLink(Group[gid][group_uid])), pInfo[playerid][player_id], "cmd");
		if(targetid != playerid) AdminLog(sprintf("Have been invited to group %s by %s", GroupLogLink(Group[gid][group_uid]), PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "wypros") )
	{
		new g_uid, targetid;
		if( sscanf(param, "dd", g_uid, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa wypros [uid grupy] [id gracza]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym id nie istnieje");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
		
		new slot = GetPlayerGroupSlot(targetid, gid);
		if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest w tej grupie.");
		
		mysql_pquery(g_sql, sprintf("DELETE FROM `crp_char_groups` WHERE `char_uid` = %d AND `group_belongs` = %d", pInfo[targetid][player_id], Group[gid][group_uid]));
		
		if( pGroup[targetid][slot][pg_duty] ) PlayerTextDrawHide(targetid, pInfo[targetid][GroupDutyTag]);
		
		CleanPlayerGroup(targetid, slot);
		
		SendGuiInformation(playerid, "Informacja", sprintf("Wyprosi³eœ gracza %s (UID: %d, ID: %d) z grupy %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
		if( targetid != playerid ) SendGuiInformation(targetid, "Informacja", sprintf("Administrator %s wyprosi³ Ciê z grupy %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));

		AdminLog(sprintf("Removed %s from group %s", PlayerLogLink(pInfo[targetid][player_id]), GroupLogLink(Group[gid][group_uid])), pInfo[playerid][player_id], "cmd");
		if(targetid != playerid) AdminLog(sprintf("Have been removed from group %s by %s", GroupLogLink(Group[gid][group_uid]), PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "value1") )
	{
		new g_uid, val1;
		if( sscanf(param, "dd", g_uid, val1) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa value1 [uid grupy] [wartosc]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym id nie istnieje");
		
		Group[gid][group_value1] = val1;
		mysql_pquery(g_sql, sprintf("UPDATE crp_groups SET group_value1 = %d WHERE group_uid = %d", val1, Group[gid][group_uid]));
		
		AdminLog(sprintf("Set group %s value1 to %d", GroupLogLink(Group[gid][group_uid]), val1), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "value2") )
	{
		new g_uid, val2;
		if( sscanf(param, "dd", g_uid, val2) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa value1 [uid grupy] [wartosc]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym id nie istnieje");
		
		Group[gid][group_value2] = val2;
		mysql_pquery(g_sql, sprintf("UPDATE crp_groups SET group_value2 = %d WHERE group_uid = %d", val2, Group[gid][group_uid]));
		
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ wartoœæ1 grupy %s (UID: %d) na %d.", Group[gid][group_name], Group[gid][group_uid], val2));
		AdminLog(sprintf("Set group %s value2 to %d", GroupLogLink(Group[gid][group_uid]), val2), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "lista") )
	{
		new page, keyword[50];
		sscanf(param, "D(1)S()[50]", page, keyword);
		if(page == 1 && isnull(keyword)) SendPlayerInformation(playerid, "Mozesz rowniez:~n~/ag lista (strona) (fraza)", 4000);

		if( strlen(keyword) > 0 ) {
			format(keyword, sizeof(keyword), "WHERE g.group_name LIKE '%%%s%%' ", keyword);
		}

		mysql_tquery(g_sql, sprintf("SELECT g.group_uid, g.group_name, cp.char_uid, cp.char_name FROM crp_groups g LEFT JOIN crp_char_groups cg ON cg.group_belongs = g.group_uid AND cg.group_perm & 32 LEFT JOIN crp_characters cp ON cp.char_uid = cg.char_uid %sGROUP BY g.group_uid LIMIT %d, 20", keyword, (page-1)*20), "OnAgroupListLoaded", "i", playerid);
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa [stworz/usun/lista/nazwa/lider/info/typ/wypros/zapros/value1/value2/parent/flagi] (...)");
	return 1;
}

function OnAgroupListLoaded(playerid)
{
	if(!cache_get_rows()) return SendPlayerInformation(playerid, "Brak wynikow spelniajacych kryteria.", 4000);

	new list_str[3000], gname[50], cname[50], cuid[20];

	format(list_str, sizeof(list_str), "UID grupy\tNazwa grupy\tUID lidera\tNazwa lidera");

	for(new i;i<cache_get_rows();i++)
	{
		cache_get(i, "group_name", gname);

		if( !cache_is_null(i, "char_name") ) cache_get(i, "char_name", cname);
		else strcopy(cname, "------");

		if( !cache_is_null(i, "char_uid") ) format(cuid, sizeof(cuid), "%d", cache_get_int(i, "char_uid"));
		else strcopy(cuid, "--");

		format(list_str, sizeof(list_str), "%s\n%d\t%s\t%s\t%s", list_str, cache_get_int(i, "group_uid"), gname, cuid, cname);
	}

	ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST_HEADERS, "Lista grup", list_str, "Zamknij", "");

	return 1;
}

COMMAND:av(playerid, params[])
{	
	return cmd_avehicle(playerid, params);
}

COMMAND:avehicle(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle [stworz/usun/model/hp/visual/przypisz/id/zaparkuj/resp/spawn/unspawn/paliwotyp/paliwo/goto/gethere/kolor1/kolor2] (...)");
	
	if( !strcmp(sub_cmd, "stworz") )
	{
		new model, c1, c2;
		if( sscanf(param, "ddd", model, c1, c2) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle stworz [model] [kolor1] [kolor2]");
		if( model < 400 || model > 611 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Poda³eœ b³êdny model pojazdu.");
		
		new Float:v_pos[4];
		GetPointInAngleOfPlayer(playerid, v_pos[0], v_pos[1], v_pos[2], 4.0, 0.0);
		
		GetPlayerFacingAngle(playerid, v_pos[3]);
		if( v_pos[3] > 90.0 ) v_pos[3] -= 90.0;
		else v_pos[3] += 90.0;
		
		new Cache:result;
		result = mysql_query(g_sql, sprintf("INSERT INTO `crp_vehicles` (vehicle_uid, vehicle_model, vehicle_posx, vehicle_posy, vehicle_posz, vehicle_posa, vehicle_world, vehicle_interior, vehicle_color1, vehicle_color2, vehicle_fueltype, vehicle_fuel) VALUES (null, %d, %f, %f, %f, %f, %d, %d, %d, %d, 1, %f)", model, v_pos[0], v_pos[1], v_pos[2], v_pos[3], GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), c1, c2, VehicleFuelMax[model-400]));
		new uid = cache_insert_id();

		cache_delete(result);
	
		new vid = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);
		
		AdminLog(sprintf("Created vehicle %s", VehicleLogLink(uid)), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Stworzy³eœ pojazd %s [UID: %d, ID: %d].", VehicleNames[model-400], uid, vid));
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle usun [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");

		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie usun¹³eœ pojazd %s [UID: %d, ID: %d]", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
		
		AdminLog(sprintf("Removed vehicle %s", VehicleLogLink(Vehicle[vid][vehicle_uid])), pInfo[playerid][player_id], "cmd");
		
		DeleteVehicle(vid, true);
	}
	else if( !strcmp(sub_cmd, "spawn") )
	{
		new v_uid;
		if( sscanf(param, "d", v_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle spawn [uid pojazdu]");
		if( v_uid < 1 ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle spawn [uid pojazdu]");
		
		new vid = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", v_uid), true);
		
		if( vid == INVALID_VEHICLE_ID ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid nie istnieje lub jest ju¿ zespawnowany.");
		else SendGuiInformation(playerid, "Informacja", sprintf("Zespawnowa³eœ pojazd %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], v_uid, vid));
		
		AdminLog(sprintf("Spawned vehicle %s", VehicleLogLink(v_uid)), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "unspawn") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle unspawn [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		
		SendGuiInformation(playerid, "Informacja", sprintf("Odspawnowa³eœ pojazd %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
		AdminLog(sprintf("Unspawned vehicle %s", VehicleLogLink(Vehicle[vid][vehicle_uid])), pInfo[playerid][player_id], "cmd");

		DeleteVehicle(vid);
	}
	else if( !strcmp(sub_cmd, "model") )
	{
		new vid, model;
		if( sscanf(param, "dd", vid, model) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle model [id pojazdu] [model]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		if( model < 400 || model > 611 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Poda³eœ b³êdny model pojazdu.");
		
		mysql_query(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_model` = %d WHERE `vehicle_uid` = %d", model, Vehicle[vid][vehicle_uid]), false);
		
		new uid = Vehicle[vid][vehicle_uid];
		DeleteVehicle(vid);
		
		new v_id = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ model pojazu na %s [UID: %d, ID: %d]", VehicleNames[GetVehicleModel(v_id)-400], uid, v_id));

		AdminLog(sprintf("Changed vehicle %s model to %d", VehicleLogLink(uid), model), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "id") )
	{
		new v_uid;
		if( sscanf(param, "d", v_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle id [uid pojazdu]");
		if( v_uid < 1 ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle id [uid pojazdu]");
		
		new vid = GetVehicleByUid(v_uid);
		
		if( vid == INVALID_VEHICLE_ID ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid nie jest zespawnowany.");
		else SendGuiInformation(playerid, "Informacja", sprintf("Pojazd %s posiada ID: %d.", VehicleNames[GetVehicleModel(vid)-400], vid));
	}
	else if( !strcmp(sub_cmd, "hp") )
	{
		new vid, Float:hp;
		if( sscanf(param, "df", vid, hp) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle hp [id pojazdu] [hp]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		if( hp < 0.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle hp [id pojazdu] [hp]");
		
		SetVehicleHealth(vid, hp);
		Vehicle[vid][vehicle_destroyed] = false; 
		mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_health` = %f WHERE `vehicle_uid` = %d", hp, Vehicle[vid][vehicle_uid]));
		
		SendGuiInformation(playerid, "Informacja", sprintf("Ustawi³eœ %.1f hp pojazdowi %s [UID: %d, ID: %d].", hp, VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
		
		AdminLog(sprintf("Changed vehicle %s health to {HEALTH:%.1f}", VehicleLogLink(Vehicle[vid][vehicle_uid]), hp), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "visual") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle visual [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		
		Vehicle[vid][vehicle_damage][0] = 0;
		Vehicle[vid][vehicle_damage][1] = 0;
		Vehicle[vid][vehicle_damage][2] = 0;
		Vehicle[vid][vehicle_damage][3] = 0;
		
		UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);
		
		SaveVehicle(vid);
		
		SendGuiInformation(playerid, "Informacja", sprintf("Naprawi³eœ uszkodzenia wizualne pojazdu %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));

		AdminLog(sprintf("Repaired vehicle %s visual damage", VehicleLogLink(Vehicle[vid][vehicle_uid])), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "zaparkuj") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle zaparkuj [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		
		GetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]);
		GetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
		Vehicle[vid][vehicle_park_world] = GetVehicleVirtualWorld(vid);
		Vehicle[vid][vehicle_park_interior] = Vehicle[vid][vehicle_interior];

		mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_posx` = %f, `vehicle_posy` = %f, `vehicle_posz` = %f, `vehicle_posa` = %f, `vehicle_world` = %d, `vehicle_interior` = %d WHERE `vehicle_uid` = %d", Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2], Vehicle[vid][vehicle_park][3], Vehicle[vid][vehicle_park_world], Vehicle[vid][vehicle_park_interior] ,Vehicle[vid][vehicle_uid]));
	
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zaparkowa³eœ pojazd %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));

		AdminLog(sprintf("Set vehicle %s spawn to {POS:%.1f,%.1f,%.1fVW:}", VehicleLogLink(Vehicle[vid][vehicle_uid]), Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2], Vehicle[vid][vehicle_park_world]), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "przypisz") )
	{
		new vid, action[10], id;
		if( sscanf(param, "ds[10]d", vid, action, id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle przypisz [id pojazdu] [postac/grupa] [id postaci/uid grupy]");
		
		new old[2];
		old[0] = Vehicle[vid][vehicle_owner_type];
		old[1] = Vehicle[vid][vehicle_owner];

		if( !strcmp(action, "postac") )
		{
			if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_PLAYER, pInfo[id][player_id], Vehicle[vid][vehicle_uid]));
			Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_PLAYER;
			Vehicle[vid][vehicle_owner] = pInfo[id][player_id];
			
			SendGuiInformation(playerid, "Informacja", sprintf("Przypisa³eœ pojazd %s [UID: %d, ID: %d] graczowi %s [ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, pInfo[id][player_name], id));

			AdminLog(sprintf("Set vehicle %s owner to player %s", VehicleLogLink(Vehicle[vid][vehicle_uid]), PlayerLogLink(pInfo[id][player_id])), pInfo[playerid][player_id], "cmd");
		}
		else if( !strcmp(action, "grupa") )
		{
			new gid = GetGroupByUid(id);
			if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa o podanym uid nie istnieje.");
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_GROUP, id, Vehicle[vid][vehicle_uid]));
			Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_GROUP;
			Vehicle[vid][vehicle_owner] = id;
			
			SendGuiInformation(playerid, "Informacja", sprintf("Przypisa³eœ pojazd %s [UID: %d, ID: %d] grupie %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, Group[gid][group_name], id));

			AdminLog(sprintf("Set vehicle %s owner to group %s", VehicleLogLink(Vehicle[vid][vehicle_uid]), GroupLogLink(id)), pInfo[playerid][player_id], "cmd");
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle przypisz [id pojazdu] [postac/grupa] [id postaci/uid grupy]");
	}
	else if( !strcmp(sub_cmd, "paliwotyp") )
	{
		new vid, type;
		if( sscanf(param, "dd", vid, type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle paliwotyp [id pojazdu] [1 - benzyna, 2 - gaz, 3 - diesel, 4 - paliwo lotnicze]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		if( type < 1 || type > 4 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Poda³eœ b³êdny typ paliwa.");
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_fueltype` = %d WHERE `vehicle_uid` = %d", type, Vehicle[vid][vehicle_uid]));
		
		AdminLog(sprintf("Set vehicle %s fueltype to {FUEL_TYPE:%d}", VehicleLogLink(Vehicle[vid][vehicle_uid]), type), pInfo[playerid][player_id], "cmd");

		Vehicle[vid][vehicle_fuel_type] = type;

		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieni³eœ typ paliwa pojazdu %s [UID: %d, ID: %d] na: %s.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, VehicleFuelTypes[type]));
	}
	else if( !strcmp(sub_cmd, "paliwo") )
	{
		new vid, Float:amount;
		if( sscanf(param, "df", vid, amount) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle paliwo [id pojazdu] [ilosc]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		if( amount < 0.0 || amount > 3000.0 ) return  SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Poda³eœ b³êdn¹ iloœæ paliwa.");
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_fuel` = %f WHERE `vehicle_uid` = %d", amount, Vehicle[vid][vehicle_uid]));
		Vehicle[vid][vehicle_fuel_current] = amount;
		
		AdminLog(sprintf("Set vehicle %s fuel to {FUEL:%.1f}", VehicleLogLink(Vehicle[vid][vehicle_uid]), amount), pInfo[playerid][player_id], "cmd");
	}
	else if( !strcmp(sub_cmd, "goto") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle goto [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		
		new Float:v_pos[3];
		GetVehiclePos(vid, v_pos[0], v_pos[1], v_pos[2]);
		
		SetPlayerPos(playerid, v_pos[0], v_pos[1], v_pos[2] + 2);
		SetPlayerVirtualWorld(playerid, GetVehicleVirtualWorld(vid));
		SetPlayerInterior(playerid, Vehicle[vid][vehicle_interior]);
		
		AdminLog(sprintf("Teleported to vehicle %s", VehicleLogLink(Vehicle[vid][vehicle_uid])), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Przeteleportowa³eœ siê do pojazdu %s [UID: %d, ID: %d, VW: %d, INT: %d]", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, GetVehicleVirtualWorld(vid), Vehicle[vid][vehicle_interior]));
	}
	else if( !strcmp(sub_cmd, "gethere") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle gethere [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");

		
		if(GetVehicleDriver(vid) == INVALID_PLAYER_ID)
		{
			new Float:pos[3];
			GetPlayerPos(playerid, pos[0], pos[1], pos[2]);

			SetVehiclePos(vid, pos[0], pos[1], pos[2]-1);
			SetVehicleVirtualWorld(vid, GetPlayerVirtualWorld(playerid));
			LinkVehicleToInterior(vid, GetPlayerInterior(playerid));
			PutPlayerInVehicle(playerid, vid, 0);

			if(Vehicle[vid][vehicle_locked]) {
				Vehicle[vid][vehicle_locked] = false;
			}	
		}
		else
		{
			new Float:v_pos[4];
			GetPointInAngleOfPlayer(playerid, v_pos[0], v_pos[1], v_pos[2], 4.0, 0.0);
			
			GetPlayerFacingAngle(playerid, v_pos[3]);
			if( v_pos[3] > 90.0 ) v_pos[3] -= 90.0;
			else v_pos[3] += 90.0;
			
			SetVehiclePos(vid, v_pos[0], v_pos[1], v_pos[2]+1);
			SetVehicleZAngle(vid, v_pos[3]);
			SetVehicleVirtualWorld(vid, GetPlayerVirtualWorld(playerid));
			LinkVehicleToInterior(vid, GetPlayerInterior(playerid));
		}
		
		new
			Float:x,
			Float:y,
			Float:z;

		GetPlayerPos(playerid, x, y, z);
		AdminLog(sprintf("Teleported vehicle %s to his location {POS:%.1f,%.1f,%.1f}", VehicleLogLink(Vehicle[vid][vehicle_uid]), x, y, z), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Przeteleportowa³eœ do siebie pojazd %s [UID: %d, ID: %d]", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else if( !strcmp(sub_cmd, "kolor1") )
	{
		new vid, color;
		if( sscanf(param, "dd", vid, color) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle kolor1 [id pojazdu] [kolor]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		if( color < 0 || color > 255 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Podany kolor nie istnieje.");
		
		AdminLog(sprintf("Set vehicle %s color1 to {COLOR1:%d}", VehicleLogLink(Vehicle[vid][vehicle_uid]), color), pInfo[playerid][player_id], "cmd");

		Vehicle[vid][vehicle_color][0] = color;
		mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_color1` = %d WHERE `vehicle_uid` = %d", color, Vehicle[vid][vehicle_uid]));
		
		ChangeVehicleColor(vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);
		
		SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ kolor pojazdu %s [UID: %d, ID: %d] na %d:%d.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]));
	}
	else if( !strcmp(sub_cmd, "kolor2") )
	{
		new vid, color;
		if( sscanf(param, "dd", vid, color) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle kolor1 [id pojazdu] [kolor]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		if( color < 0 || color > 255 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Podany kolor nie istnieje.");
		
		AdminLog(sprintf("Set vehicle %s color2 to {COLOR2:%d}", VehicleLogLink(Vehicle[vid][vehicle_uid]), color), pInfo[playerid][player_id], "cmd");

		Vehicle[vid][vehicle_color][1] = color;
		mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_color2` = %d WHERE `vehicle_uid` = %d", color, Vehicle[vid][vehicle_uid]));
		
		ChangeVehicleColor(vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);

		SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ kolor pojazdu %s [UID: %d, ID: %d] na %d:%d.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]));
	}
	else if( !strcmp(sub_cmd, "resp") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle resp [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
		
		new uid = Vehicle[vid][vehicle_uid];
		
		DeleteVehicle(vid);
		
		new vidd = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);
		
		AdminLog(sprintf("Respawned vehicle %s", VehicleLogLink(Vehicle[vid][vehicle_uid])), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Pojazd %s [UID: %d, ID: %d] powróci³ na miejsce spawnu.", VehicleNames[GetVehicleModel(vidd)-400], Vehicle[vidd][vehicle_uid], vidd));
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle [stworz/usun/model/hp/visual/przypisz/id/zaparkuj/spawn/unspawn/resp/paliwotyp/paliwo/goto/gethere/kolor1/kolor2] (...)");
	return 1;
}

COMMAND:aactor(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;

	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor [stworz/usun/animacja/pozycja/nazwa(24 znaki)]/skin");

	new
		Float:p_posx,
		Float:p_posy,
		Float:p_posz,
		Float:p_posr;

	GetPlayerPos(playerid, p_posx, p_posy, p_posz);
	GetPlayerFacingAngle(playerid, p_posr);

	if( !strcmp(sub_cmd, "stworz") )
	{
		new type, skin, name[24];
		if( sscanf(param, "dds[24]", skin, type, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor stworz [skin] [typ] [nazwa]");

		new Cache:result;
        result = mysql_query(g_sql, sprintf("INSERT INTO `crp_actors` (actor_type, actor_skin, actor_name, actor_px, actor_py, actor_pz, actor_pa, actor_vw, actor_int) VALUES (%d, %d, '%s', %f, %f, %f, %f, %d, %d)", type, skin, name, p_posx, p_posy, p_posz, p_posr, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid)));		
		
		new uid = cache_insert_id();
		cache_delete(result);

		new actorid = LoadActor(sprintf("WHERE `actor_uid` = %d", uid), true);

		TeleportPlayerInFrontOfActor(playerid, actorid);

		AdminLog(sprintf("Created actor %s", ActorLogLink(uid)), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie stworzy³eœ aktora %s [UID: %d; ID: %d]", name, uid, actorid));
	}
	else if(!strcmp(sub_cmd, "usun"))
	{
		new id;
		if( sscanf(param, "d", id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor usun [id]");

		AdminLog(sprintf("Removed actor %s", ActorLogLink(Actor[id][actor_uid])), pInfo[playerid][player_id], "cmd");
		DeleteActor(id);
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœnie usunales aktora o ID %d", id));
	}
	else if(!strcmp(sub_cmd, "pozycja"))
	{
		new id;

		GetPlayerPos(playerid, p_posx, p_posy, p_posz);
		GetPlayerFacingAngle(playerid, p_posr);

		if( sscanf(param, "d", id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor pozycja [id]");

		mysql_query(g_sql, sprintf("UPDATE `crp_actors` SET `actor_px` = '%f', `actor_py` = '%f', `actor_pz` = '%f', `actor_pa` = '%f' WHERE `actor_uid` = '%d'", p_posx, p_posy, p_posz, p_posr, Actor[id][actor_uid]), false);
	
		new auid = Actor[id][actor_uid];
		DeleteActor(id, false);

		new actorid = LoadActor(sprintf("WHERE actor_uid = %d", auid), true);

		TeleportPlayerInFrontOfActor(playerid, actorid);

		AdminLog(sprintf("Set actor %s position to {POS:%.1f,%.1f,%.1f}", ActorLogLink(Actor[actorid][actor_uid]), p_posx, p_posy, p_posz), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie przenios³eœ aktora o ID %d", actorid));
	}
	else if(!strcmp(sub_cmd, "nazwa"))
	{
		new id, name[40];
		if( sscanf(param, "ds[40]", id, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor nazwa [id] [nick]");

		mysql_query(g_sql, sprintf("UPDATE `crp_actors` SET `actor_name` = '%s' WHERE `actor_uid` = '%d'", name, Actor[id][actor_uid]), false);

		new auid = Actor[id][actor_uid];
		DeleteActor(id, false);

		new actorid = LoadActor(sprintf("WHERE actor_uid = %d", auid), true);

		AdminLog(sprintf("Set actor %s name to %s", ActorLogLink(auid), name), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieniles nick aktora o ID %d [nazwa: %s]", actorid, name));
	}
	else if(!strcmp(sub_cmd, "skin"))
	{
		new id, skin;
		if( sscanf(param, "dd", id, skin) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor skin [id] [skin]");
		
		Actor[id][actor_skin] = skin;
		
		DestroyDynamicActor(id);
		CreateDynamicActor(Actor[id][actor_skin], Actor[id][actor_pos_x], Actor[id][actor_pos_y], Actor[id][actor_pos_z], Actor[id][actor_pos_a], 1, 100.0, Actor[id][actor_virtual], Actor[id][actor_interior]);

		mysql_query(g_sql, sprintf("UPDATE `crp_actors` SET `actor_skin` = '%d' WHERE `actor_uid` = '%d'", skin, Actor[id][actor_uid]));
		
		AdminLog(sprintf("Set actor %s skin to {SKIN:%d}", ActorLogLink(Actor[id][actor_uid]), skin), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieniles skin aktora o ID %d [skin: %d]", id, skin));
	}
	else if(!strcmp(sub_cmd, "gp"))
	{
		new id;
		if( sscanf(param, "d", id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor gp [id]");

		new Float:x, Float:y, Float:z;
		GetDynamicActorPos(id, x, y, z);

		SendClientMessage(playerid, COLOR_RED, sprintf("Pozycja aktora to: x%f y%f z%f", x, y, z));
	}
	else if(!strcmp(sub_cmd, "typ"))
	{
		new id, type;
		if( sscanf(param, "dd", id, type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor typ [id] [typ]");
		Actor[id][actor_type] = type;

		mysql_pquery(g_sql, sprintf("UPDATE `crp_actors` SET `actor_type` = '%d' WHERE `actor_uid` = '%d'", type, Actor[id][actor_uid]));

		AdminLog(sprintf("Set actor %s type to {TYPE:%d}", ActorLogLink(Actor[id][actor_uid]), type), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zmieniles typ aktora o ID %d [typ: %d]", id, type));
	}
	else if(!strcmp(sub_cmd, "animacja"))
	{
		new id, anim_name[24];
		if( sscanf(param, "dS()[24]", id, anim_name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor animacja [id] [anim name]");

        strcopy(Actor[id][actor_anim], anim_name);

        mysql_pquery(g_sql, sprintf("UPDATE `crp_actors` SET `actor_anim` = '%s' WHERE `actor_uid` = %d", anim_name, Actor[id][actor_uid]));

        AdminLog(sprintf("Set actor %s anim to {ANIM:%s}", ActorLogLink(Actor[id][actor_uid]), anim_name), pInfo[playerid][player_id], "cmd");

        if( isnull(anim_name) ) ClearDynamicActorAnimations(id);
        else _ApplyDynamicActorAnimation(id, anim_name);
	}	
	else if(!strcmp(sub_cmd, "goto"))
	{
		new actorid;
		if( sscanf(param, "d", actorid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aactor goto [id]");

        TeleportPlayerInFrontOfActor(playerid, actorid);
	}

	return 1;
}

COMMAND:ap(playerid, params[])
{
	return cmd_aprzedmiot(playerid, params);
}

COMMAND:aprzedmiot(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot [stworz/stworzm/usun/value1/value2/value3/typ/owner/amount/price] (...)");
	
	if( !strcmp(sub_cmd, "stworz") )
	{
		new type, val1, val2, name[40];
		if( sscanf(param, "ddds[40]", type, val1, val2, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot stworz [typ] [value1] [value2] [nazwa]");
		if( type > MAX_ITEM_TYPE ) return SendGuiInformation(playerid, "Informacja", sprintf("Maksymalny typ przedmiotu to %d", MAX_ITEM_TYPE));

		new id = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, type, val1, val2, name, 0);
		
		AdminLog(sprintf("Created item %s", ItemLogLink(Item[id][item_uid])), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Stworzy³eœ przedmiot %s [TYP: %d, V1: %d, V2: %d].", name, type, val1, val2));
	}
	else if( !strcmp(sub_cmd, "stworzm") )
	{
		new type, model, val1, val2, name[40];
		if( sscanf(param, "dddds[40]", type, val1, val2, model, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot stworz [typ] [value1] [value2] [model] [nazwa]");
		if( type > MAX_ITEM_TYPE ) return SendGuiInformation(playerid, "Informacja", sprintf("Maksymalny typ przedmiotu to %d", MAX_ITEM_TYPE));

		new id = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, type, val1, val2, name, model);
		
		AdminLog(sprintf("Created item %s", ItemLogLink(Item[id][item_uid])), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Stworzy³eœ przedmiot %s [TYP: %d, MODEL: %d, V1: %d, V2: %d].", name, type, model, val1, val2));
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new itemuid;
		if(	sscanf(param, "d", itemuid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot usun [uid przedmiotu]");
		
		new Cache:result;
		result = mysql_query(g_sql, sprintf("SELECT 1 as exist FROM crp_items WHERE item_uid = %d", itemuid));
		
		new bool:exist = cache_get_bool(0, "exist");
		
		cache_delete(result);
		
		if( !exist ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Przedmiot o podanym uid nie istnieje.");
		
		new itemid = GetItemByUid(itemuid);
		if( itemid == -1 ) mysql_pquery(g_sql, sprintf("DELETE FROM `crp_items` WHERE `item_uid` = %d", itemuid));
		else DeleteItem(itemid, true);
		
		AdminLog(sprintf("Removed item %s", ItemLogLink(itemuid)), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Usun¹³eœ przedmiot o UID: %d.", itemuid));
	}
	else if( !strcmp(sub_cmd, "value1") )
	{
		new itemuid, value1;
		if(	sscanf(param, "dd", itemuid, value1) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot value1 [uid przedmiotu] [value1]");

		mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_value1` = %d WHERE `item_uid` = %d", value1, itemuid));
		
		new itemid = GetItemByUid(itemuid);
		if( itemid > -1 ) Item[itemid][item_value1] = value1;
		
		AdminLog(sprintf("Set item %s value1 to {VALUE1:%d}", ItemLogLink(itemuid), value1), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ wartoœæ 1 przedmiotu o UID: %d na: %d.", itemuid, value1));
	}
	else if( !strcmp(sub_cmd, "value2") )
	{
		new itemuid, value2;
		if(	sscanf(param, "dd", itemuid, value2) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot value2 [uid przedmiotu] [value2]");

		mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_value2` = %d WHERE `item_uid` = %d", value2, itemuid));
		
		new itemid = GetItemByUid(itemuid);
		if( itemid > -1 ) Item[itemid][item_value2] = value2;
		
		AdminLog(sprintf("Set item %s value2 to {VALUE2:%d}", ItemLogLink(itemuid), value2), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ wartoœæ 2 przedmiotu o UID: %d na: %d.", itemuid, value2));
	}
	else if( !strcmp(sub_cmd, "value3") )
	{
		new itemuid, value3;
		if(	sscanf(param, "dd", itemuid, value3) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot value3 [uid przedmiotu] [value3]");

		mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_value3` = %d WHERE `item_uid` = %d", value3, itemuid));
		
		new itemid = GetItemByUid(itemuid);
		if( itemid > -1 ) Item[itemid][item_value3] = value3;
		
		AdminLog(sprintf("Set item %s value3 to {VALUE3:%d}", ItemLogLink(itemuid), value3), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ wartoœæ 3 przedmiotu o UID: %d na: %d.", itemuid, value3));
	}
	else if( !strcmp(sub_cmd, "typ") )
	{
		new itemuid, typ;
		if(	sscanf(param, "dd", itemuid, typ) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot typ [uid przedmiotu] [typ]");
		if( typ > MAX_ITEM_TYPE ) return SendGuiInformation(playerid, "Informacja", sprintf("Maksymalny typ przedmiotu to %d", MAX_ITEM_TYPE));
		mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_type` = %d WHERE `item_uid` = %d", typ, itemuid));
		
		new itemid = GetItemByUid(itemuid);
		if( itemid > -1 ) Item[itemid][item_type] = typ;
		
		AdminLog(sprintf("Set item %s type to {TYPE:%d}", ItemLogLink(itemuid), typ), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ typ przedmiotu o UID: %d na: %d.", itemuid, typ));
	}
	else if( !strcmp(sub_cmd, "owner") )
	{
		new itemuid, ownertype, owner;
		if(	sscanf(param, "ddd", itemuid, ownertype, owner) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot owner [uid przedmiotu] [typ w³aœciciela] [uid w³aœciciela]");
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_uid` = %d", ownertype, owner, itemuid));

		new itemid = GetItemByUid(itemuid);
		if( itemid > -1 ) 
		{
			
			Item[itemid][item_owner_type] = ownertype;
			Item[itemid][item_owner] = owner;
		}
		
		AdminLog(sprintf("Set item %s owner to {OWNER_TYPE:%d,OWNER:%d}", ItemLogLink(itemuid), owner, ownertype), pInfo[playerid][player_id], "cmd");

		SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ w³aœciciela przedmiotu o UID: %d na: [%d:%d].", itemuid, ownertype, owner));
	}
	else if( !strcmp(sub_cmd, "amount") )
	{
		new itemuid, amount;
		if(	sscanf(param, "dd", itemuid, amount) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot amount [uid przedmiotu] [iloœæ]");
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_amount` = %d WHERE `item_uid` = %d", amount, itemuid));
		
		new itemid = GetItemByUid(itemuid);
		AdminLog(sprintf("Set item %s amount to {AMOUNT:%d}", ItemLogLink(itemuid), amount), pInfo[playerid][player_id], "cmd");
		if( itemid > -1 ) Item[itemid][item_amount] = amount;
		
		SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ iloœæ przedmiotu o UID: %d na: %d.", itemuid, amount));
	}
	else if( !strcmp(sub_cmd, "price") )
	{
		new itemuid, price;
		if(	sscanf(param, "dd", itemuid, price) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot price [uid przedmiotu] [cena]");
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_price` = %d WHERE `item_uid` = %d", price, itemuid));
		
		new itemid = GetItemByUid(itemuid);
		if( itemid > -1 ) Item[itemid][item_price] = price;
		
		AdminLog(sprintf("Set item %s price to {PRICE:%d}", ItemLogLink(itemuid), price), pInfo[playerid][player_id], "cmd");
		SendGuiInformation(playerid, "Informacja", sprintf("Zmieni³eœ cenê przedmiotu o UID: %d na: %d.", itemuid, price));
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot [stworz/usun/value1/value2/value3/typ/owner/amount/price] (...)");
	
	return 1;
}

COMMAND:attach(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach [stworz/usun/edytuj/print] (...)");
	
	if( !strcmp(sub_cmd, "stworz") )
	{
		new model, bone;
		if( sscanf(param, "dd", model, bone) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach stworz [model] [bone]");
		
		new freeid = GetPlayerFreeAttachSlot(playerid);
		if( freeid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz przyczepiæ ju¿ wiêcej obiektów.");
		
		SetPlayerAttachedObject(playerid, freeid, model, bone);
		SendFormattedClientMessage(playerid, COLOR_GOLD, "Utworzy³eœ nowy obiekt przyczepialny [INDEX: %d]. Aby zapisaæ jego pozycjê, zapisz edycjê za pomoc¹ ikonky dyskietki.", freeid);
		
		ao[playerid][freeid][ao_model] = model;
        ao[playerid][freeid][ao_bone] = bone;
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach usun [index]");
		if( !IsPlayerAttachedObjectSlotUsed(playerid, index) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym slocie nie masz ¿adnego obiektu.");
		
		ao[playerid][index][ao_model] = -1;
        ao[playerid][index][ao_bone] = -1;
		ao[playerid][index][ao_x] = 0.0;
        ao[playerid][index][ao_y] = 0.0;
        ao[playerid][index][ao_z] = 0.0;
        ao[playerid][index][ao_rx] = 0.0;
        ao[playerid][index][ao_ry] = 0.0;
        ao[playerid][index][ao_rz] = 0.0;
        ao[playerid][index][ao_sx] = 0.0;
        ao[playerid][index][ao_sy] = 0.0;
        ao[playerid][index][ao_sz] = 0.0;
		
		RemovePlayerAttachedObject(playerid, index);
		SendGuiInformation(playerid, "Informacja", "Pomyœlnie usun¹³eœ obiekt przyczepialny");
	}
	else if( !strcmp(sub_cmd, "edytuj") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach edytuj [index]");
		if( !IsPlayerAttachedObjectSlotUsed(playerid, index) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym slocie nie masz ¿adnego obiektu.");
		
		EditAttachedObject(playerid, index);
	}
	else if( !strcmp(sub_cmd, "print") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach print [index]");
		if( !IsPlayerAttachedObjectSlotUsed(playerid, index) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym slocie nie masz ¿adnego obiektu.");
		
		printf("[model %d, bone: %d] %f, %f, %f, %f, %f, %f, %f, %f, %f", ao[playerid][index][ao_model], ao[playerid][index][ao_bone], ao[playerid][index][ao_x], ao[playerid][index][ao_y], ao[playerid][index][ao_z], ao[playerid][index][ao_rx], ao[playerid][index][ao_ry], ao[playerid][index][ao_rz], ao[playerid][index][ao_sx], ao[playerid][index][ao_sy], ao[playerid][index][ao_sz]);
	}
	return 1;
}

COMMAND:kick(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	
	new targetid, reason[130];
	
	if( sscanf(params, "d s[130]", targetid, reason) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /kick [id gracza] [powód]"); 
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	AddPlayerPenalty(targetid, PENALTY_TYPE_KICK, playerid, 0, reason);
	
	return 1;
}

COMMAND:warn(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	
	new targetid, reason[130];
	
	if( sscanf(params, "d s[130]", targetid, reason) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /warn [id gracza] [powód]"); 
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	AddPlayerPenalty(targetid, PENALTY_TYPE_WARN, playerid, 0, reason);
	
	return 1;
}

COMMAND:block(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 3)) return 1;

	new type[20], targetid, time, timestr[15], timetype[5], reason[100];
	if( sscanf(params, "s[20]ds[15] s[100]", type, targetid, timestr, reason) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block [char/veh/ooc/run] [id gracza] [czas[d/h/m](0d/0h/0m - dozywocie) [powód]");

	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	if( strlen(timestr) < 2 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block [char/veh/ooc/run] [id gracza] [czas[d/h/m](0d/0h/0m - dozywocie) [powód]");

	strmid(timetype, timestr, strlen(timestr)-1, strlen(timestr));
	strmid(timestr, timestr, 0, strlen(timestr)-1);
	time = strval(timestr);

	if( time < 0 || (strcmp(timetype, "d") != 0 && strcmp(timetype, "h") != 0 && strcmp(timetype, "m") != 0) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Czas kary nie mo¿e byæ mniejszy ni¿ 0.\nFormat czasu:\n\t10d - kara na 10 dni\n\t20h - kara na 20 godzin\n\t30m - kara na 30 minut");
	
	new typee;
	if( !strcmp(type, "char") ) typee = BLOCK_CHAR;
	else if( !strcmp(type, "veh") ) typee = BLOCK_VEHICLES;
	else if( !strcmp(type, "ooc") ) typee = BLOCK_OOC;
	else if( !strcmp(type, "run") ) typee = BLOCK_RUN;
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /block [char/veh/ooc/run] [id gracza] [czas] [powód]");

	if( time > 0 )
	{
		if( !strcmp(timetype, "d") ) time = gettime() + (60*60*24*time);
		if( !strcmp(timetype, "h") ) time = gettime() + (60*60*time);
		if( !strcmp(timetype, "m") ) time = gettime() + (60*time);
	}

	AddPlayerPenalty(targetid, PENALTY_TYPE_BLOCK, playerid, time, reason, typee);
	
	return 1;
}

COMMAND:unblock(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 4)) return 1;

	new type[20], targetid;
	if( sscanf(params, "s[20]d", type, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /unblock [veh/ooc/run] [id gracza]");

	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");

	new typee;

	if( !strcmp(type, "veh") ) typee = BLOCK_VEHICLES;
	else if( !strcmp(type, "ooc") ) typee = BLOCK_OOC;
	else if( !strcmp(type, "run") ) typee = BLOCK_RUN;
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /unblock [veh/ooc/run] [id gracza]");

	if( !PlayerHasBlock(targetid, typee) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ten gracz nie ma aktywnej blokady tego typu.");

	mysql_pquery(g_sql, sprintf("UPDATE crp_penalties SET penalty_deactivate = 1 WHERE penalty_type = %d AND penalty_extraid = %d AND penalty_owner = %d", PENALTY_TYPE_BLOCK, typee, pInfo[targetid][player_id]));
	pInfo[targetid][player_block] -= typee;
	setPlayerBlockTime(targetid, typee, 0);

	new typestr[40];
	getBlockTypeName(typee, typestr);

	SendGuiInformation(playerid, "Informacja", sprintf("Zdj¹³eœ %s graczowi %s (ID:%d;UID:%d).", typestr, pInfo[targetid][player_name], targetid, pInfo[targetid][player_id]));
	SendGuiInformation(targetid, "Informacja", sprintf("Twoja %s zosta³a zdjêta przez administratora %s.", typestr, pInfo[playerid][player_name]));
	return 1;
}

COMMAND:ban(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 4)) return 1;
	
	new targetid, reason[130], time;
	
	if( sscanf(params, "dd s[130]", targetid, time, reason) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ban [id gracza] [czas w dniach] [powód]"); 
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( time < 1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Minimalny czas trwania bana to 1 dzieñ.");
	
	AddPlayerPenalty(targetid, PENALTY_TYPE_BAN, playerid, (gettime() + (time * 24 * 60 * 60)), reason, time);
	
	return 1;
}

COMMAND:slap(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	
	new targetid;
	if( sscanf(params, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /slap [id gracza]"); 
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	PlayerPlaySound(targetid, 1190, 0.0, 0.0, 0.0);
	
	new Float:p_pos[3];
	GetPlayerPos(targetid, p_pos[0], p_pos[1], p_pos[2]);
	SetPlayerPos(targetid, p_pos[0], p_pos[1], p_pos[2]+5.0);
	
	GameTextForPlayer(targetid, "~r~SLAP", 3000, 3);

	AdminLog(sprintf("Slapped player %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");
	if( targetid != playerid ) AdminLog(sprintf("Have been slapped by player %s", PlayerLogLink(pInfo[playerid][player_id])), pInfo[targetid][player_id], "cmd");
	return 1;
}

COMMAND:aj(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;
	
	new targetid, reason[130], time;
	
	if( sscanf(params, "dd s[130]", targetid, time, reason) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aj [id gracza] [czas w minutach] [powód]"); 
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( time < 0 && time > 360) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aj [id gracza] [czas w minutach] [powód]"); 
	
	AddPlayerPenalty(targetid, PENALTY_TYPE_AJ, playerid, 0, reason, time);
	return 1;
}

COMMAND:unaj(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 4)) return 1;
	
	new targetid;
	
	if( sscanf(params, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /unaj [id gracza]"); 
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( pInfo[targetid][player_aj] == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest w admin jailu.");
	
	PlayerTextDrawHide(targetid, pInfo[targetid][leftTime]);
	
	AdminLog(sprintf("Unjailed player %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "cmd");
	if( targetid != playerid ) AdminLog(sprintf("Have been unjailed by player %s", PlayerLogLink(pInfo[playerid][player_id])), pInfo[targetid][player_id], "cmd");

	SendGuiInformation(targetid, "Informacja", sprintf("Zosta³eœ zwolniony z admin jaila przez %s.", pInfo[playerid][player_name]));
	if( targetid != playerid ) SendGuiInformation(playerid, "Informacja", sprintf("Zwolni³eœ %s z admin jaila.", pInfo[targetid][player_name]));
	
	pInfo[targetid][player_aj] = 0;
	pInfo[targetid][player_aj_end_time] = 0;
	
	mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_aj` = 0 WHERE `char_uid` = %d", pInfo[targetid][player_id]));
					
	PlayerTextDrawHide(targetid, pInfo[targetid][leftTime]);
	
	scrp_SpawnPlayer(targetid, true);
	
	return 1;
}

COMMAND:apaczka(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /apaczka [dostarcz] (...)");
	
	if( !strcmp(sub_cmd, "dostarcz") )
	{
		new package_uid;
		if( sscanf(param, "d", package_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /apaczka dostarcz [uid paczki]");

		new d_uid = GetPackageDoorByUid(package_uid);		
		if( d_uid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Paczka o podanym uid nie istnieje.");
		
		mysql_pquery(g_sql, sprintf("DELETE FROM `crp_packages` WHERE `package_uid` = %d", package_uid));
		mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_ownertype` = %d AND `item_owner` = %d", ITEM_OWNER_TYPE_DOOR_WAREHOUSE, d_uid, ITEM_OWNER_TYPE_PACKAGE, package_uid));
		
		foreach(new i : Items)
		{
			if( Item[i][item_owner_type] == ITEM_OWNER_TYPE_PACKAGE && Item[i][item_owner] == package_uid )
			{
				Item[i][item_owner_type] = ITEM_OWNER_TYPE_DOOR_WAREHOUSE;
				Item[i][item_owner] = d_uid;
			}
		}

		SendGuiInformation(playerid, "Informacja", sprintf("Paczka (UID: %d) zosta³a pomyœlnie dostarczona do drzwi %s (UID: %d).", package_uid, Door[GetDoorByUid(d_uid)][door_name], d_uid));	
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /apaczka [dostarcz] (...)");
	return 1;
}

COMMAND:fly(playerid, params[])
{
	if(GetPlayerPermissions(playerid, 1)) return 1;

	if( pInfo[playerid][player_admin_fly] )
	{
		pInfo[playerid][player_admin_fly] = false;
		StopFly(playerid);
		SetPlayerHealth(playerid, 100);
	}
	else
	{
		if( !pInfo[playerid][player_admin_duty] ) return SendGuiInformation(playerid, "Informacja", "Musisz wejœæ na s³u¿bê admina, aby u¿ywaæ tej komendy.");

		pInfo[playerid][player_admin_fly] = true;
		StartFly(playerid);
		SetPlayerHealth(playerid, 9999999);
	}

	return 1;
}

COMMAND:gdo(playerid, params[])
{

	return 1;
}