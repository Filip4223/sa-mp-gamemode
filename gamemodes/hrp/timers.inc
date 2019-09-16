timer UnfreezePlayer[1000](playerid, freeze_id)
{
	if( pInfo[playerid][player_freeze_id] == freeze_id )
	{
		TogglePlayerControllable(playerid, 1);
		pInfo[playerid][player_freeze_id] = 0;
	}
}

timer HideInformationTextdraw[100](playerid, information_id)
{
	if( pInfo[playerid][player_info_td_id] == information_id )
	{
		PlayerTextDrawHide(playerid, pInfo[playerid][informationTd]);
		pInfo[playerid][player_info_td_id] = 0;
	}
}

timer StopPlayerAnim[3000](playerid)
{
	pInfo[playerid][player_overturned] = false;
	TogglePlayerControllable(playerid, 1);
	ClearAnimations(playerid);
}

timer OverturnPlayer[10](playerid)
{
	pInfo[playerid][player_overturned] = true;
	ApplyAnimation(playerid, "PED", "KO_shot_front", 4.1, 0, 0, 0, 3000, 0, 1);
}

timer PlayerDamaged[3000](playerid)
{
	TogglePlayerControllable(playerid, 1);
}

timer PlayerExamination[6000](playerid, targetid)
{
	new str[256], playername[MAX_PLAYER_NAME], targetname[MAX_PLAYER_NAME];

	GetPlayerName(playerid, playername, sizeof(playername));
	GetPlayerName(targetid, targetname, sizeof(targetname));

	UnderscoreToSpace(targetname);
	UnderscoreToSpace(playername);

	new date[64];
	GetRelativeDate(gettime(), date);

	strcat(str, sprintf("Badanie %s\n", targetname));
	strcat(str, "-\n");
	strcat(str, sprintf("Lekarz: %s\n", playername));
	strcat(str, "Diagnoza: ");

	new diagnosis[64];

	switch(pInfo[targetid][player_damage])
	{
		case DAMAGE_LEVEL_NONE: format(diagnosis, sizeof(diagnosis), "Brak obra¿eñ");
		case DAMAGE_LEVEL_LOW: format(diagnosis, sizeof(diagnosis), "Niskie obra¿enia");
		case DAMAGE_LEVEL_BAD: format(diagnosis, sizeof(diagnosis), "Powa¿ne obra¿enia");
		case DAMAGE_LEVEL_CRITICAL: format(diagnosis, sizeof(diagnosis), "Bardzo powa¿ne obra¿enia (stan krytyczny)");
	}

	strcat(str, diagnosis);

	SendGuiInformation(playerid, "Wyniki badañ", str);

	new Cache:result;
	result = mysql_query(g_sql, sprintf("INSERT INTO `crp_chits` VALUES (null, '%s')", str));
			
	new chit_uid = cache_insert_id();

	cache_delete(result);
			
	new i_id = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_CHIT, chit_uid, 0, "Wyniki badañ", 19469);

	pInfo[targetid][player_examined] = true;

	ClearAnimations(playerid);
}

timer LoginCameraMove[1000](playerid, crc)
{
	if( !pInfo[playerid][player_logged] )
	{
		if( pInfo[playerid][player_login_camera_crc] != crc ) return;
		pInfo[playerid][player_login_camera_crc]++;

		new sequence = pInfo[playerid][player_login_camera_seq];
		pInfo[playerid][player_login_camera_seq]++;

		InterpolateCameraPos(playerid, loginCamera[sequence][6], loginCamera[sequence][7], loginCamera[sequence][8], loginCamera[sequence][9], loginCamera[sequence][10], loginCamera[sequence][11], 7000, CAMERA_MOVE);
		InterpolateCameraLookAt(playerid, loginCamera[sequence][0], loginCamera[sequence][1], loginCamera[sequence][2], loginCamera[sequence][3], loginCamera[sequence][4], loginCamera[sequence][5], 7000, CAMERA_MOVE);
	
		if(pInfo[playerid][player_login_camera_seq] < sizeof(loginCamera) ) defer LoginCameraMove[7500](playerid, pInfo[playerid][player_login_camera_crc]);
	}
}

timer DelayEditObject[500](playerid, objectid)
{
	EditDynamicObject(playerid, objectid);
}

