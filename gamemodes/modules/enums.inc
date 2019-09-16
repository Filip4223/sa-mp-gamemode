enum eSettings
{
	setting_weather,
	setting_time,
	bool:setting_show_nametags,
	setting_markers,
	Float:setting_nametags_drawdistance,
	bool:setting_stunts,
	bool:setting_default_interiors,
}
new Setting[eSettings];

enum ePlayer
{
	player_uid,
	player_name[MAX_PLAYER_NAME+1],
	player_pass[128],
	player_level,
	player_language,
	bool:player_logged,
	player_session_uid,
	player_skin,
	player_cash,
	player_bank_cash,
	Float:player_pos[4],
	Text3D:player_label,
	player_zone,
	player_area,
}
new pPlayer[MAX_PLAYERS][ePlayer];

enum eArea
{
	area_id,
	area_game_id,
	area_name[8],
	Float:area_pos_min_x,
	Float:area_pos_min_y,
	Float:area_pos_max_x,
	Float:area_pos_max_y,
	area_owner,
	area_ownertype,
	area_color[16],
}
new pArea[MAX_AREAS][eArea];