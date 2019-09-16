stock LoadObject(limit[] = "", bool:return_id = false)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT crp_objects.*, IF(mat_uid IS NULL, FALSE, TRUE) as has_materials FROM crp_objects LEFT JOIN crp_objects_mats ON mat_object = object_uid %s GROUP BY object_uid ", limit));  

	new rows = cache_get_rows(), oid;

	new Iterator:loadMaterials<MAX_ITEMS>;

	new to_delete[MAX_OBJECTS], deleteid = 0;
	for(new k;k<MAX_OBJECTS;k++) to_delete[k] = -1;
	
	for(new i;i<rows;i++)
	{
		new model = cache_get_int(i, "object_model"),
			vw = cache_get_int(i, "object_world"),
			owner = cache_get_int(i, "object_owner"),
			owner_type = cache_get_int(i, "object_ownertype");
			
		if( owner_type == 0 ) continue;
			
		oid = CreateDynamicObject(model, cache_get_float(i, "object_posx"), cache_get_float(i, "object_posy"), cache_get_float(i, "object_posz"), cache_get_float(i, "object_rotx"), cache_get_float(i, "object_roty"), cache_get_float(i, "object_rotz"), vw, -1, -1, 300.0);
		
		for(new z=0; e_objects:z != e_objects; z++)
		{
			Object[oid][e_objects:z] = 0;
		}
		
		Object[oid][object_uid] = cache_get_int(i, "object_uid");
		Object[oid][object_owner_type] = owner_type;
		Object[oid][object_owner] = owner;
		
		switch( Object[oid][object_owner_type] )
		{
			case OBJECT_OWNER_TYPE_AREA:
			{
				new a_id = GetAreaByUid(owner);
				if( a_id == -1 )
				{
					printf("[honest] Obiekt o UID: %d zosta³ zniszczony poniewa¿ nie istnieje strefa do której nale¿y.", Object[oid][object_uid]);
					to_delete[deleteid] = Object[oid][object_uid];
					deleteid++;
					DeleteObject(oid, false);
					continue;
				}
			}
			
			case OBJECT_OWNER_TYPE_DOOR:
			{
				new d_id = GetDoorByUid(owner);
				if( d_id == -1 )
				{
					printf("[honest] Obiekt o UID: %d zosta³ zniszczony poniewa¿ nie istnieja drzwi do których nale¿y.", Object[oid][object_uid]);
					to_delete[deleteid] = Object[oid][object_uid];
					deleteid++;
					DeleteObject(oid, false);
					continue;
				}
			}
		}
		
		Object[oid][object_vw] = vw;
		Object[oid][object_model] = model;
		Object[oid][object_stream_distance] = 300.0;
		Object[oid][object_type] = cache_get_int(i, "object_type");
		
		switch( Object[oid][object_type] )
		{
			case OBJECT_TYPE_ATM:
			{
				// Tworzymy strefe bankomatu
				Object[oid][object_area] = CreateDynamicSphere(cache_get_float(i, "object_posx"), cache_get_float(i, "object_posy"), cache_get_float(i, "object_posz"), 2.0, vw);
				Area[Object[oid][object_area]][area_uid] = -1;
				Area[Object[oid][object_area]][area_type] = AREA_TYPE_ATM;
				
				Iter_Add(Areas, Object[oid][object_area]);
				
				// Tworzymy 3d text bankomatu
				Object[oid][object_label] = CreateDynamic3DTextLabel("{00FF00}Bankomat\n"HEX_COLOR_WHITE"(( Wpisz /bankomat stojac bardzo blisko,\nby uzyc bankomatu. ))", COLOR_WHITE, cache_get_float(i, "object_posx"), cache_get_float(i, "object_posy"), cache_get_float(i, "object_posz")+1.0, 4.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, vw);
			}
			
			case OBJECT_TYPE_BUS:
			{
				new bid = GetBusByUid(cache_get_int(i, "object_extraid"));
				if( bid == -1 ) 
				{
					DeleteObject(oid, false);
					continue;
				}
				
				Bus[bid][bus_objectid] = oid;
				
				// Tworzymy strefe bankomatu
				Object[oid][object_area] = CreateDynamicSphere(cache_get_float(i, "object_posx"), cache_get_float(i, "object_posy"), cache_get_float(i, "object_posz"), 6.0, vw);
				Area[Object[oid][object_area]][area_uid] = -1;
				Area[Object[oid][object_area]][area_type] = AREA_TYPE_BUS;
				Area[Object[oid][object_area]][area_owner] = bid;
				
				Iter_Add(Areas, Object[oid][object_area]);
				
				// Tworzymy 3d text przystanku
				Object[oid][object_label] = CreateDynamic3DTextLabel(sprintf("{FFD000}#%d %s\n"HEX_COLOR_WHITE"(( Wpisz /bus aby wybraæ miejsce podró¿y. ))", Bus[bid][bus_uid], Bus[bid][bus_name]), COLOR_WHITE, cache_get_float(i, "object_posx"), cache_get_float(i, "object_posy"), cache_get_float(i, "object_posz")+0.5, 8.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, vw);

			}
			
			case OBJECT_TYPE_GATE:
			{
				Object[oid][object_gatec_pos][0] = cache_get_float(i, "object_posx");
				Object[oid][object_gatec_pos][1] = cache_get_float(i, "object_posy");
				Object[oid][object_gatec_pos][2] = cache_get_float(i, "object_posz");
				Object[oid][object_gatec_pos][3] = cache_get_float(i, "object_rotx");
				Object[oid][object_gatec_pos][4] = cache_get_float(i, "object_roty");
				Object[oid][object_gatec_pos][5] = cache_get_float(i, "object_rotz");
				
				Object[oid][object_gateo_pos][0] = cache_get_float(i, "object_gatex");
				Object[oid][object_gateo_pos][1] = cache_get_float(i, "object_gatey");
				Object[oid][object_gateo_pos][2] = cache_get_float(i, "object_gatez");
				Object[oid][object_gateo_pos][3] = cache_get_float(i, "object_gaterotx");
				Object[oid][object_gateo_pos][4] = cache_get_float(i, "object_gateroty");
				Object[oid][object_gateo_pos][5] = cache_get_float(i, "object_gaterotz");
			}

			case OBJECT_TYPE_DRUG_COOKER:
			{
				// Tworzymy strefe bankomatu
				Object[oid][object_drug_cooked_id] = -1;
				Object[oid][object_area] = CreateDynamicSphere(cache_get_float(i, "object_posx"), cache_get_float(i, "object_posy"), cache_get_float(i, "object_posz"), 2.0, vw);
				Area[Object[oid][object_area]][area_uid] = -1;
				Area[Object[oid][object_area]][area_type] = AREA_TYPE_DRUG_COOKER;
				Area[Object[oid][object_area]][area_owner] = oid;
				
				Iter_Add(Areas, Object[oid][object_area]);
				
				// Tworzymy 3d text bankomatu
				Object[oid][object_label] = CreateDynamic3DTextLabel("{00FF00}Stol alchemiczny\n"HEX_COLOR_WHITE"(( Wpisz /craft stojac bardzo blisko,\nby uzyc stolu. ))", COLOR_WHITE, cache_get_float(i, "object_posx"), cache_get_float(i, "object_posy"), cache_get_float(i, "object_posz")+1.0, 4.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, vw);
			}
		}
		
		// transformacja
		new trans = cache_get_int(i, "has_materials");

		if(trans) {
			Iter_Add(loadMaterials, oid);
		}
		
		Iter_Add(Objects, oid);
	}
	
	cache_delete(result);

	// usuwanie osieroconych obiektow
	if( deleteid > 0 )
	{
		new query_str[2000];
		format(query_str, sizeof(query_str), "DELETE FROM crp_objects WHERE object_uid IN(");
		for(new j;j<deleteid;j++)
		{
			if( to_delete[j] == -1 ) continue;

			format(query_str, sizeof(query_str), "%s%d,", query_str, to_delete[j]);
		}

		strdel(query_str, strlen(query_str)-1, strlen(query_str));
		format(query_str, sizeof(query_str), "%s)", query_str);

		print(query_str);

		mysql_pquery(g_sql, query_str);
	}

	// wczytywanie mmatów
	foreach(new o_id : loadMaterials)
	{
		mysql_pquery(g_sql, sprintf("SELECT * FROM crp_objects_mats WHERE mat_object = %d", Object[o_id][object_uid]), "OnObjectMaterialLoaded", "i", o_id);
	}

	Iter_Clear(loadMaterials);
	
	if( return_id ) return oid;
	return rows;
}

