COMMAND:a(playerid, params[])
{
	return cmd_admins(playerid, params);
}

COMMAND:admins(playerid, params[])
{
	new str[1024];

	format(str, sizeof(str), "Nazwa\tRanga\tStatus\n");

	new Cache:result;
	result = mysql_query(g_sql, "SELECT * FROM crp_characters WHERE char_level > 0 ORDER BY char_level DESC");

	if(cache_get_rows() != 0)
	{
		for(new i=0;i<cache_get_rows();i++)
		{
			new name[MAX_PLAYER_NAME];
			cache_get(i, "char_name", name);

			new pid = GetPlayerByName(name);

			if(pid != -1)
			{
				format(str, sizeof(str), "%s%s\t%s\t{5BC06B}Aktywny {FFFFFF}(ID %d)\n", str, name, EmbededAdminLevels[cache_get_int(i, "char_level")], pid);
			}
			else format(str, sizeof(str), "%s%s\t%s\t{FFFFFF}Nieaktywny (ostatnio %s)\n", str, name, EmbededAdminLevels[cache_get_int(i, "char_level")], DateToString(cache_get_int(i, "char_last_join")));
		}
	}
	else return SendGuiInformation(playerid, "Informacja", "Brak osób spe³niaj¹cych wymogi.");
	
	cache_delete(result);

	ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST_HEADERS, "Lista ekipy", str, "Zamknij", "");
	
	return 1;
}

COMMAND:pomoc(playerid, params[])
{
	pInfo[playerid][player_dialog_tmp1] = 1;
	ShowPlayerDialog(playerid, DIALOG_POMOC, DIALOG_STYLE_LIST, "Pomoc", "01.\tWprowadzenie\n02.\tKomendy g³ówne\n03.\tPojazd\n04.\tPrzedmioty\n05.\tPraca\n06.\tAnimacje", "Wybierz", "Zamknij");
	return 1;
}

COMMAND:cuttree(playerid, params[])
{
	new 
		Float:fpos[3];
	GetPointInAngleOfPlayer(playerid, fpos[0], fpos[1], fpos[2], 2.0, 0.0);
	
	new pretender_id = -1, Float:pretender_distance = 2.0, Float:distance;
	foreach(new oid : Objects)
	{
		// jesli obiekt nie jest drzewem to skip
		if( Object[oid][object_model] != 685 ) continue;
		
		Streamer_GetDistanceToItem(fpos[0], fpos[1], fpos[2], STREAMER_TYPE_OBJECT, oid, distance);
		if( distance <= pretender_distance )
		{
			pretender_id = oid;
			pretender_distance = distance;
		}
	}
	
	if( pretender_id == -1 ) return SendGuiInformation(playerid, "Informacja", "Przed Tob¹ nie znajduje siê ¿adne drzewo.");
	
	DeleteObject(pretender_id);
	
	SendGuiInformation(playerid, "Informacja", "Drzewo zosta³o œciête.");
	
	return 1;
}

COMMAND:anim(playerid, params[])
{
	mysql_pquery(g_sql, "SELECT anim_command, anim_uid FROM crp_anim", "OnAnimCommandLoaded", "i", playerid);
	return 1;
}

function OnAnimCommandLoaded(playerid)
{
	new str[2000];
	
	DynamicGui_Init(playerid);
	
	for(new i;i<cache_get_rows();i++)
	{
		new tmp[30];
		cache_get(i, "anim_command", tmp);
		
		format(str, sizeof(str), "%s%s\n", str, tmp);
		
		DynamicGui_AddRow(playerid, cache_get_int(i, "anim_uid"));
	}
	
	ShowPlayerDialog(playerid, DIALOG_ANIMS_LIST, DIALOG_STYLE_LIST, "Animacje", str, "Zamknij", "");
	
	return 1;
}