timer LockVehicleWithDelay[500](vid)
{
	Vehicle[vid][vehicle_locked] = true;
	Vehicle[vid][vehicle_state] = 0;
}

timer PlayerSearch[5000](playerid, targetid, type)
{
	new count = 0, string[2048];
	format(string, sizeof(string), "Gotówka: $%d\n-----\n", pInfo[targetid][player_money]);
	
	switch(type)
	{
		case 0:
		{
			foreach (new item : Items)
			{
				if( Item[item][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[item][item_owner] != pInfo[targetid][player_id] ) continue;
				
				if(Item[item][item_type] == ITEM_TYPE_DRUGS)
				{
					new Float:percent = 25.0 + ( (Item[item][item_amount] - 1) * 5);
					
					new randvalue = random(100);

					printf("Drug - %f/%d", percent, randvalue);

					if(!IS_IN_RANGE(randvalue, 0, percent)) continue;

					print("Dodaje drug");
				}
				if(Item[item][item_type] == ITEM_TYPE_WEAPON && IS_IN_RANGE(Item[item][item_value1], 21, 25))
				{
					new randvalue = random(100);

					printf("Short weapon - %d", randvalue);

					new is_in_range = IS_IN_RANGE(randvalue, 0, 30);
					if(!is_in_range) continue;

					printf("Dodaje bron %d", is_in_range);
				}

				new prefix[10]; 
				if( Item[item][item_used] ) prefix = "{CFFD66}";
				
				format(string, sizeof(string), "%s%s%s{000000}\t\t%d %d  (%d:%d)\n", string, prefix, Item[item][item_name], Item[item][item_uid], Item[item][item_type], Item[item][item_value1], Item[item][item_value2]);
				count++;
			}
		}
		case 1:
		{
			foreach (new item : Items)
			{
				if( Item[item][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[item][item_owner] != pInfo[targetid][player_id] ) continue;
				
				new prefix[10]; 
				if( Item[item][item_used] ) prefix = "{CFFD66}";
				
				format(string, sizeof(string), "%s %s%s{000000}\t\t%d %d  (%d:%d)\n", string, prefix, Item[item][item_name], Item[item][item_uid], Item[item][item_type], Item[item][item_value1], Item[item][item_value2]);
				count++;
			}
		}
	}

	ClearAnimations(playerid);

	if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz o podanym id nie posiada ¿adnych przedmiotów.");
	else ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("Przedmioty gracza %s", pInfo[targetid][player_name]), string, "OK", "");
}

timer SetOfferTextDrawColor[1](playerid, i)
{
	for(new y=0; y<5; y++)
	{
		PlayerTextDrawColor(playerid, pInfo[playerid][OfferTD][y], i);
		PlayerTextDrawShow(playerid,  pInfo[playerid][OfferTD][y]);
	}

	if(i < -1) 
	{
		if(i + 10 < -1) i += 10;
		else i = -1;

		if(i != -1) defer SetOfferTextDrawColor[15](playerid, i);
	}
}

timer SetTextDrawColor[1](playerid, i, count)
{
	for(new y=0; y<3; y++)
	{
		PlayerTextDrawColor(playerid, SmallAlert[count][playerid][y], i);
		PlayerTextDrawShow(playerid, SmallAlert[count][playerid][y]);
	}

	if(i < -1) 
	{
		if(i + 10 < -1) i += 10;
		else
		{
			i = -1;
			defer TextDrawHide[5000](playerid, count);
		}

		if(i != -1)
		{
			defer SetTextDrawColor[15](playerid, i, count);
		}
	}
}

timer TextDrawHide[5000](playerid, count)
{
	for(new i = 0; i<3; i++)
	{
		PlayerTextDrawHide(playerid, SmallAlert[count][playerid][i]);
	}

	pInfo[playerid][player_txd_count]--;
}

timer SetVehicleBound[500](vid, Float:x, Float:y, Float:z, Float:a)
{
	SetVehiclePos(vid, x, y, z);
	SetVehicleZAngle(vid, a);
	
	SetVehicleVelocity(vid, 0.0, 0.0, 0.0);
}

timer StopPlayerHurted[1000](playerid)
{
	if( pInfo[playerid][player_hurted] ) 
	{
		pInfo[playerid][player_hurted] = false;
		ApplyAnimation(playerid, "CARRY", "crry_prtial", 4.0, 0, 0, 0, 0, 0, 1);
		
		RemovePlayerStatus(playerid, PLAYER_STATUS_HURT);
	}
}

timer RemovePlayerLeftLabel[100](Text3D:leftLabel)
{
	if( IsValidDynamic3DTextLabel(leftLabel) ) DestroyDynamic3DTextLabel(leftLabel);
}

timer ApplyAnim[1000](playerid, type)
{
	switch( type )
	{
		case ANIM_TYPE_BW: ApplyAnimation(playerid, "KNIFE", "KILL_Knife_Ped_Die", 4.0, 0, 0, 0, 1, 0, 1);
		case ANIM_TYPE_BW_INCAR: ApplyAnimation(playerid, "ped", "CAR_dead_LHS", 4.0, 0, 0, 0, 1, 0, 1);

	}
}

timer HideGroupDutyPane[100](playerid)
{
	PlayerTextDrawHide(playerid, pInfo[playerid][DashboardPane][0]);
	PlayerTextDrawHide(playerid, pInfo[playerid][DashboardPane][1]);
}

timer LoadAttachedObjects[1000](playerid)
{
	foreach(new itemid : Items)
	{
		if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[itemid][item_owner] == pInfo[playerid][player_id] && Item[itemid][item_type] == ITEM_TYPE_ACCESSORY && Item[itemid][item_used] )
		{
			new freeid = GetPlayerFreeAttachSlot(playerid);
			if( freeid == -1 ) break;
			
			// zakladamy
			mysql_pquery(g_sql, sprintf("SELECT * FROM crp_access WHERE access_uid = %d", Item[itemid][item_value1]), "OnAttachedObjectLoaded", "iii", playerid, freeid, itemid);
		}
	}
}