function OnObjectMaterialLoaded(o_id)
{
	new rows = cache_get_rows();
	
	new data[200], type, index, msize, fsize, bold, fcolor[16], bcolor[16], align, font[32], text[100], color[16], mmodel, txd_name[32], texture_name[32];
	for(new i;i<rows;i++)
	{
		type = cache_get_int(i, "mat_type");
		index = cache_get_int(i, "mat_index");
		cache_get(i, "mat_value", data);

		switch(type)
		{
			case 0:
			{
				sscanf(data, "p<:>s[16]ds[32]s[32]", color, mmodel, txd_name, texture_name);

				SetDynamicObjectMaterial(o_id, index, mmodel, txd_name, texture_name, HexToInt(color));
			}
			
			case 1:
			{
				sscanf(data, "p<:>ddds[16]s[16]ds[32] s[100]", msize, fsize, bold, fcolor, bcolor, align, font, text);
				
				
				for(new y=0;y<text[y];y++)
				{
					if(text[y] == '|')
					{
						strdel(text, y, y+1);
						strins(text, "\n", y);
					}
				}
				
				strreplace(text, "(", "{");
				strreplace(text, ")", "}");
				
				SetDynamicObjectMaterialText(o_id, index, text, msize, font, fsize, bold, HexToInt(fcolor), HexToInt(bcolor), align);
			}
		}
	}

	return 1;
}

