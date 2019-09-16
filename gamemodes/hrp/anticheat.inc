ptask PlayerAntyCheat[500](playerid)
{
	// HP
	if( pInfo[playerid][player_health_changed] ) pInfo[playerid][player_health_changed] = false;
	else
	{
		new Float:health;
		GetPlayerHealth(playerid, health);
		if( health > pInfo[playerid][player_health] ) 
		{
			// TODO: cheatowala pizdeczka :D
		}
	}

	if(GetPlayerSpeed(playerid) > 250 && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
	{
		if(pInfo[playerid][player_aj] == 0)
		{
			if(pInfo[playerid][player_ac_warns] > 5)
			{
				new str[128], player_state_string[16];

				switch(GetPlayerState(playerid))
				{
					case 0: format(player_state_string, sizeof(player_state_string), "none");
					case 1: format(player_state_string, sizeof(player_state_string), "on-foot");
					case 2: format(player_state_string, sizeof(player_state_string), "driver");
					case 3: format(player_state_string, sizeof(player_state_string), "passenger");
					case 7: format(player_state_string, sizeof(player_state_string), "wasted");
					case 8: format(player_state_string, sizeof(player_state_string), "spawned");
					case 9: format(player_state_string, sizeof(player_state_string), "spectating");

					default: format(player_state_string, sizeof(player_state_string), "undefined");
				}

				format(str, sizeof(str), "AntyCheat: Rapid movement m2 (power %f, excess %.2f, state %s (%d))", GetPlayerSpeed(playerid), GetPlayerSpeed(playerid) - 250.0, player_state_string, GetPlayerState(playerid));
					
				pInfo[playerid][player_ac_warns] = 0;
				AddPlayerPenalty(playerid, PENALTY_TYPE_AJ, INVALID_PLAYER_ID, 0, str, 30);
			}
			else
			{
				PlayerLog(sprintf("Rapid movement {POWER:%.1f,EXCESS:%.1f}", GetPlayerSpeed(playerid), GetPlayerSpeed(playerid) - 250.0), pInfo[playerid][player_id], "anticheat");

				if(!pInfo[playerid][player_admin_duty]) pInfo[playerid][player_ac_warns]++;
			}
		}
	}

	if(GetPlayerSpeed(playerid) - pInfo[playerid][last_player_speed] > 90 && pInfo[playerid][last_player_speed] > 1)
	{
		new str[128];
		format(str, sizeof(str), "Too faster increasing speed (%f)", GetPlayerSpeed(playerid) - pInfo[playerid][last_player_speed]);
		AddPlayerPenalty(playerid, PENALTY_TYPE_AJ, INVALID_PLAYER_ID, 0, str, 30);
	}

	pInfo[playerid][last_player_speed] = GetPlayerSpeed(playerid);
}