function OnAttachedObjectLoaded(playerid, freeid, itemid)
{
	SetPlayerAttachedObject(playerid, freeid, cache_get_int(0, "access_model"), cache_get_int(0, "access_bone"), cache_get_float(0, "access_posx"), cache_get_float(0, "access_posy"), cache_get_float(0, "access_posz"), 
	cache_get_float(0, "access_rotx"), cache_get_float(0, "access_roty"), cache_get_float(0, "access_rotz"), cache_get_float(0, "access_scalex"), cache_get_float(0, "access_scaley"), cache_get_float(0, "access_scalez"));
	
	Item[itemid][item_value2] = freeid;
}

timer VehicleEngineStart[1000](playerid, vehicleid)
{
	TextDrawHideForPlayer(playerid, vehicleEngineStarting);
	TextDrawHideForPlayer(playerid, vehicleInfo);
	
	Vehicle[vehicleid][vehicle_engine_starting] = false;
	Vehicle[vehicleid][vehicle_engine] = true;
	
	SendPlayerInformation(playerid, "Silnik pojazdu zostal ~g~odpalony~w~!", 3500);
	
	UpdateVehicleVisuals(vehicleid);
}

timer BuggedHotel[500](playerid)
{
	GameTextForPlayer(playerid, "~p~aby wyjsc z pokoju~n~~y~lalt + space", 8000, 3);
}

timer VehicleStateErase[1000](vid)
{
	Vehicle[vid][vehicle_state] = 0;
	Vehicle[vid][vehicle_state_end] = 0;
	Vehicle[vid][vehicle_state_time] = 0;
	
	UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_state_label], COLOR_WHITE, "");
}

timer StopPlayerAnimation[200](playerid)
{
	if(!pInfo[playerid][player_overturned]) ApplyAnimation(playerid, "CARRY", "crry_prtial", 4.0, 0, 0, 0, 0, 0, 1);
}

timer HideFuelTextdraw[200](playerid)
{
	PlayerTextDrawHide(playerid, pInfo[playerid][vehicleFuelInfo]);
}

timer BusSelectPrepare[5](playerid)
{
	SetPlayerFacingAngle(playerid, 0);
	
	SetPlayerCameraPos(playerid, pInfo[playerid][player_bus_camera][0]-0.5, pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]+80.0);	
	SetPlayerCameraLookAt(playerid, pInfo[playerid][player_bus_camera][0], pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]);
	
	PlayerTextDrawSetString(playerid, pInfo[playerid][BusInfo], "Za pomoca strzalek zmieniaj pozycje kamery, aby odnalezc miejsce podrozy.~n~~n~Wcisnij ~y~~k~~VEHICLE_ENTER_EXIT~ ~w~aby zobaczyc najblizszy przystanek lub ~y~~k~~PED_JUMPING~ ~w~aby anulowac.");
	PlayerTextDrawShow(playerid, pInfo[playerid][BusInfo]);
}

