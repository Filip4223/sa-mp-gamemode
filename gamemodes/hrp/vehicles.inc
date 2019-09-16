new VehicleNames[212][] = {
   "Landstalker",  "Bravura",  "Buffalo", "Linerunner", "Perennial", "Sentinel",
   "Dumper",  "Firetruck" ,  "Trashmaster" ,  "Stretch",  "Manana",  "Infernus",
   "Voodoo", "Pony",  "Mule", "Cheetah", "Ambulance",  "Leviathan",  "Moonbeam",
   "Esperanto", "Taxi",  "Washington",  "Bobcat",  "Mr Whoopee", "BF Injection",
   "Hunter", "Premier",  "Enforcer",  "Securicar", "Banshee", "Predator", "Bus",
   "Rhino",  "Barracks",  "Hotknife",  "Trailer",  "Previon", "Coach", "Cabbie",
   "Stallion", "Rumpo", "RC Bandit",  "Romero", "Packer", "Monster",  "Admiral",
   "Squalo", "Seasparrow", "Pizzaboy", "Tram", "Trailer",  "Turismo", "Speeder",
   "Reefer", "Tropic", "Flatbed","Yankee", "Caddy", "Solair","Berkley's RC Van",
   "Skimmer", "PCJ-600", "Faggio", "Freeway", "RC Baron","RC Raider","Glendale",
   "Oceanic", "Sanchez", "Sparrow",  "Patriot", "Quad",  "Coastguard", "Dinghy",
   "Hermes", "Sabre", "Rustler", "ZR-350", "Walton",  "Regina",  "Comet", "BMX",
   "Burrito", "Camper", "Marquis", "Baggage", "Dozer","Maverick","News Chopper",
   "Rancher", "FBI Rancher", "Virgo", "Greenwood","Jetmax","Hotring","Sandking",
   "Blista Compact", "Police Maverick", "Boxville", "Benson","Mesa","RC Goblin",
   "Hotring Racer", "Hotring Racer", "Bloodring Banger", "Rancher",  "Super GT",
   "Elegant", "Journey", "Bike", "Mountain Bike", "Beagle", "Cropdust", "Stunt",
   "Tanker", "RoadTrain", "Nebula", "Majestic", "Buccaneer", "Shamal",  "Hydra",
   "FCR-900","NRG-500","HPV1000","Cement Truck","Tow Truck","Fortune","Cadrona",
   "FBI Truck", "Willard", "Forklift","Tractor","Combine","Feltzer","Remington",
   "Slamvan", "Blade", "Freight", "Streak","Vortex","Vincent","Bullet","Clover",
   "Sadler",  "Firetruck", "Hustler", "Intruder", "Primo", "Cargobob",  "Tampa",
   "Sunrise", "Merit",  "Utility Truck",  "Nevada", "Yosemite", "Windsor",  "Monster",
   "Monster","Uranus","Jester","Sultan","Stratum","Elegy","Raindance","RCTiger",
   "Flash","Tahoma","Savanna", "Bandito", "Freight", "Trailer", "Kart", "Mower",
   "Dune", "Sweeper", "Broadway", "Tornado", "AT-400",  "DFT-30", "Huntley",
   "Stafford", "BF-400", "Newsvan","Tug","Trailer","Emperor","Wayfarer","Euros",
   "Hotdog", "Club", "Trailer", "Trailer","Andromada","Dodo","RC Cam", "Launch",
   "Police Car (LSPD)", "Police Car (SFPD)","Police Car (LVPD)","Police Ranger",
   "Picador",   "S.W.A.T. Van",  "Alpha",   "Phoenix",   "Glendale",   "Sadler",
   "Luggage Trailer","Luggage Trailer","Stair Trailer", "Boxville", "Farm Plow",
   "Utility Trailer"
};

