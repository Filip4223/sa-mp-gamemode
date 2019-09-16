stock GetFormattedDate(timestamp, dest[])
{
	new date[40], year, month, day, hour, minute, second;
	TimestampToDate(timestamp, year, month, day, hour, minute, second, 1, 0);
	
	format(date, sizeof(date), "%02d.%02d.%d, %02d:%02d", day, month, year, hour, minute);
	
	strcopy(dest, date);
}

stock DateToString(timestamp)
{
	new string[64];

	new difference = (gettime() - timestamp);

	if(difference <= 60) format(string, sizeof(string), "mniej ni¿ minutê temu."); 
	else if(difference > 60 && difference < 3600) format(string, sizeof(string), "%d minut temu", floatround(difference / 60));
	else if(difference > 3600 && difference < 86400) format(string, sizeof(string), "%d godzin temu", floatround(difference / 3600));
	else if(difference > 86400 && difference < 2678400) format(string, sizeof(string), "%d dni temu", floatround(difference / 86400));
	else if(difference > 2678400 && difference < 32140800) format(string, sizeof(string), "%d miesiêcy temu", floatround(difference / 2678400));
	else if(difference > 32140800) format(string, sizeof(string), "%d lat temu", floatround(difference / 32140800));
	else if(difference > 1510617600) format(string, sizeof(string), "nigdy");

	return _:string;
}

stock Float:GetPlayerSpeed(playerid)
{
	new Float:speed_x, Float:speed_y, Float:speed_z, Float:speed_value;
	if(IsPlayerInAnyVehicle(playerid)) GetVehicleVelocity(GetPlayerVehicleID(playerid), speed_x, speed_y, speed_z); 
	else GetPlayerVelocity(playerid, speed_x, speed_y, speed_z);
	speed_value = floatsqroot((speed_x * speed_x) + (speed_y * speed_y) + (speed_z * speed_z)) * 198.0;
	return speed_value;
}

stock GetPlayerByName(str[])
{
	foreach(new p : Player)
	{
		new name[MAX_PLAYER_NAME];
		GetPlayerName(p, name, sizeof(name));
		if(!strcmp(name, str)) return _:p;
	}

	return -1;
}

stock HexToInt(string[])
{
	if( string[0] == 0 ) return 0;
	new i, cur = 1, res = 0;
	for(i = strlen(string);i > 0;i--) 
	{
		if (string[i-1]<58) res=res+cur*(string[i-1]-48); 
		else res=res+cur*(string[i-1]-65+10);
		cur=cur*16;
	}
	return res;
}

stock Float:GetDistanceBetweenPoints(Float:x1,Float:y1,Float:z1,Float:x2,Float:y2,Float:z2)
{
	return floatsqroot(floatadd(floatadd(floatpower(floatsub(x1,x2),2),floatpower(floatsub(y1,y2),2)),floatpower(floatsub(z1,z2),2)));
}

stock GetWeaponSlot(weaponid)
{
	new slot = -1;
	switch(weaponid)
	{
		case 0,1: slot = 0;
		case 2 .. 9: slot = 1;
		case 10 .. 15: slot = 10;
		case 16 .. 18, 39: slot = 8;
		case 22 .. 24: slot =2;
		case 25 .. 27: slot = 3;
		case 28, 29, 32: slot = 4;
		case 30, 31: slot = 5;
		case 33, 34: slot = 6;
		case 35 .. 38: slot = 7;
		case 40: slot = 12;
		case 41 .. 43: slot = 9;
		case 44 .. 46: slot = 11;
	}
	return slot;
}

stock PlayerName(playerid)
{
	if(pInfo[playerid][player_duty_tmp][0] == EOS) return _:pInfo[playerid][player_name];
	else return _:pInfo[playerid][player_duty_tmp];
}

stock GetWeaponType(weaponid)
{
	new type = -1;
	
	switch(weaponid)
	{
		case 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15: type = WEAPON_TYPE_MELEE;
		case 22,23,24,28,32: type = WEAPON_TYPE_SHORT;
		case 25,26,27,29,30,31: type = WEAPON_TYPE_LONG;
		case 16,17,18,39,40: type = WEAPON_TYPE_GRENADE;
		case 33,34: type = WEAPON_TYPE_SNIPERS;
		
		default: type = WEAPON_TYPE_SPECIAL;
	}
	
	return type;
}

stock PlayerSetSpectate(playerid, targetid)
{
	if( !IsPlayerConnected(targetid) ) return cmd_specoff(playerid, "");
	if( !pInfo[targetid][player_logged] ) return cmd_specoff(playerid, "");
	
	SetPlayerInterior(playerid, GetPlayerInterior(targetid));
	SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));
	
	new vid = GetPlayerVehicleID(targetid);
	if( vid > 0 )
	{
		PlayerSpectateVehicle(playerid, vid);
	}
	else
	{
		PlayerSpectatePlayer(playerid, targetid);
	}
	
	pInfo[playerid][player_admin_spec_id] = targetid;
	
	new string[420];
	format(string, sizeof(string), "%d. %s~n~~n~~p~UID: ~w~%d    ~w~GID: %d (%s)", targetid, pInfo[targetid][player_name], pInfo[targetid][player_id], gInfo[targetid][global_id], gInfo[targetid][global_name]);
	
	new time = GetPlayerOnlineTime(targetid), hour = floatround(time/3600, floatround_floor);
	time -= hour * 3600;
	new minute = floatround(time/60, floatround_floor);
	
	new duty[80], slot = GetPlayerDutySlot(targetid);
	if( slot > -1 ) format(duty, sizeof(duty), "%s (UID: %d)", Group[pGroup[targetid][slot][pg_id]][group_name], Group[pGroup[targetid][slot][pg_id]][group_uid]);
	else format(duty, sizeof(duty), "brak");
	
	format(string, sizeof(string), "%s~n~~y~Czas online: ~w~%02dh %02dm    ~y~Sluzba: ~w~%s", string, hour, minute, duty);
	format(string, sizeof(string), "%s~n~~g~Gotowka: ~w~$%d    ~g~Bank: ~w~$%d", string, pInfo[targetid][player_money], pInfo[targetid][player_bank_money]);
	format(string, sizeof(string), "%s~n~~n~~w~Uzywaj ~r~SPACE ~w~oraz ~r~LCTRL ~w~aby zmieniaæ ogladanych graczy, wpisz /specoff aby zakonczyc.", string);
	
	PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], string);
	PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);
	
	return 1;
}

