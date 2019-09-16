#define CONFIG_FILEPATH "honest/config.ini"

new
	Logger:logFile;

new bool:log_nextday = false;
new knocks_bells = -1;

new MySQL:g_sql;

#define MAX_GLOBAL_SPAWNS 100
new Iterator:GlobalSpawns<MAX_GLOBAL_SPAWNS>;

#define MAX_GROUPS 200
#define MAX_GROUP_WORKERS 50
new Iterator:Groups<MAX_GROUPS>;

#undef MAX_OBJECTS
#define MAX_OBJECTS 30000
new Iterator:Objects<MAX_OBJECTS>;

#undef MAX_ACTORS
#define MAX_ACTORS 500
new Iterator:Actors<MAX_ACTORS>;

#define MAX_DOORS 500
new Iterator:Doors<MAX_DOORS>;

#define MAX_AREAS 2000
new Iterator:Areas<MAX_AREAS>;

new Iterator:FirePlaces<MAX_AREAS>;

#define MAX_FIRE_SOURCES 20
new Iterator:FireSources<MAX_FIRE_SOURCES>;

#define MAX_LABELS 1000
new Iterator:Labels<MAX_LABELS>;

new Iterator:Vehicles<MAX_VEHICLES>;

#define MAX_ITEMS 5000
new Iterator:Items<MAX_ITEMS>;

new Iterator:Skins[2]<300>;

#define MAX_BUSES 100
new Iterator:Buses<MAX_BUSES>;

#define PENALTY_DISPLAY_TIME 6000
new PenaltyHideSecure = 0;
new bool:IsPenaltyDisplayed = false;

new Iterator:Spectators<MAX_PLAYERS>;

new players_task_last_execution;
new antycrasher_task_last_execution;

new aRestartExecuting = false;

new playerIncoming[MAX_PLAYERS] = {false, ...};
new bool:playerTeleportedByServer[MAX_PLAYERS];

#define ID_PLAYER_SYNC  207 

#define LoginMethod:: login_method_
#define login_method_CHARACTER 0
#define login_method_GLOBAL 1

#define PAGE_NAME "honest-rp.pl"
#define PAGE_URL "www.honest-rp.pl."
#define SERVER_NAME "Honest RolePlay"
#define LOGIN_AUTH_URL "54.37.138.131/pass_check.php"
#define MODEL_DOWNLOAD_URL "54.37.138.131/auth/models/%s"

// It is used in 24/7 shops /kup command to calculate the buy price for customer
#define SHOP_PRICE_MULTIPLIER 1.2

#define getletter(%0) \
    (((%0) >= 0x61) ? (0x0) : (0x1)) //TRUE = SMALL LETTER, FALSE = CAPITAL LETTER

// Enumerator ustawieñ
enum e_settings
{
	setting_mysql_hostname[20],
	setting_mysql_username[20],
	setting_mysql_password[30],
	setting_mysql_database[15],
	setting_lsn_ad_finish_time,
	setting_server_hour,
	setting_server_weather,
	setting_run_mode,
	setting_gm_start_time,
	setting_remote_download_url[256]
}
new Setting[e_settings];

#define RunMode:: run_mode_

#define run_mode_DEV 	1
#define run_mode_PROD 	2

// Enumerator konta globalnego
enum e_global_account_info
{
	bool:global_logged,
	bool:global_registered,
	global_id,
	global_bad_pass,
	global_join_time,
	global_name[40],
	global_salt[30],
	global_password[80],
	global_ip[20],
	global_premium_time,
	global_game_points,
}
new gInfo[MAX_PLAYERS][e_global_account_info];

// Enumerator sesji
enum e_session_info
{
	session_id,
	session_start_time,
	session_state
}
new sInfo[MAX_PLAYERS][e_session_info];

// Enumerator gracza
enum e_players_info
{
	player_id,
	player_name[MAX_PLAYER_NAME+1],
	player_duty_tmp[MAX_PLAYER_NAME+1],
	player_ip[20],
	player_password[40],
	player_online_time,
	player_last_hour_cpoints,
	player_login_method,
	bool:player_changed_nick,
	player_afk_time,
	player_mute_time,
	player_last_join,
	player_last_activity,
	player_spawn_time,
	Text3D:player_label,
	Text3D:player_description_label,
	player_description[128],
	player_status[5],
	bool:player_freezed,
	bool:player_admin_duty,
	bool:player_admin_duty_died,
	player_admin_duty_time,
	player_admin_duty_start,
	bool:player_admin_fly,
	player_last_vw,
	
	bool:player_group_list_showed,
	
	// Binco
	bool:player_is_in_binco,
	player_binco_skinid,
	
	bool:player_is_in_binco_access,
	player_binco_access_uid,
	player_binco_access_object,
	
	// Przystanki
	bool:player_is_selecting_bus,
	bool:player_selected_bus,
	player_selected_bus_id,
	Float:player_bus_camera[6],
	player_start_bus_id,
	
	bool:player_is_waiting_for_bus,
	player_bus_waiting_start,
	
	bool:player_is_riding_bus,
	player_bus_ride_ends,

	// narkotyki
	bool:player_drug_used,
	player_drug_type,
	player_drug_start,
	player_drug_time,
	player_dependence,

	player_move_key,
	
	player_sex,
	player_age,
	player_strength,
	player_job,
	player_documents,
	player_door,
	player_block,
	player_block_time[5],
	
	player_keep,
	
	player_area,

	player_taken_damage,
	bool:player_taking_damage,
	
	bool:player_phone_call_started,
	player_phone_caller,
	player_phone_receiver,
	player_phone_object_index,
	
	bool:player_looped_anim,

	player_walk_style,
	player_talk_style,
	player_fight_style,

	bool:player_freeze_door,
	
	bool:player_is_walking,
	bool:player_is_sprinting,
	
	player_aj,
	player_aj_end_time,
	
	player_bw,
	player_bw_end_time,
	Float:player_health,
	bool:player_health_changed,
	
	bool:player_overturned,

	player_bw_reason,
	player_bw_killer,
	player_bw_weapon,
	player_bw_vehicle,
	player_bw_vehicle_seat,
	
	player_money,
	player_bank_number,
	player_bank_money,
	
	// /call 333
	player_order_pid,
	player_order_name[40],
	player_order_price,
	
	Float:player_quit_pos[4],
	player_quit_vw,
	player_quit_int,
	player_quit_time,
	
	player_score,
	
	bool:player_qs,
	bool:player_logged,
	bool:player_connected,
	bool:player_afk,
	bool:player_hurted,
	bool:player_belt,
	bool:player_called_incoming,
	player_skin,
	player_last_skin,
	player_spawn,
	player_login_camera_seq,
	player_login_camera_crc,
	
	player_tog,
	player_last_pm_playerid,
	
	bool:player_is_spectated,
	
	// tworzenie stref
	bool:player_creating_area,
	Float:player_carea_point1[3],
	Float:player_carea_point2[3],
	Text3D:player_carea_label[2],
	player_carea_zone,
	player_carea_type,
	Float:player_carea_size,
	
	bool:player_show_hints,

	bool:player_lookup_area,
	
	player_left_area_time,
	
	bool:player_admin_spec,
	player_admin_spec_id,
	
	// LSN 
	bool:player_lsn_live,
	player_lsn_wywiad_starter,
	player_lsn_wywiad_with,
	
	// anty weapon
	bool:player_modified_weapon,
	
	// czat grup
	player_last_group_slot_chat,
	
	// item proto
	bool:player_items_proto_create,
	player_items_proto_create_id,
	
	// attached objects
	bool:player_attached_item_edit,
	player_attached_item_edit_id,
	
	player_held_weapon,
	
	player_drunk_time,
	
	// obiekty
	player_edited_object,
	bool:player_edited_object_no_action,
	Float:player_edited_object_pos[6],
	
	// esel
	player_esel_edited_label,
	player_esel_edited_object,
	
	player_vehicle_target,
	
	player_dialog_tmp1,
	player_dialog_tmp2,
	Float:player_dialog_tmp3,
	player_dialog_tmp4,
	player_dialog_tmp5,
	player_dialog_tmp_string[100],
	player_info_td_id,
	player_freeze_id,
	player_tmp_whisper[180],
	player_tmp_token[16],
	player_tmp_whisper_id,
	
	player_entering_vehicle,
	player_occupied_vehicle,
	player_occupied_vehicle_seat,
	
	player_command_time,

	// kajdanki
	bool:player_is_cuffed,
	player_cuff_oindex,
	player_cuff_targetid,
	
	// kartoteka
	player_files_search_type,
	player_files_profile_uid,
	player_files_list_type,
	
	// blokady
	player_blockades[20],
	
	// koguty
	player_kogut_object,
	player_kogut_vehicle,

	// /o naprawe
	player_repair_vuid,

	// /o malowanie
	bool:player_choosing_carcolor,
	player_choosing_carcolor_cur,
	player_choosen_carcolor[2],
	player_offered_carpaint,
	player_carpaint_vuid,

	bool:player_holding_shift,
	bool:player_holding_space,
	bool:player_holding_alt,
	bool:player_holding_fire,

	bool:player_has_animation,

	bool:player_was_extinguishing,
	
	// interactions
	player_last_intact_press,

	// gym
	bool:player_is_gym_training,
	bool:player_gym_can_workout,
	bool:player_gym_is_pushing,
	player_gym_object,
	player_gym_attach_slot,
	player_gym_rep_start_tick,
	player_gym_rep_next_start,
	player_gym_reps,
	player_last_train,
	bool:player_karnet,
	player_suplement_type,

	// playertexts'
	
	PlayerText:Dashboard,
	PlayerText:DashboardPane[2],
	PlayerText:GroupsListStaticButtons[25],
	PlayerText:GroupsListRow[5],
	PlayerText:GroupDutyTag,
	PlayerText:DoorInfo[3],
	PlayerText:vehicleFuelInfo,
	PlayerText:OfferTD[6],
	PlayerText:informationTd,
	PlayerText:gymTd,
	PlayerText:AreaInfo,
	PlayerText:leftTime,
	PlayerText:BusInfo,
	PlayerText:CarColorPicker[4],

	player_txd_string[60],
	player_txd_count,

	player_speedtimer_id,

	bool:player_interface,
	bool:player_editor,

	player_level,
	player_ac_warns,

	player_used_weapon,

	player_hostname[50],

	bool:player_items_selecting,
	player_selected_items,

	bool:player_list_favorite_items,

	player_dialog,

	bool:player_edit_meters,
	player_edit_meters_vw,
	Float:player_meters_pos[3],

	Float:last_player_speed,

	player_boombox_id,
	bool:player_has_as_mus,

	//Insurance + damage
	player_insurance,
	player_hospitalization_costs,
	player_debit,
	player_damage,
	bool:player_examined
}
new pInfo[MAX_PLAYERS][e_players_info];

// Enumerator grupy gracza
enum e_player_groups
{
	pg_id,
	pg_rank_title[32],
	pg_rank_skin,
	pg_rank_payment,
	pg_rank_perm,
	pg_duty_time,
	pg_duty_start,
	bool:pg_duty,
	pg_last_payday
}
new pGroup[MAX_PLAYERS][5][e_player_groups];

// Enumerator globalnych spawnow
enum e_global_spawns
{
	gspawn_id,
	gspawn_name[100],
	Float:gspawn_pos[4],
	Text3D:gspawn_label
}
new GlobalSpawn[MAX_GLOBAL_SPAWNS][e_global_spawns];

// Enumerator grup
enum e_groups
{
	ORM:ORM_id,
	group_uid,
	group_parent_uid,
	group_type,
	group_flags,
	group_name[64],
	group_bank_money,
	group_activity_points,
	group_value1,
	group_value2,
	bool:group_ooc
}
new Group[MAX_GROUPS][e_groups];

// Enumerator obiektów
enum e_objects
{
	object_uid,
	object_model,
	object_type,
	object_owner_type,
	object_owner,
	object_vw,
	Float:object_pos[6],
	Float:object_stream_distance,
	
	Float:object_gatec_pos[6],
	Float:object_gateo_pos[6],
	bool:object_gate_opened,
	
	Text3D:object_label,
	object_area,
	
	object_drug_cooked_id,
	bool:object_is_drug_cooked,
	bool:object_is_edited,

	bool:object_is_used
}
new Object[MAX_OBJECTS][e_objects];

// Enumerator drzwi
enum e_doors
{
	door_uid,
	door_type,
	door_owner_type,
	door_owner,
	door_area_inner,
	door_area_outer,
	door_name[40],
	door_audio[100],
	bool:door_closed,
	
	bool:door_auto_closing,
	bool:door_car_crosing,
	
	door_payment,

	Float:door_pos[4],
	door_vw,
	door_int,
	
	door_value1,
	
	Float:door_spawn_pos[4],
	door_spawn_vw,
	door_spawn_int,
	door_objects_limit,
	
	door_map_icon,

	Float:door_meters_points[6],
	door_meters,
	door_meters_area
}
new Door[MAX_DOORS][e_doors];

// Actors enumerator
enum e_actors
{
	actor_uid,
	actor_type,
	actor_skin,
	actor_name[40],
	Float:actor_pos_x,
	Float:actor_pos_y,
	Float:actor_pos_z,
	Float:actor_pos_a,
	actor_interior,
	actor_virtual,
	actor_anim[24],
	Text3D:actor_label_id
}
new Actor[MAX_ACTORS][e_actors];

