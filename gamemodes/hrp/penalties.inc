stock AddPlayerPenalty(playerid, type, giverid, endtime, reason[], extra_data = -1, bool:display_info = true, length = sizeof reason)
{
	new penalty_name[32];

	if( giverid == INVALID_PLAYER_ID )
	{
		if(!GetPlayerPermissions(playerid, 5))
		{
			if( pInfo[playerid][player_admin_duty] ) return;
		}
	}

	if( type == PENALTY_TYPE_BLOCK ) {
		// dezaktywujemy stare blokady o tym samym typie
		mysql_pquery(g_sql, sprintf("UPDATE crp_penalties SET penalty_deactivate = 1 WHERE penalty_type = %d AND penalty_extraid = %d AND penalty_owner = %d", type, extra_data, pInfo[playerid][player_id]));
	}

	if( giverid != INVALID_PLAYER_ID )
	{
		mysql_pquery(g_sql, sprintf("INSERT INTO `crp_penalties` (penalty_uid, penalty_type, penalty_owner, penalty_giver, penalty_date, penalty_end, penalty_reason, penalty_extraid) VALUES(null, %d, %d, %d, %d, %d, '%s', %d)", type, pInfo[playerid][player_id], pInfo[giverid][player_id], gettime(), endtime, reason, extra_data));
	}
	else 
	{
		if( type != PENALTY_TYPE_AJ ) mysql_pquery(g_sql, sprintf("INSERT INTO `crp_penalties` (penalty_uid, penalty_type, penalty_owner, penalty_giver, penalty_date, penalty_end, penalty_reason, penalty_extraid) VALUES(null, %d, %d, %d, %d, %d, '%s', %d)", type, pInfo[playerid][player_id], -1, gettime(), endtime, reason, extra_data));
	}
	
	// Nick daj¹cego
	new giver_name[MAX_PLAYER_NAME+1];
	if( giverid == INVALID_PLAYER_ID ) format(giver_name, sizeof(giver_name), "System");
	else format(giver_name, sizeof(giver_name), "%s", pInfo[giverid][player_name]);
	
	// Powód
	if( isnull(reason) ) format(reason, length, "brak");
	
	// Informacja
	switch(type)
	{
		case PENALTY_TYPE_KICK:
		{
			penalty_name = "Kick";
			if(display_info) DisplayPenaltyInformation("Kick", giver_name, pInfo[playerid][player_name], reason);
			
			Kick(playerid);
		}
		
		case PENALTY_TYPE_WARN:
		{
			penalty_name = "Warn";
			if(display_info) DisplayPenaltyInformation("Warn", giver_name, pInfo[playerid][player_name], reason);
		}
		
		case PENALTY_TYPE_BAN:
		{
			penalty_name = "Ban";
			mysql_pquery(g_sql, sprintf("INSERT INTO `crp_bans` VALUES(null, %d, '%s', '', '%s', 0)", gInfo[playerid][global_id], gInfo[playerid][global_ip], reason));
		
			if(display_info) DisplayPenaltyInformation(sprintf("Ban (%d dni)", extra_data), giver_name, pInfo[playerid][player_name], reason);
			
			Kick(playerid);
		}
		
		case PENALTY_TYPE_AJ:
		{
			penalty_name = "AdminJail";
			mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_aj` = %d WHERE `char_uid` = %d", extra_data*60, pInfo[playerid][player_id]));
				
			if(display_info) DisplayPenaltyInformation(sprintf("Admin Jail (%d min)", extra_data), giver_name, pInfo[playerid][player_name], reason);
			
			pInfo[playerid][player_aj] = extra_data * 60;
			pInfo[playerid][player_aj_end_time] = pInfo[playerid][player_aj] + gettime();
			
			scrp_SpawnPlayer(playerid, true);
		}
		
		case PENALTY_TYPE_BLOCK:
		{
			new timestr[20];
			new penalty_time = endtime - gettime();

			if( penalty_time < 0 )
			{
				format(timestr, sizeof(timestr), "nigdy nie wygasa");
			}
			else
			{
				if( penalty_time < 60*60 )
				{
					format(timestr, sizeof(timestr), "%d minut", floatround(penalty_time/60, floatround_floor));
				}
				else if( penalty_time < 60*60*24 )
				{
					format(timestr, sizeof(timestr), "%d godzin", floatround(penalty_time/(60*60), floatround_floor));
				}
				else
				{
					format(timestr, sizeof(timestr), "%d dni", floatround(penalty_time/(60*60*24), floatround_floor));
				}
			}
			
			
			switch( extra_data )
			{
				case BLOCK_CHAR:
				{
					penalty_name = "Blokada postaci";
					if(display_info) DisplayPenaltyInformation(sprintf("Blokada postaci (%s)", timestr), giver_name, pInfo[playerid][player_name], reason);
					
					Kick(playerid);
				}
				
				case BLOCK_OOC:
				{
					penalty_name = "Blokada OOC";
					if(display_info) DisplayPenaltyInformation(sprintf("Blokada OOC (%s)", timestr), giver_name, pInfo[playerid][player_name], reason);
				}
				
				case BLOCK_VEHICLES:
				{
					penalty_name = "Blokada prowadzenia pojazdów";
					if(display_info) DisplayPenaltyInformation(sprintf("Blokada prowadzenia pojazdów (%s)", timestr), giver_name, pInfo[playerid][player_name], reason);
					
					new vid = GetPlayerVehicleID(playerid);
					if( vid > 0 )
					{
						if( GetPlayerVehicleSeat(playerid) == 0 ) RemovePlayerFromVehicle(playerid);
					}
				}
				
				case BLOCK_RUN:
				{
					penalty_name = "Blokada biegania";
					if(display_info) DisplayPenaltyInformation(sprintf("Blokada biegania (%s)", timestr), giver_name, pInfo[playerid][player_name], reason);
				}
			}
			
			if( !PlayerHasBlock(playerid, extra_data) ) pInfo[playerid][player_block] += extra_data;
			setPlayerBlockTime(playerid, extra_data, endtime);
		}
		
		case PENALTY_TYPE_GS:
		{
			penalty_name = "GameScore";

			GivePlayercPoints(playerid, extra_data);
			
			if(display_info) 
			{
				if( extra_data > 0 ) DisplayPenaltyInformation(sprintf("Punkty GameScore (+%d)", extra_data), giver_name, pInfo[playerid][player_name], reason, true);
				else DisplayPenaltyInformation(sprintf("Punkty GameScore (%d)", extra_data), giver_name, pInfo[playerid][player_name], reason, false);
			}
			
		}
	}
	new string[512];

	if( type == PENALTY_TYPE_GS )
	{
		format(string, sizeof(string), "> Otrzymujesz %d GameScore od %s. Powód: %s.", extra_data, giver_name, reason);
		if(extra_data > 0) SendClientMessage(playerid, COLOR_GREEN, string);
		else SendClientMessage(playerid, COLOR_DARK_RED, string);
	}
	else
	{
		format(string, sizeof(string), "> Otrzymujesz karê (%s) od %s. Powód: %s.", penalty_name, giver_name, reason);

		SendClientMessage(playerid, COLOR_DARK_RED, string);
		SendClientMessage(playerid, COLOR_RED, "Je¿eli kara zosta³a nadana nies³usznie - mo¿esz apelowaæ na naszym forum.");
		SendClientMessage(playerid, COLOR_RED, "Wszystkie nadane kary s¹ logowane i znaleŸæ je mo¿esz w swoim profilu na stronie.");
		SendClientMessage(playerid, COLOR_RED, "Adres naszej strony: www.honest-rp.pl. Pamiêtaj, by nie za³atwiaæ takich spraw w grze!");
	}

	PlayerLog(sprintf("Penalty %s from %s {DATA:%d,REASON:%s}", penalty_name, PlayerLogLink(giverid != INVALID_PLAYER_ID ? pInfo[giverid][player_id] : -1), extra_data, reason), pInfo[playerid][player_id], "penalty");
	if( giverid != INVALID_PLAYER_ID) PlayerLog(sprintf("Given penalty %s to %s {DATA:%d,REASON:%s}", penalty_name, PlayerLogLink(playerid), extra_data, reason), pInfo[giverid][player_id], "penalty");
}

stock DisplayPenaltyInformation(header[], giver_name[], obtainer_name[], reason[], bool:green = false)
{	
	format(PenaltyQueue[pq_header], 64, header);
	format(PenaltyQueue[pq_giver_name], MAX_PLAYER_NAME+1, "%s", giver_name);
	format(PenaltyQueue[pq_obtainer_name], MAX_PLAYER_NAME+1, "%s", obtainer_name);
	if(green) PenaltyQueue[pg_green] = true;
	else PenaltyQueue[pg_green] = false;
	replacePolishChars(reason);
	format(PenaltyQueue[pq_reason], 256, "%s", reason);	

	ShowPenalty();
}

stock ShowPenalty()
{
	if( IsPenaltyDisplayed ) 
	{
		HidePenalty(PenaltyHideSecure);
		PenaltyHideSecure ++;
	}
	else
	{
		PenaltyHideSecure = 0;
	}
	
	// Formatowanie
	if(PenaltyQueue[pg_green])
	{
		if( strlen(PenaltyQueue[pq_reason]) > 0 ) TextDrawSetString(PenaltiesTextDraw, sprintf("~g~%s~n~~w~Gracz: %s~n~Nadajacy: %s~n~~y~%s", PenaltyQueue[pq_header], PenaltyQueue[pq_obtainer_name], PenaltyQueue[pq_giver_name], BreakLines(PenaltyQueue[pq_reason], "~n~", 60)));
		else TextDrawSetString(PenaltiesTextDraw, sprintf("~g~%s~n~Gracz: %s~n~Nadajacy: %s", PenaltyQueue[pq_header], PenaltyQueue[pq_obtainer_name], PenaltyQueue[pq_giver_name]));		
	}
	else
	{
		if( strlen(PenaltyQueue[pq_reason]) > 0 ) TextDrawSetString(PenaltiesTextDraw, sprintf("~r~%s~n~~w~Gracz: %s~n~Nadajacy: %s~n~~y~%s", PenaltyQueue[pq_header], PenaltyQueue[pq_obtainer_name], PenaltyQueue[pq_giver_name], BreakLines(PenaltyQueue[pq_reason], "~n~", 60)));
		else TextDrawSetString(PenaltiesTextDraw, sprintf("~r~%s~n~Gracz: %s~n~Nadajacy: %s", PenaltyQueue[pq_header], PenaltyQueue[pq_obtainer_name], PenaltyQueue[pq_giver_name]));
	}
	// Wyswietlenie
	foreach(new p : Player)
	{
		if( pInfo[p][player_logged] )
		{
			TextDrawShowForPlayer(p, PenaltiesTextDraw);
		}
	}
	
	IsPenaltyDisplayed = true;
	
	defer HidePenalty[PENALTY_DISPLAY_TIME](PenaltyHideSecure);
}

timer HidePenalty[10](secure)
{
	if( secure != PenaltyHideSecure ) return;
	
	IsPenaltyDisplayed = false;
	
	// Ukrywamy
	foreach(new p : Player)
	{
		TextDrawHideForPlayer(p, PenaltiesTextDraw);
	}
}