COMMAND:sprobuj(playerid, params[])
{
	new action[120];
	if( sscanf(params, "s[120]", action) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /sprobuj [akcja]");
	
	new success = random(2);

	PlayerLog(sprintf("[rp:try] %s {SUCCESS:%d}", action, success), pInfo[playerid][player_id], "cmd");

	switch(success)
	{
		case 0: ProxMessage(playerid, sprintf("odniós³ sukces próbuj¹c %s.", action), PROX_TRY);
		case 1: ProxMessage(playerid, sprintf("zawiód³ próbuj¹c %s.", action), PROX_TRY);
	}
	
	return 1;
}

COMMAND:pokaz(playerid, params[])
{
	new sub_cmd[40];
	if( sscanf(params, "s[40]", sub_cmd) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pokaz [dowod/prawko/metryczke]");
	
	if( !strcmp(sub_cmd, "dowod") )
	{
		if( !PlayerHasDocument(playerid, DOCUMENT_ID) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz tego dokumentu.");
		
		pInfo[playerid][player_dialog_tmp1] = DOCUMENT_ID;
	}
	else if( !strcmp(sub_cmd, "prawko") )
	{
		if( !PlayerHasDocument(playerid, DOCUMENT_DRIVE) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz tego dokumentu.");
		
		pInfo[playerid][player_dialog_tmp1] = DOCUMENT_DRIVE;
	}
	else if( !strcmp(sub_cmd, "metryczke") )
	{
		if( !PlayerHasDocument(playerid, DOCUMENT_METRYCZKA) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz tego dokumentu.");
		
		pInfo[playerid][player_dialog_tmp1] = DOCUMENT_METRYCZKA;
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /pokaz [dowod/prawko/metryczke]");
	
	DynamicGui_Init(playerid);
	new string[2048], count;
	
	new Float:p_pos[3];
	GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
	
	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		if( pInfo[p][player_admin_spec] ) continue;
		if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 10.0 && GetPlayerVirtualWorld(playerid) == GetPlayerVirtualWorld(p) && GetPlayerInterior(playerid) == GetPlayerInterior(p) )
		{
			if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASK) > -1 ) format(string, sizeof(string), "%s##\t\t%s\n", string, pInfo[p][player_name]);
			else format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);
			
			DynamicGui_AddRow(playerid, p);
			count++;
		}
	}
	
	if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W pobli¿u nie ma ¿adnych osób.");
	else ShowPlayerDialog(playerid, DIALOG_POKAZ_DOKUMENT, DIALOG_STYLE_LIST, "Osoby znajduj¹ce siê w pobli¿u:", string, "Wyœlij", "Zamknij");
	
	return 1;
}

COMMAND:g(playerid, params[])
{
	return cmd_grupy(playerid, params);
}

COMMAND:grupy(playerid, params[])
{
	if( !IsPlayerInAnyGroup(playerid) ) return SendGuiInformation(playerid, "Informacja", "Nie mo¿esz wyœwietliæ listy grupy, poniewa¿ w ¿adnej nie jesteœ zatrudniony.");
	
	new slot, sub_cmd[20], param[60];
	if( sscanf(params, "dS()[20] S()[60]", slot, sub_cmd, param) )
	{
		ShowGroupsList(playerid);
		SendClientMessage(playerid, COLOR_GREY, "Tip: /g {slot (1-5)} [info, online, duty, przebierz, zapros, wypros, wplac, wyplac, pojazdy, respawn, magazyn, czat]");
		return 1;
	}
	else
	{
		slot -= 1;
		if( slot < 0 || slot > 4 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /g {slot (1-5)} [info, online, duty, przebierz, zapros, wypros, wplac, wyplac, pojazdy, respawn, magazyn, czat]");
		if( pGroup[playerid][slot][pg_id] == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /g {slot (1-5)} [info, online, duty, przebierz, zapros, wypros, wplac, wyplac, pojazdy, respawn, magazyn, czat]");
		if( isnull(sub_cmd) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /g %d [info, online, duty, przebierz, zapros, wypros, wplac, wyplac, pojazdy, respawn, magazyn, czat]", slot+1));
		
		new gid = pGroup[playerid][slot][pg_id];
		
		if( !strcmp(sub_cmd, "info") )
		{
			new str[300];
			
			format(str, sizeof(str), "%sTyp grupy:\t\t%s\n", str, GroupTypes[Group[gid][group_type]]);
			format(str, sizeof(str), "%sBud¿et:\t\t\t$%d\n", str, Group[gid][group_bank_money]);
			format(str, sizeof(str), "%sPunkty aktywnoœci:\t%s%d\n", str, ((Group[gid][group_activity_points] >= 500) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)), Group[gid][group_activity_points]);
			format(str, sizeof(str), "%sDotacja:\t\t$%d\n", str, GetGroupDotation(gid));
			if( Group[gid][group_parent_uid] > 0 ) format(str, sizeof(str), "%sNadrzêdna:\t\t%s (UID: %d)\n", str, Group[GetGroupByUid(Group[gid][group_parent_uid])][group_name], Group[gid][group_parent_uid]);
			new dslot = GetPlayerDutySlot(playerid);
			if( dslot > -1 && pGroup[playerid][dslot][pg_id] == gid ) 
			{
				format(str, sizeof(str), "%sCzas s³u¿by:\t\t%dm %ds\n", str, floatround(pGroup[playerid][dslot][pg_duty_time]/60, floatround_floor), pGroup[playerid][dslot][pg_duty_time]%60);
			}
			format(str, sizeof(str), "%sTag:\t\t\t%s\n", str, GetGroupTag(gid));
				
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) » Informacje", Group[gid][group_name], Group[gid][group_uid]), str, "OK", "");
		}
		else if( !strcmp(sub_cmd, "pojazdy") || !strcmp(sub_cmd, "v") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_VEHICLES) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			DynamicGui_Init(playerid);

			new count, str[600];
			foreach(new vid : Vehicles)
			{
				if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP && Vehicle[vid][vehicle_owner] == Group[gid][group_uid] )
				{
					count++;
					format(str, sizeof(str), "%s%d\t\t%s\n", str, Vehicle[vid][vehicle_uid], VehicleNames[GetVehicleModel(vid)-400]);
					DynamicGui_AddRow(playerid, vid);
				}
			}
			
			if( count == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ta grupa nie posiada ¿adnych pojazdów.");
			else ShowPlayerDialog(playerid, DIALOG_GROUP_VEHICLES, DIALOG_STYLE_LIST, sprintf("Pojazdy grupy %s [UID: %d]:", Group[gid][group_name], Group[gid][group_uid]), str, "Namierz", "Zamknij");
		}
		else if( !strcmp(sub_cmd, "czlonkowie") || !strcmp(sub_cmd, "members"))
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);

			pInfo[playerid][player_dialog_tmp4] = Group[gid][group_uid];
			mysql_tquery(g_sql, sprintf("SELECT c.char_name AS name, c.char_uid AS id, cg.group_perm AS perms FROM crp_char_groups cg LEFT JOIN crp_characters c ON c.char_uid = cg.char_uid WHERE cg.group_belongs = %d", Group[gid][group_uid], Group[gid][group_uid]), "OnGroupsMembersListLoaded", "i", playerid);
		}
		else if( !strcmp(sub_cmd, "respawn") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			new count;
			for(new vid;vid<MAX_VEHICLES;vid++)
			{
				if( Vehicle[vid][vehicle_uid] <= 0 ) continue;
				if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP && Vehicle[vid][vehicle_owner] == Group[gid][group_uid] )
				{
					if( Vehicle[vid][vehicle_occupants] > 0 ) continue;
					
					DeleteVehicle(vid);	
					count++;
				}
			}
			
			LoadVehicle(sprintf("WHERE `vehicle_ownertype` = %d AND `vehicle_owner` = %d", VEHICLE_OWNER_TYPE_GROUP, Group[gid][group_uid]));
			
			SendGuiInformation(playerid, "Informacja", sprintf("Na miejsce spawnu powróci³o %d pojazdów nale¿¹cych do grupy %s [UID: %d].", count, Group[gid][group_name], Group[gid][group_uid]));
		}
		else if( !strcmp(sub_cmd, "duty") )
		{
			new dslot = GetPlayerDutySlot(playerid);
			if( dslot != -1 && pGroup[playerid][dslot][pg_id] != gid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", sprintf("Jesteœ ju¿ na s³u¿bie w innej grupie. Aby zejœæ ze s³u¿by wpisz /g %d duty.", dslot+1));
			if( pGroup[playerid][slot][pg_duty] )
			{
				pGroup[playerid][slot][pg_duty] = false;

				GameTextForPlayer(playerid, sprintf("~w~Zakonczyles sluzbe~n~~b~~h~%s", GetGroupTag(gid)), 3000, 4);

				mysql_query(g_sql, sprintf("INSERT INTO `crp_sessions` (session_uid, session_type, session_extraid, session_owner, session_start, session_end) VALUES (null, %d, %d, %d, %d, %d)", SESSION_TYPE_DUTY, Group[gid][group_uid], pInfo[playerid][player_id], pGroup[playerid][slot][pg_duty_start], (pGroup[playerid][slot][pg_duty_start] + pGroup[playerid][slot][pg_duty_time])), false);

				new Cache:result;
				result = mysql_query(g_sql, sprintf("SELECT sum(session_end - session_start) AS session_time FROM crp_sessions WHERE session_type = 2 AND session_owner = %d AND session_extraid = %d AND from_unixtime(session_start, '%%Y-%%m-%%d') = CURDATE() GROUP BY session_type", pInfo[playerid][player_id], Group[gid][group_uid]));
				
				new duty_time = cache_get_int(0, "session_time");
				
				cache_delete(result);
				
				if( duty_time >= 2 * 60 )
				{
					//nabi³ 30 minut i dostaje wyp³ate
					GivePayday(playerid, dslot);
				}

				PlayerLog(sprintf("Ends duty in group %s {TIME:%ds}", GroupLogLink(Group[gid][group_uid]), duty_time), pInfo[playerid][player_id], "group");
				
				UpdatePlayerLabel(playerid);
				
				PlayerTextDrawHide(playerid, pInfo[playerid][GroupDutyTag]);

				// firedep
				FireDepDutyOff_hook(playerid, slot);
			}
			else
			{
				new gcolor = GetGroupColor(gid);

				GameTextForPlayer(playerid, sprintf("~w~Wchodzisz na sluzbe~n~~b~~h~%s", GetGroupTag(gid)), 3000, 4);

				PlayerLog(sprintf("Starts duty in group %s", GroupLogLink(Group[gid][group_uid])), pInfo[playerid][player_id], "group");

				pGroup[playerid][slot][pg_duty] = true;
				pGroup[playerid][slot][pg_duty_time] = 0;
				pGroup[playerid][slot][pg_duty_start] = gettime();
				
				PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][GroupDutyTag], gcolor);
				PlayerTextDrawSetString(playerid, pInfo[playerid][GroupDutyTag], GetGroupTag(gid));
				PlayerTextDrawShow(playerid, pInfo[playerid][GroupDutyTag]);
				
				UpdatePlayerLabel(playerid);
				
				ShowGroupDutyPane(playerid, slot);

				// firedep
				FireDepDutyOn_hook(playerid, slot);
			}
		}
		else if( !strcmp(sub_cmd, "magazyn") )
		{
			new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w budynku nale¿¹cym do tej grupy.");
			if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP || Door[d_id][door_owner] != Group[gid][group_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w budynku nale¿¹cym do tej grupy.");
			
			DynamicGui_Init(playerid);
			DynamicGui_SetDialogValue(playerid, d_id);
			
			new count, str[400];
			foreach(new i : Items)
			{
				if( Item[i][item_owner_type] == ITEM_OWNER_TYPE_DOOR_WAREHOUSE && Item[i][item_owner] == Door[d_id][door_uid] )
				{
					format(str, sizeof(str), "%s[UID: %d]\t%s\t[%d szt] [$%d]\n", str, Item[i][item_uid], Item[i][item_name], Item[i][item_amount], Item[i][item_price]);
					DynamicGui_AddRow(playerid, i);
					count++;
				}
			}
			
			if( !count ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym sklepie nie ma ¿adnych produktów do kupienia.");
			
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, "Przedmioty dostêpne w magazynie", str, "OK", "");
			else ShowPlayerDialog(playerid, DIALOG_DOOR_STORAGE, DIALOG_STYLE_LIST, "Przedmioty dostêpne w magazynie", str, "Wyjmij", "Zamknij");
			
		}
		else if( !strcmp(sub_cmd, "online") )
		{
			new str[600], pslot;
			foreach(new p : Player)
			{
				pslot = GetPlayerGroupSlot(p, gid);
				if( pslot > -1 )
				{
					if( pGroup[p][pslot][pg_duty] ) format(str, sizeof(str), "%s"HEX_COLOR_GREEN"%d\t\t%s\n", str, p, pInfo[p][player_name]);
					else format(str, sizeof(str), "%s%d\t\t%s\n", str, p, pInfo[p][player_name]);
				}
			}
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) » Cz³onkowie online", Group[gid][group_name], Group[gid][group_uid]), str, "OK", "");
		}
		else if( !strcmp(sub_cmd, "zapros") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			new targetid;
			if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)rupa %d zapros [id gracza]", slot+1));
			if( targetid == playerid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz zaprosiæ samego siebie.");

			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
			if( GetPlayerGroupSlot(targetid, gid) > -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest ju¿ w tej grupie.");
			
			new free_slot = GetPlayerGroupFreeSlot(targetid);
			if( free_slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie ma ju¿ wolnego slotu grupy.");
			
			mysql_pquery(g_sql, sprintf("INSERT INTO `crp_char_groups` (char_uid, group_belongs, group_perm) VALUES (%d, %d, %d)", pInfo[targetid][player_id], Group[gid][group_uid], 0));
				
			pGroup[targetid][free_slot][pg_id] = gid;
			pGroup[targetid][free_slot][pg_rank_perm] = 0;
			SendGuiInformation(playerid, "Informacja", sprintf("Zaprosi³eœ gracza %s do grupy %s (UID: %d).", pInfo[targetid][player_name], Group[gid][group_name], Group[gid][group_uid]));
			SendGuiInformation(targetid, "Informacja", sprintf("%s zaprosi³ Ciê do grupy %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}
		else if( !strcmp(sub_cmd, "wypros") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			new targetid;
			if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)rupa %d wypros [id gracza]", slot+1));
			if( targetid == playerid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz wyprosiæ samego siebie.");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
			
			new pslot = GetPlayerGroupSlot(targetid, gid);
			if( pslot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest w tej grupie.");
			
			mysql_pquery(g_sql, sprintf("DELETE FROM `crp_char_groups` WHERE `char_uid` = %d AND `group_belongs` = %d", pInfo[targetid][player_id], Group[gid][group_uid]));
			
			if( pGroup[targetid][pslot][pg_duty] ) PlayerTextDrawHide(targetid, pInfo[targetid][GroupDutyTag]);
			
			CleanPlayerGroup(targetid, pslot);
			
			SendGuiInformation(playerid, "Informacja", sprintf("Wyprosi³eœ gracza %s z grupy %s (UID: %d).", pInfo[targetid][player_name], Group[gid][group_name], Group[gid][group_uid]));
			SendGuiInformation(targetid, "Informacja", sprintf("%s wyprosi³ Ciê z grupy %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}
		else if( !strcmp(sub_cmd, "wplac") )
		{
			new amount;
			if( sscanf(param, "d", amount) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)rupa %d wplac [kwota]", slot+1));
			if( GetPlayerOnlineTime(playerid) < 18000 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz wp³acaæ pieniêdzy na konto grupy poniewa¿ masz przegrane mniej ni¿ 5h.");
			if( amount < 0 ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)rupa %d wplac [kwota]", slot+1));
			if( pInfo[playerid][player_money] < amount ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz takiej iloœci pieniêdzy przy sobie.");
			
			GivePlayerMoney(playerid, -amount);
			GiveGroupMoney(gid, amount);
			
			SendGuiInformation(playerid, "Informacja", sprintf("Wp³aci³eœ $%d na konto grupy %s (UID: %d).", amount, Group[gid][group_name], Group[gid][group_uid]));
		}
		else if( !strcmp(sub_cmd, "wyplac") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			new amount;
			if( sscanf(param, "d", amount) ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)rupa %d wyplac [kwota]", slot+1));
			if( amount < 0 ) return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /(g)rupa %d wyplac [kwota]", slot+1));
			if( Group[gid][group_bank_money] < amount ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ta grupa nie ma takiej iloœci pieniêdzy.");
			
			GivePlayerMoney(playerid, amount);
			GiveGroupMoney(gid, -amount);
			
			SendGuiInformation(playerid, "Informacja", sprintf("Wyp³aci³eœ $%d z konta grupy %s (UID: %d).", amount, Group[gid][group_name], Group[gid][group_uid]));
		}
		else if( !strcmp(sub_cmd, "przebierz") )
		{
			if( pInfo[playerid][player_last_skin] == pGroup[playerid][slot][pg_rank_skin] )
			{
				pInfo[playerid][player_last_skin] = pInfo[playerid][player_skin];
				SetPlayerSkin(playerid, pInfo[playerid][player_last_skin]);
			}
			else
			{
				if( pGroup[playerid][slot][pg_rank_skin] <= 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz ustalonego skinu grupowego.");
				
				new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_CLOTH);
				if( itemid > -1 )
				{
					Item[itemid][item_used] = false;
					mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_used = 0 WHERE item_uid = %d", Item[itemid][item_uid]));
				}
				
				pInfo[playerid][player_last_skin] = pGroup[playerid][slot][pg_rank_skin];
				SetPlayerSkin(playerid, pInfo[playerid][player_last_skin]);
			}
		}
		else if( !strcmp(sub_cmd, "czat") )
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
			
			Group[gid][group_ooc] = !Group[gid][group_ooc];
			
			mysql_pquery(g_sql, sprintf("UPDATE crp_groups SET group_ooc = %d WHERE group_uid = %d", Group[gid][group_ooc], Group[gid][group_uid]));
			
			if( Group[gid][group_ooc] ) SendGuiInformation(playerid, "Informacja", "Odblokowa³eœ mo¿liwoœæ pisania na grupowym czacie ooc.");
			else SendGuiInformation(playerid, "Informacja", "Zablokowa³eœ mo¿liwoœæ pisania na grupowym czacie ooc.");
		}
		else return SendClientMessage(playerid, COLOR_GREY, sprintf("Tip: /g %d [info, online, duty, przebierz, zapros, wypros, wplac, wyplac, pojazdy, respawn, magazyn, czat]", slot+1));
	}
	
	
	return 1;
}

function OnGroupsMembersListLoaded(playerid)
{
	new rows = cache_get_rows(), str[2048];
	format(str, sizeof(str), "UID\tNazwa\tPermisje\n");

	if(rows > 0)
	{
		DynamicGui_Init(playerid);

		for(new i = 0; i < rows; i++)
		{
			new uid = cache_get_int(i, "id");
			new perms = cache_get_int(i, "perms");
			new name[MAX_PLAYER_NAME+8];
			cache_get(i, "name", name);

			UnderscoreToSpace(name);
			strins(name, HEX_COLOR_SAMP, 0, sizeof(name));

			new perm_string[32];
			format(perm_string, sizeof(perm_string), "_______");

			if( perms & WORKER_FLAG_LEADER   == WORKER_FLAG_LEADER   ) 	perm_string[0] = 'A';
			if( perms & WORKER_FLAG_GATES    == WORKER_FLAG_GATES    ) 	perm_string[1] = 'B';
			if( perms & WORKER_FLAG_CHAT     == WORKER_FLAG_CHAT     ) 	perm_string[2] = 'C';
			if( perms & WORKER_FLAG_VEHICLES == WORKER_FLAG_VEHICLES )  perm_string[3] = 'D';
			if( perms & WORKER_FLAG_OFFER    == WORKER_FLAG_OFFER    ) 	perm_string[4] = 'E';
			if( perms & WORKER_FLAG_DOORS    == WORKER_FLAG_DOORS    ) 	perm_string[5] = 'F';
			if( perms & WORKER_FLAG_ORDER    == WORKER_FLAG_ORDER    )	perm_string[6] = 'G';

			strins(perm_string, HEX_COLOR_WHITE, 0, sizeof(perm_string));

			format(str, sizeof(str), "%s%d\t%s\t%s\n", str, uid, name, perm_string);
			DynamicGui_AddRow(playerid, uid, perms);
		}

		ShowPlayerDialog(playerid, DIALOG_GROUP_MEMBERS, DIALOG_STYLE_TABLIST_HEADERS, "Cz³onkowie grupy", str, "Zmieñ", "Zamknij");
		InfoboxCenter(playerid, 60, "~p~~h~Uprawnienia~n~~y~A ~w~- zarzadzanie grupa ~y~B ~w~- zarzadzanie bramami ~y~C ~w~- pisanie na chacie ~y~D ~w~- prowadzenie pojazdow~n~~y~E ~w~- skladanie ofert ~y~F ~w~- zamykanie drzwi ~y~G ~w~- zamawianie produktow");
	}
	else
	{
		SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Wyst¹pi³ b³¹d - zg³oœ to administracji. [#1121]");
	}
}

// ---- Obiekty ---- //
COMMAND:mc(playerid, params[])
{	
	new owner = -1, owner_type = -1;
	GetObjectDataForPlayer(playerid, owner, owner_type);
	
	if( owner_type == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do tworzenia obiektu w tym miejscu.");
	
	if( owner_type == OBJECT_OWNER_TYPE_DOOR )
	{
		new d_id = GetDoorByUid(owner);
		if( CountDoorObjects(d_id) >= Door[d_id][door_objects_limit] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Te drzwi osi¹gne³y swój limit obiektów.");
	}
	
	if( owner_type == OBJECT_OWNER_TYPE_AREA )
	{
		new a_id = GetAreaByUid(owner);
		if( CountAreaObjects(a_id) >= Area[a_id][area_objects_limit] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ta strefa osi¹gne³a swój limit obiektów.");
	}
	
	new model;
	if( sscanf(params, "d", model) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mc [model]");
	
	new Float:o_pos[3];
	GetPointInAngleOfPlayer(playerid, o_pos[0], o_pos[1], o_pos[2], 4.0, 0.0);

	new Cache:result;
	result = mysql_query(g_sql, sprintf("INSERT INTO `crp_objects` (object_uid, object_type, object_ownertype, object_owner, object_map, object_model, object_world, object_posx, object_posy, object_posz) VALUES (null, %d, %d, %d, 0, %d, %d, %f, %f, %f)", OBJECT_TYPE_NORMAL, owner_type, owner, model, GetPlayerVirtualWorld(playerid), o_pos[0], o_pos[1], o_pos[2]));

	new uid = cache_insert_id(), oid = LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);

	cache_delete(result);
	
	Streamer_UpdateEx(playerid, o_pos[0], o_pos[1], o_pos[2]);
	
	switch( owner_type )
	{
		case OBJECT_OWNER_TYPE_GLOBAL:
		{
			SendPlayerInformation(playerid, sprintf("Stworzyles ~y~obiekt globalny~w~ (UID: %d, ID: %d, MODEL: %d).", uid, oid, model), 4000);
		}
		
		case OBJECT_OWNER_TYPE_DOOR:
		{
			SendPlayerInformation(playerid, sprintf("Stworzyles ~y~obiekt drzwi~w~ (UID: %d, ID: %d, MODEL: %d).", uid, oid, model), 4000);
		}
		
		case OBJECT_OWNER_TYPE_AREA:
		{
			SendPlayerInformation(playerid, sprintf("Stworzyles ~y~obiekt strefowy~w~ (UID: %d, ID: %d, MODEL: %d).", uid, oid, model), 4000);
		}
	}
	
	return 1;
}

COMMAND:getvw(playerid, params[])
{	
	SendGuiInformation(playerid, "Informacja", sprintf("Twój virtualworld to %d.", GetPlayerVirtualWorld(playerid)));
	
	return 1;
}

COMMAND:mgate(playerid, params[])
{
	new sub_cmd[40];
	sscanf(params, "s[40]", sub_cmd);
	
	if( !isnull(sub_cmd) && !strcmp(sub_cmd, "pomoc") )
	{
		SendGuiInformation(playerid, "Pomoc » Tworzenie bramy", "Aby utworzyæ bramê wystarczy wykonaæ poni¿sze kroki:\n\t1. Tworzysz obiekt bramy i ustawiasz go w pozycji zamkniêtej zapisuj¹c go.\n\t2. W³¹czasz edycjê obiektu, ustawiasz go w pozycji otwartej bramy\n\t    po czym wpisujesz /mgate (nie zapisuj obiektu).");
	}
	else
	{
		if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
		
		new o_id = pInfo[playerid][player_edited_object];
		new str[400];
		strcat(str, sprintf("UPDATE `crp_objects` SET `object_type` = %d, `object_gatex` = %f, `object_gatey` = %f, `object_gatez` = %f, ", OBJECT_TYPE_GATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]));
		strcat(str, sprintf("`object_gaterotx` = %f, `object_gateroty` = %f, `object_gaterotz` = %f WHERE `object_uid` = %d", Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5], Object[pInfo[playerid][player_edited_object]][object_uid]));
		mysql_query(g_sql, str, false);
		
		pInfo[playerid][player_edited_object_no_action] = true;
		CancelEdit(playerid);
		
		new uid = Object[o_id][object_uid];
		
		pInfo[playerid][player_edited_object] = -1;
		Object[o_id][object_is_edited] = false;
		
		PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
		
		DeleteObject(o_id, false);
		LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
		
		RefreshPlayer(playerid);
		
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie utworzy³eœ bramê [OUID: %d].", uid));
	}
	return 1;
}

COMMAND:brama(playerid, params[])
{
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	new a_id = -1;
	if( GetPlayerPermissions(playerid, 1) ) 
	{
		if( d_id > -1 )
		{
			if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_PLAYER )
			{
				if( Door[d_id][door_owner] != pInfo[playerid][player_id] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania bram na tym terenie.");
			}
			else if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
			{
				new slot = GetPlayerGroupSlot(playerid, GetGroupByUid(Door[d_id][door_owner]));
				if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania bram na tym terenie.");
				if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_GATES) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania bram na tym terenie.");
			}
		}
		else
		{
			a_id = GetPlayerArea(playerid, AREA_TYPE_NORMAL);
			if( a_id > -1 )
			{
				if( Area[a_id][area_owner_type] == AREA_OWNER_TYPE_PLAYER )
				{
					if( Area[a_id][area_owner] != pInfo[playerid][player_id] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania bram na tym terenie.");
				}
				else if( Area[a_id][area_owner_type] == AREA_OWNER_TYPE_GROUP )
				{
					new slot = GetPlayerGroupSlot(playerid, GetGroupByUid(Area[a_id][area_owner]));
					if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania bram na tym terenie.");
					if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_GATES) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania bram na tym terenie.");
				}
			}
		}
	}
	
	new Float:distance, Float:pretender_distance = 15.0, pretender_id = -1, Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	foreach(new o_id : Objects)
	{
		if( Object[o_id][object_type] != OBJECT_TYPE_GATE ) continue;
		if( a_id > -1 && (Object[o_id][object_owner_type] != OBJECT_OWNER_TYPE_AREA || Object[o_id][object_owner] != Area[a_id][area_uid]) ) continue;
		if( d_id > -1 && (Object[o_id][object_owner_type] != OBJECT_OWNER_TYPE_DOOR || Object[o_id][object_owner] != Door[d_id][door_uid]) ) continue;
		
		Streamer_GetDistanceToItem(x, y, z, STREAMER_TYPE_OBJECT, o_id, distance, 3);
		
		if( distance < pretender_distance )
		{
			pretender_distance = distance;
			pretender_id = o_id;
		}
	}
	
	if( pretender_id == -1 ) return SendGuiInformation(playerid, "Informacja", "W pobli¿y nie znajduje siê ¿adna brama, któr¹ mo¿esz u¿ywaæ.");
	
	if( IsDynamicObjectMoving(pretender_id) ) StopDynamicObject(pretender_id);
	
	if( Object[pretender_id][object_gate_opened] )
	{
		// Zamykamy bramê
		MoveDynamicObject(pretender_id, Object[pretender_id][object_gatec_pos][0], Object[pretender_id][object_gatec_pos][1], Object[pretender_id][object_gatec_pos][2], 2.0, Object[pretender_id][object_gatec_pos][3], Object[pretender_id][object_gatec_pos][4], Object[pretender_id][object_gatec_pos][5]);
		SendPlayerInformation(playerid, "Najblizsza brama zostala ~y~zamknieta", 4000);
	}
	else
	{
		// Otwieramy bramê
		MoveDynamicObject(pretender_id, Object[pretender_id][object_gateo_pos][0], Object[pretender_id][object_gateo_pos][1], Object[pretender_id][object_gateo_pos][2], 2.0, Object[pretender_id][object_gateo_pos][3], Object[pretender_id][object_gateo_pos][4], Object[pretender_id][object_gateo_pos][5]);
		SendPlayerInformation(playerid, "Najblizsza brama zostala ~y~otwarta", 4000);
	}
	
	Object[pretender_id][object_gate_opened] = !Object[pretender_id][object_gate_opened];
	
	return 1;
}

COMMAND:msel(playerid, params[])
{
	new model;
	if( sscanf(params, "d", model) )
	{
		SelectObject(playerid);
	}
	else
	{
		new pretend_id = -1, Float:pretend_distance = 15.0;
		foreach(new oid : Objects)
		{
			if( Object[oid][object_model] != model || !CanPlayerEditObject(playerid, oid) || IsObjectEdited(oid) || Object[oid][object_vw] != GetPlayerVirtualWorld(playerid) ) continue;

			new Float:distance = GetPlayerDistanceToObject(playerid, oid);
			if( distance < pretend_distance )
			{
				pretend_id = oid;
				pretend_distance = distance;
			}
		}
		
		if( pretend_id == -1 ) return SendPlayerInformation(playerid, "W poblizu nie znajduje sie zaden obiekt o tym modelu, ktory moglbys edytowac.", 4000);
		else PlayerObjectEdit(playerid, pretend_id, pInfo[playerid][player_editor]);
	}
	
	return 1;
}

COMMAND:msave(playerid, params[])
{
	new objectid = pInfo[playerid][player_edited_object];
	if(!objectid) return SendPlayerInformation(playerid, "Nie edytujesz ¿adnego obiektu!");
	if(!pInfo[playerid][player_editor]) return SendPlayerInformation(playerid, "Ta komenda dotyczy tylko trybu klawiszowego.");

	Object[pInfo[playerid][player_edited_object]][object_is_edited] = false;
	pInfo[playerid][player_edited_object] = -1;
	pInfo[playerid][player_has_animation] = false;
	ClearAnimations(playerid);

	if( Object[objectid][object_owner_type] == OBJECT_OWNER_TYPE_AREA )
	{
		if( !IsPointInDynamicArea(GetAreaByUid(Object[objectid][object_owner]), Object[objectid][object_pos][0], Object[objectid][object_pos][1], Object[objectid][object_pos][2]) )
		{
			pInfo[playerid][player_edited_object] = -1;
			Object[objectid][object_is_edited] = false;
				
			SetDynamicObjectPos(objectid, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
			SetDynamicObjectRot(objectid, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);
				
			SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Obiekt strefowy nie mo¿e staæ poza stref¹. Powróci³ on na swoje pocz¹tkowe miejsce.");
				
			return 1;
		}
	}

	PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);

	mysql_query(g_sql, sprintf("UPDATE `crp_objects` SET `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", Object[objectid][object_pos][0], Object[objectid][object_pos][1], Object[objectid][object_pos][2], Object[objectid][object_pos][3], Object[objectid][object_pos][4], Object[objectid][object_pos][5], Object[objectid][object_uid]), false);
		
	SendPlayerInformation(playerid, "Obiekt zapisany pomyslnie", 3000);

	return 1;
}

COMMAND:mselid(playerid, params[])
{
	new o_id;
	if( sscanf(params, "d", o_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mselid [id obiektu]");

	if( !Iter_Contains(Objects, o_id) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Obiekt o podanym id nie istnieje.");
	if( GetPlayerDistanceToObject(playerid, o_id) > 40.0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Obiekt o podanym id znajduje siê za daleko Ciebie.");
	
	if( !CanPlayerEditObject(playerid, o_id) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do edytowania obiektu o podanym id.");
	if( IsObjectEdited(o_id) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Obiekt o podanym id jest ju¿ edytowany przez kogoœ innego.");
	
	EditDynamicObject(playerid, o_id);
	
	Object[o_id][object_is_edited] = true;
	pInfo[playerid][player_edited_object] = o_id;

	// Pobieramy sobie poczatkowa pozycje obiektu w razie jakiegos bledu
	GetDynamicObjectPos(o_id, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
	GetDynamicObjectRot(o_id, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);
	
	Object[o_id][object_pos][0] = pInfo[playerid][player_edited_object_pos][0];
	Object[o_id][object_pos][1] = pInfo[playerid][player_edited_object_pos][1];
	Object[o_id][object_pos][2] = pInfo[playerid][player_edited_object_pos][2];
	Object[o_id][object_pos][3] = pInfo[playerid][player_edited_object_pos][3];
	Object[o_id][object_pos][4] = pInfo[playerid][player_edited_object_pos][4];
	Object[o_id][object_pos][5] = pInfo[playerid][player_edited_object_pos][5];
	
	UpdateObjectInfoTextdraw(playerid, o_id);
	PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);
	return 1;
}

COMMAND:md(playerid, params[])
{
	return cmd_mdel(playerid, params);
}

COMMAND:mdel(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
	if( Object[pInfo[playerid][player_edited_object]][object_type] != OBJECT_TYPE_NORMAL && Object[pInfo[playerid][player_edited_object]][object_type] != OBJECT_TYPE_GATE ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz usun¹æ obiektu o tym typie.");
	
	SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie usun¹³eœ obiekt (UID: %d i ID: %d).", Object[pInfo[playerid][player_edited_object]][object_uid], pInfo[playerid][player_edited_object]));
		
	DeleteObject(pInfo[playerid][player_edited_object]);
	CancelEdit(playerid);
	ClearAnimations(playerid);

	pInfo[playerid][player_has_animation] = false;
	pInfo[playerid][player_edited_object] = -1;
	
	PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
	return 1;
}

COMMAND:mtype(playerid, params[])
{
	if( GetPlayerPermissions(playerid, 5) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
	
	new type;
	if( sscanf(params, "d", type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mtype [1 - normalny, 2 - bankomat, 5 - kuchenka do narksów, 7 - ³aw. si³., 8 - ciê¿. si³.]");
	
	SendGuiInformation(playerid, "Informacja", "Pomyœlnie zmieni³eœ typ tego obiektu.");

	new o_id = pInfo[playerid][player_edited_object];
	mysql_query(g_sql, sprintf("UPDATE `crp_objects` SET `object_type` = %d, `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", type, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5], Object[pInfo[playerid][player_edited_object]][object_uid]), false);
	
	pInfo[playerid][player_edited_object_no_action] = true;
	CancelEdit(playerid);
	ClearAnimations(playerid);

	new uid = Object[o_id][object_uid];
	
	pInfo[playerid][player_has_animation] = false;
	pInfo[playerid][player_edited_object] = -1;
	Object[o_id][object_is_edited] = false;
	
	PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
	
	DeleteObject(o_id, false);
	LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
	
	RefreshPlayer(playerid);
	
	return 1;
}

COMMAND:mmat(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
	
	new index, sub_cmd, param[100];
	if( sscanf(params, "dd S()[150]", index, sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat [index] [0 - tekstura, 1 - napis] (parametry)");
	
	switch(sub_cmd)
	{
		// tekstury
		case 0:
		{
			new color, model, txd_name[32], texture_name[32];
			if( sscanf(param, "xds[32]s[32]", color, model, txd_name, texture_name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat [index] 0 [Kolor (ARGB)] [Model] [Txdname] [Texturename]");
			if( index < 0 || index > 100 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat [index] 0 [Kolor (ARGB)] [Model] [Txdname] [Texturename]");
			
			new o_id = pInfo[playerid][player_edited_object];

			new txdname[68], txtname[68];
			mysql_escape_string(texture_name, txtname);
			mysql_escape_string(txd_name, txdname);

			// nowy system
			// ograniczenie 20 mmatów
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT mat_uid FROM crp_objects_mats WHERE mat_object = %d", Object[o_id][object_uid], 0, index));
			
			new rows = cache_get_rows();
			
			cache_delete(result);

			if(rows >= 20) return SendClientMessage(playerid, COLOR_GREY, "Ten obiekt osi¹gna³ limit 20 mmatów.");


			result = mysql_query(g_sql, sprintf("SELECT mat_uid FROM crp_objects_mats WHERE mat_object = %d AND mat_type = %d AND mat_index = %d", Object[o_id][object_uid], 0, index));

			rows = cache_get_rows();
			
			cache_delete(result);

			if( rows > 0) 
			{
				// update starego
				mysql_query(g_sql, sprintf("UPDATE crp_objects_mats SET mat_value = '%06x:%d:%s:%s' WHERE mat_object = %d AND mat_type = %d AND mat_index = %d", color, model, txdname, txtname, Object[o_id][object_uid], 0, index), false);
			}
			else
			{
				// dodajemy nowy
				mysql_query(g_sql, sprintf("INSERT INTO crp_objects_mats VALUES(null, %d, %d, %d, '%06x:%d:%s:%s')", Object[o_id][object_uid], 0, index, color, model, txdname, txtname), false);
			}
			// end
			
			mysql_query(g_sql, sprintf("UPDATE `crp_objects` SET `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5], Object[pInfo[playerid][player_edited_object]][object_uid]), false);
			
			pInfo[playerid][player_edited_object_no_action] = true;
			CancelEdit(playerid);
			
			new uid = Object[o_id][object_uid];
			
			pInfo[playerid][player_edited_object] = -1;
			Object[o_id][object_is_edited] = false;
			
			PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
			
			DeleteObject(o_id, false);
			new oid = LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
			
			Streamer_UpdateEx(playerid, Object[oid][object_pos][0], Object[oid][object_pos][1], Object[oid][object_pos][2]);
			
			cmd_mselid(playerid, sprintf("%d", oid));
		}
		
		// napisy
		case 1:
		{
			new msize, fsize, bold, fcolor, bcolor, align, font[32], text[50];
			if( sscanf(param, "dddxxds[32] s[50]", msize, fsize, bold, fcolor, bcolor, align, font, text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat [index] 1 [Matsize (10-140) [Fontsize (24-255)] [Bold] [Fontcol] [Backcol] [Align (0-2)] [Font] [Txt]");
			if( index < 0 || index > 100 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mmat [index] 1 [Matsize (10-140) [Fontsize (24-255)] [Bold] [Fontcol] [Backcol] [Align (0-2)] [Font] [Txt]");
			if( strfind(text, ":", true) != -1 || strfind(text, "^", true) != -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz u¿ywaæ znaków ^ oraz :.");
			
			new o_id = pInfo[playerid][player_edited_object];
			// nowy system
			// ograniczenie 20 mmatów
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT mat_uid FROM crp_objects_mats WHERE mat_object = %d", Object[o_id][object_uid], 0, index));

			new rows = cache_get_rows();
			
			cache_delete(result);

			if(rows >= 20) return SendClientMessage(playerid, COLOR_GREY, "Ten obiekt osi¹gna³ limit 20 mmatów.");

			result = mysql_query(g_sql, sprintf("SELECT mat_uid FROM crp_objects_mats WHERE mat_object = %d AND mat_type = %d AND mat_index = %d", Object[o_id][object_uid], 0, index));
			
			rows = cache_get_rows();
			
			cache_delete(result);

			if( rows > 0) 
			{
				// update starego
				mysql_query(g_sql, sprintf("UPDATE crp_objects_mats SET mat_value = '%d:%d:%d:%06x:%06x:%d:%s:%s' WHERE mat_object = %d AND mat_type = %d AND mat_index = %d", msize, fsize, bold, fcolor, bcolor, align, font, text, Object[o_id][object_uid], 1, index), false);
			}
			else
			{
				// dodajemy nowy
				mysql_query(g_sql, sprintf("INSERT INTO crp_objects_mats VALUES(null, %d, %d, %d, '%d:%d:%d:%06x:%06x:%d:%s:%s')", Object[o_id][object_uid], 1, index, msize, fsize, bold, fcolor, bcolor, align, font, text), false);
			}
			// end	
			
			mysql_query(g_sql, sprintf("UPDATE `crp_objects` SET `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5], Object[pInfo[playerid][player_edited_object]][object_uid]), false);
			
			pInfo[playerid][player_edited_object_no_action] = true;
			CancelEdit(playerid);
			
			new uid = Object[o_id][object_uid];
			
			pInfo[playerid][player_edited_object] = -1;
			Object[o_id][object_is_edited] = false;
			
			PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
			
			DeleteObject(o_id, false);
			new oid = LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
			
			Streamer_UpdateEx(playerid, Object[oid][object_pos][0], Object[oid][object_pos][1], Object[oid][object_pos][2]);
			
			cmd_mselid(playerid, sprintf("%d", oid));
		}
	}
	
	return 1;
}

COMMAND:mcopy(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
	if( Object[pInfo[playerid][player_edited_object]][object_type] != OBJECT_TYPE_NORMAL ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do edycji tego obiektu.");
	
	new owner = -1, owner_type = -1;
	GetObjectDataForPlayer(playerid, owner, owner_type);
	
	if( owner_type == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do tworzenia obiektu w tym miejscu.");
	
	if( owner_type == OBJECT_OWNER_TYPE_DOOR )
	{
		new d_id = GetDoorByUid(owner);
		if( CountDoorObjects(d_id) >= Door[d_id][door_objects_limit] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Te drzwi osi¹gne³y swój limit obiektów.");
	}
	
	if( owner_type == OBJECT_OWNER_TYPE_AREA )
	{
		new a_id = GetAreaByUid(owner);
		if( CountAreaObjects(a_id) >= Area[a_id][area_objects_limit] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ta strefa osi¹gne³a swój limit obiektów.");
	}

	new objectid = pInfo[playerid][player_edited_object];

	new querystr[800], Cache:result;
	format(querystr, sizeof(querystr), "INSERT INTO `crp_objects` (object_uid, object_type, object_ownertype, object_owner, object_map, object_model, object_world, object_stream, object_posx, object_posy, object_posz, object_rotx, object_roty, object_rotz) VALUES ");
	format(querystr, sizeof(querystr), "%s(null, %d, %d, %d, 0, %d, %d, %f,", querystr, Object[objectid][object_type], Object[objectid][object_owner_type], Object[objectid][object_owner], Object[objectid][object_model], Object[objectid][object_vw], Object[objectid][object_stream_distance]);
	format(querystr, sizeof(querystr), "%s %f, %f, %f, %f, %f, %f)", querystr, Object[objectid][object_pos][0], Object[objectid][object_pos][1], Object[objectid][object_pos][2], Object[objectid][object_pos][3], Object[objectid][object_pos][4], Object[objectid][object_pos][5]);
	result = mysql_query(g_sql, querystr);
	new uid = cache_insert_id();

	cache_delete(result);

	mysql_query(g_sql, sprintf("INSERT INTO crp_objects_mats (mat_object, mat_type, mat_index, mat_value) SELECT %d, mat_type, mat_index, mat_value FROM crp_objects_mats WHERE mat_object = %d", uid, Object[objectid][object_uid]), false);

	OnPlayerEditDynamicObject(playerid, objectid, EDIT_RESPONSE_FINAL, Object[objectid][object_pos][0], Object[objectid][object_pos][1], Object[objectid][object_pos][2], Object[objectid][object_pos][3], Object[objectid][object_pos][4], Object[objectid][object_pos][5]);

	new oid = LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);

	Streamer_UpdateEx(playerid, Object[oid][object_pos][0], Object[oid][object_pos][1], Object[oid][object_pos][2]);

	cmd_mselid(playerid, sprintf("%d", oid));

	SendPlayerInformation(playerid, "Obiekt skopiowany pomyslnie", 3000);
	SendClientMessage(playerid, COLOR_GREY, sprintf("Skopiowa³eœ obiekt (UID: %d, ID: %d, MODEL: %d).", uid, oid, Object[oid][object_model]));
	return 1;
}

COMMAND:rx(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
	
	new Float:rxs;
	if( sscanf(params, "f", rxs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /rx [iloœæ stopni]");
	if( rxs > 180.0 || rxs < -180.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /rx [iloœæ stopni]");
	
	new o_id = pInfo[playerid][player_edited_object];
	
	SetDynamicObjectRot(pInfo[playerid][player_edited_object], Object[o_id][object_pos][3]+rxs, Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
	
	if(!pInfo[playerid][player_editor])
	{
		OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], 360.0-rxs, Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
		pInfo[playerid][player_edited_object_no_action] = true;
	
		CancelEdit(playerid);
		EditDynamicObject(playerid, pInfo[playerid][player_edited_object]);
	}
	else
	{
		Object[o_id][object_pos][3] += rxs;
		UpdateObjectInfoTextdraw(playerid, o_id);				
	}
	
	return 1;
}

COMMAND:ry(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
	
	new Float:rys;
	if( sscanf(params, "f", rys) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ry [iloœæ stopni]");
	if( rys > 180.0 || rys < -180.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ry [iloœæ stopni]");
 	
	new o_id = pInfo[playerid][player_edited_object];
	
	SetDynamicObjectRot(pInfo[playerid][player_edited_object],  Object[o_id][object_pos][3], Object[o_id][object_pos][4]+rys, Object[o_id][object_pos][5]);
	
	if(!pInfo[playerid][player_editor])
	{
		OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], 360.0-rys, Object[o_id][object_pos][5]);
		pInfo[playerid][player_edited_object_no_action] = true;
	
		CancelEdit(playerid);
		EditDynamicObject(playerid, pInfo[playerid][player_edited_object]);
	}
	else
	{
		Object[o_id][object_pos][4] += rys;
		UpdateObjectInfoTextdraw(playerid, o_id);		
	}
	return 1;
}

COMMAND:rz(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
	
	new Float:rzs;
	if( sscanf(params, "f", rzs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /rz [iloœæ stopni]");
	if( rzs > 180.0 || rzs < -180.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /rz [iloœæ stopni]");
 	
	new o_id = pInfo[playerid][player_edited_object];
	
	SetDynamicObjectRot(pInfo[playerid][player_edited_object],  Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]+rzs);
	
	if(!pInfo[playerid][player_editor])
	{
		OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], 360.0-rzs);
		pInfo[playerid][player_edited_object_no_action] = true;
	
		CancelEdit(playerid);
		EditDynamicObject(playerid, pInfo[playerid][player_edited_object]);
	}
	else
	{
		Object[o_id][object_pos][5] += rzs;
		UpdateObjectInfoTextdraw(playerid, o_id);
	}
	return 1;
}

COMMAND:pz(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnego obiektu.");
	
	new Float:pzs;
	if( sscanf(params, "f", pzs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pz [wysokoœæ]");
 	
	new o_id = pInfo[playerid][player_edited_object];
	
	if(!pInfo[playerid][player_editor])
	{
		SetDynamicObjectPos(pInfo[playerid][player_edited_object],  Object[o_id][object_pos][0], Object[o_id][object_pos][1], pzs);
		OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_UPDATE, Object[o_id][object_pos][0], Object[o_id][object_pos][1], pzs, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
	}
	else
	{
		Object[o_id][object_pos][2] = pzs;
		UpdateObjectInfoTextdraw(playerid, o_id);		
	}
	return 1;
}

// ---- 3d teksty ---- //
COMMAND:ec(playerid, params[])
{
	new owner = -1, owner_type = -1;
	GetLabelDataForPlayer(playerid, owner, owner_type);
	
	if( owner_type == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do tworzenia etykiet w tym miejscu.");
	
	if( owner_type == LABEL_OWNER_TYPE_DOOR )
	{
		new d_id = GetDoorByUid(owner), count = CountDoorLabels(d_id);
		
		if( GetPlayerPermissions(playerid, 5) )
		{
			if( IsPlayerVip(playerid) && count >= 5 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Posiadaj¹c konto premium mo¿esz maksymalnie stworzyæ 5 etykiet w ka¿dych drzwiach.");
			else if( !IsPlayerVip(playerid) && count >= 1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadaj¹c konta premium mo¿esz maksymalnie stworzyæ 1 etykietê w ka¿dych drzwiach.");
		}
	}
	
	if( owner_type == LABEL_OWNER_TYPE_AREA )
	{
		new a_id = GetAreaByUid(owner), count = CountAreaLabels(a_id);
		
		if( GetPlayerPermissions(playerid, 5))
		{
			if( IsPlayerVip(playerid) && count >= 5 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Posiadaj¹c konto premium mo¿esz maksymalnie stworzyæ 5 etykiet w ka¿dej strefie.");
			else if( !IsPlayerVip(playerid) && count >= 1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadaj¹c konta premium mo¿esz maksymalnie stworzyæ 1 etykietê w ka¿dej strefie.");
		}
	}
	
	new Float:distance, text[128];
	if( sscanf(params, "fs[128]", distance, text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ec [dystans rysowania] [tekst]");
	if( distance <= 0.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ec [dystans rysowania] [tekst]");
	
	new Float:p_pos[3];
	GetPointInAngleOfPlayer(playerid, p_pos[0], p_pos[1], p_pos[2], 2.0, 0.0);
	
	new Cache:result;
	result = mysql_query(g_sql, sprintf("INSERT INTO `crp_3dlabels` (label_uid, label_owner, label_ownertype, label_desc, label_color, label_posx, label_posy, label_posz, label_drawdist, label_world, label_interior) VALUES (null, %d, %d, '%s', '%s', %f, %f, %f, %f, %d, %d)", owner, owner_type, text, "FFFFFF", p_pos[0], p_pos[1], p_pos[2], distance, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid)));
	
	new uid = cache_insert_id(), lid = LoadLabel(sprintf("WHERE `label_uid` = %d", uid), true);

	cache_delete(result);

	Streamer_UpdateEx(playerid, p_pos[0], p_pos[1], p_pos[2]);
	
	SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie stworzy³eœ etykietê [UID: %d, ID: %d].", uid, lid));
	
	return 1;
}

COMMAND:esel(playerid, params[])
{
	new l_id = GetNearestLabel(playerid);
	if( l_id == -1 ) return SendPlayerInformation(playerid, "W poblizu nie ma zadnej etykiety, ktora moglbys edytowac.", 4000);
	
	new string[200], text[15];
	GetDynamic3DTextLabelText(Text3D:l_id, string);			
	str_replace("\\n", "\n", string, text);
	string[0] = EOS;

	format(string, sizeof(string), "%d. Etykieta '%s...'~n~", Label[Text3D:l_id][label_uid], text);
	format(string, sizeof(string), "%s~p~Owner: ~w~%d:%d    ~p~Kolor: ~w~%06x    ~p~Draw: ~w~%.1f~n~", string, Label[Text3D:l_id][label_owner_type], Label[Text3D:l_id][label_owner], Label[Text3D:l_id][label_color], Label[Text3D:l_id][label_draw]);
	format(string, sizeof(string), "%s~n~~r~X: ~w~%.2f   ~r~Y: ~w~%.2f    ~r~Z: ~w~%.2f", string, Label[Text3D:l_id][label_pos][0], Label[Text3D:l_id][label_pos][1], Label[Text3D:l_id][label_pos][2]);
	
	PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], string);
	PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);
	
	pInfo[playerid][player_esel_edited_label] = Label[Text3D:l_id][label_uid];

	pInfo[playerid][player_esel_edited_object] = CreateDynamicObject(1455, Label[Text3D:l_id][label_pos][0], Label[Text3D:l_id][label_pos][1], Label[Text3D:l_id][label_pos][2], 0.0, 0.0, 0.0, -1, -1, playerid, 50.0);
	Streamer_UpdateEx(playerid, Label[Text3D:l_id][label_pos][0], Label[Text3D:l_id][label_pos][1], Label[Text3D:l_id][label_pos][2]);
	EditDynamicObject(playerid, pInfo[playerid][player_esel_edited_object]);
	
	SendPlayerInformation(playerid, "Aby zmienic pozycje etykiety, mozesz edytowac zaznaczony obiekt.~n~Aby ~g~zatwierdzic ~w~edycje wybierz ~y~ikonke dyskietki~w~.", 10000);
	
	DeleteLabel(l_id, false);
	
	return 1;
}

COMMAND:edel(playerid, params[])
{
	if( !IsValidDynamicObject(pInfo[playerid][player_esel_edited_object]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie edytujesz ¿adnej etykiety.");
	
	SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie usun¹³eœ etykietê [UID: %d].", pInfo[playerid][player_esel_edited_label]));

	mysql_pquery(g_sql, sprintf("DELETE FROM `crp_3dlabels` WHERE `label_uid` = %d", pInfo[playerid][player_esel_edited_label]));
	pInfo[playerid][player_esel_edited_label] = 0;
	
	DestroyDynamicObject(pInfo[playerid][player_esel_edited_object]);
	pInfo[playerid][player_esel_edited_object] = -1;
	
	SendPlayerInformation(playerid, "", 0);
	PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
	
	CancelEdit(playerid);
	return 1;
}

COMMAND:k(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(k)rzyk [tekst]");
	
	ProxMessage(playerid, params, PROX_SHOUT);
	return 1;
}

COMMAND:s(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(s)zept [tekst]");
	
	ProxMessage(playerid, params, PROX_QUIET);
	return 1;
}

COMMAND:c(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(c)icho [tekst]");
	
	ProxMessage(playerid, params, PROX_QUIET);
	return 1;
}

COMMAND:ja(playerid, params[])
{
	return cmd_me(playerid, params);
}

COMMAND:me(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /me [czynnoœæ]");
	
	ProxMessage(playerid, params, PROX_ME);
	return 1;
}

COMMAND:do(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /do [czynnoœæ]");
	
	ProxMessage(playerid, params, PROX_DO);
	return 1;
}

COMMAND:w(playerid, params[])
{
	return cmd_whisper(playerid, params);
}

COMMAND:pm(playerid, params[])
{
	return cmd_whisper(playerid, params);
}

COMMAND:silnik(playerid, params[])
{
	if( !IsPlayerInAnyVehicle(playerid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w ¿adnym pojedŸdzie.");
	
	new vid = GetPlayerVehicleID(playerid);
	
	// Sprawdzamy czy jest kierowc¹
	if( !CanPlayerUseVehicle(playerid, vid) || GetVehicleType(vid) == VEHICLE_TYPE_BIKE ) return 1;
	if( GetPlayerVehicleSeat(playerid) != 0 ) return 1;
	
	// Sprawdzamy czy silnik czasem nie jest ju¿ odpalany
	if( Vehicle[vid][vehicle_engine_starting] ) return 1;
	
	if( Vehicle[vid][vehicle_engine] )
	{
		// Silnik jest juz odpalony, wiec go gasimy
		if( CanPlayerUseVehicle(playerid, vid) ) TextDrawShowForPlayer(playerid, vehicleInfo);
		
		Vehicle[vid][vehicle_engine] = false;
		
		SaveVehicle(vid);
		
		UpdateVehicleVisuals(vid);
	}
	else
	{
		// Silnik nie jest odpalony, wiec go odpalamy

		if( Vehicle[vid][vehicle_state] > 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Na tym pojeŸdzie przeprowadzana jest aktualnie jakaœ akcja. Aby go odpaliæ poczekaj do jej ukoñczenia.");
		
		// Ale na poczatku zobaczymy czy w ogole autko ma papu
		if( Vehicle[vid][vehicle_fuel_type] == 0 ) return 1;
		if( Vehicle[vid][vehicle_fuel_current] == 0.0 ) return SendGuiInformation(playerid, "Informacja", "W baku tego pojazdu nie ma paliwa. W tej sytuacji mo¿esz zadzwoniæ po pomoc drogow¹ lub kupiæ kanister.");
		
		Vehicle[vid][vehicle_engine_starting] = true;
		
		defer VehicleEngineStart[2000](playerid, vid);
		
		TextDrawShowForPlayer(playerid, vehicleEngineStarting);
	}
	return 1;
}

COMMAND:whisper(playerid, params[])
{
	new targetid, message[180];
	if( sscanf(params, "ds[180]", targetid, message) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(w)hisper [id gracza] [tekst]");
	if( isnull(message) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(w)hisper [id gracza] [tekst]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( targetid == playerid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz pisaæ sam do siebie.");
	if( PlayerHasTog(targetid, TOG_W) ) return SendGuiInformation(playerid, "Informacja", "Gracz o podanym id ma zablokowane wiadomoœci.");
	
	if( pInfo[playerid][player_bw] > 0 && GetPlayerPermissions(targetid, 1) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Podczas bw mo¿esz wysy³aæ wiadomoœci tylko do administratorów.");
	
	if( pInfo[targetid][player_admin_duty] )
	{
		strcopy(pInfo[playerid][player_tmp_whisper], message, 180);
		randomString(pInfo[playerid][player_tmp_token], 16);
		
		pInfo[playerid][player_tmp_whisper_id] = targetid;
		
		ShowPlayerDialog(playerid, DIALOG_WHISPER_TOKEN, DIALOG_STYLE_INPUT, "Przepisz token", sprintf("Aby skontaktowaæ siê z cz³onkiem ekipy przepisz wygenerowany kod(wielkoœæ znaków ma znaczenie):\n\t%s", pInfo[playerid][player_tmp_token]), "Gotowe", "Anuluj");
	}	
	
	if( GetPlayerUsedItem(targetid, ITEM_TYPE_MASK) > -1 ) SendFormattedClientMessage(playerid, 0xFFCB73FF, "(( > %s: %s ))", pInfo[targetid][player_name], message);
	else SendFormattedClientMessage(playerid, 0xFFCB73FF, "(( > %s (%d): %s ))", pInfo[targetid][player_name], targetid, message);
	
	pInfo[targetid][player_last_pm_playerid] = playerid;
	if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASK) > -1 ) SendFormattedClientMessage(targetid, 0xFFAE31FF, "(( %s: %s ))", pInfo[playerid][player_name], message);
	else SendFormattedClientMessage(targetid, 0xFFAE31FF, "(( %s (%d): %s ))", pInfo[playerid][player_name], playerid, message);

	PlayerLog(sprintf("[ooc:pm] > %s: %s", PlayerLogLink(pInfo[targetid][player_id]), message), pInfo[playerid][player_id], "chat");
	PlayerLog(sprintf("[ooc:pm] < %s: %s", PlayerLogLink(pInfo[playerid][player_id]), message), pInfo[targetid][player_id], "chat");

	if( pInfo[targetid][player_afk] )
	{
		new afkHour, afkMinute, temp;
		TimestampToDate(pInfo[targetid][player_last_activity], temp, temp, temp, afkHour, afkMinute, temp, 1);
		
		if( afkHour == 24 ) afkHour = 0;
		SendPlayerInformation(playerid, sprintf("Gracz do ktorego piszesz jest ~y~AFK~w~ od %02d:%02d", afkHour, afkMinute), 4000);
	}
	
	return 1;
}

COMMAND:bank(playerid, params[])
{
	if( GetPlayerVirtualWorld(playerid) == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do banku.");
	
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do banku.");
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do banku.");
	
	new gid = GetGroupByUid(Door[d_id][door_owner]);
	if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do banku.");
	if( Group[gid][group_type] != GROUP_TYPE_BANK ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do banku.");
	
	if( pInfo[playerid][player_bank_number] > 0 )
	{	
		DynamicGui_Init(playerid);
		new str[200];
		
		format(str, sizeof(str), "01.\tStan konta\n");
		DynamicGui_AddRow(playerid, DG_BANK_KONTO_STAN);
		
		format(str, sizeof(str), "%s02.\tWp³aæ\n", str);
		DynamicGui_AddRow(playerid, DG_BANK_KONTO_WPLAC);
		
		format(str, sizeof(str), "%s03.\tWyp³aæ\n", str);
		DynamicGui_AddRow(playerid, DG_BANK_KONTO_WYPLAC);
		format(str, sizeof(str), "%s04.\tPrzelew\n", str);
		DynamicGui_AddRow(playerid, DG_BANK_KONTO_PRZELEW);

		format(str, sizeof(str), "%s05.\tHistoria transakcji\n", str);
		DynamicGui_AddRow(playerid, DG_BANK_HISTORY);

		format(str, sizeof(str), "%s06.\tSp³aæ debet ($%d)\n", str, abs(pInfo[playerid][player_debit]));
		DynamicGui_AddRow(playerid, DG_BANK_DEBIT);

		ShowPlayerDialog(playerid, DIALOG_BANK_KONTO, DIALOG_STYLE_LIST, sprintf("Bank » Konto osobiste %d", pInfo[playerid][player_bank_number]), str, "Wybierz", "Zamknij");
	}
	else
	{
		ShowPlayerDialog(playerid, DIALOG_BANK_ZALOZ_KONTO, DIALOG_STYLE_MSGBOX, "Bank » Zak³adanie konta", "Nie posiadasz jeszcze konta w banku. Mo¿esz je za³o¿yæ ju¿ teraz - wystarczy, ¿e wciœniesz 'Dalej'.", "Dalej", "Zamknij");
	}
	
	return 1;
}

COMMAND:wyrzuc(playerid, params[])
{
	new vid = GetPlayerVehicleID(playerid);
	if( !vid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w ¿adnym pojeŸdzie.");
	if( GetPlayerVehicleSeat(playerid) != 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ kierowc¹ pojazdu w którym siedzisz.");
	
	new targetid;
	if( sscanf(params, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /wyrzuc [id gracza]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( GetPlayerVehicleID(targetid) != vid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie siedzi w Twoim pojeŸdzie.");
	
	RemovePlayerFromVehicle(targetid);	
	
	return 1;
}

COMMAND:bankomat(playerid, params[])
{
	if( GetPlayerArea(playerid, AREA_TYPE_ATM) == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê przy bankomacie.");
	
	if( pInfo[playerid][player_bank_number] > 0 )
	{	
		DynamicGui_Init(playerid);
		new str[200];
		
		format(str, sizeof(str), "01.\tStan konta\n");
		DynamicGui_AddRow(playerid, DG_BANKOMAT_KONTO_STAN);
		
		format(str, sizeof(str), "%s02.\tWyp³aæ\n", str);
		DynamicGui_AddRow(playerid, DG_BANKOMAT_KONTO_WYPLAC);
		
		ShowPlayerDialog(playerid, DIALOG_BANKOMAT_KONTO, DIALOG_STYLE_LIST, sprintf("Bankomat » Konto osobiste %d", pInfo[playerid][player_bank_number]), str, "Wybierz", "Zamknij");
	}
	else SendGuiInformation(playerid, "Informacja", "Nie posiadasz konta w banku. Udaj siê do budynku banku oznaczonego na mapie zielonym dolarem.");
	return 1;
}

COMMAND:bus(playerid, params[])
{
	if( pInfo[playerid][player_is_waiting_for_bus] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Aktualnie oczekujesz ju¿ na autobus.");
	new aid = GetPlayerArea(playerid, AREA_TYPE_BUS);
	if( aid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê na przystanku autobusowym.");
	
	new bid = Area[aid][area_owner];
	GetBusPos(bid, pInfo[playerid][player_bus_camera][0], pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]);
	
	pInfo[playerid][player_is_selecting_bus] = true;
	pInfo[playerid][player_start_bus_id] = bid;
	TogglePlayerSpectating(playerid, 1);
	
	defer BusSelectPrepare[50](playerid);
	
	return 1;
}

COMMAND:tog(playerid, params[])
{
	if( !strcmp(params, "w") )
	{
		if( PlayerHasTog(playerid, TOG_W) )
		{
			pInfo[playerid][player_tog] -= TOG_W;
			SendGuiInformation(playerid, "Informacja", "Odblokowa³eœ mo¿liwoœæ wysy³ania do Ciebie wiadomoœci przez /w.");
		}
		else
		{
			pInfo[playerid][player_tog] += TOG_W;
			SendGuiInformation(playerid, "Informacja", "Zablokowa³eœ mo¿liwoœæ wysy³ania do Ciebie wiadomoœci przez /w.");
		}
	}
	else if( !strcmp(params, "g1") )
	{
		if( PlayerHasTog(playerid, TOG_G1) )
		{
			pInfo[playerid][player_tog] -= TOG_G1;
			SendGuiInformation(playerid, "Informacja", "Odblokowa³eœ wyœwietlanie czatu grupy na 1 slocie.");
		}
		else
		{
			pInfo[playerid][player_tog] += TOG_G1;
			SendGuiInformation(playerid, "Informacja", "Zablokowa³eœ wyœwietlanie czatu grupy na 1 slocie.");
		}
	}
	else if( !strcmp(params, "g2") )
	{
		if( PlayerHasTog(playerid, TOG_G2) )
		{
			pInfo[playerid][player_tog] -= TOG_G2;
			SendGuiInformation(playerid, "Informacja", "Odblokowa³eœ wyœwietlanie czatu grupy na 2 slocie.");
		}
		else
		{
			pInfo[playerid][player_tog] += TOG_G2;
			SendGuiInformation(playerid, "Informacja", "Zablokowa³eœ wyœwietlanie czatu grupy na 2 slocie.");
		}
	}
	else if( !strcmp(params, "g3") )
	{
		if( PlayerHasTog(playerid, TOG_G3) )
		{
			pInfo[playerid][player_tog] -= TOG_G3;
			SendGuiInformation(playerid, "Informacja", "Odblokowa³eœ wyœwietlanie czatu grupy na 3 slocie.");
		}
		else
		{
			pInfo[playerid][player_tog] += TOG_G3;
			SendGuiInformation(playerid, "Informacja", "Zablokowa³eœ wyœwietlanie czatu grupy na 3 slocie.");
		}
	}
	else if( !strcmp(params, "g4") )
	{
		if( PlayerHasTog(playerid, TOG_G4) )
		{
			pInfo[playerid][player_tog] -= TOG_G4;
			SendGuiInformation(playerid, "Informacja", "Odblokowa³eœ wyœwietlanie czatu grupy na 4 slocie.");
		}
		else
		{
			pInfo[playerid][player_tog] += TOG_G4;
			SendGuiInformation(playerid, "Informacja", "Zablokowa³eœ wyœwietlanie czatu grupy na 4 slocie.");
		}
	}
	else if( !strcmp(params, "g5") )
	{
		if( PlayerHasTog(playerid, TOG_G5) )
		{
			pInfo[playerid][player_tog] -= TOG_G5;
			SendGuiInformation(playerid, "Informacja", "Odblokowa³eœ wyœwietlanie czatu grupy na 5 slocie.");
		}
		else
		{
			pInfo[playerid][player_tog] += TOG_G5;
			SendGuiInformation(playerid, "Informacja", "Zablokowa³eœ wyœwietlanie czatu grupy na 5 slocie.");
		}
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /tog [w/g1/g2/g3/g4/g5]");
	return 1;
}

COMMAND:re(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /re [tekst]");
	if( !IsPlayerConnected(pInfo[playerid][player_last_pm_playerid]) ) return SendClientMessage(playerid, COLOR_GREY, "Gracz, z którym wczeœniej rozmawia³eœ nie jest pod³¹czony.");
	if( !pInfo[pInfo[playerid][player_last_pm_playerid]][player_logged] ) return SendClientMessage(playerid, COLOR_GREY, "Gracz, z którym wczeœniej rozmawia³eœ nie jest zalogowany.");
	
	cmd_w(playerid, sprintf("%d %s", pInfo[playerid][player_last_pm_playerid], params));
	return 1;
}

COMMAND:qs(playerid, params[])
{
	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
	
	mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d, `char_quittime`=%d WHERE `char_uid`=%d", x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), gettime(), pInfo[playerid][player_id]));
	pInfo[playerid][player_qs] = true;
	Kick(playerid);
	return 1;
}

COMMAND:report(playerid, params[])
{
	new targetid, text[100];
	if( sscanf(params, "d s[100]", targetid, text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /report [id gracza] [treœæ]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	new string[150];
	format(string, sizeof(string), "[Raport] %s [%d]: %s. (%s [%d])", pInfo[playerid][player_name], playerid, text, pInfo[targetid][player_name], targetid);
	
	foreach(new p : Player)
	{
		if( GetPlayerPermissions(p, 1)) continue;
		if( !pInfo[p][player_admin_duty] ) continue;
		
		SendClientMessage(p, 0xFF6347FF, string);	
	}
	
	SendGuiInformation(playerid, "Informacja", "Twój report zosta³ wys³any do administracji. Jeœli jakiœ administrator jest na s³u¿bie to napewno zajme siê Twoj¹ spraw¹.");
	
	return 1;
}

COMMAND:pay(playerid, params[])
{
	return cmd_plac(playerid, params);
}

COMMAND:plac(playerid, params[])
{
	if( GetPlayerOnlineTime(playerid) < 18000 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Masz przegrane mniej ni¿ 5h.");
	new targetid, amount;
	if( sscanf(params, "dd", targetid, amount) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /plac [id gracza] [iloœæ]");
	if( amount <= 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /plac [id gracza] [iloœæ]");
	if( targetid == playerid ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /plac [id gracza] [iloœæ]");
	
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	new Float:p_pos[3];
	GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
	if( GetPlayerDistanceFromPoint(targetid, p_pos[0], p_pos[1], p_pos[2]) > 5.0 || GetPlayerVirtualWorld(playerid) != GetPlayerVirtualWorld(targetid) || GetPlayerInterior(playerid) != GetPlayerInterior(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ten gracz znajduje siê zbyt daleko Ciebie.");
	
	if( pInfo[playerid][player_money] < amount ) return SendClientMessage(playerid, COLOR_GREY, "Nie masz tyle pieniêdzy.");
	

	GivePlayerMoney(playerid, -amount);
	GivePlayerMoney(targetid, amount);
	
	ProxMessage(playerid, sprintf("podaje trochê gotówki %s.", pInfo[targetid][player_name]), PROX_AME);
	
	return 1;
}

COMMAND:login(playerid, params[])
{
	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
		
	mysql_query(g_sql, sprintf("UPDATE `crp_characters` SET `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d, `char_quittime`=%d WHERE `char_uid`=%d", x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), gettime(), pInfo[playerid][player_id]), false);

	OnPlayerDisconnect(playerid, 3);
	OnPlayerConnect(playerid);
	return 1;
}

COMMAND:l(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /l [tekst]");
	ProxMessage(playerid, params, PROX_LOCAL_L);
	return 1;
}

COMMAND:b(playerid, params[])
{
	if( PlayerHasBlock(playerid, BLOCK_OOC) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Masz aktywn¹ blokadê czatu ooc.");
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /b [tekst]");
	ProxMessage(playerid, params, PROX_OOC);
	return 1;
}

COMMAND:drzwi(playerid, params[])
{
	new sub_cmd[40];	
	if( sscanf(params, "S()[40]", sub_cmd) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /drzwi [zamknij/opcje]");		
	
	if(isnull(params))
	{
		new a_id = GetPlayerArea(playerid, AREA_TYPE_DOOR_OUTER);
		if( GetPlayerVirtualWorld(playerid) == 0 && a_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Nie stoisz w ¿adnych drzwiach.");
		new d_id;

		if( a_id != -1 ) d_id = Area[a_id][area_owner];
		else d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
		
		if( d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Nie stoisz w ¿adnych drzwiach.");

		if(CanPlayerEditDoor(playerid, d_id)) return ShowPlayerDoorsEdit(playerid, d_id);
		else SendClientMessage(playerid, COLOR_GREY, "Tip: /drzwi {z(amknij)}");
	}
	else if( !strcmp(sub_cmd, "zamknij") || !strcmp(sub_cmd, "z"))
	{
		new a_id = GetPlayerArea(playerid, AREA_TYPE_DOOR_OUTER);
		if( a_id == -1 ) a_id = GetPlayerArea(playerid, AREA_TYPE_DOOR_INNER);
		
		if( a_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Nie stoisz w ¿adnych drzwiach.");
		
		new d_id = Area[a_id][area_owner];
		if( !CanPlayerUseDoor(playerid, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Nie masz uprawnieñ do u¿ywania tych drzwi.");
		
		if( Door[d_id][door_closed] )
		{
			Door[d_id][door_closed] = false;
			if( Area[a_id][area_type] == AREA_TYPE_DOOR_OUTER ) ShowPlayerDoorTextdraw(playerid, d_id);
		}
		else
		{
			Door[d_id][door_closed] = true;
			if( Area[a_id][area_type] == AREA_TYPE_DOOR_OUTER ) ShowPlayerDoorTextdraw(playerid, d_id);
		}
		
		ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.1, 0, 0, 0, 0, 0, 1);
	}
	else SendClientMessage(playerid, COLOR_GREY, "Tip: /drzwi {z(amknij)}");
	return 1;
}

COMMAND:tel(playerid, params[])
{
	new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz ¿adnego w³¹czonego telefonu.");
	
	new sub_cmd[40], param[150];
	sscanf(params, "s[40] S()[150]", sub_cmd, param);
	
	if( isnull(params) )
	{
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, itemid);
		
		new str[150];
		
		format(str, sizeof(str), "%s01\tWybierz numer\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_CALL);
		
		format(str, sizeof(str), "%s02\tNapisz SMS'a\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_SMS);
		
		format(str, sizeof(str), "%s03\tKontakty\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS);
		
		format(str, sizeof(str), "%s04\tWyœlij vCard\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_VCARD);
		
		format(str, sizeof(str), "%s05\tWy³¹cz telefon\n", str);
		DynamicGui_AddRow(playerid, DG_PHONE_TURNOFF);
		
		new th, tmin, tsek;
		gettime(th, tmin, tsek);
		
		ShowPlayerDialog(playerid, DIALOG_PHONE, DIALOG_STYLE_LIST, sprintf("%s [%d] [%02d:%02d:%02d]", Item[itemid][item_name], Item[itemid][item_value1], th, tmin, tsek), str, "Wybierz", "Anuluj");
	}
	else
	{
		if( !strcmp(sub_cmd, "odbierz") || !strcmp(sub_cmd, "o") )
		{
			if( pInfo[playerid][player_phone_caller] != INVALID_PLAYER_ID && !pInfo[playerid][player_phone_call_started] )
			{
				// Odbieramy polaczenie
				SendClientMessage(playerid, COLOR_YELLOW, "Odebrano telefon. Aby zakoñczyæ rozmowê u¿yj komendy /tel zakoncz.");
				SendClientMessage(pInfo[playerid][player_phone_caller], COLOR_YELLOW, "Rozmówca odebra³ telefon. Aby zakoñczyæ rozmowê u¿yj komendy /tel zakoncz.");
				pInfo[playerid][player_phone_call_started] = true;
				pInfo[pInfo[playerid][player_phone_caller]][player_phone_call_started] = true;
				
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
	
				pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
				if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);
			}
		}
		else if( !strcmp(sub_cmd, "zakoncz") || !strcmp(sub_cmd, "z") )
		{
			if( pInfo[playerid][player_phone_call_started] )
			{
				SendClientMessage(playerid, COLOR_YELLOW, "Rozmowa zakonczona.");
				pInfo[playerid][player_phone_call_started] = false;
				
				new targetid = -1;
				if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
				else targetid = pInfo[playerid][player_phone_caller];
				
				pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SendClientMessage(targetid, COLOR_YELLOW, "Rozmowa zakoñczona.");
				pInfo[targetid][player_phone_call_started] = false;
				pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
				if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
			}
			else if( !pInfo[playerid][player_phone_call_started] && pInfo[playerid][player_phone_caller] != INVALID_PLAYER_ID && pInfo[playerid][player_phone_receiver] == INVALID_PLAYER_ID )
			{
				// odrzuca polaczenie odbiorca
				SendClientMessage(playerid, COLOR_YELLOW, "Rozmowa zakonczona.");
				pInfo[playerid][player_phone_call_started] = false;
				
				new targetid = -1;
				if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
				else targetid = pInfo[playerid][player_phone_caller];
				
				pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SendClientMessage(targetid, COLOR_YELLOW, "Sygna³ zajêtoœci.");
				pInfo[targetid][player_phone_call_started] = false;
				pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
				if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
			}
			else if( !pInfo[playerid][player_phone_call_started] && pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID && pInfo[playerid][player_phone_receiver] != INVALID_PLAYER_ID )
			{
				// przerywa polaczenie dzwoniacy
				SendClientMessage(playerid, COLOR_YELLOW, "Rozmowa zakonczona.");
				pInfo[playerid][player_phone_call_started] = false;
				
				new targetid = -1;
				if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
				else targetid = pInfo[playerid][player_phone_caller];
				
				pInfo[playerid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SendClientMessage(targetid, COLOR_YELLOW, "Po³¹czenie przerwane.");
				pInfo[targetid][player_phone_call_started] = false;
				pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
				pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
				
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
				if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
			}
		}
	}
	return 1;
}

COMMAND:cennik(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( Group[gid][group_type] != GROUP_TYPE_GASTRONOMY && Group[gid][group_type] != GROUP_TYPE_GASTRONOMY_KONC ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");

	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ w budynku grupy.");
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP || Door[d_id][door_owner] != Group[gid][group_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ w budynku grupy.");
	
	
	new targetid;
	if( sscanf(params, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /cennik [id gracza]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	new str[2048], count;
	
	foreach(new itemid : Items)
	{
		if( Item[itemid][item_owner_type] != ITEM_OWNER_TYPE_DOOR_WAREHOUSE || Item[itemid][item_owner] != Door[d_id][door_uid] ) continue;
		
		format(str, sizeof(str), "%s%d\t$%d\t%s (x%d)\n", str, Item[itemid][item_uid], Item[itemid][item_price], Item[itemid][item_name], Item[itemid][item_amount]);
		count++;
	}
	
	if( !count ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Magazyn tych drzwi jest pust, a w cenniku nie ma ¿adnych produktów.");
	else ShowPlayerDialog(targetid, DIALOG_INFO, DIALOG_STYLE_LIST, "Menu", str, "OK", "");
	
	return 1;
}

COMMAND:podaj(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( Group[gid][group_type] != GROUP_TYPE_GASTRONOMY && Group[gid][group_type] != GROUP_TYPE_GASTRONOMY_KONC ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");

	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ w budynku grupy.");
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP || Door[d_id][door_owner] != Group[gid][group_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ w budynku grupy.");
	
	new targetid, i_uid;
	if( sscanf(params, "dd", targetid, i_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /podaj [id gracza] [uid przedmiotu(z /cennik)]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");

	new itemid = GetItemByUid(i_uid);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Przedmiot o podanym uid nie istnieje.");
	if( Item[itemid][item_owner_type] != ITEM_OWNER_TYPE_DOOR_WAREHOUSE || Item[itemid][item_owner] != Door[d_id][door_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Przedmiot o podanym uid nie nale¿y do magazynu tych drzwi.");
	if( Item[itemid][item_amount] == 0 )
	{
		DeleteItem(itemid, true);
		
		return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Przedmiot o podanym uid nie istnieje.");
	}
	
	
	new resp = SetOffer(playerid, targetid, OFFER_TYPE_ITEM_PODAJ, Item[itemid][item_price], itemid);

	if( resp ) ShowPlayerOffer(targetid, playerid, "Przedmiot z magazynu", sprintf("%s [%d:%d:%d]", Item[itemid][item_name], Item[itemid][item_type], Item[itemid][item_value1], Item[itemid][item_value2]), Item[itemid][item_price]);
	
	return 1;
}

COMMAND:o(playerid, params[])
{
	return cmd_oferuj(playerid, params);
}

COMMAND:oferuj(playerid, params[])
{
	new sub_cmd[40], param[150];
	sscanf(params, "s[40] S()[150]", sub_cmd, param);
	
	if( isnull(sub_cmd) )
	{
		SendClientMessage(playerid, COLOR_GREY, "/(o)feruj [dom/pojazd] (...)");
		SendClientMessage(playerid, COLOR_WHITE, "Inne oferty dostêpne dla Ciebie:");
		for(new i;i<5;i++)
		{
			if( pGroup[playerid][i][pg_id] != -1 )
			{
				switch( Group[pGroup[playerid][i][pg_id]][group_type] )
				{
					case GROUP_TYPE_WARSZTAT: SendClientMessage(playerid, COLOR_GREY, "  Warsztat: naprawe, malowanie");
					case GROUP_TYPE_EMS: SendClientMessage(playerid, COLOR_GREY, "  Pogotowie: metryczke, leczenie");
					case GROUP_TYPE_GYM: SendClientMessage(playerid, COLOR_GREY, "  Si³ownia: karnet, trening");
				}
			}
		}
	}
	else
	{
		if( !strcmp(sub_cmd, "pojazd") )
		{	
			new targetid, price;
			if( sscanf(param, "dd", targetid, price) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(o)feruj pojazd [id gracza] [cena]");
			new vid = GetPlayerVehicleID(playerid);
			if( vid == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w pojeŸdzie.");
			if( !CanPlayerEditVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd, w którym siê znajdujesz nie nale¿y do Ciebie.");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
			if( price < 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Podana cena nie mo¿e byæ mniejsza od 0.");
			
			new resp = SetOffer(playerid, targetid, OFFER_TYPE_VEH_SELL, price, vid);
				
			if( resp ) 
			{
				ShowPlayerOffer(targetid, playerid, "Sprzedaz pojazdu", sprintf("Pojazd %s (M: %d, UID: %d)", VehicleNames[Vehicle[vid][vehicle_model]-400], Vehicle[vid][vehicle_model], Vehicle[vid][vehicle_uid]), price, true, true);
				// Jesli silnik odpalony to gasimy
				if( Vehicle[vid][vehicle_engine] )
				{
					Vehicle[vid][vehicle_engine] = false;		
					UpdateVehicleVisuals(vid);
				}
				
				// Ustawiamy akcje na pojezdzie ;)
				Vehicle[vid][vehicle_state] = VEHICLE_STATE_SELLING;
			}
		}
		else if( !strcmp(sub_cmd, "reklame") )
		{
			new slot = GetPlayerDutySlot(playerid);
			if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			
			new gid = pGroup[playerid][slot][pg_id];
			if( Group[gid][group_type] != GROUP_TYPE_RADIO ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			
			new targetid, time;
			if( sscanf(param, "dd", targetid, time) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(o)feruj reklame [id gracza] [czas w sekundach]");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");	
			
			new resp = SetOffer(playerid, targetid, OFFER_TYPE_LSN_REKLAMA, floatround(time*1.5), gid);
				
			if( resp ) ShowPlayerOffer(targetid, playerid, "LSN", sprintf("Reklama radiowa na %d sekund.", time), floatround(time*1.5));
		}
		else if( !strcmp(sub_cmd, "wywiad") )
		{
			new slot = GetPlayerDutySlot(playerid);
			if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			
			new gid = pGroup[playerid][slot][pg_id];
			if( Group[gid][group_type] != GROUP_TYPE_RADIO ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			
			foreach(new p : Player)
			{
				if( GetPlayerGroupSlot(p, gid) > -1 )
				{
					if( pInfo[p][player_lsn_wywiad_with] != INVALID_PLAYER_ID ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Inny gracz z Twojej grupy aktualnie przeprowadza ju¿ wywiad.");
				}
			}
			
			new targetid;
			if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(o)feruj wywiad [id gracza]");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹", "Gracz o podanym id nie jest zalogowany.");
			
			new resp = SetOffer(playerid, targetid, OFFER_TYPE_LSN_WYWIAD, 0);
				
			if( resp ) ShowPlayerOffer(targetid, playerid, "LSN", "Wywiad na antenie.", 0);
		}
		else if( !strcmp(sub_cmd, "naprawe") )
		{
			new slot = GetPlayerDutySlot(playerid);
			if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			new gid = pGroup[playerid][slot][pg_id];
			if( Group[gid][group_type] != GROUP_TYPE_WARSZTAT ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			if( !IsPlayerInGroupPlace(playerid, gid, true) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê na terenie nale¿¹cym do Twojej grupy.");
		
			if( IsPlayerPerformingAnyAction(playerid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz wykonywaæ kilku akcji jednoczeœnie.");			

			new targetid, vuid;
			if( sscanf(param, "dd", targetid, vuid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(o)feruj naprawe [id gracza] [uid pojazdu]");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
			if( GetDistanceBetweenPlayers(playerid, targetid) > 20 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest zbyt daleko.");
			
			new vid = GetVehicleByUid(vuid);
			if( vid == INVALID_VEHICLE_ID ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid nie istnieje.");
			if( !CanPlayerUseVehicle(targetid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie ma uprawnieñ do pojazdu o podanym uid.");
			if( GetPlayerDistanceFromVehicle(playerid, vid) > 20 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid jest zbyt daleko.");
			if( Vehicle[vid][vehicle_engine] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid ma odpalony silnik.");

			new vehicleDmg = 0;
			for(new i;i<4;i++) vehicleDmg += Vehicle[vid][vehicle_damage][i];
			if( Vehicle[vid][vehicle_health] == 1000.0 && vehicleDmg == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid nie jest zniszczony.");
			if( Vehicle[vid][vehicle_state] > 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Na pojeŸdzie o podanym uid jest aktualnie przeprowadzana jakaœ akcja.");
			
			new price = floatround((1000.0 - Vehicle[vid][vehicle_health]) * 0.2 * VehicleEngineCapacity[GetVehicleModel(vid)-400]);		
			pInfo[targetid][player_repair_vuid] = vuid;
			new resp = SetOffer(playerid, targetid, OFFER_TYPE_NAPRAWA, price, gid);	
			if( resp ) ShowPlayerOffer(targetid, playerid, "Pojazd", sprintf("Naprawa (+%.1f)", 1000.0 - Vehicle[vid][vehicle_health]), price);
		}
		else if( !strcmp(sub_cmd, "malowanie") )
		{
			// Sprawdzamy czy jest w pomocy drogowej
			new gslot = GetPlayerDutySlot(playerid);
			if( gslot == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Nie posiadasz uprawnieñ do korzystania z tej komendy.");
			new gid = pGroup[playerid][gslot][pg_id];
			if( Group[gid][group_type] != GROUP_TYPE_WARSZTAT ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			if( !IsPlayerInGroupPlace(playerid, gid, true) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê na terenie nale¿¹cym do Twojej grupy.");

			new targetid, vuid;
			if( sscanf(param, "dd", targetid, vuid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(o)feruj malowanie [id gracza] [uid pojazdu]");
			if( !IsPlayerConnected(targetid) || !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
			if( GetDistanceBetweenPlayers(playerid, targetid) > 20 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest zbyt daleko.");
			new vid = GetVehicleByUid(vuid);
			if( vid == INVALID_VEHICLE_ID ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid nie istnieje.");
			if( !CanPlayerEditVehicle(targetid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid nie nale¿y do tego gracza.");
			if( GetPlayerDistanceFromVehicle(playerid, vid) > 20 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid jest zbyt daleko.");
			if( Vehicle[vid][vehicle_engine] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid ma odpalony silnik.");
			new slot = GetWeaponSlot(GetPlayerWeapon(playerid));
			// TODO: przerobic na get player used item
			// TODO: ogarnac temat aby nie dalo sie kilku ofert na raz
			// TODO: vehicle action podczas wybierania koloru
			// TODO: zrobic osobno /o malowanie i /o malowanie-paleta
			// TODO: dodac sprawdzenia dyststansu podczas malowania i naprawy, sprawdzic czy on disconnect sie anuluje, sprawdzic tez jak wyglada kwestia zap³aty za takie us³ugi
			if( pWeapon[playerid][slot][pw_id] != 41 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Aby móc oferowaæ malowanie pojazdu musisz mieæ w d³oni lakier.");

			pInfo[targetid][player_choosing_carcolor] = true;
			pInfo[targetid][player_choosen_carcolor][0] = -1;
			pInfo[targetid][player_choosen_carcolor][1] = -1;
			pInfo[targetid][player_offered_carpaint] = pInfo[playerid][player_id];
			pInfo[targetid][player_carpaint_vuid] = vuid;

			SendPlayerInformation(targetid, "Wybierz kolor, na ktory chcesz przemalowac swoj pojazd", 5000);
			ShowCarColorPickerForPlayer(targetid);

			SendGuiInformation(playerid, "Informacja", "Gracz, któremu oferowa³eœ malowanie wybiera teraz nowy kolor swojego pojazdu - zostaniesz poinformowany kiedy skoñczy.");
		}
		else if( !strcmp(sub_cmd, "leczenie") )
		{
			new slot = GetPlayerDutySlot(playerid);
			if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			
			new gid = pGroup[playerid][slot][pg_id];
			if( Group[gid][group_type] != GROUP_TYPE_EMS ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			if( !IsPlayerInGroupPlace(playerid, gid, true) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê na terenie nale¿¹cym do Twojej grupy.");
			
			new targetid;
			if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(o)feruj leczenie [id gracza]");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
			
			new Float:pos[3];
			GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
			if( GetPlayerDistanceFromPoint(targetid, pos[0], pos[1], pos[2]) > 5.0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest zbyt daleko.");
			
			new resp = SetOffer(playerid, targetid, OFFER_TYPE_EMS_LECZENIE, 30, gid);
				
			if( resp ) ShowPlayerOffer(targetid, playerid, "Lekarz", "Calkowite wyleczenie.", 30);
		}
		else if( !strcmp(sub_cmd, "karnet") )
		{
			new slot = GetPlayerDutySlot(playerid);
			if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			
			new gid = pGroup[playerid][slot][pg_id];
			if( Group[gid][group_type] != GROUP_TYPE_GYM ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			if( !IsPlayerInGroupPlace(playerid, gid, false) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê na terenie nale¿¹cym do Twojej grupy.");
			
			new targetid;
			if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(o)feruj karnet [id gracza]");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
			
			new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			if( d_id == -1 ) return 1;
			
			new itemid = -1;
			foreach(new i : Items)
			{
				if( Item[i][item_owner_type] == ITEM_OWNER_TYPE_DOOR_WAREHOUSE && Item[i][item_owner] == Door[d_id][door_uid] && Item[i][item_type] == ITEM_TYPE_GYM_PASS && Item[i][item_value1] == 0 && Item[i][item_amount] > 0 )
				{
					itemid = i;
					break;
				}
			}
			
			if( itemid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W magazynie tych drzwi nie ma ju¿ ¿adnych karnetów.");
			
			Item[itemid][item_value2] = Group[gid][group_uid];
			mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_value2 = %d WHERE item_uid = %d", Item[itemid][item_value2], Item[itemid][item_uid]));

			new resp = SetOffer(playerid, targetid, OFFER_TYPE_ITEM_PODAJ, Item[itemid][item_price], itemid);

			if( resp ) ShowPlayerOffer(targetid, playerid, "Przedmiot z magazynu", sprintf("%s [%d:%d:%d]", Item[itemid][item_name], Item[itemid][item_type], Item[itemid][item_value1], Item[itemid][item_value2]), Item[itemid][item_price]);
		}
		else if( !strcmp(sub_cmd, "trening") )
		{
			new slot = GetPlayerDutySlot(playerid);
			if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			
			new gid = pGroup[playerid][slot][pg_id];
			if( Group[gid][group_type] != GROUP_TYPE_GYM ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
			if( !IsPlayerInGroupPlace(playerid, gid, false) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê na terenie nale¿¹cym do Twojej grupy.");
			
			new targetid;
			if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(o)feruj trening [id gracza]");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
			
			new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			if( d_id == -1 ) return 1;
			
			new itemid = -1;
			foreach(new i : Items)
			{
				if( Item[i][item_owner_type] == ITEM_OWNER_TYPE_DOOR_WAREHOUSE && Item[i][item_owner] == Door[d_id][door_uid] && Item[i][item_type] == ITEM_TYPE_GYM_PASS && Item[i][item_value1] == 1 && Item[i][item_amount] > 0 )
				{
					itemid = i;
					break;
				}
			}
			
			if( itemid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W magazynie tych drzwi nie ma ju¿ ¿adnych karnetów.");
			
			new resp = SetOffer(playerid, targetid, OFFER_TYPE_ITEM_PODAJ, Item[itemid][item_price], itemid);

			if( resp ) ShowPlayerOffer(targetid, playerid, "Przedmiot z magazynu", sprintf("%s [%d:%d:%d]", Item[itemid][item_name], Item[itemid][item_type], Item[itemid][item_value1], Item[itemid][item_value2]), Item[itemid][item_price]);
		}
		else
		{
			SendClientMessage(playerid, COLOR_GREY, "/(o)feruj [dom/pojazd] (...)");
			SendClientMessage(playerid, COLOR_GREY, "Inne oferty dostêpne dla Ciebie:");
			for(new i;i<4;i++)
			{
				if( pGroup[playerid][i][pg_id] != -1 )
				{
					switch( Group[pGroup[playerid][i][pg_id]][group_type] )
					{
						case GROUP_TYPE_WARSZTAT: SendClientMessage(playerid, COLOR_GREY, "  Warsztat: naprawe, malowanie");
						case GROUP_TYPE_EMS: SendClientMessage(playerid, COLOR_GREY, "  Pogotowie: metryczke, leczenie");
						case GROUP_TYPE_GYM: SendClientMessage(playerid, COLOR_GREY, "  Si³ownia: karnet, trening");
					}
				}
			}
		}
	}

	return 1;
}

COMMAND:kogut(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_BUZZER) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	if( !IsPlayerInAnyVehicle(playerid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w pojeŸdzie.");
	if( GetPlayerVehicleSeat(playerid) != 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ kierowc¹ pojazdu, w którym siê znajdujesz.");
	
	new vid = GetPlayerVehicleID(playerid);
	
	if( Vehicle[vid][vehicle_owner_type] != VEHICLE_OWNER_TYPE_GROUP || Vehicle[vid][vehicle_owner] != Group[gid][group_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd, w którym siê znajdujesz nie nale¿y do Twojej grupy."); 
	
	if( !IsValidDynamicObject(Vehicle[vid][vehicle_buzzer_object]) )
	{
		new Cache:result;
		result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_items_proto` WHERE `value1` = %d AND `value2` = %d", Group[gid][group_uid], GetVehicleModel(vid)));
		
		if( cache_get_rows() == 1 )
		{
			Vehicle[vid][vehicle_buzzer_object] = CreateDynamicObject(18646, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
			AttachDynamicObjectToVehicle(Vehicle[vid][vehicle_buzzer_object], vid, cache_get_float(0, "x"), cache_get_float(0, "y"), cache_get_float(0, "z"), cache_get_float(0, "rx"), cache_get_float(0, "ry"), cache_get_float(0, "rz"));
	
			SendPlayerInformation(playerid, "~g~Kogut zalozony~n~~w~Aby sciagnac koguta z dachu wpisz ponownie /kogut", 4000);
		}
		else
		{
			if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Wyst¹pi³ b³¹d. Poinformuj o nim lidera swojej grupy.");
			
			SendClientMessage(playerid, COLOR_GREEN, "Pozycja koguta dla tego pojazdu nie zosta³a jeszcze ustalona, mo¿esz j¹ ustaliæ edytuj¹c widoczny obiekt.");
			
			new Float:v_pos[3];
			GetVehiclePos(vid, v_pos[0], v_pos[1], v_pos[2]);
			
			pInfo[playerid][player_kogut_object] = CreateDynamicObject(18646, v_pos[0], v_pos[1], v_pos[2], 0.0, 0.0, 0.0, 0);
			pInfo[playerid][player_kogut_vehicle] = vid;
			
			RemovePlayerFromVehicle(playerid);
			
			EditDynamicObject(playerid, pInfo[playerid][player_kogut_object]);
		}
		
		cache_delete(result);
	}
	else
	{
		DestroyDynamicObject(Vehicle[vid][vehicle_buzzer_object]);
		
		SendPlayerInformation(playerid, "~g~Kogut sciagniety~n~~w~Aby zalozyc koguta ponownie wpisz /kogut", 4000);
	}


	return 1;
}

COMMAND:blokada(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_BLOCKADE) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	if( IsPlayerInAnyVehicle(playerid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz u¿ywaæ tej komendy bêd¹c w pojeŸdzie.");
	
	new bslot;
	if( sscanf(params, "d", bslot) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /blokada [1-20]");
	if( bslot < 1 || bslot > 20 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /blokada [1-20]");
	bslot -= 1;
	
	if( IsValidDynamicObject(pInfo[playerid][player_blockades][bslot]) )
	{
		DestroyDynamicObject(pInfo[playerid][player_blockades][bslot]);
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie usun¹³eœ stworzon¹ przez siebie blokadê pod slotem %d.", bslot+1));
	}
	else
	{
		new Float:pos[3];
		GetPointInAngleOfPlayer(playerid, pos[0], pos[1], pos[2], 4.0, 0.0);
		
		pInfo[playerid][player_blockades][bslot] = CreateDynamicObject(3578, pos[0], pos[1], pos[2], 0.0, 0.0, 0.0, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
		SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie utworzy³eœ blokadê pod slotem %d. Aby usun¹æ stworzon¹ blokadê wpisz /blokada %d.", bslot+1, bslot+1));
		
		RefreshPlayer(playerid);
	}
	
	return 1;
}

COMMAND:alkomat(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_CUFFS) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /alkomat [id gracza]");
	if( !IsPlayerConnected(targetid) || IsPlayerNPC(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	new Float:pos[3];
	GetPlayerPos(targetid, pos[0], pos[1], pos[2]);
	
	if( !IsPlayerInRangeOfPoint(playerid, 5.0, pos[0], pos[1], pos[2]) || GetPlayerVirtualWorld(playerid) != GetPlayerVirtualWorld(targetid) || GetPlayerInterior(playerid) != GetPlayerInterior(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest zbyt daleko od Ciebie.");
	
	new Float:promile = floatdiv(pInfo[targetid][player_drunk_time], 1000);
	SendGuiInformation(playerid, "Alkomat", sprintf("Badana osoba w wydychanym powietrzu posiada %.2f promili.", promile));
	
	return 1;
}

COMMAND:wrzuc(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_CUFFS) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");

	new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_CUFFS);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz u¿yæ tej komendy jeœli nikogo nie sku³eœ.");

	new vid = GetPlayerVehicleID(playerid);
	if( vid == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz u¿yæ tej komendy jeœli nie siedzisz w pojeŸdzie");

	PutPlayerInVehicle(Item[itemid][item_value1], vid, 2);
	return 1;
}

COMMAND:zabierzprawko(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_CUFFS) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /zabierzprawko [id gracza]");
	if( !IsPlayerConnected(targetid) || IsPlayerNPC(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
	new Float:pos[3];
	GetPlayerPos(targetid, pos[0], pos[1], pos[2]);
	
	if( !IsPlayerInRangeOfPoint(playerid, 5.0, pos[0], pos[1], pos[2]) || GetPlayerVirtualWorld(playerid) != GetPlayerVirtualWorld(targetid) || GetPlayerInterior(playerid) != GetPlayerInterior(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest zbyt daleko od Ciebie.");

	if( !PlayerHasDocument(targetid, DOCUMENT_DRIVE) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ten gracz nie posiada wyrobionego prawa jazdy.");
	
	pInfo[targetid][player_documents] -= DOCUMENT_DRIVE;
	
	mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_documents = %d WHERE char_uid = %d", pInfo[targetid][player_documents], pInfo[targetid][player_id]));
	
	SendGuiInformation(playerid, "Informacja", sprintf("Pomyœlnie zabra³eœ graczowi %s (%d) prawo jazdy.", pInfo[targetid][player_name], targetid));
	SendGuiInformation(targetid, "Informacja", sprintf("Gracz %s (%d) zabra³ Ci prawo jazdy.", pInfo[playerid][player_name], playerid));
	
	return 1;
}

COMMAND:zalozblokade(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");

	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_CUFFS) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");

	new vid, money;
	if( sscanf(params, "dd", vid, money) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /zalozblokade [id pojazdu] [cena]");
	if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id nie istnieje.");
	if( GetVehicleType(vid) == VEHICLE_TYPE_TRAILER ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿na zak³adaæ blokad na naczepy.");
	if( Vehicle[vid][vehicle_block] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿na zak³adaæ blokady na auto, które ju¿ ma za³o¿on¹ blokadê.");
	if( money < 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Cena zdjêcia blokady nie mo¿e byæ mniejsza od 0.");

	if( GetPlayerDistanceFromVehicle(playerid, vid) > 20 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym id jest zbyt daleko.");

	new a_id = GetPlayerArea(playerid, AREA_TYPE_NORMAL, AREA_OWNER_TYPE_GROUP);
	if( a_id == -1 || Area[a_id][area_owner] != Group[gid][group_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê na terenie parkingu grupowego.");

	SendGuiInformation(playerid, "Informacja", sprintf("Za³o¿y³eœ blokadê na pojazd o UID %d na kwotê $%d.", Vehicle[vid][vehicle_uid], money));
	
	GetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]);
	GetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
	Vehicle[vid][vehicle_park_world] = GetVehicleVirtualWorld(vid);
	Vehicle[vid][vehicle_park_interior] = Vehicle[vid][vehicle_interior];

	Vehicle[vid][vehicle_block] = true;
	Vehicle[vid][vehicle_block_price] = money;
	Vehicle[vid][vehicle_block_group] = Group[gid][group_uid];
	
	mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET vehicle_block = 1, vehicle_blockprice = %d, vehicle_blockgroup = %d, `vehicle_posx` = %f, `vehicle_posy` = %f, `vehicle_posz` = %f, `vehicle_posa` = %f, `vehicle_world` = %d, `vehicle_interior` = %d WHERE `vehicle_uid` = %d", 
		money, Group[gid][group_uid], Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2], Vehicle[vid][vehicle_park][3], Vehicle[vid][vehicle_park_world], Vehicle[vid][vehicle_park_interior], Vehicle[vid][vehicle_uid]));

	
	return 1;
}

COMMAND:opis(playerid, params[])
{
	//TODO: dodac opcjonalny opis po params
	DynamicGui_Init(playerid);
	new string[1000];
	
	if( !isnull(pInfo[playerid][player_description]) )
	{
		new str[60];
		strcopy(str, pInfo[playerid][player_description], 60);

		format(string, sizeof(string), "%s» Aktualny opis: {9A9CCD}%s...\n", string, str);
		DynamicGui_AddRow(playerid, DG_NO_ACTION);
		
		format(string, sizeof(string), "%s» Usuñ opis\n", string);
		DynamicGui_AddRow(playerid, DG_CHAR_DESC_DELETE);
	}
	else
	{
		format(string, sizeof(string), "%s» Ustaw opis\n", string);
		DynamicGui_AddRow(playerid, DG_CHAR_DESC_ADD);
	}

	format(string, sizeof(string), "%s\t\t\n", string);
	DynamicGui_AddBlankRow(playerid);
	format(string, sizeof(string), "%s{A9C4E4}10 ostatnich opisów\n", string);
	DynamicGui_AddBlankRow(playerid);

	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT uid, text FROM `characters_descriptions` WHERE `owner` = %d ORDER BY `last_used` DESC", pInfo[playerid][player_id]));
	
	if( cache_get_rows() > 0 )
	{
		new tmpText[140], text[100];

		for(new i;i<cache_get_rows();i++)
		{
			cache_get(i, "text", tmpText);
			if(strlen(tmpText) > 90)
			{
				strcopy(text, tmpText, 90);
				format(text, sizeof(text), "%s...", text);
			}
			else strcopy(text, tmpText);

			format(string, sizeof(string), "%s  %d.   %s\n", string, i+1, text);
			DynamicGui_AddRow(playerid, DG_CHAR_DESC_OLD, cache_get_int(i, "uid"));
		}
	}
	else 
	{
		format(string, sizeof(string), "%s  Brak zapisanych opisów\n", string);
		DynamicGui_AddBlankRow(playerid);
	}
	
	cache_delete(result);

	ShowPlayerDialog(playerid, DIALOG_CHAR_DESCRIPTION, DIALOG_STYLE_LIST, "Opis postaci", string, "Wybierz", "Zamknij");
	return 1;
}

COMMAND:sms(playerid, params[])
{
	new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz w³¹czonego telefonu.");
	
	new number, text[128];
	if( sscanf(params, "d s[128]", number, text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /sms [numer telefonu] [treœc]");
	if( number == Item[itemid][item_value1] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz wysy³aæ SMS'a do siebie.");
	
	new item = GetItemByData(ITEM_TYPE_PHONE, number);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Telefon o podanym numerze nie istnieje.");
	if( !Item[itemid][item_used] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Telefon o podanym numerze jest poza zasiêgiem.");
	
	new targetid = GetPlayerByUid(Item[item][item_owner]);
	if( targetid == -1 ) return 1;
	
	new tmp_name[MAX_PLAYER_NAME+1], Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT contact_name FROM `crp_contacts` WHERE `contact_owner` = %d AND `contact_number` = %d AND `contact_deleted` = 0", Item[item][item_uid], Item[itemid][item_value1]));	

	if( cache_get_rows() > 0 )
	{
		cache_get(0, "contact_name", tmp_name);
		SendFormattedClientMessage(targetid, COLOR_YELLOW, "(SMS) [%d] %s: %s", Item[itemid][item_value1], tmp_name, text);
	}
	else
	{
		SendFormattedClientMessage(targetid, COLOR_YELLOW, "(SMS) %d: %s", Item[itemid][item_value1], text);
	}
	
	cache_delete(result);

	ProxMessage(targetid, "otrzyma³ SMS'a.", PROX_AME);
	
	new Float:p_pos[3];
	GetPlayerPos(targetid, p_pos[0], p_pos[1], p_pos[2]);
	
	foreach(new p : Player)
	{
		if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 5.0 ) PlayAudioStreamForPlayer(p, "http://angelnaina.mobile9.com/download/content_delivery.php?key=My9zYW1zdW5nZ2FsX3Y3aDhsZW5mLm1wM3wxMzkyNDEyNjYwfA==", p_pos[0], p_pos[1], p_pos[2], 5.0, 1);
	}

	result = mysql_query(g_sql, sprintf("SELECT contact_name FROM `crp_contacts` WHERE `contact_owner` = %d AND `contact_number` = %d AND `contact_deleted` = 0", Item[itemid][item_uid], Item[item][item_value1]));	
	
	if( cache_get_rows() > 0 )
	{
		cache_get(0, "contact_name", tmp_name);
		SendFormattedClientMessage(playerid, COLOR_YELLOW, "> (SMS) [%d] %s: %s", Item[item][item_value1], tmp_name, text);
	}
	else
	{
		SendFormattedClientMessage(targetid, COLOR_YELLOW, "> (SMS) %d: %s", Item[item][item_value1], text);
	}
	
	cache_delete(result);
	
	return 1;
}

COMMAND:reanimuj(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( Group[gid][group_type] != GROUP_TYPE_EMS && Group[gid][group_type] != GROUP_TYPE_FD ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new targetid;
	if( sscanf(params, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /reanimuj [id gracza]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( pInfo[targetid][player_bw] == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ten gracz nie jest nieprzytomny.");
	
	pInfo[targetid][player_bw] = 0;
	pInfo[targetid][player_bw_end_time] = 0;
	mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_bw` = 0 WHERE `char_uid` = %d", pInfo[targetid][player_id]));
	
	PlayerTextDrawHide(targetid, pInfo[targetid][leftTime]);
	RemovePlayerStatus(targetid, PLAYER_STATUS_BW);
	SetPlayerHealth(targetid, 10);
	TogglePlayerControllable(targetid, 1);
	SetCameraBehindPlayer(targetid);
	ClearAnimations(targetid);

	GiveGroupPoints(gid, 2);
	
	return 1;
}

COMMAND:m(playerid, params[])
{
	return cmd_megafon(playerid, params);
}


COMMAND:megafon(playerid, params[])
{
	new itemid = GetPlayerItem(playerid, ITEM_TYPE_MEGAFON);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz przy sobie megafonu.");
	
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(m)egafon [tekst]");
	
	ProxMessage(playerid, params, PROX_MEGAFON);
	
	return 1;
}

COMMAND:911(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ycia tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_911) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ycia tej komendy.");

	new param[16];
	if( !sscanf(params, "s[16]", param) ) 
	{
		if(!strcmp(param, "cancel")) 
		{
			PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);
			DisablePlayerCheckpoint(playerid);

			SendClientMessage(playerid, -1, HEX_COLOR_HONEST"[911]"HEX_COLOR_WHITE" Anulowano przyjazd do zg³oszenia.");

			return 1;
		}
	}

	new Cache:result, str[1024];
	result = mysql_query(g_sql, sprintf("SELECT call_uid, call_date, call_sender, call_content FROM crp_emergency WHERE call_active = true AND call_target = '%d' ORDER BY call_date DESC", gid));	

	if( cache_get_rows() == 0 ) return Alert(playerid, ALERT_TYPE_NEGATIVE, "Brak aktywnych zgloszen dla Twojej grupy.");

	DynamicGui_Init(playerid);

	format(str, sizeof(str), "Data\tNumer\tTreœæ\n");

	for(new i;i<cache_get_rows();i++)
	{
		new uid = cache_get_int(i, "call_uid");
		new sender = cache_get_int(i, "call_sender");
		new content[256], date[64];
		cache_get(i, "call_content", content);
		GetRelativeDate(cache_get_int(i, "call_date"), date);
		
		if(strlen(content) > 35) {
			strdel(content, 35, strlen(content));
			strcat(content, "...");
		}

		format(str, sizeof(str), "%s%s\t%d\t%s\n", str, date, sender, content);
		DynamicGui_AddRow(playerid, uid);
	}

	ShowPlayerDialog(playerid, DIALOG_EMERGENCY, DIALOG_STYLE_TABLIST_HEADERS, "911", str, "Przyjmij", "Zamknij");

	cache_delete(result);

	return 1;
}

COMMAND:call(playerid, params[])
{
	if( pInfo[playerid][player_phone_receiver] != INVALID_PLAYER_ID || pInfo[playerid][player_phone_caller] != INVALID_PLAYER_ID ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Prowadzisz ju¿ jak¹œ rozmowê.");
	new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
	if( itemid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz w³¹czonego telefonu.");
	
	new number;
	if( sscanf(params, "d", number) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /call [numer telefonu]");
	if( number == Item[itemid][item_value1] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz dzwoniæ do siebie.");
	
	if( number == 333 || number == 911 || number == 444 || number == 777 )
	{
		switch(number)
		{
			case 333:
			{
				new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
				if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Aby zamawiaæ produkty musisz znajdowaæ siê w budynku swojej grupy.");
				if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym budynku nie mo¿esz zamawiaæ produktów.");
				if( !CanPlayerEditDoor(playerid, d_id) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do zamawiania produktów w tym budynku.");
				
				SetPlayerCellPhoneVisuals(playerid, true);
				
				new gid = GetGroupByUid(Door[d_id][door_owner]);

				mysql_pquery(g_sql, sprintf("SELECT cp.product_uid, cp.product_listname, cp.product_price FROM crp_products cp LEFT JOIN crp_products_groups cpg ON cpg.product_id = cp.product_uid WHERE FIND_IN_SET('%d', cp.product_grouptype) OR cpg.group_id = %d GROUP BY cp.product_uid ORDER BY cp.product_listname", Group[gid][group_type], Group[gid][group_uid]), "OnGroupProductsListLoaded", "d", playerid);
			}
			
			case 911:
			{
				new str[300];
				DynamicGui_Init(playerid);
				
				foreach(new gid : Groups)
				{
					if( GroupHasFlag(gid, GROUP_FLAG_911) )
					{
						format(str, sizeof(str), "%s[%s]\t\t%s\n", str, GetGroupTag(gid), Group[gid][group_name]);
						DynamicGui_AddRow(playerid, gid);
					}
				}
				
				if( GetPlayerSpecialAction(playerid) != SPECIAL_ACTION_USECELLPHONE ) SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
	
				pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
				if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);
				
				ShowPlayerDialog(playerid, DIALOG_911, DIALOG_STYLE_LIST, "Numery alarmowe", str, "Wybierz", "Anuluj");
				
				return 1;
			}
			
			case 444:
			{
				if( GetPlayerSpecialAction(playerid) != SPECIAL_ACTION_USECELLPHONE ) SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
	
				pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
				if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);
				
				SendClientMessage(playerid, 0x9B91ECFF, "** Dodzwoni³eœ siê na gor¹c¹ liniê telewizyjn¹. Proszê opisaæ dlaczego i sk¹d Pan/Pani dzwoni. (( Centrala ))");
			
				ShowPlayerDialog(playerid, DIALOG_444_REASON, DIALOG_STYLE_INPUT, "Zg³oszenie", "Poni¿ej podaj opis zdarzenia jak i jego miejsce:", "Gotowe", "Anuluj");
				
				return 1;
			}
			
			case 777:
			{
				
			}
		}

		return 1;
	}	
	
	new item = GetItemByData(ITEM_TYPE_PHONE, number);
	if( item == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Telefon o podanym numerze nie istnieje.");
	if( !Item[item][item_used] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Telefon o podanym numerze jest poza zasiêgiem.");
	
	new targetid = GetPlayerByUid(Item[item][item_owner]);
	if( targetid == -1 ) return 1;
	if( pInfo[targetid][player_phone_receiver] != INVALID_PLAYER_ID || pInfo[targetid][player_phone_caller] != INVALID_PLAYER_ID ) return SendClientMessage(playerid, COLOR_GREY, "Nie mo¿na po³¹czyæ siê z tym numerem.");
	
	pInfo[targetid][player_phone_caller] = playerid;
	pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
	
	pInfo[playerid][player_phone_receiver] = targetid;
	pInfo[playerid][player_phone_caller] = INVALID_PLAYER_ID;

	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
	
	pInfo[playerid][player_phone_object_index] = GetPlayerFreeAttachSlot(playerid);
	if( pInfo[playerid][player_phone_object_index] > -1 ) SetPlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index], 330, 6);

	ProxMessage(targetid, sprintf("Telefon %s zaczyna wibrowaæ.", pInfo[targetid][player_name]), PROX_DO);
	
	new tmp_name[MAX_PLAYER_NAME+1], Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT contact_name, contact_uid FROM crp_contacts WHERE contact_owner = %d AND contact_number = %d AND contact_deleted = 0 LIMIT 1", Item[item][item_uid], Item[itemid][item_value1]));	
	
	if( cache_get_rows() > 0 )
	{
		cache_get(0, "contact_name", tmp_name);
		SendFormattedClientMessage(targetid, COLOR_YELLOW, "Po³¹czenie przychodz¹ce od %s (%d). Wpisz /tel odbierz aby odebraæ lub /tel zakoncz aby odrzuciæ po³¹czenie.", tmp_name, Item[itemid][item_value1]);
	}
	else
	{
		SendFormattedClientMessage(targetid, COLOR_YELLOW, "Po³¹czenie przychodz¹ce od %d. Wpisz /tel odbierz aby odebraæ lub /tel zakoncz aby odrzuciæ po³¹czenie.", Item[itemid][item_value1]);
	}
	
	cache_delete(result);
	
	SendClientMessage(playerid, COLOR_YELLOW, "S³ychaæ sygna³ wybierania...");
	
	return 1;
}

COMMAND:praca(playerid, params[])
{
	if( pInfo[playerid][player_job] == 0 ) return SendGuiInformation(playerid, "Informacja", "Nie masz ¿adnej pracy dorwyczej. Aby podj¹æ jak¹œ udaj siê pod urz¹d miasta.");
	
	new sub_cmd[40], param[150];
	sscanf(params, "s[40] S()[150]", sub_cmd, param);
	
	if( isnull(params) )
	{
		
	}
	else
	{
		if( !strcmp(sub_cmd, "opusc") )
		{
			pInfo[playerid][player_job] = 0;
			mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_job` = 0 WHERE `char_uid` = %d", pInfo[playerid][player_id]));
			
			SendGuiInformation(playerid, "Informacja", "Porzuci³eœ swoj¹ prace dorywcz¹. Aby podj¹æ siê innej pracy udaj siê pod urz¹d miasta.");
		}
	}
	
	return 1;
}

COMMAND:tankuj(playerid, params[])
{
	new vid = GetPlayerVehicleID(playerid);
	if( !vid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w ¿adnym pojeŸdzie.");
	if( GetPlayerVehicleSeat(playerid) != 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ kierowc¹ pojazdu, w którym siedzisz.");
	if( Vehicle[vid][vehicle_engine] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Silnik pojazdu musi byæ zgaszony podczas tankowania.");
	
	new a_id = GetPlayerArea(playerid, AREA_TYPE_PETROL);
	if( a_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê na stacji paliw.");
	
	new type[20], amount;
	if( sscanf(params, "s[20] d", type, amount) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tankuj [benzyna/gaz/diesel/lotnicze] [iloœæ litrów]");
	
	new petrol_type = -1;
	if( isnull(type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tankuj [benzyna/gaz/diesel/lotnicze] [iloœæ litrów]");
	
	if( !strcmp(type, "benzyna") ) petrol_type = VEHICLE_FUEL_TYPE_BENZYNA;
	else if( !strcmp(type, "gaz") ) petrol_type = VEHICLE_FUEL_TYPE_GAZ;
	else if( !strcmp(type, "diesel") ) petrol_type = VEHICLE_FUEL_TYPE_DIESEL;
	else if( !strcmp(type, "lotnicze") ) petrol_type = VEHICLE_FUEL_TYPE_LOTNICZE;
	
	if( petrol_type == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tankuj [benzyna/gaz/diesel/lotnicze] [iloœæ litrów]");
	if( amount <= 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tankuj [benzyna/gaz/diesel/lotnicze] [iloœæ litrów]");
	
	if( Vehicle[vid][vehicle_fuel_type] != petrol_type ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Do tego pojazdu nie mo¿esz zatankowaæ tego rodzaju paliwa.");
	if( VehicleFuelMax[GetVehicleModel(vid)-400] - Vehicle[vid][vehicle_fuel_current] < 1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Bak tego pojazdu jest pe³ny.");
	
	if( VehicleFuelMax[GetVehicleModel(vid)-400] - Vehicle[vid][vehicle_fuel_current] < amount ) amount = floatround(VehicleFuelMax[GetVehicleModel(vid)-400] - Vehicle[vid][vehicle_fuel_current], floatround_ceil);
	
	new price = amount;
	if( petrol_type == VEHICLE_FUEL_TYPE_BENZYNA ) price *= 3;
	else if( petrol_type == VEHICLE_FUEL_TYPE_GAZ ) price = floatround(price * 1.5);
	else if( petrol_type == VEHICLE_FUEL_TYPE_DIESEL ) price = floatround(price * 2.5);
	else if( petrol_type == VEHICLE_FUEL_TYPE_LOTNICZE ) price = price * 4;
	
	new resp = SetOffer(INVALID_PLAYER_ID, playerid, OFFER_TYPE_PETROL, price, amount);
	if( resp ) ShowPlayerOffer(playerid, INVALID_PLAYER_ID, "Paliwo", sprintf("%s (%dL)", VehicleFuelTypes[petrol_type], amount), price);
	return 1;
}

COMMAND:yo(playerid, params[])
{
	new style;
	if( sscanf(params, "d", style) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /yo [1-8]");
	if( style < 1 || style > 8 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /yo [1-8]");
	
	pInfo[playerid][player_dialog_tmp1] = style;
	
	DynamicGui_Init(playerid);
	new string[2048], count;
	
	new Float:p_pos[3];
	GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
	
	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		if( p == playerid ) continue;
		if( pInfo[p][player_admin_spec] ) continue;
		if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 3.0 )
		{
			if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASK) > -1 ) format(string, sizeof(string), "%s##\t\t%s\n", string, pInfo[p][player_name]);
			else format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);
			
			DynamicGui_AddRow(playerid, p);
			count++;
		}
	}
	
	if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W pobli¿u nie ma ¿adnych osób.");
	else ShowPlayerDialog(playerid, DIALOG_YO, DIALOG_STYLE_LIST, "Osoby znajduj¹ce siê w pobli¿u:", string, "Wybierz", "Anuluj");
	
	return 1;
}

COMMAND:przejazd(playerid, params[])
{
	new vid = GetPlayerVehicleID(playerid);
	if( !vid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w ¿adnym pojeŸdzie.");
	if( GetPlayerState(playerid) != PLAYER_STATE_DRIVER ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ kierowc¹ pojazdu, w którym siedzisz.");
	
	new a_inner_id = GetPlayerArea(playerid, AREA_TYPE_DOOR_INNER), a_outer_id = GetPlayerArea(playerid, AREA_TYPE_DOOR_OUTER);
			
	if( a_inner_id != -1 )
	{
		new d_id = Area[a_inner_id][area_owner];
		
		if( Door[d_id][door_closed] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Te drzwi s¹ zamkniête.");

		new bool:vehicleOcc[MAX_PLAYERS], vehicleOccSeat[MAX_PLAYERS];
		
		foreach(new p : Player)
		{
			if( GetPlayerVehicleID(p) != vid ) continue;
			
			vehicleOcc[p] = true;
			vehicleOccSeat[p] = GetPlayerVehicleSeat(p);
		}
		
		SetVehicleVirtualWorld(vid, Door[d_id][door_vw]);
		LinkVehicleToInterior(vid, Door[d_id][door_int]);

		SetVehicleVelocity(vid, 0.0, 0.0, 0.0);
		
		for(new i;i<MAX_PLAYERS;i++)
		{
			if( vehicleOcc[i] ) 
			{
				SetPlayerVirtualWorld(i, Door[d_id][door_vw]);
				SetPlayerInterior(i, Door[d_id][door_int]);
				PutPlayerInVehicle(i, vid, vehicleOccSeat[i]);
				FreezePlayer(i, 2500);
			}
		}
		
		defer SetVehicleBound[500](vid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2], Door[d_id][door_pos][3]);
	}
	else if( a_outer_id != -1 )
	{
		new d_id = Area[a_outer_id][area_owner];
		
		if( Door[d_id][door_closed] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Te drzwi s¹ zamkniête.");
		if( !Door[d_id][door_car_crosing] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Te drzwi maj¹ zablokowan¹ mo¿liwoœæ wjazdu pojazdów.");
		
		if( Door[d_id][door_payment] > 0 )
		{
			if( Door[d_id][door_payment] > pInfo[playerid][player_money] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz wystarczaj¹cej iloœci pieniêdzy, aby wejœæ do budynku.");
			
			GivePlayerMoney(playerid, -Door[d_id][door_payment]);
		}
		
		new bool:vehicleOcc[MAX_PLAYERS], vehicleOccSeat[MAX_PLAYERS];
		
		foreach(new p : Player)
		{
			if( GetPlayerVehicleID(p) != vid ) continue;
			
			vehicleOcc[p] = true;
			vehicleOccSeat[p] = GetPlayerVehicleSeat(p);
		}
		
		SetVehicleVirtualWorld(vid, Door[d_id][door_spawn_vw]);
		LinkVehicleToInterior(vid, Door[d_id][door_spawn_int]);

		SetVehicleVelocity(vid, 0.0, 0.0, 0.0);
		
		for(new i;i<MAX_PLAYERS;i++)
		{
			if( vehicleOcc[i] ) 
			{
				SetPlayerVirtualWorld(i, Door[d_id][door_spawn_vw]);
				SetPlayerInterior(i, Door[d_id][door_spawn_int]);
				PutPlayerInVehicle(i, vid, vehicleOccSeat[i]);
				FreezePlayer(i, 2500);
			}
		}
		
		defer SetVehicleBound[500](vid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3]);
	}
	return 1;
}

COMMAND:przetrzymaj(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_KEEP) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e sk³adaæ t¹ ofertê.");
	
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w budynku nale¿¹cym do Twojej grupy.");
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP || Door[d_id][door_owner] != Group[gid][group_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w budynku nale¿¹cym do Twojej grupy.");
	
	new targetid, hours, desc[100];
	if( sscanf(params, "dds[100]", targetid, hours, desc) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /przetrzymaj [id gracza] [czas w godzinach] [powód]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( hours < 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Podana iloœæ godzin jest b³êdna.");
	
	new Float:pos[3];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	if( GetPlayerDistanceFromPoint(targetid, pos[0], pos[1], pos[2]) > 5.0 || GetPlayerVirtualWorld(targetid) != GetPlayerVirtualWorld(playerid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest zbyt daleko.");
	
	pInfo[targetid][player_keep] = gettime() + 60*60*hours;
	
	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(targetid, x, y, z);
	GetPlayerFacingAngle(targetid, a);
	
	mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_posx = '%f', char_posy = '%f', char_posz = '%f', char_posa = '%f', char_world = %d, char_interior = %d, char_arrest = %d WHERE `char_uid`=%d", x, y, z, a, GetPlayerVirtualWorld(targetid), GetPlayerInterior(targetid), pInfo[targetid][player_keep], pInfo[targetid][player_id]));
	//mysql_pquery(g_sql, sprintf("INSERT INTO crp_files VALUES (null, %d, %d, %d, %d, %d, %d, %d, '%s')", FILES_TYPE_KEEP, FILES_OWNER_TYPE_PLAYER, pInfo[targetid][player_id], pInfo[playerid][player_id], gettime(), 0, 0, desc));
	
	SendGuiInformation(playerid, "Informacja", sprintf("Przetrzymujesz gracza %s (%d) w budynku %s przez %dh, powód: %s.", pInfo[targetid][player_name], targetid, Door[d_id][door_name], hours, desc));
	SendGuiInformation(targetid, "Informacja", sprintf("Jesteœ przetrzymywany w budynku %s przez %dh, powód: %s.", Door[d_id][door_name], hours, desc));
	
	return 1;
}

COMMAND:aresztuj(playerid, params[])
{
	return cmd_przetrzymaj(playerid, params);
}

COMMAND:pokoj(playerid, params[])
{
	if( GetPlayerVirtualWorld(playerid) == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do hotelu.");
	
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do hotelu.");
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do hotelu.");
	
	new gid = GetGroupByUid(Door[d_id][door_owner]);
	if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do hotelu.");
	if( Group[gid][group_type] != GROUP_TYPE_HOTEL ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do hotelu.");
	
	
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pokoj [zamelduj/wymelduj/wejdz]");
	
	if( !strcmp(params, "zamelduj") )
	{
		new time = GetPlayerOnlineTime(playerid), hour = floatround(time/3600, floatround_floor);
		if( hour > 40 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz wynaj¹æ pokoju w hotelu maj¹c przegrane ponad 20h. Poszukaj jakiegoœ przytulnego mieszkanka.");
		
		new did = GetDoorByUid(pInfo[playerid][player_door]);
		if( did > -1 )
		{
			if( Door[did][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
			{
				new g_id = GetGroupByUid(Door[did][door_owner]);
				if( g_id > -1 )
				{
					if( Group[g_id][group_type] == GROUP_TYPE_HOTEL )
					{
						return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Jesteœ ju¿ zameldowany w hotelu.");
					}
				}
			}
		}
		
		pInfo[playerid][player_door] = Door[d_id][door_uid];
		mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_door = %d WHERE char_uid = %d", pInfo[playerid][player_door], pInfo[playerid][player_id]));
		
		SendGuiInformation(playerid, "Informacja", "Pomyœlnie zameldowa³eœ siê w hotelu.\n\nZ Twojego konta za ka¿dym razem gdy wejdziesz po raz pierwszy w danym dniu do gry zostanie pobrana op³ata w wysokoœci $20.");
	}
	else if( !strcmp(params, "wymelduj") )
	{
		new did = GetDoorByUid(pInfo[playerid][player_door]);
		if( did > -1 )
		{
			if( did == d_id )
			{
				pInfo[playerid][player_door] = 0;
				mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_door = %d WHERE char_uid = %d", pInfo[playerid][player_door], pInfo[playerid][player_id]));
				
				return SendGuiInformation(playerid, "Informacja", "Wymeldowa³eœ siê z hotelu. Od teraz bêdziesz spawnowaæ siê w centrum b¹dŸ na lotnisku.");
			}
		}
		
		return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ zameldowany w tym hotelu.");
	}
	else if( !strcmp(params, "wejdz") )
	{	
		new did = GetDoorByUid(pInfo[playerid][player_door]);
		if( did > -1 )
		{
			if( did == d_id )
			{
				new g_id = GetGroupByUid(Door[did][door_owner]);
				if( Group[g_id][group_value1] == 0 )
				{
					SetPlayerInterior(playerid, 2);
					new vw[7];
					format(vw, sizeof(vw), "20%04d", pInfo[playerid][player_id]);
					SetPlayerVirtualWorld(playerid, strval(vw));				
					SetPlayerPos(playerid, 271.884979, 306.631988, 999.148437);
					SetPlayerFacingAngle(playerid, 0.0);
					
					defer BuggedHotel[100](playerid);
				}
				else
				{
					SetPlayerInterior(playerid, 15);
					new vw[7];
					format(vw, sizeof(vw), "20%04d", pInfo[playerid][player_id]);
					SetPlayerVirtualWorld(playerid, strval(vw));				
					SetPlayerPos(playerid,  235.6946, -1168.8226, 1029.8043);
					SetPlayerFacingAngle(playerid, 273.1100);
					
					defer BuggedHotel[100](playerid);
				}
			}
		}
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /pokoj [zamelduj/wymelduj/wejdz]");
	
	
	return 1;
}

COMMAND:d(playerid, params[])
{
	return cmd_departament(playerid, params);
}

COMMAND:departament(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_DEP) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	if( Group[gid][group_type] == GROUP_TYPE_RADIO ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Twoja grupa mo¿e jedynie odbieraæ kana³ departamentu.");
	
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(d)epartament [tekst]");
	
	new text[200];
	format(text, sizeof(text), "* Dyspozytor %s: %s *", GetGroupTag(gid), BeautifyString(params, true, true, false));
	
	ProxMessage(playerid, BeautifyString(params, true, true, false), PROX_RADIO);
	
	foreach(new p : Player)
	{
		slot = GetPlayerDutySlot(p);
		if( slot == -1 ) continue;
		if( !GroupHasFlag(pGroup[p][slot][pg_id], GROUP_FLAG_DEP) ) continue;
		
		SendClientMessage(p, 0x6AA121FF, text);
	}
	
	return 1;
}

COMMAND:v(playerid, params[])
{
	return cmd_pojazd(playerid, params);
}

COMMAND:pojazd(playerid, params[])
{	
	if( IsPlayerInAnyVehicle(playerid) )
	{
		new vid = GetPlayerVehicleID(playerid);
		
		new sub_cmd[40], param[150];
		sscanf(params, "s[40] S()[150]", sub_cmd, param);
		
		if( !CanPlayerUseVehicle(playerid, vid) )
		{
			if( isnull(sub_cmd) )
			{
				SendClientMessage(playerid, COLOR_GREY, "Tip: /pojazd [namierz, zaparkuj, tuning, przypisz, info, opis]");
				
				new str[400];
				
				new Cache:result;
				result = mysql_query(g_sql, sprintf("SELECT vehicle_uid, vehicle_model FROM `crp_vehicles` WHERE `vehicle_ownertype` = %d AND `vehicle_owner` = %d", VEHICLE_OWNER_TYPE_PLAYER, pInfo[playerid][player_id]));
				
				if( cache_get_rows() == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz zadnych prywatnych pojazdow.");
				else
				{
					DynamicGui_Init(playerid);
					for(new i;i<cache_get_rows();i++)
					{
						new uid = cache_get_int(i, "vehicle_uid");
						
						// Wszystkie pojazdy gracza dodajemy do listy
						format(str, sizeof(str), "%s%s%d\t\t%s\n", str, ((GetVehicleByUid(uid) != INVALID_VEHICLE_ID) ? (HEX_COLOR_GREEN) : (HEX_COLOR_WHITE)), uid, VehicleNames[cache_get_int(i, "vehicle_model")-400]);
						DynamicGui_AddRow(playerid, uid);
					}
				
					ShowPlayerDialog(playerid, DIALOG_PLAYER_VEHICLES, DIALOG_STYLE_LIST, "Pojazdy nale¿¹ce do Ciebie:", str, "(Un)spawn", "Zamknij");
				}
				
				cache_delete(result);

			}
			else
			{
				if( !strcmp(sub_cmd, "namierz", false) )
				{
					if( !strcmp(param, "off", false) && !isnull(param) )
					{
						if( pInfo[playerid][player_vehicle_target] > -1 )
						{
							Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
							Streamer_UpdateEx(playerid, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][0], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][1], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][2]);
							
							SendClientMessage(playerid, COLOR_GOLD, "Namierzanie zosta³o wy³¹czone.");
							pInfo[playerid][player_vehicle_target] = -1;
						}
						
						return 1;
					}
					
					new v_uid;
					if( sscanf(param, "d", v_uid) ) return SendClientMessage(playerid, COLOR_GREY, "U¿ycie: /v namierz [uid pojazdu]");
					
					vid = GetVehicleByUid(v_uid);
					if( vid == INVALID_VEHICLE_ID ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid nie istnieje lub nie jest zespawnowany.");
					
					if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania pojazdu o podanym uid.");
					
					// Jesli juz jakies namierzanie jest wlaczone to je wylaczamy
					if( pInfo[playerid][player_vehicle_target] > -1 )
					{
						Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
						
						Streamer_UpdateEx(playerid, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][0], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][1], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][2]);
					}
					
					pInfo[playerid][player_vehicle_target] = vid;
					Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
					
					new other_vw[60];
					if( GetVehicleVirtualWorld(vid) > 0 ) format(other_vw, sizeof(other_vw), "\n\n"HEX_COLOR_LIGHTER_RED"Ten pojazd znajduje siê na virtualworld %d!!", GetVehicleVirtualWorld(vid));
					
					SendGuiInformation(playerid, "Informacja", sprintf("Pojazd zosta³ oznaczony na radarze.\nAby wy³¹czyæ namierzanie wejdŸ do pojazdu lub wpisz /v namierz off.%s", other_vw));
				}
				else if( !strcmp(sub_cmd, "pasy") )
				{
					if( !pInfo[playerid][player_belt] )
					{
						AddPlayerStatus(playerid, PLAYER_STATUS_BELT);
						pInfo[playerid][player_belt] = true;
						
						SendPlayerInformation(playerid, "~y~Zapiales pasy.~n~~w~Dzieki temu podczas wypadku bedziesz bezpieczniejszy.");
					}
					else
					{
						RemovePlayerStatus(playerid, PLAYER_STATUS_BELT);
						pInfo[playerid][player_belt] = false;
						
						SendPlayerInformation(playerid, "~y~Odpiales pasy.");
					}
				}
			}
		}
		else
		{
			if( isnull(sub_cmd) )
			{
				// Sprawdzamy czy jest kierowca
				if( GetPlayerVehicleSeat(playerid) != 0 ) return 1;
				
				new header[80];
				format(header, sizeof(header), "%s (UID: %d, ID: %d)", VehicleNames[Vehicle[vid][vehicle_model]-400], Vehicle[vid][vehicle_uid], vid);
				
				DynamicGui_Init(playerid);
				
				new str[600];			
				format(str, sizeof(str), "%sMaska:\t\t\t%s\n", str, (Vehicle[vid][vehicle_bonnet]==false) ? (""HEX_COLOR_LIGHTER_RED"[Zamkniêta]") : (""HEX_COLOR_LIGHTER_GREEN"[Otwarta]"));
				DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_BONNET);
				format(str, sizeof(str), "%sBaga¿nik:\t\t%s\n", str, (Vehicle[vid][vehicle_boot]==false) ? (""HEX_COLOR_LIGHTER_RED"[Zamkniêty]") : (""HEX_COLOR_LIGHTER_GREEN"[Otwarty]"));
				DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_BOOT);
				format(str, sizeof(str), "%sŒwiat³a:\t\t\t%s\n", str, (Vehicle[vid][vehicle_lights]==false) ? (""HEX_COLOR_LIGHTER_RED"[Wy³¹czone]") : (""HEX_COLOR_LIGHTER_GREEN"[W³¹czone]"));
				DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_LIGHTS);
				
				format(str, sizeof(str), "%s-----------------\n", str);
				DynamicGui_AddBlankRow(playerid);
				
				format(str, sizeof(str), "%sRadio:\t\t\t%s\n", str, (Vehicle[vid][vehicle_radio]==false) ? (""HEX_COLOR_LIGHTER_RED"[Wy³¹czone]") : (""HEX_COLOR_LIGHTER_GREEN"[W³¹czone]"));
				DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_RADIO);
				
				new cdid = -1;
				
				foreach(new itid : Items)
				{
					if( Item[itid][item_owner_type] == ITEM_OWNER_TYPE_VEHICLE_COMPONENT && Item[itid][item_owner] == Vehicle[vid][vehicle_uid] && Item[itid][item_type] == ITEM_TYPE_CD )
					{
						cdid = itid;
						break;
					}
				}
				
				if( cdid > -1 )
				{
					format(str, sizeof(str), "%sWyci¹gnij p³ytê\n", str);
					DynamicGui_AddRow(playerid, DG_PLAYER_VEHICLE_PANEL_CDOUT);
				}
				
				format(str, sizeof(str), "%sDodatki:\t\t%sImmobiliser, ", str, (VehicleHasAccessory(vid, VEHICLE_ACCESSORY_IMMOBILISER)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_GREY));
				format(str, sizeof(str), "%s%sAlarm, ", str, (VehicleHasAccessory(vid, VEHICLE_ACCESSORY_ALARM)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_GREY));
				format(str, sizeof(str), "%s%sAudio, ", str, (VehicleHasAccessory(vid, VEHICLE_ACCESSORY_AUDIO)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_GREY));
				format(str, sizeof(str), "%s%sPrzyciemnione szyby\n", str, (VehicleHasAccessory(vid, VEHICLE_ACCESSORY_WINDOWS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_GREY));
				DynamicGui_AddBlankRow(playerid);
				
				ShowPlayerDialog(playerid, DIALOG_PLAYER_VEHICLE_PANEL, DIALOG_STYLE_LIST, header, str, "Wybierz", "Zamknij");
			}
			else
			{
				if( !strcmp(sub_cmd, "z") || !strcmp(sub_cmd, "zamknij")  )
				{
					if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania pojazdu, w którym siedzisz.");
					
					if( Vehicle[vid][vehicle_locked] )
					{
						Vehicle[vid][vehicle_locked] = false;
						GameTextForPlayer(playerid, "~w~Pojazd ~g~otwarty", 3000, 3);
					}
					else
					{
						Vehicle[vid][vehicle_locked] = true;
						GameTextForPlayer(playerid, "~w~Pojazd ~r~zamkniety", 3000, 3);
					}
				}
				else if( !strcmp(sub_cmd, "zaparkuj", false) )
				{
					// Sprawdzamy czy jest kierowca
					if( GetPlayerState(playerid) != PLAYER_STATE_DRIVER ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ kierowc¹ tego pojazdu.");
				
					if( !CanPlayerEditVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do edycji tego pojazdu.");
					
					new Float:vehiclePos[4];
					GetVehiclePos(vid, vehiclePos[0], vehiclePos[1], vehiclePos[2]);
					GetVehicleZAngle(vid, vehiclePos[3]);
					
					new Cache:result;
					result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_vehicles` WHERE `vehicle_posx` >= %f AND `vehicle_posx` <= %f AND `vehicle_posy` >= %f AND `vehicle_posy` <= %f AND `vehicle_world` = %d AND `vehicle_interior` = %d AND `vehicle_uid` <> %d", vehiclePos[0]-4.0, vehiclePos[0]+4.0, vehiclePos[1]-4.0, vehiclePos[1]+4.0, GetVehicleVirtualWorld(vid), Vehicle[vid][vehicle_interior], Vehicle[vid][vehicle_uid]));

					new rows = cache_get_rows();
					
					cache_delete(result);
					
					if( rows > 0 )
					{
						SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu jest juz zaparkowany inny pojazd.");
						
					}
					else
					{
						GetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]);
						GetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
						Vehicle[vid][vehicle_park_world] = GetVehicleVirtualWorld(vid);
						Vehicle[vid][vehicle_park_interior] = Vehicle[vid][vehicle_interior];
						
						mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_posx` = %f, `vehicle_posy` = %f, `vehicle_posz` = %f, `vehicle_posa` = %f, `vehicle_world` = %d, `vehicle_interior` = %d WHERE `vehicle_uid` = %d", Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2], Vehicle[vid][vehicle_park][3], Vehicle[vid][vehicle_park_world], Vehicle[vid][vehicle_park_interior], Vehicle[vid][vehicle_uid]));
						
						SendGuiInformation(playerid, "Informacja", sprintf("Pojazd %s (UID: %d) zosta³ pomyœlnie zaparkowany.", VehicleNames[Vehicle[vid][vehicle_model]-400], Vehicle[vid][vehicle_uid], vid));
					}
				}
				else if( !strcmp(sub_cmd, "pasy") )
				{
					if( !pInfo[playerid][player_belt] )
					{
						AddPlayerStatus(playerid, PLAYER_STATUS_BELT);
						pInfo[playerid][player_belt] = true;
						
						SendPlayerInformation(playerid, "~y~Zapiales pasy.~n~~w~Dzieki temu podczas wypadku bedziesz bezpieczniejszy.");
					}
					else
					{
						RemovePlayerStatus(playerid, PLAYER_STATUS_BELT);
						pInfo[playerid][player_belt] = false;
						
						SendPlayerInformation(playerid, "~y~Odpiales pasy.");
					}
				}
				else if( !strcmp(sub_cmd, "opis") )
				{
					if( isnull(param) && !Vehicle[vid][vehicle_has_description] ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pojazd opis [tekst]");
					else if( isnull(param) && Vehicle[vid][vehicle_has_description] )
					{
						UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_description], LABEL_DESCRIPTION, "");
						Vehicle[vid][vehicle_has_description] = false;
						SendGuiInformation(playerid, "Informacja", sprintf("Usun¹³eœ opis swojego pojazdu %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid]));
						return 1;
					}
					
					UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_description], LABEL_DESCRIPTION, BreakLines(param, "\n", 32));
					Vehicle[vid][vehicle_has_description] = true;
					SendGuiInformation(playerid, "Informacja", sprintf("Ustawi³eœ opis swojego pojazdu %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid]));
				}
				else if( !strcmp(sub_cmd, "przypisz") )
				{
					new gslot;
					if( sscanf(param, "d", gslot) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pojazd przypisz [slot grupy]");
					if( gslot < 1 || gslot > 5 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pojazd przypisz [slot grupy]");
					if( pGroup[playerid][gslot-1][pg_id] == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Na tym slocie nie masz ¿adnej grupy.");
					
					new gid = pGroup[playerid][gslot-1][pg_id];
					Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_GROUP;
					Vehicle[vid][vehicle_owner] = Group[gid][group_uid];
					mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_GROUP, Group[gid][group_uid], Vehicle[vid][vehicle_uid]));
					
					SendGuiInformation(playerid, "Informacja", sprintf("Przepisa³eœ swój pojazd %s [UID: %d] na grupê %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], Group[gid][group_name], Group[gid][group_uid]));
				}
				else if( !strcmp(sub_cmd, "info") )
				{
					if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania tego pojazdu.");
					
					new str[600];
					
					format(str, sizeof(str), "%sModel:\t\t\t%d\n", str, GetVehicleModel(vid));
					format(str, sizeof(str), "%sKolory:\t\t\t%d:%d\n", str, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);
					format(str, sizeof(str), "%sPaliwo:\t\t\t%d/%dL\n", str, floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil), VehicleFuelMax[GetVehicleModel(vid)-400]);
					format(str, sizeof(str), "%sRodzaj paliwa:\t\t%s\n", str, VehicleFuelTypes[Vehicle[vid][vehicle_fuel_type]]);
					format(str, sizeof(str), "%sStan techniczny:\t%.1f HP\n", str, Vehicle[vid][vehicle_health]);
					format(str, sizeof(str), "%sPrzebieg:\t\t%d km\n", str, floatround(Vehicle[vid][vehicle_mileage], floatround_floor));
					if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_PLAYER )
					{
						format(str, sizeof(str), "%sTyp w³aœciciela:\t\tGracz\n", str);
						format(str, sizeof(str), "%sUID gracza:\t\t%d\n", str, Vehicle[vid][vehicle_owner]);
					}
					else if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP )
					{
						format(str, sizeof(str), "%sTyp w³aœciciela:\t\tGrupa\n", str);
						format(str, sizeof(str), "%sUID grupy:\t\t%d\n", str, Vehicle[vid][vehicle_owner]);
					}
					else
					{
						format(str, sizeof(str), "%sTyp w³aœciciela:\t\tBrak\n", str);
					}
					format(str, sizeof(str), "%sRejestracja:\t\t%s\n", str, Vehicle[vid][vehicle_register]);
					format(str, sizeof(str), "%sPojemnoœc silnika:\t%.1f cm3\n\n", str, VehicleEngineCapacity[GetVehicleModel(vid)-400]*1000);
					format(str, sizeof(str), "%sParking:\n", str);
					format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"X:"HEX_COLOR_WHITE"\t\t%.4f\n", str, Vehicle[vid][vehicle_park][0]);
					format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"Y:"HEX_COLOR_WHITE"\t\t%.4f\n", str, Vehicle[vid][vehicle_park][1]);
					format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"Z:"HEX_COLOR_WHITE"\t\t%.4f\n", str, Vehicle[vid][vehicle_park][2]);
					format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"A:"HEX_COLOR_WHITE"\t\t%.4f\n", str, Vehicle[vid][vehicle_park][3]);
					format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"VW:"HEX_COLOR_WHITE"\t\t%d\n", str, Vehicle[vid][vehicle_park_world]);
					format(str, sizeof(str), "%s\t"HEX_COLOR_SAMP"INT:"HEX_COLOR_WHITE"\t\t%d\n", str, Vehicle[vid][vehicle_park_interior]);
					
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("%s (SampID: %d, UID: %d) » Informacje", VehicleNames[GetVehicleModel(vid)-400], vid, Vehicle[vid][vehicle_uid]), str, "OK", "");
				}
			}
		}
	}
	else
	{
		new sub_cmd[40], param[150];
		sscanf(params, "s[40] S()[150]", sub_cmd, param);
		
		if( isnull(sub_cmd) )
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /pojazd [namierz, zaparkuj, tuning, przypisz, zamknij, info, opis]");
			
			new str[400];
			
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT vehicle_uid, vehicle_model FROM `crp_vehicles` WHERE `vehicle_ownertype` = %d AND `vehicle_owner` = %d", VEHICLE_OWNER_TYPE_PLAYER, pInfo[playerid][player_id]));
			
			if( cache_get_rows() == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz zadnych prywatnych pojazdow.");
			else
			{
				DynamicGui_Init(playerid);
				for(new i;i<cache_get_rows();i++)
				{
					new uid = cache_get_int(i, "vehicle_uid");
					
					// Wszystkie pojazdy gracza dodajemy do listy
					format(str, sizeof(str), "%s%s%d\t\t%s\n", str, ((GetVehicleByUid(uid) != INVALID_VEHICLE_ID) ? (HEX_COLOR_GREEN) : (HEX_COLOR_WHITE)), uid, VehicleNames[cache_get_int(i, "vehicle_model")-400]);
					DynamicGui_AddRow(playerid, uid);
				}
			
				ShowPlayerDialog(playerid, DIALOG_PLAYER_VEHICLES, DIALOG_STYLE_LIST, "Pojazdy nale¿¹ce do Ciebie:", str, "(Un)spawn", "Zamknij");
			}
			
			cache_delete(result);

		}
		else
		{
			if( !strcmp(sub_cmd, "z") || !strcmp(sub_cmd, "zamknij")  )
			{
				new v_id = GetNearestPlayerVehicle(playerid);
				if( v_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W pobli¿u nie ma ¿adnego do którego masz uprawnienia.");
				
				if( Vehicle[v_id][vehicle_locked] )
				{
					Vehicle[v_id][vehicle_locked] = false;
					GameTextForPlayer(playerid, "~w~Pojazd ~g~otwarty", 3000, 3);
				}
				else
				{
					Vehicle[v_id][vehicle_locked] = true;
					GameTextForPlayer(playerid, "~w~Pojazd ~r~zamkniety", 3000, 3);
				}
				
				if( !VehicleHasAccessory(v_id, VEHICLE_ACCESSORY_ALARM) )
				{
					ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.1, 0, 0, 0, 0, 0, 1);
				}
				else
				{
					foreach(new p : Player)
					{
						if( GetPlayerDistanceFromPoint(p, Vehicle[v_id][vehicle_last_pos][0], Vehicle[v_id][vehicle_last_pos][1], Vehicle[v_id][vehicle_last_pos][2]) <= 20.0 ) PlayAudioStreamForPlayer(p, "http://iphonegu.com/wp-content/uploads/2012/09/car_lock.mp3", Vehicle[v_id][vehicle_last_pos][0], Vehicle[v_id][vehicle_last_pos][1], Vehicle[v_id][vehicle_last_pos][2], 20.0, 1);
					}
					
					ApplyAnimation(playerid, "HEIST9", "Use_SwipeCard", 4.1, 0, 0, 0, 0, 0, 1);
				}
			}
			else if( !strcmp(sub_cmd, "namierz", false) )
			{
				if( !strcmp(param, "off", false) && !isnull(param) )
				{
					if( pInfo[playerid][player_vehicle_target] > -1 )
					{
						Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
						Streamer_UpdateEx(playerid, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][0], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][1], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][2]);
						
						SendClientMessage(playerid, COLOR_GOLD, "Namierzanie zosta³o wy³¹czone.");
						pInfo[playerid][player_vehicle_target] = -1;
					}
					
					return 1;
				}
				
				new v_uid;
				if( sscanf(param, "d", v_uid) ) return SendClientMessage(playerid, COLOR_GREY, "U¿ycie: /v namierz [uid pojazdu]");
				
				new vid = GetVehicleByUid(v_uid);
				if( vid == INVALID_VEHICLE_ID ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd o podanym uid nie istnieje lub nie jest zespawnowany.");
				
				if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania pojazdu o podanym uid.");
				
				// Jesli juz jakies namierzanie jest wlaczone to je wylaczamy
				if( pInfo[playerid][player_vehicle_target] > -1 )
				{
					Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
					
					Streamer_UpdateEx(playerid, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][0], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][1], Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_last_pos][2]);
				}
				
				pInfo[playerid][player_vehicle_target] = vid;
				Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[pInfo[playerid][player_vehicle_target]][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
				
				RefreshPlayer(playerid);
				
				new other_vw[60];
				if( GetVehicleVirtualWorld(vid) > 0 ) format(other_vw, sizeof(other_vw), "\n\n"HEX_COLOR_LIGHTER_RED"Ten pojazd znajduje siê na virtualworld %d!!", GetVehicleVirtualWorld(vid));
				
				SendGuiInformation(playerid, "Informacja", sprintf("Pojazd zosta³ oznaczony na radarze.\nAby wy³¹czyæ namierzanie wejdŸ do pojazdu lub wpisz /v namierz off.%s", other_vw));
			}
		}
	}
	return 1;
}

COMMAND:heal(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( Group[gid][group_type] != GROUP_TYPE_EMS && Group[gid][group_type] != GROUP_TYPE_FD ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new targetid;
	if( sscanf(params, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /heal [id gracza]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	if( !pInfo[targetid][player_examined] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ten gracz nie zosta³ najpierw przebadany! Zrób to przy u¿yciu przedmiotu 'Badanie'!");
	if( pInfo[targetid][player_damage] == DAMAGE_LEVEL_NONE) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ten gracz jest zdrowy!");
	if( pInfo[targetid][player_damage] == DAMAGE_LEVEL_CRITICAL) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ten gracz jest w stanie krytycznym! Najpierw u¿yj adrenaliny!");

	new price = pInfo[targetid][player_hospitalization_costs];

	switch(pInfo[targetid][player_insurance])
	{
		case INSURANCE_TYPE_SMART:
		{
			price = floatround(price*0.70);
		}
		case INSURANCE_TYPE_STD:
		{
			price = floatround(price*0.25);
		}
		case INSURANCE_TYPE_PRO:
		{
			price = 0;
		}
	}

	if(price > pInfo[targetid][player_bank_money])
	{
		if(pInfo[targetid][player_bank_money] > 0)
		{
			AddPlayerBankMoney(targetid, -pInfo[targetid][player_bank_money], "Hospitalizacja");
			AddPlayerDebit(targetid, price - pInfo[targetid][player_bank_money], "Brak srodkow na hospitalizacje");
		}
		else AddPlayerDebit(targetid, price, "Brak srodkow na hospitalizacje");

		Alert(targetid, ALERT_TYPE_INFO, sprintf("Nie posiadales pieniedzy na oplacenie leczenia. Obciazono twoj rachunek kwota ~r~$%d~w~.", price));
	}
	else
	{
		AddPlayerBankMoney(targetid, -pInfo[targetid][player_bank_money], "Hospitalizacja");
		Alert(playerid, ALERT_TYPE_INFO, sprintf("Pobrano ~r~$%d ~w~za hospitalizacje z konta bankowego.", price));
	}

	pInfo[targetid][player_damage] = DAMAGE_LEVEL_NONE;
	pInfo[targetid][player_hospitalization_costs] = 0;
	mysql_tquery(g_sql, sprintf("UPDATE crp_characters SET char_damage = %d, char_hospitalization_costs = %d WHERE char_uid = %d", pInfo[targetid][player_damage], pInfo[targetid][player_hospitalization_costs], pInfo[targetid][player_id]));

	SendGuiInformation(playerid, "Sukces", "Wyleczono gracza.");

	return 1;
}

COMMAND:test(playerid, params[])
{
	Alert(playerid, ALERT_TYPE_WARNING, "Testowe ostrzezenie");
	Alert(playerid, ALERT_TYPE_NEGATIVE, "Testowe powiadomienie negatywne");
	Alert(playerid, ALERT_TYPE_SUCCESS, "Testowe powiadomienie pozytywne");
	Alert(playerid, ALERT_TYPE_INFO, "Testowa informacja");

	return 1;
}

COMMAND:stats(playerid, params[])
{
	new targetid;
	if( !sscanf(params, "d", targetid) )
	{
		if( GetPlayerPermissions(playerid, 4) ) return SendGuiInformation(playerid, "Informacja", "Nie masz uprawnieñ do korzystania z tej komendy.");

		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	}
	else
	{
		targetid = playerid;
	}
	
	new str[1000];
	DynamicGui_Init(playerid);

	new time = GetPlayerOnlineTime(targetid), hour = floatround(time/3600, floatround_floor);
	time -= hour * 3600;
	new minute = floatround(time/60, floatround_floor);

	format(str, sizeof(str), "Online:\t     %dh %dm\n", hour, minute);

	new name[MAX_PLAYER_NAME];
	format(name, sizeof(name), pInfo[playerid][player_name]);
	UnderscoreToSpace(name);

	format(str, sizeof(str), "%sDane postaci:\t     %s\n", str, name);
	DynamicGui_AddBlankRow(playerid);

	if(pInfo[playerid][player_health] < 25.0) format(str, sizeof(str), "%sZdrowie:\t     {ed6161}%.2f HP\n", str, pInfo[targetid][player_health]);
	else if(pInfo[playerid][player_health] < 75.0) format(str, sizeof(str), "%sZdrowie:\t     {f99e48}%.2f HP\n", str, pInfo[targetid][player_health]);
	else format(str, sizeof(str), "%sZdrowie:\t     {c8e9c7}%.2f HP\n", str, pInfo[targetid][player_health]);

	DynamicGui_AddBlankRow(playerid);

	format(str, sizeof(str), "%sGotówka:\t     {5bc06b}$%d\n", str, pInfo[targetid][player_money]);
	DynamicGui_AddBlankRow(playerid);

	format(str, sizeof(str), "%sKonto bankowe:\t     {5bc06b}$%d{FFFFFF}, %d, debet: {ed6161}$%d\n", str, pInfo[targetid][player_bank_money], pInfo[targetid][player_bank_number], pInfo[targetid][player_debit]);
	DynamicGui_AddBlankRow(playerid);

	format(str, sizeof(str), "%sSkin:\t     %d\n", str, pInfo[targetid][player_skin]);
	DynamicGui_AddBlankRow(playerid);

	format(str, sizeof(str), "%sBW:\t     %dmin\n", str, floatround(pInfo[targetid][player_bw]/60));
	DynamicGui_AddBlankRow(playerid);

	format(str, sizeof(str), "%sAJ:\t     %dmin\n", str, floatround(pInfo[targetid][player_aj]/60));
	DynamicGui_AddBlankRow(playerid);
	
	new insurance[16];
	switch(pInfo[playerid][player_insurance])
	{
		case INSURANCE_TYPE_NONE: format(insurance, 16, "Brak");
		case INSURANCE_TYPE_SMART: format(insurance, 16, "Smart");
		case INSURANCE_TYPE_STD: format(insurance, 16, "Standard");
		case INSURANCE_TYPE_PRO: format(insurance, 16, "Pro");
	}

	format(str, sizeof(str), "%sUbezpieczenie:\t     %s\n", str, insurance);
	DynamicGui_AddBlankRow(playerid);

	if( pInfo[targetid][player_keep] > gettime() ) {
		new ktime = pInfo[targetid][player_keep] - gettime(), khour = floatround(ktime/3600, floatround_floor);
		ktime -= khour * 3600;
		new kminute = floatround(ktime/60, floatround_floor);
		
		new keep[16];
		format(keep, sizeof(keep), "%02dh %02dm", khour, kminute);
		
		format(str, sizeof(str), "%sPrzetrzymanie:\t     %s\n", str, keep);
		
		DynamicGui_AddBlankRow(playerid);
	}

	format(str, sizeof(str), "%s-\n", str);
	DynamicGui_AddBlankRow(playerid);

	format(str, sizeof(str), "%sP³eæ:\t     %s\n", str, pInfo[targetid][player_sex] ? ("Mê¿czyzna") : ("Kobieta"));
	DynamicGui_AddBlankRow(playerid);

	new phone = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
	format(str, sizeof(str), "%sNumer telefonu:\t     %s\n", str, (phone != -1) ? (sprintf("%d", Item[phone][item_value1])) : ("Brak telefonu"));
	DynamicGui_AddBlankRow(playerid);

	new area[80], a_id = GetPlayerArea(targetid, AREA_TYPE_NORMAL);
	if( a_id > -1 ) format(area, sizeof(area), "(ID: %d, UID: %d)", a_id, Area[a_id][area_uid]);
	else strcopy(area, "---");
	format(str, sizeof(str), "%sStrefa:\t     %s\n", str, area);
	DynamicGui_AddBlankRow(playerid);

	format(str, sizeof(str), "%sSi³a:\t     %dj\n", str, pInfo[targetid][player_strength]);
	DynamicGui_AddBlankRow(playerid);

	if(targetid == playerid)
	{
		format(str, sizeof(str), "%s-\n", str);
		DynamicGui_AddBlankRow(playerid);

		format(str, sizeof(str), "%s1\t     Styl rozmowy: %s\n", str, TalkStyleName[pInfo[targetid][player_talk_style]]);
		DynamicGui_AddRow(playerid, DG_STATS_TALK_STYLE);

		format(str, sizeof(str), "%s2\t     Styl chodzenia: %s\n", str, WalkStyleName[pInfo[targetid][player_walk_style]]);
		DynamicGui_AddRow(playerid, DG_STATS_WALK_STYLE);

		format(str, sizeof(str), "%s3\t     Podpowiedzi: %s\n", str, (pInfo[targetid][player_show_hints] ? ("T") : ("N")));
		DynamicGui_AddRow(playerid, DG_STATS_SHOW_HINTS);

		format(str, sizeof(str), "%s4\t     Zamra¿anie: %s\n", str, (pInfo[targetid][player_freeze_door] ? ("T") : ("N")));
		DynamicGui_AddRow(playerid, DG_STATS_FREEZE_DOOR);

		if( pInfo[playerid][player_door] <= 0 ) format(str, sizeof(str), "%s5\t     Miejsce spawnu: globalne\n", str);
		else 
		{
			new did = GetDoorByUid(pInfo[playerid][player_door]);
			if( did > -1 ) format(str, sizeof(str), "%s5\t     Miejsce spawnu: %s (UID: %d)\n", str, Door[did][door_name], Door[did][door_uid]);
			else
			{
				pInfo[playerid][player_door] = 0;
				mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_door = 0 WHERE char_uid = %d", pInfo[playerid][player_id]));
				format(str, sizeof(str), "%s5\t     Miejsce spawnu: globalne\n", str);
			}
		}
		DynamicGui_AddRow(playerid, DG_STATS_SPAWN);

		format(str, sizeof(str), "%s6\t     Tryb budowy: %s\n", str, (pInfo[targetid][player_editor]) ? ("Klawiszowy") : ("Strza³kowy (myszka)"));
		DynamicGui_AddRow(playerid, DG_EDITOR);

		format(str, sizeof(str), "%s7\t     Interfejs "HEX_COLOR_HONEST"Honest-RP"HEX_COLOR_WHITE": %s\n", str, (pInfo[targetid][player_interface]) ? ("T") : ("N"));
		DynamicGui_AddRow(playerid, DG_INTERFACE);

		format(str, sizeof(str), "%s7\t     Wyœwietlaj tylko ulubione przedmioty: %s\n", str, (pInfo[targetid][player_list_favorite_items]) ? ("T") : ("N"));
		DynamicGui_AddRow(playerid, DG_ITEMS_FAVORITE);
	}

	ShowPlayerDialog(playerid, DIALOG_STATS, DIALOG_STYLE_TABLIST_HEADERS, sprintf("Statystyki postaci "HEX_COLOR_HONEST"%s"HEX_COLOR_WHITE" (ID: %d; UID: %d) %s", pInfo[targetid][player_name], targetid, pInfo[targetid][player_id], gInfo[targetid][global_ip]), str, "Wybierz", "Zamknij");
	return 1;
}

COMMAND:dom(playerid, params[])
{
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w ¿adnym budynku.");
	
	if( !CanPlayerEditDoor(playerid, d_id) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ w³aœcicielem tego budynku.");
	if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GLOBAL ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz zapraszaæ graczy do mieszkania socjalnego.");
	
	if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
	{
		new gid = GetGroupByUid(Door[d_id][door_owner]);
		if( !GroupHasFlag(gid, GROUP_FLAG_SPAWN) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Grupa bêd¹ca w³aœcicielem tego budynku nie ma odpowiedniej flagi.");
	}
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /dom [zapros/wypros]");
	
	if( !strcmp(sub_cmd, "zapros") )
	{
		new targetid;
		if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /dom zapros [id gracza]");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
		if( pInfo[targetid][player_id] == Door[d_id][door_owner] && Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_PLAYER ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest w³aœcicielem tego domu.");
		if( pInfo[targetid][player_door] == Door[d_id][door_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest ju¿ zaproszony do tego domu.");
		
		new did = GetDoorByUid(pInfo[targetid][player_door]);
		if( did > -1 )
		{
			if( Door[did][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
			{
				new gid = GetGroupByUid(Door[did][door_owner]);
				if( Group[gid][group_type] == GROUP_TYPE_HOTEL ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ten gracz jest zameldowany w hotelu. Aby go zaprosiæ musi siê wczeœniej z niego wymeldowaæ.");
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
							if( Group[gid][group_type] == GROUP_TYPE_SOCIAL_HOUSE ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ten gracz jest zameldowany w mieszkaniu socjalnym. Aby go zaprosiæ musi siê wczeœniej z niego wymeldowaæ.");
						}
					}	
				}
			}
		}
		
		pInfo[targetid][player_door] = Door[d_id][door_uid];
		mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_door = %d WHERE char_uid = %d", pInfo[targetid][player_door], pInfo[targetid][player_id]));
		
		SendGuiInformation(playerid, "Informacja", sprintf("Zaprosi³eœ gracza %s (ID: %d) do domu %s (UID: %d).", pInfo[targetid][player_name], targetid, Door[d_id][door_name], Door[d_id][door_uid]));
		
		SendGuiInformation(targetid, "Informacja", sprintf("Zosta³eœ zaproszony do domu %s (UID: %d) przez gracza %s (ID: %d).\n\nOd teraz bêdziesz spawnowaæ siê w tym domu. Aby zmieniæ swój spawn wybierz odpowiednia opcjê w /stats.", Door[d_id][door_name], Door[d_id][door_uid], pInfo[playerid][player_name], playerid));
	}
	else if( !strcmp(sub_cmd, "wypros") )
	{
		new targetid;
		if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /dom zapros [id gracza]");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
		if( pInfo[targetid][player_id] == Door[d_id][door_owner] && Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_PLAYER ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id jest w³aœcicielem tego domu.");
		if( pInfo[targetid][player_door] != Door[d_id][door_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zaproszony do tego domu.");
		
		pInfo[targetid][player_door] = 0;
		mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_door = %d WHERE char_uid = %d", pInfo[targetid][player_door], pInfo[targetid][player_id]));

		SendGuiInformation(playerid, "Informacja", sprintf("Wyprosi³eœ gracza %s (ID: %d) z domu %s (UID: %d).", pInfo[targetid][player_name], targetid, Door[d_id][door_name], Door[d_id][door_uid]));
		SendGuiInformation(targetid, "Informacja", sprintf("Zosta³eœ wyproszony z domu %s (UID: %d) przez gracza %s (ID: %d).\n\nOd teraz bêdziesz spawnowaæ siê na spawnie globalnym. Aby zmieniæ swój spawn wybierz odpowiednia opcjê w /stats.", Door[d_id][door_name], Door[d_id][door_uid], pInfo[playerid][player_name], playerid));

	}
	else SendClientMessage(playerid, COLOR_GREY, "Tip: /dom [zapros/wypros]"); 	
	
	return 1;
}

COMMAND:mieszkanie(playerid, params[])
{
	new a_id = GetPlayerArea(playerid, AREA_TYPE_DOOR_OUTER);
		
	if( GetPlayerVirtualWorld(playerid) == 0 && a_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie stoisz w ¿adnych drzwiach.");
	
	new d_id;
	
	if( a_id != -1 ) d_id = Area[a_id][area_owner];
	else d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GLOBAL ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie stoisz w drzwiach mieszkania.");
	
	new parent_did = GetDoorByUid(Door[d_id][door_vw]);
	if( parent_did == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie stoisz w drzwiach mieszkania.");
	if( Door[parent_did][door_owner_type] != DOOR_OWNER_TYPE_GROUP ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie stoisz w drzwiach mieszkania.");
	
	new gid = GetGroupByUid(Door[parent_did][door_owner]);
	if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie stoisz w drzwiach mieszkania.");
	if( Group[gid][group_type] != GROUP_TYPE_SOCIAL_HOUSE ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie stoisz w drzwiach mieszkania.");
	
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /mieszkanie [wynajmij]");
	
	if( !strcmp(sub_cmd, "wynajmij") )
	{
		if( Door[d_id][door_owner] > 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "To mieszkanie jest ju¿ wynajmowane przez kogoœ innego.");
		
		new cdid = GetDoorByUid(pInfo[playerid][player_door]);
		if( cdid > -1 )
		{
			if( Door[cdid][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
			{
				new cgid = GetGroupByUid(Door[cdid][door_owner]);
				if( Group[cgid][group_type] == GROUP_TYPE_HOTEL ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Jesteœ zameldowany w hotelu. Aby zmieniæ spawn musisz siê wczeœniej z niego wymeldowaæ.");
			}
			else if( Door[cdid][door_owner_type] == DOOR_OWNER_TYPE_GLOBAL )
			{
				new cparent_did = GetDoorByUid(Door[cdid][door_vw]);
				if( cparent_did > -1 )
				{
					if( Door[cparent_did][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
					{
						new cgid = GetGroupByUid(Door[cparent_did][door_owner]);
						if( cgid > -1 )
						{
							if( Group[cgid][group_type] == GROUP_TYPE_SOCIAL_HOUSE ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Jesteœ zameldowany w mieszkaniu socjalnym. Aby zmieniæ spawn musisz siê wczeœniej z niego wymeldowaæ.");
						}
					}	
				}
			}
		}
		
		pInfo[playerid][player_dialog_tmp1] = parent_did;
		pInfo[playerid][player_dialog_tmp2] = d_id;
		
		ShowPlayerDialog(playerid, DIALOG_SOCIAL_RENT, DIALOG_STYLE_MSGBOX, "Wynajmowanie mieszkania socjalnego", sprintf("Czy jesteœ pewien, ¿e chcesz wynaj¹æ to mieszkanie socjalne:\n\nSpó³dzielnia:\t\t%s\nBudynek:\t\t%s\nMieszkanie:\t\t%s\nCzynsz tygodniowy:\t$%d\n\nPamiêtaj, ¿e pierwsza op³ata zostanie pobrana ju¿ teraz.", Group[gid][group_name], Door[parent_did][door_name], Door[d_id][door_name], Door[parent_did][door_value1]), "Tak", "Nie");
	}
	else SendClientMessage(playerid, COLOR_GREY, "Tip: /mieszkanie [wynajmij]"); 	
	
	return 1;
}

COMMAND:id(playerid, params[])
{
	new nick[20];
	if( sscanf(params, "s[20]", nick) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /id [czêœæ nicku gracza]");
	
	new count, str[400];
	
	foreach(new p : Player)
	{
		if( strfind(pInfo[p][player_name], nick, true) != -1 )
		{
			format(str, sizeof(str), "%s%d\t%s\n", str, p, pInfo[p][player_name]);
			count++;
		}
	}
	
	if( !count ) ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("Wyszukiwanie graczy z fraz¹ '%s'", nick), "Brak graczy online o podanej frazie.", "OK", "");
	else ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("Wyszukiwanie graczy z fraz¹ '%s'", nick), str, "OK", "");
	
	return 1;
}

COMMAND:pasy(playerid, params[])
{
	return cmd_v(playerid, "pasy");
}

COMMAND:live(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( Group[gid][group_type] != GROUP_TYPE_RADIO ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	if( pInfo[playerid][player_lsn_live] )
	{
		TextDrawSetString(LSNtd, "				    		    ~>~ ~w~Brak sygnalu nadawania.");
		pInfo[playerid][player_lsn_live] = false;
		SendGuiInformation(playerid, "Informacja", sprintf("Zakoñczy³eœ audycjê na ¿ywo w grupie %s (UID: %d).", Group[gid][group_name], Group[gid][group_uid]));
	}
	else
	{
		foreach(new p : Player)
		{
			if( GetPlayerGroupSlot(p, gid) > -1 )
			{
				if( pInfo[p][player_lsn_live] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Inny gracz z Twojej grupy aktualnie przeprowadza audycjê na ¿ywo.");
			}
		}
		
		pInfo[playerid][player_lsn_live] = true;
		SendGuiInformation(playerid, "Informacja", sprintf("Rozpocz¹³eœ audycjê na ¿ywo w grupie %s (UID: %d).", Group[gid][group_name], Group[gid][group_uid]));
	}
	
	return 1;
}

COMMAND:wywiad(playerid, params[])
{
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /wywiad [zakoncz]");
	if( !strcmp(params, "zakoncz") )
	{
		if( pInfo[playerid][player_lsn_wywiad_starter] == INVALID_PLAYER_ID )
		{			
			pInfo[pInfo[playerid][player_lsn_wywiad_with]][player_lsn_wywiad_starter] = INVALID_PLAYER_ID;
			pInfo[pInfo[playerid][player_lsn_wywiad_with]][player_lsn_wywiad_with] = INVALID_PLAYER_ID;
		}
		else 
		{
			pInfo[pInfo[playerid][player_lsn_wywiad_starter]][player_lsn_wywiad_starter] = INVALID_PLAYER_ID;
			pInfo[pInfo[playerid][player_lsn_wywiad_starter]][player_lsn_wywiad_with] = INVALID_PLAYER_ID;
		}
		
		pInfo[playerid][player_lsn_wywiad_starter] = INVALID_PLAYER_ID;
		pInfo[playerid][player_lsn_wywiad_with] = INVALID_PLAYER_ID;
		
		TextDrawSetString(LSNtd, "				    		    ~>~ ~w~Brak sygnalu nadawania.");
		pInfo[playerid][player_lsn_live] = false;
	}
	return 1;
}

COMMAND:reklama(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( Group[gid][group_type] != GROUP_TYPE_RADIO ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tej komendy.");
	
	new text[120], time;
	if( sscanf(params, "d s[120]", time, text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /reklama [czas w sekundach] [treœæ]");
	
	replacePolishChars(text);
	TextDrawSetString(LSNtd, sprintf("				    		    ~>~ ~g~Reklama: ~w~%s", replaceColorCodes(text)));
	
	Setting[setting_lsn_ad_finish_time] = gettime() + time;
	
	return 1;
}

COMMAND:kup(playerid, params[])
{
	if( GetPlayerVirtualWorld(playerid) == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy.");
	
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy.");
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy.");
	
	new gid = GetGroupByUid(Door[d_id][door_owner]);
	if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy.");
	if( Group[gid][group_type] != GROUP_TYPE_24/7 && Group[gid][group_type] != GROUP_TYPE_FASTFOOD ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy.");
	
	new hour, minute;
	gettime(hour, minute);
	if( Group[gid][group_type] == GROUP_TYPE_FASTFOOD && hour > 10 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Fastfoody otwartê s¹ jedynie od 24:00 do 10:00.");
	
	DynamicGui_Init(playerid);
	DynamicGui_SetDialogValue(playerid, d_id);
	
	new count, str[400];
	foreach(new i : Items)
	{
		if( Item[i][item_owner_type] == ITEM_OWNER_TYPE_DOOR_WAREHOUSE && Item[i][item_owner] == Door[d_id][door_uid] )
		{
			format(str, sizeof(str), "%s$%d\t%s\n", str, floatround(Item[i][item_price]*1.2), Item[i][item_name]);
			DynamicGui_AddRow(playerid, Item[i][item_uid]);
			count++;
		}
	}
	
	if( !count ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym sklepie nie ma ¿adnych produktów do kupienia.");
	else ShowPlayerDialog(playerid, DIALOG_24/7_BUY, DIALOG_STYLE_LIST, "Przedmioty do kupienia w sklepie:", str, "Wybierz", "Zamknij");
	return 1;
}

COMMAND:kupauto(playerid, params[])
{
	if( GetPlayerVirtualWorld(playerid) == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do salonu samochodowego.");
	
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do salonu samochodowego.");
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do salonu samochodowego.");
	
	new gid = GetGroupByUid(Door[d_id][door_owner]);
	if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do salonu samochodowego.");
	if( Group[gid][group_type] != GROUP_TYPE_CARS_SHOP ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do salonu samochodowego.");

	DynamicGui_Init(playerid);

	new str[200];
	
	new Cache:result;
	result = mysql_query(g_sql, "SELECT * FROM crp_salon_category");
	
	for(new i; i < cache_get_rows(); i++)
	{
		new tmp_cat[40];
		cache_get(i, "category_name", tmp_cat);
		
		format(str, sizeof(str), "%s%s\n", str, tmp_cat);
		DynamicGui_AddRow(playerid, cache_get_int(i, "category_uid"));
	}
	
	cache_delete(result);
	
	ShowPlayerDialog(playerid, DIALOG_CARS_SHOP_CATEGORY, DIALOG_STYLE_LIST, "Kupowanie pojazdu", str, "Wybierz", "Zamknij");
	
	return 1;
}

COMMAND:przymierz(playerid, params[])
{
	if( pInfo[playerid][player_is_in_binco] || pInfo[playerid][player_is_in_binco_access] ) return SendGuiInformation(playerid, "Wystapi³ b³¹d", "Nie mo¿esz teraz u¿yæ tej komendy.");
	if( GetPlayerVirtualWorld(playerid) == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	
	new gid = GetGroupByUid(Door[d_id][door_owner]);
	if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	if( Group[gid][group_type] != GROUP_TYPE_BINCO ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	
	pInfo[playerid][player_is_in_binco] = true;
	TogglePlayerControllable(playerid, 0);
	
	new Float:pos[3];
	GetPointInAngleOfPlayer(playerid, pos[0], pos[1], pos[2], 3.5, 0.0);
	
	SetPlayerCameraPos(playerid, pos[0], pos[1], pos[2]-0.2);
	
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	SetPlayerCameraLookAt(playerid, pos[0], pos[1], pos[2]);
	
	new skinid = Iter_Random(Skins[pInfo[playerid][player_sex]]);
	
	pInfo[playerid][player_binco_skinid] = skinid;
	SetPlayerSkin(playerid, Skin[skinid][skin_value]);
	
	PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], sprintf("~g~SKIN: %d   ~p~CENA: %d~n~~n~~w~Uzyj ~y~LMB~w~ i ~y~RMB~w~ aby zmieniac dostepne skiny.~n~~y~~k~~VEHICLE_ENTER_EXIT~~w~ Aby kupic wybrany skin lub ~y~~k~~PED_JUMPING~ ~w~aby anulowac.", Skin[skinid][skin_value], Skin[skinid][skin_price]));
	PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);
	return 1;
}

COMMAND:dodatki(playerid, params[])
{
	if( pInfo[playerid][player_is_in_binco] || pInfo[playerid][player_is_in_binco_access] ) return SendGuiInformation(playerid, "Wystapi³ b³¹d", "Nie mo¿esz teraz u¿yæ tej komendy.");
	if( GetPlayerVirtualWorld(playerid) == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GROUP ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	
	new gid = GetGroupByUid(Door[d_id][door_owner]);
	if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	if( Group[gid][group_type] != GROUP_TYPE_BINCO ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym miejscu nie mo¿esz u¿yæ tej komendy. Udaj siê do sklepu z ciuchami.");
	
	pInfo[playerid][player_is_in_binco_access] = true;
	TogglePlayerControllable(playerid, 0);
	
	new Float:pos[3];
	GetPointInAngleOfPlayer(playerid, pos[0], pos[1], pos[2], 0.5, 0.0);
	
	SetPlayerCameraPos(playerid, pos[0], pos[1], pos[2]);
	
	GetPointInAngleOfPlayer(playerid, pos[0], pos[1], pos[2], 1.5, 0.0);
	SetPlayerCameraLookAt(playerid, pos[0], pos[1], pos[2]);
	
	new Float:p_pos[4];
	GetPlayerFacingAngle(playerid, p_pos[3]);
	
	if( p_pos[3] <= 90.0 ) p_pos[3] = 180.0 + p_pos[3];
	else if( p_pos[3] > 270.0 ) p_pos[3] = 90.0 + (p_pos[3] - 270.0);
	else if( p_pos[3] > 90.0 && p_pos[3] <= 180.0 ) p_pos[3] = 270.0 + (p_pos[3] - 90.0);
	else if( p_pos[3] > 180.0 && p_pos[3] <= 270.0 ) p_pos[3] = 0.0 + (p_pos[3] - 180.0);
	
	new Cache:result;
	result = mysql_query(g_sql, "SELECT access_model, access_price, access_uid, access_name, access_rotz, access_rotx, access_roty FROM crp_access WHERE access_price > 0 ORDER BY RAND() LIMIT 1");
	
	pInfo[playerid][player_binco_access_uid] = cache_get_int(0, "access_uid");
	pInfo[playerid][player_binco_access_object] = CreateDynamicObject(cache_get_int(0, "access_model"), pos[0], pos[1], pos[2], 0.0 + cache_get_float(0, "access_rotx"), 0.0 + cache_get_float(0, "access_roty"), p_pos[3] + cache_get_float(0, "access_rotz"), -1, -1, playerid);
	
	new accessname[50];
	cache_get(0, "access_name", accessname);
	
	PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], sprintf("~g~%s (%d)   ~p~CENA: $%d~n~~n~~w~Uzyj ~y~LMB~w~ i ~y~RMB~w~ aby zmieniac dostepne dodatki.~n~~y~~k~~VEHICLE_ENTER_EXIT~~w~ Aby kupic wybrany dodatek lub ~y~~k~~PED_JUMPING~ ~w~aby anulowac.", accessname, cache_get_int(0, "access_model"), cache_get_int(0, "access_price")));
	PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);
	
	cache_delete(result);
	
	RefreshPlayer(playerid);
	
	return 1;
}

COMMAND:akceptujsmierc(playerid, params[])
{
	if( pInfo[playerid][player_bw] <= 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie zosta³eœ zraniony oraz nie jesteœ nieprzytomny wiêc nie mo¿esz u¿yæ tej komendy.");
	
	ShowPlayerDialog(playerid, DIALOG_ACCEPT_DEATH, DIALOG_STYLE_MSGBOX, "Akceptowanie œmierci", "Czy jesteœ pewien, ¿e chcesz zaakceptowaæ smieræ swojej postaci?\nZostanie ona permanentnie zablokowana, a jej cia³o pojawi siê w miejscu, w którym aktualnie siê znajduje.", "Akceptuj", "Anuluj");
	return 1;
}

COMMAND:p(playerid, params[])
{
	return cmd_przedmioty(playerid, params);
}

COMMAND:przedmioty(playerid, params[])
{
	if( pInfo[playerid][player_hurted] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Podczas gdy jesteœ ranny nie mo¿esz korzystaæ z listy przedmiotów.");
	if( isnull(params) )
	{
		if(pInfo[playerid][player_list_favorite_items]) ShowPlayerFavoriteItems(playerid, true);
		else ShowPlayerItems(playerid);
	}
	else
	{
		new sub_cmd[40], param[150];
		sscanf(params, "s[40] S()[150]", sub_cmd, param);
		
		if( !strcmp(sub_cmd, "podnies") ||  !strcmp(sub_cmd, "p") )
		{
			// Pickup nearby items
			new count, vw = GetPlayerVirtualWorld(playerid), int = GetPlayerInterior(playerid);
			
			DynamicGui_Init(playerid);
			new string[300];
			
			if( !IsPlayerInAnyVehicle(playerid) )
			{
				format(string, sizeof(string), "%s{C0C0C0}Przedmioty znajduj¹ce siê w okó³ Ciebie:\n", string);
				DynamicGui_AddBlankRow(playerid);
				
				foreach (new i : Items)
				{
					if( Item[i][item_owner_type] != ITEM_OWNER_TYPE_GROUND ) continue;
					
					if( IsPlayerInRangeOfPoint(playerid, 5.0, Item[i][item_x], Item[i][item_y], Item[i][item_z]) && Item[i][item_world] == vw && Item[i][item_interior] == int )
					{
						new name[150];
						format(name, sizeof(name), "%s", Item[i][item_name]);

						if(Item[i][item_type] == ITEM_TYPE_DRUGS) {
							format(name, sizeof(name), "%s (%dg)", Item[i][item_name], Item[i][item_amount]);
						}
						else if(Item[i][item_type] == ITEM_TYPE_DRUG_INGR) {
							format(name, sizeof(name), "%s (%dszt)", Item[i][item_name], Item[i][item_amount]);
						}

						format(string, sizeof(string), "%s%d\t%s\n", string, Item[i][item_uid], name);
						DynamicGui_AddRow(playerid, DG_ITEMS_PICKUP_ROW, i);
						
						count++;
					}
				}
				
				if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W okó³ Ciebie nie znajduj¹ siê ¿adne przedmioty.");
				else ShowPlayerDialog(playerid, DIALOG_ITEMS_PICKUP, DIALOG_STYLE_LIST, "Dostêpne przedmioty", string, "Podnieœ", "Zamknij");
			}
			else if( !strcmp(sub_cmd, "u") ||  !strcmp(sub_cmd, "ulubione") )
			{
				ShowPlayerFavoriteItems(playerid);
			}
			else
			{
				new vid = GetPlayerVehicleID(playerid);
				if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do podnoszenia przedmiotów w tym pojeŸdzie.");
				
				format(string, sizeof(string), "%s{C0C0C0}Przedmioty znajduj¹ce siê w pojeŸdzie:\n", string);
				DynamicGui_AddBlankRow(playerid);
				
				foreach (new i : Items)
				{
					if( Item[i][item_owner_type] != ITEM_OWNER_TYPE_VEHICLE || Item[i][item_owner] != Vehicle[vid][vehicle_uid] ) continue;

					new name[150];
					format(name, sizeof(name), "%s", Item[i][item_name]);

					if(Item[i][item_type] == ITEM_TYPE_DRUGS) {
						format(name, sizeof(name), "%s (%dg)", Item[i][item_name], Item[i][item_amount]);
					}
					else if(Item[i][item_type] == ITEM_TYPE_DRUG_INGR) {
						format(name, sizeof(name), "%s (%dszt)", Item[i][item_name], Item[i][item_amount]);
					}
					
					format(string, sizeof(string), "%s%d\t%s\n", string, Item[i][item_uid], name);
					DynamicGui_AddRow(playerid, DG_ITEMS_PICKUP_ROW, i);
					
					count++;
				}
				
				if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tym pojeŸdzie nie ma ¿adnych przedmiotów.");
				else ShowPlayerDialog(playerid, DIALOG_ITEMS_PICKUP, DIALOG_STYLE_LIST, "Dostêpne przedmioty", string, "Podnieœ", "Zamknij");
			}
		}
		else if( !strcmp(sub_cmd, "lista") )
		{
			if( GetPlayerPermissions(playerid, 3) ) return SendGuiInformation(playerid, "Informacja", "Nie masz uprawnieñ do korzystania z tej komendy.");

			new targetid;
			if( sscanf(param, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(p)rzedmiot lista [id gracza]");
			if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest pod³¹czony do serwera.");
			if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie jest zalogowany.");
	
			new count, string[2048];
			
			foreach (new item : Items)
			{
				if( Item[item][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[item][item_owner] != pInfo[targetid][player_id] ) continue;
				
				new prefix[10]; 
				if( Item[item][item_used] ) prefix = "{CFFD66}";
				
				format(string, sizeof(string), "%s %s%s{000000}\t\t%d %d  (%d:%d)\n", string, prefix, Item[item][item_name], Item[item][item_uid], Item[item][item_type], Item[item][item_value1], Item[item][item_value2]);
				count++;
			}
			
			if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie posiada ¿adnych przedmiotów.");
			else ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("Przedmioty gracza %s (ID: %d, UID: %d)", pInfo[targetid][player_name], targetid, pInfo[targetid][player_id]), string, "OK", "");
		}
		else
		{
			new bool:found = false;
			for(new item;item<MAX_ITEMS;item++)
			{
				if(!Iter_Contains(Items, item)) continue;

				if( Item[item][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[item][item_owner] != pInfo[playerid][player_id] ) continue;
				
				new iname[50];
				format(iname, sizeof(iname), "%s", Item[item][item_name]);
				replacePolishChars(iname);

				if( strfind(iname, sub_cmd, true) == 0 )
				{
					new itemname[40], optional_action[32];
					if( !sscanf(params, "s[40] s[32]", itemname, optional_action) )
					{
						if(!strcmp(optional_action, "odloz", false))
						{
							Item_Drop(item, playerid);
							return 1;
						}
					}

					Item_Use(item, playerid);
					found = true;
					break;
				}				
			}
			
			if( !found ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(p)rzedmioty [podnies]");
		}
	}
	
	return 1;
}

/*COMMAND:kartoteka(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która ma dostêp do kartoteki.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_FILES) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która ma dostêp do kartoteki.");
	
	if( !IsPlayerInGroupPlace(playerid, gid, true, false) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Musisz znajdowaæ siê w strefie, budynku lub pojeŸdzie grupy aby u¿ywaæ kartoteki.");
	
	LSPDDB_DisplayMainPage(playerid);
	
	return 1;
}*/

COMMAND:przeszukaj(playerid, params[])
{
	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e przeszukiwaæ.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_SEARCH) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e przeszukiwaæ.");
	
	new targetid = -1, targetname[MAX_PLAYER_NAME];
	if( sscanf(params, "d", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /przeszukaj [id]");
	//if( targetid == playerid ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz zaprosiæ samego siebie.");

	GetPlayerName(playerid, targetname, sizeof(targetname));
	SpaceToUnderscore(targetname);

	DynamicGui_Init(playerid);

	new str[256];
	strcat(str, "Wybierz sposób przeszukania\n");
	strcat(str, "1.   Przeszukanie szybkie\n");
	DynamicGui_AddRow(playerid, 0);

	strcat(str, "2.   Przeszukanie dok³adne\n");
	DynamicGui_AddRow(playerid, 1);

	pInfo[playerid][player_dialog_tmp1] = targetid;

	ShowPlayerDialog(playerid, DIALOG_SEARCH, DIALOG_STYLE_TABLIST_HEADERS, sprintf("Przeszukanie %s", targetname), str, "Przeszukaj", "Anuluj");

	return 1;
}

COMMAND:craft(playerid, params[])
{
	new a_id = GetPlayerArea(playerid, AREA_TYPE_DRUG_COOKER); 
	if( a_id == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê przy stole alchemicznym");
	new objectid = Area[a_id][area_owner];

	new slot = GetPlayerDutySlot(playerid);
	if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e craftowaæ.");
	
	new gid = pGroup[playerid][slot][pg_id];
	if( Group[gid][group_type] != GROUP_TYPE_GANG && Group[gid][group_type] != GROUP_TYPE_MAFIA ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e craftowaæ.");

	if(Object[objectid][object_is_drug_cooked]) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Stó³ alchemiczny jest w tym momencie u¿ywany.");

	if(Object[objectid][object_drug_cooked_id] > -1)
	{
		// tu robimy ugotowanego narksa
		new Cache:result;
		result = mysql_query(g_sql, sprintf("SELECT * FROM crp_drug_cook WHERE cook_uid = %d", Object[objectid][object_drug_cooked_id]));

		new percent = cache_get_int(0, "cook_percent");
		new drug = cache_get_int(0, "cook_drug");

		cache_delete(result);

		mysql_pquery(g_sql, sprintf("DELETE FROM crp_drug_cook WHERE cook_uid = %d", Object[objectid][object_drug_cooked_id]));

		Object[objectid][object_drug_cooked_id] = -1;
		UpdateDynamic3DTextLabelText(Object[objectid][object_label], COLOR_WHITE, "{00FF00}Stol alchemiczny\n"HEX_COLOR_WHITE"(( Wpisz /craft stojac bardzo blisko,\nby uzyc stolu. ))");

		new Float:quality = floatdiv(percent, 100);

		new rando = random(201)+1;
		if(rando <= 100)
		{
			quality = quality - floatdiv(rando, 1000);
		}
		else
		{
			quality = quality + floatdiv(rando, 1000);
		}

		new name[60];
		format(name, sizeof(name), "%s", DrugTypes[drug]);

		if(quality <= 0.65)
		{
			format(name, sizeof(name), "[MZ] %s", DrugTypes[drug]);
			quality = 0.5;
		}
		else if(quality <= 0.85)
		{
			quality = 0.75;
			format(name, sizeof(name), "[LZ] %s", DrugTypes[drug]);
		}
		else
		{
			quality = 1.0;
		}

		new unikat = random(1000); // zmienic na 1000
		if(quality == 1.0 && unikat <= 50)
		{
			quality = 1.3;

			// wyswietlamy gui do wybrania nazwy
			pInfo[playerid][player_dialog_tmp1] = 2; 
			pInfo[playerid][player_dialog_tmp2] = drug;
			pInfo[playerid][player_dialog_tmp3] = quality;

			ShowPlayerDialog(playerid, DIALOG_CRAFT, DIALOG_STYLE_INPUT, "Craftowanie » Wybór nazwy", sprintf("Uda³o Ci siê scraftowaæ narkotyk %s o nadprzciêtnej jakoœci, mo¿esz nadaæ mu swoj¹ w³asn¹ nazwê:", DrugTypes[pInfo[playerid][player_dialog_tmp2]]), "Gotowe", "");
		}
		else
		{
			new amount = random(3)+1;
			Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_DRUGS, drug, floatround(quality*100), name, 1575, amount);
			SendGuiInformation(playerid, "Craftowanie zakoñczone", sprintf("Otrzyma³eœ:\n\n\t"HEX_COLOR_LIGHTER_GREEN"%dg "HEX_COLOR_WHITE"narkotyku "HEX_COLOR_LIGHTER_RED"%s "HEX_COLOR_WHITE"(jakoœæ "HEX_COLOR_LIGHTER_GREEN"%.1f"HEX_COLOR_WHITE")", amount, name, quality));
		}
		
		return 1;
	}

	new string[500];

	pInfo[playerid][player_dialog_tmp1] = 0;

	DynamicGui_Init(playerid);

	for(new i=2;i<sizeof(DrugTypes);i++)
	{
		format(string, sizeof(string), "%s%s\n", string, DrugTypes[i]);
		DynamicGui_AddRow(playerid, i);
	}

	ShowPlayerDialog(playerid, DIALOG_CRAFT, DIALOG_STYLE_LIST, "Craftowanie", string, "Wybierz", "Zamknij");

	return 1;
}