/********************************************************/
/*           ROLE PLAY GAMEMODE
		Author:  promsters
		Version: 1.5
		Website: www.honest-rp.pl
		Last modified: 13.01.2019
		Created: 02.07.2013
		Credits: Filip
*/
/*******************************************************/
#include <a_samp>

#define FIXES_Single
#include <fixes>
#include <strlib>
#include <dini>
#include <a_mysql>
#include <md5>
#include <streamer>
#include <timestamptodate>
#include <sscanf2>
#include <kickfix>
#include <sprintf>
#include <crashdetect>
#include <log-plugin>
#include <Pawn.RakNet>
#include <dns>

#include <YSI\y_iterate>
#include <YSI\y_hooks>
#include <YSI\y_timers>
#include <YSI\y_flooding>
#include <zcmd>

#include "hrp/color_management.inc"
#include "hrp/version.inc"
#include "hrp/config.inc"
#include "hrp/code_timer.inc"
#include "hrp/misc.inc"
#include "hrp/dynamicgui.inc"
#include "hrp/readycheck.inc"

#include "vendor/fly.inc"

#include "hrp/logs.inc"
#include "hrp/actions.inc"
#include "hrp/functions.inc"
#include "hrp/areas.inc"
#include "hrp/groups.inc"
#include "hrp/doors.inc"
#include "hrp/actors.inc"
#include "hrp/vehicles.inc"
#include "hrp/offers.inc"
#include "hrp/items.inc"
#include "hrp/buses.inc"
#include "hrp/labels.inc"
#include "hrp/player.inc"
#include "hrp/products.inc"
#include "hrp/gym.inc"
#include "hrp/interactions.inc"
#include "hrp/penalties.inc"
#include "hrp/textdraws.inc"
#include "hrp/objects.inc"
#include "hrp/drugs.inc"
#include "hrp/acmd.inc"
#include "hrp/anticheat.inc"
#include "hrp/lspddb.inc"
#include "hrp/cmd.inc"
#include "hrp/timers.inc"
#include "hrp/car_color_picker.inc"
#include "hrp/firedep.inc"
#include "hrp/raknet.inc"

main() {}

public OnGameModeInit()
{
	Code_ExTimer_Begin(GameModeInit);

	// Wylaczamy to gowno sampowskie
	ShowPlayerMarkers(0);
    ShowNameTags(0);
    DisableInteriorEnterExits();
    EnableStuntBonusForAll(0);
    ManualVehicleEngineAndLights();

    Iter_Init(Skins);

	// Tworzymy 3d labele graczy
	for(new i;i<MAX_PLAYERS;i++)
	{
		pInfo[i][player_label] = Create3DTextLabel("", 0xFFFFFF60, 0.0, 0.0, 0.0, 12.0, 0, 1);
		pInfo[i][player_description_label] = Create3DTextLabel("", LABEL_DESCRIPTION, 0.0, 0.0, 0.0, 4.0, 0, 1);
	}
	
	// Wczytujemy konfiguracje mysql
	LoadConfiguration();
	CreateLoggers();

	// czymy z baz danych
	if( !ConnectMysql() ) return SendRconCommand("exit");

	LoadCustomModels();
	CreateTextdraws();
	
	if(Setting[setting_run_mode] == RunMode::DEV)
	{
		print("================================");
		print("|           DEV  MODE          |");
		print("|            ENABLED           |");
		print("================================");
	}
	
	LoadGlobalSpawns();
	LoadGroups();
	LoadAreas();
	LoadDoors();
	LoadLabels();
	LoadBuses();
	LoadObjects();
	LoadVehicles();
	LoadActors();
	mysql_query(g_sql, "UPDATE crp_vehicles SET vehicle_spawn_restart = 0", false);

	LoadItems();
	LoadSkins();
	
	mysql_query(g_sql, "DELETE FROM crp_logged_players", false);

	hook_ReadyCheck();

	SetMaxConnections(3, e_FLOOD_ACTION_KICK);

	Setting[setting_gm_start_time] = GetTickCount();
	
	printf("[honest] HRP Gamemode v"HRP_HUMAN_VERSION" zostal wczytany pomyslnie [czas wykonania: %d ms]", Code_ExTimer_End(GameModeInit));
	return 1;
}

public OnGameModeExit()
{
	foreach(new v : Vehicles)
	{
		SaveVehicle(v, true);
	}

	mysql_close(g_sql);
	return 1;
}

public OnPlayerRequestDownload(playerid, type, crc)
{
	if( strlen(Setting[setting_remote_download_url]) < 1 ) return 1;

	new filename[64];
	if(type == DOWNLOAD_REQUEST_MODEL_FILE)
	{
		FindModelFileNameFromCRC(crc, filename, sizeof(filename));
	}
	else if(type == DOWNLOAD_REQUEST_TEXTURE_FILE)
	{
		FindTextureFileNameFromCRC(crc, filename, sizeof(filename));
	}

	RedirectDownload(playerid, sprintf(Setting[setting_remote_download_url], filename));

	return 1;
}


public OnIncomingConnection(playerid, ip_address[], port)
{
	playerIncoming[playerid] = true;
    pInfo[playerid][player_connected] = false; 
    return 1;
}


public OnPlayerFinishedDownloading(playerid, virtualworld)
{
	if(pInfo[playerid][player_connected]) return 1;

	pInfo[playerid][player_connected] = true;

	OutputLoginForm(playerid, true);

	return 1;
}

public OnPlayerConnect(playerid)
{
	if( IsPlayerNPC(playerid) ) return 1;

    SendClientMessage(playerid, -1, "test deploy");

	for(new i=0;i<11;i++) SetPlayerSkillLevel(playerid, i, 999);
	
	// Czycimy dane gracza
	CleanGlobalData(playerid);
	CleanPlayerData(playerid);
	
	CreatePlayerTextdraws(playerid);
	
	gInfo[playerid][global_join_time] = gettime();
	
	UpdatePlayerColor(playerid);
	
	// Ustawiamy specta
	TogglePlayerSpectating(playerid, 1);
	
	// Pobieramy nick i ip
	GetPlayerName(playerid, pInfo[playerid][player_name], 60);
	strreplace_char(pInfo[playerid][player_name], '_', ' ');
	GetPlayerIp(playerid, gInfo[playerid][global_ip], 20);
	
	SetPlayerRealTime(playerid);
	
	SetPlayerWeather(playerid, Setting[setting_server_weather]);

	OnPlayerFinishedDownloading(playerid, -1);

	/** AFTER RESTART FIX **/
	/*
	if( !playerIncoming[playerid] )
	{
		pInfo[playerid][player_called_incoming] = false;
		OnPlayerFinishedDownloading(playerid, -1);
	}
	else pInfo[playerid][player_called_incoming] = true;

	playerIncoming[playerid] = false;*/
	return 1;
}


public OnPlayerDisconnect(playerid, reason)
{
	if( IsPlayerNPC(playerid) ) return 1;
	if( !pInfo[playerid][player_logged] ) return 1;


	PlayerLog(sprintf("Disconnected {REASON:%d}", reason), pInfo[playerid][player_id], "session");
	PlayerLog(sprintf("{PC_LOSS:%.2f,CONN_TIME:%dms,MSG_RECV:%d,BYTE_RECV:%d,MSg_SEND:%d,BYTE_SENT:%d,MSG_RCV_LAST_SEC:%d}", NetStats_PacketLossPercent(playerid), NetStats_GetConnectedTime(playerid), NetStats_MessagesReceived(playerid), NetStats_BytesReceived(playerid), NetStats_MessagesSent(playerid), NetStats_BytesSent(playerid), NetStats_MessagesRecvPerSecond(playerid)), pInfo[playerid][player_id], "session");

	if(pInfo[playerid][player_boombox_id] > 0)
	{
		foreach(new p : Player)
		{
			if(GetPlayerVirtualWorld(playerid) == GetPlayerVirtualWorld(p) && pInfo[p][player_area] == pInfo[playerid][player_area] && pInfo[p][player_has_as_mus])
			{
				pInfo[p][player_has_as_mus] = false;
				StopAudioStreamForPlayer(p);
			}
		}
		Area[pInfo[playerid][player_area]][area_music_url][0] = EOS;
		DestroyDynamicObject(pInfo[playerid][player_boombox_id]);
		pInfo[playerid][player_boombox_id] = 0;
	}
	action_OnPlayerDisconnect(playerid, reason);

	pInfo[playerid][player_called_incoming] = false;

	TextDrawHideForPlayer(playerid, HonestLogoAboveHud);
	TextDrawHideForPlayer(playerid, LSNtd);
	TextDrawHideForPlayer(playerid, LSNtd2);

	if( reason == 0 || aRestartExecuting )
	{
		cmd_qs(playerid, "");
	}
	
	new Float:pos[3];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);

	new Text3D:leftLabel;

	switch( reason )
	{
		case 0:
		{
			leftLabel = CreateDynamic3DTextLabel(sprintf("(( %s (Timeout) ))", pInfo[playerid][player_name]), COLOR_GREY, pos[0], pos[1], pos[2], 4.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
		}
		
		case 1:
		{
			leftLabel = CreateDynamic3DTextLabel(sprintf("(( %s (/quit) ))", pInfo[playerid][player_name]), COLOR_GREY, pos[0], pos[1], pos[2], 4.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
		}
		
		case 2:
		{
			if( pInfo[playerid][player_qs] ) leftLabel = CreateDynamic3DTextLabel(sprintf("(( %s (/qs) ))", pInfo[playerid][player_name]), COLOR_GREY, pos[0], pos[1], pos[2], 4.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
			else leftLabel = CreateDynamic3DTextLabel(sprintf("(( %s (Kick/Ban) ))", pInfo[playerid][player_name]), COLOR_GREY, pos[0], pos[1], pos[2], 4.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
		}
	}

	if( IsValidDynamic3DTextLabel(leftLabel) ) defer RemovePlayerLeftLabel[6000](leftLabel);
	
	if( pInfo[playerid][player_lsn_wywiad_starter] != INVALID_PLAYER_ID || pInfo[playerid][player_lsn_wywiad_with] != INVALID_PLAYER_ID )
	{
		cmd_wywiad(playerid, "zakoncz");
	}
	
	if( Iter_Contains(Spectators, playerid) ) Iter_Remove(Spectators, playerid);
	
	// Check if was spectated
	foreach(new p : Player)
	{
		if( pInfo[p][player_admin_spec_id] == playerid && pInfo[p][player_admin_spec] )
		{
			new targetid = GetPlayerNextSpectateId(p);
			if( targetid == INVALID_PLAYER_ID ) targetid = GetPlayerPrevSpectateId(p);
			
			if( targetid != INVALID_PLAYER_ID ) PlayerSetSpectate(p, targetid);
			else cmd_specoff(playerid, "");
		}
	}

	// admin fly
	if( pInfo[playerid][player_admin_fly] )
	{
		cmd_fly(playerid, "");
	}
	
	// jesli jest skuty
	if( pInfo[playerid][player_is_cuffed] )
	{
		new targetid = pInfo[playerid][player_cuff_targetid];

		new itemid = GetPlayerUsedItem(targetid, ITEM_TYPE_CUFFS);
		Item[itemid][item_used] = false;
		SendClientMessage(targetid, COLOR_LIGHTER_RED, sprintf("Gracz %s (ID: %d, UID: %d), ktrego skue wyszed z serwera.", pInfo[playerid][player_name], playerid, pInfo[playerid][player_id]));
		PlayerLog(sprintf("During disconnect was cuffed by %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "session");
	}

	// jesli kogos skuwal
	new cuffsid = GetPlayerUsedItem(playerid, ITEM_TYPE_CUFFS);
	if( cuffsid != -1 )
	{
		new targetid = Item[cuffsid][item_value1];

		Item[cuffsid][item_used] = false;

		pInfo[targetid][player_is_cuffed] = false;
		pInfo[targetid][player_cuff_targetid] = INVALID_PLAYER_ID;

		RemovePlayerAttachedObject(targetid, pInfo[targetid][player_cuff_oindex]);
		pInfo[targetid][player_cuff_oindex] = -1;

		SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);

		SendClientMessage(targetid, COLOR_LIGHTER_RED, sprintf("Gracz %s (ID: %d), ktry Ci sku wyszed z serwera.", pInfo[playerid][player_name], playerid));
		PlayerLog(sprintf("During disconnect was cuffing player %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "session");
	}


	// Session saving
	if( gInfo[playerid][global_registered] )
	{
		if( sInfo[playerid][session_state] == SESSION_STATE_NONE ) sInfo[playerid][session_state] = SESSION_STATE_ABORT;
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_sessions` SET `session_end` = %d, `session_extraid` = %d, `session_owner` = %d WHERE `session_uid` = %d", gettime(), sInfo[playerid][session_state], pInfo[playerid][player_id], sInfo[playerid][session_id]));
	}
	
	// Edycja obiektu
	if( IsValidDynamicObject(pInfo[playerid][player_edited_object]) )
	{
		OnPlayerEditDynamicObject(playerid, pInfo[playerid][player_edited_object], EDIT_RESPONSE_CANCEL, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}
	
	for(new r;r<20;r++)
	{
		if( IsValidDynamicObject(pInfo[playerid][player_blockades][r]) ) DestroyDynamicObject(pInfo[playerid][player_blockades][r]);
	}
	
	if( pInfo[playerid][player_creating_area] )
	{
		if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
		if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
		
		GangZoneDestroy(pInfo[playerid][player_carea_zone]);
	}
	
	if( pInfo[playerid][player_is_in_binco_access] )
	{
		DestroyDynamicObject(pInfo[playerid][player_binco_access_object]);
	}

	if( pInfo[playerid][player_is_gym_training] )
	{
		Object[pInfo[playerid][player_gym_object]][object_is_used] = false;
	}
	
	if( pOffer[playerid][offer_type] > 0 )
	{
		if( pOffer[playerid][offer_sellerid] == INVALID_PLAYER_ID )
		{
			// on oferowal
			new buyerid = pOffer[playerid][offer_buyerid];
			
			OnPlayerOfferRejected(buyerid, pOffer[buyerid][offer_type]);

			for(new x=0; e_player_offer:x != e_player_offer; x++)
			{
				pOffer[buyerid][e_player_offer:x] = 0;
			}
			for(new i;i<6;i++) PlayerTextDrawHide(buyerid, pInfo[buyerid][OfferTD][i]);
			CancelSelectTextDraw(buyerid);
			SendGuiInformation(buyerid, "Wystpi bd", "Gracz, ktry zoy Ci ofert wyszed z serwera.");
		}
		else OnPlayerOfferResponse(playerid, 0);
	}
	
	if( pInfo[playerid][player_lsn_live] )
	{
		cmd_live(playerid, "");
	}
	
	if( pInfo[playerid][player_lookup_area] )
	{
		cmd_as(playerid, "podglad");
	}
	
	if( pInfo[playerid][player_admin_duty] )
	{
		cmd_aduty(playerid, "");
	}
	
	new slot = GetPlayerDutySlot(playerid);
	if( slot > -1 )
	{
		cmd_g(playerid, sprintf("%d duty", slot+1));
	}
	
	for(new i;i<13;i++)
	{
		if( pWeapon[playerid][i][pw_itemid] > -1 ) Item_Use(pWeapon[playerid][i][pw_itemid], playerid);
	}
	
	for(new item;item<MAX_ITEMS;item++)
	{
		if( Item[item][item_uid] < 1 ) continue;
		if( Item[item][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[item][item_owner] != pInfo[playerid][player_id] ) continue;
		
		DeleteItem(item);
	}
	
	foreach(new gs : GlobalSpawns)
	{
		Streamer_RemoveArrayData(STREAMER_TYPE_3D_TEXT_LABEL, GlobalSpawn[gs][gspawn_label], E_STREAMER_PLAYER_ID, playerid);
	}
	
	PlayerTextDrawHide(playerid, pInfo[playerid][AreaInfo]);
	
	SavePlayer(playerid);
	
	DestroyPlayerTextdraws(playerid);
	
	if( pInfo[playerid][player_phone_call_started] )
	{
		if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID )
		{
			new targetid = -1;
			if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
			else targetid = pInfo[playerid][player_phone_caller];
			
			PlayerLog(sprintf("During disconnect was on phone with %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "session");

			SendClientMessage(targetid, COLOR_YELLOW, "Rozmowa przerwana.");
			pInfo[targetid][player_phone_call_started] = false;
			pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
			pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
			
			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
			SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
			if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
			if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
		}
	}
	else
	{
		if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID && pInfo[playerid][player_phone_receiver] != INVALID_PLAYER_ID )
		{
			new targetid = -1;
			if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
			else targetid = pInfo[playerid][player_phone_caller];
			
			PlayerLog(sprintf("During disconnect was on phone with %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "session");

			SendClientMessage(targetid, COLOR_YELLOW, "Rozmowa przerwana.");
			pInfo[targetid][player_phone_call_started] = false;
			pInfo[targetid][player_phone_receiver] = INVALID_PLAYER_ID;
			pInfo[targetid][player_phone_caller] = INVALID_PLAYER_ID;
			
			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
			SetPlayerSpecialAction(targetid, SPECIAL_ACTION_STOPUSECELLPHONE);
			if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
			if( pInfo[targetid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(targetid, pInfo[targetid][player_phone_object_index]);
		}
	}	
	return 1;
}

public OnReverseDNS(ip[], host[], extra)
{
	strcopy(pInfo[extra][player_hostname], host, 50);
}

stock OnPlayerWeaponChange(playerid, newweapon, oldweapon)
{
	new wslot = GetWeaponSlot(newweapon);
	if( gettime() - pInfo[playerid][player_spawn_time] > 5 )
	{
		if( newweapon > 0 && (pWeapon[playerid][wslot][pw_id] != newweapon || pWeapon[playerid][wslot][pw_itemid] < 0) )
		{
			if( !(newweapon == 46 && (GetVehicleType(GetPlayerVehicleID(playerid)) == VEHICLE_TYPE_AIRPLANE || GetVehicleType(GetPlayerVehicleID(playerid)) == VEHICLE_TYPE_HELICOPTER)) )
			{
				PlayerLog(sprintf("Probably weaponhack {NW:%d,OW:%d}", newweapon, pWeapon[playerid][wslot][pw_id]), pInfo[playerid][player_id], "anticheat");

				new str[80];
				format(str, sizeof(str), "AntyCheat: Weapon spawn w%d ow%d", newweapon, pWeapon[playerid][wslot][pw_id]);
				AddPlayerPenalty(playerid, PENALTY_TYPE_KICK, INVALID_PLAYER_ID, 0, str);
				return 1;
			}
			else
			{
				GivePlayerWeapon(playerid, 46, -5);
			}
		}
	}
	
	if( oldweapon > 0 && oldweapon < 47 )
	{
		new slot = GetWeaponSlot(oldweapon), wid, wammo;
		GetPlayerWeaponData(playerid, slot, wid, wammo);

		if( pWeapon[playerid][slot][pw_itemid] > -1 && wid > 0 && wammo == 0 )
		{
			new itemid = pWeapon[playerid][slot][pw_itemid];
			if( Item[itemid][item_used] )
			{
				pWeapon[playerid][slot][pw_ammo] = 0;
				Item_Use(pWeapon[playerid][slot][pw_itemid], playerid);
			}
		}
	}

	if( newweapon > 1 && newweapon < 47 )
	{
		wslot = GetWeaponSlot(newweapon);
		if( pWeapon[playerid][wslot][pw_object_index] > -1 )
		{
			RemovePlayerAttachedObject(playerid, pWeapon[playerid][wslot][pw_object_index]);
			pWeapon[playerid][wslot][pw_object_index] = -1;
		}
	}
	
	if( oldweapon > 0 && oldweapon < 47 )
	{
		wslot = GetWeaponSlot(oldweapon);
		if( pWeapon[playerid][wslot][pw_id] != oldweapon ) return 1;
		if( pWeapon[playerid][wslot][pw_id] != oldweapon ) return 1;
		if( WeaponVisualModel[oldweapon] > -1 )
		{
			new freeid = GetPlayerFreeAttachSlot(playerid);
			if( freeid == -1 ) return 1;
			
			new itemid = pWeapon[playerid][wslot][pw_itemid], ow = oldweapon;
			if( Item[itemid][item_group] > 0 ) SetPlayerAttachedObject(playerid, freeid, WeaponVisualModel[ow], WeaponVisualBone[ow], FWeaponVisualPos[ow][0], FWeaponVisualPos[ow][1], FWeaponVisualPos[ow][2], FWeaponVisualPos[ow][3], FWeaponVisualPos[ow][4], FWeaponVisualPos[ow][5], FWeaponVisualPos[ow][6], FWeaponVisualPos[ow][7], FWeaponVisualPos[ow][8]);
			else SetPlayerAttachedObject(playerid, freeid, WeaponVisualModel[ow], WeaponVisualBone[ow], WeaponVisualPos[ow][0], WeaponVisualPos[ow][1], WeaponVisualPos[ow][2], WeaponVisualPos[ow][3], WeaponVisualPos[ow][4], WeaponVisualPos[ow][5], WeaponVisualPos[ow][6], WeaponVisualPos[ow][7], WeaponVisualPos[ow][8]);
			pWeapon[playerid][wslot][pw_object_index] = freeid;
		}
	}
	return 1;
}

public OnPlayerUpdate(playerid)
{
	if( IsPlayerNPC(playerid) ) return 1;

	if( pInfo[playerid][player_logged] )
	{
		new wid = GetPlayerWeapon(playerid);
		if( pInfo[playerid][player_held_weapon] != wid )
		{
			OnPlayerWeaponChange(playerid, wid, pInfo[playerid][player_held_weapon]);
			pInfo[playerid][player_held_weapon] = wid;
		}
	
		if( pInfo[playerid][player_afk] )
		{
			RemovePlayerStatus(playerid, PLAYER_STATUS_AFK);	
			pInfo[playerid][player_afk] = false;
		}
		
		pInfo[playerid][player_last_activity] = gettime();
	}
	
	if(pInfo[playerid][player_has_animation]) ApplyAnimation(playerid, "CRACK", "crckidle1", 4.1, 1, 0, 0, 0, 0, 1);

	new Keys, UpDown, LeftRight;
    GetPlayerKeys(playerid, Keys, UpDown, LeftRight);

 	if(PlayerHasBlock(playerid, BLOCK_RUN))
 	{
		if( (UpDown != 0 || LeftRight != 0) && (Keys & KEY_SPRINT) && PlayerHasBlock(playerid, BLOCK_RUN) && !pInfo[playerid][player_is_sprinting] )
		{
			switch( pInfo[playerid][player_walk_style] )
			{
				case 1: ApplyAnimation(playerid, "PED", "WALK_player", 4.1, 1, 1, 1, 1, 50, 1);
				case 2: ApplyAnimation(playerid, "PED", "WALK_civi", 4.1, 1, 1, 1, 1, 50, 1);
				case 3: ApplyAnimation(playerid, "PED", "WALK_gang1", 4.1, 1, 1, 1, 1, 50, 1);
				case 4: ApplyAnimation(playerid, "PED", "WALK_gang2", 4.1, 1, 1, 1, 1, 50, 1);
				case 5: ApplyAnimation(playerid, "PED", "WALK_old", 4.1, 1, 1, 1, 1, 50, 1);
				case 6: ApplyAnimation(playerid, "PED", "WALK_fatold", 4.1, 1, 1, 1, 1, 50, 1);
				case 7: ApplyAnimation(playerid, "PED", "WALK_fat", 4.1, 1, 1, 1, 1, 50, 1);
				case 8: ApplyAnimation(playerid, "PED", "WOMAN_walknorm", 4.1, 1, 1, 1, 1, 50, 1);
				case 9: ApplyAnimation(playerid, "PED", "WOMAN_walkbusy", 4.1, 1, 1, 1, 1, 50, 1);
				case 10: ApplyAnimation(playerid, "PED", "WOMAN_walkpro", 4.1, 1, 1, 1, 1, 50, 1);
				case 11: ApplyAnimation(playerid, "PED", "WOMAN_walksexy", 4.1, 1, 1, 1, 1, 50, 1);
				case 12: ApplyAnimation(playerid, "PED", "WALK_drunk", 4.1, 1, 1, 1, 1, 50, 1);
				case 13: ApplyAnimation(playerid, "PED", "Walk_Wuzi", 4.1, 1, 1, 1, 1, 50, 1);
			}

			GameTextForPlayer(playerid, "~r~Blokada ~w~biegania", 1000, 3);

			pInfo[playerid][player_is_sprinting] = true;
		}
		
		if( !(Keys & KEY_SPRINT) && pInfo[playerid][player_is_sprinting] )
		{
			pInfo[playerid][player_is_sprinting] = false;
			ClearAnimations(playerid, 1);
			StopPlayerAnimation(playerid);
		}
	}
 
	if( UpDown == KEY_UP && pInfo[playerid][player_move_key] != KEY_UP )
	{
		pInfo[playerid][player_move_key] = KEY_UP;
		OnPlayerPressMoveKey(playerid, 1);
	}
	else if( UpDown == KEY_DOWN && pInfo[playerid][player_move_key] != KEY_DOWN )
	{
		pInfo[playerid][player_move_key] = KEY_DOWN;
		OnPlayerPressMoveKey(playerid, 2);
	}
	else if( LeftRight == KEY_LEFT && pInfo[playerid][player_move_key] != KEY_LEFT )
	{
		pInfo[playerid][player_move_key] = KEY_LEFT;
		OnPlayerPressMoveKey(playerid, 3);
	}
	else if( LeftRight == KEY_RIGHT && pInfo[playerid][player_move_key] != KEY_RIGHT )
	{
		pInfo[playerid][player_move_key] = KEY_RIGHT;
		OnPlayerPressMoveKey(playerid, 4);
	}
	
	if( UpDown == 0 && LeftRight == 0 ) 
	{
		pInfo[playerid][player_move_key] = 0;
		
		if( (Keys & KEY_WALK) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && pInfo[playerid][player_is_walking] )
		{
			pInfo[playerid][player_is_walking] = false;
			StopPlayerAnimation(playerid);
		}
	}
	else
	{
		if( (Keys & KEY_WALK) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && !pInfo[playerid][player_is_walking] )
		{
			pInfo[playerid][player_is_walking] = true;
			switch( pInfo[playerid][player_walk_style] )
			{
				case 1: ApplyAnimation(playerid, "PED", "WALK_player", 4.1, 1, 1, 1, 1, 50, 1);
				case 2: ApplyAnimation(playerid, "PED", "WALK_civi", 4.1, 1, 1, 1, 1, 50, 1);
				case 3: ApplyAnimation(playerid, "PED", "WALK_gang1", 4.1, 1, 1, 1, 1, 50, 1);
				case 4: ApplyAnimation(playerid, "PED", "WALK_gang2", 4.1, 1, 1, 1, 1, 50, 1);
				case 5: ApplyAnimation(playerid, "PED", "WALK_old", 4.1, 1, 1, 1, 1, 50, 1);
				case 6: ApplyAnimation(playerid, "PED", "WALK_fatold", 4.1, 1, 1, 1, 1, 50, 1);
				case 7: ApplyAnimation(playerid, "PED", "WALK_fat", 4.1, 1, 1, 1, 1, 50, 1);
				case 8: ApplyAnimation(playerid, "PED", "WOMAN_walknorm", 4.1, 1, 1, 1, 1, 50, 1);
				case 9: ApplyAnimation(playerid, "PED", "WOMAN_walkbusy", 4.1, 1, 1, 1, 1, 50, 1);
				case 10: ApplyAnimation(playerid, "PED", "WOMAN_walkpro", 4.1, 1, 1, 1, 1, 50, 1);
				case 11: ApplyAnimation(playerid, "PED", "WOMAN_walksexy", 4.1, 1, 1, 1, 1, 50, 1);
				case 12: ApplyAnimation(playerid, "PED", "WALK_drunk", 4.1, 1, 1, 1, 1, 50, 1);
				case 13: ApplyAnimation(playerid, "PED", "Walk_Wuzi", 4.1, 1, 1, 1, 1, 50, 1);
			}
		}
		
		if( !(Keys & KEY_WALK) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && pInfo[playerid][player_is_walking] )
		{
			pInfo[playerid][player_is_walking] = false;
			StopPlayerAnimation(playerid);
		}
	}

	return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
	if( IsPlayerNPC(playerid) ) return 0;
	if( !pInfo[playerid][player_logged] ) return 0;

	/* zadajcy nie jest zalogowany */
	if(issuerid != INVALID_PLAYER_ID && !pInfo[issuerid][player_logged]) return 0;

	/* ma bw */
	if( pInfo[playerid][player_bw] > 0 ) return 0;

	/* zadajcy jest skuty */
	if( issuerid != INVALID_PLAYER_ID && pInfo[issuerid][player_is_cuffed] ) return 0;

	/* Admin duty */
	if( pInfo[playerid][player_admin_duty] ) return 0;
	
	new weapon = 0;
	if(pInfo[playerid][player_used_weapon] != -1) weapon = Item[pInfo[playerid][player_used_weapon]][item_uid]; 

	if(weapon != 0)
	{
		//if 
	}

	pInfo[playerid][player_taken_damage] = gettime();
	pInfo[playerid][player_taking_damage] = true;
	UpdatePlayerLabel(playerid);
	
	// przeliczanie obrae
	if(issuerid != INVALID_PLAYER_ID)
	{
		if(IsPlayerConnected(issuerid))
		{
			PlayerLog(sprintf("Was damaged by %s {W:%d,HP:%.1f,PING:%d,WUID:%d,WBODYPART:%d}", PlayerLogLink(pInfo[issuerid][player_id]), weaponid, amount, GetPlayerPing(playerid), weapon, bodypart), pInfo[playerid][player_id], "dmg");
			PlayerLog(sprintf("Damaged %s {W:%d,HP:%.1f,PING:%d,WUID:%d,WBODYPART:%d}", PlayerLogLink(pInfo[playerid][player_id]), weaponid, amount, GetPlayerPing(playerid), weapon, bodypart), pInfo[issuerid][player_id], "dmg");

			if(weaponid == 0)
			{
				// domyslnie 4-7hp/uderzenie
				new Float:damage_multip = floatdiv((pInfo[issuerid][player_strength]-3500), 500);
				if(damage_multip < 1.0) damage_multip = 1.0;

				amount = (random(4) + 4) * damage_multip;
			}
		}
	}
	else
	{
		PlayerLog(sprintf("Was damaged by terrain/vehicle {W:%d,HP:%.1f,PING:%d,WUID:%d,WBODYPART:%d}", weaponid, amount, GetPlayerPing(playerid), weapon, bodypart), pInfo[playerid][player_id], "dmg");
	}

	new damage_level = pInfo[playerid][player_damage];

	if( (pInfo[playerid][player_health] - amount) <= 0.0 )
	{
		if( pInfo[playerid][player_hurted] ) StopPlayerHurted(playerid);
		if( pInfo[playerid][player_belt] ) pInfo[playerid][player_belt] = false;
		
		if( issuerid != INVALID_PLAYER_ID )
		{
			switch( weaponid )
			{
				case 0,1,2,3,5,6,7,10,11,12,13,14,15,41,42,40,39,43,44,45,46:
				{
					switch(random(10))
					{
						case 0..5:
						{
							damage_level = DAMAGE_LEVEL_LOW;
							pInfo[playerid][player_hospitalization_costs] += 400;
						}
						case 6..8:
						{
							damage_level = DAMAGE_LEVEL_BAD;
							pInfo[playerid][player_hospitalization_costs] += 900;
						}
						case 9:
						{
							damage_level = DAMAGE_LEVEL_CRITICAL;
							pInfo[playerid][player_hospitalization_costs] += 1500;
						}
					}
					pInfo[playerid][player_bw] = 60 * 5;
				}
				default:
				{
					switch(bodypart)
					{
						case 3: //torso
						{
							switch(random(10))
							{
								case 0..2:
								{
									damage_level = DAMAGE_LEVEL_CRITICAL;
									pInfo[playerid][player_hospitalization_costs] += 2800;									
								}
								case 3..7:
								{
									damage_level = DAMAGE_LEVEL_BAD;
									pInfo[playerid][player_hospitalization_costs] += 1600;
								}
								case 8..9:
								{
									damage_level = DAMAGE_LEVEL_LOW;
									pInfo[playerid][player_hospitalization_costs] += 800;
								}
							}
						}
						case 4: //groin
						{
							switch(random(10))
							{
								case 0..1:
								{
									damage_level = DAMAGE_LEVEL_CRITICAL;
									pInfo[playerid][player_hospitalization_costs] += 2300;									
								}
								case 2..5:
								{
									damage_level = DAMAGE_LEVEL_BAD;
									pInfo[playerid][player_hospitalization_costs] += 1100;
								}
								case 6..9:
								{
									damage_level = DAMAGE_LEVEL_LOW;
									pInfo[playerid][player_hospitalization_costs] += 600;								
								}
							}
						}
						case 5,6,7,8: //arms&legs
						{
							switch(random(10))
							{
								case 0..1:
								{
									damage_level = DAMAGE_LEVEL_CRITICAL;
									pInfo[playerid][player_hospitalization_costs] += 1800;	
								}
								case 2..6:
								{
									damage_level = DAMAGE_LEVEL_BAD;
									pInfo[playerid][player_hospitalization_costs] += 900;
								}
								case 7..9:
								{
									damage_level = DAMAGE_LEVEL_LOW;
									pInfo[playerid][player_hospitalization_costs] += 300;											
								}
							}
						}
						case 9: //head
						{
							switch(random(10))
							{
								case 0..5:
								{
									damage_level = DAMAGE_LEVEL_CRITICAL;
									pInfo[playerid][player_hospitalization_costs] += 4000;
								}
								case 6..8:
								{
									damage_level = DAMAGE_LEVEL_BAD;
									pInfo[playerid][player_hospitalization_costs] += 2000;
								}
								case 9:
								{
									damage_level = DAMAGE_LEVEL_LOW;
									pInfo[playerid][player_hospitalization_costs] += 1200;									
								}
							}
						}
						default:
						{
							switch(random(10))
							{
								case 0..5:
								{
									damage_level = DAMAGE_LEVEL_CRITICAL;
									pInfo[playerid][player_hospitalization_costs] += 4000;
								}
								case 6..8:
								{
									damage_level = DAMAGE_LEVEL_BAD;
									pInfo[playerid][player_hospitalization_costs] += 2000;
								}
								case 9:
								{
									damage_level = DAMAGE_LEVEL_LOW;
									pInfo[playerid][player_hospitalization_costs] += 1200;									
								}
							}
						}
					}

					pInfo[playerid][player_bw] = 600;
				}
			}
			
			new wslot = GetWeaponSlot(weaponid), itemid = pWeapon[issuerid][wslot][pw_itemid];
				
			pInfo[playerid][player_bw_killer] = pInfo[issuerid][player_id];
			if(itemid > -1) pInfo[playerid][player_bw_weapon] = Item[itemid][item_uid];
			
			if( GetWeaponType(weaponid) == WEAPON_TYPE_MELEE ) pInfo[playerid][player_bw_reason] = BW_REASON_BEAT;
			else if( GetWeaponType(weaponid) == WEAPON_TYPE_SHORT || GetWeaponType(weaponid) == WEAPON_TYPE_LONG ) pInfo[playerid][player_bw_reason] = BW_REASON_SHOOT;
			else pInfo[playerid][player_bw_reason] = BW_REASON_FIRE;
		}
		else 
		{
			switch(random(10))
			{
				case 0..5:
				{
					damage_level = DAMAGE_LEVEL_LOW;
					pInfo[playerid][player_hospitalization_costs] += 400;
				}
				case 6..8:
				{
					damage_level = DAMAGE_LEVEL_BAD;
					pInfo[playerid][player_hospitalization_costs] += 800;
				}
				case 9:
				{
					damage_level = DAMAGE_LEVEL_CRITICAL;
					pInfo[playerid][player_hospitalization_costs] += 1600;
				}
			}

			pInfo[playerid][player_bw_reason] = BW_REASON_SUICIDE;
			pInfo[playerid][player_bw_killer] = 0;
			pInfo[playerid][player_bw_weapon] = 0;
			pInfo[playerid][player_bw] = 60 * 5;
		}
		pInfo[playerid][player_bw_end_time] = pInfo[playerid][player_bw] + gettime();  
		// Setup bw pos

		if(pInfo[playerid][player_damage] > damage_level) damage_level = pInfo[playerid][player_damage];

		if(damage_level == DAMAGE_LEVEL_CRITICAL && GetMedicsOnline() == 0) damage_level = DAMAGE_LEVEL_BAD;

		switch(damage_level)
		{
			case DAMAGE_LEVEL_CRITICAL:
			{
				new str[850];
				format(str, sizeof(str), "%s> Twoja posta w wyniku odniesionych powanych obrae stracia przytomno.\n\nPamitaj, e poniewa na "HEX_COLOR_HONEST"Honest RolePlay "HEX_COLOR_SAMP"cenimy sobie wysoki poziom gry RolePlay po stanie nieprzytomnoci musisz nadal odpowiednio odgrywa rann posta.\nNaturalnym jest, e dopiero po odzyskaniu przytomnoci Twoja posta bdzie osabiona oraz zdezorientowana. Postaraj si to odwzorowa w grze In Character zarwno samemu jak i z innymi graczami.\n", str);
				format(str, sizeof(str), "%sZe wzgldu na utrat przytomnoci przez Twoj posta skryptowe funkcje jak ruch, rozmowa czy komendy zostay Ci odebrane.\nTwoja posta znajduje si w krytycznym stanie, dlatego do czasu przyjazdu medykw bdzie ograniczna.\n\n"HEX_COLOR_WHITE"> Po przegraniu dwch godzin komunikat przestanie si wywietla. yczymy miej gry!", str, pInfo[playerid][player_name]);
				SendGuiInformation(playerid, "Odzyskanie przytomnoci", str);

				pInfo[playerid][player_bw] = 9999;
				pInfo[playerid][player_bw_end_time] = 9999;
			}

			default:
			{
				new str[850];
				format(str, sizeof(str), "%s> Twoja posta w wyniku odniesionych obrae stracia przytomno.\n\nPamitaj, e poniewa na "HEX_COLOR_HONEST"Honest RolePlay "HEX_COLOR_SAMP"cenimy sobie wysoki poziom gry RolePlay po stanie nieprzytomnoci musisz nadal odpowiednio odgrywa rann posta.\nNaturalnym jest, e dopiero po odzyskaniu przytomnoci Twoja posta bdzie osabiona oraz zdezorientowana. Postaraj si to odwzorowa w grze In Character zarwno samemu jak i z innymi graczami.\n", str);
				format(str, sizeof(str), "%sZe wzgldu na utrat przytomnoci przez Twoj posta skryptowe funkcje jak ruch, rozmowa czy komendy zostay Ci odebrane.\n\n"HEX_COLOR_WHITE"> Po przegraniu dwch godzin komunikat przestanie si wywietla. yczymy miej gry!", str, pInfo[playerid][player_name]);
				SendGuiInformation(playerid, "Odzyskanie przytomnoci", str);
			}
		}

		pInfo[playerid][player_examined] = false;
		pInfo[playerid][player_damage] = damage_level;

		mysql_tquery(g_sql, sprintf("UPDATE crp_characters SET char_damage = %d, char_hospitalization_costs = %d WHERE char_uid = %d", pInfo[playerid][player_damage], pInfo[playerid][player_hospitalization_costs], pInfo[playerid][player_id]));

		SetPlayerHealth(playerid, 0);
		PlayerLog(sprintf("Brutally wounded {TIME:%dmin}", pInfo[playerid][player_bw] / 60), pInfo[playerid][player_id], "dmg");
	}
	else 
	{
		SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health] - amount));
		
		if( pInfo[playerid][player_health] <= 30.0 && issuerid != INVALID_PLAYER_ID && GetWeaponType(weaponid) != WEAPON_TYPE_MELEE && weaponid > 0 && !pInfo[playerid][player_hurted] )
		{
			pInfo[playerid][player_hurted] = true;
			pInfo[playerid][player_looped_anim] = false;
			
			AddPlayerStatus(playerid, PLAYER_STATUS_HURT);
			
			ApplyAnimation(playerid, "SWEET", "Sweet_injuredloop", 4.0, 0, 1, 1, 1, 0, 1);

			PlayerLog("Hurted status", pInfo[playerid][player_id], "dmg");
			
			defer StopPlayerHurted[30000](playerid);
		}
	}

	return 0;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	if( killerid != INVALID_PLAYER_ID ) PlayerLog(sprintf("Self-reported or system-called death by %s {REASON:%d}", PlayerLogLink(pInfo[killerid][player_id]), reason), pInfo[playerid][player_id], "death");
	else PlayerLog(sprintf("Self-reported or system-called death {REASON:%d}", reason), pInfo[playerid][player_id], "death");

	for(new i;i<13;i++)
	{
		if( pWeapon[playerid][i][pw_itemid] > -1 ) Item_Use(pWeapon[playerid][i][pw_itemid], playerid);
	}

	if(pInfo[playerid][player_admin_duty])
	{
		new
			Float:x,
			Float:y,
			Float:z,
			Float:a;
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);
						
		pInfo[playerid][player_quit_pos][0] = x;
		pInfo[playerid][player_quit_pos][1] = y;
		pInfo[playerid][player_quit_pos][2] = z;
		pInfo[playerid][player_quit_pos][3] = a;
		pInfo[playerid][player_quit_vw] = GetPlayerVirtualWorld(playerid);
		pInfo[playerid][player_quit_int] = GetPlayerInterior(playerid);
		pInfo[playerid][player_health] = 100.0;
		pInfo[playerid][player_admin_duty_died] = true;
		
		scrp_SpawnPlayer(playerid);
	}
	
	if( pInfo[playerid][player_bw] == 0 )
	{
		pInfo[playerid][player_bw_vehicle] = GetPlayerVehicleID(playerid);
		pInfo[playerid][player_bw_vehicle_seat] = pInfo[playerid][player_occupied_vehicle_seat];
		pInfo[playerid][player_bw_reason] = BW_REASON_SUICIDE;
		pInfo[playerid][player_bw_killer] = 0;
		pInfo[playerid][player_bw_weapon] = 0;
		pInfo[playerid][player_bw] = 60 * 5;
		pInfo[playerid][player_bw_end_time] = pInfo[playerid][player_bw] + gettime();
	
		PlayerLog("Brutally wounded (gta-called) {TIME:5min}", pInfo[playerid][player_id], "dmg");
	}

	if( pInfo[playerid][player_is_cuffed] )
	{
		new issuerid = pInfo[playerid][player_cuff_targetid];
		new itemid = GetPlayerUsedItem(issuerid, ITEM_TYPE_CUFFS);

		pInfo[playerid][player_is_cuffed] = false;
		pInfo[playerid][player_cuff_targetid] = INVALID_PLAYER_ID;

		RemovePlayerAttachedObject(playerid, pInfo[playerid][player_cuff_oindex]);
		pInfo[playerid][player_cuff_oindex] = -1;

		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);

		Item[itemid][item_used] = false;

		SendPlayerInformation(issuerid, "Gracz, ktorego skules zginal przez co zostal automatycznie odkuty.", 5000);

		PlayerLog(sprintf("Died when was cuffed by %s", PlayerLogLink(pInfo[issuerid][player_id])), pInfo[playerid][player_id], "death");
	}
	
	// Check if was spectated
	foreach(new p : Player)
	{
		if( pInfo[p][player_admin_spec_id] == playerid && pInfo[p][player_admin_spec] )
		{
			new targetid = GetPlayerNextSpectateId(p);
			if( targetid == INVALID_PLAYER_ID ) targetid = GetPlayerPrevSpectateId(p);
			
			if( targetid != INVALID_PLAYER_ID ) PlayerSetSpectate(p, targetid);
			else cmd_specoff(playerid, "");
		}
	}
	
	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
			
	mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_bw`=%d, `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d WHERE `char_uid`=%d", pInfo[playerid][player_bw], x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), pInfo[playerid][player_id]));
	
	pInfo[playerid][player_quit_pos][0] = x;
	pInfo[playerid][player_quit_pos][1] = y;
	pInfo[playerid][player_quit_pos][2] = z;
	pInfo[playerid][player_quit_pos][3] = a;
	pInfo[playerid][player_quit_vw] = GetPlayerVirtualWorld(playerid);
	pInfo[playerid][player_quit_int] = GetPlayerInterior(playerid);
	pInfo[playerid][player_health] = 0.0;
	
	scrp_SpawnPlayer(playerid);

	return 1;
}

public OnPlayerClickTextDraw(playerid, Text:clickedid)
{
	if( clickedid == Text:INVALID_TEXT_DRAW )
	{
		if( pInfo[playerid][player_group_list_showed] )
		{
			HideGroupsList(playerid);
		}
		else if( pOffer[playerid][offer_type] > 0 && !pOffer[playerid][offer_accepted] )
		{
			OnPlayerOfferResponse(playerid, 0);
		}
	}

	if( pInfo[playerid][player_choosing_carcolor] && clickedid == Text:INVALID_TEXT_DRAW )
	{
		HideCarColorPickerForPlayer(playerid);
		pInfo[playerid][player_choosing_carcolor] = false;

		new senderid = GetPlayerByUid(pInfo[playerid][player_offered_carpaint]);
		if( senderid != INVALID_PLAYER_ID )
		{
			SendClientMessage(senderid, COLOR_GOLD, "Gracz, ktremu oferowae malowanie anulowa wybieranie nowego koloru pojazdu.");
		}

		SendGuiInformation(playerid, "Informacja", "Anulowae wybieranie nowego koloru pojazdu.");

		new vid = GetVehicleByUid(pInfo[playerid][player_carpaint_vuid]);
		ChangeVehicleColor(vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);
	}
	
    return 1;
}

public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
    if( pInfo[playerid][player_group_list_showed] )
	{
		for(new i=0;i<5;i++)
		{
			if( playertextid == pInfo[playerid][GroupsListStaticButtons][i*5] ) cmd_g(playerid, sprintf("%d info", i+1));
			else if( playertextid == pInfo[playerid][GroupsListStaticButtons][(i*5)+1] ) cmd_g(playerid, sprintf("%d pojazdy", i+1));
			else if( playertextid == pInfo[playerid][GroupsListStaticButtons][(i*5)+2] ) cmd_g(playerid, sprintf("%d duty", i+1));
			else if( playertextid == pInfo[playerid][GroupsListStaticButtons][(i*5)+3] ) cmd_g(playerid, sprintf("%d magazyn", i+1));
			else if( playertextid == pInfo[playerid][GroupsListStaticButtons][(i*5)+4] ) cmd_g(playerid, sprintf("%d online", i+1));
		}
	
		HideGroupsList(playerid);
	}

	CarColorPickerCheck(playerid, playertextid);

    return 1;
}

public OnPlayerSelectDynamicObject(playerid, objectid, modelid, Float:x, Float:y, Float:z)
{
	CancelEdit(playerid);
	pInfo[playerid][player_edited_object_no_action] = true;
	if( !CanPlayerEditObject(playerid, objectid) ) return EditDynamicObject(playerid, objectid), CancelEdit(playerid), SendGuiInformation(playerid, "Wystpi bd", "Nie masz uprawnie do edycji tego obiektu.");
	if( IsObjectEdited(objectid) ) return EditDynamicObject(playerid, objectid), CancelEdit(playerid), SendGuiInformation(playerid, "Wystpi bd", "Ten obiekt jest ju edytowany przez kogo innego.");
	pInfo[playerid][player_edited_object_no_action] = false;
	
	Object[objectid][object_is_edited] = true;
	pInfo[playerid][player_edited_object] = objectid;
	
	// Pobieramy sobie poczatkowa pozycje obiektu w razie jakiegos bledu
	GetDynamicObjectPos(objectid, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
	GetDynamicObjectRot(objectid, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);
	
	Object[objectid][object_pos][0] = pInfo[playerid][player_edited_object_pos][0];
	Object[objectid][object_pos][1] = pInfo[playerid][player_edited_object_pos][1];
	Object[objectid][object_pos][2] = pInfo[playerid][player_edited_object_pos][2];
	Object[objectid][object_pos][3] = pInfo[playerid][player_edited_object_pos][3];
	Object[objectid][object_pos][4] = pInfo[playerid][player_edited_object_pos][4];
	Object[objectid][object_pos][5] = pInfo[playerid][player_edited_object_pos][5];

	switch(pInfo[playerid][player_editor])
	{
		case EDITOR_TYPE_SAMP:
		{
			EditDynamicObject(playerid, objectid);
			
			Alert(playerid, ALERT_TYPE_INFO, "Mozesz przelaczyc sie na nasz edytor obiektow wybierajac opcje ~y~Tryb budowy ~w~w ~g~/stats~w~");
		}

		case EDITOR_TYPE_CUSTOM:
		{
			ApplyAnimation(playerid, "CRACK", "crckidle1", 4.1, 1, 0, 0, 0, 0, 1);
			pInfo[playerid][player_has_animation] = true;
		}

		default: SendClientMessage(playerid, COLOR_LIGHTER_RED, "> Wystpi bd podczas wybierania edytora.");
	}

	PlayerLog(sprintf("Started object editing {OBJECT_UID:%d}", Object[objectid][object_uid]), pInfo[playerid][player_id], "object");

	UpdateObjectInfoTextdraw(playerid, objectid);
	PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);
    return 1;
}

public OnPlayerEditDynamicObject(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
	if( !IsValidDynamicObject(objectid) ) return 1;
	
	if( pInfo[playerid][player_edited_object_no_action] )
	{
		pInfo[playerid][player_edited_object_no_action] = false;
		return 1;
	}
	
	if( objectid == pInfo[playerid][player_kogut_object] )
	{
		if( response == EDIT_RESPONSE_FINAL )
		{
			if( GetVehicleModel(pInfo[playerid][player_kogut_vehicle]) > 0 )
			{
				new vid = pInfo[playerid][player_kogut_vehicle];
				
				new Float:ofx, Float:ofy, Float:ofz, Float:ofaz;
				new Float:finalx, Float:finaly;
				new Float:px, Float:py, Float:pz, Float:roz;
				GetVehiclePos(vid, px, py, pz);
				GetVehicleZAngle(vid, roz);
				ofx = x-px;
				ofy = y-py;
				ofz = z-pz;
				ofaz = rz-roz;
				finalx = ofx*floatcos(roz, degrees)+ofy*floatsin(roz, degrees);
				finaly = -ofx*floatsin(roz, degrees)+ofy*floatcos(roz, degrees);
				
				mysql_pquery(g_sql, sprintf("INSERT INTO `crp_items_proto` (uid, value1, value2, x, y, z, rx, ry, rz) VALUES (null, %d, %d, %f, %f, %f, %f, %f, %f)", Group[pGroup[playerid][GetPlayerDutySlot(playerid)][pg_id]][group_uid], GetVehicleModel(vid), finalx, finaly, ofz, rx, ry, ofaz));
				
				SendGuiInformation(playerid, "Informacja", sprintf("Pozycja koguta dla pojazdu o modelu %d zostaa pomylnie ustalona.", GetVehicleModel(vid)));
			}
			
			DestroyDynamicObject(pInfo[playerid][player_kogut_object]);
			
			pInfo[playerid][player_kogut_object] = -1;
			pInfo[playerid][player_kogut_vehicle] = -1;
		}
		
		if( response == EDIT_RESPONSE_CANCEL )
		{
			DestroyDynamicObject(objectid);
			
			pInfo[playerid][player_kogut_object] = -1;
			pInfo[playerid][player_kogut_vehicle] = -1;
		}
		return 1;
	}
	
	// Items proto creation
	if( pInfo[playerid][player_items_proto_create] && Item[pInfo[playerid][player_items_proto_create_id]][item_object] == objectid  )
	{
		if( response == EDIT_RESPONSE_FINAL )
		{
			new 
				Float:pPos[6],
				itemid = pInfo[playerid][player_items_proto_create_id];
			GetPlayerPos(playerid, pPos[0], pPos[1], pPos[2]);
			
			pPos[2] = z - pPos[2];
			pPos[3] = rx;
			pPos[4] = ry;
			pPos[5] = rz;
			
			Item[itemid][item_z] = z;
			Item[itemid][item_rx] = pPos[3];
			Item[itemid][item_ry] = pPos[4];
			Item[itemid][item_rz] = pPos[5];
			
			new str[400];
			strcat(str, sprintf("UPDATE `crp_items` SET `item_ownertype` = %d, `item_owner` = 0, `item_posx` = %f, `item_posy` = %f, `item_posz` = %f,", Item[itemid][item_owner_type], Item[itemid][item_owner], Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z]));
			strcat(str, sprintf(" `item_rotx` = %f, `item_roty` = %f, `item_rotz` = %f, `item_world` = %d, `item_interior` = %d  WHERE `item_uid` = %d", Item[itemid][item_rx], Item[itemid][item_ry], Item[itemid][item_rz], Item[itemid][item_world], Item[itemid][item_interior], Item[itemid][item_uid]));
			mysql_pquery(g_sql, str);
			
			mysql_pquery(g_sql, sprintf("INSERT INTO `crp_items_proto` (`uid`,`model`,`z`,`rx`,`ry`,`rz`) VALUES (null, %d, %f, %f, %f, %f)", Item[itemid][item_model], pPos[2], pPos[3], pPos[4], pPos[5]));
			
			pInfo[playerid][player_items_proto_create_id] = -1;
			pInfo[playerid][player_items_proto_create] = false;
			
			SendGuiInformation(playerid, "Informacja", sprintf("Pomylnie dodano wzr przedmiotu o modelu %d.", Item[itemid][item_model]));
		}
		return 1;
	}
	
	if( objectid == pInfo[playerid][player_esel_edited_object] && pInfo[playerid][player_esel_edited_label] > 0 )
	{
		if( response == EDIT_RESPONSE_FINAL )
		{
			mysql_query(g_sql, sprintf("UPDATE `crp_3dlabels` SET `label_posx` = %f, `label_posy` = %f, `label_posz` = %f WHERE `label_uid` = %d", x, y, z, pInfo[playerid][player_esel_edited_label]), false);
			
			LoadLabel(sprintf("WHERE `label_uid` = %d", pInfo[playerid][player_esel_edited_label]), true);

			Streamer_UpdateEx(playerid, x, y, z);
			
			SendPlayerInformation(playerid, "Etykieta zapisana pomyslnie", 3000);

		}
		
		if( response == EDIT_RESPONSE_CANCEL )
		{
			SendPlayerInformation(playerid, "Edycja etykiety anulowana.", 3000);
			
			LoadLabel(sprintf("WHERE `label_uid` = %d", pInfo[playerid][player_esel_edited_label]));
		}
		
		if( response == EDIT_RESPONSE_CANCEL || response == EDIT_RESPONSE_FINAL )
		{
			DestroyDynamicObject(objectid);
			
			pInfo[playerid][player_esel_edited_label] = 0;
			pInfo[playerid][player_esel_edited_object] = -1;
			
			SendPlayerInformation(playerid, "", 0);
			
			PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
		}
		
		return 1;
	}
	
	if( response == EDIT_RESPONSE_FINAL || response == EDIT_RESPONSE_CANCEL )
	{
		Object[pInfo[playerid][player_edited_object]][object_is_edited] = false;
		pInfo[playerid][player_edited_object] = -1;
		
		PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
	}
	
	if( response == EDIT_RESPONSE_CANCEL )
	{
		SetDynamicObjectPos(objectid, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
		SetDynamicObjectRot(objectid, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);
		
		new str[400];
		strcat(str, sprintf("UPDATE `crp_objects` SET `object_posx` = %f, `object_posy` = %f, `object_posz` = %f,", pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]));
		strcat(str, sprintf(" `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5], Object[objectid][object_uid]));
		mysql_pquery(g_sql, str);
		
		SendPlayerInformation(playerid, "Edycja obiektu anulowana", 3000);
	}
	
	if( response == EDIT_RESPONSE_FINAL )
	{
		if( Object[objectid][object_owner_type] == OBJECT_OWNER_TYPE_AREA )
		{
			if( !IsPointInDynamicArea(GetAreaByUid(Object[objectid][object_owner]), x, y, z) )
			{
				pInfo[playerid][player_edited_object] = -1;
				Object[objectid][object_is_edited] = false;
				
				SetDynamicObjectPos(objectid, pInfo[playerid][player_edited_object_pos][0], pInfo[playerid][player_edited_object_pos][1], pInfo[playerid][player_edited_object_pos][2]);
				SetDynamicObjectRot(objectid, pInfo[playerid][player_edited_object_pos][3], pInfo[playerid][player_edited_object_pos][4], pInfo[playerid][player_edited_object_pos][5]);
				
				SendGuiInformation(playerid, "Wystpi bd", "Obiekt strefowy nie moe sta poza stref. Powrci on na swoje pocztkowe miejsce.");
				
				return 1;
			}
		}
	
		SetDynamicObjectPos(objectid, x, y, z);
		SetDynamicObjectRot(objectid, rx, ry, rz);

		mysql_query(g_sql, sprintf("UPDATE `crp_objects` SET `object_posx` = %f, `object_posy` = %f, `object_posz` = %f, `object_rotx` = %f, `object_roty` = %f, `object_rotz` = %f WHERE `object_uid` = %d", x, y, z, rx, ry, rz, Object[objectid][object_uid]), false);
		
		new uid = Object[objectid][object_uid];
		DeleteObject(objectid, false);
		
		LoadObject(sprintf("WHERE `object_uid` = %d", uid), true);
		
		Streamer_UpdateEx(playerid,  x, y, z);
		
		SendPlayerInformation(playerid, "Obiekt zapisany pomyslnie", 3000);
	}
	else if( response == EDIT_RESPONSE_UPDATE )
	{
		Object[objectid][object_pos][0] = x;
		Object[objectid][object_pos][1] = y;
		Object[objectid][object_pos][2] = z;
		Object[objectid][object_pos][3] = rx;
		Object[objectid][object_pos][4] = ry;
		Object[objectid][object_pos][5] = rz;
		
		UpdateObjectInfoTextdraw(playerid, objectid);
	}
	return 1;
}

public OnPlayerText(playerid, text[])
{
	// Animacje
	if( text[0] == '.' && text[1] != ' ' )
	{
		new alias[60];
		mysql_escape_string(text, alias);
		ApplyCommandAnim(playerid, sprintf("WHERE anim_command = '%s'", alias));
		
		return 0;
	}
	
	if( text[0] == '@' )
	{
		new textt[130];
		format(textt, sizeof(textt), "%s", text);
		if( textt[1] == '@' )
		{
			// Podgrupy
			if( textt[2] != ' ' && textt[3] == ' ' )
			{
				new slot;
				sscanf(textt[2], "d", slot);
				if( slot >= 1 && slot <= 5 )
				{
					strdel(textt, 0, 4);
					SendGroupOOC(playerid, slot, textt, true);
				}
			}
			else if( textt[2] == ' ' && pInfo[playerid][player_last_group_slot_chat] > -1 )
			{
				strdel(textt, 0, 3);
				SendGroupOOC(playerid, pInfo[playerid][player_last_group_slot_chat], textt, true);
			}
		}
		else
		{
			if( textt[1] != ' ' && textt[2] == ' ' )
			{
				// Grupy
				new slot;
				sscanf(textt[1], "d", slot);
				if( slot >= 1 && slot <= 5 )
				{
					strdel(textt, 0, 3);
					SendGroupOOC(playerid, slot, textt);
				}
			}
			else if( textt[1] == ' ' && pInfo[playerid][player_last_group_slot_chat] > -1 )
			{
				strdel(textt, 0, 2);
				SendGroupOOC(playerid, pInfo[playerid][player_last_group_slot_chat], textt);
			}
		}
		
		return 0;
	}
	
	if( text[0] == '!' )
	{
		new textt[130];
		format(textt, sizeof(textt), "%s", text);

		if( textt[1] != ' ' && textt[2] == ' ' )
		{
			// Grupy
			new slot;
			sscanf(textt[1], "d", slot);
			if( slot >= 1 && slot <= 5 )
			{
				strdel(textt, 0, 3);
				SendGroupIC(playerid, slot, textt);
			}
		}
		else if( textt[1] == ' ' && pInfo[playerid][player_last_group_slot_chat] > -1 )
		{
			strdel(textt, 0, 2);
			SendGroupIC(playerid, pInfo[playerid][player_last_group_slot_chat], textt);
		}
		
		return 0;
	}

	if( pInfo[playerid][player_bw] > 0 )
	{
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Jeste nieprzytomny. Moesz uywa komend /me i /do, aby odegra swj stan nieprzytomnoci.\n\nDodatkowo masz rwnie moliwo wysania prywatnej wiadomoci (/w), ale tylko do administratora bdcego na subie.", "OK", "");
		return 0;
	}
	
	if( !strcmp(text, ":D", true) )
	{
		ApplyAnimation(playerid, "RAPPING", "Laugh_01", 4.0, 0, 0, 0, 0, 0, 1);
		return ProxMessage(playerid, "mieje si.", PROX_AME);
	}
	
	if( !strcmp(text, "xd", true) )
	{
		ApplyAnimation(playerid, "RAPPING", "Laugh_01", 4.0, 0, 0, 0, 0, 0, 1);
		return ProxMessage(playerid, "wybucha miechem.", PROX_AME);
	}
	
	if( !strcmp(text, ":)", true) )		return ProxMessage(playerid, "umiecha si.", PROX_AME);
	if( !strcmp(text, ":(", true) ) 	return ProxMessage(playerid, "robi smutn min.", PROX_AME);
	if( !strcmp(text, ":/", true) ) 	return ProxMessage(playerid, "krzywi si.", PROX_AME);
	if( !strcmp(text, ":\\", true) ) 	return ProxMessage(playerid, "krzywi si.", PROX_AME);
	if( !strcmp(text, ":P", true) ) 	return ProxMessage(playerid, "wystawia jzyk.", PROX_AME);
	if( !strcmp(text, ":O", true) ) 	return ProxMessage(playerid, "robi zdziwion min.", PROX_AME);
	
	if( pInfo[playerid][player_phone_call_started] )
	{
		ProxMessage(playerid, text, PROX_PHONE);	
		return 0;
	}
	
	if( pInfo[playerid][player_lsn_live] )
	{
		if( Setting[setting_lsn_ad_finish_time] > 0 ) Setting[setting_lsn_ad_finish_time] = 0;
		replacePolishChars(text);
		TextDrawSetString(LSNtd, sprintf("				    		    ~>~ ~p~~h~Na zywo - %s: ~w~%s", pInfo[playerid][player_name], replaceColorCodes(text)));
	}
	else if( pInfo[playerid][player_lsn_wywiad_starter] != INVALID_PLAYER_ID || pInfo[playerid][player_lsn_wywiad_with] != INVALID_PLAYER_ID )
	{
		if( Setting[setting_lsn_ad_finish_time] > 0 ) Setting[setting_lsn_ad_finish_time] = 0;
		replacePolishChars(text);
		
		if( pInfo[playerid][player_lsn_wywiad_starter] == INVALID_PLAYER_ID ) TextDrawSetString(LSNtd, sprintf("				    		    ~>~ ~r~Wywiad z %s: %s: ~w~%s", pInfo[pInfo[playerid][player_lsn_wywiad_with]][player_name], pInfo[playerid][player_name], replaceColorCodes(text)));
		else TextDrawSetString(LSNtd, sprintf("				    		    ~>~ ~r~Wywiad z %s: ~w~%s", pInfo[playerid][player_name], replaceColorCodes(text)));
	}
	else
	{
		// Local message
		ProxMessage(playerid, text, PROX_LOCAL);
	}
	return 0;
}

stock OnPlayerPressMoveKey(playerid, key)
{
	new keys, updown, leftright;
	GetPlayerKeys(playerid, keys, updown, leftright);
	if( pInfo[playerid][player_is_selecting_bus] && !pInfo[playerid][player_selected_bus] )
	{
		if( key == 1 )
		{
			pInfo[playerid][player_bus_camera][0] += 25.0;
			SetPlayerCameraLookAt(playerid, pInfo[playerid][player_bus_camera][0], pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]);
		}
		else if( key == 2 )
		{
			pInfo[playerid][player_bus_camera][0] -= 25.0;
			SetPlayerCameraLookAt(playerid, pInfo[playerid][player_bus_camera][0], pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]);
		}
		else if( key == 3 )
		{
			pInfo[playerid][player_bus_camera][1] += 25.0;
			SetPlayerCameraLookAt(playerid, pInfo[playerid][player_bus_camera][0], pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]);
		}
		else if( key == 4 )
		{
			pInfo[playerid][player_bus_camera][1] -= 25.0;
			SetPlayerCameraLookAt(playerid, pInfo[playerid][player_bus_camera][0], pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]);
		}
		SetPlayerCameraPos(playerid, pInfo[playerid][player_bus_camera][0]-0.5, pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]+80.0);
		return;
	}

	else if( pInfo[playerid][player_edited_object] != -1 && pInfo[playerid][player_has_animation] )
	{
		new objectid = pInfo[playerid][player_edited_object];
		new Float:power = 0.1;
		new bool:move = true;

		if( pInfo[playerid][player_holding_space] ) power = 1.0;
		if( pInfo[playerid][player_holding_alt] ) power = 0.01;
		if( pInfo[playerid][player_holding_shift] ) move = false;

		new
			Float:x,
			Float:y,
			Float:z,
			Float:rx,
			Float:ry,
			Float:rz;

		Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_X, x);
        Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_Y, y);
        Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_Z, z);
        Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_R_X, rx);
		Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_R_Y, ry);
		Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_R_Z, rz);

		if( key == 1 )
		{
			if(move)
			{
				SetDynamicObjectPos(objectid, x+power, y, z);
				Object[objectid][object_pos][0] = x+power; 
			}
			else
			{
				SetDynamicObjectPos(objectid, x, y, z+power);
				Object[objectid][object_pos][2] = z+power; 
			}
		}
		else if( key == 2 )
		{
			if(move)
			{
				SetDynamicObjectPos(objectid, x-power, y, z);
				Object[objectid][object_pos][0] = x-power; 
			}
			else
			{
				SetDynamicObjectPos(objectid, x, y, z-power);
				Object[objectid][object_pos][2] = z-power; 
			}
		}
		else if( key == 3 )
		{
			if(move)
			{
				SetDynamicObjectPos(objectid, x, y+power, z);
				Object[objectid][object_pos][1] = y+power;
			}
			else
			{
				if(power == 0.1) power = 1.0;
				else if(power == 1.0) power = 45.0;
				else power = 0.1;
				SetDynamicObjectRot(objectid, rx, ry, rz-power);
				Object[objectid][object_pos][5] = rz-power; 
			}
		}
		else if( key == 4 )
		{
			if(move)
			{
				SetDynamicObjectPos(objectid, x, y-power, z);
				Object[objectid][object_pos][1] = y-power;
			}
			else
			{	
				if(power == 0.1) power = 1.0;
				else if(power == 1.0) power = 45.0;
				else power = 0.1;
				SetDynamicObjectRot(objectid, rx, ry, rz+power);
				Object[objectid][object_pos][5] = rz+power; 
			}
		}

		UpdateObjectInfoTextdraw(playerid, objectid);
		PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);
	}
}

