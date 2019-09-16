/*Levele administracyjne:
0 - gracz
1 - gm 1
2 - gm 2
3 - gm 3
4 - gm 4
5 - gm 5
6 - CM
7 - ADMIN
*/

COMMAND:area(playerid, params[])
{
	new owner[32], type[8];
	if(pArea[pPlayer[playerid][player_area]][area_ownertype] == OWNER_TYPE_PLAYER) type = "Gracz";
	else if(pArea[pPlayer[playerid][player_area]][area_ownertype] == OWNER_TYPE_GROUP) type = "Grupa";
	else owner = "Brak w³aœciciela";

	if(pArea[pPlayer[playerid][player_area]][area_owner] == -1)
		format(owner, sizeof(owner), "Brak w³aœciciela");
	else
	{
		if(pArea[pPlayer[playerid][player_area]][area_ownertype] == OWNER_TYPE_PLAYER)
		{
			new Cache:result = mysql_query(m_sql, sprintf("SELECT `player_name` FROM `samp_players` WHERE `player_uid` = '%d'", pArea[pPlayer[playerid][player_area]][area_owner]));
			cache_get(0, "player_name", owner);
			cache_delete(result);
		}
		else if(pArea[pPlayer[playerid][player_area]][area_ownertype] == OWNER_TYPE_GROUP)
		{
			format(owner, sizeof(owner), "%d (UID GRUPY)", pArea[pPlayer[playerid][player_area]][area_owner]);	//POZNIEJ DODAC NAZWE		
		}
	}

	Debug(sprintf("%d %s %d %s %s", pPlayer[playerid][player_area], pArea[pPlayer[playerid][player_area]][area_name], pPlayer[playerid][player_area], (pArea[pPlayer[playerid][player_area]][area_ownertype]) ? ("Grupa") : ("Gracz"), owner));

	ShowPlayerInformation(playerid, sprintf("Informacje o strefie %s (%d)\n-\nTyp w³aœciciela: %s\nW³aœciciel: %s", pArea[pPlayer[playerid][player_area]][area_name], pPlayer[playerid][player_area], type, owner));
	return 1;
}

COMMAND:astrefa(playerid, params[]) 
{
	if(pPlayer[playerid][player_level] < 7) return CommandError(playerid);

	new sub[16], param[128];
	if( sscanf(params, "s[16] S()[128]", sub, param) ) return SendClientMessage(playerid, -1, "Tip: /astrefa [przypisz/usun/obiekty] (...)");

	if(!strcmp(sub, "przypisz"))
	{
		new type[8], uid, aid;
		if( sscanf(param, "ds[32]d", aid, type, uid) ) return SendClientMessage(playerid, -1, "Tip: /astrefa przypisz [id] [gracz/grupa] [uid gracza/uid grupy]");
		if( !Iter_Contains(Areas, aid) ) return SendClientMessage(playerid, -1, "Nie ma takiej strefy.");

		if( !strcmp(type, "gracz") )
		{
			mysql_query(m_sql, sprintf("UPDATE samp_areas SET area_ownertype = %d, area_owner = %d WHERE area_id = %d", OWNER_TYPE_PLAYER, uid, pArea[aid][area_id]));

			pArea[aid][area_owner] = uid;
			pArea[aid][area_ownertype] = OWNER_TYPE_PLAYER;

			ShowPlayerInformation(playerid, sprintf("Pomyœlnie ustawi³eœ w³aœciciela strefy %d na gracza o UID %d.", pArea[aid][area_id], uid));
		}

		else if( !strcmp(type, "grupa") )
		{
			mysql_query(m_sql, sprintf("UPDATE samp_areas SET area_ownertype = %d, area_owner = %d WHERE area_id = %d", OWNER_TYPE_GROUP, uid, pArea[aid][area_id]));

			pArea[aid][area_owner] = uid;
			pArea[aid][area_ownertype] = OWNER_TYPE_GROUP;

			ShowPlayerInformation(playerid, sprintf("Pomyœlnie ustawi³eœ w³aœciciela strefy %d na grupê o UID %d.", pArea[aid][area_id], uid));
		}
	}

	else return SendClientMessage(playerid, -1, "Tip: /astrefa [przypisz/usun/obiekty] (...)");

    return 1; 
} 
alias:astrefa("aarea", "as");