timer DrugCooking[100](cooking_uid, objectid, start, end)
{

	new Float:percent = floatdiv((gettime()-start),(end-start))*100;

	if(percent >= 100)
	{
		UpdateDynamic3DTextLabelText(Object[objectid][object_label], COLOR_WHITE, "{00FF00}Stol alchemiczny\n"HEX_COLOR_WHITE"Gotowanie zakoñczone\n (( u¿yj /craft aby zgarn¹æ towar ))");
		Object[objectid][object_is_drug_cooked] = false;
		Object[objectid][object_drug_cooked_id] = cooking_uid;
	}
	else UpdateDynamic3DTextLabelText(Object[objectid][object_label], COLOR_WHITE, sprintf("{00FF00}Stol alchemiczny\n"HEX_COLOR_WHITE"Gotowanie\n%d%%", floatround(percent)));

	if(percent < 100) {
		defer DrugCooking[1000](cooking_uid, objectid, start, end);
	}
}

timer PreloadAllActorAnimLibs[200](actorid)
{
	PreloadActorAnimLib(actorid,"BOMBER");
   	PreloadActorAnimLib(actorid,"RAPPING");
    PreloadActorAnimLib(actorid,"SHOP");
   	PreloadActorAnimLib(actorid,"BEACH");
   	PreloadActorAnimLib(actorid,"SMOKING");
	PreloadActorAnimLib(actorid,"RYDER");
	PreloadActorAnimLib(actorid,"PLAYIDLES");
	PreloadActorAnimLib(actorid,"POOL");
	PreloadActorAnimLib(actorid,"DANCING");
	PreloadActorAnimLib(actorid,"LOWRIDER");
	PreloadActorAnimLib(actorid,"INT_SHOP");
	PreloadActorAnimLib(actorid,"wuzi");
	PreloadActorAnimLib(actorid,"CRIB");
	PreloadActorAnimLib(actorid,"POLICE");
	PreloadActorAnimLib(actorid,"GRAVEYARD");
	PreloadActorAnimLib(actorid,"FIGHT_D");
    PreloadActorAnimLib(actorid,"ON_LOOKERS");
	PreloadActorAnimLib(actorid,"RIOT");
	PreloadActorAnimLib(actorid,"GANGS");
    PreloadActorAnimLib(actorid,"DEALER");
	PreloadActorAnimLib(actorid,"VENDING");
	PreloadActorAnimLib(actorid,"HEIST9");
	PreloadActorAnimLib(actorid,"CRACK");
	PreloadActorAnimLib(actorid,"SPRAYCAN");
	PreloadActorAnimLib(actorid,"JST_BUISNESS");
	PreloadActorAnimLib(actorid,"PAULNMAC");
	PreloadActorAnimLib(actorid,"GRENADE");
	PreloadActorAnimLib(actorid,"WAYFARER");
	PreloadActorAnimLib(actorid,"INT_OFFICE");
	PreloadActorAnimLib(actorid,"ROB_BANK");
	PreloadActorAnimLib(actorid,"STRIP");
	PreloadActorAnimLib(actorid,"GHANDS");
	PreloadActorAnimLib(actorid,"PARK");
	PreloadActorAnimLib(actorid,"CAR");
	PreloadActorAnimLib(actorid,"CARRY");
	PreloadActorAnimLib(actorid,"KISSING");
	PreloadActorAnimLib(actorid,"MISC");
	PreloadActorAnimLib(actorid,"OTB");
	PreloadActorAnimLib(actorid,"COP_AMBIENT");
	PreloadActorAnimLib(actorid,"MEDIC");
	PreloadActorAnimLib(actorid,"INT_HOUSE");
	PreloadActorAnimLib(actorid,"FOOD");
	PreloadActorAnimLib(actorid,"PED");
	PreloadActorAnimLib(actorid,"SWEET");
	PreloadActorAnimLib(actorid,"KNIFE");
	PreloadActorAnimLib(actorid,"CASINO");
	PreloadActorAnimLib(actorid,"AIRPORT");
	PreloadActorAnimLib(actorid,"Attractors");
	PreloadActorAnimLib(actorid,"BAR");
	PreloadActorAnimLib(actorid,"BASEBALL");
	PreloadActorAnimLib(actorid,"BD_FIRE");
	PreloadActorAnimLib(actorid,"benchpress");
	PreloadActorAnimLib(actorid,"BF_injection");
	PreloadActorAnimLib(actorid,"BLOWJOBZ");
	PreloadActorAnimLib(actorid,"BOX");
	PreloadActorAnimLib(actorid,"BSKTBALL");
	PreloadActorAnimLib(actorid,"BUDDY");
	PreloadActorAnimLib(actorid,"CAMERA");
	PreloadActorAnimLib(actorid,"DAM_JUMP");
	PreloadActorAnimLib(actorid,"FAT");
}