stock GetPlayerNextSpectateId(playerid)
{
	new targetid = pInfo[playerid][player_admin_spec_id];
	
	targetid = Iter_Next(Player, targetid);
	
	if( targetid == playerid )
	{
		targetid = Iter_Next(Player, targetid);
	}
	
	if( !IsPlayerConnected(targetid) )
	{
		targetid = INVALID_PLAYER_ID;
	}
	
	return targetid;
}

stock GetPlayerPrevSpectateId(playerid)
{
	new targetid = pInfo[playerid][player_admin_spec_id];
	
	targetid = Iter_Prev(Player, targetid);
	
	if( targetid == playerid )
	{
		targetid = Iter_Prev(Player, targetid);
	}
	
	if( !IsPlayerConnected(targetid) )
	{
		targetid = INVALID_PLAYER_ID;
	}
	
	return targetid;
}

stock replacePolishChars(str[])
{
	for(new i=0; i<strlen(str); i++)
	{
		switch(str[i])
		{
			//other languages supported by samp
			case 192 .. 197: str[i] = 0x41;
			case 200 .. 203: str[i] = 0x45;
			case 224 .. 229: str[i] = 0x61;
			case 232 .. 235: str[i] = 0x65;
			case 204 .. 207: str[i] = 0x49;
			case 236 .. 239: str[i] = 0x69;
			case 210 .. 214: str[i] = 0x4f;
			case 242 .. 246: str[i] = 0x6f;
			case 217 .. 220: str[i] = 0x55;
			case 249 .. 252: str[i] = 0x75;
			//polish
			case 185: str[i] = 0x61; //male a
			case 230: str[i] = 0x63; //male c
			case 179: str[i] = 0x6C; //male l
			case 241: str[i] = 0x6E; //male n
			case 156: str[i] = 0x73; //male s
			case 159: str[i] = 0x7A; //male z
			case 191: str[i] = 0x7A; //male z
			case 165: str[i] = 0x41; //duze A
			case 198: str[i] = 0x43; //duze C
			case 163: str[i] = 0x4C; //duze L
			case 209: str[i] = 0x4E; //duze N
			case 140: str[i] = 0x53; //duze S
			case 143: str[i] = 0x5A; //duze Z
			case 175: str[i] = 0x5A; //duze Z
		}
	}
}

stock GivePayday(playerid, slot)
{
	if((gettime() - pGroup[playerid][slot][pg_last_payday]) < 86400) return 0;
	else {
		SendClientMessage(playerid, -1, "dostajesz wyplate");
		mysql_pquery(g_sql, sprintf("UPDATE crp_char_groups SET group_payday = %d WHERE char_uid = %d, group_belongs = %d", gettime(), pInfo[playerid][player_id], Group[pGroup[playerid][slot][pg_id]][group_uid]));
	}



	return 1;
}

stock Alert(playerid, type, desc[])
{
	replacePolishChars(desc);

	if(!pInfo[playerid][player_interface] || strlen(desc) > 60) 
	{
		InfoboxRight(playerid, 4, desc);
		return 1;
	}

	if(pInfo[playerid][player_txd_count] >= 4) return 1; //wiecej sie nie zmiesci na ekranie

	switch(type)
	{
		case ALERT_TYPE_WARNING: //WARNING (wykrzyknik)
		{
			pInfo[playerid][player_txd_count]++;
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][0], "mdl-2005:notka");
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][1], "Ostrzezenie");
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][2], desc);

			for(new i=0; i<3; i++) PlayerTextDrawShow(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][i]);

			defer SetTextDrawColor[2](playerid, -255, pInfo[playerid][player_txd_count]);
		}
		case ALERT_TYPE_INFO: //INFO (informacja)
		{
			pInfo[playerid][player_txd_count]++;
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][0], "mdl-2005:notkainfo");
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][1], "Informacja");
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][2], desc);

			for(new i=0; i<3; i++) PlayerTextDrawShow(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][i]);

			defer SetTextDrawColor[2](playerid, -255, pInfo[playerid][player_txd_count]);		
		}
		case ALERT_TYPE_NEGATIVE: //NEGATIVE (wykrzyknik)
		{
			pInfo[playerid][player_txd_count]++;
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][0], "mdl-2005:notkanegative");
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][1], "Wystapil blad");
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][2], desc);

			for(new i=0; i<3; i++) PlayerTextDrawShow(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][i]);

			defer SetTextDrawColor[2](playerid, -255, pInfo[playerid][player_txd_count]);					
		}
		case ALERT_TYPE_SUCCESS: //SUCCESS (ptaszek)
		{
			pInfo[playerid][player_txd_count]++;
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][0], "mdl-2005:ok");
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][1], "Sukces");
			PlayerTextDrawSetString(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][2], desc);

			for(new i=0; i<3; i++) PlayerTextDrawShow(playerid, SmallAlert[pInfo[playerid][player_txd_count]][playerid][i]);

			defer SetTextDrawColor[2](playerid, -255, pInfo[playerid][player_txd_count]);	
		}
	}

	return 1;
}