stock DeleteObject(o_id, bool:from_db = true)
{
	if( from_db )
	{
		mysql_pquery(g_sql, sprintf("DELETE FROM `crp_objects` WHERE `object_uid` = %d", Object[o_id][object_uid]));
		mysql_pquery(g_sql, sprintf("DELETE FROM crp_objects_mats WHERE mat_object= %d", Object[o_id][object_uid]));
	}
	
	DestroyDynamicObject(o_id);
	
	Iter_Remove(Objects, o_id);

	if( IsValidDynamic3DTextLabel(Object[o_id][object_label]) ) DestroyDynamic3DTextLabel(Object[o_id][object_label]);
	if( IsValidDynamicArea(Object[o_id][object_area]) ) DeleteArea(Object[o_id][object_area], false);
	
	for(new z=0; e_objects:z != e_objects; z++)
    {
		Object[o_id][e_objects:z] = 0;
    }
	
	return 1;
}

stock IsObjectEdited(o_id)
{
	if( Object[o_id][object_is_edited] )
	{
		foreach(new p : Player)
		{
			if( pInfo[p][player_edited_object] == o_id ) return 1;
		}
	}
	
	return 0;
}

stock GetObjectDataForPlayer(playerid, &owner, &owner_type)
{
	if( GetPlayerVirtualWorld(playerid) == 0 )
	{
		new a_id = GetPlayerArea(playerid, AREA_TYPE_NORMAL);
		if( a_id != -1 )
		{
			if( !GetPlayerPermissions(playerid, 5) || CanPlayerEditArea(playerid, a_id) )
			{
				owner = Area[a_id][area_uid];
				owner_type = OBJECT_OWNER_TYPE_AREA;
				
				return 1;
			}
		}
		
		if( !GetPlayerPermissions(playerid, 5) )
		{
			owner = 0;
			owner_type = OBJECT_OWNER_TYPE_GLOBAL;
			
			return 1;
		}
	}
	else
	{
		new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
		
		if( d_id == -1 ) return 1;
		if( Door[d_id][door_type] != DOOR_TYPE_NORMAL ) return 1;
		
		if( !GetPlayerPermissions(playerid, 5) )
		{
			owner = Door[d_id][door_uid];
			owner_type = OBJECT_OWNER_TYPE_DOOR;
			
			return 1;
		}
		
		if( CanPlayerEditDoor(playerid, d_id) )
		{
			owner = Door[d_id][door_uid];
			owner_type = OBJECT_OWNER_TYPE_DOOR;
			
			return 1;
		}
	}
	return 1;
}