public OnVehicleSpawn(vehicleid)
{
	return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	Gym_OnPlayerKey(playerid, newkeys, oldkeys);

	if( PRESSED(KEY_YES) )
	{
		InteractionRequest(playerid);
	}

	if( pOffer[playerid][offer_type] > 0 && pInfo[playerid][player_interface])
	{
		if( PRESSED(KEY_YES) ) OnPlayerOfferResponse(playerid, 1);
		if( PRESSED(KEY_NO) ) OnPlayerOfferResponse(playerid, 0);
	}

	if( pInfo[playerid][player_edit_meters] )
	{
		if( PRESSED(KEY_FIRE) )
		{
			new Float:nx, Float:ny, Float:nz;
			GetPlayerPos(playerid, nx, ny, nz);

			new d_uid = pInfo[playerid][player_edit_meters_vw];
			new d_id = GetDoorByUid(d_uid);

			new diffx, diffy, diffz;

			if(floatabs(nx) > floatabs(pInfo[playerid][player_meters_pos][0])) diffx = floatround(floatabs(nx)-floatabs(pInfo[playerid][player_meters_pos][0]));  
			else diffx = floatround(floatabs(pInfo[playerid][player_meters_pos][0])-floatabs(nx));

			if(floatabs(ny) > floatabs(pInfo[playerid][player_meters_pos][1])) diffy = floatround(floatabs(ny)-floatabs(pInfo[playerid][player_meters_pos][1]));
			else diffy = floatround(floatabs(pInfo[playerid][player_meters_pos][1])-floatabs(ny));

			if(floatabs(nz) > floatabs(pInfo[playerid][player_meters_pos][2])) diffz = floatround(floatabs(nz)-floatabs(pInfo[playerid][player_meters_pos][2]));
			else diffz = floatround(floatabs(pInfo[playerid][player_meters_pos][2])-floatabs(nz));

			if(diffx == 0) diffx = 1;
			if(diffy == 0) diffy = 1;
			if(diffz == 0) diffz = 1;

			if(diffx*diffy*diffz > Door[d_id][door_meters])
			{
				pInfo[playerid][player_edit_meters] = false;
				pInfo[playerid][player_edit_meters_vw] = 0;
				return SendGuiInformation(playerid, "Wystpi bd", "Zaznaczye za duy teren!");
			}

			Door[d_id][door_meters_points][0] = pInfo[playerid][player_meters_pos][0];
			Door[d_id][door_meters_points][1] = pInfo[playerid][player_meters_pos][1];
			Door[d_id][door_meters_points][2] = pInfo[playerid][player_meters_pos][2];
			Door[d_id][door_meters_points][3] = nx;
			Door[d_id][door_meters_points][4] = ny;
			Door[d_id][door_meters_points][5] = nz;

			if(Door[d_id][door_meters_points][2] > nz) nz -= 0.25;
			else Door[d_id][door_meters_points][2] -= 0.25;

			mysql_query(g_sql, sprintf("UPDATE crp_doors SET door_meters_x = %f, door_meters_y = %f, door_meters_z = %f, door_meters_nx = %f, door_meters_ny = %f, door_meters_nz = %f WHERE door_uid = %d", Door[d_id][door_meters_points][0], Door[d_id][door_meters_points][1], Door[d_id][door_meters_points][2], Door[d_id][door_meters_points][3], Door[d_id][door_meters_points][4], Door[d_id][door_meters_points][5], Door[d_id][door_uid]));
			
			DeleteDoor(d_id, false);
			LoadDoor(sprintf("WHERE `door_uid` = %d", d_uid));

			pInfo[playerid][player_edit_meters] = false;
			pInfo[playerid][player_edit_meters_vw] = 0;

			SendGuiInformation(playerid, "Sukces", sprintf("Pomylnie wyznaczye metra budynku - %dm2", diffx*diffy*diffz));
		}
	}

	if( pInfo[playerid][player_is_cuffed] )
    {
       if( PRESSED(KEY_JUMP) ) ApplyAnimation(playerid, "GYMNASIUM", "gym_jog_falloff", 4.1, 0, 1, 1, 0, 0);
       if( PRESSED(KEY_FIRE) ) StopPlayerAnimation(playerid);
       if( PRESSED(KEY_HANDBRAKE | KEY_SECONDARY_ATTACK) ) StopPlayerAnimation(playerid);
    }

    if( HOLDING(KEY_WALK) ) pInfo[playerid][player_holding_alt] = true;
    else pInfo[playerid][player_holding_alt] = false;

    if( HOLDING(KEY_SPRINT) ) pInfo[playerid][player_holding_space] = true;
    else pInfo[playerid][player_holding_space] = false;

	if( HOLDING(KEY_FIRE) ) pInfo[playerid][player_holding_fire] = true;
	else pInfo[playerid][player_holding_fire] = false;

	if( HOLDING(KEY_JUMP) ) pInfo[playerid][player_holding_shift] = true;
	else pInfo[playerid][player_holding_shift] = false;

	if( pInfo[playerid][player_edited_object] != -1 )
	{
		if(newkeys == KEY_FIRE)
		{
			if(!pInfo[playerid][player_has_animation]) ApplyAnimation(playerid, "CRACK", "crckidle1", 4.1, 1, 0, 0, 0, 0, 1);
			else ClearAnimations(playerid);

			pInfo[playerid][player_has_animation] = !pInfo[playerid][player_has_animation];
		}
	}

	// Odpalanie/gaszenie silnika i wiate
	if( IsPlayerInAnyVehicle(playerid) )
	{
		new vid = GetPlayerVehicleID(playerid);
		
		// Sprawdzamy czy jest kierowc
		if( !CanPlayerUseVehicle(playerid, vid) ) return 1;
		if( GetPlayerVehicleSeat(playerid) != 0 ) return 1;
		
		if( PRESSED(KEY_FIRE | KEY_ACTION)  )
		{
			// Sprawdzamy czy silnik czasem nie jest ju odpalany
			if( GetVehicleType(vid) == VEHICLE_TYPE_BIKE ) return 1;
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

				if( Vehicle[vid][vehicle_state] > 0 ) return SendGuiInformation(playerid, "Wystpi bd", "Na tym pojedzie przeprowadzana jest aktualnie jaka akcja. Aby go odpali poczekaj do jej ukoczenia.");
				if( Vehicle[vid][vehicle_block] ) return 1;
				
				// Ale na poczatku zobaczymy czy w ogole autko ma papu
				if( Vehicle[vid][vehicle_fuel_type] == 0 ) return 1;
				if( Vehicle[vid][vehicle_fuel_current] == 0.0 ) return SendGuiInformation(playerid, "Informacja", "W baku tego pojazdu nie ma paliwa. W tej sytuacji moesz zadzwoni po pomoc drogow lub kupi kanister.");
				
				Vehicle[vid][vehicle_engine_starting] = true;
				
				defer VehicleEngineStart[2000](playerid, vid);
				
				TextDrawShowForPlayer(playerid, vehicleEngineStarting);
			}
			
			return 1;
		}
		else if( PRESSED(KEY_FIRE) )
		{
			if( GetVehicleType(vid) == VEHICLE_TYPE_BIKE ) return 1;
			if( Vehicle[vid][vehicle_lights] )
			{
				// Swiatla sie swieca, wiec je gasimy
				Vehicle[vid][vehicle_lights] = false;
				
				UpdateVehicleVisuals(vid);
			}
			else
			{
				// Swiatla sie nie swieca, wiec ja zaswiecamy
				Vehicle[vid][vehicle_lights] = true;
				
				UpdateVehicleVisuals(vid);
			}
			
			return 1;
		}
	}
	else
	{
		if( pInfo[playerid][player_admin_spec] )
		{
			if( PRESSED(KEY_FIRE) )
			{
				new targetid = GetPlayerPrevSpectateId(playerid);
				
				if( targetid != INVALID_PLAYER_ID ) PlayerSetSpectate(playerid, targetid);
			}
			
			if( PRESSED(KEY_SPRINT) )
			{
				new targetid = GetPlayerNextSpectateId(playerid);
				
				if( targetid != INVALID_PLAYER_ID ) PlayerSetSpectate(playerid, targetid);
			}
		}
		
		if( pInfo[playerid][player_is_selecting_bus] && !pInfo[playerid][player_selected_bus] )
		{
			if( PRESSED(KEY_JUMP) )
			{
				scrp_SpawnPlayer(playerid, false);
				TogglePlayerSpectating(playerid, 0);
				
				pInfo[playerid][player_is_selecting_bus] = false;
				
				PlayerTextDrawHide(playerid, pInfo[playerid][BusInfo]);
			}
			
			if( PRESSED(KEY_SECONDARY_ATTACK) )
			{
				new bid = FindNearestBus(pInfo[playerid][player_start_bus_id], pInfo[playerid][player_bus_camera][0]-0.5, pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]);
				if( bid == -1 ) return 1;
				
				pInfo[playerid][player_selected_bus] = true;
				pInfo[playerid][player_selected_bus_id] = bid;
				
				new Float:pos[3];
				GetPointInAngleOfObject(Bus[bid][bus_objectid], pos[0], pos[1], pos[2], 10.0, 0.0);
				
				InterpolateCameraPos(playerid, pInfo[playerid][player_bus_camera][0]-0.5, pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]+80.0, pos[0], pos[1], pos[2]+1.5, 2000, CAMERA_MOVE);
				
				GetBusPos(bid, pos[0], pos[1], pos[2]);
				InterpolateCameraLookAt(playerid, pInfo[playerid][player_bus_camera][0], pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2], pos[0], pos[1], pos[2], 2000, CAMERA_MOVE);
				
				GetBusPos(pInfo[playerid][player_start_bus_id], pos[0], pos[1], pos[2]);
				
				new Float:distance;
				Streamer_GetDistanceToItem(pos[0], pos[1], pos[2], STREAMER_TYPE_OBJECT, Bus[pInfo[playerid][player_selected_bus_id]][bus_objectid], distance, 2);
				
				new price = floatround(distance*Bus[pInfo[playerid][player_start_bus_id]][bus_ratio]*0.04);
				
				new busname[60];
				strcopy(busname, Bus[bid][bus_name], 60);
				replacePolishChars(busname);
				
				PlayerTextDrawSetString(playerid, pInfo[playerid][BusInfo], sprintf("Przystanek:    ~y~%s~n~~w~Cena podrozy: ~g~$%d~n~~n~~w~Wcisnij ~y~~k~~VEHICLE_ENTER_EXIT~ ~w~aby zaakceptowac podroz lub ~y~~k~~PED_JUMPING~ ~w~aby powrocic do wyboru przystanku.", busname, price));
			}
			
			return 1;
		}

		
		if( pInfo[playerid][player_selected_bus] )
		{
			if( PRESSED(KEY_JUMP) )
			{			
				new Float:pos[3];
				GetPlayerCameraPos(playerid, pos[0], pos[1], pos[2]);
				
				InterpolateCameraPos(playerid, pos[0], pos[1], pos[2], pInfo[playerid][player_bus_camera][0]-0.5, pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2]+80.0, 2000, CAMERA_MOVE);
				
				GetBusPos(pInfo[playerid][player_selected_bus_id], pos[0], pos[1], pos[2]);
				InterpolateCameraLookAt(playerid, pos[0], pos[1], pos[2], pInfo[playerid][player_bus_camera][0], pInfo[playerid][player_bus_camera][1], pInfo[playerid][player_bus_camera][2], 2000, CAMERA_MOVE);

				pInfo[playerid][player_selected_bus] = false;
				pInfo[playerid][player_selected_bus_id] = -1;
				
				PlayerTextDrawSetString(playerid, pInfo[playerid][BusInfo], "Za pomoca strzalek zmieniaj pozycje kamery, aby odnalezc miejsce podrozy.~n~~n~Wcisnij ~y~~k~~VEHICLE_ENTER_EXIT~ ~w~aby zobaczyc najblizszy przystanek lub ~y~~k~~PED_JUMPING~ ~w~aby anulowac.");
			}
			
			if( PRESSED(KEY_SECONDARY_ATTACK) )
			{
				scrp_SpawnPlayer(playerid, false);
				TogglePlayerSpectating(playerid, 0);
				
				pInfo[playerid][player_is_selecting_bus] = false;
				pInfo[playerid][player_selected_bus] = false;	

				PlayerTextDrawHide(playerid, pInfo[playerid][BusInfo]);
				GameTextForPlayer(playerid, "~r~twoj autobus przyjedzie~n~za okolo 30 sekund", 6000, 3);
				
				pInfo[playerid][player_is_waiting_for_bus] = true;
				pInfo[playerid][player_bus_waiting_start] = gettime();
			}
			
			return 1;
		}
		

		if( pInfo[playerid][player_is_in_binco] )
		{
			if( PRESSED(KEY_FIRE) )
			{
				new skinid = Iter_Prev(Skins[pInfo[playerid][player_sex]], pInfo[playerid][player_binco_skinid]);
				if( skinid < 0 || skinid > 299 ) return 1;
				
				pInfo[playerid][player_binco_skinid] = skinid;
				SetPlayerSkin(playerid, Skin[skinid][skin_value]);
				
				PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], sprintf("~g~SKIN: %d   ~p~CENA: $%d~n~~n~~w~Uzyj ~y~LMB~w~ i ~y~RMB~w~ aby zmieniac dostepne skiny.~n~~y~~k~~VEHICLE_ENTER_EXIT~~w~ Aby kupic wybrany skin lub ~y~~k~~PED_JUMPING~ ~w~aby anulowac.", Skin[skinid][skin_value], Skin[skinid][skin_price]));
			}
			
			if( PRESSED(KEY_HANDBRAKE) )
			{
				new skinid = Iter_Next(Skins[pInfo[playerid][player_sex]], pInfo[playerid][player_binco_skinid]);
				if( skinid < 0 || skinid > 299 ) return 1;
				
				pInfo[playerid][player_binco_skinid] = skinid;
				SetPlayerSkin(playerid, Skin[skinid][skin_value]);
				
				PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], sprintf("~g~SKIN: %d   ~p~CENA: $%d~n~~n~~w~Uzyj ~y~LMB~w~ i ~y~RMB~w~ aby zmieniac dostepne skiny.~n~~y~~k~~VEHICLE_ENTER_EXIT~~w~ Aby kupic wybrany skin lub ~y~~k~~PED_JUMPING~ ~w~aby anulowac.", Skin[skinid][skin_value], Skin[skinid][skin_price]));
			}
			
			if( PRESSED(KEY_JUMP) )
			{
				pInfo[playerid][player_is_in_binco] = false;
				TogglePlayerControllable(playerid, 1);
				
				SetPlayerSkin(playerid, pInfo[playerid][player_last_skin]);
				SetCameraBehindPlayer(playerid);
				
				PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
			}
			
			if( PRESSED(KEY_SECONDARY_ATTACK) )
			{
				new skinid = pInfo[playerid][player_binco_skinid];
				if( pInfo[playerid][player_money] < Skin[skinid][skin_price] ) return GameTextForPlayer(playerid, "~r~Nie masz wystarczajacej~n~ilosci pieniedzy", 3000, 3);
				
				GivePlayerMoney(playerid, -Skin[skinid][skin_price]);
				
				new itemid = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_CLOTH, Skin[skinid][skin_value], 0, sprintf("Ubranie (%d)", 2386, Skin[skinid][skin_value]));
				
				ProxMessage(playerid, "odbiera ubranie od sprzedawcy.", PROX_AME);
				
				pInfo[playerid][player_is_in_binco] = false;
				TogglePlayerControllable(playerid, 1);
				
				SetPlayerSkin(playerid, pInfo[playerid][player_last_skin]);
				SetCameraBehindPlayer(playerid);
				
				PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);

				PlayerLog(sprintf("Bought clothes item %s {PRICE:%d}", Item[itemid][item_uid], Skin[skinid][skin_price]), pInfo[playerid][player_id], "object");
			}
			
			return 1;
		}

		
		if( pInfo[playerid][player_is_in_binco_access] )
		{
			if( PRESSED(KEY_FIRE) )
			{			
				mysql_tquery(g_sql, sprintf("SELECT access_model, access_price, access_uid, access_name, access_rotz, access_rotx, access_roty FROM crp_access WHERE access_price > 0 AND access_uid > %d ORDER BY access_uid LIMIT 1", pInfo[playerid][player_binco_access_uid]), "OnBincoGetNextLoaded", "i", playerid);
				
			}
			
			if( PRESSED(KEY_HANDBRAKE) )
			{
				mysql_tquery(g_sql, sprintf("SELECT access_model, access_price, access_uid, access_name, access_rotz, access_rotx, access_roty FROM crp_access WHERE access_price > 0 AND access_uid < %d ORDER BY access_uid DESC LIMIT 1", pInfo[playerid][player_binco_access_uid]), "OnBincoGetNextLoaded", "i", playerid);
			}
			
			if( PRESSED(KEY_JUMP) )
			{
				DestroyDynamicObject(pInfo[playerid][player_binco_access_object]);
				
				pInfo[playerid][player_is_in_binco_access] = false;
				TogglePlayerControllable(playerid, 1);
				
				SetCameraBehindPlayer(playerid);
				
				PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
			}
			
			if( PRESSED(KEY_SECONDARY_ATTACK) )
			{
				new Cache:result;
				result = mysql_query(g_sql, sprintf("SELECT access_model, access_price, access_uid, access_name, access_bone FROM crp_access WHERE access_uid = %d", pInfo[playerid][player_binco_access_uid]));
				
				new price = cache_get_int(0, "access_price"), model = cache_get_int(0, "access_model"), bone = cache_get_int(0, "access_bone");	
				
				new accessname[50];
				cache_get(0, "access_name", accessname);

				cache_delete(result);
				
				if( pInfo[playerid][player_money] < price ) GameTextForPlayer(playerid, "~r~Nie masz wystarczajacej~n~ilosci pieniedzy", 3000, 3);
				else
				{
					GivePlayerMoney(playerid, -price);
					Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_ACCESSORY, 0, bone, sprintf("[A] %s", accessname), model);
				
					ProxMessage(playerid, "odbiera dodatek od sprzedawcy.", PROX_AME);
					
					DestroyDynamicObject(pInfo[playerid][player_binco_access_object]);
					
					pInfo[playerid][player_is_in_binco_access] = false;
					TogglePlayerControllable(playerid, 1);
					
					SetCameraBehindPlayer(playerid);
					
					PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
				}
			}
			
			return 1;
		}

	
		if( PRESSED(KEY_SECONDARY_ATTACK) || PRESSED(KEY_HANDBRAKE) )
		{
			if( pInfo[playerid][player_looped_anim] ) 
			{
				pInfo[playerid][player_looped_anim] = false;
				StopPlayerAnimation(playerid);
			}
		}
		
		if( PRESSED( KEY_SPRINT | KEY_WALK ) )
		{
			new a_inner_id = GetPlayerArea(playerid, AREA_TYPE_DOOR_INNER), a_outer_id = GetPlayerArea(playerid, AREA_TYPE_DOOR_OUTER);
			
			new vw[7];
			format(vw, sizeof(vw), "20%04d", pInfo[playerid][player_id]);
			
			if( a_inner_id != -1 )
			{
				if( pInfo[playerid][player_is_cuffed] ) return SendGuiInformation(playerid, "Wystpi bd", "Nie moesz wyj z budynku gdy jeste skuty.");
				if( pInfo[playerid][player_keep] > gettime() ) return SendGuiInformation(playerid, "Wystpi bd", "Nie moesz wyj z tego budynku poniewa jeste w nim przetrzymywany.");
				
				new d_id = Area[a_inner_id][area_owner];
				
				if( Door[d_id][door_closed] ) return SendGuiInformation(playerid, "Wystpi bd", "Te drzwi s zamknite.");
				
				SetPlayerPos(playerid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]+0.2);
				SetPlayerFacingAngle(playerid, Door[d_id][door_pos][3]);
				
				SetCameraBehindPlayer(playerid);
				
				SetPlayerVirtualWorld(playerid, Door[d_id][door_vw]);
				SetPlayerInterior(playerid, Door[d_id][door_int]);

				PlayerLog(sprintf("Exits door %s", DoorLogLink(Door[d_id][door_uid])), pInfo[playerid][player_id], "door");

				new cuffsid = GetPlayerUsedItem(playerid, ITEM_TYPE_CUFFS);
				if( cuffsid != -1 )
				{
					new targetid = Item[cuffsid][item_value1];

					SetPlayerPos(targetid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]+0.2);
					SetPlayerFacingAngle(targetid, Door[d_id][door_pos][3]);
				
					SetCameraBehindPlayer(targetid);
				
					SetPlayerVirtualWorld(targetid, Door[d_id][door_vw]);
					SetPlayerInterior(targetid, Door[d_id][door_int]);
				}
			}
			else if( a_outer_id != -1 )
			{
				if( pInfo[playerid][player_is_cuffed] ) return SendGuiInformation(playerid, "Wystpi bd", "Nie moesz wej do budynku gdy jeste skuty.");
				if( pInfo[playerid][player_keep] > gettime() ) return SendGuiInformation(playerid, "Wystpi bd", "Nie moesz przej do tego budynku poniewa jeste przetrzymywany.");
				
				new d_id = Area[a_outer_id][area_owner];
				
				if( Door[d_id][door_closed] ) return SendGuiInformation(playerid, "Wystpi bd", "Te drzwi s zamknite.");
				
				if( Door[d_id][door_payment] > 0 )
				{
					if( Door[d_id][door_payment] > pInfo[playerid][player_money] ) return SendGuiInformation(playerid, "Wystpi bd", "Nie masz wystarczajcej iloci pienidzy, aby wej do budynku.");
					
					GivePlayerMoney(playerid, -Door[d_id][door_payment]);
				}

				PlayerLog(sprintf("Enters door %s {PAYMENT:%d}", DoorLogLink(Door[d_id][door_uid]), Door[d_id][door_payment]), pInfo[playerid][player_id], "door");
				
				SetPlayerPos(playerid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]+0.2);
				SetPlayerFacingAngle(playerid, Door[d_id][door_spawn_pos][3]);
				
				SetCameraBehindPlayer(playerid);
				
				SetPlayerVirtualWorld(playerid, Door[d_id][door_spawn_vw]);
				SetPlayerInterior(playerid, Door[d_id][door_spawn_int]);

				new cuffsid = GetPlayerUsedItem(playerid, ITEM_TYPE_CUFFS);
				if( cuffsid != -1 )
				{
					new targetid = Item[cuffsid][item_value1];

					SetPlayerPos(targetid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]+0.2);
					SetPlayerFacingAngle(targetid, Door[d_id][door_spawn_pos][3]);
					
					SetCameraBehindPlayer(targetid);
					
					SetPlayerVirtualWorld(targetid, Door[d_id][door_spawn_vw]);
					SetPlayerInterior(targetid, Door[d_id][door_spawn_int]);
				}
			}
			else if( GetPlayerVirtualWorld(playerid) == strval(vw) )
			{
				new did = GetDoorByUid(pInfo[playerid][player_door]);
				if( did > -1 )
				{
					if( Door[did][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
					{
						new g_id = GetGroupByUid(Door[did][door_owner]);
						if( Group[g_id][group_type] == GROUP_TYPE_HOTEL )
						{
							SetPlayerPos(playerid, Door[did][door_spawn_pos][0], Door[did][door_spawn_pos][1], Door[did][door_spawn_pos][2]);
							SetPlayerFacingAngle(playerid, Door[did][door_spawn_pos][3]);
							
							SetCameraBehindPlayer(playerid);
							
							SetPlayerVirtualWorld(playerid, Door[did][door_spawn_vw]);
							SetPlayerInterior(playerid, Door[did][door_spawn_int]);
						}
					}
				}
			}
		}

		
		if( pInfo[playerid][player_creating_area] )
		{
			if( PRESSED(KEY_HANDBRAKE) )
			{
				if( pInfo[playerid][player_carea_point1][0] == 0.0 && pInfo[playerid][player_carea_point1][1] == 0.0 && pInfo[playerid][player_carea_point1][2] == 0.0 )
				{
					GetPlayerPos(playerid, pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2]);
					
					pInfo[playerid][player_carea_label][0] = CreateDynamic3DTextLabel(sprintf("Punkt pierwszy\n(%f, %f, %f)", pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2]), COLOR_LIGHTER_RED, pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
					
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Stworzye pierwszy punkt strefy.", "OK", "");
				}
				else if( pInfo[playerid][player_carea_point2][0] == 0.0 && pInfo[playerid][player_carea_point2][1] == 0.0 && pInfo[playerid][player_carea_point2][2] == 0.0 )
				{
					GetPlayerPos(playerid, pInfo[playerid][player_carea_point2][0], pInfo[playerid][player_carea_point2][1], pInfo[playerid][player_carea_point2][2]);
					
					pInfo[playerid][player_carea_label][1] = CreateDynamic3DTextLabel(sprintf("Punkt drugi\n(%f, %f, %f)", pInfo[playerid][player_carea_point2][0], pInfo[playerid][player_carea_point2][1], pInfo[playerid][player_carea_point2][2]), COLOR_LIGHTER_RED, pInfo[playerid][player_carea_point2][0], pInfo[playerid][player_carea_point2][1], pInfo[playerid][player_carea_point2][2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
					
					pInfo[playerid][player_carea_zone] = GangZoneCreate(Min(pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point2][0]), Min(pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point2][1]), Max(pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point2][0]), Max(pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point2][1]));
					GangZoneShowForPlayer(playerid, pInfo[playerid][player_carea_zone], 0xFF3C3C80);
									
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Stworzye drugi punkt strefy.", "OK", "");
				}
				else
				{
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Zaznaczye ju dwa punkty strefy, aby usun ostatnio stworzony wcinij LSHIFT lub ENTER aby stworzy stref.", "OK", "");
				}
			}
			
			if( PRESSED(KEY_FIRE) )
			{
				if( pInfo[playerid][player_carea_point2][0] != 0.0 && pInfo[playerid][player_carea_point2][1] != 0.0 && pInfo[playerid][player_carea_point2][2] != 0.0 )
				{
					pInfo[playerid][player_carea_point2][0] = 0.0;
					pInfo[playerid][player_carea_point2][1] = 0.0;
					pInfo[playerid][player_carea_point2][2] = 0.0;
					
					if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
					
					GangZoneDestroy(pInfo[playerid][player_carea_zone]);
					
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Usune drugi punkt strefy.", "OK", "");
				}
				else if( pInfo[playerid][player_carea_point1][0] != 0.0 && pInfo[playerid][player_carea_point1][1] != 0.0 && pInfo[playerid][player_carea_point1][2] != 0.0 )
				{
					pInfo[playerid][player_carea_point1][0] = 0.0;
					pInfo[playerid][player_carea_point1][1] = 0.0;
					pInfo[playerid][player_carea_point1][2] = 0.0;
					
					if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
					
					ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Usune pierwszy punkt strefy.", "OK", "");
				}
			}
			
			if( PRESSED(KEY_WALK | KEY_SPRINT) )
			{
				pInfo[playerid][player_carea_point1][0] = 0.0;
				pInfo[playerid][player_carea_point1][1] = 0.0;
				pInfo[playerid][player_carea_point1][2] = 0.0;
				
				pInfo[playerid][player_carea_point2][0] = 0.0;
				pInfo[playerid][player_carea_point2][1] = 0.0;
				pInfo[playerid][player_carea_point2][2] = 0.0;
				
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
				if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
				
				GangZoneDestroy(pInfo[playerid][player_carea_zone]);
				
				pInfo[playerid][player_creating_area] = false;
				
				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Wyczye tryb tworzenia strefy.", "OK", "");
				SendPlayerInformation(playerid, "", 0);
			}
		}

	}
	return 1;
}