stock Float:GetPlayerDistanceFromPlayer(playerid, targetid)
{
	new Float:fDist[3];
	GetPlayerPos(playerid, fDist[0], fDist[1], fDist[2]);
	return GetPlayerDistanceFromPoint(targetid, fDist[0], fDist[1], fDist[2]);
}

stock IsPlayerOnlinyByUid(uid)
{
	foreach(new p : Player)
	{
		if(pInfo[p][player_id] == uid) return p;
	}

	return INVALID_PLAYER_ID;
}

function OnTransactionListLoaded(playerid, page, all_rows)
{	
	DynamicGui_Init(playerid);

	pInfo[playerid][player_dialog_tmp1] = page;
	
	new str[2700];

	format(str, sizeof(str), "Typ\tData\tOpis\tIloœæ\n");
	
	if( page > 1 )
	{
		format(str, sizeof(str), "%s \t"HEX_COLOR_SAMP"Poprzednia strona\n", str);
		DynamicGui_AddRow(playerid, DG_TRANSACTIONS_PREV);
	}
	
	new tmp_str[60];
	
	for(new i;i<cache_get_rows();i++)
	{
		cache_get(i, "transaction_description", tmp_str);
		new price = cache_get_int(i, "transaction_price");
		new type = cache_get_int(i, "transaction_type");
		new date = cache_get_int(i, "transaction_date");

		new date_string[32];
		GetRelativeDate(date, date_string);

		new type_string[16];
		switch(type)
		{
			case 0: format(type_string, sizeof(type_string), "  {94c684}+"); //dodatnio
			case 1: format(type_string, sizeof(type_string), "  {ed6161}-"); //ujemnie
			case 2: format(type_string, sizeof(type_string), "  {75aaff}-"); //debet -
			case 3: format(type_string, sizeof(type_string), "  {75aaff}+"); //debet +
		}

		new price_string[16];
		if(type == 2 || type == 3)
		{
			if(price < 0) format(price_string, 16, "{94c684}$%d", price);
			else format(price_string, 16, "{ed6161}$%d", price);
		}
		else
		{
			if(price < 0) format(price_string, 16, "{ed6161}$%d", price);
			else format(price_string, 16, "{94c684}$%d", price);
		}

		format(str, sizeof(str), "%s%s\t%s\t%s\t%s\n", str, type_string, date_string, tmp_str, price_string);
		DynamicGui_AddBlankRow(playerid);
	}
	
	if( all_rows > (page*20) )
	{
		format(str, sizeof(str), "%s \t"HEX_COLOR_SAMP"Nastepna strona\n", str);
		DynamicGui_AddRow(playerid, DG_TRANSACTIONS_NEXT);
	}
	
	ShowPlayerDialog(playerid, DIALOG_TRANSACTIONS_HISTORY, DIALOG_STYLE_TABLIST_HEADERS, "Historia transakcji", str, "Wybierz", "Wróæ");

	return 1;	
}

stock ShowPlayerTransactionsHistory(playerid, page=1)
{
	new Cache:result;
	result = mysql_query(g_sql, "SELECT COUNT(*) as count FROM `crp_transactions`");
	
	new all_rows = cache_get_int(0, "count");
	
	cache_delete(result);

	mysql_tquery(g_sql, sprintf("SELECT * FROM `crp_transactions` LIMIT %d, 20", (page-1)*20), "OnTransactionListLoaded", "iii", playerid, page, all_rows);
	
	return 1;
}

stock randomString(strDest[], strLen = 10)
{
    while(strLen--)
        strDest[strLen] = random(2) ? (random(26) + (random(2) ? 'a' : 'A')) : (random(10) + '0');
}

stock Float:AngleBetweenPoints(Float:XA, Float:YA, Float:XB, Float:YB)
{
	new Float:Angle=-(90+(atan2(YA-YB,XA-XB)));// * 180.0 / 3.141592653;
	return Angle;
}

/*
/   distance - odleglosc od gracza
/   angle - +30.0 gdy chcemy przesunac wzglednie w lewo oraz -30.0 gdy wzglednie prawo
/
*/
stock GetPointInAngleOfPlayer(playerid, &Float:x, &Float:y, &Float:z, Float:distance, Float:angle)
{
	new Float:current_angle;
	GetPlayerPos(playerid, x, y, z);
	
	GetPlayerFacingAngle(playerid, current_angle);

	if( IsPlayerInAnyVehicle(playerid) ) 
	{
        GetVehicleZAngle(GetPlayerVehicleID(playerid), current_angle);
    }
	
	new Float:a = current_angle + angle;
	
	x += (distance * floatsin(-a, degrees));
	y += (distance * floatcos(-a, degrees));
}