stock CanPlayerEditObject(playerid, o_id)
{
	if( !GetPlayerPermissions(playerid, 5) ) return 1;
	
	switch( Object[o_id][object_owner_type] )
	{	
		case OBJECT_OWNER_TYPE_DOOR:
		{
			if( CanPlayerEditDoor(playerid, GetDoorByUid(Object[o_id][object_owner])) ) return 1;
		}
		
		case OBJECT_OWNER_TYPE_AREA:
		{
			if( CanPlayerEditArea(playerid, GetAreaByUid(Object[o_id][object_owner])) ) return 1;
		}
	}
	return 0;
}

stock Float:GetPlayerDistanceToObject(playerid, object_id)
{
	if( !IsValidDynamicObject(object_id) ) return 0.0;
	
	new Float:p_pos[3], Float:distance;
	GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
	
	Streamer_GetDistanceToItem(p_pos[0], p_pos[1], p_pos[2], STREAMER_TYPE_OBJECT, object_id, distance);
	return distance;
}

stock UpdateObjectInfoTextdraw(playerid, o_id)
{
	new string[200];

	format(string, sizeof(string), "~n~~n~~y~Obiekt: ~w~%d:%d, ~y~Owner ~w~%d:%d, ~y~Model: ~w~%d", o_id, Object[o_id][object_uid], Object[o_id][object_owner_type], Object[o_id][object_owner], Object[o_id][object_model]);
	format(string, sizeof(string), "%s~n~~p~Pozycja: ~w~%.2f, %.2f, %.2f~n~~r~Rotacja: ~w~%.2f, %.2f, %.2f", string, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
	PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], string);
}

stock GetObjectByUid(uid)
{
	foreach(new o_id : Objects)
	{
		if( Object[o_id][object_uid] == uid ) return o_id;
	}
	return -1;
}

