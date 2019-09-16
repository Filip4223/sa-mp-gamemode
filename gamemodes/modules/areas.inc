public OnPlayerEnterDynamicArea(playerid, areaid)
{
	pPlayer[playerid][player_area] = areaid;
	
	GangZoneDestroy(pPlayer[playerid][player_zone]);
	pPlayer[playerid][player_zone] = GangZoneCreate(pArea[areaid][area_pos_min_x], pArea[areaid][area_pos_min_y], pArea[areaid][area_pos_max_x], pArea[areaid][area_pos_max_y]);
	GangZoneShowForPlayer(playerid, pPlayer[playerid][player_zone], strval(pArea[areaid][area_color]));
}

public OnPlayerLeaveDynamicArea(playerid, areaid)
{
	GangZoneDestroy(pPlayer[playerid][player_zone]);
	
	pPlayer[playerid][player_area] = -1;
}

stock LoadAreas()
{
	print("[Loading] Start areas loading...");

	new Cache:result, iter = 0;
	result = mysql_query(m_sql, "SELECT * FROM `samp_areas`");

	for(new i = 0; i<cache_get_rows(); i++)
	{
		new Float:x = cache_get_float(i, "area_min_x"),
			Float:y = cache_get_float(i, "area_min_y"), 
			Float:mx = cache_get_float(i, "area_max_x"),
			Float:my = cache_get_float(i, "area_max_y");

		new areaid = CreateDynamicRectangle(x, y, mx, my);

		pArea[areaid][area_id] = cache_get_int(i, "area_id");
		pArea[areaid][area_owner] = cache_get_int(i, "area_owner");
		pArea[areaid][area_ownertype] = cache_get_int(i, "area_ownertype");
		cache_get(i, "area_name", pArea[areaid][area_name]);
		cache_get(i, "area_color", pArea[areaid][area_color]);
		pArea[areaid][area_pos_min_x] = x;
		pArea[areaid][area_pos_min_y] = y;
		pArea[areaid][area_pos_max_x] = mx;
		pArea[areaid][area_pos_max_y] = my;
		pArea[areaid][area_game_id] = areaid;

		iter++;

		Iter_Add(Areas, areaid);
	}

	printf("[Loading] Successfully loaded %d areas.", iter);

	cache_delete(result);
}

stock GetAreaByUid(a_uid)
{
	foreach(new a_id : Areas)
	{
		printf("a_id %d : %d", a_id, pArea[a_id][area_id]);
		if( pArea[a_id][area_id] == a_uid ) return a_id;
	}
	
	return -1;
}

/*stock GenerateAreas()
{
	new x = 3000, 
		y = 3000;

	for(new i = 0; i < 60; i++)
	{
		for(new j = 0; j < 60; j++)
		{
			x -= 100;

			mysql_query(m_sql, sprintf("INSERT INTO samp_areas VALUES('null', '%s', %d, %d, %d, %d)", sprintf("%dx%d", i, j), x, y, x+100, y+100));

			Debug(sprintf("%d | %d", x, y));
		}

		y -= 100;
		x = 3000;
	}

	return 1;
}*/