/*stock PlayerLog(playerid, prefix[], str[], {Float, _}:...)
{
    new arguments = (numargs() - 3) * 4;
    if(arguments)
    {
        new message[256];
        new pos_start, pos_end;

        #emit CONST.alt        str
        #emit LCTRL          5
        #emit ADD
        #emit STOR.S.pri        pos_start

        #emit LOAD.S.alt        arguments
        #emit ADD
        #emit STOR.S.pri        pos_end

        do {
            #emit LOAD.I
            #emit PUSH.pri
            pos_end -= 4;
            #emit LOAD.S.pri      pos_end
        }
        while(pos_end > pos_start);

        #emit PUSH.S          str
        #emit PUSH.C          256
        #emit PUSH.ADR         message

        arguments += 12;
        #emit PUSH.S          arguments
        #emit SYSREQ.C         format

        arguments += 4;
        #emit LCTRL          4
        #emit LOAD.S.alt        arguments
        #emit ADD
        #emit SCTRL          4

        new output[272];
        format(output, sizeof(output), "%s %s", prefix, message);
        Log(pInfo[playerid][player_log], INFO, output);
    } 
    else 
    {
        new output[272];
        format(output, sizeof(output), "%s %s", prefix, str);
    	Log(pInfo[playerid][player_log], INFO, output);
    }

    return 1;
}*/

stock GetPointInAngleOfActor(actorid, &Float:x, &Float:y, &Float:z, Float:distance, Float:angle)
{
	new Float:current_angle;
	Streamer_GetFloatData(STREAMER_TYPE_ACTOR, actorid, E_STREAMER_X, x);
    Streamer_GetFloatData(STREAMER_TYPE_ACTOR, actorid, E_STREAMER_Y, y);
    Streamer_GetFloatData(STREAMER_TYPE_ACTOR, actorid, E_STREAMER_Z, z);
	
	GetDynamicActorFacingAngle(actorid, current_angle);
	
	new Float:a = current_angle + angle;
	
	x += (distance * floatsin(-a, degrees));
	y += (distance * floatcos(-a, degrees));	
}

stock GetVehicleSpeed(vehicleid)
{
    if(vehicleid != INVALID_VEHICLE_ID)
    {
		new Float:Pos[3],Float:VS ;
		GetVehicleVelocity(vehicleid, Pos[0], Pos[1], Pos[2]);
		VS = floatsqroot(Pos[0]*Pos[0] + Pos[1]*Pos[1] + Pos[2]*Pos[2])*197;
		return floatround(VS,floatround_round);
    }
    return INVALID_VEHICLE_ID;
}

stock SetPlayerFacingPlayer(playerid, targetid)
{
	new Float:p_pos[4];
	GetPlayerFacingAngle(playerid, p_pos[3]);
	
	if( p_pos[3] <= 90.0 ) SetPlayerFacingAngle(targetid, 180.0 + p_pos[3]);
	else if( p_pos[3] > 270.0 ) SetPlayerFacingAngle(targetid, 90.0 + (p_pos[3] - 270.0));
	else if( p_pos[3] > 90.0 && p_pos[3] <= 180.0 ) SetPlayerFacingAngle(targetid, 270.0 + (p_pos[3] - 90.0));
	else if( p_pos[3] > 180.0 && p_pos[3] <= 270.0 ) SetPlayerFacingAngle(targetid, 0.0 + (p_pos[3] - 180.0));
	
	return 1;
}

stock GetRelativeDate(timestamp, dest[], length = sizeof dest)
{
	if( timestamp == 0 ) strcopy(dest, "Nigdy");
	else
	{
		new 
			year, month, day, hour, minute, second,
			t_year, t_month, t_day, t_hour, t_minute, t_second;
		
		// Pobieramy date ostatniego logowania
		TimestampToDate(timestamp, t_year, t_month, t_day, t_hour, t_minute, t_second, 1, 0);
		
		// Na poczatku sprawdzamy czy to czasem nie bylo dzisiaj ;)
		TimestampToDate(gettime(), year, month, day, hour, minute, second, 1, 0);
		
		if( t_year == year && t_month == month && t_day == day )
		{
			format(dest, length, "dziœ, %02d:%02d", t_hour, t_minute);
			return;
		}
		
		// Jesli to nie bylo dzisiaj to sprawdzamy czy wczoraj
		TimestampToDate(gettime()-86400, year, month, day, hour, minute, second, 1, 0);
		
		if( t_year == year && t_month == month && t_day == day )
		{
			format(dest, length, "wczoraj, %02d:%02d", t_hour, t_minute);
			return;
		}
		
		// Ani dzisiaj, ani wczoraj - wyswietlamy date
		format(dest, length, "%02d.%02d.%02d, %02d:%02d", t_day, t_month, t_year, t_hour, t_minute);
		return;
	}
}

stock IsValidSkin(skinid)
{
    if( skinid == 74 || skinid < 0 ) return 0;
     
    if( skinid > 299 )
    {
    	new Cache:result;
    	result = mysql_query(g_sql, sprintf("SELECT id AS `exists` FROM crp_custom_models WHERE type = 'skin' AND gameid = %d", skinid));
    	new rows = cache_get_rows();
    	cache_delete(result);

    	if( rows > 0 ) return 1;

    	return 0;
    }

    return 1;
}

stock UnderscoreToSpace(string[])
{
	strreplace_char(string, '_', ' ');
}

stock SpaceToUnderscore(string[])
{
	strreplace_char(string, ' ', '_');
}

stock strreplace_char(string[], find, replace)
{
    for(new i=0; string[i]; i++)
    {
        if(string[i] == find)
        {
            string[i] = replace;
        }
    }
}