stock RemoveBuildingsForPlayer(playerid)
{
	RemoveBuildingForPlayer(playerid, 985, 2497.4063, 2777.0703, 11.5313, 0.25);
	RemoveBuildingForPlayer(playerid, 986, 2497.4063, 2769.1094, 11.5313, 0.25);
	RemoveBuildingForPlayer(playerid, 7023, 2501.5156, 2781.2891, 9.8203, 0.25);

	// dystrybutory
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1778.4531, 14.1406, 0.25);
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1774.3125, 14.1406, 0.25);
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1771.3438, 14.1406, 0.25);
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1767.2891, 14.1406, 0.25);

	// Pay'n'spray
	RemoveBuildingForPlayer(playerid, 5340, 2643.966064, -2039.156005, 14.374130, 5.843682);
	RemoveBuildingForPlayer(playerid, 5779, 1041.196044, -1025.975952, 31.924230, 5.057569);
	RemoveBuildingForPlayer(playerid, 9093, 2388.426025, 1043.562988, 11.552020, 6.092648);
	RemoveBuildingForPlayer(playerid, 3294, -99.430442, 1111.265991, 22.820320, 6.645609);
	RemoveBuildingForPlayer(playerid, 7507, 1963.546997, 2159.733886, 12.132809, 55.413230);
	RemoveBuildingForPlayer(playerid, 7891, 1968.489013, 2163.300048, 11.999870, 6.277705);
	RemoveBuildingForPlayer(playerid, 1553, 1939.542968, 2199.803955, 11.078929, 3.858560);
	RemoveBuildingForPlayer(playerid, 1552, 1939.663940, 2198.608886, 11.117190, 6.730954);
	RemoveBuildingForPlayer(playerid, 1553, 1987.552001, 2130.847900, 10.664319, 3.858560);
	RemoveBuildingForPlayer(playerid, 1552, 1990.022949, 2130.968994, 11.117190, 6.730954);
	RemoveBuildingForPlayer(playerid, 1553, 1991.381958, 2130.847900, 11.268039, 3.858560);
	RemoveBuildingForPlayer(playerid, 3294, -1421.093017, 2591.303955, 57.706489, 6.645609);
	RemoveBuildingForPlayer(playerid, 9625, -2426.074951, 1028.078002, 51.746631, 6.726603);
	RemoveBuildingForPlayer(playerid, 10575, -2716.299072, 216.756393, 5.531970, 5.989109);
	RemoveBuildingForPlayer(playerid, 11319, -1904.628051, 277.849487, 42.901859, 6.643784);
	RemoveBuildingForPlayer(playerid, 11313, -1935.662963, 239.483993, 35.389560, 5.989067);
	RemoveBuildingForPlayer(playerid, 13028, 719.785400, -462.720794, 16.519800, 4.971903);
	RemoveBuildingForPlayer(playerid, 6400, 486.934387, -1734.508056, 13.618590, 5.242478);
	RemoveBuildingForPlayer(playerid, 5856, 1023.853027, -1029.406005, 33.054649, 4.915647);
	RemoveBuildingForPlayer(playerid, 5422, 2071.539062, -1831.818969, 14.337619, 4.915630);
	// Pay'n'spray

	//South Central - warsztat
	RemoveBuildingForPlayer(playerid, 4075, 1791.7969, -1716.9844, 12.5234, 0.25);
	RemoveBuildingForPlayer(playerid, 4076, 1783.1016, -1702.3047, 14.3516, 0.25);
	RemoveBuildingForPlayer(playerid, 4077, 1781.4766, -1648.4219, 26.8359, 0.25);
	RemoveBuildingForPlayer(playerid, 4078, 1783.1016, -1647.3125, 23.2578, 0.25);
	RemoveBuildingForPlayer(playerid, 1531, 1767.2109, -1617.5391, 15.0391, 0.25);
	RemoveBuildingForPlayer(playerid, 1531, 1799.1328, -1708.7656, 14.1016, 0.25);
	RemoveBuildingForPlayer(playerid, 713, 1807.5156, -1625.8828, 12.7031, 0.25);
	RemoveBuildingForPlayer(playerid, 1266, 1805.0234, -1692.4453, 25.1484, 0.25);
	RemoveBuildingForPlayer(playerid, 4018, 1791.7969, -1716.9844, 12.5234, 0.25);
	RemoveBuildingForPlayer(playerid, 4027, 1783.1016, -1702.3047, 14.3516, 0.25);
	RemoveBuildingForPlayer(playerid, 700, 1777.8516, -1677.1953, 14.3125, 0.25);
	RemoveBuildingForPlayer(playerid, 1294, 1753.7656, -1671.3125, 17.2500, 0.25);
	RemoveBuildingForPlayer(playerid, 1260, 1805.0234, -1692.4453, 25.1484, 0.25);
	RemoveBuildingForPlayer(playerid, 700, 1761.4609, -1651.7422, 14.3125, 0.25);
	RemoveBuildingForPlayer(playerid, 4028, 1781.4766, -1648.4219, 26.8359, 0.25);
	RemoveBuildingForPlayer(playerid, 3984, 1783.1016, -1647.3125, 23.2578, 0.25);
	RemoveBuildingForPlayer(playerid, 1294, 1753.7656, -1632.0625, 17.2500, 0.25);
	RemoveBuildingForPlayer(playerid, 700, 1792.8047, -1640.9531, 14.3125, 0.25);
	RemoveBuildingForPlayer(playerid, 5551, 2140.5156, -1735.1406, 15.8906, 0.25);
	RemoveBuildingForPlayer(playerid, 5410, 2140.5156, -1735.1406, 15.8906, 0.25);
	//Jefferson, stary bar ze sprunkiem
	RemoveBuildingForPlayer(playerid, 17535, 2364.0547, -1391.5313, 41.3516, 0.25);
	RemoveBuildingForPlayer(playerid, 1525, 2346.5156, -1350.7813, 24.2813, 0.25);
	RemoveBuildingForPlayer(playerid, 17964, 2337.3359, -1330.8516, 25.7109, 0.25);
	RemoveBuildingForPlayer(playerid, 17965, 2322.2813, -1355.1953, 25.4063, 0.25);
	RemoveBuildingForPlayer(playerid, 17966, 2347.9219, -1364.2891, 27.1563, 0.25);
	RemoveBuildingForPlayer(playerid, 1261, 2356.3906, -1313.9922, 39.6953, 0.25);
	RemoveBuildingForPlayer(playerid, 1266, 2317.5859, -1355.8281, 37.2188, 0.25);
	RemoveBuildingForPlayer(playerid, 17970, 2364.0547, -1391.5313, 41.3516, 0.25);
	RemoveBuildingForPlayer(playerid, 17543, 2322.2813, -1355.1953, 25.4063, 0.25);
	RemoveBuildingForPlayer(playerid, 1260, 2317.5938, -1355.8203, 37.2266, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 2336.9844, -1350.5781, 22.7266, 0.25);
	RemoveBuildingForPlayer(playerid, 17544, 2337.3359, -1330.8516, 25.7109, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 2338.9141, -1336.4766, 22.7266, 0.25);
	RemoveBuildingForPlayer(playerid, 17542, 2347.9219, -1364.2891, 27.1563, 0.25);
	RemoveBuildingForPlayer(playerid, 955, 2352.1797, -1357.1563, 23.7734, 0.25);
	RemoveBuildingForPlayer(playerid, 1267, 2356.3906, -1313.9922, 39.6953, 0.25);
	//Stacja za szpitalen z net4game
	RemoveBuildingForPlayer(playerid, 5931, 1114.3125, -1348.1016, 17.9844, 0.25);
	RemoveBuildingForPlayer(playerid, 1440, 1085.7031, -1361.0234, 13.2656, 0.25);
	RemoveBuildingForPlayer(playerid, 5810, 1114.3125, -1348.1016, 17.9844, 0.25);
	RemoveBuildingForPlayer(playerid, 5993, 1110.8984, -1328.8125, 13.8516, 0.25);
	RemoveBuildingForPlayer(playerid, 5811, 1131.1953, -1380.4219, 17.0703, 0.25);
	RemoveBuildingForPlayer(playerid, 1440, 1141.9844, -1346.1094, 13.2656, 0.25);
	RemoveBuildingForPlayer(playerid, 1440, 1148.6797, -1385.1875, 13.2656, 0.25);

	//Alhambra
	RemoveBuildingForPlayer(playerid, 5544, 1873.7422, -1682.4766, 34.7969, 0.25);
	RemoveBuildingForPlayer(playerid, 1524, 1837.6641, -1640.3828, 13.7578, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1855.7188, -1741.5391, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1879.5078, -1741.4844, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1908.2188, -1741.4844, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 712, 1929.5781, -1736.9063, 21.3906, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1931.0391, -1726.3281, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1832.3828, -1694.3125, 9.7188, 0.25);
	RemoveBuildingForPlayer(playerid, 1537, 1837.4375, -1683.9688, 12.3047, 0.25);
	RemoveBuildingForPlayer(playerid, 1533, 1837.4375, -1683.9531, 12.3047, 0.25);
	RemoveBuildingForPlayer(playerid, 1537, 1837.4375, -1686.9844, 12.3125, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1832.8984, -1670.7656, 9.7188, 0.25);
	RemoveBuildingForPlayer(playerid, 1533, 1837.4375, -1677.9219, 12.2969, 0.25);
	RemoveBuildingForPlayer(playerid, 1537, 1837.4375, -1680.9531, 12.2969, 0.25);
	RemoveBuildingForPlayer(playerid, 1533, 1837.4375, -1680.9375, 12.2969, 0.25);
	RemoveBuildingForPlayer(playerid, 5408, 1873.7422, -1682.4766, 34.7969, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1931.0391, -1702.2891, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 712, 1929.5781, -1694.4609, 21.3906, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1931.0391, -1667.0313, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1931.0391, -1637.8984, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1855.7188, -1623.2813, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1879.5078, -1623.1016, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1908.2188, -1622.9844, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 712, 1929.5781, -1627.6250, 21.3906, 0.25);

	//Kilka lamp przy stacji idlee jak i p³otków
	RemoveBuildingForPlayer(playerid, 1440, 1085.7031, -1361.0234, 13.2656, 0.25);
	RemoveBuildingForPlayer(playerid, 1440, 1141.9844, -1346.1094, 13.2656, 0.25);
	RemoveBuildingForPlayer(playerid, 1440, 1148.6797, -1385.1875, 13.2656, 0.25);
	//South Central
	RemoveBuildingForPlayer(playerid, 4025, 1777.8359, -1773.9063, 12.5234, 0.25);
	RemoveBuildingForPlayer(playerid, 4070, 1719.7422, -1770.7813, 23.4297, 0.25);
	RemoveBuildingForPlayer(playerid, 4071, 1722.5000, -1775.3984, 14.5156, 0.25);
	RemoveBuildingForPlayer(playerid, 4072, 1777.4375, -1782.3047, 12.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 1531, 1724.7344, -1741.5000, 14.1016, 0.25);
	RemoveBuildingForPlayer(playerid, 4215, 1777.5547, -1775.0391, 36.7500, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1700.8516, -1778.3984, 12.4922, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1701.3047, -1794.4297, 12.5469, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1701.1484, -1753.2266, 12.3516, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1701.2578, -1764.4844, 12.4375, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 1700.8984, -1743.9844, 12.4531, 0.25);
	RemoveBuildingForPlayer(playerid, 3983, 1722.5000, -1775.3984, 14.5156, 0.25);
	RemoveBuildingForPlayer(playerid, 3986, 1719.7422, -1770.7813, 23.4297, 0.25);
	RemoveBuildingForPlayer(playerid, 647, 1791.1797, -1816.6328, 14.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 700, 1788.5078, -1816.5703, 13.3516, 0.25);
	RemoveBuildingForPlayer(playerid, 647, 1796.9609, -1817.3906, 14.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 700, 1801.8828, -1817.3047, 13.3516, 0.25);
	RemoveBuildingForPlayer(playerid, 4012, 1777.4375, -1782.3047, 12.6250, 0.25);
	RemoveBuildingForPlayer(playerid, 4019, 1777.8359, -1773.9063, 12.5234, 0.25);

	//Dwa obiekty obok pizzerii
	RemoveBuildingForPlayer(playerid, 5551, 2140.5156, -1735.1406, 15.8906, 0.25);
	RemoveBuildingForPlayer(playerid, 5410, 2140.5156, -1735.1406, 15.8906, 0.25);
	//Obok pizzerii
	RemoveBuildingForPlayer(playerid, 712, 2100.8125, -1764.3750, 21.3906, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 2105.0859, -1765.6094, 10.8047, 0.25);
	RemoveBuildingForPlayer(playerid, 1226, 2114.7188, -1785.1797, 16.3984, 0.25);
	RemoveBuildingForPlayer(playerid, 1308, 2128.3125, -1786.7109, 12.7031, 0.25);
	//Jefferson, stary bar ze sprunkiem
	RemoveBuildingForPlayer(playerid, 17535, 2364.0547, -1391.5313, 41.3516, 0.25);
	RemoveBuildingForPlayer(playerid, 1525, 2346.5156, -1350.7813, 24.2813, 0.25);
	RemoveBuildingForPlayer(playerid, 17964, 2337.3359, -1330.8516, 25.7109, 0.25);
	RemoveBuildingForPlayer(playerid, 17965, 2322.2813, -1355.1953, 25.4063, 0.25);
	RemoveBuildingForPlayer(playerid, 17966, 2347.9219, -1364.2891, 27.1563, 0.25);
	RemoveBuildingForPlayer(playerid, 1261, 2356.3906, -1313.9922, 39.6953, 0.25);
	RemoveBuildingForPlayer(playerid, 1266, 2317.5859, -1355.8281, 37.2188, 0.25);
	RemoveBuildingForPlayer(playerid, 17970, 2364.0547, -1391.5313, 41.3516, 0.25);
	RemoveBuildingForPlayer(playerid, 17543, 2322.2813, -1355.1953, 25.4063, 0.25);
	RemoveBuildingForPlayer(playerid, 1260, 2317.5938, -1355.8203, 37.2266, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 2336.9844, -1350.5781, 22.7266, 0.25);
	RemoveBuildingForPlayer(playerid, 17544, 2337.3359, -1330.8516, 25.7109, 0.25);
	RemoveBuildingForPlayer(playerid, 620, 2338.9141, -1336.4766, 22.7266, 0.25);
	RemoveBuildingForPlayer(playerid, 17542, 2347.9219, -1364.2891, 27.1563, 0.25);
	RemoveBuildingForPlayer(playerid, 955, 2352.1797, -1357.1563, 23.7734, 0.25);
	RemoveBuildingForPlayer(playerid, 1267, 2356.3906, -1313.9922, 39.6953, 0.25);
}