timer PreloadAllAnimLibs[200](playerid)
{
	PreloadAnimLib(playerid,"BOMBER");
   	PreloadAnimLib(playerid,"RAPPING");
    PreloadAnimLib(playerid,"SHOP");
   	PreloadAnimLib(playerid,"BEACH");
   	PreloadAnimLib(playerid,"SMOKING");
	PreloadAnimLib(playerid,"RYDER");
	PreloadAnimLib(playerid,"PLAYIDLES");
	PreloadAnimLib(playerid,"POOL");
	PreloadAnimLib(playerid,"DANCING");
	PreloadAnimLib(playerid,"LOWRIDER");
	PreloadAnimLib(playerid,"INT_SHOP");
	PreloadAnimLib(playerid,"wuzi");
	PreloadAnimLib(playerid,"CRIB");
	PreloadAnimLib(playerid,"POLICE");
	PreloadAnimLib(playerid,"GRAVEYARD");
	PreloadAnimLib(playerid,"FIGHT_D");
    PreloadAnimLib(playerid,"ON_LOOKERS");
	PreloadAnimLib(playerid,"RIOT");
	PreloadAnimLib(playerid,"GANGS");
    PreloadAnimLib(playerid,"DEALER");
	PreloadAnimLib(playerid,"VENDING");
	PreloadAnimLib(playerid,"HEIST9");
	PreloadAnimLib(playerid,"CRACK");
	PreloadAnimLib(playerid,"SPRAYCAN");
	PreloadAnimLib(playerid,"JST_BUISNESS");
	PreloadAnimLib(playerid,"PAULNMAC");
	PreloadAnimLib(playerid,"GRENADE");
	PreloadAnimLib(playerid,"WAYFARER");
	PreloadAnimLib(playerid,"INT_OFFICE");
	PreloadAnimLib(playerid,"ROB_BANK");
	PreloadAnimLib(playerid,"STRIP");
	PreloadAnimLib(playerid,"GHANDS");
	PreloadAnimLib(playerid,"PARK");
	PreloadAnimLib(playerid,"CAR");
	PreloadAnimLib(playerid,"CARRY");
	PreloadAnimLib(playerid,"KISSING");
	PreloadAnimLib(playerid,"MISC");
	PreloadAnimLib(playerid,"OTB");
	PreloadAnimLib(playerid,"COP_AMBIENT");
	PreloadAnimLib(playerid,"MEDIC");
	PreloadAnimLib(playerid,"INT_HOUSE");
	PreloadAnimLib(playerid,"FOOD");
	PreloadAnimLib(playerid,"PED");
	PreloadAnimLib(playerid,"SWEET");
	PreloadAnimLib(playerid,"KNIFE");
	PreloadAnimLib(playerid,"CASINO");
	PreloadAnimLib(playerid,"AIRPORT");
	PreloadAnimLib(playerid,"Attractors");
	PreloadAnimLib(playerid,"BAR");
	PreloadAnimLib(playerid,"BASEBALL");
	PreloadAnimLib(playerid,"BD_FIRE");
	PreloadAnimLib(playerid,"benchpress");
	PreloadAnimLib(playerid,"BF_injection");
	PreloadAnimLib(playerid,"BLOWJOBZ");
	PreloadAnimLib(playerid,"BOX");
	PreloadAnimLib(playerid,"BSKTBALL");
	PreloadAnimLib(playerid,"BUDDY");
	PreloadAnimLib(playerid,"CAMERA");
	PreloadAnimLib(playerid,"DAM_JUMP");
	PreloadAnimLib(playerid,"FAT");
}