stock str_replace (newstr [], oldstr [], srcstr [], deststr [], bool: ignorecase = false, size = sizeof (deststr))
{
    new
        newlen = strlen (newstr),
        oldlen = strlen (oldstr),
        srclen = strlen (srcstr),
        idx,
        rep;

    for (new i = 0; i < srclen; ++i)
    {
        if ((i + oldlen) <= srclen)
        {
            if (!strcmp (srcstr [i], oldstr, ignorecase, oldlen))
            {
                deststr [idx] = '\0';
                strcat (deststr, newstr, size);
                ++rep;
                idx += newlen;
                i += oldlen - 1;
            }
            else
            {
                if (idx < (size - 1))
                    deststr [idx++] = srcstr [i];
                else
                    return rep;
            }
        }
        else
        {
            if (idx < (size - 1))
                deststr [idx++] = srcstr [i];
            else
                return rep;
        }
    }
    deststr [idx] = '\0';
    return rep;
}

stock utf8_translate(source[], dest[])
{
	new j = 0;
	for(new i=0;i<strlen(source);i++)
	{
		if( source[i] < 128 )
		{
			dest[j++] = source[i];
		}
		else
		{
			switch( source[i] )
			{
				case '¹':
				{
					dest[j++] = 196;
					dest[j++] = 133;
				}

				case '¥':
				{
					dest[j++] = 196;
					dest[j++] = 132;
				}

				case 'æ':
				{
					dest[j++] = 196;
					dest[j++] = 135;
				}

				case 'Æ':
				{
					dest[j++] = 196;
					dest[j++] = 134;
				}

				case 'ê':
				{
					dest[j++] = 196;
					dest[j++] = 153;
				}

				case 'Ê':
				{
					dest[j++] = 196;
					dest[j++] = 152;
				}

				case '³':
				{
					dest[j++] = 197;
					dest[j++] = 130;
				}

				case '£':
				{
					dest[j++] = 197;
					dest[j++] = 129;
				}

				case 'ñ':
				{
					dest[j++] = 197;
					dest[j++] = 132;
				}

				case 'Ñ':
				{
					dest[j++] = 197;
					dest[j++] = 131;
				}

				case 'ó':
				{
					dest[j++] = 195;
					dest[j++] = 179;
				}

				case 'Ó':
				{
					dest[j++] = 195;
					dest[j++] = 147;
				}

				case 'œ':
				{
					dest[j++] = 197;
					dest[j++] = 155;
				}

				case 'Œ':
				{
					dest[j++] = 197;
					dest[j++] = 154;
				}

				case 'Ÿ':
				{
					dest[j++] = 197;
					dest[j++] = 186;
				}

				case '':
				{
					dest[j++] = 197;
					dest[j++] = 185;
				}

				case '¿':
				{
					dest[j++] = 197;
					dest[j++] = 188;
				}

				case '¯':
				{
					dest[j++] = 197;
					dest[j++] = 187;
				}
			}
		}
	}

	dest[j] = '\0';
}

stock replaceColorCodes(string[])
{
	new text[200];
	format(text, sizeof(text), "%s", string);
	
	c_code_replace(text, "[b]", "~b~");
	c_code_replace(text, "[r]", "~r~");
	c_code_replace(text, "[g]", "~g~");
	c_code_replace(text, "[p]", "~p~");
	c_code_replace(text, "[y]", "~y~");
	c_code_replace(text, "[h]", "~h~");
	c_code_replace(text, "[w]", "~w~");
	
	return text;
}

stock c_code_replace(str[], find[], replace[], length = sizeof str)
{
	new pos;
	while( (pos = strfind(str, find), pos) > -1 )
	{
		strdel(str, pos, pos+strlen(find));
		strins(str, replace, pos, length);
	}
}

stock BreakLines(string[], delimiter[], limit)
{
	new inserts, tempLimit = limit, pos[50], string2[250], lastEmptyPos;
	format(string2, 250, string);
	
	for(new i; i < strlen(string); i++)
	{
		if( string[i] == ' ' ) lastEmptyPos = i;
		if( string[i] == '~' && string[i+1] == 'n' && string[i+2] == '~' ) tempLimit = i + limit;
		if( i >= tempLimit )
		{
			inserts += 1;
			tempLimit = i + limit;
			
			pos[inserts-1] = lastEmptyPos + ((inserts-1) * strlen(delimiter));
			if( inserts > 1 ) pos[inserts-1] -= (inserts-1);
		}
	}
	
	for(new d; d < 50; d++)
	{
		if( pos[d] == 0 ) break;
		strdel(string2, pos[d], pos[d]+1);
		strins(string2, delimiter, pos[d]);
	}
	
	return _:string2;
}

stock sortIntegers(array[], left, right)
{
    new
        tempLeft = left,
        tempRight = right-1,
        pivot = array[(left + right) / 2],
        tempVar
    ;
    while(tempLeft <= tempRight)
    {
        while(array[tempLeft] < pivot) tempLeft++;
        while(array[tempRight] > pivot) tempRight--;
        
        if(tempLeft <= tempRight)
        {
            tempVar = array[tempLeft], array[tempLeft] = array[tempRight], array[tempRight] = tempVar;
            tempLeft++, tempRight--;
        }
    }
    if(left < tempRight) sortIntegers(array, left, tempRight);
    if(tempLeft < right) sortIntegers(array, tempLeft, right);
}