new Float:VehicleEngineCapacity[212] = {
	3.6,  1.8,  3.8, 9.4, 1.9, 2.6,
	28.0,  6.6 ,  5.8 ,  3.0,  1.6,  8.3,
	2.8, 3.5,  4.0, 8.0, 5.8,  10.0,  3.0,
	2.4, 2.0,  3.3,  2.5,  2.5, 2.0,
	15.0, 2.8,  5.0,  4.5, 6.4, 4.4, 6.0,
	25.0,  7.0,  3.4,  0.0,  1.5, 7.0, 1.8,
	1.8, 2.5, 0.0,  2.5, 7.0, 6.5,  1.8,
	5.0, 9.0, 0.2, 0.0, 0.0, 6.2, 6.0,
	6.5, 6.5, 6.8, 4.5, 0.6, 2.2, 2.6,
	5.5, 1.0, 0.2, 1.4, 0.0, 0.0, 2.3,
	2.0, 0.8, 5.5, 4.2, 0.7,  5.5, 3.0,
	2.8, 4.0, 6.7, 4.2, 1.6,  1.7,  4.4, 0.0,
	3.6, 2.2, 4.6, 1.0, 5.6, 6.0, 6.0,
	5.2, 5.5, 2.4, 1.8, 6.0, 5.2, 6.4,
	1.6, 6.5, 2.8, 2.8, 3.2, 0.0,
	5.0, 5.0, 5.0, 5.2, 5.8,
	2.8, 3.5, 0.0, 0.0, 6.8, 5.8, 5.5,
	10.1, 11.4, 1.8, 1.6, 2.4, 15.0, 30.0,
	1.3, 1.6, 1.4, 8.0, 4.0, 1.6, 1.6,
	4.2, 1.8, 0.6, 0.8, 4.5, 2.0, 2.3,
	2.4, 2.4, 0.0, 0.0, 1.4, 1.8, 7.3, 2.0,
	1.8, 7.5, 2.0, 1.8, 1.6, 14.0, 2.2,
	1.8, 1.8, 3.2, 20.0, 3.6, 1.6, 7.0,
	7.0, 1.8, 4.6, 3.4, 2.4, 3.4, 11.0, 0.0,
	2.0, 1.8, 2.2, 1.2, 0.0, 0.0, 0.2, 0.8,
	7.0, 0.8, 2.2, 2.4, 18.0, 6.2, 4.2,
	2.8, 1.1, 3.0, 0.4, 0.0, 1.8, 1.3, 3.0,
	2.8, 1.9, 0.0, 0.0, 40.0, 9.0, 0.0, 8.0,
	2.5, 2.5, 2.5, 4.4,
	2.1, 6.0, 4.0, 4.2, 1.8, 1.6,
	0.0, 0.0, 0.0, 4.6, 0.0,
	0.0
};

new VehicleFuelMax[212] = {
80, 30, 60, 400, 40, 50, 400, 250, 250, 80, 30, 60, 50, 70, 80, 60, 85, 900, 40, 45, 50, 70, 50, 30, 25, 900, 80, 65, 65, 50, 0, 120, 1020, 140, 40, 0, 30, 150, 45, 40, 60, 10, 
60, 120, 100, 50, 0, 120, 10, 0, 0, 60, 0, 100, 100, 120, 70, 25, 40, 60, 200, 30, 10, 25, 100, 100, 30, 30, 20, 100, 80, 25, 0, 0, 30, 50, 160, 50, 35, 40, 60, 0, 70, 70, 0, 30, 
160, 350, 250, 80, 120, 40, 50, 0, 100, 80, 40, 400, 80, 60, 50, 100, 100, 100, 100, 80, 50, 80, 80, 0, 0, 350, 200, 250, 400, 400, 60, 50, 40, 1200, 2000, 35, 45, 50, 70, 70, 
30, 30, 90, 60, 15, 25, 80, 50, 60, 70, 50, 0, 0, 60, 70, 70, 40, 40, 150, 40, 50, 50, 450, 50, 60, 80, 80, 1000, 100, 50, 100, 100, 75, 70, 85, 90, 60, 500, 100, 50, 65, 65, 
30, 0, 0, 10, 15, 120, 15, 50, 65, 1200, 150, 90, 60, 35, 80, 30, 0, 45, 45, 60, 120, 45, 0, 0, 2000, 300, 100, 0, 80, 80, 70, 90, 60, 150, 60, 50, 30, 30, 0, 0, 0, 120, 0, 0
};

