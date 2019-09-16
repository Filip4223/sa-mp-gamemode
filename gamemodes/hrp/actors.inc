stock LoadActor(limit[] = "", bool:return_id = false)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT * FROM crp_actors %s", limit));  
	
	new rows = cache_get_rows(), a_id, Text3D:l_id, area_id;
	
	for(new i;i<rows;i++)
	{
		a_id = CreateDynamicActor(cache_get_int(i, "actor_skin"), cache_get_float(i, "actor_px"), cache_get_float(i, "actor_py"), cache_get_float(i, "actor_pz"), cache_get_float(i, "actor_pa"), 1, 100.0, cache_get_int(i, "actor_vw"), cache_get_int(i, "actor_int"));
		
		Iter_Add(Actors, a_id);	
		
		cache_get(i, "actor_name", Actor[a_id][actor_name]);
		cache_get(i, "actor_anim", Actor[a_id][actor_anim]);

		Actor[a_id][actor_uid] = cache_get_int(i, "actor_uid");
		Actor[a_id][actor_type] = cache_get_int(i, "actor_type");
		Actor[a_id][actor_skin] = cache_get_int(i, "actor_skin");
		Actor[a_id][actor_pos_x] = cache_get_float(i, "actor_px");
		Actor[a_id][actor_pos_y] = cache_get_float(i, "actor_py");
		Actor[a_id][actor_pos_z] = cache_get_float(i, "actor_pz");
		Actor[a_id][actor_pos_a] = cache_get_float(i, "actor_pa");
		Actor[a_id][actor_interior] = cache_get_int(i, "actor_int");
		Actor[a_id][actor_virtual] = cache_get_int(i, "actor_vw");
		Actor[a_id][actor_type] = cache_get_int(i, "actor_type");

		if( !isnull(Actor[a_id][actor_anim]) ) _ApplyDynamicActorAnimation(a_id, Actor[a_id][actor_anim]);

		l_id = CreateDynamic3DTextLabel(sprintf("(( %d. %s ))\n(aktor)", a_id, Actor[a_id][actor_name]), 0xADAB3EB7, Actor[a_id][actor_pos_x], Actor[a_id][actor_pos_y], Actor[a_id][actor_pos_z]+1.1, 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Actor[a_id][actor_virtual], Actor[a_id][actor_interior]);
		Actor[a_id][actor_label_id] = l_id;

		if(Actor[a_id][actor_type] != 0)
		{
			area_id = CreateDynamicSphere(Actor[a_id][actor_pos_x], Actor[a_id][actor_pos_y], Actor[a_id][actor_pos_z], 5.0);
			Area[area_id][area_type] = AREA_TYPE_ACTOR;
		}
	}

	cache_delete(result);
	
	if( return_id ) return a_id;
	return rows;
}

stock DeleteActor(a_id, bool:from_mysql = true)
{	
	if( from_mysql ) mysql_pquery(g_sql, sprintf("DELETE FROM `crp_actors` WHERE `actor_uid` = %d", Actor[a_id][actor_uid]));
	
	Iter_Remove(Actors, a_id);
	
	DestroyDynamic3DTextLabel(Actor[a_id][actor_label_id]);
	DestroyDynamicActor(a_id);
}

stock GetActorByUid(uid)
{
	foreach(new a_id : Actors)
	{
		if( Actor[a_id][actor_uid] == uid ) return a_id;
	}
	
	return -1;
}

stock _ApplyDynamicActorAnimation(a_id, name[])
{
	strreplace_char(name, '.', '.');
	mysql_pquery(g_sql, sprintf("SELECT * FROM `crp_anim` WHERE `anim_command` LIKE '.%s' AND anim_action = 0", name), "DynamicActorAnimationResponse", "d", a_id);
}

function DynamicActorAnimationResponse(a_id)
{
	new anim_lib[24], anim_name[24];

	if( cache_get_rows() )
	{
		cache_get(0, "anim_lib", anim_lib);
		cache_get(0, "anim_name", anim_name);

		ApplyDynamicActorAnimation(a_id, anim_lib, anim_name, cache_get_float(0, "anim_speed"), cache_get_int(0, "anim_opt1"), cache_get_int(0, "anim_opt2"), cache_get_int(0, "anim_opt3"), cache_get_int(0, "anim_opt4"), cache_get_int(0, "anim_opt5"));
	}
}

stock GetNearestActor(playerid)
{
    new Float:pretender_distance = 5.0, Float:distance, pretender_id = -1;
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    foreach(new actorid : Actors)
    {
        if( GetPlayerVirtualWorld(playerid) == Actor[actorid][actor_virtual] && GetPlayerInterior(playerid) == Actor[actorid][actor_interior] )
        {
            Streamer_GetDistanceToItem(x, y, z, STREAMER_TYPE_ACTOR, actorid, distance);

            if(distance < pretender_distance)
            {
                pretender_distance = distance;
                pretender_id = actorid;
            }
        }
    }

    return pretender_id;
}


stock TeleportPlayerInFrontOfActor(playerid, actorid)
{
	new id, Float:x, Float:y, Float:z, Float:a;

    GetPointInAngleOfActor(actorid, x, y, z, 1.0, 0.0);
	GetDynamicActorFacingAngle(id, a);

	SetPlayerPos(playerid, x, y, z);

	a += 180.0;
	if( a > 360.0 ) a -= 360.0;
	SetPlayerFacingAngle(playerid, a);
    
}