stock BeautifyString(string[], capitalize = true, add_dot = true, add_shout_mark = false, add_megaphone_mark = false)
{
	new beauty[400];
	format(beauty, sizeof(beauty), string);
	
	if( capitalize ) beauty[0] = toupper(beauty[0]);	
	if( add_dot )
	{
		if( beauty[strlen(string)-1] != '.' && beauty[strlen(string)-1] != '!' && beauty[strlen(string)-1] != '?' )
		{
			strins(beauty, ".", strlen(string));
		}
	}
	if( add_shout_mark )
	{
		if( beauty[strlen(string)-1] != '.' )
		{
			strins(beauty, "!", strlen(string));
		}
		else
		{
			strdel(beauty, strlen(beauty)-1, strlen(beauty));
			strins(beauty, "!", strlen(string)-1);
		}
	}
	if( add_megaphone_mark )
	{
		if( beauty[strlen(string)-1] != '.' )
		{
			strins(beauty, "!!!", strlen(string));
		}
		else
		{
			strdel(beauty, strlen(beauty)-1, strlen(beauty));
			strins(beauty, "!!!", strlen(string)-1);
		}
	}
	//test

	//testa

	return _:beauty;
}

stock strsearch(const string[], sub, len, maxlen)
{
	if( len > maxlen ) len = maxlen;
	for( new i = len; i >= 0; i-- )
	{
		if( string[i] == sub )
		{
			return i;
		}
	}
	
	return -1;
}

stock Float:Min(Float:x1, Float:x2)
{
	if( floatcmp(x1, x2) == 1 ) return x2;
	else return x1;
}

stock Float:Max(Float:x1, Float:x2)
{
	if( floatcmp(x1, x2) == 1 ) return x1;
	else return x2;
}

stock IsNumeric(const string[])
{
    for (new i = 0, j = strlen(string); i < j; i++)
    {
        if (string[i] > '9' || string[i] < '0') return 0;
    }
    return 1;
}

