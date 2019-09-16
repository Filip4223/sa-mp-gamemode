#define RANDOMFIRE_SPREAD_DELAY 20
#define RANDOMFIRE_DELAY 1*60

// Enumerator po¿arów
enum e_fire
{
	fire_last_time,
	bool:fire_is_burning,
	fire_map_icon,
	fire_last_spread
}
new Fire[e_fire];

// Enumerator zrodel ognia
enum e_fire_sources
{
	fs_object,
	Text3D:fs_label,
	Float:fs_health,
	fs_start_time
}
new FireSource[MAX_FIRE_SOURCES][e_fire_sources];

stock ProcessRandomFires()
{
	if( Iter_Count(FirePlaces) == 0 ) return;
	if( gettime() - Fire[fire_last_time] > RANDOMFIRE_DELAY && !Fire[fire_is_burning] )
	{
		Fire[fire_last_time] = gettime();

		// sprawdzamy ile fd'ków jest na duty

		if( GetFiremansCount() > 0 )
		{
			Fire[fire_is_burning] = true;
			Fire[fire_last_spread] = gettime();
			
			new fid = Iter_Random(FirePlaces);
			
			Fire[fire_map_icon] = CreateDynamicMapIcon(Area[fid][area_pos][0], Area[fid][area_pos][1], Area[fid][area_pos][2], 20, 0, -1, -1, 0, 100000.0);
			Streamer_SetIntData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_STYLE, 1);
			Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, 0);
			
			CreateFireSource(Area[fid][area_pos][0], Area[fid][area_pos][1], Area[fid][area_pos][2]);
			
			// Wysylamy wiadomosc do strazakow
			foreach(new p : Player)
			{
				new gslot = GetPlayerDutySlot(p);
				if( gslot == -1 ) continue;
				if( Group[pGroup[p][gslot][pg_id]][group_type] != GROUP_TYPE_FD ) continue;
				
				SendClientMessage(p, COLOR_LIGHTER_RED, "Wybuch³ po¿ar! Miejsce po¿aru zosta³o oznaczone na mapie.");
				SendClientMessage(p, COLOR_LIGHTER_RED, "Nie zwlekaj z wyjazdem poniewa¿ ogieñ mo¿e siê rozprzestrzeniaæ!");
				
				Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, p);
			}
		}
	}

	if( Fire[fire_is_burning] )
	{
		if( Iter_Free(FireSources) == INVALID_ITERATOR_SLOT ) return;

		if( gettime() - Fire[fire_last_spread] > RANDOMFIRE_SPREAD_DELAY )
		{			
			new bool:found = false;
			new Float:pos[3];
			
			new bool:isany = false;
			foreach(new x : FireSources)
			{
				if( FireSource[x][fs_health] == 1000.0 )
				{
					isany = true;
					break;
				}
			}
			
			if( isany )
			{
				new loops = 0;
				while(!found)
				{
					loops++;

					if( loops > 10000 )
					{
						print("------------ DEEEEBUG fires:113 -----------------");
						break;
					}

					new fsid = Iter_Random(FireSources);
					if( FireSource[fsid][fs_health] < 1000.0 ) continue;
					
					Streamer_GetFloatData(STREAMER_TYPE_OBJECT, FireSource[fsid][fs_object], E_STREAMER_X, pos[0]);
					Streamer_GetFloatData(STREAMER_TYPE_OBJECT, FireSource[fsid][fs_object], E_STREAMER_Y, pos[1]);
					Streamer_GetFloatData(STREAMER_TYPE_OBJECT, FireSource[fsid][fs_object], E_STREAMER_Z, pos[2]);
					
					if( pos[0] == 0.0 || pos[1] == 0.0 ) continue;

					new rnd = rand(75, 100);
					new Float:tmp = floatdiv(rnd, 100.0);
				
					if( random(2) == 0 ) pos[0] -= tmp;
					else pos[0] += tmp;
					
					rnd = rand(75, 100);
					tmp = floatdiv(rnd, 100.0);
					
					if( random(2) == 0 ) pos[1] -= tmp;
					else pos[1] += tmp;
					
					new bool:can;
					
					foreach(new x : FireSources)
					{
						new Float:distance;
						Streamer_GetDistanceToItem(pos[0], pos[1], pos[2], STREAMER_TYPE_OBJECT, FireSource[x][fs_object], distance, 2);
						
						if( distance < 1.0 )
						{
							can = true;
							break;
						}
					}
					
					if( !can )
					{
						found = true;
						Fire[fire_last_spread] = gettime();
						CreateFireSource(pos[0], pos[1], pos[2]+2.5);
					}
					
				}
			}
			else
			{
				Fire[fire_last_spread] = gettime();
			}
		}
	}
}

