#define LOG_SOURCE_PLAYER 0
#define LOG_SOURCE_VEHICLE 1
#define LOG_SOURCE_GROUP 2

new LogSources[][3] = {
	'p',
	'v',
	'g'
};

function GroupLog(message[], grp_uid, action[])
{
	LogMessage(message, grp_uid, LOG_SOURCE_GROUP, "group", action, INFO);
}

function PlayerLog(message[], player_uid, action[])
{
	LogMessage(message, player_uid, LOG_SOURCE_PLAYER, "player", action, INFO);
}

function VehicleLog(message[], veh_uid, action[])
{
	LogMessage(message, veh_uid, LOG_SOURCE_VEHICLE, "vehicle", action, INFO);
}

function AdminLog(message[], admin_uid, action[])
{
	LogMessage(message, admin_uid, LOG_SOURCE_PLAYER, "admin", action, INFO);
}

function LogMessage(message[], source_id, source_type, type[], action[], E_LOGLEVEL:level)
{
	Log(logFile, level, "[%s] [%s] <%s:%d> %s", type, action, LogSources[source_type], source_id, message);
}


stock PlayerLogLink(player_uid)
{
	new link_str[40];
	format(link_str, sizeof(link_str), "<p:%d>", player_uid);

	return link_str;
}

stock VehicleLogLink(veh_uid)
{
	new link_str[40];
	format(link_str, sizeof(link_str), "<v:%d>", veh_uid);

	return link_str;
}

stock GroupLogLink(g_uid)
{
	new link_str[40];
	format(link_str, sizeof(link_str), "<g:%d>", g_uid);

	return link_str;
}

stock OfferTypeLogLink(offerType)
{
	new link_str[40];
	format(link_str, sizeof(link_str), "<ot:%d>", offerType);

	return link_str;
}

stock ItemLogLink(i_uid)
{
	new link_str[40];
	format(link_str, sizeof(link_str), "<i:%d>", i_uid);

	return link_str;
}

stock DoorLogLink(d_uid)
{
	new link_str[40];
	format(link_str, sizeof(link_str), "<d:%d>", d_uid);

	return link_str;
}

stock AreaLogLink(a_uid)
{
	new link_str[40];
	format(link_str, sizeof(link_str), "<a:%d>", a_uid);

	return link_str;
}

stock ActorLogLink(act_uid)
{
	new link_str[40];
	format(link_str, sizeof(link_str), "<act:%d>", act_uid);

	return link_str;
}