stock ProxMessage(playerid, text[], type)
{
	// Some config
	new Float:ProxConfigDistance[][5] =
		{
			{ 10.0, 8.0, 6.0, 4.0, 2.0 }, // PROX_NORMAL
			{ 25.0, 18.0, 12.0, 8.0, 3.0 }, // PROX_SHOUT
			{ 3.0, 2.5, 2.0, 1.5, 1.0 }, // PROX_QUIET
			{ 12.0, 12.0, 12.0, 12.0, 12.0}, // PROX_ME
			{ 12.0, 12.0, 12.0, 12.0, 12.0},  // PROX_DO
			{ 10.0, 8.0, 6.0, 4.0, 2.0 }, // PROX_LOCAL_L
			{ 10.0, 10.0, 10.0, 10.0, 10.0 },// PROX_OOC
			{ 9.0, 7.0, 5.5, 4.0, 2.0 }, // PROX_RADIO
			{ 7.0, 5.0, 4.5, 3.0, 1.5 }, // PROX_PHONE
			{ 45.0, 45.0, 45.0, 45.0, 45.0}, // PROX_MEGAFON
			{ 12.0, 12.0, 12.0, 12.0, 12.0}, // PROX_TRY
			{ 12.0, 12.0, 12.0, 12.0, 12.0} // PROX_AME
		};
		
	new ProxConfigColor[][5] =
		{
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_LOCAL
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_SHOUT
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xD8D8D8FF }, // PROX_QUIET
			{ 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF }, // PROX_ME
			{ 0x9B91ECFF, 0x9B91ECFF, 0x9B91ECFF, 0x9B91ECFF, 0x9B91ECFF }, // PROX_DO
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_LOCAL_L
			{ COLOR_GREY, COLOR_GREY, COLOR_GREY, COLOR_GREY, COLOR_GREY }, // PROX OOC	
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_RADIO
			{ 0x737373FF, 0x888888FF, 0xB0B0B0FF, 0xD8D8D8FF, 0xFFFFFFFF }, // PROX_PHONE
			{ 0xFFFB00FF, 0xFFFB00FF, 0xFFFB00FF, 0xFFFB00FF, 0xFFFB00FF }, // PROX_MEGAFON
			{ 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF }, // PROX_TRY
			{ 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF, 0xB58ADAFF } // PROX_AME
		};
	
	new ProxConfigText[][32] =
		{
			"mówi", // PROX_LOCAL
			"krzyczy", // PROX_SHOUT
			"szepcze", // PROX_QUIET
			"",
			"",
			"mówi", // PROX_LOCAL_L
			"",
			"(radio)", // PROX_RADIO
			"(telefon)",
			"(megafon) :o<", // PROX_MEGAFON
			"", // PROX_TRY
			"" //PROX_AME
		};
	new
		Float:x,
		Float:y,
		Float:z,
		vw = GetPlayerVirtualWorld(playerid),
		int = GetPlayerInterior(playerid);
		
	GetPlayerPos(playerid, x, y, z);
	
	new beautyText[300];
	
	new logstr[256];

	switch( type )
	{
		case PROX_SHOUT:
		{
			format(logstr, sizeof(logstr), "[ic:shout] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, false, true));
		}
		
		case PROX_QUIET:
		{
			format(logstr, sizeof(logstr), "[ic:silent] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_ME:
		{
			format(logstr, sizeof(logstr), "[ic:rp:me] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			format(beautyText, sizeof(beautyText), "** %s %s", pInfo[playerid][player_name], BeautifyString(text, false, true));
		}
		
		case PROX_TRY:
		{
			format(beautyText, sizeof(beautyText), "*** %s %s", pInfo[playerid][player_name], BeautifyString(text, false, true));
		}
		
		case PROX_DO:
		{
			format(logstr, sizeof(logstr), "[ic:rp:do] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			format(beautyText, sizeof(beautyText), "** %s (( %s ))", BeautifyString(text, true, true), pInfo[playerid][player_name]);
		}
		
		case PROX_LOCAL_L:
		{
			format(logstr, sizeof(logstr), "[ic:say] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_OOC:
		{
			format(logstr, sizeof(logstr), "[ooc:say] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			format(beautyText, sizeof(beautyText), "(( [%d] %s: %s ))", playerid, pInfo[playerid][player_name], text);
		}
		
		case PROX_RADIO:
		{
			format(logstr, sizeof(logstr), "[ic:radio] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_LOCAL:
		{
			if( GetPlayerState(playerid) == PLAYER_STATE_ONFOOT )
			{
				if( !pInfo[playerid][player_looped_anim] && !pInfo[playerid][player_phone_call_started] )
				{
					new time = strlen(text) * 50;
					
					switch( pInfo[playerid][player_talk_style] )
					{
						case 0: ApplyAnimation(playerid, "PED", "IDLE_CHAT", 4.1, 1, 0, 0, 1, 50, 1);
						case 1: ApplyAnimation(playerid, "GHANDS", "gsign1", 4.1, 1, 0, 0, 1, 50, 1);
						case 2: ApplyAnimation(playerid, "GHANDS", "gsign2", 4.1, 1, 0, 0, 1, 50, 1);
						case 3: ApplyAnimation(playerid, "GHANDS", "gsign3", 4.1, 1, 0, 0, 1, 50, 1);
						case 4: ApplyAnimation(playerid, "GHANDS", "gsign4", 4.1, 1, 0, 0, 1, 50, 1);
						case 5: ApplyAnimation(playerid, "GHANDS", "gsign5", 4.1, 1, 0, 0, 1, 50, 1);
					}
					
					defer StopPlayerAnimation[time](playerid);
				}
			}
			
			format(logstr, sizeof(logstr), "[ic:say] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
		}
		
		case PROX_PHONE:
		{
			format(beautyText, sizeof(beautyText), "%s %s: %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, true));
			
			new targetid = -1;
			if( pInfo[playerid][player_phone_caller] == INVALID_PLAYER_ID ) targetid = pInfo[playerid][player_phone_receiver];
			else targetid = pInfo[playerid][player_phone_caller];
			
			format(logstr, sizeof(logstr), "[ic:phone] %s | talkingWith: %s", text, PlayerLogLink(pInfo[targetid][player_id]));
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);

			extended_SendClientMessage(targetid, COLOR_YELLOW, sprintf("%d (%s) %s", Item[itemid][item_value1], (pInfo[targetid][player_sex] == 0 ? ("kobieta") : ("mê¿czyzna")), BeautifyString(text, true, true)));
		}
		
		case PROX_MEGAFON:
		{
			format(logstr, sizeof(logstr), "[ic:megaphone] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");
			format(beautyText, sizeof(beautyText), "%s %s %s", pInfo[playerid][player_name], ProxConfigText[type], BeautifyString(text, true, false, false, true));
		}

		case PROX_AME:
		{
			format(logstr, sizeof(logstr), "[ic:rp:ame] %s", text);
			PlayerLog(logstr, pInfo[playerid][player_id], "chat");

			format(beautyText, sizeof(beautyText), "* %s %s", pInfo[playerid][player_name], BeautifyString(text, false, true));

			SetPlayerChatBubble(playerid, beautyText, 0xB58ADAFF, 10.0, 3000);
		}
	}
	
	foreach (new p : Player)
	{
		// Some checks
		if( !pInfo[p][player_logged] ) continue;
		if( type == PROX_RADIO && p == playerid ) continue;
		if( GetPlayerVirtualWorld(p) != vw || GetPlayerInterior(p) != int ) continue;
		if( type != PROX_ME && type != PROX_DO && pInfo[p][player_bw] > 0 ) continue;
		
		new Float:distance = GetPlayerDistanceFromPoint(p, x, y, z);
		if( distance > ProxConfigDistance[type][0] ) continue;

		// Color formatting
		new color;
		if( distance <= ProxConfigDistance[type][0] ) color = ProxConfigColor[type][0];
		if( distance <= ProxConfigDistance[type][1] ) color = ProxConfigColor[type][1];
		if( distance <= ProxConfigDistance[type][2] ) color = ProxConfigColor[type][2];
		if( distance <= ProxConfigDistance[type][3] ) color = ProxConfigColor[type][3];
		if( distance <= ProxConfigDistance[type][4] ) color = ProxConfigColor[type][4];
		
		if(type == PROX_LOCAL)
		{
			new pos[10] = {-1, ...}, pos2[10] = {-1, ...}, counter;

	    	strreplace(beautyText, ":D", "*œmieje siê*", true);
			strreplace(beautyText, ";D", "*œmieje siê*", true);
			strreplace(beautyText, ":P", "*wystawia jêzyk*", true);
			strreplace(beautyText, ";P", "*wystawia jêzyk*", true);
			strreplace(beautyText, ":)", "*uœmiecha siê*", true);
			strreplace(beautyText, ";)", "*puszcza oczko*", true);
			strreplace(beautyText, "C:", "*uœmiecha siê*", true);
			strreplace(beautyText, "C;", "*uœmiecha siê*", true);
			strreplace(beautyText, ";O", "*robi zdziwion¹ minê*", true);
			strreplace(beautyText, ":O", "*robi zdziwion¹ minê*", true);
			strreplace(beautyText, ";(", "*p³acze*", true);
			strreplace(beautyText, ":(", "*robi smutn¹ minê*", true);
			strreplace(beautyText, ":C", "*robi smutn¹ minê*", true);
			strreplace(beautyText, ";C", "*robi smutn¹ minê*", true);
			strreplace(beautyText, ":*", "*wysy³a buziaka*", true);
			strreplace(beautyText, ";*", "*wysy³a buziaka*", true);
			strreplace(beautyText, ":/", "*krzywi siê*", true);
			strreplace(beautyText, ";/", "*krzywi siê*", true);
			strreplace(beautyText, "XD", "*wybucha œmiechem*", true);

			new start_pos = -1;
			for(new i; i<strlen(beautyText); i++)
			{
				if(beautyText[i] == '*')
				{
					if(start_pos > -1)
					{
						if(i == start_pos+1) continue;

						pos2[counter] = start_pos;
						pos[counter] = i+2;
						counter++;
						start_pos = -1;
					}
					else
					{
						start_pos = i;
					}
				}
			}

			for(new i;i<10;i++)
			{
				if(pos2[i] == -1) break;
				strins(beautyText, "{B589DD}", pos2[i] + (i*8));
			}

			new colorhex[10];
			format(colorhex, sizeof(colorhex), "{%06x}", color >>> 8);

			for(new i = 0;i<10;i++)
			{
				if(pos[i] == -1) break;
				if(beautyText[pos[i] + 8 + i*16] == EOS || beautyText[pos[i] + 8 + i*16 + 1] == EOS) break;
	            strins(beautyText, colorhex, pos[i] + 8 + i*16);
			}
		}

		extended_SendClientMessage(p, color, beautyText);
	}
	
	return 0;
}

stock rand(v_min, v_max)
{
	new rndd = -1;
	while (rndd < v_min) rndd = random(v_max+1);
	
	return rndd;
}

stock GetMedicsOnline()
{
	new count = 0;

	foreach(new p : Player)
	{
		new slot = GetPlayerDutySlot(p);
		if(slot != -1)
		{
			if(Group[pGroup[p][slot][pg_id]][group_type] == GROUP_TYPE_EMS && !pInfo[p][player_afk]) count++;
		}
	}

	return count;
}

stock IsPlayerInPublicGroup(playerid)
{
	for(new i = 0; i < 5; i++)
	{
		if( pGroup[playerid][i][pg_id] > -1 )
		{
			if(Group[pGroup[playerid][i][pg_id]][group_type] == GROUP_TYPE_PD || Group[pGroup[playerid][i][pg_id]][group_type] == GROUP_TYPE_FBI) return 1;
		}
	}

	return 0;
}

stock SendGuiInformation(playerid, caption[], text[])
{
	return ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, caption, text, "OK", "");
}

stock IntToBase(number, const base) {
    new str[32];
    if(1 < base < 37) {
        new
            m = 1,
            depth = 0;
        while (m <= number) {
            m *= base;
            depth++;
        }
        for ( ; (--depth) != -1; ) {
            str[depth] = (number % base);
            number = ((number - str[depth]) / base);
            if(str[depth] > 9) str[depth] += 'A'; else str[depth] += '0';
        }
    }
    return str;
}
#define IntToDual(%0) IntToBase(%0, 2)
#define IntToOctal(%0) IntToBase(%0, 8)
#define IntToHex(%0) IntToBase(%0, 16)


new WeaponVisualModel[47] = {
	-1, -1, 333, 334, 335, 336, 337, 338, 339, 341, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 346, 347, 348, 349, 350, 351, 352, 353,
	355, 356, 372, 357, 358, -1, -1, -1, -1, -1, -1, 365, -1, -1, -1, -1, -1
};

new WeaponVisualBone[47] = {
	-1, -1, 1, 7, 7, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 8, 8, 8, 1, 8, 1, 8, 1,
	1, 1, 8, 1, 1, -1, -1, -1, -1, -1, -1, 8, -1, -1, -1, -1, -1
};

new Float:FWeaponVisualPos[47][9] = {
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.017000, -0.114999, -0.140000, 88.999992, 86.500015, -1.800061, 1.000000, 1.000000, 1.000000 },
	{ 0.059000, 0.070000, -0.049000, -22.500000, 90.099998, -43.800014, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.422000, -0.168000, 0.193000, -7.400008, -34.300003, 173.700027, 1.000000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ 0.159000, 0.146999, 0.164000, -13.199998, 150.000045, 25.100008, 1.000000, 1.059000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
};

new Float:WeaponVisualPos[47][9] = {
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.017000, -0.114999, -0.140000, 88.999992, 86.500015, -1.800061, 1.000000, 1.000000, 1.000000 },
	{ 0.059000, 0.070000, -0.049000, -22.500000, 90.099998, -43.800014, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.354000, -0.095000, -0.067000, 8.399996, -61.099990, -177.199996, 1.000000, 1.000000, 1.000000 },
	{ 0.422000, -0.168000, 0.193000, -7.400008, -34.300003, 173.700027, 1.000000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.153000, -0.092000, 0.074000, 155.699951, -8.700000, 4.100007, 0.790000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ -0.154000, -0.101999, -0.021999, 176.299926, 34.300022, 7.500000, 1.000000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ -0.067999, -0.059000, 0.131000, -102.499969, 0.000000, 0.400004, 0.790000, 1.000000, 1.000000 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
};