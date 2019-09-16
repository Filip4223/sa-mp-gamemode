stock LoadLabel(limit[] = "", bool:return_id = false)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_3dlabels` %s", limit));  
	
	new rows = cache_get_rows(), Text3D:label_id, text[200], st[20];
	
	for(new i;i<rows;i++)
	{
		cache_get(i, "label_desc", text);
		
		cache_get(i, "label_color", st);
		format(st, sizeof(st), "0x%sFF", st);
		
		new color = HexToInt(st);
		
		label_id = CreateDynamic3DTextLabel(LabelFormatText(text), color, cache_get_float(i, "label_posx"), cache_get_float(i, "label_posy"), cache_get_float(i, "label_posz"), cache_get_float(i, "label_drawdist"), INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, cache_get_int(i, "label_world"), cache_get_int(i, "label_interior"));
		
		Label[label_id][label_color] = color;
		Label[label_id][label_pos][0] = cache_get_float(i, "label_posx");
		Label[label_id][label_pos][1] = cache_get_float(i, "label_posy");
		Label[label_id][label_pos][2] = cache_get_float(i, "label_posz");
		Label[label_id][label_vw] = cache_get_int(i, "label_world");
		Label[label_id][label_draw] = cache_get_float(i, "label_drawdist");
	
		Label[label_id][label_uid] = cache_get_int(i, "label_uid");
		Label[label_id][label_owner] = cache_get_int(i, "label_owner");
		Label[label_id][label_owner_type] = cache_get_int(i, "label_ownertype");
		
		Iter_Add(Labels, _:label_id);
	}
	
	cache_delete(result);

	if( return_id ) return _:label_id;
	return rows;
}

stock DeleteLabel(l_id, bool:from_database = true)
{
	if( from_database ) mysql_pquery(g_sql, sprintf("DELETE FROM `crp_3dlabels` WHERE `label_uid` = %d", Label[Text3D:l_id][label_uid]));
		
	DestroyDynamic3DTextLabel(Text3D:l_id);
	
	Iter_Remove(Labels, l_id);
	
	for(new z=0; e_labels:z != e_labels; z++)
	{
		Label[Text3D:l_id][e_labels:z] = 0;
	}
	return 1;
}

stock GetLabelDataForPlayer(playerid, &owner, &owner_type)
{
	if( GetPlayerVirtualWorld(playerid) == 0 )
	{
		new a_id = GetPlayerArea(playerid, AREA_TYPE_NORMAL);
		if( a_id != -1 )
		{
			if( CanPlayerEditArea(playerid, a_id) )
			{
				owner = Area[a_id][area_uid];
				owner_type = LABEL_OWNER_TYPE_AREA;
				
				return 1;
			}
		}
		
		if(!GetPlayerPermissions(playerid, 5))
		{
			owner = 0;
			owner_type = LABEL_OWNER_TYPE_GLOBAL;
			
			return 1;
		}
	}
	else
	{
		new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
		
		if( d_id == -1 ) return 1;
		if( Door[d_id][door_type] != DOOR_TYPE_NORMAL ) return 1;
		
		if(!GetPlayerPermissions(playerid, 5))
		{
			owner = Door[d_id][door_uid];
			owner_type = LABEL_OWNER_TYPE_DOOR;
			
			return 1;
		}
		
		if( CanPlayerEditDoor(playerid, d_id) )
		{
			owner = Door[d_id][door_uid];
			owner_type = LABEL_OWNER_TYPE_DOOR;
			
			return 1;
		}
	}
	return 1;
}

stock _:LabelFormatText(text[], length = sizeof text)
{
	new tmpd[200];
	strcopy(tmpd, text, length);
	
	for(new y=0;y<tmpd[y];y++)
	{
		if(tmpd[y] == '|')
		{
			strdel(tmpd, y, y+1);
			strins(tmpd, "\n", y);
		}
	}
					
	strreplace(tmpd, "(", "{");
	strreplace(tmpd, ")", "}");
	return tmpd;
}

stock CanPlayerEditLabel(playerid, l_id)
{
	if(!GetPlayerPermissions(playerid, 5)) return 1;
	
	switch( Label[Text3D:l_id][label_owner_type] )
	{	
		case LABEL_OWNER_TYPE_DOOR:
		{
			new d_id = GetDoorByUid(Label[Text3D:l_id][label_owner]);
			if( d_id == -1 ) return 0;
			
			if( CanPlayerEditDoor(playerid, d_id) ) return 1;
		}
		
		case LABEL_OWNER_TYPE_AREA:
		{
			new a_id =  GetAreaByUid(Label[Text3D:l_id][label_owner]);
			if( a_id == -1 ) return 0;
			if( CanPlayerEditArea(playerid, a_id) ) return 1;
		}
	}
	return 0;
}

stock GetPlayerDistanceTo3DTextLabel(playerid, Text3D:label_id, &Float:distance)
{
	if( !IsValidDynamic3DTextLabel(label_id) ) distance = 1000.0;
	
	new Float:pos[3];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	
	Streamer_GetDistanceToItem(pos[0], pos[1], pos[2], STREAMER_TYPE_3D_TEXT_LABEL, label_id, distance);
}

stock GetNearestLabel(playerid)
{
	new Float:distance = 0.0, Float:pretenderDistance = 30.0, pretenderId = -1;
	foreach(new l_id : Labels)
	{
		if( !CanPlayerEditLabel(playerid, l_id) || Label[Text3D:l_id][label_vw] != GetPlayerVirtualWorld(playerid) ) continue;
		
		GetPlayerDistanceTo3DTextLabel(playerid, Text3D:l_id, distance);
		
		if( distance < pretenderDistance )
		{
			pretenderDistance = distance;
			pretenderId = l_id;
		}
	}
	
	return pretenderId;
}

