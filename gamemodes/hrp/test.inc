stock RemovePlayersFromDoor(doorid)
{
	if( !Iter_Contains(Doors, doorid) ) return 0;
	// tu sprawdzamy czy to czasem nie jest przejscie do innych drzwi
	if( Door[doorid][door_spawn_vw] != Door[doorid][door_uid] ) return 0;

	foreach(new p : Player)
	{
		if(GetPlayerVirtualWorld(p) == Door[doorid][door_spawn_vw])
		{
			SetPlayerPos(p, Door[doorid][door_pos][0], Door[doorid][door_pos][1], Door[doorid][door_pos][2]);
			SetPlayerFacingAngle(p, Door[doorid][door_pos][3]);
			SetPlayerVirtualWorld(p, Door[doorid][door_vw]);
			SetPlayerInterior(p, Door[doorid][door_int]);

			SendClientMessage(p, -1, sprintf("Zosta³eœ wyrzucony z drzwi %s (UID: %d; ID: %d)", Door[doorid][door_name], Door[doorid][door_uid], doorid));
		}
	}
}