// Enumerator stref
enum e_areas
{
	area_uid,
	area_type,
	area_owner_type,
	area_owner,
	area_objects_limit,
	Float:area_pos[3],
	
	area_map_icon,

	area_music_url[128],
	area_boombox_id,
}
new Area[MAX_AREAS][e_areas];

// Enumerator labeli
enum e_labels
{
	label_uid,
	label_owner,
	label_owner_type,
	label_color,
	label_vw,
	Float:label_draw,
	Float:label_pos[3]
}
new Label[Text3D:MAX_LABELS][e_labels];

// Enumerator pojazdow
enum e_vehicles
{
	vehicle_uid,
	vehicle_model,
	vehicle_owner_type,
	vehicle_owner,
	vehicle_color[2],
	vehicle_paintjob,
	Float:vehicle_park[4],
	vehicle_park_world,
	vehicle_park_interior,
	Text3D:vehicle_description,
	vehicle_has_description,
	Float:vehicle_health,
	bool:vehicle_destroyed,
	Float:vehicle_mileage,
	vehicle_driver,
	vehicle_interior,
	vehicle_register[10],
	vehicle_occupants,
	
	bool:vehicle_locked,
	
	Float:vehicle_last_pos[3],
	
	vehicle_last_fuel_td,
	
	// Zniszczenia wizualne
	vehicle_damage[4],
	
	vehicle_map_icon,
	
	// Wypadki
	Float:vehicle_last_vector_length,
	
	// Paliwo
	vehicle_fuel_type,
	Float:vehicle_fuel_current,
	vehicle_fuel_max,
	
	// Kogut
	vehicle_buzzer_object,
	
	// accessories
	vehicle_accessories,

	bool:vehicle_radio,
	
	// Status
	vehicle_state,
	vehicle_state_time,
	vehicle_state_end,
	Text3D:vehicle_state_label,
	vehicle_state_paytype,
	
	bool:vehicle_engine,
	bool:vehicle_engine_starting,
	bool:vehicle_lights,
	bool:vehicle_boot,
	bool:vehicle_bonnet,

	// malowanie
	vehicle_carpaint_seller,
	vehicle_carpaint_buyer,

	// blokady
	bool:vehicle_block,
	vehicle_block_price,
	vehicle_block_group
}
new Vehicle[MAX_VEHICLES][e_vehicles];

// Enumerator przedmiotów
enum e_items
{
	// Basic data
	item_uid,
	item_name[40],
	item_type,
	item_value1,
	item_value2,
	item_value3,
	item_created,
	item_used,
	bool:item_selected,
	bool:item_favorite,
	
	// Owner data
	item_owner_type,
	item_owner,
	item_group,
	
	// Storage data
	item_price,
	item_amount,
	
	// Ground data
	item_object,
	item_model,
	Float:item_x,
	Float:item_y,
	Float:item_z,
	Float:item_rx,
	Float:item_ry,
	Float:item_rz,
	item_world,
	item_interior,
}
new Item[MAX_ITEMS][e_items];

// Enumerator broni gracza
enum e_player_weapons
{
	pw_itemid,
	pw_id,
	pw_ammo,
	pw_object_index
}
new pWeapon[MAX_PLAYERS][13][e_player_weapons];

enum attached_object_data
{
	ao_model,
	ao_bone,
	Float:ao_x,
	Float:ao_y,
	Float:ao_z,
	Float:ao_rx,
	Float:ao_ry,
	Float:ao_rz,
	Float:ao_sx,
	Float:ao_sy,
	Float:ao_sz
}

new ao[MAX_PLAYERS][MAX_PLAYER_ATTACHED_OBJECTS][attached_object_data];

enum e_player_offer
{
	offer_sellerid,
	offer_buyerid,
	offer_type,
	bool:offer_accepted,
	offer_price,
	offer_extraid,
	offer_start
}
new pOffer[MAX_PLAYERS][e_player_offer];

// Enumerator akcji gracza (oferty)
enum e_player_action
{
	action_buyer_uid,
	action_type,
	action_extraid,
	action_price,
	action_payment_type,
	action_groupid,

	action_time,
	action_time_left,

	// distance check
	bool:action_distance_check,
	Float:action_max_distance,
	Float:action_source_pos[3],
	action_distance_warn_times
}
new pAction[MAX_PLAYERS][e_player_action];
//////////////////////

// Enumerator kar
enum e_penalty_queue
{
	pq_header[64],
	pq_giver_name[MAX_PLAYER_NAME+1],
	pq_obtainer_name[MAX_PLAYER_NAME+1],
	pq_reason[256],
	bool:pg_green,
}
new PenaltyQueue[e_penalty_queue];

// Enumerator skinów
enum e_skins
{
	skin_value,
	skin_price
}
new Skin[300][e_skins];

// Enumerator przystankow
enum e_buses
{
	bus_uid,
	bus_objectid,
	bus_name[60],
	Float:bus_ratio
}
new Bus[MAX_BUSES][e_buses];

#define strContains(%0,%1) ((strfind(%0, %1) != -1) ? (true) : (false))

#define function%0(%1) forward%0(%1); public%0(%1)

new MessageString[250];
#define SendFormattedClientMessage(%0,%1,%2,%3) (format(MessageString, sizeof(MessageString), %2, %3), SendClientMessage(%0, %1, MessageString))

#define abs(%0) ((%0 < 0) ? (-%0) : (%0)) //absolute value for integers
	   
#define PRESSED(%0) (((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))
#define HOLDING(%0) ((newkeys & (%0)) == (%0))
#define RELEASED(%0) (((newkeys & (%0)) != (%0)) && ((oldkeys & (%0)) == (%0)))

#define strtoupper(%0) for(new l = strlen(%0) - 1; l >= 0; l--) %0[l] = toupper(%0[l])

stock _PutPlayerInVehicle(playerid, vehicleid, seat)
{
	pInfo[playerid][player_entering_vehicle] = vehicleid;
	return PutPlayerInVehicle(playerid, vehicleid, seat);
}
#undef PutPlayerInVehicle
#define PutPlayerInVehicle(%0,%1,%2) _PutPlayerInVehicle(%0,%1,%2)

stock hrp_SetVehicleHealth(vehicleid, Float:health)
{
	Vehicle[vehicleid][vehicle_health] = health;
	SetVehicleHealth(vehicleid, health);
}
#define SetVehicleHealth hrp_SetVehicleHealth

stock hrp_LinkVehicleToInterior(vehicleid, interior)
{
	Vehicle[vehicleid][vehicle_interior] = interior;
	LinkVehicleToInterior(vehicleid, interior);
}
#define LinkVehicleToInterior hrp_LinkVehicleToInterior

stock SetPlayerMoney(playerid, money)
{
	pInfo[playerid][player_money] = money;
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, pInfo[playerid][player_money]);
}