public OnPlayerCommandReceived(playerid, cmdtext[])
{
	if( !pInfo[playerid][player_logged] ) return 0;
	
	//PlayerLog(playerid, "[cmdl]", "[%s] %s", pInfo[playerid][player_name], cmdtext);

	pInfo[playerid][player_command_time] = GetTickCount();
	
	if( pInfo[playerid][player_bw] > 0 )
	{
		if( strfind(cmdtext, "/me", true) == -1 && strfind(cmdtext, "/id", true) == -1 && strfind(cmdtext, "/ac", true) == -1 && strcmp(cmdtext, "/admins") != 0 && strcmp(cmdtext, "/akceptujsmierc") != 0 && strcmp(cmdtext, "/a") != 0 && strfind(cmdtext, "/do", true) == -1 && strfind(cmdtext, "/w", true) == -1 && strfind(cmdtext, "/bw", true) == -1 && strfind(cmdtext, "/unbw", true) == -1 && strfind(cmdtext, "/aduty", true) == -1 && strfind(cmdtext, "/duty", true) == -1 )
		{
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Jeste nieprzytomny. Moesz uywa komend /me i /do, aby odegra swj stan nieprzytomnoci.\n\n Dodatkowo masz rwnie moliwo wysania prywatnej wiadomoci (/w), ale tylko do administratora bdcego na subie.", "OK", "");
			return 0;
		}
	}
	
	return 1;
}

public OnPlayerCommandPerformed(playerid, cmdtext[], success)
{
	if( !success ) 
	{
		Alert(playerid, ALERT_TYPE_WARNING, sprintf("Nie moesz uy komendy ~r~%s", cmdtext));
		return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	}

	PlayerLog(sprintf("Executed command: %s", cmdtext), pInfo[playerid][player_id], "cmd");

	return 1;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if( PlayerHasBlock(playerid, BLOCK_VEHICLES) && !ispassenger ) 
	{
		ClearAnimations(playerid, 1);
		SendGuiInformation(playerid, "Wystpi bd", "Masz aktywn blokad prowadzenia pojazdw.");
		return 1;
	}
	
	if( pInfo[playerid][player_hurted] ) 
	{
		ClearAnimations(playerid, 1);
		ApplyAnimation(playerid, "SWEET", "Sweet_injuredloop", 4.0, 0, 1, 1, 1, 0, 1);
		
		return 1;
	}
	
	if( Vehicle[vehicleid][vehicle_locked] )
	{
		ClearAnimations(playerid, 1);
		GameTextForPlayer(playerid, "~r~Pojazd zamkniety", 2500, 3);
		return 1;
	}
	

	if( Vehicle[vehicleid][vehicle_destroyed] && !ispassenger )
	{
		ClearAnimations(playerid, 1);
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Pojazd zniszczony", "Twj pojazd jest cakowicie zniszczony.\nMogo do tego doj wskutek wybuchu lub wpadnicia do wody.\n\nAby przywrci go do stanu uywalnoci, musisz zadzwoni po pomoc drogow\naby zaholowali pojazd do warsztatu, gdzie zajm si nim mechanicy.\n\nAby zaakceptowa ofert naprawy bdziesz musia siedzie w rodku jako pasaer.", "Zamknij", "");	
		return 1;
	}
	
	pInfo[playerid][player_entering_vehicle] = vehicleid;

	return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
	playerTeleportedByServer[playerid] = true;
	PlayerLog(sprintf("Exits vehicle %s {VSPEED:%.1f}", VehicleLogLink(Vehicle[vehicleid][vehicle_uid]), GetVehicleSpeed(vehicleid)), pInfo[playerid][player_id], "vehicle");

    return 1;
}

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
    // Check if was spectated
	foreach(new p : Spectators)
	{
		if( pInfo[p][player_admin_spec_id] == playerid && pInfo[p][player_admin_spec] )
		{
			SetPlayerInterior(p, newinteriorid);
		}
	}
    return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
	PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);
	DisablePlayerCheckpoint(playerid);
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	// Check if was spectated
	foreach(new p : Spectators)
	{
		if( pInfo[p][player_admin_spec_id] == playerid && pInfo[p][player_admin_spec] )
		{
			PlayerSetSpectate(p, playerid);
		}
	}
	
	if( (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER) && (oldstate != PLAYER_STATE_DRIVER && oldstate != PLAYER_STATE_PASSENGER) )
	{
		if( pInfo[playerid][player_entering_vehicle] != GetPlayerVehicleID(playerid) )
		{
			PlayerLog(sprintf("Unathorized entry to vehicle %s", VehicleLogLink(Vehicle[GetPlayerVehicleID(playerid)][vehicle_uid])), pInfo[playerid][player_id], "anticheat");

			new str[80];
			format(str, sizeof(str), "Unauthorized entry to vehicle (vid: %d)", GetPlayerVehicleID(playerid));
			AddPlayerPenalty(playerid, PENALTY_TYPE_KICK, INVALID_PLAYER_ID, 0, str);
			return 1;
		}
		else
		{
			pInfo[playerid][player_entering_vehicle] = -1;
			
			new vid = GetPlayerVehicleID(playerid);

			if( Vehicle[vid][vehicle_radio] )
			{
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
					mysql_pquery(g_sql, sprintf("SELECT audio_url FROM crp_audiourls WHERE audio_uid = %d", Item[cdid][item_value1]), "OnCdUrlLoaded", "i", playerid);	
				}
			}

			if( vid != INVALID_VEHICLE_ID )
			{
				pInfo[playerid][player_occupied_vehicle] = vid;
				pInfo[playerid][player_occupied_vehicle_seat] = GetPlayerVehicleSeat(playerid);
				Vehicle[vid][vehicle_occupants] += 1;
				// Wylaczamy namierzanie
				if( pInfo[playerid][player_vehicle_target] == vid )
				{
					Streamer_RemoveArrayData(STREAMER_TYPE_MAP_ICON, Vehicle[vid][vehicle_map_icon], E_STREAMER_PLAYER_ID, playerid);
					Streamer_UpdateEx(playerid, Vehicle[vid][vehicle_last_pos][0], Vehicle[vid][vehicle_last_pos][1], Vehicle[vid][vehicle_last_pos][2]);

					pInfo[playerid][player_vehicle_target] = -1;
					SendGuiInformation(playerid, "Informacja", "Namierzanie pojazdu zostao wyczone.");
				}
				
				PlayerLog(sprintf("Entered vehicle %s {SEAT:%d,FUEL:%d}", VehicleLogLink(Vehicle[vid][vehicle_uid]), GetPlayerVehicleSeat(playerid), floatround(Vehicle[vid][vehicle_fuel_current])), pInfo[playerid][player_id], "vehicle");

				if( newstate == PLAYER_STATE_DRIVER )
				{
					// Sprawdzamy czy silnik nie jest juz czasem odpalon
					if( !Vehicle[vid][vehicle_engine] && CanPlayerUseVehicle(playerid, vid) && GetVehicleType(vid) != VEHICLE_TYPE_BIKE ) TextDrawShowForPlayer(playerid, vehicleInfo);
					if( Vehicle[vid][vehicle_engine] && !CanPlayerUseVehicle(playerid, vid) && Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP ) 
					{
						Vehicle[vid][vehicle_engine] = false;
						SaveVehicle(vid);
		
						UpdateVehicleVisuals(vid);
					}
					
					// Ustawiamy kierowc
					Vehicle[vid][vehicle_driver] = playerid;
					
					if( GetVehicleType(vid) == VEHICLE_TYPE_BIKE )
					{
						Vehicle[vid][vehicle_engine] = true;		
						UpdateVehicleVisuals(vid);
					}
				}
			}
		}
	}
	
	if( oldstate == PLAYER_STATE_DRIVER && newstate != PLAYER_STATE_DRIVER )
	{
		TextDrawHideForPlayer(playerid, vehicleInfo);
		StopAudioStreamForPlayer(playerid);
	}

	if( newstate == PLAYER_STATE_DRIVER && oldstate != PLAYER_STATE_DRIVER )
	{
		new vehicleid = GetPlayerVehicleID(playerid);
		if( Vehicle[vehicleid][vehicle_block] )
		{
			new gid = GetGroupByUid(Vehicle[vehicleid][vehicle_block_group]);
			if( gid > -1 ) ShowPlayerDialog(playerid, DIALOG_VEHICLE_BLOCK, DIALOG_STYLE_MSGBOX, "Blokada pojazdu", sprintf("Na ten pojazd zostaa zaoona blokada przez pracownika grupy %s.\nCena blokady: $%d.", Group[gid][group_name], Vehicle[vehicleid][vehicle_block_price]), "Zapa", "Zamknij");
		}
	}

	if( oldstate == PLAYER_STATE_PASSENGER && newstate != PLAYER_STATE_PASSENGER )
	{
		StopAudioStreamForPlayer(playerid);
	}
	
	if( (oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER) && newstate != PLAYER_STATE_DRIVER && newstate != PLAYER_STATE_PASSENGER )
	{
		if( pInfo[playerid][player_occupied_vehicle] > -1 )
		{
			if( Vehicle[pInfo[playerid][player_occupied_vehicle]][vehicle_locked] && GetVehicleType(pInfo[playerid][player_occupied_vehicle]) != VEHICLE_TYPE_BIKE )
			{
				new Float:p_pos[3];
				GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
				SetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
				PutPlayerInVehicle(playerid, pInfo[playerid][player_occupied_vehicle], pInfo[playerid][player_occupied_vehicle_seat]);
				GameTextForPlayer(playerid, "~r~Pojazd zamkniety", 2500, 3);
				return 1;
			}
			
			Vehicle[pInfo[playerid][player_occupied_vehicle]][vehicle_occupants] -= 1;
			pInfo[playerid][player_occupied_vehicle] = -1;
			
			if( pInfo[playerid][player_belt] )
			{
				RemovePlayerStatus(playerid, PLAYER_STATUS_BELT);
				pInfo[playerid][player_belt] = false;
				
				SendPlayerInformation(playerid, "~r~Wyszedles z auta nie odpinajac pasow. Musisz poczekac 5 sekund.", 4000);
				
				FreezePlayer(playerid, 5000);
			}
		}
	}
	
	return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
	Vehicle[vehicleid][vehicle_destroyed] = true;
	
	new otype = Vehicle[vehicleid][vehicle_owner_type], uid = Vehicle[vehicleid][vehicle_uid];

	new driver = GetVehicleDriver(vehicleid);

	foreach(new p : Player)
	{
		if(IsPlayerInVehicle(p, vehicleid)) RemovePlayerFromVehicle(p);
	}

	DeleteVehicle(vehicleid);
	
	if( otype == VEHICLE_OWNER_TYPE_GROUP ) LoadVehicle(sprintf("WHERE vehicle_uid = %d", uid), true);
	if( driver != INVALID_PLAYER_ID ) SendGuiInformation(driver, "Informacja", "Pojazd wpad do wody i zosta cakowicie zniszczony. Zosta przwyrcony do jego pozycji spawnu.\nAby naprawi samochd skontaktuj si z warsztatem bd zaholuj do niego pojazd.");
	
    return 1;
}