stock LoadVehicle(limit[] = "", bool:return_id = false)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_vehicles` %s", limit));
	
	new rows = cache_get_rows(), vid = INVALID_VEHICLE_ID;
	
	for(new i;i<rows;i++)
	{
		if( GetVehicleByUid(cache_get_int(i, "vehicle_uid")) != INVALID_VEHICLE_ID ) continue;
		
		// Pobieramy dane potrzebne do stworzenia pojazdu
		new tmp_model, Float:tmp_park[4], tmp_color[2], tmp_int, tmp_vw;
		
		tmp_model = cache_get_int(i, "vehicle_model");
		
		tmp_color[0] = cache_get_int(i, "vehicle_color1");
		tmp_color[1] = cache_get_int(i, "vehicle_color2");

		if( cache_get_int(i, "vehicle_spawn_restart") == 1 )
		{
			new lastpos[100];
			cache_get(i, "vehicle_lastpos", lastpos);

			sscanf(lastpos, "p<:>ffffdd", tmp_park[0], tmp_park[1], tmp_park[2], tmp_park[3], tmp_int, tmp_vw);
		}
		else
		{
			tmp_park[0] = cache_get_float(i, "vehicle_posx");
			tmp_park[1] = cache_get_float(i, "vehicle_posy");
			tmp_park[2] = cache_get_float(i, "vehicle_posz");
			tmp_park[3] = cache_get_float(i, "vehicle_posa");
			tmp_vw = cache_get_int(i, "vehicle_world");
			tmp_int = cache_get_int(i, "vehicle_interior");

		}
		
		// Tworzymy pojazd
		vid = CreateVehicle(tmp_model, tmp_park[0], tmp_park[1], tmp_park[2], tmp_park[3], tmp_color[0], tmp_color[1], -1);	
		LinkVehicleToInterior(vid, cache_get_int(i, "vehicle_interior"));
		SetVehicleVirtualWorld(vid, cache_get_int(i, "vehicle_world"));
		
		Vehicle[vid][vehicle_park_world] = cache_get_int(i, "vehicle_world");
		Vehicle[vid][vehicle_park_interior] = cache_get_int(i, "vehicle_interior");

		// Wpychamy dane pojazdu do enumeratora
		Vehicle[vid][vehicle_uid] = cache_get_int(i, "vehicle_uid");
		Vehicle[vid][vehicle_model] = tmp_model;
		Vehicle[vid][vehicle_owner_type] = cache_get_int(i, "vehicle_ownertype");
		Vehicle[vid][vehicle_owner] = cache_get_int(i, "vehicle_owner"); 
		Vehicle[vid][vehicle_color][0] = tmp_color[0];
		Vehicle[vid][vehicle_color][1] = tmp_color[1];
		Vehicle[vid][vehicle_paintjob] = cache_get_int(i, "vehicle_paintjob");
		Vehicle[vid][vehicle_park][0] = cache_get_float(i, "vehicle_posx");
		Vehicle[vid][vehicle_park][1] = cache_get_float(i, "vehicle_posy");
		Vehicle[vid][vehicle_park][2] = cache_get_float(i, "vehicle_posz");
		Vehicle[vid][vehicle_park][3] = cache_get_float(i, "vehicle_posa");
		Vehicle[vid][vehicle_accessories] = cache_get_int(i, "vehicle_access");
		Vehicle[vid][vehicle_health] = cache_get_float(i, "vehicle_health");
		
		Vehicle[vid][vehicle_mileage] = cache_get_float(i, "vehicle_mileage");
		
		Vehicle[vid][vehicle_last_pos][0] = tmp_park[0];
		Vehicle[vid][vehicle_last_pos][1] = tmp_park[1];
		Vehicle[vid][vehicle_last_pos][2] = tmp_park[2];
		
		Vehicle[vid][vehicle_map_icon] = CreateDynamicMapIcon(Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2], 55, 0, 0, 0, 0, 10000.0);
		Streamer_SetIntData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_STYLE, 3);
		Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_PLAYER_ID, 0);
		
		Vehicle[vid][vehicle_description] = CreateDynamic3DTextLabel("", LABEL_DESCRIPTION, 0.0, 0.0, 0.0, 4.0, INVALID_PLAYER_ID, vid);
		
		new tmp_dmg[80];
		cache_get(i, "vehicle_visual", tmp_dmg);	
		sscanf(tmp_dmg, "a<d>[4]", Vehicle[vid][vehicle_damage]);
		
		Vehicle[vid][vehicle_engine] = false;
		Vehicle[vid][vehicle_lights] = false;
		Vehicle[vid][vehicle_boot] = false;
		Vehicle[vid][vehicle_bonnet] = false;
		Vehicle[vid][vehicle_destroyed] = false;
		
		Vehicle[vid][vehicle_locked] = true;
		Vehicle[vid][vehicle_last_fuel_td] = gettime();
		Vehicle[vid][vehicle_driver] = INVALID_PLAYER_ID;
		
		// Wczytywanie paliwa
		Vehicle[vid][vehicle_fuel_current] = cache_get_int(i, "vehicle_fuel");
		Vehicle[vid][vehicle_fuel_type] = cache_get_int(i, "vehicle_fueltype");

		// blokada
		Vehicle[vid][vehicle_block] = cache_get_bool(i, "vehicle_block");
		Vehicle[vid][vehicle_block_price] = cache_get_int(i, "vehicle_blockprice");
		Vehicle[vid][vehicle_block_group] = cache_get_int(i, "vehicle_blockgroup");

		SetVehicleNumberPlate(vid, sprintf("LS%04d", Vehicle[vid][vehicle_uid]));
		
		Iter_Add(Vehicles, vid);
		
		// Wykrywanie zniszczonych pojazdow
		if( Vehicle[vid][vehicle_health] <= 250.0 )
		{
			Vehicle[vid][vehicle_health] = 300.0;
			Vehicle[vid][vehicle_destroyed] = true;
			SetVehicleHealth(vid, 300.0);
		}
		else SetVehicleHealth(vid, Vehicle[vid][vehicle_health]);
		
		if( Vehicle[vid][vehicle_paintjob] >= 0 && Vehicle[vid][vehicle_paintjob] < 3 ) ChangeVehiclePaintjob(vid, Vehicle[vid][vehicle_paintjob]);
		
		UpdateVehicleVisuals(vid);
		UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);

		// Tworzenie labela statusów
		Vehicle[vid][vehicle_state_label] = CreateDynamic3DTextLabel("", COLOR_WHITE, 0.0, 0.0, 1.0, 10.0, INVALID_PLAYER_ID, vid, 0, 0, 0, -1, 50.0);
	}
	
	cache_delete(result);

	if( return_id ) return vid;
	return rows;
}

stock DeleteVehicle(vehicleid, bool:from_db = false)
{
	if( from_db ) mysql_pquery(g_sql, sprintf("DELETE FROM `crp_vehicles` WHERE `vehicle_uid` = %d", Vehicle[vehicleid][vehicle_uid]));
	else SaveVehicle(vehicleid);
	
	DestroyDynamic3DTextLabel(Vehicle[vehicleid][vehicle_state_label]);
	DestroyDynamic3DTextLabel(Vehicle[vehicleid][vehicle_description]);
	
	for(new z=0; e_vehicles:z != e_vehicles; z++)
	{
		Vehicle[vehicleid][e_vehicles:z] = 0;
	}
		
	Iter_Remove(Vehicles, vehicleid);
	
	DestroyVehicle(vehicleid);
}

stock UpdateVehicleVisuals(vehicleid)
{
	SetVehicleParamsEx(vehicleid, Vehicle[vehicleid][vehicle_engine], Vehicle[vehicleid][vehicle_lights], false, false, Vehicle[vehicleid][vehicle_bonnet], Vehicle[vehicleid][vehicle_boot], false);
}

stock SaveVehicle(vehicleid, bool:arestart=false)
{
	new visual_damage[32];
	format(visual_damage, sizeof(visual_damage), "%d %d %d %d", Vehicle[vehicleid][vehicle_damage][0], Vehicle[vehicleid][vehicle_damage][1], Vehicle[vehicleid][vehicle_damage][2], Vehicle[vehicleid][vehicle_damage][3]);

	new Float:saveHealth = Vehicle[vehicleid][vehicle_health];
	if( Vehicle[vehicleid][vehicle_destroyed] ) saveHealth = 0.0;

	new arestart_pos[100];
	if(arestart)
	{
		format(arestart_pos, sizeof(arestart_pos), ", `vehicle_spawn_restart` = 1");
	}

	new Float:x, Float:y, Float:z, Float:a;
	GetVehiclePos(vehicleid, x, y, z);
	GetVehicleZAngle(vehicleid, a);

	mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_health` = %f, `vehicle_visual` = '%s', `vehicle_mileage` = %f, `vehicle_fuel` = %f, `vehicle_lastpos` = '%f:%f:%f:%f:%d:%d'%s WHERE `vehicle_uid` = %d", saveHealth, visual_damage, Vehicle[vehicleid][vehicle_mileage], Vehicle[vehicleid][vehicle_fuel_current], x, y, z, a, Vehicle[vehicleid][vehicle_interior], GetVehicleVirtualWorld(vehicleid), arestart_pos, Vehicle[vehicleid][vehicle_uid]));
}