stock hrp_GivePlayerMoney(playerid, money)
{
	pInfo[playerid][player_money] += money;	
	mysql_tquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_cash` = %d WHERE `char_uid` = %d", pInfo[playerid][player_money], pInfo[playerid][player_id]));
	GivePlayerMoney(playerid, pInfo[playerid][player_money] - GetPlayerMoney(playerid));
}
#define GivePlayerMoney hrp_GivePlayerMoney

stock _SetPlayerVirtualWorld(playerid, vw)
{
	OnPlayerVirtualWorldChange(playerid, vw);
	SetPlayerVirtualWorld(playerid, vw);
}
#define SetPlayerVirtualWorld(%0,%1) _SetPlayerVirtualWorld(%0, %1)

stock hrp_SetPlayerHealth(playerid, Float:health)
{
	pInfo[playerid][player_health_changed] = true;
	pInfo[playerid][player_health] = health;
	SetPlayerHealth(playerid, health);
}
#define SetPlayerHealth hrp_SetPlayerHealth

stock hrp_TogglePlayerControllable(playerid, toggle)
{
	pInfo[playerid][player_freezed] = !toggle;
	TogglePlayerControllable(playerid, toggle);
}
#define TogglePlayerControllable hrp_TogglePlayerControllable

stock hrp_SetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
	playerTeleportedByServer[playerid] = true;
	SetPlayerPos(playerid, x, y, z);
}
#define SetPlayerPos hrp_SetPlayerPos

stock hrp_SpawnPlayer(playerid)
{
	playerTeleportedByServer[playerid] = true;
	SpawnPlayer(playerid);
}
#define FIXES_SpawnPlayer hrp_SpawnPlayer

stock hrp_TogglePlayerSpectating(playerid, toggle)
{
	if( toggle == 0 ) playerTeleportedByServer[playerid] = true;
	TogglePlayerSpectating(playerid, toggle);
}
#define TogglePlayerSpectating hrp_TogglePlayerSpectating

stock extended_SendClientMessage(playerid, color, message[], length = sizeof message)
{	
	if( strlen(message) > 100 )
	{
		new last_whitespace = -1, line[100], last_color[10];
		for(new i;i < strlen(message); i++)
		{
			if( i > 60 && message[i] == ' ' ) last_whitespace = i;
			
			if( i > 80 )
			{
				if( last_whitespace == -1 ) strmid(line, message, 0, 80);
				else strmid(line, message, 0, last_whitespace);
				
				SendClientMessage(playerid, color, sprintf("%s ...", line));
				
				for(new a = 0; a < strlen(line); a++)
				{
					if(line[a] == '{' && line[a+7] == '}')
					{
						strmid(last_color, line, a, a+7+1);
					}
				}

				if( last_whitespace == -1 ) strmid(line, message, 80, length);
				else strmid(line, message, last_whitespace, length);

				if(isnull(last_color)) SendClientMessage(playerid, color, sprintf("... %s", line));
				else SendClientMessage(playerid, color, sprintf("%s ... %s", last_color, line));

				break;
			}			
		}
	}
	else SendClientMessage(playerid, color, message);

	return 1;
}

#define GroupHasFlag(%0,%1) ((Group[%0][group_flags] & (%1)) == (%1))
#define WorkerHasFlag(%0,%1,%2) ((pGroup[%0][%1][pg_rank_perm] & (%2)) == (%2))
#define PlayerHasTog(%0,%1) ((pInfo[%0][player_tog] & (%1)) == (%1))
#define VehicleHasAccessory(%0,%1) ((Vehicle[%0][vehicle_accessories] & (%1)) == (%1))
#define PlayerHasDocument(%0,%1) ((pInfo[%0][player_documents] & (%1)) == (%1))


/** -------------- ALERTS -------------- **/
#define ALERT_TYPE_WARNING	0
#define ALERT_TYPE_SUCCESS  1
#define ALERT_TYPE_NEGATIVE 2
#define ALERT_TYPE_INFO		3


/** -------------- INFO BOXES --------------------- **/

new PlayerText:InfoBoxTextdraw[7][MAX_PLAYERS];
new InfoBoxChecksum[7][MAX_PLAYERS];

#define InfoBox:: info_box_type_
#define info_box_type_RIGHT 0
#define info_box_type_RIGHT_BIG 1
#define info_box_type_CENTER 2
#define info_box_type_CENTER_SMALL 3
#define info_box_type_UNDER_HUD 4

/** -------------- INFO BOXES --------------------- **/
new Text:vehicleInfo;
new Text:vehicleEngineStarting;
new Text:PlayerVip;
new Text:GroupsListStaticHeader;
new Text:LSNtd;
new Text:LSNtd2;

new Text:PenaltiesTextDraw;

new Text:BlackScreen;
new Text:ZegarekTD;

new PlayerText:SmallAlert[6][MAX_PLAYERS][3];

new Text:HonestLogoLogin;
new Text:HonestLogoAboveHud;

new Text:Panorama[2];

#define COLOR_GREY 0xAFAFAFAA
#define COLOR_RED 0xAA3333AA
#define COLOR_DARK_RED 0xB00000FF
#define COLOR_SMOOTH_RED 0xEA5959FF
#define COLOR_LIGHTER_RED 0xFF3C3CFF
#define COLOR_LIGHTER_GREEN 0xD6EE76FF
#define COLOR_GREEN 0x33AA33FF
#define COLOR_GOLD 0xFCAD43FF
#define COLOR_LIGHT_PURPLE 0x9B91ECFF
#define COLOR_PINK  0xB58ADAFF
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_SAMP 0xA9C4E4FF
#define COLOR_TURQUE 0x00E7EBFF
#define COLOR_YELLOW 0xFFE229FF
#define COLOR_LIGHTER_ORANGE 0xFF8C55FF
#define COLOR_HONEST 0xE5DA67FF

#define LABEL_DESCRIPTION 0x9B91ECFF
#define LABEL_NICK_NORMAL_COLOR 0xf2f2f270
#define LABEL_NICK_DAMAGED_COLOR 0xFF000070
#define LABEL_NICK_CREW_COLOR 0xFFFFFFFF 

#define HEX_COLOR_SMOOTH_RED "{EA5959}"
#define HEX_COLOR_LIGHTER_RED "{FF3C3C}"
#define HEX_COLOR_LIGHTER_GREEN "{D6EE76}"
#define HEX_COLOR_LIGHTER_BLUE "{0098D9}"
#define HEX_COLOR_LIGHTER_YELLOW "{F2E200}"
#define HEX_COLOR_LIGHTER_ORANGE "{FFAA3B}"
#define HEX_COLOR_GOLD "{FCAD43}"
#define HEX_COLOR_RED "{AA3333}"
#define HEX_COLOR_GREEN "{199908}"
#define HEX_COLOR_GREY "{AFAFAF}"
#define HEX_COLOR_WHITE "{FFFFFF}"
#define HEX_COLOR_BROWN "{965701}"
#define HEX_COLOR_SAMP "{A9C4E4}"
#define HEX_COLOR_CREW_GM "{1ffff3}"
#define HEX_COLOR_CREW_ADMIN "{A800A3}"
#define HEX_COLOR_CREW_ADMIN_ROOT "{c10f0f}"
#define HEX_COLOR_HONEST "{E5DA67}"
#define HEX_COLOR_SAMP_DARK "{7DA8DB}"

// Poziomy ekipy
#define CREW_FLAG_GM 			1
#define CREW_FLAG_ADMIN 		2
#define CREW_FLAG_ADMIN_ROOT 	4
#define CREW_FLAG_DOORS 		8
#define CREW_FLAG_VEHICLES		16
#define CREW_FLAG_GROUPS 		32
#define CREW_FLAG_AREAS 		64
#define CREW_FLAG_EDITOR 		128
#define CREW_FLAG_ITEMS 		256

// Prox types
#define PROX_LOCAL 0
#define PROX_SHOUT 1
#define PROX_QUIET 2
#define PROX_ME 3
#define PROX_DO 4
#define PROX_LOCAL_L 5
#define PROX_OOC 6
#define PROX_RADIO 7
#define PROX_PHONE 8
#define PROX_MEGAFON 9
#define PROX_TRY 10
#define PROX_AME 11
#define QS_TIME 600

// Animacje
#define ANIM_TYPE_BW 1
#define ANIM_TYPE_BW_INCAR 2

// Statusy
#define AFK_TIME 3 // w sekundach
#define PLAYER_STATUS_AFK 1
#define PLAYER_STATUS_BELT 2
#define PLAYER_STATUS_HURT 4
#define PLAYER_STATUS_BW 5
#define PLAYER_STATUS_CERPEK 6
#define PLAYER_STATUS_TIPSY 7
#define PLAYER_STATUS_DRUNK 8
#define PLAYER_STATUS_GLOVES 9

// Sesje
#define SESSION_STATE_NONE 0
#define SESSION_STATE_ALREADY_LOGGED 1
#define SESSION_STATE_ABORT 2
#define SESSION_STATE_BAD_PASS 3
#define SESSION_STATE_NORMAL 4
#define SESSION_STATE_LOGIN_TIMEOUT 5

#define SESSION_TYPE_LOGIN 1
#define SESSION_TYPE_DUTY 2
#define SESSION_TYPE_ADMIN_DUTY 3

// Togi
#define TOG_W 1
#define TOG_G1 2
#define TOG_G2 4
#define TOG_G3 8
#define TOG_G4 16
#define TOG_G5 32

// BW reason
#define BW_REASON_SUICIDE 1
#define BW_REASON_FIRE 2
#define BW_REASON_BEAT 3
#define BW_REASON_SHOOT 4

// Obiekty
#define OBJECT_TYPE_NORMAL 1
#define OBJECT_TYPE_ATM 2
#define OBJECT_TYPE_BUS 3
#define OBJECT_TYPE_GATE 4
#define OBJECT_TYPE_DRUG_COOKER 5
#define OBJECT_TYPE_FIRE_SOURCE 6
#define OBJECT_TYPE_GYM_BENCH 7
#define OBJECT_TYPE_GYM_DUMBBELL 8
#define OBJECT_TYPE_TANK 9

#define OBJECT_OWNER_TYPE_GLOBAL 1
#define OBJECT_OWNER_TYPE_DOOR 2
#define OBJECT_OWNER_TYPE_AREA 3
#define OBJECT_OWNER_TYPE_MAP 4

// Drzwi
#define DOOR_TYPE_NORMAL 1

#define DOOR_OWNER_TYPE_GLOBAL 1
#define DOOR_OWNER_TYPE_PLAYER 2
#define DOOR_OWNER_TYPE_GROUP 3

//Aktorzy
#define ACTOR_TYPE_NONE 	 1
#define ACTOR_TYPE_GOV 		 2
#define ACTOR_TYPE_INSURANCE 3

// Strefy
#define AREA_SHAPE_SQUARE 1
#define AREA_SHAPE_CIRCLE 2

#define AREA_TYPE_NORMAL 1
#define AREA_TYPE_DOOR_INNER 2
#define AREA_TYPE_DOOR_OUTER 3
#define AREA_TYPE_CERPEK 4
#define AREA_TYPE_CERPEK2 5
#define AREA_TYPE_ATM 6
#define AREA_TYPE_BUS 7
#define AREA_TYPE_PETROL 8
#define AREA_TYPE_DRUG_COOKER 9
#define AREA_TYPE_FIRE_PLACE 10
#define AREA_TYPE_ACTOR	11
#define AREA_TYPE_DOOR_METERS 12

#define AREA_OWNER_TYPE_GLOBAL 1
#define AREA_OWNER_TYPE_GROUP 2
#define AREA_OWNER_TYPE_PLAYER 3

// 3d teksty
#define LABEL_OWNER_TYPE_GLOBAL 1
#define LABEL_OWNER_TYPE_DOOR 2
#define LABEL_OWNER_TYPE_AREA 3

// Typy grup
#define GROUP_FLAG_IC 				1
#define GROUP_FLAG_OOC 				2
#define GROUP_FLAG_TAX 				4
#define GROUP_FLAG_COLORED			8
#define GROUP_FLAG_DEP 				16
#define GROUP_FLAG_SPAWN 			32
#define GROUP_FLAG_KEEP 			64
#define GROUP_FLAG_BLOCKADE 		128
#define GROUP_FLAG_WEAPON_FLAG 		256
#define GROUP_FLAG_911 				512
#define GROUP_FLAG_RACES 			1024
#define GROUP_FLAG_LEAVE 			2048
#define GROUP_FLAG_MASK 			4096
#define GROUP_FLAG_TAG 				8192
#define GROUP_FLAG_SEARCH 			16384
#define GROUP_FLAG_CUFFS 			32768
#define GROUP_FLAG_DUTY_TAG			65536
#define GROUP_FLAG_DUTY_ANYPLACE	131072
#define GROUP_FLAG_DUTY_INVEHICLE	262144
#define GROUP_FLAG_EXAMINE_CORPSE	524288	
#define GROUP_FLAG_FILES			1048576
#define GROUP_FLAG_BUZZER			2097152

new WeaponNames[45][] = {
	"Fist",
	"Brass Knuckles",
	"Golf Club",
	"Nightstick",
	"Knife",
	"Baseball Bat",
	"Shovel",
	"Pool Cue",
	"Katana",
	"Chainsaw",
	"Purple Dildo",
	"Dildo",
	"Vibrator",
	"Silver Vibrator",
	"Flowers",
	"Grenade",
	"Tear Gas",
	"Molotov Cocktail",
	"Colt-45 9mm",
	"Silenced Colt-45 9mm",
	"Desert Eagle",
	"Sawnoff Shotgun",
	"Combat Shotgun",
	"Micro SMG/Uzi",
	"MP5",
	"AK-47",
	"M4",
	"Tec-9",
	"Country Rifle",
	"Sniper Rifle",
	"RPG",
	"HS Rocket",
	"Flamethrower",
	"Minigun",
	"Satchel Charge",
	"Detonator",
	"Spraycan",
	"Fire Extinguisher",
	"Camera",
	"Night Vis Goggles",
	"Thermal Goggles",
	"Parachute",
	"Cellphone",
	"Jetpack",
	"Skateboard"
};

new AdminLevels[7][] = {
	"", //instead of +1 in variables
	"Support 1",
	"Support 2",
	"Support 3",
	"Support 4",
	"Administrator 1",
	"Administrator 2"
};

new EmbededAdminLevels[7][] = {
	"", //instead of +1 in variables
	"{89afd3}Support 1",
	"{89afd3}Support 2",
	"{89afd3}Support 3",
	"{89afd3}Support 4",
	"{ed6161}Administrator 1",
	"{ed6161}Administrator 2 (CEO)"
};

new GroupFlags[22][] = {
	"Czat IC",
	"Czat OOC",
	"Zobowi¹zany do p³acenia podatku",
	"Kolorowy nick na s³u¿bie",
	"Czat s³u¿b publicznych",
	"Spawn w budynku grupowym",
	"Mo¿liwoœæ przetrzymywania",
	"Stawianie blokad",
	"U¿ywanie oflagowanych broni",
	"Listowanie na 911",
	"Tworzenie wyœcigów",
	"Opuszczanie grupy",
	"U¿ywanie masek",
	"Tagowanie",
	"Przeszukiwanie",
	"U¿ywanie kajdanek",
	"Tag grupy na s³u¿bie",
	"Naliczanie s³u¿by wszêdzie",
	"Naliczanie s³u¿by w pojazdach",
	"Przeprowadzanie sekcji zw³ok",
	"Dostêp do kartoteki",
	"Mo¿liwoœc u¿ycia koguta"
};

new GroupFlagsBit[22] = {
	1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152
}; 

#define WORKER_FLAG_CHAT 1
#define WORKER_FLAG_DOORS 2
#define WORKER_FLAG_VEHICLES 4
#define WORKER_FLAG_OFFER 8
#define WORKER_FLAG_ORDER 16
#define WORKER_FLAG_LEADER 32
#define WORKER_FLAG_GATES 64

#define GROUP_TYPE_GOV 1
#define GROUP_TYPE_GASTRONOMY 2
#define GROUP_TYPE_GANG 3
#define GROUP_TYPE_SPEDITION 4
#define GROUP_TYPE_24/7 5
#define GROUP_TYPE_RADIO 6
#define GROUP_TYPE_PD 7
#define GROUP_TYPE_FD 8
#define GROUP_TYPE_WARSZTAT 9
#define GROUP_TYPE_GASTRONOMY_KONC 10
#define GROUP_TYPE_EMS 11
#define GROUP_TYPE_TAXI 12
#define GROUP_TYPE_BINCO 13
#define GROUP_TYPE_BANK 14
#define GROUP_TYPE_CARS_SHOP 15
#define GROUP_TYPE_HOTEL 16
#define GROUP_TYPE_SOCIAL_HOUSE 17
#define GROUP_TYPE_MAFIA 18
#define GROUP_TYPE_FBI 19
#define GROUP_TYPE_GYM 20
#define GROUP_TYPE_FASTFOOD 21

new GroupTypes[][30] = {
	"Nieokreœlony",
	"Rz¹d",
	"Gastronomia",
	"Gang",
	"Spedycja",
	"24/7",
	"Stacja radiowa",
	"Policja",
	"Stra¿ po¿arna",
	"Warsztat",
	"Gastronomia + Koncesja",
	"Pogotowie",
	"Firma taksówkarska",
	"Binco",
	"Bank",
	"Salon samochodowy",
	"Hotel",
	"Spó³dzielnia socjalna",
	"Mafia",
	"FBI",
	"Si³ownia",
	"Fastfood"
};

new GroupDefaultFlags[] = {
	GROUP_FLAG_IC + GROUP_FLAG_OOC,
	GROUP_FLAG_IC + GROUP_FLAG_OOC + GROUP_FLAG_COLORED + GROUP_FLAG_DEP + GROUP_FLAG_DUTY_TAG + GROUP_FLAG_DUTY_INVEHICLE, // Rzad
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Gastro
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Gang
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Sped
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // 24/7
	GROUP_FLAG_IC + GROUP_FLAG_OOC + GROUP_FLAG_COLORED + GROUP_FLAG_DEP + GROUP_FLAG_DUTY_TAG + GROUP_FLAG_911 + GROUP_FLAG_DUTY_ANYPLACE + GROUP_FLAG_DUTY_INVEHICLE, // Stacje radiowe
	GROUP_FLAG_IC + GROUP_FLAG_OOC + GROUP_FLAG_COLORED + GROUP_FLAG_DEP + GROUP_FLAG_DUTY_TAG + GROUP_FLAG_CUFFS + GROUP_FLAG_911 + GROUP_FLAG_TAX + GROUP_FLAG_MASK + GROUP_FLAG_BLOCKADE + GROUP_FLAG_WEAPON_FLAG + GROUP_FLAG_SEARCH + GROUP_FLAG_DUTY_ANYPLACE, // Policja
	GROUP_FLAG_IC + GROUP_FLAG_OOC + GROUP_FLAG_COLORED + GROUP_FLAG_DEP + GROUP_FLAG_DUTY_TAG + GROUP_FLAG_911 + GROUP_FLAG_TAX + GROUP_FLAG_DUTY_ANYPLACE, // Stra¿ po¿
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Warsztat
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Gastro + koncesja
	GROUP_FLAG_IC + GROUP_FLAG_OOC + GROUP_FLAG_COLORED + GROUP_FLAG_DEP + GROUP_FLAG_DUTY_TAG + GROUP_FLAG_911 + GROUP_FLAG_TAX + GROUP_FLAG_DUTY_ANYPLACE, // EMS
	GROUP_FLAG_IC + GROUP_FLAG_OOC + GROUP_FLAG_DUTY_ANYPLACE, // Taxi
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Binco
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Bank
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Sklep samochodowy
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Hotel
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Socjalka
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Mafia
	GROUP_FLAG_IC + GROUP_FLAG_OOC + GROUP_FLAG_COLORED + GROUP_FLAG_DEP + GROUP_FLAG_DUTY_TAG + GROUP_FLAG_CUFFS + GROUP_FLAG_911 + GROUP_FLAG_TAX + GROUP_FLAG_MASK + GROUP_FLAG_BLOCKADE + GROUP_FLAG_WEAPON_FLAG + GROUP_FLAG_SEARCH + GROUP_FLAG_DUTY_ANYPLACE, // FBI
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Si³ownia
	GROUP_FLAG_IC + GROUP_FLAG_OOC, // Fastfood
};

#define ORDER_FOOD 1
#define ORDER_PHONES 2
#define ORDER_SHITS 4
#define ORDER_LSPD 8
#define ORDER_GANG 16
#define ORDER_DRINK 32
#define ORDER_BEVERAGE 64
#define ORDER_LSN 128
#define ORDER_SYND 256
#define ORDER_FBI 512
#define ORDER_GYM 1024

// Pojazdy
#define VEHICLE_OWNER_TYPE_PLAYER 1
#define VEHICLE_OWNER_TYPE_GROUP 2

#define VEHICLE_TYPE_AIRPLANE 1
#define VEHICLE_TYPE_HELICOPTER 2
#define VEHICLE_TYPE_BIKE 3
#define VEHICLE_TYPE_MOTORBIKE 4
#define VEHICLE_TYPE_CAR 5
#define VEHICLE_TYPE_TRAILER 6
#define VEHICLE_TYPE_CARGO 7
#define VEHICLE_TYPE_TRUCK 8
#define VEHICLE_TYPE_BOAT 9

#define VEHICLE_FUEL_TYPE_BENZYNA 1
#define VEHICLE_FUEL_TYPE_GAZ 2
#define VEHICLE_FUEL_TYPE_DIESEL 3
#define VEHICLE_FUEL_TYPE_LOTNICZE 4

#define VEHICLE_ACCESSORY_IMMOBILISER 1
#define VEHICLE_ACCESSORY_ALARM 2
#define VEHICLE_ACCESSORY_AUDIO 4
#define VEHICLE_ACCESSORY_WINDOWS 8

#define BENZYNA_MULTIPLIER 1.1
#define GAZ_MULTIPLIER 1.3
#define DIESEL_MULTIPLIER 0.8
#define LOTNICZE_MULTIPLIER 2.0

#define FUEL_DRAIN_INCREASE_MILEAGE_CAP 10000

new VehicleFuelTypes[5][20] =
{
	"Brak",
	"Benzyna",
	"Gaz",
	"Ropa naftowa",
	"Paliwo lotnicze"
};

#define VEHICLE_STATE_FUELING 1
#define VEHICLE_STATE_REPAIRING 2
#define VEHICLE_STATE_SELLING 3
#define VEHICLE_STATE_PAINT 4

//Obra¿enia
#define DAMAGE_LEVEL_NONE     0
#define DAMAGE_LEVEL_LOW      1
#define DAMAGE_LEVEL_BAD      2
#define DAMAGE_LEVEL_CRITICAL 3

// Przedmioty
#define ITEM_OWNER_TYPE_GROUND 1
#define ITEM_OWNER_TYPE_PLAYER 2
#define ITEM_OWNER_TYPE_DOOR 3
#define ITEM_OWNER_TYPE_VEHICLE 4
#define ITEM_OWNER_TYPE_ITEM 5
#define ITEM_OWNER_TYPE_PACKAGE 6
#define ITEM_OWNER_TYPE_DOOR_WAREHOUSE 7
#define ITEM_OWNER_TYPE_VEHICLE_COMPONENT 8
#define ITEM_OWNER_TYPE_BAG 9

#define WEAPON_TYPE_MELEE 1
#define WEAPON_TYPE_SHORT 2
#define WEAPON_TYPE_LONG 3
#define WEAPON_TYPE_GRENADE 4
#define WEAPON_TYPE_SPECIAL 5
#define WEAPON_TYPE_SNIPERS 6

#define ITEM_TYPE_WEAPON 1
#define ITEM_TYPE_AMMO 2
#define ITEM_TYPE_MASK 3
#define ITEM_TYPE_PHONE 4
#define ITEM_TYPE_FOOD 5
#define ITEM_TYPE_NOTEPAD 6
#define ITEM_TYPE_CHIT 7
#define ITEM_TYPE_CLOTH 8
#define ITEM_TYPE_DRINK 9
#define ITEM_TYPE_MEGAFON 10
#define ITEM_TYPE_PETY 11
#define ITEM_TYPE_ZEGAREK 12
#define ITEM_TYPE_VEHICLE_MOD 13
#define ITEM_TYPE_ACCESSORY 14
#define ITEM_TYPE_CUFFS 15
#define ITEM_TYPE_GYM_PASS 16
#define ITEM_TYPE_MEDICINE 17
#define ITEM_TYPE_CORPSE 18
#define ITEM_TYPE_GLOVES 19
#define ITEM_TYPE_CD 20
#define ITEM_TYPE_DRUGS 21
#define ITEM_TYPE_DRUG_INGR 22
#define ITEM_TYPE_BOOMBOX 23
#define ITEM_TYPE_EXAMINATION 24
#define ITEM_TYPE_ADRENALINE 25
#define ITEM_TYPE_BAG 26

#define SUPLEMENT_TYPE_TIME 		0 
#define SUPLEMENT_TYPE_MOREPOWER 	1

#define MAX_ITEM_TYPE 26

new ItemTypes[27][50] =
{
	"Nieokreœlony",
	"Broñ",
	"Amunicja",
	"Maska",
	"Telefon",
	"Jedzenie",
	"Zeszyt",
	"Karteczka",
	"Ubranie",
	"Napój",
	"Megafon",
	"Papierosy",
	"Zegarek",
	"Komponent pojazdu",
	"Akcesoria postaci",
	"Kajdanki",
	"Karnet na si³ownie",
	"Lek",
	"Zw³oki",
	"Rêkawiczki",
	"P³yta CD",
	"Narkotyk",
	"Sk³adnik narkotyków",
	"Boombox",
	"Badanie",
	"Adrenalina",
	"Torba"
};

// Narkotyki
#define DRUG_NONE 						0 	//Nic
#define DRUG_LSD 						1 	//LSD
#define DRUG_COCAINE 					2 	//Kokaina
#define DRUG_CRACK						3 	//Crack
#define DRUG_META 						4 	//Meta
#define DRUG_MARIHUANA					5 	//marihuana
#define DRUG_HEROIN						6   // heroina
#define DRUG_AMFA						7   // amfa
#define DRUG_PROTEIN					8	// protein

new DrugTypes[9][50] = 
{
	"null",
	"LSD",
	"Kokaina",
	"Crack",
	"Metamfetamina",
	"Marihuana",
	"Heroina",
	"Amfetamina",
	"Thyroxine"
};

// P³atnoœci
#define PAYMENT_TYPE_CASH 0
#define PAYMENT_TYPE_CARD 1

// Oferty
#define OFFER_TYPE_VCARD 1
#define OFFER_TYPE_DOCUMENT 2
#define OFFER_TYPE_LSN_REKLAMA 3
#define OFFER_TYPE_LSN_WYWIAD 4
#define OFFER_TYPE_PETROL 5
#define OFFER_TYPE_ITEM 6
#define OFFER_TYPE_ITEM_PODAJ 7
#define OFFER_TYPE_SALON_VEH 8
#define OFFER_TYPE_EMS_LECZENIE 9
#define OFFER_TYPE_YO 10
#define OFFER_TYPE_NAPRAWA 11
#define OFFER_TYPE_VEH_SELL 12
#define OFFER_TYPE_MALOWANIE 13

// Prace dorywcze
#define WORK_TYPE_SALESMAN 1
#define WORK_TYPE_PAPERMAN 2
#define WORK_TYPE_STORAGEMAN 3
#define WORK_TYPE_FISHMAN 4

// Dokumenty
#define DOCUMENT_ID 1
#define DOCUMENT_DRIVE 2
#define DOCUMENT_METRYCZKA 4

// Kary
#define PENALTY_TYPE_WARN 1
#define PENALTY_TYPE_KICK 2
#define PENALTY_TYPE_BAN 3
#define PENALTY_TYPE_BLOCK 4
#define PENALTY_TYPE_AJ 5
#define PENALTY_TYPE_GS 6

#define BLOCK_CHAR 1
#define BLOCK_VEHICLES 2
#define BLOCK_RUN 4
#define BLOCK_OOC 8
#define BLOCK_DESCRIPTION 16

new BlockBit[5] = {1,2,4,8,16};
new BlockName[5][40] = {
	"Blokada postaci",
	"Blokada pojazdów",
	"Blokada biegania",
	"Blokada czatu ooc",
	"Blokada opisów"
};

// Animacje mówienia
new TalkStyleName[6][20] = {
	"Normalny",
	"Gangstuch 1",
	"Gangstuch 2",
	"Gangstuch 3",
	"Gangstuch 4",
	"Gangstuch 5"
};

// Animacje chodzenia
new WalkStyleName[14][20] = {
	"Domyœlny",
	"Normalny 1",
	"Normalny 2",
	"Gangstuch 1",
	"Gangstuch 2",
	"Staruszek",
	"Gruby Staruszek",
	"Grubas",
	"Dama 1",
	"Dama 2",
	"Dziwka 1",
	"Dziwka 2",
	"Upity",
	"Œlepiec"
};

// Edytory obiektów
#define EDITOR_TYPE_SAMP		0
#define EDITOR_TYPE_CUSTOM		1

/*
// Kartoteka
#define FILES_OWNER_TYPE_PLAYER 0
#define FILES_OWNER_TYPE_VEHICLE 1
#define FILES_OWNER_TYPE_GROUP 2

#define FILES_TYPE_NOTE 1
#define FILES_TYPE_WARN 2
#define FILES_TYPE_KEEP 3
#define FILES_TYPE_FINE 4
#define FILES_TYPE_BLOCK 5

new FileType[6][] = {
	"",
	"Notatka",
	"Ostrze¿enie",
	"Areszt",
	"Mandat",
	"Blokada"
};*/

// -----------------------------------------------

#define DIALOG_INFO 1
#define DIALOG_LOGIN 2
#define DIALOG_LOGIN_NO_ACCOUNT 3
#define DIALOG_GIVE_CREW 4

#define DIALOG_DRZWI 6
#define DIALOG_DRZWI_NAME 7
#define DIALOG_DRZWI_SPAWN 8
#define DIALOG_DRZWI_SPAWN_COORDS 9
#define DIALOG_DRZWI_AUDIO 10
#define DIALOG_DRZWI_PAYMENT 11

#define DIALOG_ADRZWI_PICKUP 12
#define DIALOG_ADRZWI_CHANGE_INTERIOR 13
#define DIALOG_DRZWI_OBJECTS_LIMIT 14

#define DG_DRZWI_NAME 1
#define DG_DRZWI_SPAWN 2
#define DG_DRZWI_SPAWN_COORDS 3
#define DG_DRZWI_AUDIO 4
#define DG_DRZWI_SCHOWEK 5
#define DG_DRZWI_MAGAZYN 6
#define DG_DRZWI_MAP_LOAD 7
#define DG_DRZWI_PAYMENT 8
#define DG_DRZWI_CARS 9
#define DG_DRZWI_CLOSING 10
#define DG_DRZWI_METERS 11

#define DG_DRZWI_CHANGE_INTERIOR_PREV 1
#define DG_DRZWI_CHANGE_INTERIOR_NEXT 2
#define DG_DRZWI_CHANGE_INTERIOR_ROW 3

#define DIALOG_AGRUPA_TYP 15

#define DIALOG_CHAR_DESCRIPTION 16
#define DIALOG_CHAR_DESCRIPTION_ADD 17

#define DG_CHAR_DESC_OLD 1
#define DG_CHAR_DESC_ADD 2
#define DG_CHAR_DESC_DELETE 3

#define DIALOG_PLAYER_VEHICLES 18
#define DIALOG_GROUP_VEHICLES 19
#define DIALOG_PLAYER_VEHICLE_PANEL 20
#define DG_PLAYER_VEHICLE_PANEL_LIGHTS 1
#define DG_PLAYER_VEHICLE_PANEL_BOOT 2
#define DG_PLAYER_VEHICLE_PANEL_BONNET 3
#define DG_PLAYER_VEHICLE_PANEL_RADIO 4
#define DG_PLAYER_VEHICLE_PANEL_CDOUT 5

#define DIALOG_PLAYER_ITEMS 21
#define DG_ITEMS_ITEM_ROW 1
#define DG_ITEMS_SELECTING 2
#define DG_ITEMS_SEARCH	3
#define DG_ITEMS_FAVORITES 4
#define DG_ITEMS_VIEW_ALL 5

#define DIALOG_ITEM_MORE 22
#define DG_ITEMS_MORE_DROPG 1
#define DG_ITEMS_MORE_SELL 2
#define DG_ITEMS_MORE_PUT_IN_DOOR 3
#define DG_ITEMS_MORE_INFO 4
#define DG_ITEMS_MORE_DRUGS_DIVIDE 5
#define DG_ITEMS_MORE_DRUGS_JOIN 6
#define DG_ITEMS_MORE_FAVORITE 7
#define DG_ITEMS_MORE_PUT_IN_BAG 8
//multi-selecting
#define DG_MULTIITEMS_MORE_INFO 9
#define DG_MULTIITEMS_MORE_DROPG 10
#define DG_MULTIITEMS_MORE_PUT_IN_DOOR 11
#define DG_MULTIITEMS_MORE_DRUGS_JOIN 12
#define DG_MULTIITEMS_FAVORITES 13

#define DIALOG_ITEMS_PICKUP 23
#define DG_ITEMS_PICKUP_ROW 1

#define DIALOG_USE_AMMO 24

#define DIALOG_PHONE 25
#define DG_PHONE_CALL 1
#define DG_PHONE_SMS 2
#define DG_PHONE_CONTACTS 3
#define DG_PHONE_TURNOFF 4
#define DG_PHONE_VCARD 5

#define DIALOG_PHONE_SMS_NUMBER 26
#define DIALOG_PHONE_SMS_TEXT 27

#define DIALOG_PHONE_CONTACTS 28
#define DG_PHONE_CONTACTS_BASE 1
#define DG_PHONE_CONTACTS_ROW 2

#define DIALOG_PHONE_VCARD 29
#define DIALOG_PHONE_CONTACTS_ROW 30
#define DIALOG_PHONE_CONTACTS_ROW_NAME 31

#define DIALOG_PHONE_CALL_NUMBER 32

#define DIALOG_WORKS 33
#define DIALOG_DOCUMENTS 34
#define DIALOG_PAYMENT 35

#define DIALOG_ORDER_PRODUCTS 36
#define DIALOG_ORDER_PRODUCTS_CATEGORY 37
#define DG_ORDER_PRODUCTS_CATEGORY_ITEM 1

#define DIALOG_ORDER_PRODUCTS_ITEM_NAME 38
#define DIALOG_ORDER_PRODUCTS_ITEM_AMOUNT 39

#define DIALOG_24/7_BUY 40
#define DIALOG_NOTEPAD 41

#define DIALOG_STATS 42
#define DG_STATS_TALK_STYLE 1
#define DG_STATS_WALK_STYLE 2
#define DG_STATS_SPAWN 3
#define DG_STATS_FREEZE_DOOR 4
#define DG_STATS_SHOW_HINTS 5
#define DG_INTERFACE 6
#define DG_EDITOR 7
#define DG_ITEMS_FAVORITE 8

#define DIALOG_BANKOMAT_KONTO 44
#define DIALOG_BANKOMAT_KONTO_STAN 45
#define DIALOG_BANKOMAT_KONTO_WYPLAC 46

#define DG_BANKOMAT_KONTO_STAN 1
#define DG_BANKOMAT_KONTO_WYPLAC 3

#define DIALOG_BANK_ZALOZ_KONTO 47
#define DIALOG_BANK_KONTO 48
#define DIALOG_BANK_KONTO_STAN 49
#define DIALOG_BANK_KONTO_WPLAC 50
#define DIALOG_BANK_KONTO_WYPLAC 51
#define DIALOG_BANK_KONTO_PRZELEW 52

#define DG_BANK_KONTO_STAN 1
#define DG_BANK_KONTO_WPLAC 2
#define DG_BANK_KONTO_WYPLAC 3
#define DG_BANK_KONTO_PRZELEW 4
#define DG_BANK_HISTORY 5
#define DG_BANK_DEBIT 6

#define DIALOG_DOOR_STORAGE 53

#define DIALOG_CARS_SHOP_CATEGORY 54

#define DIALOG_POMOC 55

#define DIALOG_ITEMS_MORE_SELL 56
#define DIALOG_ITEMS_MORE_SELL_PRICE 57
#define DIALOG_ITEMS_MORE_INFO 73

#define DIALOG_CARS_SHOP_LIST 58

#define DIALOG_CARS_SHOP_FUELTYPE 59

#define DIALOG_CARS_SHOP_COLOR 60

#define DIALOG_STATS_TALK_STYLE 61
#define DIALOG_STATS_WALK_STYLE 62

#define DIALOG_POKAZ_DOKUMENT 63

#define DIALOG_911 64
#define DIALOG_911_REASON 65

#define DIALOG_AGRUPA_FLAGI 66
#define DIALOG_444_REASON 67

#define DIALOG_STATS_SPAWN 68

#define DIALOG_YO 69
#define DIALOG_SEARCH 70

#define DIALOG_ACCESSORY 71

#define DIALOG_SOCIAL_RENT 72

#define DIALOG_ANIMS_LIST 74

#define DIALOG_ACCEPT_DEATH 75

#define DIALOG_FILES 76
#define DIALOG_FILES_SEARCH 77
#define DIALOG_FILES_SEARCH_RESULT 78
#define DIALOG_FILES_PROFILE 79
#define DG_FILES_SEARCH_CHAR 0
#define DG_FILES_SEARCH_GROUP 1
#define DG_FILES_SEARCH_VEH 2

#define DG_FILES_PROFILE_LIST_ALL 1
#define DG_FILES_PROFILE_LIST_UNPAID 2

#define DIALOG_FILES_PROFILE_RECORDS 80
#define DIALOG_FILES_PROFILE_RECORD 81

#define DIALOG_WHISPER_TOKEN 82

#define DIALOG_CD_URL 83
#define DIALOG_CD_NAME 84

#define DIALOG_HANDCUFFS_SELECT 85

#define DIALOG_APOMOC 86
#define DIALOG_ITEMS_DRUGS_DIVIDE 87
#define DIALOG_ITEMS_DRUGS_JOIN 88

#define DIALOG_CRAFT 89

#define DIALOG_VEHICLE_BLOCK 90

#define DIALOG_MALOWANIE_CARCOLOR 91

#define DIALOG_GLOBAL_CHOOSE_CHAR 92

#define DIALOG_CHANGENAME 93

#define DIALOG_LOGIN_NO_ACCOUNT_SUGGESTED 94

#define DIALOG_TRANSACTIONS_HISTORY 95
#define DG_TRANSACTIONS_NEXT 0
#define DG_TRANSACTIONS_PREV 1

#define DIALOG_GOV 96
#define DG_GOV_DRIVER_LICENSE 1
#define DG_GOV_BUSINESS 2
#define DG_GOV_RENT 3
#define DG_GOV_ID 4
#define DG_GOV_JOB 5

#define DIALOG_OFFER 97

#define DIALOG_OFFER_HIGH 98

#define DIALOG_BOOMBOX 99

#define DIALOG_INSURANCE    100
#define DG_INSURANCE_STD    1
#define DG_INSURANCE_SMART  2
#define DG_INSURANCE_PRO    3
#define DG_INSURANCE_CANCEL 4

#define DIALOG_INSURANCE_SMART  101
#define DIALOG_INSURANCE_STD    102
#define DIALOG_INSURANCE_PRO    103
#define DIALOG_INSURANCE_CANCEL 104

#define DIALOG_EXAMINATION 105

#define DIALOG_ADRENALINE 106

#define DIALOG_BAG 107

#define DIALOG_PUT_IN_BAG 108

#define DIALOG_GROUP_MEMBERS 109

#define DIALOG_GROUP_MEMBERS_INPUT 110

#define DIALOG_EMERGENCY 111

//Typy ubezpieczeñ
#define INSURANCE_TYPE_NONE  0
#define INSURANCE_TYPE_STD   1
#define INSURANCE_TYPE_SMART 2
#define INSURANCE_TYPE_PRO   3

stock hrp_ShowPlayerDialog(playerid, dialog, style, title[], caption[], button1[], button2[])
{
	pInfo[playerid][player_dialog] = dialog;
	new title2[140];
	format(title2, sizeof(title2), ""HEX_COLOR_HONEST""PAGE_NAME" » "HEX_COLOR_WHITE"%s", title);
	return ShowPlayerDialog(playerid, dialog, style, title2, caption, button1, button2);
}
#define ShowPlayerDialog hrp_ShowPlayerDialog

stock GetPlayerPermissions(playerid, required)
{
	if(pInfo[playerid][player_level] >= required) return 0;
	else return 1;
}

new DrugIngr[19][50] = 
{
	"null",					// 0
	"Doniczka",				// 1
	"Woda",					// 2
	"Dziwne nasiona",		// 3
	"Winian egotaminy",		// 4
	"Kwas lizergowy",		// 5
	"Liœcie krasnodrzewu",	// 6
	"Soda oczyszczona",		// 7
	"Ocet",					// 8
	"Efedryna",				// 9
	"Jod",					// 10
	"Morfina",				// 11
	"Siarczan",				// 12
	"Amoniak",				// 13
	"Aceton",				// 14
	"Fenyloaceton ",		// 15
	"Hydroksamina",			// 16
	"Wodór",				// 17
	"Nikiel"				// 18
};

new DrugSchema[9][10] = 
{
	{0}, 				// nic
	{2, 4, 5}, 			// lsd
	{6}, 				// kokaina
	{7, 8}, 			// crack
	{9, 10}, 			// meta
	{1, 2, 3}, 			// marihuana
	{11, 12, 13, 14}, 	// heroina
	{15, 16, 17, 18},   // amfetamina
	{2} 				// thyroxine
};

new DrugSchemaAmount[9][10] = 
{
	{0}, 			// nic
	{4, 1, 1},  	// lsd
	{1}, 			// kokaina
	{2, 2}, 		// crack
	{2, 3}, 		// meta
	{1, 4, 1},  	// marihuana
	{1, 2, 2, 3}, 	// heroina
	{1, 1, 5, 3}, 	// amfetamina
	{50} 			// thryoxine
};




new Float:CuffObjectOffsets[][] =
{
{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // 0
{0.021000, 0.101998, -0.009999, -6.099919, -7.100001, 83.099945, 1.489999, 3.036000, 1.957998}, // 1
{0.021000, 0.049998, -0.009999, -6.099919, -7.100001, 83.099945, 0.989999, 3.036000, 1.957998}, // 2
{0.037000, 0.034998, -0.017999, -6.099919, -7.100001, 83.099945, 1.025999, 3.036000, 1.714998}, // 3
{0.045001, 0.115998, 0.008000, 3.100080, -6.900001, 71.299934, 1.556999, 2.976998, 1.732998}, // 4
{0.031001, 0.046998, 0.007000, 3.100080, -6.900001, 82.899940, 1.230999, 2.976998, 1.693999}, // 5
{0.031001, 0.039998, -0.003999, -6.199919, -6.900001, 82.899940, 0.986999, 2.976998, 1.383998}, // 6
{0.061001, 0.037999, -0.017999, -12.599921, 2.999991, 77.899871, 1.132000, 2.822998, 1.527998}, // 7
{0.045001, 0.037999, -0.017999, -12.599921, 2.999991, 77.899871, 1.132000, 2.822998, 1.527998}, // 8
{0.066001, 0.041999, -0.017999, -12.599921, 2.999991, 77.899871, 1.132000, 2.822998, 1.527998}, // 9
{0.066001, 0.068998, -0.024999, -12.599921, 2.999991, 77.899871, 1.296999, 2.822998, 1.777998}, // 10
{0.030001, 0.041998, -0.024999, -12.599921, 2.999991, 77.899871, 1.296999, 2.822998, 1.777998}, // 11
{0.030001, 0.041998, -0.024999, -12.599921, 2.999991, 77.899871, 1.296999, 2.822998, 1.777998}, // 12
{0.030001, 0.041998, -0.024999, -12.599921, 2.999991, 77.899871, 1.296999, 2.822998, 1.777998}, // 13
{0.048000, 0.091998, -0.016999, -12.599921, 2.999991, 77.899871, 1.624000, 2.822998, 2.102999}, // 14
{0.034001, 0.059998, -0.033999, -12.599921, 2.999991, 77.899871, 1.624000, 2.822998, 2.102999}, // 15
{0.056000, 0.085998, -0.008999, -12.599921, 2.999991, 77.899871, 1.624000, 2.822998, 2.102999}, // 16
{0.061000, 0.046998, -0.019999, -12.599921, 2.999991, 77.899871, 1.381999, 2.785998, 1.922999}, // 17
{0.041000, 0.044998, -0.002999, -12.599921, 2.999991, 77.899871, 1.236999, 2.785998, 1.618000}, // 18
{0.041000, 0.071998, -0.007999, -12.599921, 2.999991, 77.899871, 1.479999, 2.785998, 1.993999}, // 19
{0.048000, 0.048998, -0.007999, 1.100076, 2.999991, 77.899871, 1.253999, 2.785998, 1.709000}, // 20
{0.041000, 0.053998, -0.010999, 2.600077, -12.600008, 77.899871, 1.163999, 2.785998, 1.643000}, // 21
{0.041000, 0.053998, -0.010999, 2.600077, -12.600008, 77.899871, 1.163999, 2.785998, 1.643000}, // 22
{0.029000, 0.124998, 0.008000, 1.300076, -9.400010, 79.599876, 2.001999, 2.785998, 2.481001}, // 23
{0.029000, 0.048998, -0.023999, 1.300076, -9.400010, 79.599876, 1.312999, 2.785998, 1.844001}, // 24
{0.046000, 0.042998, -0.027999, 1.300076, -9.400010, 79.599876, 1.312999, 2.785998, 1.844001}, // 25
{0.060000, 0.075998, -0.021999, 1.300076, -9.400010, 79.599876, 1.478999, 2.785998, 1.980001}, // 26
{0.033000, 0.049998, -0.013999, 1.300076, -9.400010, 79.599876, 1.138999, 2.785998, 1.805001}, // 27
{0.033000, 0.056998, -0.008999, 1.300076, -9.400010, 79.599876, 1.138999, 2.785998, 1.805001}, // 28
{0.033000, 0.044998, -0.032999, 1.300076, -9.400010, 79.599876, 1.138999, 2.785998, 1.805001}, // 29
{0.033000, 0.044998, -0.032999, 1.300076, -9.400010, 79.599876, 1.138999, 2.785998, 1.805001}, // 30
{0.032000, 0.084998, -0.012999, -10.499922, -9.400010, 79.599876, 1.436999, 2.785998, 2.023001}, // 31
{-0.001999, 0.053998, -0.012999, -10.499922, -9.400010, 79.599876, 1.436999, 2.785998, 2.023001}, // 32
{0.053000, 0.053999, -0.005999, -11.899930, -9.299997, 84.299919, 1.129999, 3.036000, 1.453999}, // 33
{0.032000, 0.072999, -0.005999, -11.899930, -9.299997, 71.599922, 1.516999, 3.036000, 1.889999}, // 34
{-0.003999, 0.041999, -0.031000, -11.899930, -9.299997, 79.799903, 1.724999, 3.036000, 2.250999}, // 35
{0.007001, 0.050999, -0.031000, -11.899930, -9.299997, 79.799903, 1.528999, 3.036000, 2.096999}, // 36
{0.007001, 0.041999, -0.031000, -11.899930, -9.299997, 79.799903, 1.528999, 3.036000, 2.096999}, // 37
{0.038001, 0.061999, -0.031000, -6.599930, 8.400003, 79.799903, 1.461000, 3.036000, 1.789999}, // 38
{0.043001, 0.068999, -0.035000, -6.599930, -11.699990, 79.799903, 1.668999, 3.036000, 2.037998}, // 39
{0.043001, 0.042999, -0.006000, -6.599930, -11.699990, 79.799903, 1.275000, 3.036000, 1.454998}, // 40
{0.043001, 0.042999, -0.006000, -6.599930, -11.699990, 79.799903, 1.275000, 3.036000, 1.454998}, // 41
{0.031001, 0.042999, -0.025000, -6.599930, -18.699989, 79.799903, 1.487000, 3.036000, 1.865998}, // 42
{0.031001, 0.042999, -0.014000, -20.399934, -23.199989, 79.799903, 1.273000, 3.036000, 1.522998}, // 43
{0.031001, 0.065999, -0.014000, -20.399934, -23.199989, 79.799903, 1.273000, 3.036000, 1.522998}, // 44
{0.031001, 0.065999, -0.014000, -20.399934, -23.199989, 79.799903, 1.273000, 3.036000, 1.522998}, // 45
{0.007001, 0.043999, -0.014000, -20.399934, -23.199989, 79.799903, 1.273000, 3.036000, 1.522998}, // 46
{0.018001, 0.056999, -0.014000, -17.399932, -29.199985, 71.799911, 1.273000, 3.036000, 1.522998}, // 47
{0.018001, 0.044999, -0.014000, -17.399932, -29.199985, 71.799911, 1.273000, 3.036000, 1.522998}, // 48
{-0.012998, 0.027999, -0.038000, -25.099925, -37.999988, 72.599906, 1.384000, 3.036000, 1.650998}, // 49
{0.064001, 0.045999, -0.037000, -15.799923, -15.199990, 72.399909, 1.160000, 3.036000, 1.238998}, // 50
{0.064001, 0.070999, -0.012000, -15.799923, -15.199990, 72.399909, 1.348000, 3.036000, 1.684998}, // 51
{0.064001, 0.070999, -0.012000, -15.799923, -15.199990, 72.399909, 1.348000, 3.036000, 1.684998}, // 52
{0.024001, 0.070999, -0.018000, -8.599923, -7.199986, 77.999916, 1.348000, 3.036000, 1.684998}, // 53
{0.052001, 0.070999, -0.053999, -14.899925, -2.299986, 77.999916, 1.348000, 3.036000, 1.684998}, // 54
{0.014001, 0.036999, -0.009999, -14.899925, -16.999986, 77.999916, 1.178000, 3.036000, 1.408998}, // 55
{0.028001, 0.058999, -0.023999, -14.899925, -16.999986, 77.999916, 1.284000, 3.036000, 1.815998}, // 56
{0.034001, 0.019998, -0.016999, -14.899925, -16.999986, 77.999916, 0.983000, 3.036000, 1.392997}, // 57
{0.034001, 0.071998, -0.016999, -14.899925, -16.999986, 77.999916, 1.146001, 3.036000, 1.701997}, // 58
{0.034001, 0.044998, -0.016999, -14.899925, -16.999986, 77.999916, 1.146001, 3.036000, 1.701997}, // 59
{0.034001, 0.044998, -0.016999, -14.899925, -16.999986, 77.999916, 1.146001, 3.036000, 1.701997}, // 60
{0.037001, 0.044998, -0.038999, -14.899925, -16.999986, 77.999916, 1.146001, 3.036000, 1.701997}, // 61
{0.071001, 0.093998, 0.007000, -14.899925, -16.999986, 77.999916, 1.518001, 3.036000, 2.299997}, // 62
{0.036000, 0.079998, -0.015999, -14.899925, 18.100011, 77.999916, 1.437001, 3.036000, 1.674997}, // 63
{0.036000, 0.058998, -0.015999, -14.899925, 18.100011, 77.999916, 1.183001, 3.036000, 1.428997}, // 64
{0.036000, 0.055998, -0.015999, -14.899925, 18.100011, 77.999916, 1.183001, 3.036000, 1.428997}, // 65
{0.058999, 0.024998, -0.024999, -14.899925, -13.399993, 77.999916, 1.064001, 3.036000, 1.246997}, // 66
{0.058999, 0.024998, -0.024999, -14.899925, -13.399993, 77.999916, 1.064001, 3.036000, 1.246997}, // 67
{0.058999, 0.040998, -0.024999, -14.899925, -13.399993, 77.999916, 1.064001, 3.036000, 1.246997}, // 68
{0.026999, 0.050998, -0.015999, -19.699926, -13.399993, 76.299911, 1.064001, 3.036000, 1.246997}, // 69
{0.059999, 0.044998, -0.033999, -19.699926, -13.399993, 76.299911, 1.064001, 3.036000, 1.246997}, // 70
{0.059999, 0.044998, -0.033999, -19.699926, -13.399993, 76.299911, 1.064001, 3.036000, 1.246997}, // 71
{0.041999, 0.071998, -0.011999, -10.899921, -13.399993, 76.299911, 1.279001, 3.036000, 1.471997}, // 72
{0.041999, 0.071998, -0.011999, -10.899921, -13.399993, 76.299911, 1.279001, 3.036000, 1.471997}, // 73
{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // 74
{0.025999, 0.052998, -0.009000, -10.899921, -13.399993, 76.299911, 1.279001, 3.036000, 1.471997}, // 75
{0.048001, 0.046999, -0.016999, -18.699926, 0.000000, 85.599922, 1.038998, 3.036000, 1.323998}, // 76
{0.021999, 0.046998, -0.037000, -10.899921, -19.700000, 76.299911, 1.279001, 3.036000, 1.471997}, // 77
{0.021999, 0.046998, -0.024000, -14.399919, -32.399986, 75.899925, 1.592002, 3.036000, 2.203997}, // 78
{0.045999, 0.046998, -0.024000, -14.399919, -15.699983, 75.899925, 1.253002, 3.036000, 1.767997}, // 79
{-0.007000, 0.046998, -0.024000, -14.399919, -15.699983, 75.899925, 1.253002, 3.036000, 1.767997}, // 80
{-0.007000, 0.046998, -0.024000, -14.399919, -15.699983, 75.899925, 1.253002, 3.036000, 1.767997}, // 81
{0.042999, 0.044998, -0.009000, -14.399919, -15.699983, 75.899925, 1.253002, 3.036000, 1.767997}, // 82
{0.042999, 0.044998, -0.009000, -14.399919, -15.699983, 75.899925, 1.253002, 3.036000, 1.767997}, // 83
{0.042999, 0.044998, -0.009000, -14.399919, -15.699983, 75.899925, 1.253002, 3.036000, 1.767997}, // 84
{0.055999, 0.028998, -0.036000, -14.399919, -15.699983, 75.899925, 1.572001, 3.036000, 2.302998}, // 85
{0.055999, 0.028998, -0.019000, -14.399919, -15.699983, 75.899925, 1.356001, 3.036000, 1.726998}, // 86
{0.003000, 0.037998, -0.029999, -18.699926, -8.600000, 75.199928, 1.010998, 3.036000, 1.472998}, // 87
{0.014999, 0.058998, -0.044000, -14.399919, -23.499982, 75.899925, 1.356001, 3.036000, 1.726998}, // 88
{0.041999, 0.068998, -0.031000, -14.399919, -23.499982, 75.899925, 1.356001, 3.036000, 1.726998}, // 89
{0.020999, 0.057998, -0.019000, -14.399919, -23.499982, 75.899925, 1.356001, 3.036000, 1.726998}, // 90
{0.031001, 0.051998, -0.008999, -7.899918, -8.899999, 86.299896, 1.096998, 3.036000, 1.562999}, // 91
{0.020999, 0.057998, -0.019000, -14.399919, -23.499982, 75.899925, 1.356001, 3.036000, 1.726998}, // 92
{0.020999, 0.057998, -0.019000, -14.399919, -23.499982, 75.899925, 1.356001, 3.036000, 1.726998}, // 93
{0.020999, 0.057998, -0.019000, -14.399919, -23.499982, 75.899925, 1.356001, 3.036000, 1.726998}, // 94
{0.020999, 0.066998, -0.019000, -14.399919, -23.499982, 75.899925, 1.356001, 3.036000, 1.726998}, // 95
{0.020999, 0.073998, -0.019000, -14.399919, -23.499982, 75.899925, 1.726001, 3.036000, 2.126998}, // 96
{0.049999, 0.054998, -0.019000, -14.399919, -23.499982, 75.899925, 1.726001, 3.036000, 2.126998}, // 97
{0.049999, 0.047998, -0.019000, -14.399919, -23.499982, 75.899925, 1.397001, 3.036000, 1.587998}, // 98
{0.049999, 0.075998, -0.019000, -14.399919, -23.499982, 75.899925, 1.397001, 3.036000, 1.587998}, // 99
{0.021999, 0.057998, -0.019000, -14.399919, -23.499982, 75.899925, 1.397001, 3.036000, 1.587998}, // 100
{0.054999, 0.057998, -0.047000, -14.399919, -23.499982, 75.899925, 1.397001, 3.036000, 1.587998}, // 101
{0.031999, 0.057998, -0.014000, -14.399919, -23.499982, 75.899925, 1.397001, 3.036000, 1.587998}, // 102
{0.079999, 0.147998, -0.014000, -14.399919, -23.499982, 75.899925, 2.485005, 3.036000, 2.238998}, // 103
{0.045999, 0.043998, -0.006000, -14.399919, -23.499982, 75.899925, 1.124004, 3.036000, 1.722997}, // 104
{0.081999, 0.163998, -0.026000, -9.099919, -6.399981, 75.899925, 2.497004, 3.036000, 3.194998}, // 105
{0.081999, 0.059998, -0.004000, -9.099919, -6.399981, 75.899925, 1.028003, 3.036000, 1.371998}, // 106
{0.057999, 0.040998, -0.004000, -9.099919, -6.399981, 75.899925, 1.028003, 3.036000, 1.371998}, // 107
{0.057999, 0.090998, -0.004000, -9.099919, -6.399981, 75.899925, 1.350003, 3.036000, 1.619998}, // 108
{0.043999, 0.046998, -0.004000, -9.099919, -6.399981, 75.899925, 1.004003, 3.036000, 1.254998}, // 109
{0.043999, 0.046998, -0.004000, -9.099919, -6.399981, 75.899925, 1.004003, 3.036000, 1.254998}, // 110
{0.056999, 0.046998, -0.038000, -9.099919, -6.399981, 75.899925, 1.004003, 3.036000, 1.254998}, // 111
{0.056999, 0.065998, -0.038000, -9.099919, -6.399981, 75.899925, 1.004003, 3.036000, 1.254998}, // 112
{0.067999, 0.091998, -0.008000, -9.099919, -6.399981, 75.899925, 1.249004, 3.036000, 1.553998}, // 113
{0.033999, 0.057998, -0.008000, -9.099919, -6.399981, 75.899925, 1.249004, 3.036000, 1.553998}, // 114
{0.064999, 0.057998, -0.009000, -9.099919, -6.399981, 75.899925, 1.249004, 3.036000, 1.686998}, // 115
{0.028999, 0.051998, -0.009000, -9.099919, -6.399981, 75.899925, 1.249004, 3.036000, 1.686998}, // 116
{0.052999, 0.025998, 0.004999, -9.099919, -6.399981, 75.899925, 0.841004, 3.036000, 1.327998}, // 117
{0.052999, 0.025998, 0.004999, -9.099919, -6.399981, 75.899925, 0.841004, 3.036000, 1.327998}, // 118
{0.056999, 0.025998, -0.017000, -9.099919, -6.399981, 75.899925, 1.001004, 3.036000, 1.789998}, // 119
{0.071999, 0.090998, -0.017000, -9.099919, -6.399981, 75.899925, 1.623004, 3.036000, 1.789998}, // 120
{0.028999, 0.084998, -0.017000, -9.099919, -6.399981, 75.899925, 1.452004, 3.036000, 1.789998}, // 121
{0.028999, 0.084998, -0.017000, -9.099919, -6.399981, 75.899925, 1.452004, 3.036000, 1.789998}, // 122
{0.081999, 0.054998, -0.017000, -9.099919, -6.399981, 75.899925, 1.452004, 3.036000, 1.789998}, // 123
{0.018999, 0.054998, -0.017000, -9.099919, -6.399981, 75.899925, 1.452004, 3.036000, 1.789998}, // 124
{0.039999, 0.041998, -0.019000, -9.099919, -6.399981, 75.899925, 1.173004, 3.036000, 1.568998}, // 125
{0.039999, 0.041998, -0.019000, -9.099919, -6.399981, 75.899925, 1.173004, 3.036000, 1.568998}, // 126
{0.056999, 0.114998, -0.010000, -9.099919, -8.199983, 75.899925, 1.358004, 3.036000, 2.048998}, // 127
{0.056999, 0.060998, -0.010000, -9.099919, -8.199983, 75.899925, 1.358004, 3.036000, 2.048998}, // 128
{0.038999, 0.050999, -0.010000, -9.099919, -8.199983, 75.899925, 0.822004, 3.036000, 1.233998}, // 129
{0.038999, 0.071998, -0.038000, -9.099919, -8.199983, 75.899925, 1.235004, 3.036000, 1.482998}, // 130
{0.038999, 0.060999, -0.019000, -9.099919, -8.199983, 75.899925, 1.235004, 3.036000, 1.482998}, // 131
{0.038999, 0.049999, -0.003000, -9.099919, -8.199983, 75.899925, 1.059004, 3.036000, 1.114998}, // 132
{0.041999, 0.079999, -0.017000, -9.099919, -8.199983, 75.899925, 1.285004, 3.036000, 1.503998}, // 133
{0.041999, 0.040999, -0.017000, -9.099919, -8.199983, 75.899925, 0.880004, 3.036000, 1.045998}, // 134
{0.041999, 0.040999, -0.027000, -9.099919, -8.199983, 75.899925, 0.880004, 3.036000, 1.045998}, // 135
{0.059999, 0.040999, -0.003000, -9.099919, -8.199983, 75.899925, 1.104004, 3.036000, 1.376998}, // 136
{0.001999, 0.040999, -0.003000, -17.499917, -26.699991, 75.899925, 1.104004, 3.036000, 1.376998}, // 137
{0.001999, 0.074998, -0.012000, -14.899912, -26.699991, 75.899925, 1.375004, 3.036000, 1.726998}, // 138
{0.001999, 0.061998, -0.012000, -14.899912, -26.699991, 75.899925, 1.375004, 3.036000, 1.726998}, // 139
{0.019999, 0.070998, -0.012000, -14.899912, -26.699991, 75.899925, 1.375004, 3.036000, 1.726998}, // 140
{0.041999, 0.052998, -0.012000, -14.899912, -26.699991, 75.899925, 1.206004, 3.036000, 1.726998}, // 141
{0.041999, 0.052998, -0.012000, -14.899912, -26.699991, 75.899925, 1.206004, 3.036000, 1.726998}, // 142
{0.041999, 0.052998, -0.012000, -14.899912, -26.699991, 75.899925, 1.206004, 3.036000, 1.726998}, // 143
{0.041999, 0.052998, -0.012000, -14.899912, -26.699991, 75.899925, 1.206004, 3.036000, 1.726998}, // 144
{0.022999, 0.069998, -0.012000, -14.899912, -26.699991, 75.899925, 1.206004, 3.036000, 1.726998}, // 145
{0.022999, 0.075998, -0.012000, -14.899912, -26.699991, 75.899925, 1.365004, 3.036000, 1.862998}, // 146
{0.050999, 0.050998, -0.032000, -14.899912, -11.900002, 75.899925, 1.118004, 3.036999, 1.449998}, // 147
{0.050999, 0.042998, -0.032000, -14.899912, -11.900002, 75.899925, 1.118004, 3.036999, 1.449998}, // 148
{0.050999, 0.062998, -0.029000, -14.899912, -11.900002, 75.899925, 1.118004, 3.036999, 1.449998}, // 149
{0.061999, 0.035998, -0.000000, -14.899912, -11.900002, 75.899925, 0.932004, 3.036999, 1.031998}, // 150
{0.061999, 0.071998, -0.030000, -14.899912, -11.900002, 75.899925, 1.307004, 3.036999, 1.250998}, // 151
{0.061999, 0.040999, -0.030000, -14.899912, -11.900002, 75.899925, 0.972004, 3.036999, 0.992997}, // 152
{0.061999, 0.040999, -0.030000, -14.899912, -11.900002, 75.899925, 0.972004, 3.036999, 0.992997}, // 153
{0.061999, 0.040999, -0.030000, -14.899912, -11.900002, 75.899925, 0.972004, 3.036999, 0.992997}, // 154
{0.061999, 0.032999, -0.014000, -14.899912, -11.900002, 75.899925, 0.972004, 3.036999, 0.992997}, // 155
{0.027999, 0.093999, -0.014000, -14.899912, -11.900002, 75.899925, 1.390004, 3.036999, 1.494998}, // 156
{0.027999, 0.051999, -0.014000, -14.899912, -11.900002, 75.899925, 0.927004, 3.036999, 1.138997}, // 157
{0.045999, 0.074999, -0.014000, -14.899912, -11.900002, 75.899925, 1.412004, 3.036999, 2.106997}, // 158
{0.018999, 0.055999, -0.014000, -14.899912, -11.900002, 75.899925, 1.095004, 3.036999, 1.621996}, // 159
{0.030999, 0.043999, -0.014000, -14.899912, -11.900002, 75.899925, 0.898004, 3.036999, 1.350997}, // 160
{0.030999, 0.043999, -0.014000, -14.899912, -11.900002, 75.899925, 1.039004, 3.036999, 1.494997}, // 161
{0.030999, 0.043999, -0.014000, -14.899912, -11.900002, 75.899925, 1.039004, 3.036999, 1.494997}, // 162
{0.050999, 0.043999, -0.034000, -14.899912, -11.900002, 75.899925, 1.039004, 3.036999, 1.494997}, // 163
{0.050999, 0.043999, -0.034000, -14.899912, -11.900002, 75.899925, 1.039004, 3.036999, 1.494997}, // 164
{0.029999, 0.028999, -0.034000, -14.899912, -11.900002, 75.899925, 1.039004, 3.036999, 1.494997}, // 165
{0.029999, 0.028999, -0.034000, -14.899912, -11.900002, 75.899925, 1.039004, 3.036999, 1.494997}, // 166
{0.029999, 0.042999, -0.018000, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 167
{0.029999, 0.035999, -0.018000, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 168
{0.029999, 0.035999, -0.018000, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 169
{0.029999, 0.035999, -0.018000, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 170
{0.029999, 0.035999, -0.018000, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 171
{0.029999, 0.035999, -0.018000, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 172
{0.029999, 0.035999, -0.018000, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 173
{0.029999, 0.035999, -0.018000, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 174
{0.029999, 0.035999, -0.018000, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 175
{0.039999, 0.051999, 0.000999, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 176
{0.039999, 0.051999, 0.000999, -14.899912, -11.900002, 75.899925, 1.179004, 3.036999, 1.674997}, // 177
{0.028999, 0.076999, 0.000999, -14.899912, -11.900002, 75.899925, 1.296003, 3.036999, 1.674997}, // 178
{0.028999, 0.049999, 0.000999, -14.899912, -11.900002, 75.899925, 1.296003, 3.036999, 1.674997}, // 179
{0.028999, 0.049999, 0.000999, -14.899912, -11.900002, 75.899925, 1.296003, 3.036999, 1.674997}, // 180
{0.047999, 0.091999, 0.000999, -14.899912, -11.900002, 75.899925, 1.541003, 3.036999, 2.083997}, // 181
{0.047999, 0.049999, -0.024000, -14.899912, -11.900002, 75.899925, 0.903003, 3.036999, 1.464997}, // 182
{0.047999, 0.049999, -0.024000, -14.899912, -11.900002, 75.899925, 0.903003, 3.036999, 1.464997}, // 183
{0.072999, 0.092999, 0.005999, -14.899912, -11.900002, 75.899925, 1.687003, 3.036999, 2.431997}, // 184
{0.072999, 0.045999, -0.012000, -14.899912, -11.900002, 75.899925, 0.800003, 3.036999, 1.623997}, // 185
{0.072999, 0.045999, -0.012000, -14.899912, -11.900002, 75.899925, 0.800003, 3.036999, 1.623997}, // 186
{0.072999, 0.045999, -0.012000, -14.899912, -11.900002, 75.899925, 0.947003, 3.036999, 1.623997}, // 187
{0.072999, 0.089999, -0.000999, -5.599913, -11.900002, 75.899925, 1.380003, 3.036999, 1.623997}, // 188
{0.050999, 0.045999, -0.010999, -5.599913, -11.900002, 75.899925, 0.871003, 3.036999, 1.430997}, // 189
{0.050999, 0.065998, -0.010999, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 190
{0.029999, 0.062998, -0.002999, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 191
{0.029999, 0.062998, -0.002999, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 192
{0.029999, 0.062998, -0.002999, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 193
{0.029999, 0.062998, -0.002999, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 194
{0.029999, 0.062998, -0.002999, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 195
{0.007999, 0.062998, -0.016999, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 196
{0.017999, 0.062998, -0.034999, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 197
{0.017999, 0.062998, 0.003000, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 198
{0.028999, 0.055998, -0.014999, -5.599913, -11.900002, 75.899925, 1.079003, 3.036999, 1.724997}, // 199
{0.028999, 0.069998, -0.010999, -5.599913, -11.900002, 75.899925, 1.294003, 3.036999, 1.724997}, // 200
{0.028999, 0.051998, -0.010999, -5.599913, -11.900002, 75.899925, 1.181003, 3.036999, 1.724997}, // 201
{0.044999, 0.083998, 0.000000, -5.599913, -11.900002, 75.899925, 1.265003, 3.036999, 1.724997}, // 202
{0.093999, 0.049999, 0.000000, -5.599913, -11.900002, 75.899925, 0.981003, 3.036999, 1.439996}, // 203
{0.093999, 0.049999, 0.000000, -5.599913, -11.900002, 75.899925, 0.981003, 3.036999, 1.439996}, // 204
{0.036000, 0.083998, -0.011999, -8.699912, -11.900002, 80.799919, 1.349002, 3.036999, 1.759997}, // 205
{0.036000, 0.083998, -0.011999, -8.699912, -11.900002, 80.799919, 1.349002, 3.036999, 1.759997}, // 206
{0.036000, 0.047998, -0.011999, -8.699912, -11.900002, 80.799919, 0.933002, 3.036999, 1.325996}, // 207
{0.036000, 0.034000, -0.011999, -8.699912, -11.900002, 80.799919, 0.668002, 3.036999, 1.020996}, // 208
{0.036000, 0.050000, 0.001000, -8.699912, -11.900002, 80.799919, 0.881002, 3.036999, 1.081997}, // 209
{0.014000, 0.037000, -0.018999, -8.699912, -25.800001, 75.799903, 0.881002, 3.036999, 1.081997}, // 210
{0.014000, 0.043000, -0.018999, -8.699912, -25.800001, 75.799903, 0.982002, 3.036999, 1.250997}, // 211
{0.067000, 0.043000, -0.014000, -8.699912, -25.800001, 75.799903, 0.982002, 3.036999, 1.250997}, // 212
{0.067000, 0.043000, -0.044000, -8.699912, -25.800001, 75.799903, 0.982002, 3.036999, 1.250997}, // 213
{0.042000, 0.043000, -0.015000, -8.699912, -25.800001, 75.799903, 0.982002, 3.036999, 1.250997}, // 214
{0.042000, 0.043000, -0.015000, -8.699912, -25.800001, 75.799903, 0.982002, 3.036999, 1.250997}, // 215
{0.042000, 0.043000, -0.015000, -8.699912, -25.800001, 75.799903, 0.982002, 3.036999, 1.250997}, // 216
{0.042000, 0.086000, -0.010000, -8.699912, -25.800001, 75.799903, 1.403002, 3.036999, 1.802997}, // 217
{0.041000, 0.077000, -0.043000, -8.699912, -25.800001, 75.799903, 1.403002, 3.036999, 1.802997}, // 218
{0.041000, 0.048000, -0.017000, -8.699912, -25.800001, 75.799903, 1.031002, 3.036999, 1.274997}, // 219
{0.041000, 0.048000, -0.017000, -8.699912, -25.800001, 75.799903, 1.031002, 3.036999, 1.452997}, // 220
{0.041000, 0.084000, -0.017000, -8.699912, -25.800001, 75.799903, 1.342002, 3.036999, 1.627997}, // 221
{0.041000, 0.084000, -0.017000, -8.699912, -25.800001, 75.799903, 1.342002, 3.036999, 1.627997}, // 222
{0.041000, 0.040000, -0.017000, -8.699912, -25.800001, 75.799903, 0.833002, 3.036999, 1.150997}, // 223
{0.041000, 0.040000, -0.017000, -8.699912, -25.800001, 75.799903, 0.833002, 3.036999, 1.150997}, // 224
{0.041000, 0.040000, -0.017000, -8.699912, -25.800001, 75.799903, 0.833002, 3.036999, 1.150997}, // 225
{0.041000, 0.040000, -0.017000, -8.699912, -25.800001, 75.799903, 0.833002, 3.036999, 1.150997}, // 226
{0.041000, 0.040000, -0.017000, -8.699912, -25.800001, 75.799903, 0.833002, 3.036999, 1.150997}, // 227
{0.041000, 0.040000, -0.017000, -8.699912, -25.800001, 75.799903, 0.833002, 3.036999, 1.150997}, // 228
{0.041000, 0.056000, -0.017000, -8.699912, -25.800001, 75.799903, 1.032002, 3.036999, 1.247997}, // 229
{0.041000, 0.039000, -0.017000, -8.699912, -25.800001, 75.799903, 1.032002, 3.036999, 1.247997}, // 230
{0.041000, 0.067000, -0.028000, -8.699912, -25.800001, 75.799903, 1.137002, 3.036999, 1.633997}, // 231
{0.041000, 0.079000, -0.028000, -8.699912, -25.800001, 75.799903, 1.137002, 3.036999, 2.040997}, // 232
{0.041000, 0.048000, -0.006000, -8.699912, -25.800001, 75.799903, 1.137002, 3.036999, 2.040997}, // 233
{0.041000, 0.060000, -0.006000, -8.699912, -25.800001, 75.799903, 1.137002, 3.036999, 2.040997}, // 234
{0.041000, 0.060000, -0.006000, -8.699912, -25.800001, 75.799903, 1.137002, 3.036999, 2.040997}, // 235
{0.041000, 0.060000, -0.006000, -8.699912, -25.800001, 75.799903, 1.137002, 3.036999, 2.040997}, // 236
{0.006000, 0.053000, -0.018000, -8.699912, -25.800001, 83.799911, 1.137002, 3.036999, 2.040997}, // 237
{0.027000, 0.061000, -0.018000, -8.699912, -25.800001, 83.799911, 1.137002, 3.036999, 2.040997}, // 238
{0.053000, 0.048000, -0.018000, -8.699912, -25.800001, 83.799911, 1.137002, 3.036999, 2.040997}, // 239
{0.007000, 0.048000, -0.018000, -8.699912, -25.800001, 83.799911, 1.137002, 3.036999, 2.040997}, // 240
{0.023000, 0.051000, -0.018000, -8.699912, -25.800001, 83.799911, 1.266002, 3.036999, 2.292997}, // 241
{0.023000, 0.051000, -0.018000, -8.699912, -25.800001, 83.799911, 1.266002, 3.036999, 2.292997}, // 242
{-0.007999, 0.051000, -0.018000, -8.699912, -25.800001, 83.799911, 1.266002, 3.036999, 2.292997}, // 243
{-0.007999, 0.031000, -0.018000, -8.699912, -25.800001, 83.799911, 1.266002, 3.036999, 2.292997}, // 244
{0.022000, 0.086000, -0.018000, -8.699912, -25.800001, 83.799911, 1.648002, 3.036999, 3.000997}, // 245
{-0.014000, 0.032000, -0.018000, -8.699912, -25.800001, 83.799911, 1.648002, 3.036999, 2.544996}, // 246
{0.019000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 1.648002, 3.036999, 2.544996}, // 247
{0.035000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 1.648002, 3.036999, 2.544996}, // 248
{0.035000, 0.047000, -0.040000, -8.699912, -25.800001, 83.799911, 1.648002, 3.036999, 2.544996}, // 249
{0.035000, 0.102000, 0.009999, -8.699912, -25.800001, 83.799911, 1.648002, 3.036999, 2.544996}, // 250
{0.016000, 0.069000, -0.026000, -8.699912, -25.800001, 83.799911, 1.373001, 3.036999, 2.438996}, // 251
{0.081000, 0.090000, -0.026000, -8.699912, -25.800001, 83.799911, 1.373001, 3.036999, 2.438996}, // 252
{0.023000, 0.060000, -0.026000, -8.699912, -25.800001, 83.799911, 1.373001, 3.036999, 2.438996}, // 253
{0.023000, 0.060000, -0.026000, -8.699912, -25.800001, 83.799911, 1.373001, 3.036999, 2.438996}, // 254
{0.030000, 0.053000, -0.026000, -8.699912, -25.800001, 83.799911, 1.138001, 3.036999, 1.889996}, // 255
{0.030000, 0.053000, -0.026000, -8.699912, -25.800001, 83.799911, 1.138001, 3.036999, 1.889996}, // 256
{0.030000, 0.053000, -0.026000, -8.699912, -25.800001, 83.799911, 1.138001, 3.036999, 1.889996}, // 257
{0.065000, 0.091000, -0.026000, -8.699912, -25.800001, 83.799911, 1.418002, 3.036999, 2.230996}, // 258
{0.065000, 0.091000, -0.026000, -8.699912, -25.800001, 83.799911, 1.418002, 3.036999, 2.230996}, // 259
{0.065000, 0.035000, -0.026000, -8.699912, -25.800001, 83.799911, 0.999001, 3.036999, 1.600996}, // 260
{0.048000, 0.042000, -0.013000, -8.699912, -25.800001, 83.799911, 0.999001, 3.036999, 1.600996}, // 261
{0.061000, 0.042000, -0.016000, -8.699912, -25.800001, 83.799911, 0.999001, 3.036999, 1.600996}, // 262
{0.061000, 0.042000, -0.016000, -8.699912, -25.800001, 83.799911, 0.999001, 3.036999, 1.600996}, // 263
{0.040000, 0.042000, -0.016000, -8.699912, -25.800001, 83.799911, 0.983002, 3.036999, 1.767996}, // 264
{0.040000, 0.042000, -0.016000, -8.699912, -25.800001, 83.799911, 0.983002, 3.036999, 1.767996}, // 265
{0.040000, 0.042000, -0.016000, -8.699912, -25.800001, 83.799911, 0.983002, 3.036999, 1.767996}, // 266
{0.017000, 0.004000, -0.034000, -8.699912, -25.800001, 83.799911, 0.581001, 3.036999, 1.216996}, // 267
{0.017000, 0.042000, -0.026000, -8.699912, -25.800001, 83.799911, 0.860001, 3.036999, 1.685996}, // 268
{0.048000, 0.066000, -0.018000, -8.699912, -25.800001, 83.799911, 1.020001, 3.036999, 1.947996}, // 269
{0.048000, 0.052000, -0.018000, -8.699912, -25.800001, 83.799911, 1.020001, 3.036999, 1.947996}, // 270
{0.048000, 0.052000, -0.018000, -8.699912, -25.800001, 83.799911, 1.020001, 3.036999, 1.947996}, // 271
{0.048000, 0.087000, -0.018000, -8.699912, -25.800001, 83.799911, 1.449001, 3.036999, 2.544995}, // 272
{0.007000, 0.036000, -0.018000, -8.699912, -25.800001, 83.799911, 0.770001, 3.036999, 1.757996}, // 273
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 274
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 275
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 276
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 277
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 278
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 279
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 280
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 281
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 282
{0.026000, 0.047000, -0.018000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 283
{0.042000, 0.047000, -0.017000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 284
{0.069000, 0.021000, -0.029000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 285
{0.069000, 0.045000, -0.029000, -8.699912, -25.800001, 83.799911, 0.905001, 3.036999, 1.879996}, // 286
{0.065000, 0.045000, -0.021000, -8.699912, -25.800001, 83.799911, 1.182001, 3.036999, 2.423996}, // 287
{0.065000, 0.045000, -0.021000, -8.699912, -25.800001, 83.799911, 0.961001, 3.036999, 1.897996}, // 288
{0.065000, 0.045000, -0.021000, -8.699912, -25.800001, 83.799911, 0.857001, 3.036999, 1.744996}, // 289
{0.065000, 0.045000, -0.021000, -8.699912, -25.800001, 83.799911, 0.857001, 3.036999, 1.744996}, // 290
{0.030000, 0.038000, -0.021000, -8.699912, -25.800001, 83.799911, 0.680001, 3.036999, 1.562996}, // 291
{0.030000, 0.073000, -0.021000, -8.699912, -25.800001, 83.799911, 1.147001, 3.036999, 1.958996}, // 292
{0.050000, 0.031000, -0.025000, -8.699912, -25.800001, 83.799911, 0.841001, 3.036999, 1.370996}, // 293
{0.050000, 0.031000, -0.025000, -8.699912, -25.800001, 83.799911, 0.841001, 3.036999, 1.370996}, // 294
{-0.003999, 0.024000, -0.025000, -8.699912, -25.800001, 83.799911, 0.841001, 3.036999, 1.370996}, // 295
{-0.003999, 0.024000, -0.025000, -8.699912, -25.800001, 83.799911, 0.841001, 3.036999, 1.370996}, // 296
{0.019000, 0.050000, -0.039000, -8.699912, -25.800001, 83.799911, 0.841001, 3.036999, 1.370996}, // 297
{0.009000, 0.011000, -0.017000, -8.699912, -25.800001, 83.799911, 0.841001, 3.036999, 1.370996}, // 298
{0.009000, 0.057000, -0.017000, -8.699912, -25.800001, 83.799911, 0.841001, 3.036999, 1.370996}, // 299
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 300
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 301
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 302
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 303
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 304
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 305
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 306
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 307
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 308
{0.030000, 0.048000, -0.017000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 309
{0.030000, 0.048000, -0.025000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996}, // 310
{0.030000, 0.048000, -0.025000, -8.699912, -25.800001, 83.799911, 1.027001, 3.036999, 1.621996} // 311
};

new Float:loginCamera[][13] = {
{1573.006592, -1598.320435, -4.944113, 1572.520020, -1666.170044, -2.530392, 1487.590210, -1584.006226, 45.047333, 1507.403564, -1716.821167, 53.987839},
{1572.520020, -1666.170044, -2.530392, 1577.178711, -1829.642944, 10.939192, 1507.403564, -1716.821167, 53.987839, 1558.427856, -1732.269287, 23.853445},
{1577.178711, -1829.642944, 10.939192, 1569.679688, -1962.786255, 2.965276, 1558.427856, -1732.269287, 23.853445, 1567.960327, -1864.450317, 21.050629},
{1569.679688, -1962.786255, 2.965276, 1664.523438, -1857.660400, 41.780590, 1567.960327, -1864.450317, 21.050629, 1571.311523, -1856.334473, 77.971428},
{1664.523438, -1857.660400, 41.780590, 1723.841064, -1928.479736, 9.053623, 1571.311523, -1856.334473, 77.971428, 1673.827637, -1848.116821, 41.309757},
{1723.841064, -1928.479736, 9.053623, 1819.823730, -1870.588135, 28.623436, 1673.827637, -1848.116821, 41.309757, 1719.890015, -1869.689819, 32.150127},
{1819.823730, -1870.588135, 28.623436, 1866.801270, -1921.478638, 18.142744, 1719.890015, -1869.689819, 32.150127, 1795.094482, -1855.755127, 41.349644},
{1866.801270, -1921.478638, 18.142744, 1937.045288, -1721.500732, -14.408785, 1795.094482, -1855.755127, 41.349644, 1949.183472, -1809.354126, 31.791237},
{1937.045288, -1721.500732, -14.408785, 1989.832642, -1564.008057, 2.694421, 1949.183472, -1809.354126, 31.791237, 1935.950806, -1642.625122, 32.961555},
{1989.832642, -1564.008057, 2.694421, 1764.875244, -1605.435913, 10.603093, 1935.950806, -1642.625122, 32.961555, 1860.637817, -1606.854858, 39.369720},
{1764.875244, -1605.435913, 10.603093, 1661.782959, -1663.071289, 13.365737, 1860.637817, -1606.854858, 39.369720, 1681.971313, -1567.796631, 36.063114},
{1661.782959, -1663.071289, 13.365737, 1573.006592, -1598.320435, -4.944113, 1681.971313, -1567.796631, 36.063114, 1487.590210, -1584.006226, 45.047333}
};