stock PlayerObjectEdit(playerid, o_id, type)
{
	Object[o_id][object_is_edited] = true;
	pInfo[playerid][player_edited_object] = o_id;

	GetDynamicObjectPos(o_id, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
	GetDynamicObjectRot(o_id, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);

	Object[o_id][object_pos][0] = pInfo[playerid][player_edited_object_pos][0];
	Object[o_id][object_pos][1] = pInfo[playerid][player_edited_object_pos][1];
	Object[o_id][object_pos][2] = pInfo[playerid][player_edited_object_pos][2];
	Object[o_id][object_pos][3] = pInfo[playerid][player_edited_object_pos][3];
	Object[o_id][object_pos][4] = pInfo[playerid][player_edited_object_pos][4];
	Object[o_id][object_pos][5] = pInfo[playerid][player_edited_object_pos][5];

	switch(type)
	{
		case EDITOR_TYPE_SAMP:
		{
			EditDynamicObject(playerid, o_id);
			
			Alert(playerid, ALERT_TYPE_INFO, "Mozesz przelaczyc sie na nasz edytor obiektow wybierajac opcje ~y~Edytor obiektow ~w~w ~g~/stats~w~.");
		}

		case EDITOR_TYPE_CUSTOM:
		{
			ApplyAnimation(playerid, "CRACK", "crckidle1", 4.1, 1, 0, 0, 0, 0, 1);
			pInfo[playerid][player_has_animation] = true;
		}

		default: SendClientMessage(playerid, COLOR_LIGHTER_RED, "> Wyst¹pi³ b³¹d podczas wybierania edytora.");
	}

	UpdateObjectInfoTextdraw(playerid, o_id);
	PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);

	return 1;
}