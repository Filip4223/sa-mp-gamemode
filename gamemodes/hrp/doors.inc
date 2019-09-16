stock LoadDoor(limit[] = "", bool:return_id = false)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT * FROM crp_doors %s", limit));  
	
	new rows = cache_get_rows(), d_id;
	
	for(new i;i<rows;i++)
	{
		d_id = CreateDynamicPickup(cache_get_int(i, "door_pickupid"), 1, cache_get_float(i, "door_enterx"), cache_get_float(i, "door_entery"), cache_get_float(i, "door_enterz"), cache_get_int(i, "door_entervw"), cache_get_int(i, "door_enterint"), -1, 200.0);
		
		Iter_Add(Doors, d_id);	
		
		cache_get(i, "door_name", Door[d_id][door_name]);
		cache_get(i, "door_audiourl", Door[d_id][door_audio]);
		
		Door[d_id][door_uid] = cache_get_int(i, "door_uid");
		Door[d_id][door_type] = cache_get_int(i, "door_type");
		Door[d_id][door_owner_type] = cache_get_int(i, "door_ownertype");
		Door[d_id][door_owner] = cache_get_int(i, "door_owner");

		Door[d_id][door_objects_limit] = cache_get_int(i, "door_objects");
		Door[d_id][door_auto_closing] = cache_get_bool(i, "door_lock");
		Door[d_id][door_car_crosing] = cache_get_bool(i, "door_garage");
		Door[d_id][door_payment] = cache_get_int(i, "door_enterpay");

		Door[d_id][door_pos][0] = cache_get_float(i, "door_enterx");
		Door[d_id][door_pos][1] = cache_get_float(i, "door_entery");
		Door[d_id][door_pos][2] = cache_get_float(i, "door_enterz");
		Door[d_id][door_pos][3] = cache_get_float(i, "door_entera");
		Door[d_id][door_vw] = cache_get_int(i, "door_entervw");
		Door[d_id][door_int] = cache_get_int(i, "door_enterint");
		Door[d_id][door_spawn_pos][0] = cache_get_float(i, "door_exitx");
		Door[d_id][door_spawn_pos][1] = cache_get_float(i, "door_exity");
		Door[d_id][door_spawn_pos][2] = cache_get_float(i, "door_exitz");
		Door[d_id][door_spawn_pos][3] = cache_get_float(i, "door_exita");
		Door[d_id][door_spawn_vw] = cache_get_int(i, "door_exitvw");
		if( Door[d_id][door_spawn_vw] == -1 ) Door[d_id][door_spawn_vw] = Door[d_id][door_uid];
		Door[d_id][door_spawn_int] = cache_get_int(i, "door_exitint");
		
		Door[d_id][door_value1] = cache_get_int(i, "door_value1");

		Door[d_id][door_meters] = cache_get_int(i, "door_meters");
		
		Door[d_id][door_meters_points][0] = cache_get_float(i, "door_meters_x");
		Door[d_id][door_meters_points][1] = cache_get_float(i, "door_meters_y");
		Door[d_id][door_meters_points][2] = cache_get_float(i, "door_meters_z");
		Door[d_id][door_meters_points][3] = cache_get_float(i, "door_meters_nx");
		Door[d_id][door_meters_points][4] = cache_get_float(i, "door_meters_ny");
		Door[d_id][door_meters_points][5] = cache_get_float(i, "door_meters_nz");

		new map_model = cache_get_int(i, "door_mapmodel");
		if( map_model != -1 )
		{
			Door[d_id][door_map_icon] = CreateDynamicMapIcon(Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2], map_model, 0, Door[d_id][door_vw], Door[d_id][door_int]);
		}
			
		// Strefa zewn. drzwi
		Door[d_id][door_area_outer] = CreateDynamicSphere(Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2], 1.2, Door[d_id][door_vw], Door[d_id][door_int]);
		
		Area[Door[d_id][door_area_outer]][area_type] = AREA_TYPE_DOOR_OUTER;
		Area[Door[d_id][door_area_outer]][area_owner_type] = 0;
		Area[Door[d_id][door_area_outer]][area_owner] = d_id;
		Area[Door[d_id][door_area_outer]][area_uid] = -1;
		
		Iter_Add(Areas, Door[d_id][door_area_outer]);
		
		// Strefa metra¿u drzwi
		Door[d_id][door_meters_area] = CreateDynamicCuboid(Door[d_id][door_meters_points][0], Door[d_id][door_meters_points][1], Door[d_id][door_meters_points][2], Door[d_id][door_meters_points][3], Door[d_id][door_meters_points][4], Door[d_id][door_meters_points][5]);
		Area[Door[d_id][door_meters_area]][area_type] = AREA_TYPE_DOOR_METERS;
		Area[Door[d_id][door_area_inner]][area_owner_type] = 0;
		Area[Door[d_id][door_area_inner]][area_owner] = d_id;
		Area[Door[d_id][door_area_inner]][area_uid] = -1;

		Iter_Add(Areas, Door[d_id][door_meters_area]);

		// Strefa wewn. drzwi
		Door[d_id][door_area_inner] = CreateDynamicSphere(Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], 1.2, Door[d_id][door_spawn_vw], Door[d_id][door_spawn_int]);
		
		Area[Door[d_id][door_area_inner]][area_type] = AREA_TYPE_DOOR_INNER;
		Area[Door[d_id][door_area_inner]][area_owner_type] = 0;
		Area[Door[d_id][door_area_inner]][area_owner] = d_id;
		Area[Door[d_id][door_area_inner]][area_uid] = -1;
		
		Iter_Add(Areas, Door[d_id][door_area_inner]);
		
		Door[d_id][door_closed] = Door[d_id][door_auto_closing];
	}

	cache_delete(result);
	
	if( return_id ) return d_id;
	return rows;
}