stock CreateFireSource(Float:x, Float:y, Float:z)
{
	new fsid = Iter_Free(FireSources);
	if( fsid == INVALID_ITERATOR_SLOT ) return 1;
	
	FireSource[fsid][fs_object] = CreateDynamicObject(18688, x, y, z-2.5, 0.0, 0.0, 0.0);
	Object[FireSource[fsid][fs_object]][object_type] = OBJECT_TYPE_FIRE_SOURCE;
	Iter_Add(Objects, FireSource[fsid][fs_object]);
	
	FireSource[fsid][fs_health] = 1000.0;
	FireSource[fsid][fs_label] = CreateDynamic3DTextLabel("100%", 0xF07800FF, x, y, z, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1);
	FireSource[fsid][fs_start_time] = gettime();
	
	Iter_Add(FireSources, fsid);
	
	printf("Creating fire source (fsid: %d;x: %f;y: %f;z: %f)", fsid, x, y, z);
	
	return 1;
}

stock StopFireSource(fsid)
{
	DeleteObject(FireSource[fsid][fs_object], false);
	if( IsValidDynamic3DTextLabel(FireSource[fsid][fs_label]) ) DestroyDynamic3DTextLabel(FireSource[fsid][fs_label]);
	
	for(new z=0; e_fire_sources:z != e_fire_sources; z++)
    {
		FireSource[fsid][e_fire_sources:z] = 0;
    }
	
	Iter_Remove(FireSources, fsid);
	
	printf("Stopping fire source (fsid: %d;count: %d)", fsid, Iter_Count(FireSources));
	
	if( Iter_Count(FireSources) == 0 ) StopFire();
}

stock StopFire()
{
	Fire[fire_is_burning] = false;

	new str[150], ttime = gettime() - Fire[fire_last_time];
	format(str, sizeof(str), ""HEX_COLOR_LIGHTER_ORANGE"Po¿ar zosta³ ugaszony przez stra¿aków po %dmin i %ds.", floatround(ttime/60, floatround_floor), ttime%60);
	
	new count = 0;
	
	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		pInfo[p][player_was_extinguishing] = false;

		new slot = GetPlayerDutySlot(p);
		if(slot == -1) continue;

		new gid = pGroup[p][slot][pg_id];
		if(Group[gid][group_type] != GROUP_TYPE_FD) continue;

		Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, p);
		
		if( pInfo[p][player_was_extinguishing] )
		{
			// Otrzymuj¹ nagrodê
			count++;

			GiveGroupMoney(gid, 50);
			
			SendGuiInformation(p, "Informacja", sprintf("Bra³eœ udzia³ w gaszeniu po¿aru, który w³aœnie zosta³ ugaszony (po %dmin i %ds). Otrzymujesz 1 pkt. score oraz 1 punkt aktywnoœci.", floatround(ttime/60, floatround_floor), ttime%60));
		}
		else
		{
			SendClientMessage(p, -1, str);
		}
	}
	
	DestroyDynamicMapIcon(Fire[fire_map_icon]);
	
	Fire[fire_last_time] = gettime();
}

stock GetFiremansCount()
{
	new count = 0;
	foreach(new p : Player)
	{
		new slot = GetPlayerDutySlot(p);

		if( slot > -1 )
		{
			if(Group[pGroup[p][slot][pg_id]][group_type] == GROUP_TYPE_FD) count++;
		}
	}


	return count;
}

stock FireDepDutyOff_hook(playerid, slot)
{
	if(!Fire[fire_is_burning]) return;

	new gid = pGroup[playerid][slot][pg_id];

	if(Group[gid][group_type] == GROUP_TYPE_FD)
	{
		Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, playerid);
	}
}

stock FireDepDutyOn_hook(playerid, slot)
{
	if(!Fire[fire_is_burning]) return;

	new gid = pGroup[playerid][slot][pg_id];

	if(Group[gid][group_type] == GROUP_TYPE_FD)
	{
		SendClientMessage(playerid, COLOR_LIGHTER_RED, "Aktualnie trwa po¿ar! Miejsce po¿aru zosta³o oznaczone na mapie.");
		
		Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, playerid);
	}
}