stock GetVehicleType(vehicleid, model = -1)
{
	if( model == -1 ) model = GetVehicleModel(vehicleid);
	switch( model )
	{
		case 460, 476, 511, 512, 513, 519, 520, 553, 577, 592, 593: return VEHICLE_TYPE_AIRPLANE;
		case 548, 425, 417, 487, 488, 497, 563, 447, 469: return VEHICLE_TYPE_HELICOPTER;
		case 509, 481, 510: return VEHICLE_TYPE_BIKE;
		case 462, 448, 581, 522, 461, 521, 523, 463, 586, 468, 471: return VEHICLE_TYPE_MOTORBIKE;
		case 472, 473, 493, 595, 484, 430, 453, 452, 446, 454: return VEHICLE_TYPE_BOAT;
		case 435, 450, 569, 570, 584, 590, 591, 606, 608, 607, 610, 611: return VEHICLE_TYPE_TRAILER;
		case 423, 428, 588, 499, 482, 498, 609, 455, 414, 413, 440, 459, 456: return VEHICLE_TYPE_CARGO;
		case 515, 514, 403: return VEHICLE_TYPE_TRUCK;
		default: return VEHICLE_TYPE_CAR;
	}
	
	return VEHICLE_TYPE_CAR;
}


stock GetPointInAngleOfVehicle(vehicleid, &Float:x, &Float:y, &Float:z, Float:distance, Float:angle)
{
	new Float:current_angle;
	GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, current_angle);
	
	new Float:a = current_angle + angle;
	
	x += (distance * floatsin(-a, degrees));
	y += (distance * floatcos(-a, degrees));
}