stock DeleteDoor(d_id, bool:from_mysql = true)
{	
	if( from_mysql ) mysql_pquery(g_sql, sprintf("DELETE FROM `crp_doors` WHERE `door_uid` = %d", Door[d_id][door_uid]));
	
	Iter_Remove(Doors, d_id);
	
	DestroyDynamicPickup(d_id);
	
	if( IsValidDynamicMapIcon(Door[d_id][door_map_icon]) ) DestroyDynamicMapIcon(Door[d_id][door_map_icon]);
	
	DestroyDynamicArea(Door[d_id][door_area_inner]);
	Iter_Remove(Areas, Door[d_id][door_area_inner]);
		
	DestroyDynamicArea(Door[d_id][door_area_outer]);
	Iter_Remove(Areas, Door[d_id][door_area_outer]);

	DestroyDynamicArea(Door[d_id][door_meters_area]);
	Iter_Remove(Areas, Door[d_id][door_meters_area]);
	
	for(new z=0; e_doors:z != e_doors; z++)
    {
		Door[d_id][e_doors:z] = 0;
    }
}

stock GetDoorByUid(uid)
{
	foreach(new d_id : Doors)
	{
		if( Door[d_id][door_uid] == uid ) return d_id;
	}
	
	return -1;
}

stock CanPlayerUseDoor(playerid, d_id)
{
	if(!GetPlayerPermissions(playerid, 5)) return 1;
	
	switch( Door[d_id][door_owner_type] )
	{
		case DOOR_OWNER_TYPE_GLOBAL:
		{
			if( Door[d_id][door_owner] == pInfo[playerid][player_id] )
			{
				new parent_did = GetDoorByUid(Door[d_id][door_vw]);
				if( parent_did > -1 )
				{
					if( Door[parent_did][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
					{
						new gid = GetGroupByUid(Door[parent_did][door_owner]);
						if( gid > -1 )
						{
							if( Group[gid][group_type] == GROUP_TYPE_SOCIAL_HOUSE )
							{
								return 1;
							}
						}
					}
				}
			}
		}
		
		case DOOR_OWNER_TYPE_PLAYER:
		{
			if( Door[d_id][door_owner] == pInfo[playerid][player_id] ) return 1;
			if( pInfo[playerid][player_door] == Door[d_id][door_uid] ) return 1;
		}
		
		case DOOR_OWNER_TYPE_GROUP:
		{
			new gid = GetGroupByUid(Door[d_id][door_owner]);
			
			new slot = GetPlayerGroupSlot(playerid, gid);
			if( slot > -1 ) 
			{
				if( WorkerHasFlag(playerid, slot, WORKER_FLAG_DOORS) ) return 1;
			}
		}
	}
	
	return 0;
}

stock ShowPlayerDoorsEdit(playerid, d_id)
{
	new 
		header[80];
	format(header, sizeof(header), "Drzwi %d (%s; SampID: %d)", Door[d_id][door_uid], Door[d_id][door_name], d_id);
	
	new str[400];
	
	DynamicGui_Init(playerid);
	DynamicGui_SetDialogValue(playerid, d_id);
	
	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GLOBAL )
	{
		format(str, sizeof(str), "%s01\tZmieñ nazwê drzwi\n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_NAME);	
	}
	
	if( GetPlayerVirtualWorld(playerid) == Door[d_id][door_vw] )
	{			
		format(str, sizeof(str), "%s02\tZmieñ pozycjê wewnêtrznych drzwi (koordynaty)\n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_SPAWN_COORDS);
		
		if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GLOBAL )
		{
			format(str, sizeof(str), "%s03\tZa³aduj obiekty\n", str);
			DynamicGui_AddRow(playerid, DG_DRZWI_MAP_LOAD);
		}
	}
	
	format(str, sizeof(str), "%s04\tZmieñ œcie¿kê audio\n", str);
	DynamicGui_AddRow(playerid, DG_DRZWI_AUDIO);

	format(str, sizeof(str), "%s05\tSchowek\n", str);
	DynamicGui_AddRow(playerid, DG_DRZWI_SCHOWEK);	

	if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
	{
		format(str, sizeof(str), "%s06\tMagazyn\n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_MAGAZYN);	
	}		

	if( GetPlayerVirtualWorld(playerid) == Door[d_id][door_spawn_vw] )
	{
		format(str, sizeof(str), "%s07\tZmieñ pozycjê wewnêtrznych drzwi\n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_SPAWN);
	}

	if( Door[d_id][door_owner_type] != DOOR_OWNER_TYPE_GLOBAL )
	{
		format(str, sizeof(str), "%s08\tZmieñ op³atê za wejœcie\n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_PAYMENT);
	}

	format(str, sizeof(str), "%s09\tW(y)³¹cz przeje¿d¿anie pojazdami\n", str);
	DynamicGui_AddRow(playerid, DG_DRZWI_CARS);

	format(str, sizeof(str), "%s10\tW(y)³¹cz zamykanie po restarcie\n", str);
	DynamicGui_AddRow(playerid, DG_DRZWI_CLOSING);

	if( GetPlayerVirtualWorld(playerid) == Door[d_id][door_spawn_vw] )
	{
		format(str, sizeof(str), "%s11\tUstaw metra¿ budynku", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_METERS);
	}
	new owner_str[80];
	switch( Door[d_id][door_owner_type] )
	{
		case DOOR_OWNER_TYPE_GLOBAL:
		{
			format(owner_str, sizeof(owner_str), "globalne; brak");
		}
	
		case DOOR_OWNER_TYPE_PLAYER:
		{			
			new name[MAX_PLAYER_NAME+1];
			GetPlayerNameByUid(Door[d_id][door_owner], name);
			
			format(owner_str, sizeof(owner_str), "prywatne; %s", name);
		}
	
		case DOOR_OWNER_TYPE_GROUP:
		{			
			new gid = GetGroupByUid(Door[d_id][door_owner]);
			format(owner_str, sizeof(owner_str), "grupowe; %s", Group[gid][group_name]);
		}
	}

	new string[420];
	format(string, sizeof(string), " ~n~ ~n~%d. %s~n~~n~~p~Owner: ~w~%d:%d (%s)    ~w~Obiektow: %d/%d", Door[d_id][door_uid], Door[d_id][door_name], Door[d_id][door_owner_type], Door[d_id][door_owner], owner_str, CountDoorObjects(d_id), Door[d_id][door_objects_limit]);
	format(string, sizeof(string), "%s~n~~y~Oplata za wejscie: ~w~$%d    ~y~Przejazd: ~w~%s    ~y~Zamykanie: ~w~%s", string, Door[d_id][door_payment], ((Door[d_id][door_car_crosing]) ? ("T") : ("N")), ((Door[d_id][door_auto_closing]) ? ("T") : ("N")));
	format(string, sizeof(string), "%s~n~~n~~r~Zewnatrz: ~w~%.2f, %.2f, %.2f~n~~r~Wewnatrz: ~w~%.2f, %.2f, %.2f", string, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2], Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
	format(string, sizeof(string), "%s~n~~b~VW:~w~ %d    ~b~INT: ~w~%d~n~~b~Zew. drzwi: ~w~%d    ~b~Wew. drzwi: ~w~%d", string, Door[d_id][door_spawn_vw], Door[d_id][door_spawn_int], Door[d_id][door_area_outer], Door[d_id][door_area_inner]);


	PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], string);
	PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);

	ShowPlayerDialog(playerid, DIALOG_DRZWI, DIALOG_STYLE_LIST, header, str, "Wybierz", "Zamknij");

	return 1;
}

stock CanPlayerEditDoor(playerid, d_id)
{
	if(!GetPlayerPermissions(playerid, 5)) return 1;

	switch( Door[d_id][door_owner_type] )
	{
		case DOOR_OWNER_TYPE_GLOBAL:
		{
			if( Door[d_id][door_owner] == pInfo[playerid][player_id] )
			{
				new parent_did = GetDoorByUid(Door[d_id][door_vw]);
				if( parent_did > -1 )
				{
					if( Door[parent_did][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
					{
						new gid = GetGroupByUid(Door[parent_did][door_owner]);
						if( gid > -1 )
						{
							if( Group[gid][group_type] == GROUP_TYPE_SOCIAL_HOUSE )
							{
								return 1;
							}
						}
					}
				}
			}
		}
		
		case DOOR_OWNER_TYPE_PLAYER:
		{
			if( Door[d_id][door_owner] == pInfo[playerid][player_id] ) return 1;
		}
		
		case DOOR_OWNER_TYPE_GROUP:
		{
			new gid = GetGroupByUid(Door[d_id][door_owner]);
			
			new slot = GetPlayerGroupSlot(playerid, gid);
			if( slot > -1 ) 
			{
				if( WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return 1;
			}
		}
	}
	return 0;
}

stock CountDoorObjects(d_id)
{
	new count;
	foreach(new oid : Objects)
	{
		if( Object[oid][object_owner_type] == OBJECT_OWNER_TYPE_DOOR && Object[oid][object_owner] == Door[d_id][door_uid] ) count++;
	}
	
	return count;
}

stock CountDoorLabels(d_id)
{
	new count;
	foreach(new lid : Labels)
	{
		if( Label[Text3D:lid][label_owner_type] == LABEL_OWNER_TYPE_DOOR && Label[Text3D:lid][label_owner] == Door[d_id][door_uid] ) count++;
	}
	
	return count;
}

stock ShowPlayerDoorTextdraw(playerid, d_id)
{
	if(GetPlayerPermissions(playerid, 5))
	{
		if( Door[d_id][door_payment] > 0 && !Door[d_id][door_closed] ) PlayerTextDrawSetString(playerid, pInfo[playerid][DoorInfo][1], sprintf("%s~n~~n~Wejscie: ~g~$%d", Door[d_id][door_name], Door[d_id][door_payment]));
		else if( Door[d_id][door_payment] == 0 ) PlayerTextDrawSetString(playerid, pInfo[playerid][DoorInfo][1], Door[d_id][door_name]);
	}
	else
	{
		if( Door[d_id][door_payment] > 0 && !Door[d_id][door_closed] ) PlayerTextDrawSetString(playerid, pInfo[playerid][DoorInfo][1], sprintf("%s (%d)~n~~n~Wejscie: ~g~$%d", Door[d_id][door_name], d_id, Door[d_id][door_payment]));
		else if( Door[d_id][door_payment] == 0 ) PlayerTextDrawSetString(playerid, pInfo[playerid][DoorInfo][1], sprintf("%s (%d)", Door[d_id][door_name], d_id));
	}
	
	if( Door[d_id][door_closed] ) PlayerTextDrawSetString(playerid, pInfo[playerid][DoorInfo][2], "~n~~w~[~r~~h~DRZWI ZAMKNIETE~w~]");
	else PlayerTextDrawSetString(playerid, pInfo[playerid][DoorInfo][2], "~y~Aby wejsc, wcisnij jednoczesnie~n~~w~[~b~~h~LALT + SPACE~w~]");

	for(new i;i<3;i++) PlayerTextDrawShow(playerid, pInfo[playerid][DoorInfo][i]);
}

stock HidePlayerDoorTextdraw(playerid)
{
	for(new i;i<3;i++) PlayerTextDrawHide(playerid, pInfo[playerid][DoorInfo][i]);
}

stock DoorsDefaultInteriorsList(playerid, d_id, page=1)
{
	new Cache:result;
	result = mysql_query(g_sql, "SELECT COUNT(*) as count FROM `crp_default_interiors`");
	
	new all_rows = cache_get_int(0, "count");
	
	cache_delete(result);

	mysql_tquery(g_sql, sprintf("SELECT name, id FROM `crp_default_interiors` LIMIT %d, 20", (page-1)*20), "OnDoorsInteriorsListLoaded", "iiii", playerid, d_id, page, all_rows);
	
	return 1;
}

function OnDoorsInteriorsListLoaded(playerid, d_id, page, all_rows)
{
	new header[64], Float:pp = all_rows / 20;
	format(header, sizeof(header), "Zmiana wnêtrza drzwi (%d/%d)", page, floatround(pp, floatround_ceil));
	
	pInfo[playerid][player_dialog_tmp1] = page;
	
	DynamicGui_Init(playerid);
	DynamicGui_SetDialogValue(playerid, d_id);
	
	new str[800];
	
	if( page > 1 )
	{
		format(str, sizeof(str), "%s"HEX_COLOR_SAMP"<<< Poprzednia strona\n  \n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_CHANGE_INTERIOR_PREV);
		
		DynamicGui_AddBlankRow(playerid);
	}
	
	if( page == 1 )
	{
		format(str, sizeof(str), "%s0.\tBrak (w³asne obiekty)\n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_CHANGE_INTERIOR_ROW, -1);
	}
	
	new tmp_str[60];
	
	for(new i;i<cache_get_rows();i++)
	{
		cache_get(i, "name", tmp_str);
		
		format(str, sizeof(str), "%s%d.\t%s\n", str, (i+1)+((page-1)*20), tmp_str);
		DynamicGui_AddRow(playerid, DG_DRZWI_CHANGE_INTERIOR_ROW, cache_get_int(i, "id"));
	}
	
	if( all_rows > (page*20) )
	{
		format(str, sizeof(str), "%s"HEX_COLOR_SAMP"\t\t\t\t\t\t\t   Nastepna strona >>>\n", str);
		DynamicGui_AddRow(playerid, DG_DRZWI_CHANGE_INTERIOR_NEXT);
	}
	
	ShowPlayerDialog(playerid, DIALOG_ADRZWI_CHANGE_INTERIOR, DIALOG_STYLE_LIST, header, str, "Wybierz", "Wróæ");

	return 1;
}