stock ProcessFireExtinguish(playerid)
{
	new slot = GetPlayerDutySlot(playerid);
	if(slot == -1) return;

	new gid = pGroup[playerid][slot][pg_id];
	if(Group[gid][group_type] != GROUP_TYPE_FD) return;

	if( pInfo[playerid][player_holding_fire] && GetPlayerWeapon(playerid) == 42 )
	{
		new Float:exting_pos[3];
		GetPointInAngleOfPlayer(playerid, exting_pos[0], exting_pos[1], exting_pos[2], 1.0, 0.0);
		
		new Float:distance;
		
		for(new fsid;fsid<MAX_FIRE_SOURCES;fsid++)
		{
			if(!Iter_Contains(FireSources, fsid)) continue;
			
			Streamer_GetDistanceToItem(exting_pos[0], exting_pos[1], exting_pos[2]-1.5, STREAMER_TYPE_OBJECT, FireSource[fsid][fs_object], distance);
			
			if( distance < 1.5 )
			{
				if( IsValidDynamicObject(FireSource[fsid][fs_object]) )
				{
					pInfo[playerid][player_was_extinguishing] = true;
					
					if( FireSource[fsid][fs_health] - 80.0 < 0 )
					{
						StopFireSource(fsid);

						SendPlayerInformation(playerid, "~y~Ugasiles zrodlo ognia", 4000);
					}
					else
					{
						FireSource[fsid][fs_health] -= 80.0;
						
						new str[10];
						format(str, sizeof(str), "%d%", floatround(floatdiv(FireSource[fsid][fs_health], 1000.0)*100));
						UpdateDynamic3DTextLabelText(FireSource[fsid][fs_label], 0xF07800FF, str);
					}
					break;
				}
			}
		}
	}
	/* BEZ FIRETRUCKA PÓKI CO
	new vid = GetPlayerVehicleID(playerid);
	if( vid != INVALID_VEHICLE_ID && GetPlayerVehicleSeat(playerid) == 0 )
	{
		// fire truck
		if(Vehicle[vid][vehicle_model] == 407 && pInfo[playerid][player_holding_fire])
		{
			new Float:cx, Float:cy, Float:cz, Float:vx, Float:vy, Float:vz;

			GetPlayerCameraPos(playerid, cx, cy, cz);
			GetPlayerCameraFrontVector(playerid, vx, vy, vz);

			new Float:distance;

			foreach(new fsid : FireSources)
			{			
				Streamer_GetDistanceToItem(cx, cy, cz, STREAMER_TYPE_OBJECT, FireSource[fsid][fs_object], distance);

				cx = cx + vx * distance;
				cy = cy + vy * distance;
				cz = cz + vz * distance;

				new Float:current_angle = (atan2(vy,vx)-90.0);
				while(current_angle < 0.0) current_angle += 360.0;
				while(current_angle >= 360.0) current_angle -= 360.0;

				new Float:a = current_angle - 90.0;
	
				cx += (1 * floatsin(-a, degrees));
				cy += (1 * floatcos(-a, degrees));

				CreateObject(11706, cx, cy, cz, 0.0, 0.0, 0.0);

				Streamer_GetDistanceToItem(cx, cy, cz, STREAMER_TYPE_OBJECT, FireSource[fsid][fs_object], distance);
				
				if( distance < 1.5 )
				{
					if( IsValidDynamicObject(FireSource[fsid][fs_object]) )
					{
						pInfo[playerid][player_was_extinguishing] = true;
						
						if( FireSource[fsid][fs_health] - 180.0 < 0 )
						{
							StopFireSource(fsid);

							SendPlayerInformation(playerid, "~y~Ugasiles zrodlo ognia", 4000);
						}
						else
						{
							FireSource[fsid][fs_health] -= 180.0;
							
							new str[10];
							format(str, sizeof(str), "%d%", floatround(floatdiv(FireSource[fsid][fs_health], 1000.0)*100));
							UpdateDynamic3DTextLabelText(FireSource[fsid][fs_label], 0xF07800FF, str);
						}
						break;
					}
				}
			}
		}
	}*/
}