stock GetVehicleByUid(v_uid)
{
	foreach(new vid : Vehicles)
	{
		if( Vehicle[vid][vehicle_uid] == v_uid ) return vid;
	}
	
	return INVALID_VEHICLE_ID;
}

stock CanPlayerUseVehicle(playerid, vehicleid)
{
	if(!GetPlayerPermissions(playerid, 5)) return 1;
	
	if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_PLAYER )
	{
		if( Vehicle[vehicleid][vehicle_owner] == pInfo[playerid][player_id] ) return 1;
	}
	else if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP )
	{
		new gid = GetGroupByUid(Vehicle[vehicleid][vehicle_owner]), slot = GetPlayerGroupSlot(playerid, gid);
		if( slot > -1 )
		{
			if( WorkerHasFlag(playerid, slot, WORKER_FLAG_VEHICLES) ) return 1;
		}
	}
	
	return 0;
}

stock CanPlayerEditVehicle(playerid, vehicleid)
{
	if(!GetPlayerPermissions(playerid, 5)) return 1;
	
	if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_PLAYER )
	{
		if( Vehicle[vehicleid][vehicle_owner] == pInfo[playerid][player_id] ) return 1;
	}
	else if( Vehicle[vehicleid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP )
	{
		new gid = GetGroupByUid(Vehicle[vehicleid][vehicle_owner]), slot = GetPlayerGroupSlot(playerid, gid);
		if( slot > -1 )
		{
			if( WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return 1;
		}
	}
	
	return 0;
}

stock GetNearestPlayerVehicle(playerid)
{
	new 
		Float:vehicle_pos[3],
		Float:pretender_distance = 10.0,
		pretender = -1;
		
	foreach(new v_id : Vehicles)
	{
		if( !CanPlayerUseVehicle(playerid, v_id) ) continue;
		
		GetVehiclePos(v_id, vehicle_pos[0], vehicle_pos[1], vehicle_pos[2]);
		new 
			Float:distance = GetPlayerDistanceFromPoint(playerid, vehicle_pos[0], vehicle_pos[1], vehicle_pos[2]);
		if( distance <= pretender_distance )
		{
			if( !VehicleHasAccessory(v_id, VEHICLE_ACCESSORY_ALARM) && distance > 4.0 ) continue;
			// TODO: suppport for alarm
			pretender_distance = distance;
			pretender = v_id;
		}
	}
	
	return pretender;
}

stock GetVehicleDriver(vehicleid, checks = true)
{	
	if( IsPlayerConnected(Vehicle[vehicleid][vehicle_driver]) )
	{
		if( checks )
		{
			if( GetPlayerVehicleID(Vehicle[vehicleid][vehicle_driver]) == vehicleid && GetPlayerVehicleSeat(Vehicle[vehicleid][vehicle_driver]) == 0 ) return Vehicle[vehicleid][vehicle_driver];
		}
		else return Vehicle[vehicleid][vehicle_driver];
	}
	return INVALID_PLAYER_ID;
}

task VehiclesTask[500]()
{
	for(new vid;vid<MAX_VEHICLES;vid++)
	{
		if(!Iter_Contains(Vehicles, vid)) continue;
		if( Vehicle[vid][vehicle_engine] )
		{
			if( gettime() - Vehicle[vid][vehicle_last_fuel_td] > 30 )
			{
				new driver = GetVehicleDriver(vid);
				if( driver != INVALID_PLAYER_ID )
				{
					PlayerTextDrawSetString(driver, pInfo[driver][vehicleFuelInfo], sprintf("~y~PALIWO: ~g~%d/%d L", floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil), VehicleFuelMax[GetVehicleModel(vid)-400]));
					PlayerTextDrawShow(driver, pInfo[driver][vehicleFuelInfo]);
					
					defer HideFuelTextdraw[4000](driver);
				}
				
				Vehicle[vid][vehicle_last_fuel_td] = gettime();
			}
		
	
			// Sprawdzamy hp pojazdu
			new Float:vehicleHp;
			GetVehicleHealth(vid, vehicleHp);
			if( vehicleHp > Vehicle[vid][vehicle_health] )
			{
				SetVehicleHealth(vid, Vehicle[vid][vehicle_health]);
                new str[80], playerid = GetVehicleDriver(vid);
                format(str, sizeof(str), "Forced vehicle repair (vid %d, %.2f->%.2f)", Vehicle[vid][vehicle_health], vehicleHp);
                AddPlayerPenalty(playerid, PENALTY_TYPE_KICK, INVALID_PLAYER_ID, 0, str);
			}
			else 
			{
				if( vehicleHp < Vehicle[vid][vehicle_health] )
				{
					new Float:lost = Vehicle[vid][vehicle_health] - vehicleHp;
					new playerid = GetVehicleDriver(vid);
					
					VehicleLog(sprintf("Loss: %.2f->%.2f (delta %.2f). Driver: %s", Vehicle[vid][vehicle_health], vehicleHp, lost, (playerid != INVALID_PLAYER_ID ? PlayerLogLink(pInfo[playerid][player_id]) : "none")), Vehicle[vid][vehicle_uid], "hp_loss");

					Vehicle[vid][vehicle_health] = vehicleHp;
					OnVehicleHealthLoss(vid, lost);
				}
			}
			
			// Sprawdzamy zniszczenia wizualne
			new vehicleDmg[4];
			GetVehicleDamageStatus(vid, vehicleDmg[0], vehicleDmg[1], vehicleDmg[2], vehicleDmg[3]);
			
			if( vehicleDmg[0] < Vehicle[vid][vehicle_damage][0] || vehicleDmg[1] < Vehicle[vid][vehicle_damage][1] || vehicleDmg[2] < Vehicle[vid][vehicle_damage][2] || vehicleDmg[3] < Vehicle[vid][vehicle_damage][3] )
			{
				UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);
				// Trza dodac kick gracza za naprawiwanie pojazdu cheatem
			}
			else
			{
				Vehicle[vid][vehicle_damage][0] = vehicleDmg[0];
				Vehicle[vid][vehicle_damage][1] = vehicleDmg[1];
				Vehicle[vid][vehicle_damage][2] = vehicleDmg[2];
				Vehicle[vid][vehicle_damage][3] = vehicleDmg[3];
			}
			
			// Czas na przebieg
			new Float:distance = GetVehicleDistanceFromPoint(vid, Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2]);
			if( distance >= 0 )
			{
				if( distance > 0 ) Vehicle[vid][vehicle_mileage] += distance/1000;
				
				if( Vehicle[vid][vehicle_engine] && GetVehicleType(vid) != VEHICLE_TYPE_BIKE )
				{
					// Czas na paliwko
					new Float:fuel_drain;
					
					// Pojemnosc silnika
					fuel_drain = floatdiv(VehicleEngineCapacity[Vehicle[vid][vehicle_model]-400], 10000);
					
					// Typ paliwa
					switch( Vehicle[vid][vehicle_fuel_type] )
					{
						case VEHICLE_FUEL_TYPE_BENZYNA:
						{
							fuel_drain *= BENZYNA_MULTIPLIER;
						}
						
						case VEHICLE_FUEL_TYPE_DIESEL:
						{
							fuel_drain *= DIESEL_MULTIPLIER;
						}
						
						case VEHICLE_FUEL_TYPE_GAZ:
						{
							fuel_drain *= GAZ_MULTIPLIER;
						}
						
						case VEHICLE_FUEL_TYPE_LOTNICZE:
						{
							fuel_drain *= LOTNICZE_MULTIPLIER;
						}
					}
					
					// Przebieg pojazdu
					if( Vehicle[vid][vehicle_mileage] > FUEL_DRAIN_INCREASE_MILEAGE_CAP )
					{
						fuel_drain *= floatdiv(Vehicle[vid][vehicle_mileage], FUEL_DRAIN_INCREASE_MILEAGE_CAP);
					}
					
					if( distance > 0 ) fuel_drain *= distance;
					else fuel_drain *= 1.4;
					
					fuel_drain *= 1.2;
					

					if( floatcmp(Vehicle[vid][vehicle_fuel_current], fuel_drain) <= 0 )
					{
						Vehicle[vid][vehicle_fuel_current] = 0.0;
						Vehicle[vid][vehicle_engine] = false;
						UpdateVehicleVisuals(vid);
						
						new playerid = GetVehicleDriver(vid);
						
						if( playerid != INVALID_PLAYER_ID )
						{
							TextDrawShowForPlayer(playerid, vehicleInfo);
							SendPlayerInformation(playerid, "W baku Twojego pojazdu skonczylo sie paliwo.", 5000);
						}
					}
					else Vehicle[vid][vehicle_fuel_current] -= fuel_drain;
				}
			}

			// Ustawiamy ostatnia pozycje na aktualna :)
			GetVehiclePos(vid, Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2]);
			
			if( distance > 0 )
			{
				// Poprawiamy pozycje map icony
				Streamer_SetFloatData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_X, Vehicle[vid][vehicle_last_pos][0]);
				Streamer_SetFloatData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_Y, Vehicle[vid][vehicle_last_pos][1]);
				Streamer_SetFloatData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_Z, Vehicle[vid][vehicle_last_pos][2]);
			}
			
			new speed = GetVehicleSpeed(vid);
			if( GetVehicleType(vid) == VEHICLE_TYPE_BIKE && speed > 80 )
			{
				new Float:vel[3];
				GetVehicleVelocity(vid, vel[0], vel[1], vel[2]);
				SetVehicleVelocity(vid, vel[0]/2, vel[1]/2, vel[2]/2);
			}
		}
		else
		{
			// Akcje pojazdow
			if( Vehicle[vid][vehicle_state] > 0 )
			{
				switch( Vehicle[vid][vehicle_state] )
				{
					case VEHICLE_STATE_FUELING:
					{
						if( gettime() >= Vehicle[vid][vehicle_state_end] )
						{
							Vehicle[vid][vehicle_state] = 0;
							
							UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_state_label], COLOR_PINK, "Tankowanie pojazdu\nZakoñczono");
							defer VehicleStateErase[3000](vid);
						}
						else
						{
							new time_elapsed = Vehicle[vid][vehicle_state_time] - (Vehicle[vid][vehicle_state_end] - gettime());
							
							new Float:as = floatdiv(time_elapsed, Vehicle[vid][vehicle_state_time]);
							new str[40];
							format(str, sizeof(str), "Tankowanie pojazdu\n%d%", floatround(as*100));
							
							UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_state_label], COLOR_PINK, str);
						}
					}

					case VEHICLE_STATE_PAINT:
					{
						if( gettime() >= Vehicle[vid][vehicle_state_end] )
						{
							Vehicle[vid][vehicle_state] = 0;
							
							UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_state_label], COLOR_PINK, "Trwa malowanie pojazdu\nZakoñczono");
							defer VehicleStateErase[3000](vid);

							new sellerid = GetPlayerByUid(Vehicle[vid][vehicle_carpaint_seller]);
							new playerid = GetPlayerByUid(Vehicle[vid][vehicle_carpaint_buyer]);

							if( sellerid != INVALID_PLAYER_ID ) SendGuiInformation(sellerid, "Informacja", sprintf("Malowanie pojazdu o uid %d zosta³o zakoñczone.", Vehicle[vid][vehicle_uid]));
							if( playerid != INVALID_PLAYER_ID ) SendGuiInformation(playerid, "Informacja", sprintf("Malowanie Twojego pojazdu o uid %d zosta³o zakoñczone.", Vehicle[vid][vehicle_uid]));

							ChangeVehicleColor(vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);

							mysql_pquery(g_sql, sprintf("UPDATE crp_vehicles SET vehicle_color1 = %d, vehicle_color2 = %d WHERE vehicle_uid = %d", Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1], Vehicle[vid][vehicle_uid]));
						}
						else
						{
							new time_elapsed = Vehicle[vid][vehicle_state_time] - (Vehicle[vid][vehicle_state_end] - gettime());
							
							new Float:as = floatdiv(time_elapsed, Vehicle[vid][vehicle_state_time]);
							new str[40];
							format(str, sizeof(str), "Trwa malowanie pojazdu\n%d%", floatround(as*100));
							
							UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_state_label], COLOR_PINK, str);
						}
					}
				}
			}
		}
	}
}