stock OnVehicleHealthLoss(vehicleid, Float:hp)
{
	new Float:takedown = floatdiv(hp, 10);

	foreach(new p : Player)
	{
		if( GetPlayerVehicleID(p) == vehicleid )
		{
			if( pInfo[p][player_belt] )
			{
				// odpinamy pasy
				SetPlayerHealth(p, floatround(floatround(pInfo[p][player_health] - takedown)*0.5));
			}
			else SetPlayerHealth(p, floatround(pInfo[p][player_health] - takedown));

			pInfo[p][player_taken_damage] = gettime();
			UpdatePlayerLabel(p);
		}
	}

	if( Vehicle[vehicleid][vehicle_health] < 250 )
	{
		SetVehicleHealth(vehicleid, 250);
		Vehicle[vehicleid][vehicle_destroyed] = true;
		Vehicle[vehicleid][vehicle_engine] = false;
		UpdateVehicleVisuals(vehicleid);
		SaveVehicle(vehicleid);

		new driver = GetVehicleDriver(vehicleid);
		if(driver != INVALID_PLAYER_ID)
		{
			if( CanPlayerUseVehicle(driver, vehicleid) ) TextDrawShowForPlayer(driver, vehicleInfo);
		}

		// flip na wszelki wypadek
		new Float:angle;
		GetVehicleZAngle(vehicleid, angle);
		SetVehicleZAngle(vehicleid, angle);

		// zapisujemy pozycje i ustawiamy jako destroyed
		GetVehiclePos(vehicleid, Vehicle[vehicleid][vehicle_park][0], Vehicle[vehicleid][vehicle_park][1], Vehicle[vehicleid][vehicle_park][2]);
		GetVehicleZAngle(vehicleid, Vehicle[vehicleid][vehicle_park][3]);
		Vehicle[vehicleid][vehicle_park_world] = GetVehicleVirtualWorld(vehicleid);
		Vehicle[vehicleid][vehicle_park_interior] = Vehicle[vehicleid][vehicle_interior];
		
		mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_posx` = %f, `vehicle_posy` = %f, `vehicle_posz` = %f, `vehicle_posa` = %f, `vehicle_world` = %d, `vehicle_interior` = %d WHERE `vehicle_uid` = %d", Vehicle[vehicleid][vehicle_park][0], Vehicle[vehicleid][vehicle_park][1], Vehicle[vehicleid][vehicle_park][2], Vehicle[vehicleid][vehicle_park][3], Vehicle[vehicleid][vehicle_park_world], Vehicle[vehicleid][vehicle_park_interior], Vehicle[vehicleid][vehicle_uid]));

		if( driver != INVALID_PLAYER_ID ) SendGuiInformation(driver, "Informacja", "Pojazd zosta cakowicie zniszczony. Jego pozycja zostaa zamieniona na t w ktrej si znajduje.\nAby naprawi samochd skontaktuj si z warsztatem bd zaholuj do niego pojazd.");
	}
	
	return 1;
}

public OnVehicleDamageStatusUpdate(vehicleid, playerid)
{
    if( Vehicle[vehicleid][vehicle_health] >= 900 ) UpdateVehicleDamageStatus(vehicleid, Vehicle[vehicleid][vehicle_damage][0], Vehicle[vehicleid][vehicle_damage][1], Vehicle[vehicleid][vehicle_damage][2], Vehicle[vehicleid][vehicle_damage][3]);
    return 1;	
}

public OnPlayerRequestClass(playerid, classid)
{
    return 1;
}

public OnPlayerRequestSpawn(playerid)
{
    return 1;
}

public OnPlayerSpawn(playerid)
{
	pInfo[playerid][player_quit_time] = 0;
	
	SetPlayerTeam(playerid, 10);
	defer PreloadAllAnimLibs[2000](playerid);
	
	LoadAttachedObjects(playerid);
	
	// BW
	if( pInfo[playerid][player_bw] > 0 )
	{
		SetPlayerHealth(playerid, 1);
		
		SetPlayerVirtualWorld(playerid, pInfo[playerid][player_quit_vw]);
		SetPlayerInterior(playerid, pInfo[playerid][player_quit_int]);

		if(pInfo[playerid][player_bw_vehicle] != INVALID_VEHICLE_ID)
		{
			PutPlayerInVehicle(playerid, pInfo[playerid][player_bw_vehicle], pInfo[playerid][player_bw_vehicle_seat]);
			
			defer ApplyAnim[200](playerid, ANIM_TYPE_BW_INCAR);
		}
		else
		{
			defer ApplyAnim[200](playerid, ANIM_TYPE_BW);
		}

		
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);

		SetPlayerCameraPos(playerid, x, y, z + 8.0);
		SetPlayerCameraLookAt(playerid, x, y, z);

		TogglePlayerControllable(playerid, 0);
		
		UpdatePlayerBWTextdraw(playerid);
	}
	else 
	{
		new health = floatround(pInfo[playerid][player_health]);
		if( health == 0 ) health = 5;
		SetPlayerHealth(playerid, health);
	}
	
	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
	
	SetPlayerRealTime(playerid);

	if( GetPlayerVirtualWorld(playerid) > 0 )
	{
		if( pInfo[playerid][player_freeze_door] ) FreezePlayer(playerid, 2500);
	}

	if(!pInfo[playerid][player_interface]) Alert(playerid, ALERT_TYPE_INFO, "Dla graczy grajacych na wyzszych rozdzielczosciach zalecamy wlaczenie interfejsu ~y~Honest ~y~RolePlay~w~. Jest to mozliwe poprzez wybor w ~g~/stats~w~.");

	new
		Float:x,
		Float:y,
		Float:z;

	if(GetPlayerVirtualWorld(playerid) > 0) Streamer_UpdateEx(playerid, x, y, z);

	return 1;
}

public OnPlayerEnterDynamicArea(playerid, areaid)
{
	if( !pInfo[playerid][player_logged] ) return 1;

	pInfo[playerid][player_area] = areaid;

	switch( Area[areaid][area_type] )
	{
		case AREA_TYPE_DOOR_OUTER:
		{
			new d_id = Area[areaid][area_owner];
			
			ShowPlayerDoorTextdraw(playerid, d_id);
		}

		case AREA_TYPE_NORMAL:
		{
			PlayerTextDrawHide(playerid, pInfo[playerid][AreaInfo]);
			PlayerTextDrawSetString(playerid, pInfo[playerid][AreaInfo], sprintf("~y~#%d", areaid));
			PlayerTextDrawShow(playerid, pInfo[playerid][AreaInfo]);

			if(!isnull(Area[areaid][area_music_url]))
			{
				new Float:ox, Float:oy, Float:oz;
				GetDynamicObjectPos(Area[areaid][area_boombox_id], ox, oy, oz);
				PlayAudioStreamForPlayer(playerid, Area[areaid][area_music_url], ox, oy, oz, 20.0, 1);

				pInfo[playerid][player_has_as_mus] = true;
				SendClientMessageToAll(COLOR_YELLOW, sprintf("[D] areamusicurl%s areaboomboxid%d", Area[areaid][area_music_url], Area[areaid][area_boombox_id]));
			}
		}
		
		case AREA_TYPE_ACTOR:
		{
			ShowHint(playerid, "Moesz wej w interakcj z tym aktorem wciskajc ~y~Y~w~.");
		}

		case AREA_TYPE_CERPEK:
		{
			ApplyAnimation(0, "ON_LOOKERS", "wave_loop", 4.0, 0, 1, 1, 0, 0, 1);
			
			if( pInfo[playerid][player_job] == 0 )
			{
				SendClientMessage(playerid, 0xD8D8D8FF, "CERPEK: Hej, pewnie rozgldasz si za prac. Mam co dla Ciebie!");
				
				DynamicGui_Init(playerid);
				
				DynamicGui_AddRow(playerid, WORK_TYPE_SALESMAN);
				DynamicGui_AddRow(playerid, WORK_TYPE_PAPERMAN);
				DynamicGui_AddRow(playerid, WORK_TYPE_STORAGEMAN);
				DynamicGui_AddRow(playerid, WORK_TYPE_FISHMAN);
				
				ShowPlayerDialog(playerid, DIALOG_WORKS, DIALOG_STYLE_LIST, "Dostpne prace dorywcze:", "Sprzedawca\nRoznosiciel gazet\nMagazynier\nRybak", "Wybierz", "Zamknij");
			}
			else
			{
				SendClientMessage(playerid, 0xD8D8D8FF, "Urzdnik mwi: Niestety, nie mam dla Ciebie adnych ofert pracy, bo jeste ju zatrudniony. Aby opuci dotychczasow prac wpisz /praca opusc.");
			}
		}
		
		case AREA_TYPE_CERPEK2:
		{
			DynamicGui_Init(playerid);
			new string[100];
			
			if( !PlayerHasDocument(playerid, DOCUMENT_ID) )
			{			
				format(string, sizeof(string), "%sDowd osobisty ($50)\n", string);
				DynamicGui_AddRow(playerid, DOCUMENT_ID);
			}
			
			if( !PlayerHasDocument(playerid, DOCUMENT_DRIVE) )
			{			
				format(string, sizeof(string), "%sPrawo jazdy ($150)\n", string);
				DynamicGui_AddRow(playerid, DOCUMENT_DRIVE);
			}
			
			if( strlen(string) > 0 )
			{
				SendClientMessage(playerid, 0xD8D8D8FF, "CERPEK: Witaj, pewnie chcesz wyrobi niezbdne dokumenty.");
				ShowPlayerDialog(playerid, DIALOG_DOCUMENTS, DIALOG_STYLE_LIST, "Dostpne dokumenty:", string, "Wybierz", "Zamknij");
			}
		}
	}
	return 1;
}

public OnPlayerLeaveDynamicArea(playerid, areaid)
{
	if( !pInfo[playerid][player_logged] ) return 1;

	switch( Area[areaid][area_type] )
	{
		case AREA_TYPE_DOOR_OUTER:
		{
			HidePlayerDoorTextdraw(playerid);
		}
		
		case AREA_TYPE_NORMAL:
		{
			PlayerTextDrawHide(playerid, pInfo[playerid][AreaInfo]);
			if(pInfo[playerid][player_has_as_mus])
			{
				pInfo[playerid][player_has_as_mus] = false;
				SendClientMessageToAll(COLOR_YELLOW, sprintf("[D] Exited area!!! areamusicurl%s areaboomboxid%d", Area[areaid][area_music_url], Area[areaid][area_boombox_id]));
				StopAudioStreamForPlayer(playerid);
			}
		}
	}
	return 1;
}

public OnDynamicActorStreamIn(actorid, forplayerid)
{
	SetDynamicActorPos(actorid, Actor[actorid][actor_pos_x], Actor[actorid][actor_pos_y], Actor[actorid][actor_pos_z]);

	return 1;
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	new wslot = GetWeaponSlot(weaponid);
	
	if(pWeapon[playerid][wslot][pw_id] != weaponid)
	{
    	new str[32];
    	format(str, sizeof(str), "AntyCheat: Unauthorized weapon shot (%s)", WeaponNames[weaponid]);
    	AddPlayerPenalty(playerid, PENALTY_TYPE_KICK, INVALID_PLAYER_ID, 0, str);

    	PlayerLog(sprintf("Unathorized shot {WEAPON:%d,HIT_TYPE:%d,HIT_ID:%d,X:%.1f,Y:%.1f,Z:%.1f}", weaponid, hittype, hitid, fX, fY, fZ), pInfo[playerid][player_id], "anticheat");
    	return 1;
	}

	pWeapon[playerid][wslot][pw_ammo] -= 1;
	
	PlayerLog(sprintf("Shot {WEAP:%d,HIT_TYPE:%d,HIT_ID:%d,X:%.1f,Y:%.1f,Z:%.1f}", weaponid, hittype, hitid, fX, fY, fZ), pInfo[playerid][player_id], "ammo");

	if( pWeapon[playerid][wslot][pw_ammo] == 0 )
	{
		Item_Use(pWeapon[playerid][wslot][pw_itemid], playerid);
	}
	return 1;
}

public OnPlayerEditAttachedObject(playerid, response, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ)
{
	if( pInfo[playerid][player_attached_item_edit] )
	{
		new itemid = pInfo[playerid][player_attached_item_edit_id];
		
		if( response )
		{
			new str[500];
			strcat(str, sprintf("UPDATE crp_access SET access_posx = %f, access_posy = %f, access_posz = %f, access_rotx = %f, access_roty = %f, access_rotz = %f", fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ));
			strcat(str, sprintf(", access_scalex = %f, access_scaley = %f, access_scalez = %f WHERE access_uid = %d", fScaleX, fScaleY, fScaleZ, Item[itemid][item_value1]));
			
			mysql_pquery(g_sql, str);
			
			SendGuiInformation(playerid, "Informacja", sprintf("Pomylnie zapisae pozycj dodatku %s [UID: %d]", Item[itemid][item_name], Item[itemid][item_uid]));
		}
		else
		{
			Item_Use(itemid, playerid);
			
			SendGuiInformation(playerid, "Informacja", sprintf("Anulowae edycj pozycji dodatku %s [UID: %d]", Item[itemid][item_name], Item[itemid][item_uid]));
		}
		
		pInfo[playerid][player_attached_item_edit] = false;
		pInfo[playerid][player_attached_item_edit_id] = -1;
		
		return 1;
	}
	
    if(response)
    {
		RemovePlayerAttachedObject(playerid, index);
		SetPlayerAttachedObject(playerid, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ);
        
		ao[playerid][index][ao_x] = fOffsetX;
        ao[playerid][index][ao_y] = fOffsetY;
        ao[playerid][index][ao_z] = fOffsetZ;
        ao[playerid][index][ao_rx] = fRotX;
        ao[playerid][index][ao_ry] = fRotY;
        ao[playerid][index][ao_rz] = fRotZ;
        ao[playerid][index][ao_sx] = fScaleX;
        ao[playerid][index][ao_sy] = fScaleY;
        ao[playerid][index][ao_sz] = fScaleZ;
    }
	
    return 1;
}



public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	new inputtext2[140];
    new j=0,k=0;
    while(inputtext[j] != EOS)
    {
        if(inputtext[j] != '%')
        {
            inputtext2[k] = inputtext[j];
            
            k++;
        }
        j++;
    }

    #define inputtext inputtext2
	
    if(pInfo[playerid][player_dialog] != dialogid)
    {
    	new str[64];
    	format(str, sizeof(str), "AntyCheat: Spoofed dialog %d response : %d", dialogid, pInfo[playerid][player_dialog]);
    	AddPlayerPenalty(playerid, PENALTY_TYPE_KICK, INVALID_PLAYER_ID, 0, str);
    }

    pInfo[playerid][player_dialog] = -1;

	switch( dialogid )
	{
		case DIALOG_LOGIN:
		{
			if( !response ) return ShowPlayerDialog(playerid, DIALOG_CHANGENAME, DIALOG_STYLE_INPUT, "Zmiana nicku", "W ponisze pole wprowad nowy nick:", "Wybierz", "Zamknij");
			
			if( isnull(inputtext) )
			{
				gInfo[playerid][global_bad_pass] += 1;
				return OutputLoginForm(playerid);
			}
			
			new text[100];
			utf8_translate(inputtext, text);

			strreplace(text, "&", "&amp;");
			strreplace(text, "<", "&lt;");
			strreplace(text, ">", "&gt;");
			strreplace(text, "\"", "&quot;");
			strreplace(text, "$", "&#036;");
			strreplace(text, "!", "&#33;");
			strreplace(text, "'", "&#39;");

			mysql_escape_string(text, text);

			CheckPasswordCorrectness(playerid, text);
		}

		case DIALOG_CHANGENAME:
		{
			if( !response ) return Kick(playerid);
			
			new oldname[MAX_PLAYER_NAME];
			GetPlayerName(playerid, oldname, sizeof(oldname));

			if(strlen(inputtext) < 3 || strlen(inputtext) > 24)
            {
                return ShowPlayerDialog(playerid, DIALOG_CHANGENAME, DIALOG_STYLE_INPUT, "Zmiana nicku", "W ponisze pole wprowad nowy nick:\n\n"HEX_COLOR_LIGHTER_RED"Podae niepoprawny nick.", "Wybierz", "Zamknij");
            }

            new name[40];
            format(name, sizeof(name), inputtext);
            SpaceToUnderscore(name);

            new changed = SetPlayerName(playerid, name);
            if(changed != 1)
            {
                return ShowPlayerDialog(playerid, DIALOG_CHANGENAME, DIALOG_STYLE_INPUT, "Zmiana nicku", "W ponisze pole wprowad nowy nick:\n\n"HEX_COLOR_LIGHTER_RED"Podae niepoprawny nick.", "Wybierz", "Zamknij");
            }
            
            UnderscoreToSpace(name);
            strcopy(pInfo[playerid][player_name], name, MAX_PLAYER_NAME+1);

            GameTextForPlayer(playerid, "~n~~n~~n~~y~Nick postaci zostal zmieniony", 2000, 5);

            pInfo[playerid][player_changed_nick] = true;
            gInfo[playerid][global_registered] = false;
            OutputLoginForm(playerid, true);
		}
		
		case DIALOG_LOGIN_NO_ACCOUNT:
		{
			if( !response ) return Kick(playerid);

			ShowPlayerDialog(playerid, DIALOG_CHANGENAME, DIALOG_STYLE_INPUT, "Zmiana nicku", "W ponisze pole wprowad nowy nick.", "Wybierz", "Zamknij");\
		}

		case DIALOG_LOGIN_NO_ACCOUNT_SUGGESTED:
		{
			if( !response ) return ShowPlayerDialog(playerid, DIALOG_CHANGENAME, DIALOG_STYLE_INPUT, "Zmiana nicku", "W ponisze pole wprowad nowy nick.", "Wybierz", "Zamknij");

			new char_uid = DynamicGui_GetDataInt(playerid, listitem);

			if(char_uid == 0) ShowPlayerDialog(playerid, DIALOG_CHANGENAME, DIALOG_STYLE_INPUT, "Zmiana nicku", "W ponisze pole wprowad nowy nick:", "Wybierz", "Zamknij");
			else
			{
				new name[MAX_PLAYER_NAME], oldname[MAX_PLAYER_NAME];

				new Cache:result;
				result = mysql_query(g_sql, sprintf("SELECT char_name FROM crp_characters WHERE char_uid = %d", char_uid));
				cache_get(0, "char_name", name);
				cache_delete(result);

				SpaceToUnderscore(name);

				GetPlayerName(playerid, oldname, sizeof(oldname));

	            if(SetPlayerName(playerid, name) != 1) return ShowPlayerDialog(playerid, DIALOG_CHANGENAME, DIALOG_STYLE_INPUT, "Zmiana nicku", "W ponisze pole wprowad nowy nick:\n\n"HEX_COLOR_LIGHTER_RED"Wystpi bd w trakcie zmiany nicku.", "Wybierz", "Zamknij");

	            UnderscoreToSpace(name);

           		strcopy(pInfo[playerid][player_name], name, MAX_PLAYER_NAME+1);

	            GameTextForPlayer(playerid, "~n~~n~~n~~y~Nick postaci zostal zmieniony", 2000, 5);
	            

	            pInfo[playerid][player_changed_nick] = true;
	            gInfo[playerid][global_registered] = false;
	            OutputLoginForm(playerid, true);
			}
		}

		case DIALOG_GLOBAL_CHOOSE_CHAR:
		{
			new uid = DynamicGui_GetValue(playerid, listitem);

	        pInfo[playerid][player_id] = uid;

	        OnCharacterLoggedIn(playerid);
		}
		
		case DIALOG_DRZWI:
		{
			PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
			
			if( !response )
			{
				return 1;
			}
			
			new d_id = DynamicGui_GetDialogValue(playerid);
			
			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_DRZWI_NAME:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;
					
					ShowPlayerDialog(playerid, DIALOG_DRZWI_NAME, DIALOG_STYLE_INPUT, "Zmiana nazwy drzwi", "W polu poniej podaj now nazw dla tych drzwi:", "Gotowe", "Zamknij");
				}
				
				case DG_DRZWI_SPAWN:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;
					
					ShowPlayerDialog(playerid, DIALOG_DRZWI_SPAWN, DIALOG_STYLE_MSGBOX, "Zmiana wewntrznej pozycji drzwi", "Czy jeste pewien, e chcesz zmieni wewntrzn pozycj drzwi na t, w ktrej aktualnie si znajdujesz?", "Zmie", "Zamknij");
				}
				
				case DG_DRZWI_SPAWN_COORDS:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;
					
					ShowPlayerDialog(playerid, DIALOG_DRZWI_SPAWN_COORDS, DIALOG_STYLE_INPUT, "Zmiana wewntrznej pozycji drzwi (koordynaty)", "W poniszym polu podaj pozycj x,y,z,a odzielajc poszczeglne wsprzne przecinkami:", "Zmie", "Zamknij");
				}
				
				case DG_DRZWI_AUDIO:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;
					
					ShowPlayerDialog(playerid, DIALOG_DRZWI_AUDIO, DIALOG_STYLE_INPUT, "Zmiana cieki audio", "W poniszym polu podaj ciek do pliku lub streamu radia(pozostaw pole puste, aby wyczy muzyk):", "Zmie", "Zamknij");
				}
				
				case DG_DRZWI_PAYMENT:
				{
					pInfo[playerid][player_dialog_tmp1] = d_id;
					
					ShowPlayerDialog(playerid, DIALOG_DRZWI_PAYMENT, DIALOG_STYLE_INPUT, "Zmiana opaty za wejcie", "W poniszym polu podaj ilo pienidzy pobieranych ze wejcie do budynku:", "Zmie", "Zamknij");
				}
				
				case DG_DRZWI_CARS:
				{
					Door[d_id][door_car_crosing] = !Door[d_id][door_car_crosing];
					mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_garage` = %d WHERE `door_uid` = %d", Door[d_id][door_car_crosing], Door[d_id][door_uid]));
					
					return cmd_drzwi(playerid, "opcje");
				}
				
				case DG_DRZWI_CLOSING:
				{					
					Door[d_id][door_auto_closing] = !Door[d_id][door_auto_closing];
					mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_lock` = %d WHERE `door_uid` = %d", Door[d_id][door_auto_closing], Door[d_id][door_uid]));
					
					return cmd_drzwi(playerid, "opcje");
				}

				case DG_DRZWI_METERS:
				{
					pInfo[playerid][player_edit_meters] = true;

					new
						Float:x,
						Float:y,
						Float:z;

					GetPlayerPos(playerid, x, y, z);

					pInfo[playerid][player_meters_pos][0] = x;
					pInfo[playerid][player_meters_pos][1] = y;
					pInfo[playerid][player_meters_pos][2] = z;

					pInfo[playerid][player_edit_meters_vw] = GetPlayerVirtualWorld(playerid);
				}
				
				case DG_DRZWI_MAP_LOAD:
				{
					new p_count;
		
					if( Door[d_id][door_spawn_vw] > 0 )
					{
						foreach(new p : Player)
						{
							if( GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] )
							{
								SetPlayerVirtualWorld(p, Door[d_id][door_vw]);
								SetPlayerInterior(p, Door[d_id][door_int]);

								SetPlayerPos(p, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
								SetPlayerFacingAngle(p, Door[d_id][door_pos][3]);
								
								SendClientMessage(p, COLOR_LIGHTER_RED, "W drzwiach, w ktrych bye przeadowano obiekty. Zostae przeniesiony do ich wejcia.");
								
								p_count++;
							}
						}
					}
					
					for(new oid;oid<MAX_OBJECTS;oid++)
					{
						if( Object[oid][object_uid] < 1 ) continue;
						if( Object[oid][object_owner_type] == OBJECT_OWNER_TYPE_DOOR && Object[oid][object_owner] == Door[d_id][door_uid] )
						{
							DeleteObject(oid, false);
						}
					}
					
					new count = LoadObject(sprintf("WHERE object_ownertype = %d AND object_owner = %d", OBJECT_OWNER_TYPE_DOOR, Door[d_id][door_uid]));
					
					GetPlayerPos(playerid, Door[d_id][door_spawn_pos][0],Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
					GetPlayerFacingAngle(playerid, Door[d_id][door_spawn_pos][3]);
					
					new Cache:result;
					result = mysql_query(g_sql, sprintf("SELECT * FROM crp_doors WHERE door_uid = %d", Door[d_id][door_uid]));

					Door[d_id][door_spawn_pos][0] = cache_get_float(0, "door_exitx");
					Door[d_id][door_spawn_pos][1] = cache_get_float(0, "door_exity");
					Door[d_id][door_spawn_pos][2] = cache_get_float(0, "door_exitz");
					Door[d_id][door_spawn_pos][3] = cache_get_float(0, "door_exita");
					
					cache_delete(result);
					
					DestroyDynamicArea(Door[d_id][door_area_inner]);
					Iter_Remove(Areas, Door[d_id][door_area_inner]);
					
					// Strefa wewn. drzwi
					Door[d_id][door_area_inner] = CreateDynamicSphere(Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], 2.0, Door[d_id][door_spawn_vw], Door[d_id][door_spawn_int]);
					
					Area[Door[d_id][door_area_inner]][area_type] = AREA_TYPE_DOOR_INNER;
					Area[Door[d_id][door_area_inner]][area_owner_type] = 0;
					Area[Door[d_id][door_area_inner]][area_owner] = d_id;
					Area[Door[d_id][door_area_inner]][area_uid] = -1;
					
					Iter_Add(Areas, Door[d_id][door_area_inner]);
							
					SendGuiInformation(playerid, "Informacja", sprintf("Obiekty drzwi %s (UID: %d) zostay pomylnie przeadowane (%d obiektw).", Door[d_id][door_name], Door[d_id][door_uid], count));
				}
				
				case DG_DRZWI_SCHOWEK:
				{
					new count;
			
					DynamicGui_Init(playerid);
					new string[300];
			
					format(string, sizeof(string), "%s{C0C0C0}Przedmioty znajdujce si w schowku drzwi:\n", string);
					DynamicGui_AddBlankRow(playerid);
					
					foreach (new i : Items)
					{
						if( Item[i][item_owner_type] != ITEM_OWNER_TYPE_DOOR || Item[i][item_owner] != Door[d_id][door_uid] ) continue;
						
						format(string, sizeof(string), "%s%d\t%s\n", string, Item[i][item_uid], Item[i][item_name]);
						DynamicGui_AddRow(playerid, DG_ITEMS_PICKUP_ROW, i);
						
						count++;
					}
					
					if( count == 0 ) SendGuiInformation(playerid, "Wystpi bd", "W schowku tych drzwi nie ma przedmiotw.");
					else ShowPlayerDialog(playerid, DIALOG_ITEMS_PICKUP, DIALOG_STYLE_LIST, "Dostpne przedmioty", string, "Podnie", "Zamknij");
				}
			}
		}
		
		case DIALOG_DRZWI_NAME:
		{
			if( !response ) return cmd_drzwi(playerid, "opcje");
			
			if( strlen(inputtext) < 6 ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_NAME, DIALOG_STYLE_INPUT, "Zmiana nazwy drzwi", "W polu poniej podaj now nazw dla tych drzwi:\n\n"HEX_COLOR_LIGHTER_RED"Nazwa musi zawiera minimum 6 znakw.", "Gotowe", "Zamknij");
			if( strlen(inputtext) > 30 ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_NAME, DIALOG_STYLE_INPUT, "Zmiana nazwy drzwi", "W polu poniej podaj now nazw dla tych drzwi:\n\n"HEX_COLOR_LIGHTER_RED"Nazwa moe zawiera maksymalnie 30 znakw.", "Gotowe", "Zamknij");
			if( strfind(inputtext, "~~") != -1 ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_NAME, DIALOG_STYLE_INPUT, "Zmiana nazwy drzwi", "W polu poniej podaj now nazw dla tych drzwi:\n\n"HEX_COLOR_LIGHTER_RED"Nazwa zawiera bdne znaki.", "Gotowe", "Zamknij");
			
			new d_id = pInfo[playerid][player_dialog_tmp1];
		
			new text[260];
			mysql_escape_string(inputtext, text);
			mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_name` = '%s' WHERE `door_uid` = %d", text, Door[d_id][door_uid]));

			PlayerLog(sprintf("Changed name of doors %s to {NAME:%s,OLD_NAME:%s}", DoorLogLink(Door[d_id][door_uid]), inputtext, Door[d_id][door_name]), pInfo[playerid][player_id], "door");
			strcopy(Door[d_id][door_name], inputtext, 30);


			SendFormattedClientMessage(playerid, COLOR_GREY, "Nazwa drzwi zostaa pomylnie zmieniona na: %s.", inputtext);
			if( GetPlayerArea(playerid, AREA_TYPE_DOOR_OUTER) != -1 ) ShowPlayerDoorTextdraw(playerid, d_id);
			cmd_drzwi(playerid, "opcje");
		}
		
		case DIALOG_DRZWI_SPAWN:
		{
			if( !response ) return cmd_drzwi(playerid, "opcje");
			
			new d_id = pInfo[playerid][player_dialog_tmp1];
			
			GetPlayerPos(playerid, Door[d_id][door_spawn_pos][0],Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
			GetPlayerFacingAngle(playerid, Door[d_id][door_spawn_pos][3]);
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_exitx` = %f, `door_exity` = %f, `door_exitz` = %f, `door_exita` = %f WHERE `door_uid` = %d", Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3], Door[d_id][door_uid]));
			
			PlayerLog(sprintf("Changed inner position of doors %s to {POS:%.1f,%.1f,%.1f}", DoorLogLink(Door[d_id][door_uid]), Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]), pInfo[playerid][player_id], "door");

			SendClientMessage(playerid, COLOR_GOLD, "Wewntrzna pozycja drzwi zostaa pomylnie zmieniona.");
			
			DestroyDynamicArea(Door[d_id][door_area_inner]);
			Iter_Remove(Areas, Door[d_id][door_area_inner]);
			
			// Strefa wewn. drzwi
			Door[d_id][door_area_inner] = CreateDynamicSphere(Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], 2.0, Door[d_id][door_spawn_vw], Door[d_id][door_spawn_int]);
			
			Area[Door[d_id][door_area_inner]][area_type] = AREA_TYPE_DOOR_INNER;
			Area[Door[d_id][door_area_inner]][area_owner_type] = 0;
			Area[Door[d_id][door_area_inner]][area_owner] = d_id;
			Area[Door[d_id][door_area_inner]][area_uid] = -1;
			
			Iter_Add(Areas, Door[d_id][door_area_inner]);
			
			return cmd_drzwi(playerid, "opcje");
		}
		
		case DIALOG_DRZWI_SPAWN_COORDS:
		{
			if( !response ) return cmd_drzwi(playerid, "opcje");
			
			new d_id = pInfo[playerid][player_dialog_tmp1];
			
			if( sscanf(inputtext, "p<,>a<f>[4]", Door[d_id][door_spawn_pos]) ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_SPAWN_COORDS, DIALOG_STYLE_INPUT, "Zmiana wewntrznej pozycji drzwi (koordynaty)", "W poniszym polu podaj pozycj x,y,z,a odzielajc poszczeglne wsprzne przecinkami:\n\n"HEX_COLOR_LIGHTER_RED"Podane dane maj zy format.", "Zmie", "Zamknij");
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_exitx` = %f, `door_exity` = %f, `door_exitz` = %f, `door_exita` = %f WHERE `door_uid` = %d", Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3], Door[d_id][door_uid]));
			
			PlayerLog(sprintf("Changed inner position of doors %s to {POS:%.1f,%.1f,%.1f}", DoorLogLink(Door[d_id][door_uid]), Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]), pInfo[playerid][player_id], "door");

			SendClientMessage(playerid, COLOR_GOLD, "Wewntrzna pozycja drzwi zostaa pomylnie zmieniona.");
			
			DestroyDynamicArea(Door[d_id][door_area_inner]);
			Iter_Remove(Areas, Door[d_id][door_area_inner]);
			
			// Strefa wewn. drzwi
			Door[d_id][door_area_inner] = CreateDynamicSphere(Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], 2.0, Door[d_id][door_spawn_vw], Door[d_id][door_spawn_int]);
			
			Area[Door[d_id][door_area_inner]][area_type] = AREA_TYPE_DOOR_INNER;
			Area[Door[d_id][door_area_inner]][area_owner_type] = 0;
			Area[Door[d_id][door_area_inner]][area_owner] = d_id;
			Area[Door[d_id][door_area_inner]][area_uid] = -1;
			
			Iter_Add(Areas, Door[d_id][door_area_inner]);
			
			return cmd_drzwi(playerid, "opcje");
		}
		
		case DIALOG_DRZWI_AUDIO:
		{
			if( !response ) return cmd_drzwi(playerid, "opcje");

			new d_id = pInfo[playerid][player_dialog_tmp1];
			
			Door[d_id][door_audio][0] = EOS;
			
			sscanf(inputtext, "s[100]", Door[d_id][door_audio]);
			
			new text[200];
			mysql_escape_string(Door[d_id][door_audio], text);
			mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_audiourl` = '%s' WHERE `door_uid` = %d", text, Door[d_id][door_uid]));
			
			PlayerLog(sprintf("Changed audio url of doors %s to {URL:%s}", DoorLogLink(Door[d_id][door_uid]), text), pInfo[playerid][player_id], "door");

			SendClientMessage(playerid, COLOR_GOLD, "cieka audio zostaa pomylnie zmieniona.");
			
			foreach(new p : Player)
			{
				if( GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] )
				{
					if( !isnull(Door[d_id][door_audio]) ) PlayAudioStreamForPlayer(p, Door[d_id][door_audio], 0);
					else StopAudioStreamForPlayer(p);
				}
			}

			return cmd_drzwi(playerid, "opcje");
		}
		
		case DIALOG_DRZWI_PAYMENT:
		{
			if( !response ) return cmd_drzwi(playerid, "opcje");
			
			new payment;
			if( sscanf(inputtext, "d", payment) ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_PAYMENT, DIALOG_STYLE_INPUT, "Zmiana opaty za wejcie", "W poniszym polu podaj ilo pienidzy pobieranych ze wejcie do budynku:\n\n"HEX_COLOR_LIGHTER_RED"Podae niepoprawn kwot.", "Zmie", "Zamknij");
			if( payment < 0 ) return ShowPlayerDialog(playerid, DIALOG_DRZWI_PAYMENT, DIALOG_STYLE_INPUT, "Zmiana opaty za wejcie", "W poniszym polu podaj ilo pienidzy pobieranych ze wejcie do budynku:\n\n"HEX_COLOR_LIGHTER_RED"Podae niepoprawn kwot.", "Zmie", "Zamknij");
			
			new d_id = pInfo[playerid][player_dialog_tmp1];
			
			PlayerLog(sprintf("Changed payment of doors %s to {PAYMENT:%d,OLD_PAYMENT:%d}", DoorLogLink(Door[d_id][door_uid]), payment, Door[d_id][door_payment]), pInfo[playerid][player_id], "door");

			Door[d_id][door_payment] = payment;
			mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_enterpay` = %d WHERE `door_uid` = %d", Door[d_id][door_payment], Door[d_id][door_uid]));
			
			if( GetPlayerVirtualWorld(playerid) == Door[d_id][door_vw] ) ShowPlayerDoorTextdraw(playerid, d_id);
			
			return cmd_drzwi(playerid, "opcje");
		}
		
		case DIALOG_TRANSACTIONS_HISTORY:
		{
			if( !response ) return 1;

			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_TRANSACTIONS_NEXT:
				{
					ShowPlayerTransactionsHistory(playerid, pInfo[playerid][player_dialog_tmp1]+1);
				}
				case DG_TRANSACTIONS_PREV:
				{
					ShowPlayerTransactionsHistory(playerid, pInfo[playerid][player_dialog_tmp1]-1);
				}
			}
		}

		case DIALOG_ADRZWI_CHANGE_INTERIOR:
		{
			new d_id = DynamicGui_GetDialogValue(playerid);
				
			if( !response ) return 1;

			
			// Next/Prev page
			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_DRZWI_CHANGE_INTERIOR_PREV:
				{
					DoorsDefaultInteriorsList(playerid, d_id, pInfo[playerid][player_dialog_tmp1]-1);
				}
				
				case DG_DRZWI_CHANGE_INTERIOR_NEXT:
				{
					DoorsDefaultInteriorsList(playerid, d_id, pInfo[playerid][player_dialog_tmp1]+1);
				}
				
				case DG_DRZWI_CHANGE_INTERIOR_ROW:
				{
					foreach(new p : Player)
					{
						if( GetPlayerVirtualWorld(p) == Door[d_id][door_spawn_vw] )
						{
							SetPlayerVirtualWorld(p, Door[d_id][door_vw]);
							SetPlayerInterior(p, Door[d_id][door_int]);

							SetPlayerPos(p, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
							SetPlayerFacingAngle(p, Door[d_id][door_pos][3]);
							
							SendClientMessage(p, COLOR_LIGHTER_RED, "Drzwi, w ktrych si znajdowae zostay zmienione przez administratora. Zostae przeniesiony do ich wejcia.");
						}
					}
					
					if( DynamicGui_GetDataInt(playerid, listitem) == -1 )
					{
						Door[d_id][door_spawn_int] = 0;
						Door[d_id][door_spawn_pos][0] = Door[d_id][door_pos][0];
						Door[d_id][door_spawn_pos][1] = Door[d_id][door_pos][1];
						Door[d_id][door_spawn_pos][2] = Door[d_id][door_pos][2];
						Door[d_id][door_spawn_pos][3] = Door[d_id][door_pos][3];
					}
					else
					{
						new Cache:result;
						result = mysql_query(g_sql, sprintf("SELECT interior, x, y, z, a FROM `crp_default_interiors` WHERE `id` = %d", DynamicGui_GetDataInt(playerid, listitem)));
						
						Door[d_id][door_spawn_int] = cache_get_int(0, "interior");
						Door[d_id][door_spawn_pos][0] = cache_get_float(0, "x");
						Door[d_id][door_spawn_pos][1] = cache_get_float(0, "y");
						Door[d_id][door_spawn_pos][2] = cache_get_float(0, "z");
						Door[d_id][door_spawn_pos][3] = cache_get_float(0, "a");
						
						cache_delete(result);
					}
					
					mysql_pquery(g_sql, sprintf("UPDATE `crp_doors` SET `door_exitint` = %d, `door_exitx` = %f, `door_exity` = %f, `door_exitz` = %f, `door_exita` = %f WHERE `door_uid` = %d", Door[d_id][door_spawn_int], Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3], Door[d_id][door_uid]));
			
					DestroyDynamicArea(Door[d_id][door_area_inner]);
					Iter_Remove(Areas, Door[d_id][door_area_inner]);
					
					// Strefa wewn. drzwi
					Door[d_id][door_area_inner] = CreateDynamicSphere(Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], 2.0, Door[d_id][door_spawn_vw], Door[d_id][door_spawn_int]);
					
					Area[Door[d_id][door_area_inner]][area_type] = AREA_TYPE_DOOR_INNER;
					Area[Door[d_id][door_area_inner]][area_owner_type] = 0;
					Area[Door[d_id][door_area_inner]][area_owner] = d_id;
					Area[Door[d_id][door_area_inner]][area_uid] = -1;
					
					Iter_Add(Areas, Door[d_id][door_area_inner]);
					
					AdminLog(sprintf("Changed default interior of doors %s to {INERIOR:%d}", DoorLogLink(Door[d_id][door_uid]), DynamicGui_GetDataInt(playerid, listitem)), pInfo[playerid][player_id], "door");

					SendFormattedClientMessage(playerid, COLOR_GOLD, "Interior drzwi zosta pomylnie zmieniony [INTERIOR: %d, UID: %d, ID: %d].", Door[d_id][door_spawn_int], Door[d_id][door_uid], d_id);
				}
			}
		}
		
		case DIALOG_ADRZWI_PICKUP:
		{
			if( !response ) return 1;
			
			new d_id = DynamicGui_GetDialogValue(playerid);
			
			mysql_query(g_sql, sprintf("UPDATE `crp_doors` SET `door_pickupid` = %d WHERE `door_uid` = %d", DynamicGui_GetDataInt(playerid, listitem), Door[d_id][door_uid]), false);
			
			new uid = Door[d_id][door_uid];
			DeleteDoor(d_id, false);
			
			new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);

			AdminLog(sprintf("Changed pickup of doors %s to {PICKUP:%d}", DoorLogLink(Door[d_id][door_uid]), DynamicGui_GetDataInt(playerid, listitem)), pInfo[playerid][player_id], "door");

			SendFormattedClientMessage(playerid, COLOR_GOLD, "Pickup drzwi zosta pomylnie zmieniony! [PICKUP: %d, UID: %d, ID: %d]", DynamicGui_GetDataInt(playerid, listitem), uid, did);
		}

		case DIALOG_AGRUPA_TYP:
		{
			if( !response ) return 1;
			
			new gid = DynamicGui_GetDialogValue(playerid), type = DynamicGui_GetDataInt(playerid, listitem);
			
			Group[gid][group_type] = type;
			Group[gid][group_flags] = GroupDefaultFlags[type];
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_groups` SET `group_type` = %d, `group_flags` = %d WHERE `group_uid` = %d", Group[gid][group_type], Group[gid][group_flags], Group[gid][group_uid]));
			
			AdminLog(sprintf("Changed type of group %s to {TYPE:%d,FLAGS:%d}", GroupLogLink(Group[gid][group_uid]), Group[gid][group_type], Group[gid][group_flags]), pInfo[playerid][player_id], "group");

			SendGuiInformation(playerid, "Informacja", sprintf("Pomylnie zmienie typ oraz flagi grupy [TYP: %d, FLAG: %d, UID: %d, ID: %d].", Group[gid][group_type], Group[gid][group_flags], Group[gid][group_uid], gid));
		}
		
		case DIALOG_AGRUPA_FLAGI:
		{
			if( !response ) return 1;
			
			new gid = DynamicGui_GetDialogValue(playerid);
			
			new flag_index = DynamicGui_GetValue(playerid, listitem);
			
			if( GroupHasFlag(gid, GroupFlagsBit[flag_index]) )
			{
				AdminLog(sprintf("Removed flag from group %s to {FLAG:%d}", GroupLogLink(Group[gid][group_uid]), GroupFlagsBit[flag_index]), pInfo[playerid][player_id], "group");
				Group[gid][group_flags] -= GroupFlagsBit[flag_index]; 
			}
			else
			{
				AdminLog(sprintf("Added flag to group %s to {FLAG:%d}", GroupLogLink(Group[gid][group_uid]), GroupFlagsBit[flag_index]), pInfo[playerid][player_id], "group");
				Group[gid][group_flags] += GroupFlagsBit[flag_index];
			}

			mysql_pquery(g_sql, sprintf("UPDATE crp_groups SET group_flags = %d WHERE group_uid = %d", Group[gid][group_flags], Group[gid][group_uid]));
			
			cmd_ag(playerid, sprintf("flagi %d", Group[gid][group_uid]));
		}
		
		case DIALOG_CHAR_DESCRIPTION:
		{
			if( response == 0 ) return 1;		
			new dg_value = DynamicGui_GetValue(playerid, listitem);
			
			if( dg_value == DG_CHAR_DESC_DELETE )
			{
				Update3DTextLabelText(pInfo[playerid][player_description_label], LABEL_DESCRIPTION, "");
				pInfo[playerid][player_description][0] = EOS;
				SendGuiInformation(playerid, "Informacja", "Twj aktualny opis zosta usunity.");
				PlayerLog("Removed description", pInfo[playerid][player_id], "basic");
			}
			else if( dg_value == DG_CHAR_DESC_ADD)
			{
				ShowPlayerDialog(playerid, DIALOG_CHAR_DESCRIPTION_ADD, DIALOG_STYLE_INPUT, "Opis postaci", "Poniej wpisz opis, ktry chcesz ustawi. (max. 128 znakw)", "Ustaw", "Zamknij");
			}
			else if( dg_value == DG_CHAR_DESC_OLD )
			{
				// -- Zmiana opisu na wczeniej zapisany -- //
				new Cache:result;
				result = mysql_query(g_sql, sprintf("SELECT * FROM `characters_descriptions` WHERE `uid` = %d", DynamicGui_GetDataInt(playerid, listitem)));
				
				new oldDesc[150];
				cache_get(0, "text", oldDesc);		
				
				cache_delete(result);

				mysql_pquery(g_sql, sprintf("UPDATE `characters_descriptions` SET `last_used` = '%d' WHERE `uid`='%d'", gettime(), DynamicGui_GetDataInt(playerid, listitem)));
								
				strcopy(pInfo[playerid][player_description], oldDesc);

				Update3DTextLabelText(pInfo[playerid][player_description_label], LABEL_DESCRIPTION, BreakLines(oldDesc, "\n", 32));
				SendGuiInformation(playerid, "Informacja", "Twj aktualny opis zosta zmieniony.");

				PlayerLog(sprintf("Set his description to: %s", oldDesc), pInfo[playerid][player_id], "basic");
			}
		}
	  
		case DIALOG_CHAR_DESCRIPTION_ADD:
		{
			if( response == 0 ) return cmd_opis(playerid, "");
			if(strlen(inputtext) > 128 ) return ShowPlayerDialog(playerid, DIALOG_CHAR_DESCRIPTION_ADD, DIALOG_STYLE_INPUT, "Opis postaci", "Poniej wpisz opis, ktry chcesz ustawi. (max. 128 znakw)", "Ustaw", "Zamknij");
			
			new text[260];
			mysql_escape_string(inputtext, text);

			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT * FROM `characters_descriptions` WHERE `text` LIKE '%s' AND `owner`='%d'", text, pInfo[playerid][player_id]));
			
			new descUid = 0;
			if( cache_get_rows() )
			{
				descUid = cache_get_int(0, "uid");
				cache_delete(result);
			}
			
			if(descUid > 0) {
				mysql_pquery(g_sql, sprintf("UPDATE `characters_descriptions` SET `last_used`='%d' WHERE `uid`='%d'", gettime(), descUid));
			}
			else {
				mysql_pquery(g_sql, sprintf("INSERT INTO `characters_descriptions` (uid, owner, text, last_used) VALUES (null, '%d', '%s', '%d')", pInfo[playerid][player_id], text, gettime()));
			}
					
			strcopy(pInfo[playerid][player_description], text, 128);

			PlayerLog(sprintf("Set his description to: %s", text), pInfo[playerid][player_id], "basic");

			Update3DTextLabelText(pInfo[playerid][player_description_label], LABEL_DESCRIPTION, BreakLines(pInfo[playerid][player_description], "\n", 32));
			SendGuiInformation(playerid, "Informacja", "Twj aktualny opis zosta zmieniony.");
		}
		
		case DIALOG_PLAYER_VEHICLES:
		{
			if( !response ) return 1;
			
			new v_uid = DynamicGui_GetValue(playerid, listitem), vid = GetVehicleByUid(v_uid);
			if( vid != INVALID_VEHICLE_ID )
			{
				if( Vehicle[vid][vehicle_state] > 0 ) return SendGuiInformation(playerid, "Wystpi bd", "Na tym pojedzie przeprowadzana jest aktualnie jaka akcja. Aby go odspawnowa poczekaj do jej ukoczenia.");
				PlayerLog(sprintf("Unspawned vehicle %s {SAMPID:%d,HEALTH:%.1f}", VehicleLogLink(Vehicle[vid][vehicle_uid]), vid, Vehicle[vid][vehicle_health]), pInfo[playerid][player_id], "vehicle");
				DeleteVehicle(vid);
				GameTextForPlayer(playerid, "~r~Pojazd odspawnowany", 3000, 3);
			}
			else
			{
				new count = 0;
				foreach(new v_id : Vehicles)
				{
					if( Vehicle[v_id][vehicle_owner_type] == VEHICLE_OWNER_TYPE_PLAYER && Vehicle[v_id][vehicle_owner] == pInfo[playerid][player_id] ) count++;
				}

				if( IsPlayerVip(playerid) && count >= 5 ) return SendGuiInformation(playerid, "Wystpi bd", "Posiadasz konto premium wic Twj limit to 5 zespawnowanych pojazdw.");
				else if( !IsPlayerVip(playerid) && count >= 3 ) return SendGuiInformation(playerid, "Wystpi bd", "Nie posiadasz konta premium wic Twj limit to 3 zespawnowane pojazdy.");

				LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", v_uid), true);

				new vehicleid = GetVehicleByUid(v_uid);

				PlayerLog(sprintf("Spawned vehicle %s {SAMPID:%d,HEALTH:%.1f}", VehicleLogLink(Vehicle[vehicleid][vehicle_uid]), vehicleid, Vehicle[vehicleid][vehicle_health]), pInfo[playerid][player_id], "vehicle");
				GameTextForPlayer(playerid, "~g~Pojazd zespawnowany", 3000, 3);
			}
		}
		
		case DIALOG_GROUP_VEHICLES:
		{
			if( !response ) return 1;
			
			new vid = DynamicGui_GetValue(playerid, listitem);
			
			cmd_v(playerid, sprintf("namierz %d", Vehicle[vid][vehicle_uid]));
		}
		
		case DIALOG_PLAYER_VEHICLE_PANEL:
		{
			if( !response ) return 1;
			
			new vid = GetPlayerVehicleID(playerid);		
			if( vid == INVALID_VEHICLE_ID ) return 1;
			
			new selected = DynamicGui_GetValue(playerid, listitem);
			
			switch( selected )
			{
				case DG_PLAYER_VEHICLE_PANEL_LIGHTS:
				{
					Vehicle[vid][vehicle_lights] = !Vehicle[vid][vehicle_lights];
				}
				
				case DG_PLAYER_VEHICLE_PANEL_BOOT:
				{
					Vehicle[vid][vehicle_boot] = !Vehicle[vid][vehicle_boot];
				}
				
				case DG_PLAYER_VEHICLE_PANEL_BONNET:
				{
					Vehicle[vid][vehicle_bonnet] = !Vehicle[vid][vehicle_bonnet];
				}

				case DG_PLAYER_VEHICLE_PANEL_RADIO:
				{
					if( !Vehicle[vid][vehicle_radio] )
					{
						new cdid = -1;
					
						foreach(new itid : Items)
						{
							if( Item[itid][item_owner_type] == ITEM_OWNER_TYPE_VEHICLE_COMPONENT && Item[itid][item_owner] == Vehicle[vid][vehicle_uid] && Item[itid][item_type] == ITEM_TYPE_CD )
							{
								cdid = itid;
								break;
							}
						}
						
						if( cdid == -1 ) return SendGuiInformation(playerid, "Wystpi bd", "Aby wczy radio musisz najpierw wsadzi do niego pyt CD.");
						
						mysql_tquery(g_sql, sprintf("SELECT audio_url FROM crp_audiourls WHERE audio_uid = %d", Item[cdid][item_value1]), "OnCdUrlLoaded_vehid", "i", vid);
					}
					else
					{
						foreach(new p : Player)
						{
							if( GetPlayerVehicleID(p) == vid )
							{
								StopAudioStreamForPlayer(p);
							}
						}
					}
					Vehicle[vid][vehicle_radio] = !Vehicle[vid][vehicle_radio];
				}
				
				case DG_PLAYER_VEHICLE_PANEL_CDOUT:
				{
					new cdid = -1;
					
					foreach(new itid : Items)
					{
						if( Item[itid][item_owner_type] == ITEM_OWNER_TYPE_VEHICLE_COMPONENT && Item[itid][item_owner] == Vehicle[vid][vehicle_uid] && Item[itid][item_type] == ITEM_TYPE_CD )
						{
							cdid = itid;
							break;
						}
					}
					
					if( cdid == -1 ) return 1;
					
					if( Vehicle[vid][vehicle_radio] )
					{
						foreach(new p : Player)
						{
							if( GetPlayerVehicleID(p) == vid )
							{
								StopAudioStreamForPlayer(p);
							}
						}
						
						Vehicle[vid][vehicle_radio] = false;
					}
					
					Item[cdid][item_owner_type] = ITEM_OWNER_TYPE_PLAYER;
					Item[cdid][item_owner] = pInfo[playerid][player_id];
					
					mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_ownertype = %d, item_owner = %d WHERE item_uid = %d", Item[cdid][item_owner_type], Item[cdid][item_owner], Item[cdid][item_uid]));
					
					ProxMessage(playerid, "wyciga pyt z radia.", PROX_AME);
				}
			}
			
			if( selected == DG_PLAYER_VEHICLE_PANEL_LIGHTS || selected == DG_PLAYER_VEHICLE_PANEL_BONNET || selected == DG_PLAYER_VEHICLE_PANEL_BOOT ) UpdateVehicleVisuals(vid);
			
			cmd_pojazd(playerid, "");
		}
		
		case DIALOG_PLAYER_ITEMS:
		{
			PlayerTextDrawHide(playerid, pInfo[playerid][Dashboard]);
			new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);
			if( !response && dg_value == DG_NO_ACTION ) return 1;
			
			if(dg_value == DG_ITEMS_SEARCH)
			{
				if(!response) return 1;

				cmd_p(playerid, "podnies");
			}

			if(dg_value == DG_ITEMS_FAVORITES)
			{
				if(!response) return 1;

				ShowPlayerFavoriteItems(playerid);

				if(!pInfo[playerid][player_list_favorite_items]) Alert(playerid, ALERT_TYPE_INFO, "Jezeli chcesz ustawic ulubione przedmioty jako ~y~glowny widok ~w~mozesz ustawic to w ~y~/stats~w~.");
			}

			if(dg_value == DG_ITEMS_SELECTING)
			{
				if(!response)
				{
					if(pInfo[playerid][player_items_selecting]) return ShowPlayerMoreOptions(playerid, 0);
					else return 1;
				}

				pInfo[playerid][player_items_selecting] = !pInfo[playerid][player_items_selecting];

				if(pInfo[playerid][player_items_selecting] == true) Alert(playerid, ALERT_TYPE_INFO, "Rozpoczynasz ~y~zaznaczanie ~w~przedmiotow. Aby dodac przedmiot do zaznaczania kliknij na niego i wybierz opcje ~y~Zaznacz~w~. Aby zakonczyc wybierz ponownie opcje ~y~Funkcja zaznaczania~w~.");
				else
				{
					foreach(new i : Items)
					{
						if(Item[i][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[i][item_owner] == pInfo[playerid][player_id] && Item[i][item_selected] == true)
						{
							Item[i][item_selected] = false;
						}
					}

					pInfo[playerid][player_selected_items] = 0;
					Alert(playerid, ALERT_TYPE_INFO, "Zakonczyles zaznaczanie przedmiotow. Odznaczono przedmioty.");
				}

				ShowPlayerItems(playerid);
			}

			if( response && dg_value == DG_ITEMS_VIEW_ALL )
			{
				ShowPlayerItems(playerid);
			}

			if( response && dg_value == DG_ITEMS_ITEM_ROW )
			{
				if(pInfo[playerid][player_items_selecting] == true)
				{
					if(!Item[dg_data][item_selected])
					{
						if(Item[dg_data][item_used])
						{
							Alert(playerid, ALERT_TYPE_NEGATIVE, "~r~Nie mozesz ~w~dodac przedmiotu, ktory jest aktualnie uzywany!");
							ShowPlayerItems(playerid);
							return 1;
						}

						Alert(playerid, ALERT_TYPE_SUCCESS, sprintf("~g~Dodales ~w~do zaznaczania ~y~%s~w~.", Item[dg_data][item_name]));
						Item[dg_data][item_selected] = true;
						pInfo[playerid][player_selected_items]++;
						ShowPlayerItems(playerid);
					}
					else
					{
						Alert(playerid, ALERT_TYPE_SUCCESS, sprintf("~r~Usunales ~w~z zaznaczania ~y~%s~w~.", Item[dg_data][item_name]));
						Item[dg_data][item_selected] = false;
						pInfo[playerid][player_selected_items]--;
						ShowPlayerItems(playerid);
					}
				}
				else
				{
					// Use item
					Item_Use(dg_data, playerid);
				}
			}
			
			if( !response && dg_value == DG_ITEMS_ITEM_ROW )
			{
				ShowPlayerMoreOptions(playerid, dg_data);
			}
		}
		
		case DIALOG_PUT_IN_BAG:
		{
			if(!response) return 1;
			new dg_value = DynamicGui_GetValue(playerid, listitem);
			new itemid = pInfo[playerid][player_dialog_tmp1];

			if(Item[itemid][item_used]) return SendGuiInformation(playerid, "Wystpi bd", "Przedmiot jest aktualnie uywany!");

			Item[itemid][item_owner_type] = ITEM_OWNER_TYPE_BAG;
			Item[itemid][item_owner] = Item[dg_value][item_uid];

			mysql_query(g_sql, sprintf("UPDATE crp_items SET item_ownertype = 9, item_owner = %d WHERE item_uid = %d", Item[dg_value][item_uid], Item[itemid][item_uid]));
			SendClientMessage(playerid, -1, sprintf("UPDATE crp_items SET item_ownertype = %d, item_owner = %d WHERE item_uid = %d", ITEM_OWNER_TYPE_BAG, Item[dg_value][item_uid], Item[itemid][item_uid]));

			ProxMessage(playerid, "wkada co do torby", PROX_AME);
		}

		case DIALOG_ITEM_MORE:
		{
			new dg_value = DynamicGui_GetValue(playerid, listitem), itemid = DynamicGui_GetDataInt(playerid, listitem);
			if( !response ) return 1;
			
			new suffix[256];

			if(dg_value == DG_ITEMS_MORE_FAVORITE)
			{
				Item[itemid][item_selected] = false;
				Item[itemid][item_favorite] = !Item[itemid][item_favorite];

				mysql_tquery(g_sql, sprintf("UPDATE crp_items SET item_favorite = %d WHERE item_uid = %d", Item[itemid][item_favorite], Item[itemid][item_uid]));

				if(Item[itemid][item_favorite]) SendGuiInformation(playerid, "Sukces", sprintf("Pomylnie dodano przedmiot %s (%d) do ulubionych.", Item[itemid][item_name], Item[itemid][item_uid]));
				else SendGuiInformation(playerid, "Sukces", sprintf("Pomylnie usunito przedmiot %s (%d) z ulubionych.", Item[itemid][item_name], Item[itemid][item_uid]));
			}

			if(dg_value == DG_ITEMS_MORE_PUT_IN_BAG)
			{
				DynamicGui_Init(playerid);

				pInfo[playerid][player_dialog_tmp1] = itemid;

				new str[512];
				foreach(new i : Items)
				{
					if(Item[i][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[i][item_owner] == pInfo[playerid][player_id] && Item[i][item_type] == ITEM_TYPE_BAG)
					{
						format(str, sizeof(str), "%s%d\t%s", str, Item[i][item_uid], Item[i][item_name]);
						DynamicGui_AddRow(playerid, i);
					} 
				}

				ShowPlayerDialog(playerid, DIALOG_PUT_IN_BAG, DIALOG_STYLE_TABLIST, "Wybierz torb", str, "Wybierz", "Anuluj");
			}

			if(dg_value == DG_MULTIITEMS_FAVORITES)
			{
				foreach(new i : Items)
				{
					if(Item[i][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[i][item_owner] == pInfo[playerid][player_id] && Item[i][item_selected] == true)
					{
						Item[i][item_selected] = false;
						Item[i][item_favorite] = true;
						format(suffix, sizeof(suffix), "%sitem_uid = %d OR ", suffix, Item[i][item_uid]);
					}
				}	

				strdel(suffix, strlen(suffix)-4, strlen(suffix));
				mysql_tquery(g_sql, sprintf("UPDATE crp_items SET item_favorite = 1 WHERE %s", suffix));

				pInfo[playerid][player_selected_items] = 0;
				pInfo[playerid][player_items_selecting] = false;
				SendGuiInformation(playerid, "Sukces", "Pomylnie dodano przedmioty do ulubionych.");
			}

			if( dg_value == DG_MULTIITEMS_MORE_DROPG )
			{
				foreach(new i : Items)
				{
					if(Item[i][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[i][item_owner] == pInfo[playerid][player_id] && Item[i][item_selected] == true)
					{	
						Item[i][item_selected] = false;
						Item_Drop(i, playerid, true);
					}
				}

				ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
				pInfo[playerid][player_items_selecting] = false;
				pInfo[playerid][player_selected_items] = 0;
				ProxMessage(playerid, "odkada kilka przedmiotw na ziemi", PROX_AME);
			}

			if( dg_value == DG_MULTIITEMS_MORE_PUT_IN_DOOR )
			{
				new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));

				if( d_id > -1 )
				{
					foreach(new i : Items)
					{
						if(Item[i][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[i][item_owner] == pInfo[playerid][player_id] && Item[i][item_selected] == true)
						{	
							Item[i][item_owner_type] = ITEM_OWNER_TYPE_DOOR;
							Item[i][item_owner] = Door[d_id][door_uid];

							mysql_tquery(g_sql, sprintf("UPDATE `crp_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_uid` = %d", Item[i][item_owner_type], Item[i][item_owner], Item[i][item_uid]));

							PlayerLog(sprintf("Put an item %s inside door %s storage", ItemLogLink(Item[i][item_uid]), DoorLogLink(Door[d_id][door_uid])), pInfo[playerid][player_id], "item");
						}
					}

					ProxMessage(playerid, "wkada kilka rzeczy do schowka.", PROX_AME);
					ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
				}
			}

			if( dg_value == DG_MULTIITEMS_MORE_INFO )
			{
				new string[1024];

				format(string, sizeof(string), "Informacje o przedmiotach...\n\n");
				foreach(new i : Items)
				{
					if(Item[i][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[i][item_owner] == pInfo[playerid][player_id] && Item[i][item_selected] == true)
					{
						new status_str[40], created_str[40];
						
						if( Item[itemid][item_used] ) format(status_str, sizeof(status_str), "Tak");
						else format(status_str, sizeof(status_str), "Nie");
						
						GetRelativeDate(Item[i][item_created], created_str);

						format(string, sizeof(string), "%sIdentyfikator: %d\n", string, Item[i][item_uid]);
						format(string, sizeof(string), "%sTyp: %d\n", string, ItemTypes[Item[i][item_type]]);
						format(string, sizeof(string), "%sW uyciu: %d\n", string, status_str);
						format(string, sizeof(string), "%sWartoci: %d:%d\n", string, Item[i][item_value1], Item[i][item_value2]);
						format(string, sizeof(string), "%sIlo: %d\n", string, Item[i][item_amount]);
						format(string, sizeof(string), "%sGrupa: %d\n", string, Item[i][item_group]);
						format(string, sizeof(string), "%sUtworzony: %s\n", string, created_str);
						format(string, sizeof(string), "%s\n", string);
					}
				}

				ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacje o przedmiotach", string, "Zamknij", "");
			}

			if( dg_value == DG_ITEMS_MORE_DROPG )
			{
				// Drop item to the ground
				Alert(playerid, ALERT_TYPE_INFO, sprintf("Mozesz zrobic to szybciej uzywajac ~y~/p %s ~y~odloz~w~.", Item[itemid][item_name]));

				Item_Drop(itemid, playerid);
			}
			
			if( dg_value == DG_ITEMS_MORE_PUT_IN_DOOR )
			{
				new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
				if( d_id > -1 )
				{
					Item[itemid][item_owner_type] = ITEM_OWNER_TYPE_DOOR;
					Item[itemid][item_owner] = Door[d_id][door_uid];
					
					ProxMessage(playerid, "wkada co do schowka.", PROX_AME);
					
					mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_uid` = %d", Item[itemid][item_owner_type], Item[itemid][item_owner], Item[itemid][item_uid]));
					
					ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);

					PlayerLog(sprintf("Put an item %s inside door %s storage", ItemLogLink(Item[itemid][item_uid]), DoorLogLink(Door[d_id][door_uid])), pInfo[playerid][player_id], "item");
				}
			}
			else if( dg_value == DG_ITEMS_MORE_SELL )
			{
				if( Item[itemid][item_used] ) return SendGuiInformation(playerid, "Wystpi bd", "Nie moesz oferowa uywanego przedmiotu.");
				
				pInfo[playerid][player_dialog_tmp2] = itemid;
				ShowPlayerDialog(playerid, DIALOG_ITEMS_MORE_SELL_PRICE, DIALOG_STYLE_INPUT, "Sprzedawanie przedmiotu", "W poniszym polu podaj cen za jak chcesz sprzeda wybrany przedmiot:", "Dalej", "Anuluj");
			}
			else if( dg_value == DG_ITEMS_MORE_INFO )
			{
				pInfo[playerid][player_dialog_tmp2] = itemid;
				
				new str[300], status_str[40], created_str[40];
				
				if( Item[itemid][item_used] ) format(status_str, sizeof(status_str), "Tak");
				else format(status_str, sizeof(status_str), "Nie");
				
				GetRelativeDate(Item[itemid][item_created], created_str);
				
				format(str, sizeof(str), "Identyfikator:\t\t%d\nTyp:\t\t\t%s\nW uyciu:\t\t\t%s\n\nWaciwo 1:\t\t%d\nWaciwo 2:\t\t%d\nIlo:\t\t\t%d\n\nGrupa:\t\t\t%d\nUtworzony:\t\t%s", Item[itemid][item_uid], ItemTypes[Item[itemid][item_type]], status_str, Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_amount], Item[itemid][item_group], created_str);
				
				ShowPlayerDialog(playerid, DIALOG_ITEMS_MORE_INFO, DIALOG_STYLE_MSGBOX, sprintf("Informacje o przedmiocie  %s", Item[itemid][item_name]), str, "Wr", "");
			}
			else if( dg_value == DG_ITEMS_MORE_DRUGS_DIVIDE )
			{
				pInfo[playerid][player_dialog_tmp2] = itemid;
				new str[150];
				format(str, sizeof(str), "Wpisz ile sztuk przedmiotu chcesz wydzieli (max %d)", Item[itemid][item_amount]-1);

				ShowPlayerDialog(playerid, DIALOG_ITEMS_DRUGS_DIVIDE, DIALOG_STYLE_INPUT, sprintf("Dzielenie przedmiotu  %s", Item[itemid][item_name]), str, "Podziel", "Anuluj");
			}
			else if( dg_value == DG_ITEMS_MORE_DRUGS_JOIN )
			{
				pInfo[playerid][player_dialog_tmp5] = itemid;

				new str[600], count = 0;

				DynamicGui_Init(playerid);

				for(new item=0;item<MAX_ITEMS;item++)
				{
					if(!Iter_Contains(Items, item)) continue;

					if( Item[item][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[item][item_owner] != pInfo[playerid][player_id] ) continue;
					if( item == itemid ) continue;
					if( Item[item][item_type] != Item[itemid][item_type] || Item[item][item_value1] != Item[itemid][item_value1] || Item[item][item_value2] != Item[itemid][item_value2] ) continue;

					if(Item[item][item_type] == ITEM_TYPE_DRUGS) format(str, sizeof(str), "%s%s (%dg) [UID: %d]\n", str, Item[item][item_name], Item[item][item_amount], Item[item][item_uid]);
					else if(Item[item][item_type] == ITEM_TYPE_DRUG_INGR) format(str, sizeof(str), "%s%s (%dszt) [UID: %d]\n", str, Item[item][item_name], Item[item][item_amount], Item[item][item_uid]);
					DynamicGui_AddRow(playerid, item);
					count++;
				}

				if(!count)
				{
					DynamicGui_Init(playerid);
				
					DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, pInfo[playerid][player_dialog_tmp5]);
					pInfo[playerid][player_dialog] = DIALOG_PLAYER_ITEMS;
					OnDialogResponse(playerid, DIALOG_PLAYER_ITEMS, 0, 0, "");

					return 1;
				}

				ShowPlayerDialog(playerid, DIALOG_ITEMS_DRUGS_JOIN, DIALOG_STYLE_LIST, sprintf("czenie przedmiotu  %s", Item[itemid][item_name]), str, "Pocz", "Wr");
			}
		}
		
		case DIALOG_ITEMS_MORE_INFO:
		{
			DynamicGui_Init(playerid);
			
			DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, pInfo[playerid][player_dialog_tmp2]);
			pInfo[playerid][player_dialog] = DIALOG_PLAYER_ITEMS;
			OnDialogResponse(playerid, DIALOG_PLAYER_ITEMS, 0, 0, "");
			
			return 1;
		}
		
		case DIALOG_ITEMS_MORE_SELL_PRICE:
		{
			if( !response )
			{
				DynamicGui_Init(playerid);
				
				DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, pInfo[playerid][player_dialog_tmp2]);
				pInfo[playerid][player_dialog] = DIALOG_PLAYER_ITEMS;
				OnDialogResponse(playerid, DIALOG_PLAYER_ITEMS, 0, 0, "");
				
				return 1;
			}
			
			new price;
			if( sscanf(inputtext, "d", price) ) return ShowPlayerDialog(playerid, DIALOG_ITEMS_MORE_SELL_PRICE, DIALOG_STYLE_INPUT, "Sprzedawanie przedmiotu", "W poniszym polu podaj cen za jak chcesz sprzeda wybrany przedmiot:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae poprawnej ceny.", "Dalej", "Anuluj");
			if( price < 0 ) return ShowPlayerDialog(playerid, DIALOG_ITEMS_MORE_SELL_PRICE, DIALOG_STYLE_INPUT, "Sprzedawanie przedmiotu", "W poniszym polu podaj cen za jak chcesz sprzeda wybrany przedmiot:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae poprawnej ceny.", "Dalej", "Anuluj");
			
			pInfo[playerid][player_dialog_tmp1] = price;
			
			DynamicGui_Init(playerid);
			new string[2048], count;
			
			new Float:p_pos[3];
			GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
			
			foreach(new p : Player)
			{
				if( !pInfo[p][player_logged] ) continue;
				if( p == playerid ) continue;
				if( pInfo[p][player_admin_spec] ) continue;
				if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 10.0 )
				{
					if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASK) > -1 ) format(string, sizeof(string), "%s##\t\t%s\n", string, pInfo[p][player_name]);
					else format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);
					
					DynamicGui_AddRow(playerid, p);
					count++;
				}
			}
			
			if( count == 0 ) SendGuiInformation(playerid, "Wystpi bd", "W pobliu nie ma adnych osb.");
			else ShowPlayerDialog(playerid, DIALOG_ITEMS_MORE_SELL, DIALOG_STYLE_LIST, "Osoby znajdujce si w pobliu:", string, "Wybierz", "Anuluj");
		}
		
		case DIALOG_ITEMS_MORE_SELL:
		{
			if( !response ) return 1;
			
			new targetid = DynamicGui_GetValue(playerid, listitem);
			
			if( !IsPlayerConnected(targetid) ) return 1;
			if( !pInfo[targetid][player_logged] ) return 1;

			new resp = SetOffer(playerid, targetid, OFFER_TYPE_ITEM, pInfo[playerid][player_dialog_tmp1], pInfo[playerid][player_dialog_tmp2]);
			
			new itemid = pInfo[playerid][player_dialog_tmp2];
			new name[150];
			format(name, sizeof(name), "%s", Item[itemid][item_name]);

			if(Item[itemid][item_type] == ITEM_TYPE_DRUGS) {
				format(name, sizeof(name), "%s (%dg)", Item[itemid][item_name], Item[itemid][item_amount]);
			}
			else if(Item[itemid][item_type] == ITEM_TYPE_DRUG_INGR) {
				format(name, sizeof(name), "%s (%dszt)", Item[itemid][item_name], Item[itemid][item_amount]);
			}

			if( resp ) ShowPlayerOffer(targetid, playerid, "Przedmiot", sprintf("%s [%d:%d:%d]", name, Item[itemid][item_type], Item[itemid][item_value1], Item[itemid][item_value2]), pInfo[playerid][player_dialog_tmp1]);
		}
		
		case DIALOG_ITEMS_PICKUP:
		{
			new dg_value = DynamicGui_GetValue(playerid, listitem), itemid = DynamicGui_GetDataInt(playerid, listitem);
			if( !response ) return 1;
			
			if( dg_value == DG_ITEMS_PICKUP_ROW )
			{
				Item_Pickup(itemid, playerid);
			}
		}
		
		case DIALOG_USE_AMMO:
		{
			if( !response ) return 1;
			
			new ammoid = DynamicGui_GetDialogValue(playerid), itemid = DynamicGui_GetValue(playerid, listitem);
			
			Item[itemid][item_value2] += Item[ammoid][item_value2];
			mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_value2` = %d WHERE `item_uid` = %d", Item[itemid][item_value2], Item[itemid][item_uid]));
			
			SendGuiInformation(playerid, "Informacja", sprintf("Zaadowae %d naboi do broni %s [UID: %d].", Item[ammoid][item_value2], Item[itemid][item_name], Item[itemid][item_uid]));
			
			PlayerLog(sprintf("Put %d ammo inside weapon %s ", Item[ammoid][item_value2], ItemLogLink(Item[itemid][item_uid])), pInfo[playerid][player_id], "item");

			DeleteItem(ammoid, true);
		}
		
		case DIALOG_PHONE:
		{
			if( !response ) return 1;
			
			new dg_value = DynamicGui_GetValue(playerid, listitem), itemid = DynamicGui_GetDialogValue(playerid);
			
			if( dg_value == DG_PHONE_TURNOFF )
			{
				Item[itemid][item_used] = false;
				mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_used` = 0 WHERE `item_uid` = %d", Item[itemid][item_uid]));
				
				GameTextForPlayer(playerid, "~w~Telefon ~r~wylaczony", 3000, 3);

				PlayerLog(sprintf("Turns off phone %s", ItemLogLink(Item[itemid][item_uid])), pInfo[playerid][player_id], "item");
			}
			else if( dg_value == DG_PHONE_CALL )
			{
				ShowPlayerDialog(playerid, DIALOG_PHONE_CALL_NUMBER, DIALOG_STYLE_INPUT, "Wybieranie numeru", "W poniszym polu podaj numer telefonu, z ktrym chcesz si poczy:", "Dalej", "Zamknij");
			}
			else if( dg_value == DG_PHONE_SMS )
			{
				ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_NUMBER, DIALOG_STYLE_INPUT, "Wysyanie SMS'a", "W poniszym polu podaj numer telefonu, na ktrych chcesz wysa SMS'a:", "Dalej", "Zamknij");
			}
			else if( dg_value == DG_PHONE_CONTACTS )
			{
				DynamicGui_Init(playerid);
				new string[2048];
				
				format(string, sizeof(string), "%s911\tNumer alarmowy\n", string);
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_BASE, 911);
				
				format(string, sizeof(string), "%s333\tHurtownia\n", string);
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_BASE, 333);
				
				format(string, sizeof(string), "%s777\tTaxi\n", string);
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_BASE, 777);
				
				format(string, sizeof(string), "%s444\tLos Santos News\n", string);
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_BASE, 444);
				
				format(string, sizeof(string), "%s-----\n", string);
				DynamicGui_AddBlankRow(playerid);
				
				new Cache:result;
				result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_contacts` WHERE `contact_owner` = %d AND `contact_deleted` = 0", Item[itemid][item_uid]));

				if( cache_get_rows() == 0 ) SendGuiInformation(playerid, "Wystpi bd", "Nie posiadasz adnych zapisanych kontaktw.");
				else
				{
					for(new i;i<cache_get_rows();i++)
					{
						new tmp[MAX_PLAYER_NAME+1];
						cache_get(i, "contact_name", tmp);
						
						format(string, sizeof(string), "%s%d\t%s\n", string, cache_get_int(i, "contact_number"), tmp);
						DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_ROW, cache_get_int(i, "contact_uid"));
					}
				}
				
				cache_delete(result);
				
				ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS, DIALOG_STYLE_LIST, sprintf("%s [%d]  Kontakty", Item[itemid][item_name], Item[itemid][item_value1]), string, "Wybierz", "Zamknij");
			}
			else if( dg_value == DG_PHONE_VCARD )
			{
				DynamicGui_Init(playerid);
				new string[2048], count;
				
				new Float:p_pos[3];
				GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
				
				foreach(new p : Player)
				{
					if( !pInfo[p][player_logged] ) continue;
					if( p == playerid ) continue;
					if( pInfo[p][player_admin_spec] ) continue;
					if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 10.0 )
					{
						if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASK) > -1 ) format(string, sizeof(string), "%s##\t\t%s\n", string, pInfo[p][player_name]);
						else format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);
						
						DynamicGui_AddRow(playerid, p);
						count++;
					}
				}
				
				if( count == 0 ) SendGuiInformation(playerid, "Wystpi bd", "W pobliu nie ma adnych osb.");
				else ShowPlayerDialog(playerid, DIALOG_PHONE_VCARD, DIALOG_STYLE_LIST, "Osoby znajdujce si w pobliu:", string, "Wylij", "Zamknij");
			}
		}
		
		case DIALOG_PHONE_SMS_NUMBER:
		{
			if( !response ) return 1;
			
			new number;
			if( sscanf(inputtext, "d", number) ) return ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_NUMBER, DIALOG_STYLE_INPUT, "Wysyanie SMS'a", "W poniszym polu podaj numer telefonu, na ktrych chcesz wysa SMS'a:\n\n"HEX_COLOR_LIGHTER_RED"Podany numer jest bdny.", "Dalej", "Zamknij");
		
			pInfo[playerid][player_dialog_tmp1] = number;
			
			ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_TEXT, DIALOG_STYLE_INPUT, "Wysyanie SMS'a", "W poniszym polu podaj tre SMS'a:", "Wylij", "Zamknij");
		}
		
		case DIALOG_PHONE_SMS_TEXT:
		{
			if( !response ) return 1;
			
			if( isnull(inputtext) ) return ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_TEXT, DIALOG_STYLE_INPUT, "Wysyanie SMS'a", "W poniszym polu podaj tre SMS'a:\n\n"HEX_COLOR_LIGHTER_RED"Tre smsa nie moe by pusta.", "Wylij", "Zamknij");

			cmd_sms(playerid, sprintf("%d %s", pInfo[playerid][player_dialog_tmp1], inputtext));
		}
		
		case DIALOG_PHONE_CALL_NUMBER:
		{
			if( !response ) return 1;
			
			new number;
			if( sscanf(inputtext, "d", number) ) return ShowPlayerDialog(playerid, DIALOG_PHONE_CALL_NUMBER, DIALOG_STYLE_INPUT, "Wybieranie numeru", "W poniszym polu podaj numer telefonu, z ktrym chcesz si poczy:\n\n"HEX_COLOR_LIGHTER_RED"Podany numer jest bdny.", "Dalej", "Zamknij");

			cmd_call(playerid, sprintf("%d", number));
		}
		
		case DIALOG_PHONE_CONTACTS:
		{
			if( !response ) return 1;
			
			new dg_value = DynamicGui_GetValue(playerid, listitem), dg_data = DynamicGui_GetDataInt(playerid, listitem);
			
			if( dg_value == DG_PHONE_CONTACTS_BASE )
			{
				cmd_call(playerid, sprintf("%d", dg_data));
			}
			else if( dg_value == DG_PHONE_CONTACTS_ROW )
			{
				new Cache:result;
				result = mysql_query(g_sql, sprintf("SELECT contact_name, contact_number FROM `crp_contacts` WHERE `contact_uid` = %d", dg_data));
				
				new tmp[MAX_PLAYER_NAME+1];
				cache_get(0, "contact_name", tmp);
				
				pInfo[playerid][player_dialog_tmp1] = dg_data;
				ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS_ROW, DIALOG_STYLE_LIST, sprintf("Kontakt %s [%d]", tmp, cache_get_int(0, "contact_number")), "01\tZadzwo\n02\tWylij SMS'a\n03\tZmie nazw\n04\tUsu", "Wybierz", "Zamknij");
				
				cache_delete(result);
			}
		}
		
		case DIALOG_PHONE_VCARD:
		{
			if( !response ) return 1;
			
			new targetid = DynamicGui_GetValue(playerid, listitem);
			
			if( !IsPlayerConnected(targetid) ) return 1;
			if( !pInfo[targetid][player_logged] ) return 1;

			new resp = SetOffer(playerid, targetid, OFFER_TYPE_VCARD, 0, GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE));
			
			if( resp ) ShowPlayerOffer(targetid, playerid, "vCard", sprintf("vCard %s [%d]", pInfo[playerid][player_name], Item[pOffer[targetid][offer_extraid]][item_value1]), 0);
		}
		
		case DIALOG_PHONE_CONTACTS_ROW:
		{
			if( !response )
			{
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE));
				
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS);
				pInfo[playerid][player_dialog] = DIALOG_PHONE;
				OnDialogResponse(playerid, DIALOG_PHONE, 1, 0, "");
				
				return 1;
			}

			new Cache:result;

			if( listitem == 0 )
			{
				result = mysql_query(g_sql, sprintf("SELECT contact_number FROM `crp_contacts` WHERE `contact_uid` = %d", pInfo[playerid][player_dialog_tmp1]));

				cmd_call(playerid, sprintf("%d", cache_get_int(0, "contact_number")));

				cache_delete(result);
			}
			else if( listitem == 1 )
			{
				result = mysql_query(g_sql, sprintf("SELECT contact_number FROM `crp_contacts` WHERE `contact_uid` = %d", pInfo[playerid][player_dialog_tmp1]));
				
				pInfo[playerid][player_dialog_tmp1] = cache_get_int(0, "contact_number");

				cache_delete(result);

				ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_TEXT, DIALOG_STYLE_INPUT, "Wysyanie SMS'a", "W poniszym polu podaj tre SMS'a:", "Wylij", "Zamknij");
			}
			else if( listitem == 2 )
			{
				// zmien nazwe
				ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS_ROW_NAME, DIALOG_STYLE_INPUT, "Zmiana nazwy kontaktu", "W ponisze pole wpisz now nazw kontaktu (max 24 znaki):", "Gotowe", "Zamknij");
			}
			else
			{
				// usun
				mysql_query(g_sql, sprintf("UPDATE `crp_contacts` SET `contact_deleted` = 1 WHERE `contact_uid` = %d", pInfo[playerid][player_dialog_tmp1]), false);
				SendPlayerInformation(playerid, "Kontakt zostal ~r~usuniety~w~.", 5000);
				
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE));
				
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS);
				pInfo[playerid][player_dialog] = DIALOG_PHONE;
				OnDialogResponse(playerid, DIALOG_PHONE, 1, 0, "");
			}
		}
		
		case DIALOG_PHONE_CONTACTS_ROW_NAME:
		{
			if( !response )
			{
				DynamicGui_Init(playerid);
				
				DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_ROW, pInfo[playerid][player_dialog_tmp1]);
				pInfo[playerid][player_dialog] = DIALOG_PHONE_CONTACTS;
				OnDialogResponse(playerid, DIALOG_PHONE_CONTACTS, 1, 0, "");
				return 1;
			}
			
			if( strlen(inputtext) < 2 ) return ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS_ROW_NAME, DIALOG_STYLE_INPUT, "Zmiana nazwy kontaktu", "W ponisze pole wpisz now nazw kontaktu (max 24 znaki):\n\n"HEX_COLOR_LIGHTER_RED"Podana nazwa jest za krtka.", "Gotowe", "Zamknij");
			if( strlen(inputtext) > 24 ) return ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACTS_ROW_NAME, DIALOG_STYLE_INPUT, "Zmiana nazwy kontaktu", "W ponisze pole wpisz now nazw kontaktu (max 24 znaki):\n\n"HEX_COLOR_LIGHTER_RED"Podana nazwa jest zbyt duga.", "Gotowe", "Zamknij");
			
			new text[260];
			mysql_escape_string(inputtext, text);
			
			mysql_query(g_sql, sprintf("UPDATE `crp_contacts` SET `contact_name` = '%s' WHERE `contact_uid` = %d", text, pInfo[playerid][player_dialog_tmp1]), false);
			SendPlayerInformation(playerid, "Nazwa kontaktu zostala ~g~zmieniona~w~.", 5000);

			DynamicGui_Init(playerid);
				
			DynamicGui_AddRow(playerid, DG_PHONE_CONTACTS_ROW, pInfo[playerid][player_dialog_tmp1]);
			pInfo[playerid][player_dialog] = DIALOG_PHONE_CONTACTS;
			OnDialogResponse(playerid, DIALOG_PHONE_CONTACTS, 1, 0, "");
		}
		
		case DIALOG_WORKS:
		{
			if( !response ) return SendClientMessage(playerid, 0xD8D8D8FF, "Urzdnik mwi: Przykro mi, e nic Pan nie wybra. Zapraszam ponownie!");
			
			pInfo[playerid][player_job] = DynamicGui_GetValue(playerid, listitem);
			
			mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_job` = %d WHERE `char_uid` = %d", pInfo[playerid][player_job], pInfo[playerid][player_id]));
			
			SendClientMessage(playerid, 0xD8D8D8FF, "Urzdnik mwi: wietny wybr! Powodzenia!");
		}
		
		case DIALOG_PAYMENT:
		{
			if( !response ) return OnPlayerPaymentResponse(playerid, 0, 0);

			if( listitem == 1 )
			{
				new price = pOffer[playerid][offer_price];
				if( pInfo[playerid][player_bank_number] == 0 ) 
				{
					SendPlayerInformation(playerid, "Nie posiadasz ~r~konta~w~ w banku.", 4000);
					return ShowPlayerDialog(playerid, DIALOG_PAYMENT, DIALOG_STYLE_LIST, "Sposb patnoci", "Patno gotwk\nPatno kart kredytow", "Wybierz", "Anuluj");
				}
				
				if( pInfo[playerid][player_bank_money] < price ) 
				{
					SendPlayerInformation(playerid, "Nie posiadasz wystarczajacej ilosci ~r~pieniedzy~w~ na koncie.", 4000);
					return ShowPlayerDialog(playerid, DIALOG_PAYMENT, DIALOG_STYLE_LIST, "Sposb patnoci", "Patno gotwk\nPatno kart kredytow", "Wybierz", "Anuluj");
				}

				if( pInfo[playerid][player_debit] > 0 )
				{
					SendPlayerInformation(playerid, "Najpierw splac swoj dlug.", 4000);
					return ShowPlayerDialog(playerid, DIALOG_PAYMENT, DIALOG_STYLE_LIST, "Sposb patnoci", "Patno gotwk\nPatno kart kredytow", "Wybierz", "Anuluj");
				}
				
				AddPlayerBankMoney(playerid, -price, "Patno za usug");
				
				PlayerLog(sprintf("Made offer card payment {AMOUNT:%d}", -price), pInfo[playerid][player_id], "cash");

				OnPlayerPaymentResponse(playerid, 1, 1);
			}
			else
			{
				if( pInfo[playerid][player_money] < pOffer[playerid][offer_price] ) 
				{
					SendPlayerInformation(playerid, "Nie posiadasz wystarczajacej ilosci ~r~pieniedzy~w~ przy sobie.", 4000);
					return ShowPlayerDialog(playerid, DIALOG_PAYMENT, DIALOG_STYLE_LIST, "Sposb patnoci", "Patno gotwk\nPatno kart kredytow", "Wybierz", "Anuluj");
				}

				GivePlayerMoney(playerid, -pOffer[playerid][offer_price]);
				
				PlayerLog(sprintf("Made offer cash payment {AMOUNT:%d}", -pOffer[playerid][offer_price]), pInfo[playerid][player_id], "cash");

				OnPlayerPaymentResponse(playerid, 0, 1);
			}
		}
		
		case DIALOG_ORDER_PRODUCTS:
		{
			if( !response )
			{
				SetPlayerCellPhoneVisuals(playerid, false);
				return 1;
			}
			
			new p_uid = DynamicGui_GetDataInt(playerid, listitem);
			
			pInfo[playerid][player_order_pid] = p_uid;
			
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT product_listname, product_name FROM crp_products WHERE product_uid = %d", p_uid));

			new product_listname[100], pname[100];

			cache_get(0, "product_listname", product_listname);

			cache_get(0, "product_name", pname);	
			if( cache_is_null(0, "product_name") ) pname[0] = EOS;

			cache_delete(result);
			
			if( isnull(pname) )
			{
				// Prosimy o podanie nazwy produktu
				strcopy(pInfo[playerid][player_dialog_tmp_string], product_listname, 100);
				ShowPlayerDialog(playerid, DIALOG_ORDER_PRODUCTS_ITEM_NAME, DIALOG_STYLE_INPUT, "Zamawianie produktw  Ustalenie nazwy", sprintf("W poniszym polu podaj nazw pod ktr bdziesz sprzedawa produkt "HEX_COLOR_HONEST"%s"HEX_COLOR_SAMP":", product_listname), "Dalej", "Anuluj");
			}
			else
			{
				// Prosimy o podanie ilosci produktow
				strcopy(pInfo[playerid][player_order_name], pname, 100);
				ShowPlayerDialog(playerid, DIALOG_ORDER_PRODUCTS_ITEM_AMOUNT, DIALOG_STYLE_INPUT, "Zamawianie produktw  Ilo", "W poniszym polu podaj ilo jak chcesz zamwi:", "Dalej", "Anuluj");
			}
		}
		
		case DIALOG_ORDER_PRODUCTS_ITEM_NAME:
		{
			if( !response ) return cmd_call(playerid, "333");
			
			if( strlen(inputtext) < 4 ) return ShowPlayerDialog(playerid, DIALOG_ORDER_PRODUCTS_ITEM_NAME, DIALOG_STYLE_INPUT, "Zamawianie produktw  Ustalenie nazwy", sprintf("Podaj nazw pod ktr bdziesz sprzedawa produkt "HEX_COLOR_HONEST"%s"HEX_COLOR_SAMP":\n\n"HEX_COLOR_LIGHTER_RED"Nazwa produktu musi mie minimalnie 4 znaki.", pInfo[playerid][player_dialog_tmp_string]), "Dalej", "Anuluj");
			if( strlen(inputtext) > 40 ) return ShowPlayerDialog(playerid, DIALOG_ORDER_PRODUCTS_ITEM_NAME, DIALOG_STYLE_INPUT, "Zamawianie produktw  Ustalenie nazwy", sprintf("Podaj nazw pod ktr bdziesz sprzedawa produkt "HEX_COLOR_HONEST"%s"HEX_COLOR_SAMP":\n\n"HEX_COLOR_LIGHTER_RED"Nazwa produktu moe mie maksymalnie 40 znakw.", pInfo[playerid][player_dialog_tmp_string]), "Dalej", "Anuluj");
			
			strcopy(pInfo[playerid][player_order_name], inputtext, 40);
			
			ShowPlayerDialog(playerid, DIALOG_ORDER_PRODUCTS_ITEM_AMOUNT, DIALOG_STYLE_INPUT, "Zamawianie produktw  Ilo", "W poniszym polu podaj ilo jak chcesz zamwi:", "Dalej", "Anuluj");
		}
		
		case DIALOG_ORDER_PRODUCTS_ITEM_AMOUNT:
		{
			if( !response ) return cmd_call(playerid, "333");
			
			new amount;
			if( sscanf(inputtext, "d", amount) ) return ShowPlayerDialog(playerid, DIALOG_ORDER_PRODUCTS_ITEM_AMOUNT, DIALOG_STYLE_INPUT, "Zamawianie produktw  Ilo", "W poniszym polu podaj ilo jak chcesz zamwi:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae poprawnej iloci.", "Dalej", "Anuluj");
			if( amount < 1 ) return ShowPlayerDialog(playerid, DIALOG_ORDER_PRODUCTS_ITEM_AMOUNT, DIALOG_STYLE_INPUT, "Zamawianie produktw  Ilo", "W poniszym polu podaj ilo jak chcesz zamwi:\n\n"HEX_COLOR_LIGHTER_RED"Musisz zamwi przynajmniej 1 produkt.", "Dalej", "Anuluj");
			
			new did = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			new gid = GetGroupByUid(Door[did][door_owner]); 

			if( gid == -1 ) 
			{
				SetPlayerCellPhoneVisuals(playerid, false);
				return 1;
			}

			mysql_pquery(g_sql, sprintf("SELECT product_price, product_type, product_model, product_value1, product_value2 FROM crp_products WHERE product_uid = %d", pInfo[playerid][player_order_pid]), "OnGroupProductsOrderDataLoaded", "dd", playerid, amount);
		}
		
		case DIALOG_24/7_BUY:
		{
			if( !response ) return 1;
			
			new itemid = GetItemByUid(DynamicGui_GetValue(playerid, listitem));
			if( itemid == -1 ) return SendGuiInformation(playerid, "Wystpi bd", "Tego przedmiotu nie ma ju w magazynie.");
			
			new d_id = DynamicGui_GetDialogValue(playerid);
			new gid = GetGroupByUid(Door[d_id][door_owner]);

			new price = floatround(Item[itemid][item_price]*SHOP_PRICE_MULTIPLIER);
			
			if( pInfo[playerid][player_money] < price ) return SendGuiInformation(playerid, "Wystpi bd", "Nie masz wystarczajcej iloci pienidzy.");
			
			GivePlayerMoney(playerid, -price);
			GiveGroupMoney(gid, price);
			
			Item[itemid][item_amount] -= 1;
			mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_amount` = %d WHERE `item_uid` = %d", Item[itemid][item_amount], Item[itemid][item_uid]));
			
			new created_itemid = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, Item[itemid][item_type], Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_name], Item[itemid][item_model]);
			
			GameTextForPlayer(playerid, "~g~Zakupiono", 3000, 3);
			
			PlayerLog(sprintf("Bought item %s in shop %s {PRICE:%d}", ItemLogLink(Item[created_itemid][item_uid]), DoorLogLink(Door[d_id][door_uid]), price), pInfo[playerid][player_id], "item");

			if( Item[itemid][item_amount] == 0 ) DeleteItem(itemid, true);
			
			return cmd_kup(playerid, "");
		}
		
		case DIALOG_NOTEPAD:
		{
			if( !response ) return 1;
			
			new itemid = pInfo[playerid][player_dialog_tmp1];
			
			if( isnull(inputtext) ) return ShowPlayerDialog(playerid, DIALOG_NOTEPAD, DIALOG_STYLE_INPUT, "Tworzenie notatki", "W poniszym polu podaj tre notatki:\n\n"HEX_COLOR_LIGHTER_RED"Tre nie moe by pusta.", "Gotowe", "Anuluj");
			
			new text[260], Cache:result;
			mysql_escape_string(inputtext, text);
			result = mysql_query(g_sql, sprintf("INSERT INTO `crp_chits` VALUES (null, '%s')", text));
			
			new chit_uid = cache_insert_id();

			cache_delete(result);
			
			new i_id = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_CHIT, chit_uid, 0, "Karteczka", 19469);
			
			SendGuiInformation(playerid, "Informacja", sprintf("Utworzye przedmiot Karteczka (UID: %d) przy pomocy %s (UID: %d).", Item[i_id][item_uid], Item[itemid][item_name], Item[itemid][item_uid]));

			Item[itemid][item_value1] -= 1;		
			
			if( Item[itemid][item_value1] == 0 ) DeleteItem(itemid, true);
			else mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_value1` = %d WHERE `item_uid` = %d", Item[itemid][item_value1], Item[itemid][item_uid]));	
		}
		
		case DIALOG_BANKOMAT_KONTO:
		{
			if( !response ) return 1;
			
			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_BANKOMAT_KONTO_STAN: // Stan konta
				{
					ShowPlayerDialog(playerid, DIALOG_BANKOMAT_KONTO_STAN, DIALOG_STYLE_MSGBOX, "Bankomat  Stan konta", sprintf("Aktualnie stan Twojego konta to: "HEX_COLOR_WHITE"$%d", pInfo[playerid][player_bank_money]), "Zamknij", "");
				}
				
				case DG_BANKOMAT_KONTO_WYPLAC: // Wyplata
				{
					ShowPlayerDialog(playerid, DIALOG_BANKOMAT_KONTO_WYPLAC, DIALOG_STYLE_INPUT, "Bankomat  Wypata", "W polu poniej podaj kwot, ktr chcesz wypaci z konta bankowego:", "Gotowe", "Zamknij");
				}
			}
		}
		
		case DIALOG_BANKOMAT_KONTO_STAN:
		{
			cmd_bankomat(playerid, "");
		}
		
		case DIALOG_BANKOMAT_KONTO_WYPLAC:
		{
			if( !response ) return cmd_bankomat(playerid, "");
			
			new amount;
			if( sscanf(inputtext, "d", amount) ) return ShowPlayerDialog(playerid, DIALOG_BANKOMAT_KONTO_WYPLAC, DIALOG_STYLE_INPUT, "Bankomat  Wypata", "W polu poniej podaj kwot, ktr chcesz wypaci z konta bankowego:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae kwoty.", "Gotowe", "Zamknij");
			if( amount <= 0 ) return ShowPlayerDialog(playerid, DIALOG_BANKOMAT_KONTO_WYPLAC, DIALOG_STYLE_INPUT, "Bankomat  Wypata", "W polu poniej podaj kwot, ktr chcesz wypaci z konta bankowego:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae poprawnej kwoty.", "Gotowe", "Zamknij");
			
			if( amount > pInfo[playerid][player_bank_money] ) return ShowPlayerDialog(playerid, DIALOG_BANKOMAT_KONTO_WYPLAC, DIALOG_STYLE_INPUT, "Bankomat  Wypata", "W polu poniej podaj kwot, ktr chcesz wypaci z konta bankowego:\n\n"HEX_COLOR_LIGHTER_RED"Na koncie nie ma tyle pienidzy.", "Gotowe", "Zamknij");

			if( pInfo[playerid][player_debit] > 0 ) return SendPlayerInformation(playerid, "Najpierw splac swoj dlug.", 4000);
			
			new cash_before = pInfo[playerid][player_money], bank_before = pInfo[playerid][player_bank_money];

			GivePlayerMoney(playerid, amount);
			AddPlayerBankMoney(playerid, -amount);

			PlayerLog(sprintf("Withdraws money from ATM {AMOUNT:%d,CASH_BEFORE:%d,CASH_AFTER:%d,BANK_BEFORE:%d,BANK_AFTER:%d}", amount, cash_before, pInfo[playerid][player_money], bank_before, pInfo[playerid][player_bank_money]), pInfo[playerid][player_id], "cash");

			SendFormattedClientMessage(playerid, COLOR_GOLD, "Wypacie ze swojego konta (%d) $%d pienidzy.", pInfo[playerid][player_bank_number], amount);			

			cmd_bankomat(playerid, "");
		}
		
		case DIALOG_BANK_ZALOZ_KONTO:
		{
			if( !response ) return 1;
			
			if( pInfo[playerid][player_bank_number] == 0 )
			{
				new bank_number[9];	
				format(bank_number, sizeof(bank_number), "4%d", pInfo[playerid][player_id]);
				
				if( strlen(bank_number) < 8 )
				{
					new length = 8-strlen(bank_number);
					for(new i;i<length;i++)
					{
						format(bank_number, sizeof(bank_number), "%s%d", bank_number, random(10));
					}
				}
				
				pInfo[playerid][player_bank_number] = strval(bank_number);
				
				mysql_query(g_sql, sprintf("UPDATE `crp_characters` SET `char_banknumb` = %d WHERE `char_uid` = %d", pInfo[playerid][player_bank_number], pInfo[playerid][player_id]), false);
				
				PlayerLog(sprintf("Opens bank account {NUMBER:%d}", pInfo[playerid][player_bank_number]), pInfo[playerid][player_id], "cash");

				ShowPlayerDialog(playerid, DIALOG_BANK_ZALOZ_KONTO, DIALOG_STYLE_MSGBOX, "Bank  Zakadanie konta", sprintf("Twoje konto bankowe zostao pomylnie zaoone.\n\nNumer konta: "HEX_COLOR_LIGHTER_RED"%d", pInfo[playerid][player_bank_number]), "OK", "");
			}
			else
			{
				cmd_bank(playerid, "");
			}
		}
		
		case DIALOG_BANK_KONTO:
		{
			if( !response ) return 1;
			
			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_BANK_KONTO_STAN: // Stan konta
				{
					ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_STAN, DIALOG_STYLE_MSGBOX, "Bank  Stan konta", sprintf("Aktualnie stan Twojego konta to: "HEX_COLOR_WHITE"$%d", pInfo[playerid][player_bank_money]), "Zamknij", "");
				}
				
				case DG_BANK_KONTO_WPLAC: // Wplata
				{
					ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_WPLAC, DIALOG_STYLE_INPUT, "Bank  Wpata", "W polu poniej podaj kwot, ktr chcesz wpaci na konto bankowe:", "Gotowe", "Zamknij");
				}
				
				case DG_BANK_KONTO_WYPLAC: // Wyplata
				{
					ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_WYPLAC, DIALOG_STYLE_INPUT, "Bank  Wypata", "W polu poniej podaj kwot, ktr chcesz wypaci z konta bankowego:", "Gotowe", "Zamknij");
				}
				
				case DG_BANK_KONTO_PRZELEW: // Przelew
				{
					if( GetPlayerOnlineTime(playerid) < 18000 ) return SendGuiInformation(playerid, "Wystpi bd", "Nie moesz dokonywa przeleww poniewa masz przegrane mniej ni 5h.");
					
					pInfo[playerid][player_dialog_tmp2] = 0;
					ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_PRZELEW, DIALOG_STYLE_INPUT, "Bank  Przelew [1/2]", "W polu poniej podaj numer konta bankowego, na ktre chcesz przela pienidze:", "Dalej", "Zamknij");
				}

				case DG_BANK_HISTORY: // Historia transakcji
				{
					ShowPlayerTransactionsHistory(playerid);
				}

				case DG_BANK_DEBIT:
				{
					if(pInfo[playerid][player_debit] <= 0) return SendGuiInformation(playerid, "Bd", "Nie posiadasz debetu lub ma on bdn kwot.");
					if(pInfo[playerid][player_debit] > pInfo[playerid][player_bank_money]) return SendGuiInformation(playerid, "Bd", "Nie posiadasz tylu pienidzy na koncie.");
				
					AddPlayerBankMoney(playerid, -pInfo[playerid][player_debit], "Splata debetu");

					SendGuiInformation(playerid, "Sukces", sprintf("Spacie swj debet $%d. Masz teraz na koncie $%d.", pInfo[playerid][player_debit], pInfo[playerid][player_bank_money]));

					AddPlayerDebit(playerid, -pInfo[playerid][player_debit], "Splata debetu");
				}
			}
		}
		
		case DIALOG_BANK_KONTO_STAN:
		{
			cmd_bank(playerid, "");
		}
		
		case DIALOG_BANK_KONTO_WPLAC:
		{
			if( !response ) return cmd_bank(playerid, "");	
			
			new amount;
			if( sscanf(inputtext, "d", amount) ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_WPLAC, DIALOG_STYLE_INPUT, "Bank  Wpata", "W polu poniej podaj kwot, ktr chcesz wpaci na konto bankowe:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae kwoty.", "Gotowe", "Zamknij");
			if( amount <= 0 ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_WPLAC, DIALOG_STYLE_INPUT, "Bank  Wpata", "W polu poniej podaj kwot, ktr chcesz wpaci na konto bankowe:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae poprawnej kwoty.", "Gotowe", "Zamknij");
			if( amount > pInfo[playerid][player_money] ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_WPLAC, DIALOG_STYLE_INPUT, "Bank  Wpata", "W polu poniej podaj kwot, ktr chcesz wpaci na konto bankowe:\n\n"HEX_COLOR_LIGHTER_RED"Nie posiadasz tyle pienidzy.", "Gotowe", "Zamknij");
			
			new cash_before = pInfo[playerid][player_money], bank_before = pInfo[playerid][player_bank_money];

			GivePlayerMoney(playerid, -amount);
			AddPlayerBankMoney(playerid, amount, "Wplata na konto");

			PlayerLog(sprintf("Deposit money to bank {AMOUNT:%d,CASH_BEFORE:%d,CASH_AFTER:%d,BANK_BEFORE:%d,BANK_AFTER:%d}", amount, cash_before, pInfo[playerid][player_money], bank_before, pInfo[playerid][player_bank_money]), pInfo[playerid][player_id], "cash");
			
			SendFormattedClientMessage(playerid, COLOR_GOLD, "Wpacie na swoje konto (%d) $%d pienidzy.", pInfo[playerid][player_bank_number], amount);
			cmd_bank(playerid, "");
		}
		
		case DIALOG_BANK_KONTO_WYPLAC:
		{
			if( !response ) return cmd_bank(playerid, "");
			
			new amount;
			if( sscanf(inputtext, "d", amount) ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_WYPLAC, DIALOG_STYLE_INPUT, "Bank  Wypata", "W polu poniej podaj kwot, ktr chcesz wypaci z konta bankowego:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae kwoty.", "Gotowe", "Zamknij");
			if( amount <= 0 ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_WYPLAC, DIALOG_STYLE_INPUT, "Bank  Wypata", "W polu poniej podaj kwot, ktr chcesz wypaci z konta bankowego:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae poprawnej kwoty.", "Gotowe", "Zamknij");
			
			if( amount > pInfo[playerid][player_bank_money] ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_WYPLAC, DIALOG_STYLE_INPUT, "Bank  Wypata", "W polu poniej podaj kwot, ktr chcesz wypaci z konta bankowego:\n\n"HEX_COLOR_LIGHTER_RED"Na koncie nie ma tyle pienidzy.", "Gotowe", "Zamknij");
			if( pInfo[playerid][player_debit] > 0 ) return SendPlayerInformation(playerid, "Najpierw splac swoj dlug.", 4000);
				
			new cash_before = pInfo[playerid][player_money], bank_before = pInfo[playerid][player_bank_money];

			GivePlayerMoney(playerid, amount);
			AddPlayerBankMoney(playerid, -amount);

			PlayerLog(sprintf("Withdraws money from bank {AMOUNT:%d,CASH_BEFORE:%d,CASH_AFTER:%d,BANK_BEFORE:%d,BANK_AFTER:%d}", amount, cash_before, pInfo[playerid][player_money], bank_before, pInfo[playerid][player_bank_money]), pInfo[playerid][player_id], "cash");

			SendFormattedClientMessage(playerid, COLOR_GOLD, "Wypacie ze swojego konta (%d) $%d pienidzy.", pInfo[playerid][player_bank_number], amount);
			
			cmd_bank(playerid, "");
		}
		
		case DIALOG_BANK_KONTO_PRZELEW:
		{
			if( !response ) return cmd_bank(playerid, "");
			
			if( pInfo[playerid][player_dialog_tmp2] == 0 )
			{
				new bank_number;
				if( sscanf(inputtext, "d", bank_number) ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_PRZELEW, DIALOG_STYLE_INPUT, "Bank  Przelew [1/2]", "W polu poniej podaj numer konta bankowego, na ktre chcesz przela pienidze:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae numeru konta.", "Dalej", "Zamknij");
				if( bank_number == pInfo[playerid][player_bank_number] ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_PRZELEW, DIALOG_STYLE_INPUT, "Bank  Przelew [1/2]", "W polu poniej podaj numer konta bankowego, na ktre chcesz przela pienidze:\n\n"HEX_COLOR_LIGHTER_RED"Nie moesz dokona przelewu na to konto.", "Dalej", "Zamknij");		
				
				// Konto osobiste
				new Cache:result;
				result = mysql_query(g_sql, sprintf("SELECT char_uid FROM `crp_characters` WHERE `char_banknumb` = %d", bank_number));
				
				new rows = cache_get_rows();

				cache_delete(result);

				if( rows == 0 ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_PRZELEW, DIALOG_STYLE_INPUT, "Bank  Przelew [1/2]", "W polu poniej podaj numer konta bankowego, na ktre chcesz przela pienidze:\n\n"HEX_COLOR_LIGHTER_RED"Konto o podanym numerze nie istnieje.", "Dalej", "Zamknij");
				
				pInfo[playerid][player_dialog_tmp2] = bank_number;
				ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_PRZELEW, DIALOG_STYLE_INPUT, "Bank  Przelew [2/2]", "W polu poniej podaj iloc pienidzy, ktre chcesz przela na podane konto:", "Dalej", "Zamknij");
			}
			else
			{	
				new amount;
				if( sscanf(inputtext, "d", amount) ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_PRZELEW, DIALOG_STYLE_INPUT, "Bank  Przelew [2/2]", "W polu poniej podaj iloc pienidzy, ktre chcesz przela na podane konto:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae kwoty.", "Dalej", "Zamknij");
				if( amount <= 0 ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_PRZELEW, DIALOG_STYLE_INPUT, "Bank  Przelew [2/2]", "W polu poniej podaj iloc pienidzy, ktre chcesz przela na podane konto:\n\n"HEX_COLOR_LIGHTER_RED"Nie podae poprawnej kwoty.", "Dalej", "Zamknij");
				
				if( amount > pInfo[playerid][player_bank_money] ) return ShowPlayerDialog(playerid, DIALOG_BANK_KONTO_PRZELEW, DIALOG_STYLE_INPUT, "Bank  Przelew [2/2]", "W polu poniej podaj iloc pienidzy, ktre chcesz przela na podane konto:\n\n"HEX_COLOR_LIGHTER_RED"Na koncie nie ma tyle pienidzy.", "Dalej", "Zamknij");

				if( pInfo[playerid][player_debit] > 0 ) return SendPlayerInformation(playerid, "Najpierw splac swoj dlug.", 4000);

				new bool:added = false;
				foreach(new p : Player)
				{
					if( pInfo[p][player_bank_number] == pInfo[playerid][player_dialog_tmp2] )
					{
						AddPlayerBankMoney(p, amount, sprintf("Przelew od %d", pInfo[playerid][player_bank_number]));
						added = true;
					}
				}
					
				if( !added )
				{
					mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_bankcash` = char_bankcash + %d WHERE `char_banknumb` = %d", amount, pInfo[playerid][player_dialog_tmp2]));
				}

				new bank_before = pInfo[playerid][player_bank_money];

				AddPlayerBankMoney(playerid, -amount, sprintf("Przelew na %d", pInfo[playerid][player_dialog_tmp2]));
				SendFormattedClientMessage(playerid, COLOR_GOLD, "Przelae ze swojego konta (%d) $%d pienidzy na konto (%d).", pInfo[playerid][player_bank_number], amount, pInfo[playerid][player_dialog_tmp2]);
				
				new Cache:result, name[MAX_PLAYER_NAME];
				result = mysql_query(g_sql, sprintf("SELECT char_name, char_uid FROM crp_characters WHERE char_banknumb = %d", pInfo[playerid][player_dialog_tmp2]));
				cache_get(0, "char_name", name);
				new uid = cache_get_int(0, "char_uid");
				cache_delete(result);

				UnderscoreToSpace(name);

				PlayerLog(sprintf("Transfer bank money to %s {AMOUNT:%d,BANK_BEFORE:%d,BANK_AFTER:%d}", PlayerLogLink(uid), amount, bank_before, pInfo[playerid][player_bank_money]), pInfo[playerid][player_id], "cash");
				PlayerLog(sprintf("%s you bank money {AMOUNT:%d}", PlayerLogLink(pInfo[playerid][player_id]), amount), uid, "cash");

				
				cmd_bank(playerid, "");
			}
		}
		
		case DIALOG_POMOC:
		{
			if( !response ) return 1;
			
			switch( pInfo[playerid][player_dialog_tmp1] )
			{
				case 1:
				{
					switch( listitem )
					{
						case 0:
						{
							pInfo[playerid][player_dialog_tmp1] = 2;
							
							new str[600];
							
							strcat(str, "Witaj na serwerze Honest RolePlay.\n\nPojawie si w jednym z kilku miejsc rozmieszczonych w ok Pershing Square, w ktrym znajdziesz wiele rznego rodzaju\ninstytucji");
							strcat(str, ", dziki ktrym bdziesz mg przyj si do pracy\na take skorzysta z ich usug np. kupi telefon, bd zje obiad.\n\nNa pocztku udaj si do urzdu miasta, aby wyrobi niezbdne dokumenty.");
							
							ShowPlayerDialog(playerid, DIALOG_POMOC, DIALOG_STYLE_MSGBOX, "Wprowadzenie [1/2]", str, "Dalej", "Zamknij");
						}

						case 1:
						{
							// komendy
							new str[2200];

							strcat(str, "/(a)dmins\tlista adminw online\n/pomoc\tpomoc\n/anim\tlista animacji\n/sprobuj\twiadomo\n/pokaz\tpokazywanie dokumentw\n/(g)rupy");
							strcat(str, "\tzarzdzanie grupami\n/mc\ttworzenie obiektu\n/msel\tzaznaczanie obiektu\n/mselid\tzaznaczanie obiektu po id\n/mdel\tusuwanie obiektu\n/mtype\tzmiana typu obiektu\n/mmat\tzmiana tekstury obiektu\n/rx\tobrt ");
							strcat(str, "obiektu w osi x\n/ry\tobrt obiektu w osi y\n/rz\tobrt obiektu w osi z\n/pz\tprzesuwanie obiektu w osi z\n/mgate\tzmiana obiektu w brame\n/brama\totwieranie/zamykanie bramy\n/ec\t/tworzenie 3d tekstu\n/esel\t");
							strcat(str, "zaznaczanie 3d tekstu\n/edel\tusuwanie 3d tekstu\n/k\tkrzyk\n/s\tkrzyk\n/c\tszept\n/ja\twiadomo\n/me\twiadomo\n/do\twiadomo\n/w\tprywatna wiadomosc\n/pm\tprywatna wiadomosc\n/silnik\todpalanie silnika\n/bank\tw");
							strcat(str, " banku\n/wyrzuc\twyrzucanie z pojazdu\n/bankomat\tprzy bankomacie\n/bus\tna przystanku\n/tog\tblokowanie czatw\n/re\todpowied na /w\n/qs\tquit-save\n/report\tzgaszanie gracza\n/pay\tpacenie\n/plac\tpacenie");
							strcat(str, " \n/login\trelog\n/l\tlokalny czat\n/b\tczat ooc\n/drzwi\tzarzdzanie drzwiami\n/tel\ttelefon\n/cennik\tgastro\n/podaj\tgastro\n/(o)feruj\toferowanie\n/kogut\tpolicja\n/blokada\tpolicja\n/alkomat\tpolicja\n/wrzu");
							strcat(str, " c\tpolicja\n/zabierzprawko\tpolicja\n/opis\topis postaci\n/sms\twysyanie sms\n/reanimuj\tpogotowie\n/(m)egafon\tmegafon\n/call\tdzwonienie\n/praca\tmenu pracy dorywczej\n/tankuj\ttankowanie\n/yo\tprzywitanie\n");
							strcat(str, " /przejazd\tprzejazd autem przez drzwi\n/przetrzymaj\tprzetrzymanie w drzwiach\n/aresztuj\tpolicja\n/pokoj\tpokoj w hotelu\n/(d)epartament\tczat sub pub.\n/v\tmenu pojazdu\n/pojazd\tmenu pojazdu\n/stats\tstatys");
							strcat(str, " tyki postaci\n/dom\tmenu domu\n/mieszkanie\tmenu mieszkania\n/id\tsprawdzanie id gracza\n/pasy\tzapinanie pasw\n/live\tlsn\n/wywiad\tlsn\n/reklama\tlsn\n/kup\tkupowanie w sklepie\n/kupauto\tkupowanie auta w salo");
							strcat(str, " nie\n/przymierz\tkupowanie w odzieowym\n/dodatki\tkupowanie w odzieowym\n/akceptujsmierc\twiadomo\n/(p)rzedmioty\tmenu przedmiotw\n/kartoteka\tpolicja\n/przeszukaj\tprzeszukiwanie\n");

							ShowPlayerDialog(playerid, DIALOG_POMOC, DIALOG_STYLE_TABLIST, "Komendy gwne", str, "Dalej", "Zamknij");
						}

						case 5: cmd_anim(playerid, "");
					}
				}
			}
		}
		
		case DIALOG_CARS_SHOP_CATEGORY:
		{
			if( !response ) return 1;
			
			new category = DynamicGui_GetValue(playerid, listitem);
			
			DynamicGui_Init(playerid);

			new str[2048];
			
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT * FROM crp_salon_vehicles WHERE salon_cat = %d ORDER BY salon_price", category));
			
			for(new i; i < cache_get_rows(); i++)
			{
				format(str, sizeof(str), "%s$%d\t\t\t%s\n", str, cache_get_int(i, "salon_price"), VehicleNames[cache_get_int(i, "salon_model")-400]);
				DynamicGui_AddRow(playerid, cache_get_int(i, "salon_model"), cache_get_int(i, "salon_price"));
			}
			
			cache_delete(result);
			
			ShowPlayerDialog(playerid, DIALOG_CARS_SHOP_LIST, DIALOG_STYLE_LIST, "Kupowanie pojazdu", str, "Wybierz", "Zamknij");
		}
		
		case DIALOG_CARS_SHOP_LIST:
		{
			if( !response ) return cmd_kupauto(playerid, "");
			
			new model = DynamicGui_GetValue(playerid, listitem), price = DynamicGui_GetDataInt(playerid, listitem);
			
			new resp = SetOffer(INVALID_PLAYER_ID, playerid, OFFER_TYPE_SALON_VEH, price, model);
			
			if( resp ) ShowPlayerOffer(playerid, INVALID_PLAYER_ID, "Pojazd z salonu", sprintf("%s (%d)", VehicleNames[model-400], model), price);
		}
		
		case DIALOG_CARS_SHOP_FUELTYPE:
		{
			if( !response ) return ShowPlayerDialog(playerid, DIALOG_CARS_SHOP_FUELTYPE, DIALOG_STYLE_LIST, "Wybr rodzaju paliwa pojazdu", "Benzyna\nGaz\nDiesel", "Wybierz", "Zamknij");
			
			pInfo[playerid][player_dialog_tmp2] = listitem+1;
			
			ShowPlayerDialog(playerid, DIALOG_CARS_SHOP_COLOR, DIALOG_STYLE_INPUT, "Wybr koloru pojazdu", "W ponisze pole wpisz kolor pojazdu w formacie kolor1:kolor2, np. 24:35.", "Wybierz", "Zamknij");
		}
		
		case DIALOG_CARS_SHOP_COLOR:
		{
			if( !response ) return ShowPlayerDialog(playerid, DIALOG_CARS_SHOP_COLOR, DIALOG_STYLE_INPUT, "Wybr koloru pojazdu", "W ponisze pole wpisz kolor pojazdu w formacie kolor1:kolor2, np. 24:35.", "Wybierz", "Zamknij");
			
			new color1, color2;
			if( sscanf(inputtext, "p<:>dd", color1, color2) ) return ShowPlayerDialog(playerid, DIALOG_CARS_SHOP_COLOR, DIALOG_STYLE_INPUT, "Wybr koloru pojazdu", "W ponisze pole wpisz kolor pojazdu w formacie kolor1:kolor2, np. 24:35.\n\n"HEX_COLOR_LIGHTER_RED"Podany kolor ma zy format.", "Wybierz", "Zamknij");
			
			if( color1 < 0 || color1 > 255 || color2 < 0 || color2 > 255 ) return ShowPlayerDialog(playerid, DIALOG_CARS_SHOP_COLOR, DIALOG_STYLE_INPUT, "Wybr koloru pojazdu", "W ponisze pole wpisz kolor pojazdu w formacie kolor1:kolor2, np. 24:35.\n\n"HEX_COLOR_LIGHTER_RED"Podany kolor jest bdny (zakres 0-255).", "Wybierz", "Zamknij");
		
			new str[500];
			strcat(str, "INSERT INTO `crp_vehicles` (vehicle_uid, vehicle_model, vehicle_posx, vehicle_posy, vehicle_posz, vehicle_posa, vehicle_world, vehicle_interior, vehicle_color1, vehicle_color2, vehicle_fueltype, vehicle_fuel, vehicle_ownertype, vehicle_owner) VALUES");
			strcat(str, sprintf("(null, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d, %d, %d)", pInfo[playerid][player_dialog_tmp1], 873.787, -1249.45, 14.9158, 270.406, 0, 0, color1, color2, pInfo[playerid][player_dialog_tmp2], VehicleFuelMax[pInfo[playerid][player_dialog_tmp1]-400], VEHICLE_OWNER_TYPE_PLAYER, pInfo[playerid][player_id]));
			
			print(str);
			mysql_tquery(g_sql, str);

			SendGuiInformation(playerid, "Informacja", "Twj pojazd znajduje si w stanowym magazynie pojazdw. Aby go zespanowa uyj /v.\n\nNastpnie moesz go namierzy uywajc /v namierz.");
		}
		
		case DIALOG_STATS:
		{
			if( !response ) return 1;
			
			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_STATS_TALK_STYLE:
				{
					new str[200];
					
					for(new i;i<sizeof(TalkStyleName);i++)
					{
						format(str, sizeof(str), "%s%s\n", str, TalkStyleName[i]);
					}
					
					ShowPlayerDialog(playerid, DIALOG_STATS_TALK_STYLE, DIALOG_STYLE_LIST, "Wybr sposobu mwienia", str, "Wybierz", "Zamknij");
				}
				
				case DG_STATS_WALK_STYLE:
				{
					new str[300];
					
					for(new i;i<sizeof(WalkStyleName);i++)
					{
						format(str, sizeof(str), "%s%s\n", str, WalkStyleName[i]);
					}
					
					ShowPlayerDialog(playerid, DIALOG_STATS_WALK_STYLE, DIALOG_STYLE_LIST, "Wybr sposobu chodzenia", str, "Wybierz", "Zamknij");
				}

				case DG_STATS_FREEZE_DOOR:
				{
					pInfo[playerid][player_freeze_door] = !pInfo[playerid][player_freeze_door];
					mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_freeze_door = %d WHERE char_uid = %d", pInfo[playerid][player_freeze_door], pInfo[playerid][player_id]));

					return cmd_stats(playerid, "");
				}

				case DG_STATS_SHOW_HINTS:
				{
					pInfo[playerid][player_show_hints] = !pInfo[playerid][player_show_hints];
					mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_show_hints = %d WHERE char_uid = %d", pInfo[playerid][player_show_hints], pInfo[playerid][player_id]));

					return cmd_stats(playerid, "");
				}
				
				case DG_STATS_SPAWN:
				{
					if( !response ) return 1;
					
					new did = GetDoorByUid(pInfo[playerid][player_door]);
					if( did > -1 )
					{
						if( Door[did][door_owner_type] == DOOR_OWNER_TYPE_GROUP )
						{
							new gid = GetGroupByUid(Door[did][door_owner]);
							if( Group[gid][group_type] == GROUP_TYPE_HOTEL ) return SendGuiInformation(playerid, "Wystpi bd", "Jeste zameldowany w hotelu. Aby zmieni spawn musisz si wczeniej z niego wymeldowa.");
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
										if( Group[gid][group_type] == GROUP_TYPE_SOCIAL_HOUSE ) return SendGuiInformation(playerid, "Wystpi bd", "Jeste zameldowany w mieszkaniu socjalnym. Aby zmieni spawn musisz si wczeniej z niego wymeldowa.");
									}
								}	
							}
						}
					}
					
					new str[400], count;
					DynamicGui_Init(playerid);
					
					format(str, sizeof(str), "----------\t\t\tGlobalny\n");
					DynamicGui_AddRow(playerid, 0);
					
					foreach(new d_id : Doors)
					{
						if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_PLAYER && Door[d_id][door_owner] == pInfo[playerid][player_id] )
						{
							format(str, sizeof(str), "%s(UID: %d)\t\t%s\n", str, Door[d_id][door_uid], Door[d_id][door_name]);
							DynamicGui_AddRow(playerid, Door[d_id][door_uid]);
							count++;
						}
					}

					for(new i=0;i<5;i++)
					{
						if( pGroup[playerid][i][pg_id] > -1 )
						{
							new gid = pGroup[playerid][i][pg_id];

							if( GroupHasFlag(gid, GROUP_FLAG_SPAWN) && WorkerHasFlag(playerid, i, WORKER_FLAG_DOORS) )
							{
								foreach(new d_id : Doors)
								{
									if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP && Door[d_id][door_owner] == Group[gid][group_uid] )
									{
										format(str, sizeof(str), "%s(UID: %d;GRUPA: %s)\t\t%s\n", str, Door[d_id][door_uid], Group[gid][group_name], Door[d_id][door_name]);
										DynamicGui_AddRow(playerid, Door[d_id][door_uid]);
										count++;
									}
								}
							}
						}
					}
					
					if( count == 0 ) SendGuiInformation(playerid, "Wystpi bd", "Nie jeste wacicielem adnego budynku, w ktrym mgby ustawi spawn.");
					else ShowPlayerDialog(playerid, DIALOG_STATS_SPAWN, DIALOG_STYLE_LIST, "Dostpne spawny:", str, "Wybierz", "Zamknij");
				}

				case DG_INTERFACE:
				{
					pInfo[playerid][player_interface] = !pInfo[playerid][player_interface];
					mysql_tquery(g_sql, sprintf("UPDATE crp_characters SET char_interface = %d", pInfo[playerid][player_interface]));
					SendGuiInformation(playerid, "Informacja", sprintf("Pomylnie %s interfejs "HEX_COLOR_HONEST"Honest RolePlay", (pInfo[playerid][player_interface] ? ("wczye") : ("wyczye"))));
				}

				case DG_EDITOR:
				{
					pInfo[playerid][player_editor] = !pInfo[playerid][player_editor];
					mysql_tquery(g_sql, sprintf("UPDATE crp_characters SET char_editor = %d", pInfo[playerid][player_editor]));
					SendGuiInformation(playerid, "Informacja", sprintf("Pomylnie zmienie tryb budowy na %s", (pInfo[playerid][player_editor]) ? ("klawiszowy") : ("strzakowy")));					
				}

				case DG_ITEMS_FAVORITE:
				{
					pInfo[playerid][player_list_favorite_items] = !pInfo[playerid][player_list_favorite_items];
					mysql_tquery(g_sql, sprintf("UPDATE crp_characters SET char_favorite_items = %d", pInfo[playerid][player_list_favorite_items]));
					SendGuiInformation(playerid, "Informacja", sprintf("Pomylnie zmienie priorytet wywietlania przedmiotw na %s", (pInfo[playerid][player_list_favorite_items]) ? ("wysoki") : ("niski")));							
				}
			}
		}
		
		case DIALOG_STATS_TALK_STYLE:
		{
			if( !response ) return cmd_stats(playerid, "");
			
			pInfo[playerid][player_talk_style] = listitem;
			mysql_tquery(g_sql, sprintf("UPDATE crp_characters SET char_talkstyle = %d WHERE char_uid = %d", pInfo[playerid][player_talk_style], pInfo[playerid][player_id]));
			
			cmd_stats(playerid, "");
		}
		
		case DIALOG_STATS_WALK_STYLE:
		{
			if( !response ) return cmd_stats(playerid, "");
			
			pInfo[playerid][player_walk_style] = listitem;
			mysql_tquery(g_sql, sprintf("UPDATE crp_characters SET char_walkstyle = %d WHERE char_uid = %d", pInfo[playerid][player_walk_style], pInfo[playerid][player_id]));
			
			cmd_stats(playerid, "");
		}
		
		case DIALOG_STATS_SPAWN:
		{
			if( !response ) return cmd_stats(playerid, "");
			
			pInfo[playerid][player_door] = DynamicGui_GetValue(playerid, listitem);
			mysql_tquery(g_sql, sprintf("UPDATE crp_characters SET char_door = %d WHERE char_uid = %d", DynamicGui_GetValue(playerid, listitem), pInfo[playerid][player_id]));
			
			cmd_stats(playerid, "");
		}
		
		case DIALOG_POKAZ_DOKUMENT:
		{
			if( !response ) return 1;
			
			new targetid = DynamicGui_GetValue(playerid, listitem);
			
			if( !IsPlayerConnected(targetid) ) return 1;
			if( !pInfo[targetid][player_logged] ) return 1;
			
			new str[80];
			if( pInfo[playerid][player_dialog_tmp1] == DOCUMENT_ID ) format(str, sizeof(str), "Typ dokumentu:\tDowd osobisty\n");
			else if( pInfo[playerid][player_dialog_tmp1] == DOCUMENT_DRIVE ) format(str, sizeof(str), "Typ dokumentu:\tPrawo jazdy\n");
			else if( pInfo[playerid][player_dialog_tmp1] == DOCUMENT_METRYCZKA ) format(str, sizeof(str), "Typ dokumentu:\tMetryczka\n");
			
			new imie[15], nazwisko[15];
			sscanf(pInfo[playerid][player_name], "s[15] s[15]", imie, nazwisko);
			
			format(str, sizeof(str), "%sImie:\t\t\t%s\nNazwisko:\t\t%s\nWiek:\t\t%d", str, imie, nazwisko, pInfo[playerid][player_age]);
			
			SendGuiInformation(targetid, "Dokument", str);
			
			ProxMessage(playerid, sprintf("pokazuje dokument %s.", pInfo[targetid][player_name]), PROX_AME);
		}
		
		case DIALOG_EMERGENCY:
		{
			if(!response) return 1;

			new Cache:result, call_uid;

			call_uid = DynamicGui_GetValue(playerid, listitem);
			result = mysql_query(g_sql, sprintf("SELECT * FROM crp_emergency WHERE call_uid = '%d'", call_uid));

			if(cache_get_int(0, "call_active") == 0) return Alert(playerid, ALERT_TYPE_NEGATIVE, "Ktoœ ju¿ przej¹³ to zg³oszenie.");

			new Float:x = cache_get_float(0, "call_pos_x"),
				Float:y = cache_get_float(0, "call_pos_y"),
				Float:z = cache_get_float(0, "call_pos_z"),
				sender = cache_get_int(0, "call_sender"),
				uid = cache_get_int(0, "call_uid"),
				date[64],
				content[256];

			cache_get(0, "call_content", content);
			GetRelativeDate(cache_get_int(0, "call_date"), date);
			replacePolishChars(content);

			cache_delete(result);

			PlayerTextDrawSetString(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], sprintf("~p~Zgloszenie 911~n~~y~UID: ~w~%d  ~y~Numer telefonu: ~w~%d~n~~n~~b~Tresc: ~w~%s~n~~n~~w~Pozycje wskazano ~g~na mapie~w~.", uid, sender, content));
    		
    		//we need to hide last txd and show new
    		PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);
    		PlayerTextDrawShow(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);

    		mysql_pquery(g_sql, sprintf("UPDATE crp_emergency SET call_active = 0 WHERE call_uid = '%d'", uid));

    		SendClientMessage(playerid, -1, HEX_COLOR_HONEST"[911] "HEX_COLOR_SAMP"Wskazano punkt zg³oszenia na mapie. Aby anulowaï¿½ wpisz "HEX_COLOR_HONEST"/911 cancel"HEX_COLOR_SAMP".");

    		SetPlayerCheckpoint(playerid, x, y, z, 5.0);
		}

		case DIALOG_911:
		{
			if( !response )
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				
				return 1;
			}
			
			new gid = DynamicGui_GetValue(playerid, listitem);
		

			SendClientMessage(playerid, COLOR_YELLOW, sprintf("[Telefon] %s, centrala zg³oszeñ alarmowych. Proszê podaæ swoj¹ lokalizacjê i opisaæ sytuacjê.", Group[gid][group_name]));

			pInfo[playerid][player_dialog_tmp1] = gid;
			
			ShowPlayerDialog(playerid, DIALOG_911_REASON, DIALOG_STYLE_INPUT, "Zgoszenie", "Poniej podaj opis zdarzenia jak i jego miejsce:", "Gotowe", "Anuluj");
		}
		
		case DIALOG_911_REASON:
		{
			if( !response )
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				
				return 1;
			}
			
			if( isnull(inputtext) ) return ShowPlayerDialog(playerid, DIALOG_911_REASON, DIALOG_STYLE_INPUT, "Zgoszenie", "Poniej podaj opis zdarzenia jak i jego miejsce:\n\n"HEX_COLOR_LIGHTER_RED"To pole nie moe by puste.", "Gotowe", "Anuluj");
			
			new gid = pInfo[playerid][player_dialog_tmp1], itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);

			new str[200], content[400];

			content = BeautifyString(inputtext, true, true, false);
			mysql_escape_string(content, content);

			format(str, sizeof(str), "* Nowe zg³oszenie na /911 od %d.", Item[itemid][item_value1]);

			foreach(new p : Player)
			{
				new slot = GetPlayerDutySlot(p);
				if( slot == -1 ) continue;
				if( pGroup[p][slot][pg_id] != gid )
				{
					if( Group[pGroup[p][slot][pg_id]][group_parent_uid] != Group[gid][group_uid] ) continue;
				}
				SendClientMessage(p, 0xFF8554FF, str);
			}

            SendClientMessage(playerid, COLOR_YELLOW, "[Telefon] Rozumiem, przyjê³am zg³oszenie. Proszê oczekiwaæ na przyjêcie zg³oszenia przez odpowiednie s³u¿by.");

		    new Float:x,	
			    Float:y,
                Float:z;

            GetPlayerPos(playerid, x, y, z);

			mysql_pquery(g_sql, sprintf("INSERT INTO `crp_emergency` VALUES (null, '%d', '%d', '%s', '%d', '%f', '%f', '%f', '%d')", Item[itemid][item_value1], gid, content, true, x, y, z, gettime()));
			
			PlayerLog(sprintf("Send notification to group %s with message %s", GroupLogLink(Group[gid][group_uid]), inputtext), pInfo[playerid][player_id], "basic");
			GroupLog(sprintf("Incoming notification from player %s with message %s", PlayerLogLink(pInfo[playerid][player_id]), inputtext), Group[gid][group_uid], "notification");

			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
			if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
		}
		
		case DIALOG_DOOR_STORAGE:
		{
			if( !response ) return 1;
			
			new itemid = DynamicGui_GetValue(playerid, listitem);
			new d_id = GetDoorByUid(Item[itemid][item_owner]);
			new gid = GetGroupByUid(Door[d_id][door_owner]);
			new created_itemid = 0;
			if( GroupHasFlag(gid, GROUP_FLAG_WEAPON_FLAG) ) created_itemid = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, Item[itemid][item_type], Item[itemid][item_model], Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_name], 0, 0, Group[gid][group_uid]);
			else created_itemid = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, Item[itemid][item_type], Item[itemid][item_model], Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_name]);
			Item[itemid][item_amount] -= 1;
			
			if( Item[itemid][item_amount] > 0 ) mysql_tquery(g_sql, sprintf("UPDATE crp_items SET item_amount = %d WHERE item_uid = %d", Item[itemid][item_amount], Item[itemid][item_uid]));
			else DeleteItem(itemid, true);
			
			ProxMessage(playerid, "wyciga co z magazynu.", PROX_AME);

			PlayerLog(sprintf("Takes an item %s from group %s warehouse, door %s", ItemLogLink(Item[created_itemid][item_uid]), GroupLogLink(Group[gid][group_uid]), Door[d_id][door_uid]), pInfo[playerid][player_id], "basic");
			GroupLog(sprintf("Player %s took an item %s from warehouse", PlayerLogLink(pInfo[playerid][player_id]), ItemLogLink(Item[created_itemid][item_uid])), Group[gid][group_uid], "warehouse");
		}
		
		case DIALOG_444_REASON:
		{
			if( !response )
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
				if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
				
				return 1;
			}
			
			if( isnull(inputtext) ) return ShowPlayerDialog(playerid, DIALOG_444_REASON, DIALOG_STYLE_INPUT, "Zgoszenie", "Poniej podaj opis zdarzenia jak i jego miejsce:\n\n"HEX_COLOR_LIGHTER_RED"To pole nie moe by puste.", "Gotowe", "Anuluj");
			
			new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);

			new str[200], content[400];

			content = BeautifyString(inputtext, true, true, false);
			mysql_escape_string(content, content);

			format(str, sizeof(str), "* Nowe zg³oszenie na /911 od %d.", Item[itemid][item_value1]);
			
			new send_to_groups[20], send_to = 0;

			foreach(new p : Player)
			{
				new slot = GetPlayerDutySlot(p);
				if( slot == -1 ) continue;
				if( Group[pGroup[p][slot][pg_id]][group_type] != GROUP_TYPE_RADIO ) continue;

				new do_send = true;

				for(new i;i<=send_to;i++)
				{
					if( send_to_groups[i] == Group[pGroup[p][slot][pg_id]][group_uid] ) do_send = false;
				}

				if( do_send )
				{
					send_to++;
					send_to_groups[send_to] = Group[pGroup[p][slot][pg_id]][group_uid];

					PlayerLog(sprintf("Send notification to group %s with message %s", GroupLogLink(send_to_groups[send_to]), inputtext), pInfo[playerid][player_id], "basic");
					GroupLog(sprintf("Incoming notification from player %s with message %s", PlayerLogLink(pInfo[playerid][player_id]), inputtext), send_to_groups[send_to], "notification");
				}

				SendClientMessage(p, 0xFF8554FF, str);
			}

			SendClientMessage(playerid, 0x9B91ECFF, "** Dzikujemy za Twoje zgoszenie, jeden z naszych pracownikw ju je weryfikuje. (( Centrala ))");
			
			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
			if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
		}
		
		case DIALOG_YO:
		{
			if( !response ) return 1;
			
			new targetid = DynamicGui_GetValue(playerid, listitem);
			
			if( !IsPlayerConnected(targetid) ) return 1;
			if( !pInfo[targetid][player_logged] ) return 1;

			new resp = SetOffer(playerid, targetid, OFFER_TYPE_YO, 0, pInfo[playerid][player_dialog_tmp1]);
			
			if( resp ) ShowPlayerOffer(targetid, playerid, "Dodatki", sprintf("Przywitanie (%d)", pInfo[playerid][player_dialog_tmp1]), 0);
		}
		
		case DIALOG_SEARCH:
		{
			if( !response ) return 1;
			
			new option = DynamicGui_GetValue(playerid, listitem);
			new targetid = pInfo[playerid][player_dialog_tmp1];

			if( !IsPlayerConnected(targetid) ) return 1;
			if( !pInfo[targetid][player_logged] ) return 1;

			switch(option)
			{
				case 0:
				{
					defer PlayerSearch[1](playerid, targetid, 0);
				}
				case 1:
				{
					Alert(playerid, ALERT_TYPE_INFO, "Rozpoczynasz ~g~dokladne ~w~przeszukanie gracza. Potrwa ono ~y~kilkanascie sekund~w~. Mozesz je przerwac spacja.");
					ApplyAnimation(playerid, "COP_AMBIENT", "Copbrowse_loop", 4.0, 1, 0, 0, 0, 0, 1);

					defer PlayerSearch[3000](playerid, targetid, 1);
				}
			}

			PlayerLog(sprintf("Searched player %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "basic");
			PlayerLog(sprintf("Was searched by player %s", PlayerLogLink(pInfo[playerid][player_id])), pInfo[targetid][player_id], "basic");

			ProxMessage(playerid, sprintf("przeszukuje %s.", pInfo[targetid][player_name]), PROX_AME);
		}
		
		case DIALOG_ACCESSORY:
		{
			new itemid = pInfo[playerid][player_dialog_tmp1];

			new freeid = GetPlayerFreeAttachSlot(playerid);
			if( freeid == -1 ) return SendGuiInformation(playerid, "Wystpi bd", "Podczas uywania przedmiotu wystpi bd.");
			
			// zakladamy
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT * FROM crp_access WHERE access_uid = %d", Item[itemid][item_value1]));
			
			SetPlayerAttachedObject(playerid, freeid, cache_get_int(0, "access_model"), cache_get_int(0, "access_bone"), cache_get_int(0, "access_posx"), cache_get_float(0, "access_posy"), cache_get_float(0, "access_posz"), 
			cache_get_float(0, "access_rotx"), cache_get_float(0, "access_roty"), cache_get_float(0, "access_rotz"), cache_get_float(0, "access_scalex"), cache_get_float(0, "access_scaley"), cache_get_float(0, "access_scalez"));
			
			Item[itemid][item_value2] = freeid;
			
			cache_delete(result);
			
			Item[itemid][item_used] = true;
			mysql_tquery(g_sql, sprintf("UPDATE crp_items SET item_used = 1 WHERE item_uid = %d", Item[itemid][item_uid]));

			if(response)
			{
				// dostosowanie
				pInfo[playerid][player_attached_item_edit] = true;
				pInfo[playerid][player_attached_item_edit_id] = itemid;
				EditAttachedObject(playerid, freeid);
			}
		}
		
		case DIALOG_SOCIAL_RENT:
		{
			if( !response ) return 1;
			
			new parent_did = pInfo[playerid][player_dialog_tmp1], d_id = pInfo[playerid][player_dialog_tmp2];
			if( pInfo[playerid][player_debit] > 0 ) return SendPlayerInformation(playerid, "Najpierw splac swoj dlug.", 4000);
			if( pInfo[playerid][player_bank_money] < Door[parent_did][door_value1] ) return SendGuiInformation(playerid, "Wystpi bd", "Na Twoim koncie nie ma wymaganej iloci pienidzy do opacenia czynszu.");
			if( Door[d_id][door_owner] > 0 ) return SendGuiInformation(playerid, "Wystpi bd", "Wystpi nieoczekiwany bd, sprbuj ponownie lub skontaktuj si z administracj.");
			
			AddPlayerBankMoney(playerid, -Door[parent_did][door_value1], "Opacenie czynszu");
			
			pInfo[playerid][player_door] = Door[d_id][door_uid];
			Door[d_id][door_owner] = pInfo[playerid][player_id];
			
			mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_door = %d WHERE char_uid = %d", pInfo[playerid][player_door], pInfo[playerid][player_id]));
			mysql_pquery(g_sql, sprintf("UPDATE crp_doors SET door_owner = %d WHERE door_uid = %d", Door[d_id][door_owner], Door[d_id][door_uid]));
		}
		
		case DIALOG_ANIMS_LIST:
		{
			if( !response ) return 1;
			
			new dg_value = DynamicGui_GetValue(playerid, listitem);
			
			ApplyCommandAnim(playerid, sprintf("WHERE anim_uid = %d", dg_value));
		}
		
		case DIALOG_ACCEPT_DEATH:
		{
			if( !response ) return 1;

			AddPlayerPenalty(playerid, PENALTY_TYPE_BLOCK, INVALID_PLAYER_ID, 0, "Character kill", BLOCK_CHAR, false);
			DisplayPenaltyInformation("Usmiercenie postaci", "System", pInfo[playerid][player_name], "");
			
			new Float:x, Float:y, Float:z;
			GetPointInAngleOfPlayer(playerid, x, y, z, 1.0, 0.0);
			
			new Cache:result;
			result = mysql_query(g_sql, sprintf("INSERT INTO crp_corpses VALUES (null, %d, %d, %d, %d, %d, 0)", pInfo[playerid][player_bw_reason], pInfo[playerid][player_id], pInfo[playerid][player_bw_killer], pInfo[playerid][player_bw_weapon], gettime()));
			new i_val1 = cache_insert_id();

			cache_delete(result);
			
			result = mysql_query(g_sql, "SELECT * FROM `crp_items_proto` WHERE `model` = 3092");
			
			new Float:rx = cache_get_float(0, "rx"), Float:ry = cache_get_float(0, "ry"), Float:rz = cache_get_float(0, "rz");
			z = floatadd(z, cache_get_float(0, "z"));
			
			cache_delete(result);
			
			new query[400];
			strcat(query, "INSERT INTO `crp_items` (`item_uid`,`item_model`,`item_ownertype`,`item_type`,`item_value1`,`item_name`,`item_created`,`item_posx`,`item_posy`,`item_posz`,`item_rotx`,`item_roty`,`item_rotz`,`item_world`,`item_interior`) ");
			strcat(query, sprintf("VALUES (null, 3092, %d, %d, %d, 'Zwoki', %d, %f, %f, %f, %f, %f, %f, %d, %d)", ITEM_OWNER_TYPE_GROUND, ITEM_TYPE_CORPSE, i_val1, gettime(), x, y, z, rx, ry, rz, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid)));
			
			result = mysql_query(g_sql, query);
			new uid = cache_insert_id();

			cache_delete(result);
			
			LoadItem(sprintf("WHERE `item_uid` = %d", uid), true);
			
			PlayerLog("Accepted character kill", pInfo[playerid][player_id], "basic");

			Kick(playerid);
		}
		
		case DIALOG_FILES:
		{
			if( !response ) return 1;
		
			new dg_value = DynamicGui_GetValue(playerid, listitem);

			switch(dg_value)
			{
				case DG_FILES_SEARCH_GROUP: ShowPlayerDialog(playerid, DIALOG_FILES_SEARCH, DIALOG_STYLE_INPUT, "Kartoteka  Wyszukiwanie grupy", "W poniszym polu podaj nazw(lub jej cz) szukanej grupy:", "Szukaj", "Wr");
				case DG_FILES_SEARCH_CHAR: ShowPlayerDialog(playerid, DIALOG_FILES_SEARCH, DIALOG_STYLE_INPUT, "Kartoteka  Wyszukiwanie osoby", "W poniszym polu podaj imi i/lub nazwisko szukanej osoby:", "Szukaj", "Wr");
				case DG_FILES_SEARCH_VEH: ShowPlayerDialog(playerid, DIALOG_FILES_SEARCH, DIALOG_STYLE_INPUT, "Kartoteka  Wyszukiwanie pojazdu", "W poniszym polu podaj numer nadwozia(rejestracja) szukanego pojazdu:", "Szukaj", "Wr");
				default: return LSPDDB_DisplayMainPage(playerid);
			}

			pInfo[playerid][player_files_search_type] = dg_value;
		}
		
		case DIALOG_FILES_SEARCH:
		{
			if( !response ) return LSPDDB_DisplayMainPage(playerid);
			
			new input[250];
			mysql_escape_string(inputtext, input);
			
			// Zbyt krtki szukany wyraz
			if( strlen(inputtext) < 3 && pInfo[playerid][player_files_search_type] != 1 )
			{
				switch(pInfo[playerid][player_files_search_type])
				{
					case DG_FILES_SEARCH_CHAR:
					{
						ShowPlayerDialog(playerid, DIALOG_FILES_SEARCH, DIALOG_STYLE_INPUT, "Kartoteka  Wyszukiwanie osoby", "W poniszym polu podaj imi i/lub nazwisko szukanej osoby:\n\n"HEX_COLOR_LIGHTER_RED"Fraza wyszukiwania musi zawiera przynajmniej 3 znaki.", "Szukaj", "Wr");
					}
					
					case DG_FILES_SEARCH_GROUP:
					{
						ShowPlayerDialog(playerid, DIALOG_FILES_SEARCH, DIALOG_STYLE_INPUT, "Kartoteka  Wyszukiwanie grupy", "W poniszym polu podaj nazw(lub jej cz) szukanej grupy:\n\n"HEX_COLOR_LIGHTER_RED"Fraza wyszukiwania musi zawiera przynajmniej 3 znaki.", "Szukaj", "Wr");
					}
				}
				
				return 1;
			}
			
			new query[200];
			
			switch(pInfo[playerid][player_files_search_type])
			{
				case DG_FILES_SEARCH_CHAR:
				{
					format(query, sizeof(query), "SELECT DISTINCT file_owner, char_name, char_uid FROM crp_files, crp_characters WHERE file_ownertype = %d AND char_uid = file_owner AND char_name LIKE '%%%s%%'", pInfo[playerid][player_files_search_type], input);
				}
				
				case DG_FILES_SEARCH_VEH: 
				{
					format(query, sizeof(query), "SELECT DISTINCT file_owner, vehicle_model, vehicle_uid FROM crp_files, crp_vehicles WHERE file_ownertype = %d AND vehicle_uid = file_owner AND file_owner = %d", pInfo[playerid][player_files_search_type], strval(input));
				}
				
				case DG_FILES_SEARCH_GROUP:
				{
					format(query, sizeof(query), "SELECT DISTINCT file_owner, group_name, group_uid FROM crp_files, crp_groups WHERE file_ownertype = %d AND group_uid = file_owner AND group_name LIKE '%%%s%%'", pInfo[playerid][player_files_search_type], input);
				}
			}

			new Cache:result;
			result = mysql_query(g_sql, query);
			
			if( cache_get_rows() == 0 )
			{
				switch(pInfo[playerid][player_files_search_type])
				{
					case DG_FILES_SEARCH_CHAR:
					{
						ShowPlayerDialog(playerid, DIALOG_FILES_SEARCH, DIALOG_STYLE_INPUT, "Kartoteka  Wyszukiwanie osoby", "W poniszym polu podaj imi i/lub nazwisko szukanej osoby:\n\n"HEX_COLOR_LIGHTER_RED"Brak wynikw dla tego zapytania.", "Szukaj", "Wr");
					}
					
					case DG_FILES_SEARCH_VEH: 
					{
						ShowPlayerDialog(playerid, DIALOG_FILES_SEARCH, DIALOG_STYLE_INPUT, "Kartoteka  Wyszukiwanie pojazdu", "W poniszym polu podaj numer nadwozia(rejestracja) szukanego pojazdu:\n\n"HEX_COLOR_LIGHTER_RED"Brak wynikw dla tego zapytania.", "Szukaj", "Wr");
					}
					
					case DG_FILES_SEARCH_GROUP:
					{
						ShowPlayerDialog(playerid, DIALOG_FILES_SEARCH, DIALOG_STYLE_INPUT, "Kartoteka  Wyszukiwanie grupy", "W poniszym polu podaj nazw(lub jej cz) szukanej grupy:\n\n"HEX_COLOR_LIGHTER_RED"Brak wynikw dla tego zapytania.", "Szukaj", "Wr");
					}
				}
			}
			else
			{
				new str[1024];
				
				DynamicGui_Init(playerid);
				
				for(new i;i<cache_get_rows();i++)
				{
					new tmp_str[60];
					switch(pInfo[playerid][player_files_search_type])
					{
						case DG_FILES_SEARCH_CHAR:
						{
							cache_get(i, "char_name", tmp_str);
							strreplace(tmp_str, "_", " ");
							format(str, sizeof(str), "%s%s\n", str, tmp_str);
							DynamicGui_AddRow(playerid, cache_get_int(i, "char_uid"));
						}
						
						case DG_FILES_SEARCH_VEH: 
						{
							format(str, sizeof(str), "%s%s\t\t[%UID: %d]\n", str, VehicleNames[cache_get_int(i, "vehicle_model")-400], cache_get_int(i, "vehicle_uid"));
							DynamicGui_AddRow(playerid, cache_get_int(i, "vehicle_uid"));
						}
						
						case DG_FILES_SEARCH_GROUP:
						{
							cache_get(i, "group_name", tmp_str);
							format(str, sizeof(str), "%s%s\n", str, tmp_str);
							DynamicGui_AddRow(playerid, cache_get_int(i, "group_uid"));
						}
					}
				}
				
				ShowPlayerDialog(playerid, DIALOG_FILES_SEARCH_RESULT, DIALOG_STYLE_LIST, sprintf("Kartoteka  Rezultaty wyszukiwania (%d)", cache_get_rows()), str, "Przegldaj", "Wr");
			}
			
			cache_delete(result);
		}
		
		/*case DIALOG_FILES_SEARCH_RESULT:
		{
			if( !response )
			{
				cmd_kartoteka(playerid, "");
				return 1;
			}
			
			new uid = DynamicGui_GetValue(playerid, listitem);
			
			switch(pInfo[playerid][player_files_search_type])
			{
				case 0:
				{
					new Cache:result;
					result = mysql_query(g_sql, sprintf("SELECT char_name, char_birth, char_sex, char_door FROM crp_characters WHERE char_uid = %d", uid));
					
					new name[MAX_PLAYER_NAME+1], imie[20], nazwisko[20];					
					cache_get(0, "char_name", name);
					sscanf(name, "p<_>s[20]s[20]", imie, nazwisko);
					
					new wiek = cache_get_int(0, "char_birth"), plec = cache_get_int(0, "char_sex"), zameldowanie = cache_get_int(0, "char_door");
					if( plec == 0 ) plec = 'K';
					else plec = 'M';
					
					new zameldowanie_str[60], did = GetDoorByUid(zameldowanie);
					if( zameldowanie > 0 && did > -1 ) format(zameldowanie_str, sizeof(zameldowanie_str), "%s (UID: %d)", Door[did][door_name], zameldowanie);
					else format(zameldowanie_str, sizeof(zameldowanie_str), "Brak");
					
					cache_delete(result);
					
					result = mysql_query(g_sql, sprintf("SELECT COUNT(*) as ilosc FROM crp_files WHERE file_ownertype = %d AND file_owner = %d", pInfo[playerid][player_files_search_type], uid));
					
					new ilosc = cache_get_int(0, "ilosc");
					
					cache_delete(result);
					
					new online[40];
					if( GetPlayerByUid(uid) > 0 ) format(online, sizeof(online), " "HEX_COLOR_LIGHTER_GREEN"(( online ))");
					
					new str[400];

					format(str, sizeof(str), "Imie:\t\t\t%s\nNazwisko:\t\t%s\nRok urodzenia:\t\t%d\nPe:\t\t\t%c\nZameldowanie:\t\t%s\nIlo wpisw:\t\t%d\n \n Przegldaj wszystkie wpisy\n Przegldaj niezapacone mandaty", imie, nazwisko, wiek, plec, zameldowanie_str, ilosc);
					
					DynamicGui_Init(playerid);
					DynamicGui_SetDialogValue(playerid, uid);
					
					DynamicGui_AddBlankRow(playerid);
					DynamicGui_AddBlankRow(playerid);
					DynamicGui_AddBlankRow(playerid);
					DynamicGui_AddBlankRow(playerid);
					DynamicGui_AddBlankRow(playerid);
					DynamicGui_AddBlankRow(playerid);
					DynamicGui_AddBlankRow(playerid);
					DynamicGui_AddRow(playerid, DG_FILES_PROFILE_LIST_ALL);
					DynamicGui_AddRow(playerid, DG_FILES_PROFILE_LIST_UNPAID);
					
					ShowPlayerDialog(playerid, DIALOG_FILES_PROFILE, DIALOG_STYLE_LIST, sprintf("Kartoteka  Profil osoby (%s %s)%s", imie, nazwisko, online), str, "Wybierz", "Wr");
				}
				
				case 1: 
				{
					
				}
				
				case 2:
				{
					
				}
			}
		}
		
		case DIALOG_FILES_PROFILE:
		{
			if( !response )
			{
				cmd_kartoteka(playerid, "");
				return 1;
			}
			
			new uid = DynamicGui_GetDialogValue(playerid), dg_option = DynamicGui_GetValue(playerid, listitem);
			
			pInfo[playerid][player_files_list_type] = dg_option;
			
			new query[200];
			
			switch(dg_option)
			{
				case DG_FILES_PROFILE_LIST_ALL:
				{
					format(query, sizeof(query), "SELECT file_desc, file_time, file_type, file_uid FROM crp_files WHERE file_ownertype = %d AND file_owner = %d ORDER BY file_time DESC", pInfo[playerid][player_files_search_type], uid);
				}
				
				case DG_FILES_PROFILE_LIST_UNPAID:
				{
					format(query, sizeof(query), "SELECT file_desc, file_time, file_type, file_uid FROM crp_files WHERE file_ownertype = %d AND file_owner = %d AND file_value > 0 AND file_paidtime = 0 ORDER BY file_time DESC", pInfo[playerid][player_files_search_type], uid);
				}
				
				default:
				{
					DynamicGui_Init(playerid);
					DynamicGui_AddRow(playerid, uid);
					pInfo[playerid][player_dialog] = DIALOG_FILES_SEARCH_RESULT;
					OnDialogResponse(playerid, DIALOG_FILES_SEARCH_RESULT, 1, 0, "");
					
					return 1;
				}
			}
			
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT char_name FROM crp_characters WHERE char_uid = %d", uid));
			
			new name[MAX_PLAYER_NAME+1], imie[20], nazwisko[20];					
			cache_get(0, "char_name", name);
			sscanf(name, "p<_>s[20]s[20]", imie, nazwisko);
			
			cache_delete(result);
			
			result = mysql_query(g_sql, query);
			
			if( cache_get_rows() == 0 )
			{
				DynamicGui_Init(playerid);
				DynamicGui_AddRow(playerid, uid);
				pInfo[playerid][player_dialog] = DIALOG_FILES_SEARCH_RESULT;
				OnDialogResponse(playerid, DIALOG_FILES_SEARCH_RESULT, 1, 0, "");
				
				SendPlayerInformation(playerid, "Brak wpisow do wyswietlenia", 4000);
			}
			else
			{
				new str[2048], desc[150], data[40];
				
				DynamicGui_Init(playerid);
				
				for(new i;i<cache_get_rows();i++)
				{
					cache_get(0, "file_desc", desc);
					strdel(desc, 39, 149);
					
					GetRelativeDate(cache_get_int(0, "file_time"), data);
					
					if( strlen(data) < 16 ) format(str, sizeof(str), "%s(%s) (%s)\t\t%s...\n", str, FileType[cache_get_int(0, "file_type")], data, desc);
					else format(str, sizeof(str), "%s(%s) (%s)\t%s...\n", str, FileType[cache_get_int(0, "file_type")], data, desc);
					DynamicGui_AddRow(playerid, cache_get_int(0, "file_uid"));
				}
				
				pInfo[playerid][player_files_profile_uid] = uid;
				
				ShowPlayerDialog(playerid, DIALOG_FILES_PROFILE_RECORDS, DIALOG_STYLE_LIST, sprintf("Kartoteka  Wpisy osoby (%s %s, ilo: %d)", imie, nazwisko, cache_get_rows()), str, "Wybierz", "Wr");
			}
			
			cache_delete(result);
		}
		
		case DIALOG_FILES_PROFILE_RECORDS:
		{
			if( !response )
			{
				DynamicGui_Init(playerid);
				DynamicGui_AddRow(playerid, pInfo[playerid][player_files_profile_uid]);
				pInfo[playerid][player_dialog] = DIALOG_FILES_SEARCH_RESULT;
				OnDialogResponse(playerid, DIALOG_FILES_SEARCH_RESULT, 1, 0, "");
				
				return 1;
			}
			
			new file_uid = DynamicGui_GetValue(playerid, listitem);
			
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT *, char_name FROM crp_files, crp_characters WHERE file_uid = %d AND file_giver = char_uid", file_uid));
			
			new str[500], name_str[MAX_PLAYER_NAME+1], date_str[40], payment_str[80], offer_str[40]; 
			
			cache_get(0, "char_name", name_str);
			strreplace(name_str, "_", " ");
			
			GetRelativeDate(cache_get_int(0, "file_time"), date_str);
			
			new file_type = cache_get_int(0, "file_type");
			
			if( file_type == FILES_TYPE_FINE || file_type == FILES_TYPE_BLOCK && cache_get_int(0, "file_value") > 0 )
			{
				if( cache_get_int(0, "file_paidtime") == 0 ) 
				{
					format(payment_str, sizeof(payment_str), "Naleno:\t\t$%d\nStatus patnoci:\t"HEX_COLOR_LIGHTER_RED"Niezapacono\n", cache_get_int(0, "file_value"));
					format(offer_str, sizeof(offer_str), "\n \n Oferuj zapat");
				}
				else 
				{
					new date_paid[40];
					GetRelativeDate(cache_get_int(0, "file_paidtime"), date_paid);
					format(payment_str, sizeof(payment_str), "Naleno:\t\t$%d\nStatus patnoci:\t"HEX_COLOR_LIGHTER_GREEN"Zapacono"HEX_COLOR_GREY" (%s)\n", cache_get_int(0, "file_value"), date_paid);
				}
			}

			new desc_str[250];
			cache_get(0, "file_desc", desc_str);
			
			format(str, sizeof(str), "Typ:\t\t\t%s\nNadajcy:\t\t%s\nData nadania:\t\t%s \n%sTre:\n   "HEX_COLOR_GREY"%s%s", FileType[file_type], name_str, date_str, payment_str, BreakLines(desc_str, "\n   "HEX_COLOR_GREY"", 64), offer_str);
			
			cache_delete(result);
			
			ShowPlayerDialog(playerid, DIALOG_FILES_PROFILE_RECORD, DIALOG_STYLE_LIST, sprintf("Kartoteka  Szczegy wpisu (UID: %d)", file_uid), str, "Wybierz", "Wr");
		}
		
		case DIALOG_FILES_PROFILE_RECORD:
		{
			if( !response )
			{
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, pInfo[playerid][player_files_profile_uid]);
				DynamicGui_AddRow(playerid, pInfo[playerid][player_files_list_type]);
				pInfo[playerid][player_dialog] = DIALOG_FILES_PROFILE;
				OnDialogResponse(playerid, DIALOG_FILES_PROFILE, 1, 0, "");
				
				return 1;
			}
		}*/
		
		case DIALOG_WHISPER_TOKEN:
		{
			if( !response ) return 1;
			
			if( strcmp(inputtext, pInfo[playerid][player_tmp_token], false) != 0 )
			{
				randomString(pInfo[playerid][player_tmp_token], 16);
		
				ShowPlayerDialog(playerid, DIALOG_WHISPER_TOKEN, DIALOG_STYLE_INPUT, "Przepisz token", sprintf("Aby skontaktowa si z czonkiem ekipy przepisz wygenerowany kod(wielko znakw ma znaczenie):\n\t%s\n\n"HEX_COLOR_LIGHTER_RED"Przepisany kod by bdny.", pInfo[playerid][player_tmp_token]), "Gotowe", "Anuluj");
				return 1;
			}
			
			new targetid = pInfo[playerid][player_tmp_whisper_id];
		
			pInfo[targetid][player_last_pm_playerid] = playerid;
			pInfo[playerid][player_last_pm_playerid] = targetid;

			PlayerLog("Correctly entered admin PM token", pInfo[playerid][player_id], "chat");
			cmd_w(playerid, sprintf("%d %s", pInfo[playerid][player_last_pm_playerid], pInfo[playerid][player_tmp_whisper]));
			SendGuiInformation(playerid, "Informacja", "Aby pomin proces autoryzacji tokenem, moesz poczeka na odpowied od administratora i uy /re.");
		}

		case DIALOG_CD_URL:
		{
			if( !response ) return 1;
			if( strlen(inputtext) < 15 ) return ShowPlayerDialog(playerid, DIALOG_CD_URL, DIALOG_STYLE_INPUT, "Tworzenie pyty  Adres radia", "W poniszym polu podaj adres do internetowego radia (ogg/vorbis;mp3;.pls):\n\n"HEX_COLOR_LIGHTER_RED"Podany adres jest zbyt krtki.", "Dalej", "Anuluj");
			if( (strfind(inputtext, "http://", true) == -1 && strfind(inputtext, "https://", true) == -1) || (strfind(inputtext, ".mp3", true) == -1 && strfind(inputtext, ".pls", true) == -1 && strfind(inputtext, ".m3u", true) == -1) )
			return ShowPlayerDialog(playerid, DIALOG_CD_URL, DIALOG_STYLE_INPUT, "Tworzenie pyty  Adres radia", "W poniszym polu podaj adres do internetowego radia (ogg/vorbis;mp3;.pls):\n\n"HEX_COLOR_LIGHTER_RED"Dozwolone rozszerzenia to .pls, .m3u oraz .mp3.", "Dalej", "Anuluj");
			new url[120];
			mysql_escape_string(inputtext, url);
			
			new Cache:result;
			result = mysql_query(g_sql, sprintf("INSERT INTO crp_audiourls VALUES (null, '%s')", url));
			
			Item[pInfo[playerid][player_dialog_tmp1]][item_value1] = cache_insert_id();

			cache_delete(result);
			
			mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_value1 = %d WHERE item_uid = %d", Item[pInfo[playerid][player_dialog_tmp1]][item_value1], Item[pInfo[playerid][player_dialog_tmp1]][item_uid]));
			
			ShowPlayerDialog(playerid, DIALOG_CD_NAME, DIALOG_STYLE_INPUT, "Tworzenie pyty  Nazwa", "W poniszym polu podaj nazw pyty (min. 3 znaki):", "Gotowe", "Anuluj");
		}
		
		case DIALOG_CD_NAME:
		{
			if( !response ) return ShowPlayerDialog(playerid, DIALOG_CD_NAME, DIALOG_STYLE_INPUT, "Tworzenie pyty  Nazwa", "W poniszym polu podaj nazw pyty (min. 3 znaki):", "Gotowe", "Anuluj");
			if( strlen(inputtext) < 3 ) return ShowPlayerDialog(playerid, DIALOG_CD_NAME, DIALOG_STYLE_INPUT, "Tworzenie pyty  Nazwa", "W poniszym polu podaj nazw pyty (min. 3 znaki):\n\n"HEX_COLOR_LIGHTER_RED"Nazwa pyty musi zawiera min. 3 znaki.", "Gotowe", "Anuluj");
			if( strlen(inputtext) > 20 ) return ShowPlayerDialog(playerid, DIALOG_CD_NAME, DIALOG_STYLE_INPUT, "Tworzenie pyty  Nazwa", "W poniszym polu podaj nazw pyty (min. 3 znaki):\n\n"HEX_COLOR_LIGHTER_RED"Nazwa pyty moe zawiera max. 20 znakw.", "Gotowe", "Anuluj");
			new cdname[120];
			mysql_escape_string(inputtext, cdname);
			
			format(Item[pInfo[playerid][player_dialog_tmp1]][item_name], 40, "%s (CD)", cdname);
			mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_name = '%s' WHERE item_uid = %d", Item[pInfo[playerid][player_dialog_tmp1]][item_name], Item[pInfo[playerid][player_dialog_tmp1]][item_uid]));
			
			SendGuiInformation(playerid, "Informacja", "Pomylnie utworzye pyt. Moesz jej teraz uy w pojedzie.");
		}

		case DIALOG_BAG:
		{
			if( !response ) return 1;

			new itemid = DynamicGui_GetValue(playerid, listitem);

			Item[itemid][item_owner_type] = ITEM_OWNER_TYPE_PLAYER;
			Item[itemid][item_owner] = pInfo[playerid][player_id];
			mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_ownertype = %d AND item_owner = %d WHERE item_uid = %d", ITEM_OWNER_TYPE_PLAYER, pInfo[playerid][player_id], Item[itemid][item_uid]));

			ProxMessage(playerid, "wyciga co z torby.", PROX_AME);
		}

		case DIALOG_ADRENALINE:
		{
			if( !response ) return 1;

			new targetid = DynamicGui_GetValue(playerid, listitem);

			pInfo[targetid][player_bw] = 0;
			pInfo[targetid][player_bw_end_time] = 0;
			
			PlayerTextDrawHide(targetid, pInfo[targetid][leftTime]);
			RemovePlayerStatus(targetid, PLAYER_STATUS_BW);
			SetPlayerHealth(targetid, 20);
			TogglePlayerControllable(targetid, 1);
			SetCameraBehindPlayer(targetid);
			ClearAnimations(targetid);
			SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);

			new targetname[MAX_PLAYER_NAME];
			GetPlayerName(targetid, targetname, sizeof(targetname));
			SpaceToUnderscore(targetname);

			new str[64];
			format(str, sizeof(str), "aplikuje adrenalin %s", targetname);
			ProxMessage(playerid, str, PROX_AME);

			pInfo[targetid][player_damage] = DAMAGE_LEVEL_BAD;
			mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_bw` = 0, `char_damage` = 2 WHERE `char_uid` = %d", pInfo[targetid][player_id]));
		}

		case DIALOG_EXAMINATION:
		{
			if( !response ) return 1;

			new targetid = DynamicGui_GetValue(playerid, listitem);

			new targetname[MAX_PLAYER_NAME];
			GetPlayerName(targetid, targetname, sizeof(targetname));
			UnderscoreToSpace(targetname);

			new str[64];
			format(str, sizeof(str), "zaczyna bada %s", targetname);

			Alert(playerid, ALERT_TYPE_INFO, "Rozpoczynasz badanie gracza. Potrwa ono ~y~kilka sekund~w~.");

			ApplyAnimation(playerid, "COP_AMBIENT", "Copbrowse_loop", 4.0, 1, 0, 0, 0, 0, 1);

			ProxMessage(playerid, str, PROX_AME);
			defer PlayerExamination[6000](playerid, targetid);
		}

		case DIALOG_HANDCUFFS_SELECT:
		{
			if( !response ) return 1;
			
			new targetid = DynamicGui_GetValue(playerid, listitem);
			
			if( !IsPlayerConnected(targetid) ) return 1;
			if( !pInfo[targetid][player_logged] ) return 1;
			if( pInfo[targetid][player_is_cuffed] ) return SendGuiInformation(playerid, "Wystpi bd", "Ten gracz jest ju skuty przez kogo innego.");

			pInfo[targetid][player_is_cuffed] = true;
			pInfo[targetid][player_cuff_targetid] = playerid;
			GameTextForPlayer(targetid, "~w~Zostales ~r~skuty", 3000, 3);

			for(new i;i<13;i++)
			{
				if( pWeapon[targetid][i][pw_itemid] > -1 ) Item_Use(pWeapon[targetid][i][pw_itemid], targetid);
			}

			GameTextForPlayer(playerid, "~g~Skules ~w~gracza", 3000, 3);
			new itemid = pInfo[playerid][player_dialog_tmp1];
			Item[itemid][item_used] = true;
			Item[itemid][item_value1] = targetid;

			SetPlayerSpecialAction(targetid, SPECIAL_ACTION_CUFFED);
			new freeid = GetPlayerFreeAttachSlot(targetid);
			if( freeid == -1 ) return 1;
			new skin = pInfo[targetid][player_last_skin];
			pInfo[targetid][player_cuff_oindex] = freeid;
			SetPlayerAttachedObject(targetid, freeid, 19418, 6, CuffObjectOffsets[skin][0], CuffObjectOffsets[skin][1], CuffObjectOffsets[skin][2], CuffObjectOffsets[skin][3], CuffObjectOffsets[skin][4], CuffObjectOffsets[skin][5], CuffObjectOffsets[skin][6], CuffObjectOffsets[skin][7], CuffObjectOffsets[skin][8]);
		
			PlayerLog(sprintf("Cuffed player %s", PlayerLogLink(pInfo[targetid][player_id])), pInfo[playerid][player_id], "basic");
			PlayerLog(sprintf("Was cuffed by player %s", PlayerLogLink(pInfo[playerid][player_id])), pInfo[targetid][player_id], "basic");
		}

		case DIALOG_APOMOC:
		{
			if( !response ) return 1;
			
			switch( pInfo[playerid][player_dialog_tmp1] )
			{
				case 1:
				{
					switch( listitem )
					{
						case 0:
						{
							pInfo[playerid][player_dialog_tmp1] = 2;
						
							new str[1200];
							
							strcat(str, "/aflags\tnadawanie flag ekipy\n/duty\tsuba admina\n/spec\tspectate innych graczy\n/specoff\twyczenie spectate");
							strcat(str, "\n/tp\tteleport na x,y,z\n/ptp\tteleport kogos do kogos\n/to\tteleport siebie do kogos\n/goto\tjw\n/fly\tlatanie\n/tm\tteleport kogos do siebie\n/gethere\tjw\n/givecash\tnadawanie kasy\n/i\tnadawanie info do graczy");
							strcat(str, "\n/ac\tczat ekipy\n/hp\tnadawanie hp\n/atime\tczas serwera\n/aweather\tpogoda serwera\n/ado\t/do na cay serwer\n/bw\tnadawanie/zdjemowanie bw\n/setvw\tzmiana virtualworlda\n/setskin\tzmiana skina");
							strcat(str, "\n/arestart\trestart serwera\n/freeze\tzamraanie gracza\n/unfreeze\todmraanie gracza\n/anick\tzmiana nicku gracza\n/ags\tnadawanie gamescore\n/attach\tdo debugowania przyczepialnych\n/kick");
							strcat(str, "\twyrzucenie gracza\n/warn\tostrzeenie gracza\n/block\tblokady postaci\n/unblock\tzdejmowanie blokad\n/ban\tban na konto\n/slap\tslap\n/aj\tadmin jail\n/unaj\tzdejmowanie admin jaila\n--//--");
							strcat(str, "\n/(ag)rupa\tzarzdzanie grupami\n/(av)ehicle\tzarzdzanie pojazdami\n/(ap)rzedmiot\tzarzdzanie przedmiotami\n/(ad)rzwi\tzarzdzanie drzwiami\n/(as)trefa\tzarzdzanie strefami\n/abus\tzarzdzanie");
							strcat(str, " przystankami\n/apaczka\tto byy paczki dla org. przestepczych\n/achangelog\tchangelog serwerowy\n/aspawn\tzarzdzanie miejscami spawnu");
							strcat(str, "");

							ShowPlayerDialog(playerid, DIALOG_APOMOC, DIALOG_STYLE_TABLIST, "Komendy administratora", str, "Wstecz", "");
						}
						
						case 1:
						{
							pInfo[playerid][player_dialog_tmp1] = 2;
							new str[1200];

							strcat(str, "1\tITEM_TYPE_WEAPON\tBro\n2\tITEM_TYPE_AMMO\tAmunicja\n3\tITEM_TYPE_MASK\tMaska\n4\tITEM_TYPE_PHONE\tTelefon");
							strcat(str, "\n5\tITEM_TYPE_FOOD\tJedzenie\n6\tITEM_TYPE_NOTEPAD\tNotatnik\n7\tITEM_TYPE_CHIT\tKartka\n8\tITEM_TYPE_CLOTH\tCiuchy\n9\tITEM_TYPE_DRINK\tNapj\n10\tITEM_TYPE_MEGAFON\tMegafon\n11\tITEM_TYPE_PETY");
							strcat(str, "\tPapierosy\n12\tITEM_TYPE_ZEGAREK\tZegarek\n13\tITEM_TYPE_VEHICLE_MOD\tKomponent pojazdu\n14\tITEM_TYPE_ACCESSORY\tAkcesoria postaci\n15\tITEM_TYPE_CUFFS\tKajdanki\n16\tITEM_TYPE_GYM_PASS\tKarnet");
							strcat(str, " siownia\n17\tITEM_TYPE_MEDICINE\tLeki\n18\tITEM_TYPE_CORPSE\tZwoki\n19\tITEM_TYPE_GLOVES\tRkawiczki\n20\tITEM_TYPE_CD\tPyta CD\n--- Waciciele ---\n1\tITEM_OWNER_TYPE_GROUND\tPodoga\n2\tITEM");
							strcat(str, " _OWNER_TYPE_PLAYER\tGracz\n3\tITEM_OWNER_TYPE_DOOR\tDrzwi (podoga)\n4\tITEM_OWNER_TYPE_VEHICLE\tPojazd\n5\tITEM_OWNER_TYPE_ITEM\tInny przedmiot\n6\tITEM_OWNER_TYPE_PACKAGE\tPaczka\n7\tITEM_OWNER_T");
							strcat(str, " YPE_DOOR_WAREHOUSE\tMagazyn drzwi\n8\tITEM_OWNER_TYPE_VEHICLE_COMPONENT\tPojazd (jako komponent)\n");

							ShowPlayerDialog(playerid, DIALOG_APOMOC, DIALOG_STYLE_TABLIST, "Typy i waciciele przedmiotw", str, "Wstecz", "");
						}

						case 2:
						{
							pInfo[playerid][player_dialog_tmp1] = 2;
							new str[1200];

							strcat(str, "1\tGROUP_TYPE_GOV\tRzd\n2\tGROUP_TYPE_GASTRONOMY\tGastronomia\n3\tGROUP_TYPE_GANG\tGang\n4\tGROUP_TYPE_SPEDITION\tSpedycja\n5");
							strcat(str, "\tGROUP_TYPE_24/7\tSklep 24/7\n6\tGROUP_TYPE_RADIO\tStacja radiowa\n7\tGROUP_TYPE_PD\tPolicja\n8\tGROUP_TYPE_FD\tStra poarna\n9\tGROUP_TYPE_WARSZTAT\tWarsztat samochodowy\n10\tGROUP_TYPE_GASTRONOMY_KO");
							strcat(str, "NC\tGastronomia z koncesj\n11\tGROUP_TYPE_EMS\tPogotowie\n12\tGROUP_TYPE_TAXI\tFirma taks.\n13\tGROUP_TYPE_BINCO\tSklep odzieowy\n14\tGROUP_TYPE_BANK\tBank\n15\tGROUP_TYPE_CARS_SHOP\tSalon samoch.\n16\t");
							strcat(str, "GROUP_TYPE_HOTEL\tHotele\n17\tGROUP_TYPE_SOCIAL_HOUSE\tSpdzielnia\n18\tGROUP_TYPE_MAFIA\tMafia\n19\tGROUP_TYPE_FBI\tFBI\n20\tGROUP_TYPE_GYM\tSiownia\n21\tGROUP_TYPE_FASTFOOD\tFastfood\n");

							ShowPlayerDialog(playerid, DIALOG_APOMOC, DIALOG_STYLE_TABLIST, "Typy grup", str, "Wstecz", "");
						}

						case 3:
						{
							pInfo[playerid][player_dialog_tmp1] = 2;
							new str[1200];

							strcat(str, "--- Waciciele drzwi ---\t  \t  \n1\tDOOR_OWNER_TYPE_GLOBAL\tGlobalne\n2\tDOOR_OWNER_TYPE_PLAYER\tGracz\n3\tDOOR_OWNE");
							strcat(str, "R_TYPE_GROUP\tGrupa\n--- Waciciele pojazdw ---\n1\tVEHICLE_OWNER_TYPE_PLAYER\tGracz\n2\tVEHICLE_OWNER_TYPE_GROUP\tGrupa\n--- Typy stref ---\n1\tAREA_TYPE_NORMAL\tZwyka\n6\tAREA_TYPE_ATM\tBankomat\n7\tARE");
							strcat(str, "A_TYPE_BUS\tPrzystanek\n8\tAREA_TYPE_PETROL\tStacja paliw\n--- Waciciele stref ---\n1\tAREA_OWNER_TYPE_GLOBAL\tGlobalna\n2\tAREA_OWNER_TYPE_GROUP\tGrupa\n3\tAREA_OWNER_TYPE_PLAYER\tGracz");

							ShowPlayerDialog(playerid, DIALOG_APOMOC, DIALOG_STYLE_TABLIST, "Typy i waciciele przedmiotw", str, "Wstecz", "");
						}
					}
				}

				case 2: 
				{
					return cmd_apomoc(playerid, "");
				}
			}
		}

		case DIALOG_INSURANCE:
		{
			if(!response) return SendClientMessage(playerid, 0xD8D8D8FF, "Pracownik ubezpieczalni mwi: Przykro mi, e nie mog pomc. Do zobaczenia!");

			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_INSURANCE_SMART: ShowPlayerDialog(playerid, DIALOG_INSURANCE_SMART, DIALOG_STYLE_MSGBOX, "Potwierdzenie wyboru ubezpieczenia", HEX_COLOR_SAMP"Wybrae opcj pakietu ubezpieczenia "HEX_COLOR_WHITE"SMART"HEX_COLOR_SAMP".\n\n"HEX_COLOR_WHITE"W pakiecie otrzymujesz:\n"HEX_COLOR_SAMP"- Refundacje 30% kosztw leczenia szpitalnego.\n\n"HEX_COLOR_WHITE"Koszty:\n"HEX_COLOR_SAMP"- 10% z kadej otrzymywanej wypaty.\n\n"HEX_COLOR_LIGHTER_RED"Czy chcesz wybra ten pakiet?", "Tak", "Nie");
				case DG_INSURANCE_STD: ShowPlayerDialog(playerid, DIALOG_INSURANCE_STD, DIALOG_STYLE_MSGBOX, "Potwierdzenie wyboru ubezpieczenia", HEX_COLOR_SAMP"Wybrae opcj pakietu ubezpieczenia "HEX_COLOR_WHITE"STANDARD"HEX_COLOR_SAMP".\n\n"HEX_COLOR_WHITE"W pakiecie otrzymujesz:\n"HEX_COLOR_SAMP"- Refundacje 75% kosztw leczenia szpitalnego.\n- Leki w aptece tasze o 30%\n\n"HEX_COLOR_WHITE"Koszty:\n"HEX_COLOR_SAMP"- 20% z kadej otrzymywanej wypaty.\n- Jednorazowa opata $50.\n\n"HEX_COLOR_LIGHTER_RED"Czy chcesz wybra ten pakiet?", "Tak", "Nie");
				case DG_INSURANCE_PRO: ShowPlayerDialog(playerid, DIALOG_INSURANCE_PRO, DIALOG_STYLE_MSGBOX, "Potwierdzenie wyboru ubezpieczenia", HEX_COLOR_SAMP"Wybrae opcj pakietu ubezpieczenia "HEX_COLOR_WHITE"PRO"HEX_COLOR_SAMP".\n\n"HEX_COLOR_WHITE"W pakiecie otrzymujesz:\n"HEX_COLOR_SAMP"- Refundacje 100% kosztw leczenia szpitalnego.\n- Leki w aptece tasze o 85%\n\n"HEX_COLOR_WHITE"Koszty:\n"HEX_COLOR_SAMP"- 35% z kadej otrzymywanej wypaty.\n- Jednorazowa opata $1000.\n\n"HEX_COLOR_LIGHTER_RED"Czy chcesz wybra ten pakiet?", "Tak", "Nie");
				
				case DG_INSURANCE_CANCEL: ShowPlayerDialog(playerid, DIALOG_INSURANCE_CANCEL, DIALOG_STYLE_MSGBOX, "Potwierdzenie rezygnacji", "Czy chcesz na pewno chcesz zrezygnowa z usug ubezpieczalni? Nie zostan Ci zwrcone adne poniesione przez Ciebie koszta.", "Tak", "Nie");
			}
		}

		case DIALOG_INSURANCE_SMART:
		{
			if(!response) return 1;

			pInfo[playerid][player_insurance] = INSURANCE_TYPE_SMART;

			mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_insurance = %d WHERE char_uid = %d", pInfo[playerid][player_insurance], pInfo[playerid][player_id]));

			ProxMessage(playerid, "podpisuje umow z ubezpieczalni.", PROX_AME);
			SendGuiInformation(playerid, "Sukces", "Pomylnie podpisano umow z ubezpieczalni.\n\nPakiet: {FFFFFF}SMART.");
		}

		case DIALOG_INSURANCE_STD:
		{
			if(!response) return 1;

			if(pInfo[playerid][player_money] < 50) return SendGuiInformation(playerid, "Bd", "Nie posiadasz wymaganej iloci gotwki!");

			pInfo[playerid][player_insurance] = INSURANCE_TYPE_STD;
			GivePlayerMoney(playerid, -50);

			mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_insurance = %d WHERE char_uid = %d", pInfo[playerid][player_insurance], pInfo[playerid][player_id]));

			ProxMessage(playerid, "podpisuje umow z ubezpieczalni.", PROX_AME);
			SendGuiInformation(playerid, "Sukces", "Pomylnie podpisano umow z ubezpieczalni.\n\nPakiet: {FFFFFF}STANDARD.");
		}

		case DIALOG_INSURANCE_PRO:
		{
			if(!response) return 1;

			if(pInfo[playerid][player_money] < 1000) return SendGuiInformation(playerid, "Bd", "Nie posiadasz wymaganej iloci gotwki!");

			pInfo[playerid][player_insurance] = INSURANCE_TYPE_PRO;
			GivePlayerMoney(playerid, -1000);

			mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_insurance = %d WHERE char_uid = %d", pInfo[playerid][player_insurance], pInfo[playerid][player_id]));

			ProxMessage(playerid, "podpisuje umow z ubezpieczalni.", PROX_AME);
			SendGuiInformation(playerid, "Sukces", "Pomylnie podpisano umow z ubezpieczalni.\n\nPakiet: {FFFFFF}PRO.");
		}

		case DIALOG_INSURANCE_CANCEL:
		{
			if(!response) return 1;

			pInfo[playerid][player_insurance] = INSURANCE_TYPE_NONE;
			mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_insurance = %d WHERE char_uid = %d", pInfo[playerid][player_insurance], pInfo[playerid][player_id]));

			ProxMessage(playerid, "rozwizuje umow z ubezpieczalni.", PROX_AME);
			SendGuiInformation(playerid, "Sukces", "Pomylnie rozwizano umow z ubezpieczalni.");
		}

		case DIALOG_GOV:
		{
			if(!response) return SendClientMessage(playerid, 0xD8D8D8FF, "Urzdnik mwi: Miego dnia, do widzenia.");

			switch( DynamicGui_GetValue(playerid, listitem) )
			{
				case DG_GOV_DRIVER_LICENSE:
				{
					if( PlayerHasDocument(playerid, DOCUMENT_DRIVE) ) return SendClientMessage(playerid, 0xD8D8D8FF, "Urzdnik mwi: Widz, e posiada Pan ju prawo jazdy. Nie mog go zatem wyrobi.");

					new resp = SetOffer(INVALID_PLAYER_ID, playerid, OFFER_TYPE_DOCUMENT, 50, DOCUMENT_DRIVE);
			
					if( resp ) ShowPlayerOffer(playerid, INVALID_PLAYER_ID, "Dokument", "Prawo jazdy", 150);
				}

				case DG_GOV_ID:
				{
					if( PlayerHasDocument(playerid, DOCUMENT_ID) ) return SendClientMessage(playerid, 0xD8D8D8FF, "Urzdnik mwi: Widz, e posiada Pan ju dowd osobisty. Nie mog go zatem wyrobi.");

					new resp = SetOffer(INVALID_PLAYER_ID, playerid, OFFER_TYPE_DOCUMENT, 0, DOCUMENT_ID);
			
					if( resp ) ShowPlayerOffer(playerid, INVALID_PLAYER_ID, "Dokument", "Dowod osobisty", 50);
				}

				case DG_GOV_JOB:
				{
					if( pInfo[playerid][player_job] != 0 )
					{
						SendClientMessage(playerid, 0xD8D8D8FF, "Urzdnik mwi: Gotowe.");
						return cmd_praca(playerid, "opusc");
					}

					DynamicGui_Init(playerid);
					
					DynamicGui_AddRow(playerid, WORK_TYPE_SALESMAN);
					DynamicGui_AddRow(playerid, WORK_TYPE_PAPERMAN);
					DynamicGui_AddRow(playerid, WORK_TYPE_STORAGEMAN);
					DynamicGui_AddRow(playerid, WORK_TYPE_FISHMAN);
					
					ShowPlayerDialog(playerid, DIALOG_WORKS, DIALOG_STYLE_LIST, "Dostpne prace dorywcze:", "> Sprzedawca\n> Roznosiciel gazet\n> Magazynier\n> Rybak", "Wybierz", "Zamknij");
				}

				case DG_GOV_RENT:
				{
					//TODO: oplacanie budynku
				}

				case DG_GOV_BUSINESS:
				{
					//TODO: dorobic
					SendClientMessage(playerid, -1, "Urzdnik mwi: Przykro mi. Niestety nie mog zarejestrowa firmy bez uzyskania zgody.");
					Alert(playerid, ALERT_TYPE_NEGATIVE, "Aby uzyc tej opcji najpierw musisz przejsc pozytywnie przez proces zakladania firmy na forum.");
				}
			}
		}

		case DIALOG_OFFER:
		{
			if(!response) return OnPlayerOfferResponse(playerid, 0);

			OnPlayerOfferResponse(playerid, 1);
		}

		case DIALOG_OFFER_HIGH:
		{
			if(!response) return OnPlayerOfferResponse(playerid, 0);
			if(strcmp(inputtext, "potwierdzam")) return OnPlayerOfferResponse(playerid, 0);

			OnPlayerOfferResponse(playerid, 1);
		}

		case DIALOG_ITEMS_DRUGS_DIVIDE:
		{
			if( !response )
			{
				DynamicGui_Init(playerid);
				
				DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, pInfo[playerid][player_dialog_tmp2]);
				pInfo[playerid][player_dialog] = DIALOG_PLAYER_ITEMS;
				OnDialogResponse(playerid, DIALOG_PLAYER_ITEMS, 0, 0, "");
				
				return 1;
			}

			new itemid = pInfo[playerid][player_dialog_tmp2];

			new str[150];
			format(str, sizeof(str), "Dzielenie przedmiotu  %s", Item[itemid][item_name]);

			new amount;
			if( sscanf(inputtext, "d", amount) ) return ShowPlayerDialog(playerid, DIALOG_ITEMS_DRUGS_DIVIDE, DIALOG_STYLE_INPUT, str, sprintf("Wpisz ile sztuk przedmiotu chcesz wydzieli(max %d):\n\n"HEX_COLOR_LIGHTER_RED"Musisz poda ilo sztuk.",Item[itemid][item_amount]-1), "Podziel", "Anuluj");
			if( amount <= 0 || amount > Item[itemid][item_amount]-1 ) return ShowPlayerDialog(playerid, DIALOG_ITEMS_DRUGS_DIVIDE, DIALOG_STYLE_INPUT, str, sprintf("Wpisz ile sztuk przedmiotu chcesz wydzieli(max %d):\n\n"HEX_COLOR_LIGHTER_RED"Nie moesz wydzieli tylu sztuk",Item[itemid][item_amount]-1), "Podziel", "Anuluj");

			Item[itemid][item_amount] -= amount;
			mysql_tquery(g_sql, sprintf("UPDATE crp_items SET item_amount = %d WHERE item_uid = %d", Item[itemid][item_amount], Item[itemid][item_uid]));

			Item_Create(Item[itemid][item_owner_type], GetPlayerByUid(Item[itemid][item_owner]), Item[itemid][item_type], Item[itemid][item_model], Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_name], amount);

			GameTextForPlayer(playerid, "~g~Przedmiot zostal rozdzielony", 3000, 3);

			DynamicGui_Init(playerid);
				
			DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, pInfo[playerid][player_dialog_tmp2]);
			pInfo[playerid][player_dialog] = DIALOG_PLAYER_ITEMS;
			OnDialogResponse(playerid, DIALOG_PLAYER_ITEMS, 0, 0, "");
		}

		case DIALOG_ITEMS_DRUGS_JOIN:
		{
			if( !response )
			{
				DynamicGui_Init(playerid);
				
				DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, pInfo[playerid][player_dialog_tmp5]);
				OnDialogResponse(playerid, DIALOG_PLAYER_ITEMS, 0, 0, "");
				
				return 1;
			}

			new item = DynamicGui_GetValue(playerid, listitem);
			new itemid = pInfo[playerid][player_dialog_tmp5];

			PlayerLog(sprintf("Merged item %s {NAME:%s,TYPE:%d,AMOUNT:%d} with item %s {NAME:%s,TYPE:%d,AMOUNT:%d}", ItemLogLink(Item[item][item_uid]), Item[item][item_name], Item[item][item_type], Item[item][item_amount], ItemLogLink(Item[itemid][item_uid]), Item[itemid][item_name], Item[itemid][item_type], Item[itemid][item_amount]), pInfo[playerid][player_id], "item");

			Item[itemid][item_amount] += Item[item][item_amount];
			mysql_tquery(g_sql, sprintf("UPDATE crp_items SET item_amount = %d WHERE item_uid = %d", Item[itemid][item_amount], Item[itemid][item_uid]));

			DeleteItem(item, true);

			GameTextForPlayer(playerid, "~g~Przedmiot zostal polaczony", 3000, 3);

			DynamicGui_Init(playerid);
				
			DynamicGui_AddRow(playerid, DG_ITEMS_MORE_DRUGS_JOIN, itemid);
			pInfo[playerid][player_dialog] = DIALOG_PLAYER_ITEMS;
			OnDialogResponse(playerid, DIALOG_ITEM_MORE, 1, 0, "");

			return 1;
		}

		case DIALOG_GROUP_MEMBERS:
		{
			if( !response ) return PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);

			new char_uid = DynamicGui_GetValue(playerid, listitem);

			pInfo[playerid][player_dialog_tmp2] = DynamicGui_GetDataInt(playerid, listitem);

			new char_name[MAX_PLAYER_NAME];
			GetPlayerNameByUid(char_uid, char_name);

			pInfo[playerid][player_dialog_tmp1] = char_uid;

			ShowPlayerDialog(playerid, DIALOG_GROUP_MEMBERS_INPUT, DIALOG_STYLE_INPUT, "Edycja uprawnie", sprintf("Aby wprowadzi edycj w uprawnieniach "HEX_COLOR_WHITE"%s "HEX_COLOR_SAMP"wprowad poniej uprawnienia\nz listy dodajc prefix +, jeeli chcesz doda uprawnienia lub - jeeli chcesz je usun.\n\nPrzykad: {FFFFFF}+A"HEX_COLOR_SAMP", jezeli chcesz dodac uprawnienia do zarzadzania lub {FFFFFF}-BC "HEX_COLOR_SAMP"jezeli chcesz odebrac uprawnienia do bram i chatu.", char_name), "Zmie", "Anuluj");
		}

		case DIALOG_GROUP_MEMBERS_INPUT:
		{
			if (!response) return PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);

			new input[64];
			format(input, 64, inputtext);

			new char_uid = pInfo[playerid][player_dialog_tmp1];
			new char_perms = pInfo[playerid][player_dialog_tmp2];
			new targetid = IsPlayerOnlinyByUid(char_uid);
			new count = 0;

			switch(input[0])
			{
				case '+':
				{
					for(new i = 1; i<strlen(input); i++)
					{
						switch(input[i])
						{
							case 'A','a': if( !(char_perms & WORKER_FLAG_LEADER)   )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm + %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_LEADER, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'B','b': if( !(char_perms & WORKER_FLAG_GATES)    )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm + %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_GATES, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'C','c': if( !(char_perms & WORKER_FLAG_CHAT)     )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm + %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_CHAT, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;			
							case 'D','d': if( !(char_perms & WORKER_FLAG_VEHICLES) )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm + %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_VEHICLES, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'E','e': if( !(char_perms & WORKER_FLAG_OFFER)    )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm + %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_OFFER, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'F','f': if( !(char_perms & WORKER_FLAG_DOORS)    )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm + %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_DOORS, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'G','g': if( !(char_perms & WORKER_FLAG_ORDER)    )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm + %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_ORDER, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
						}
						if(targetid != INVALID_PLAYER_ID && count > 0) InfoboxRight(playerid, 5, "Zmieniono Twoje uprawnienia w grupie. Aby je zaaktualizowac przeloguj sie.");
					}
				}
				case '-':
				{
					for(new i = 1; i<strlen(input); i++)
					{
						switch(input[i])
						{
							case 'A','a': if( char_perms & WORKER_FLAG_LEADER   )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm - %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_LEADER, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'B','b': if( char_perms & WORKER_FLAG_GATES    )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm - %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_GATES, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'C','c': if( char_perms & WORKER_FLAG_CHAT     )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm - %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_CHAT, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;			
							case 'D','d': if( char_perms & WORKER_FLAG_VEHICLES )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm - %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_VEHICLES, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'E','e': if( char_perms & WORKER_FLAG_OFFER    )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm - %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_OFFER, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'F','f': if( char_perms & WORKER_FLAG_DOORS    )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm - %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_DOORS, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
							case 'G','g': if( char_perms & WORKER_FLAG_ORDER    )  mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_perm = group_perm - %d WHERE char_uid = %d AND group_belongs = %d", WORKER_FLAG_ORDER, char_uid, pInfo[playerid][player_dialog_tmp4])), count++;
						}
						if(targetid != INVALID_PLAYER_ID && count > 0) InfoboxRight(playerid, 5, "Zmieniono Twoje uprawnienia w grupie. Aby je zaaktualizowac przeloguj sie.");
					}
				}
				default:
				{
					SendGuiInformation(playerid, "Wystpi bd", "Nie podae prefixu + lub -.");
				}
			}

			PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);
		}

		case DIALOG_CRAFT:
		{
			switch(pInfo[playerid][player_dialog_tmp1])
			{
				case 0:
				{
					if(!response) return 1;

					new drug = DynamicGui_GetValue(playerid, listitem);

					new string[600];

					pInfo[playerid][player_dialog_tmp1] = 1;

					new ingr_count = 0, ingr_count_have = 0;

					for(new i=0;i<10;i++)
					{
						if(DrugSchema[drug][i] == 0) continue;
						new ingr = DrugSchema[drug][i];

						// zliczamy wszystkie skadniki potrzebne
						ingr_count += DrugSchemaAmount[drug][i];

						new count = 0;
						foreach(new itemid : Items)
						{
							if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[itemid][item_owner] == pInfo[playerid][player_id] )
							{
								if( Item[itemid][item_type] == ITEM_TYPE_DRUG_INGR && Item[itemid][item_value1] == ingr )
								{
									count += Item[itemid][item_amount];
								}
							}
						}

						new color[15];
						format(color, sizeof(color), HEX_COLOR_LIGHTER_RED);
						if(count >= DrugSchemaAmount[drug][i]) format(color, sizeof(color), HEX_COLOR_LIGHTER_GREEN);

						// zliczamy ile skadnikw posiada
						if(count > DrugSchemaAmount[drug][i]) ingr_count_have += DrugSchemaAmount[drug][i];
						else ingr_count_have += count;

						format(string, sizeof(string), "%s%s%s\t(%d/%d)\n", string, color, DrugIngr[ingr], count, DrugSchemaAmount[drug][i]);
					}
					
					new Float:pp = floatdiv(ingr_count_have, ingr_count);
					new percent = floatround(pp*100);

					new color[15];
					format(color, sizeof(color), HEX_COLOR_LIGHTER_RED);
					if(percent >= 50) format(color, sizeof(color), HEX_COLOR_LIGHTER_GREEN);

					format(string, sizeof(string), "%s  \nPosiadasz %s%d%% "HEX_COLOR_WHITE"wymaganych skadnikw do utworzenia tego narkotyku.\nWymagane jest minimum 50%% lecz pamitaj, e im wicej skadnikw\nposiadasz tym lepszej jakoci towar wyprodukujesz.", string, color, percent);

					DynamicGui_Init(playerid);
					DynamicGui_SetDialogValue(playerid, percent);
					pInfo[playerid][player_dialog_tmp2] = drug;

					ShowPlayerDialog(playerid, DIALOG_CRAFT, DIALOG_STYLE_TABLIST, sprintf("Craftowanie  %s", DrugTypes[drug]), string, "Gotuj", "Wr");			
				}

				case 1:
				{
					if(!response) return cmd_craft(playerid, "");

					new a_id = GetPlayerArea(playerid, AREA_TYPE_DRUG_COOKER);
					new objectid = Area[a_id][area_owner];
					new drug = pInfo[playerid][player_dialog_tmp2];

					new percent = DynamicGui_GetDialogValue(playerid);

					if(percent < 50)
					{
						pInfo[playerid][player_dialog_tmp1] = 0;
						DynamicGui_Init(playerid);
							
						DynamicGui_AddRow(playerid, drug);
						pInfo[playerid][player_dialog] = DIALOG_CRAFT;
						OnDialogResponse(playerid, DIALOG_CRAFT, 1, 0, "");

						GameTextForPlayer(playerid, "~r~Brak skladnikow", 3000, 3);
						return 1;	
					}

					// zabieramy przedmioty
					for(new i=0;i<10;i++)
					{
						if(DrugSchema[drug][i] == 0) continue;
						new ingr = DrugSchema[drug][i];

						
						new count = DrugSchemaAmount[drug][i];
						for(new itemid=0;itemid<MAX_ITEMS;itemid++)
						{
							if(!Iter_Contains(Items, itemid)) continue;
							if( count == 0 ) break;

							if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[itemid][item_owner] == pInfo[playerid][player_id] )
							{
								if( Item[itemid][item_type] == ITEM_TYPE_DRUG_INGR && Item[itemid][item_value1] == ingr )
								{
									if(Item[itemid][item_amount] > count)
									{
										Item[itemid][item_amount] -= count;
										count = 0;
										mysql_tquery(g_sql, sprintf("UPDATE crp_items SET item_amount = %d WHERE item_uid = %d", Item[itemid][item_amount], Item[itemid][item_uid]));
									}
									else 
									{
										count -= Item[itemid][item_amount];
										DeleteItem(itemid, true);
									}
								}
							}
						}
					}

					// wrzucamy do bazy
					new Cache:result;
					result = mysql_query(g_sql, sprintf("INSERT INTO crp_drug_cook VALUES (null, %d, %d)", percent, drug));
					new drug_cook_id = cache_insert_id();

					cache_delete(result);

					// odpalamy gotowanie
					Object[objectid][object_is_drug_cooked] = true;
					UpdateDynamic3DTextLabelText(Object[objectid][object_label], COLOR_WHITE, "{00FF00}Stol alchemiczny\n"HEX_COLOR_WHITE"Gotowanie\n0%");

					defer DrugCooking[1000](drug_cook_id, objectid, gettime(), gettime()+30);
				}

				case 2:
				{
					if(!response)
					{
						ShowPlayerDialog(playerid, DIALOG_CRAFT, DIALOG_STYLE_INPUT, "Craftowanie  Wybr nazwy", sprintf("Udao Ci si scraftowa narkotyk %s o nadprzcitnej jakoci, moesz nada mu swoj wasn nazw:", DrugTypes[pInfo[playerid][player_dialog_tmp2]]), "Gotowe", "");

						return 1;
					}

					new name[60];
					if(strlen(inputtext) < 6) return ShowPlayerDialog(playerid, DIALOG_CRAFT, DIALOG_STYLE_INPUT, "Craftowanie  Wybr nazwy", sprintf("Udao Ci si scraftowa narkotyk %s o nadprzcitnej jakoci, moesz nada mu swoj wasn nazw:\n"HEX_COLOR_LIGHTER_RED"Nazwa musi zawiera min. 6 znakw", DrugTypes[pInfo[playerid][player_dialog_tmp2]]), "Gotowe", "");

					mysql_escape_string(inputtext, name);

					new amount = random(3)+1;
					new itemid = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_DRUGS, pInfo[playerid][player_dialog_tmp2], floatround(pInfo[playerid][player_dialog_tmp3]*100), name, 1575, amount);
					SendGuiInformation(playerid, "Craftowanie zakoczone", sprintf("Otrzymae:\n\n\t"HEX_COLOR_LIGHTER_GREEN"%dg "HEX_COLOR_WHITE"narkotyku "HEX_COLOR_LIGHTER_RED"%s "HEX_COLOR_WHITE"(jako "HEX_COLOR_LIGHTER_GREEN"%.1f"HEX_COLOR_WHITE")", amount, name, pInfo[playerid][player_dialog_tmp3]));
					
					PlayerLog(sprintf("Crafted drug %s {NAME:%s,AMOUNT:%dg,QUALITY:%.2f}", ItemLogLink(Item[itemid][item_uid]), name, amount, pInfo[playerid][player_dialog_tmp3]), pInfo[playerid][player_id], "craft");
				}
			}
			
		}

		case DIALOG_BOOMBOX:
		{
			if( !response ) return 1;

			new itemid = pInfo[playerid][player_dialog_tmp1];
			new Float:player_pos[4];

			GetPlayerPos(playerid, player_pos[0], player_pos[1], player_pos[2]);
			GetPlayerFacingAngle(playerid, player_pos[3]);

			pInfo[playerid][player_boombox_id] = CreateDynamicObject(2226, player_pos[0], player_pos[1], player_pos[2]-1.0, 0.0, 0.0, player_pos[3]);
			Item[itemid][item_used] = true;

			Area[pInfo[playerid][player_area]][area_boombox_id] = pInfo[playerid][player_boombox_id];

			ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
			ProxMessage(playerid, "odkada boombox na ziemi.", PROX_AME);

			foreach(new p : Player)
			{
				if(pInfo[p][player_area] == pInfo[playerid][player_area] && GetPlayerVirtualWorld(playerid) == GetPlayerVirtualWorld(p))
				{
					PlayAudioStreamForPlayer(p, inputtext, player_pos[0], player_pos[1], player_pos[2], 20.0, 1);
					pInfo[p][player_has_as_mus] = true;

					SendClientMessageToAll(COLOR_YELLOW, sprintf("[D] playing audiostream for p%d", p));
				}
			}

			format(Area[pInfo[playerid][player_area]][area_music_url], 128, inputtext);
		}

		case DIALOG_VEHICLE_BLOCK:
		{
			if( !response ) return 1;

			new vid = GetPlayerVehicleID(playerid);
			if( pInfo[playerid][player_money] < Vehicle[vid][vehicle_block_price] ) return SendGuiInformation(playerid, "Wystpi bd", "Nie posiadasz wymaganej iloci gotwki.");

			GivePlayerMoney(playerid, -Vehicle[vid][vehicle_block_price]);

			new gid = GetGroupByUid(Vehicle[vid][vehicle_block_group]);
			GiveGroupMoney(gid, Vehicle[vid][vehicle_block_price]);

			PlayerLog(sprintf("Removed wheel block from vehicle %s {PRICE:%d}", VehicleLogLink(Vehicle[vid][vehicle_uid]), Vehicle[vid][vehicle_block_price]), pInfo[playerid][player_id], "basic");

			Vehicle[vid][vehicle_block] = false;
			Vehicle[vid][vehicle_block_price] = 0;
			Vehicle[vid][vehicle_block_group] = 0;

			mysql_pquery(g_sql, sprintf("UPDATE crp_vehicles SET vehicle_block = 0, vehicle_blockprice = 0, vehicle_blockgroup = 0 WHERE vehicle_uid = %d", Vehicle[vid][vehicle_uid]));

			SendGuiInformation(playerid, "Informacja", "Blokada zostaa zdjta z tego pojazdu.\n"HEX_COLOR_LIGHTER_RED"Nie zapomnij zaparkowa pojazdu (/v zaparkuj)!");
		}

		case DIALOG_MALOWANIE_CARCOLOR:
		{
			if( !response ) return 1;

			new carcolor = strval(inputtext);
			if( carcolor < 0 || carcolor > 254 ) return ShowPlayerDialog(playerid, DIALOG_MALOWANIE_CARCOLOR, DIALOG_STYLE_INPUT, "Podaj numer koloru", "W poniszym polu podaj numer koloru (0-254):\n\n"HEX_COLOR_LIGHTER_RED"Podany kolor jest bdny.", "Gotowe", "Anuluj");
		
			new vid = GetVehicleByUid(pInfo[playerid][player_carpaint_vuid]);
			pInfo[playerid][player_choosing_carcolor_cur] = carcolor;

			if( pInfo[playerid][player_choosen_carcolor][0] == -1 ) ChangeVehicleColor(vid, pInfo[playerid][player_choosing_carcolor_cur], Vehicle[vid][vehicle_color][1]);
			else ChangeVehicleColor(vid, pInfo[playerid][player_choosen_carcolor][0], pInfo[playerid][player_choosing_carcolor_cur]);

			PlayerTextDrawSetString(playerid, pInfo[playerid][CarColorPicker][2], sprintf("~b~~h~~h~%d~n~~w~Aktualny", pInfo[playerid][player_choosing_carcolor_cur]));
		}
	}
	
	return 1;
}