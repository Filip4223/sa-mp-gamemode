stock LoadArea(limit[] = "", bool:return_id = false)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_areas` %s", limit));
	
	new rows = cache_get_rows();
	
	new area_id;
	for(new i;i<rows;i++)
	{
		area_id = -1;
		new shape = cache_get_int(i, "area_shape");
		
		new Float:point1[3], Float:point2[3];
		
		new str[100];
		cache_get(i, "area_point1", str);
		sscanf(str, "p<|>a<f>[3]", point1);
		
		new str2[100];
		cache_get(i, "area_point2", str2);
		sscanf(str2, "p<|>a<f>[3]", point2);
		
		if( shape == AREA_SHAPE_SQUARE )
		{
			// Tworzymy prostokatna strefe
			new Float:pointes[8];
			pointes[0] = point1[0];
			pointes[1] = point1[1];
			pointes[2] = point1[0];
			pointes[3] = point2[1];
			pointes[4] = point2[0];
			pointes[5] = point2[1];
			pointes[6] = point2[0];
			pointes[7] = point1[1];
			
			area_id = CreateDynamicPolygon(pointes, -FLOAT_INFINITY, FLOAT_INFINITY, 8, cache_get_int(i, "area_vw"));
		}
		else if( shape == AREA_SHAPE_CIRCLE )
		{
			// Tworzymy okragla strefe
			area_id = CreateDynamicSphere(point1[0], point1[1], point1[2], cache_get_float(i, "area_size"), cache_get_int(i, "area_vw"));
		}
		
		if( area_id == -1 )
		{
			printf("[honest] B³¹d podczas wczytywania strefy o uid: %d", cache_get_int(i, "area_uid"));
			continue;
		}
		
		Area[area_id][area_uid] = cache_get_int(i, "area_uid");
		
		Area[area_id][area_type] = cache_get_int(i, "area_type");
		Area[area_id][area_owner_type] = cache_get_int(i, "area_ownertype");
		Area[area_id][area_owner] = cache_get_int(i, "area_owner");
		
		Area[area_id][area_objects_limit] = cache_get_int(i, "area_objects");

		switch(Area[area_id][area_type])
		{
			case AREA_TYPE_FIRE_PLACE:
			{
				Iter_Add(FirePlaces, area_id);

				Area[area_id][area_pos][0] = point1[0];
				Area[area_id][area_pos][1] = point1[1];
				Area[area_id][area_pos][2] = point1[2];
				
				
			}
		}
		
		Iter_Add(Areas, area_id);
	}
	
	cache_delete(result);
	
	if( return_id ) return area_id;
	return rows;
}

stock DeleteArea(a_id, bool:from_database = true)
{
	if( from_database ) mysql_pquery(g_sql, sprintf("DELETE FROM `crp_areas` WHERE `area_uid` = %d", Area[a_id][area_uid]));
	
	DestroyDynamicArea(a_id);
		
	Iter_Remove(Areas, a_id);
	
	for(new z=0; e_areas:z != e_areas; z++)
    {
		Area[a_id][e_areas:z] = 0;
    }
}

stock GetPlayerArea(playerid, type, owner_type = -1)
{
	foreach(new a_id : Areas)
	{
		if( Area[a_id][area_type] == type && IsPlayerInDynamicArea(playerid, a_id) )
		{
			if( owner_type == -1 ) return a_id;
			else if( Area[a_id][area_owner_type] == owner_type ) return a_id;
		}
	}
	return -1;
}

stock GetAreaByUid(a_uid)
{
	foreach(new a_id : Areas)
	{
		if( Area[a_id][area_uid] == a_uid ) return a_id;
	}
	
	return -1;
}

stock CountAreaObjects(a_id)
{
	new count;
	foreach(new o_id : Objects)
	{
		if( Object[o_id][object_owner_type] == OBJECT_OWNER_TYPE_AREA && Object[o_id][object_owner] == Area[a_id][area_uid] ) count++;
	}
	
	return count;
}

stock CountAreaLabels(a_id)
{
	new count;
	foreach(new lid : Labels)
	{
		if( Label[Text3D:lid][label_owner_type] == LABEL_OWNER_TYPE_AREA && Label[Text3D:lid][label_owner] == Area[a_id][area_uid] ) count++;
	}
	
	return count;
}

stock CanPlayerEditArea(playerid, a_id)
{
	if(GetPlayerPermissions(playerid, 5)) return 1;
	
	switch( Area[a_id][area_owner_type] )
	{
		case AREA_OWNER_TYPE_PLAYER:
		{
			if( Area[a_id][area_owner] == pInfo[playerid][player_id] ) return 1;
		}
		
		case AREA_OWNER_TYPE_GROUP:
		{
			new gid = GetGroupByUid(Area[a_id][area_owner]);
			if( gid == -1 ) return 0;
			
			new slot = GetPlayerGroupSlot(playerid, gid);
			if( slot == -1 ) return 0;
			
			if( WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return 1;
		}
	}
	return 0;
}

stock CanPlayerUseArea(playerid, a_id)
{
	if(GetPlayerPermissions(playerid, 5)) return 1;

	switch( Area[a_id][area_owner_type] )
	{
		case AREA_OWNER_TYPE_PLAYER:
		{
			if( Area[a_id][area_owner] == pInfo[playerid][player_id] ) return 1;
		}
		
		case AREA_OWNER_TYPE_GROUP:
		{
			new gid = GetGroupByUid(Area[a_id][area_owner]);
			if( gid == -1 ) return 0;
			
			new slot = GetPlayerGroupSlot(playerid, gid);
			if( slot == -1 ) return 0;
			
			if(pGroup[playerid][slot][pg_id] == gid) return 1;
		}
	}
	
	return 0;
}