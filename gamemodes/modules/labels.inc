stock CreatePlayerLabel(playerid)
{
	new Float:x,
		Float:y,
		Float:z;

	GetPlayerPos(playerid, x, y, z);

	pPlayer[playerid][player_label] = Create3DTextLabel(sprintf("%d. %s", playerid, BeautifyName(playerid)), 0xE5E5E577, z, y, z, 15.0, GetPlayerVirtualWorld(playerid), 0);
	Attach3DTextLabelToPlayer(pPlayer[playerid][player_label], playerid, 0.0, 0.0, 0.1);
}

stock DestroyPlayerLabel(PlayerText3D:label)
{
	DeletePlayer3DTextLabel(playerid, pPlayer[playerid][player_label]);
}