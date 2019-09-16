#define ACTION_TYPE_CAR_REPAIR 1

#define ACTION_MAX_DISTANCE_WARNS 10

stock IsPlayerPerformingAnyAction(playerid)
{
	if( !pAction[playerid][action_type] ) return false;

	return true;
}

stock CleanPlayerAction(playerid)
{
	for(new z=0; e_player_action:z != e_player_action; z++)
    {
		pAction[playerid][e_player_action:z] = 0;
    }
}

stock CancelPlayerAction(playerid)
{
	if( !pAction[playerid][action_type] ) return;

	new buyerid = GetPlayerByUid(pAction[playerid][action_buyer_uid]);

	// Cash chargeback
	if( pAction[playerid][action_buyer_uid] != 0 && pAction[playerid][action_price] > 0 )
	{
		if( pAction[playerid][action_payment_type] == PAYMENT_TYPE_CASH )
		{
			if( buyerid != INVALID_PLAYER_ID ) GivePlayerMoney(buyerid, pAction[playerid][action_price]);
			else mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_cash = char_cash + %d WHERE char_uid = %d", pAction[playerid][action_price], pAction[playerid][action_buyer_uid]));
		}
		else if( pAction[playerid][action_payment_type] == PAYMENT_TYPE_CARD )
		{
			if( buyerid != INVALID_PLAYER_ID ) AddPlayerBankMoney(buyerid, pAction[playerid][action_price], "Zwrot p³atnoœci");
			else mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_bankcash = char_bankcash + %d WHERE char_uid = %d", pAction[playerid][action_price], pAction[playerid][action_buyer_uid]));
		}
	}

	// Notify buyer
	if( buyerid != INVALID_PLAYER_ID )
	{
		SendClientMessage(buyerid, COLOR_LIGHTER_RED, sprintf("Akcja wykonywana na Twoje zlecenie przez %s zosta³a przerwana.", pInfo[playerid][player_name]));
	}

	OnActionCancel(playerid, pAction[playerid][action_type]);
	CleanPlayerAction(playerid);
}

stock FinishPlayerAction(playerid)
{
	switch(pAction[playerid][action_type])
	{
		case ACTION_TYPE_CAR_REPAIR:
		{
			if( pInfo[playerid][player_debit] > 0 ) {
				SendPlayerInformation(playerid, "Najpierw splac swoj dlug.", 4000);
				return;
			}
			new vid = pAction[playerid][action_extraid];

			if( pAction[playerid][action_payment_type] == PAYMENT_TYPE_CARD ) AddPlayerBankMoney(playerid, floatround(0.05 * pAction[playerid][action_price]), "P³atnoœæ kart¹");
			else if( pAction[playerid][action_payment_type] == PAYMENT_TYPE_CASH ) GivePlayerMoney(playerid, floatround(0.05 * pAction[playerid][action_price]));
			
			new gid = pAction[playerid][action_groupid];
			GiveGroupMoney(gid, floatround(0.15 * pAction[playerid][action_price]));
			GiveGroupPoints(gid, 3);
			
			Vehicle[vid][vehicle_state] = 0;
			
			UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_state_label], COLOR_PINK, "Naprawianie pojazdu\nZakoñczono");
			defer VehicleStateErase[3000](vid);
			
			SetVehicleHealth(vid, 1000.0);
			Vehicle[vid][vehicle_destroyed] = false; 
			
			Vehicle[vid][vehicle_damage][0] = 0;
			Vehicle[vid][vehicle_damage][1] = 0;
			Vehicle[vid][vehicle_damage][2] = 0;
			Vehicle[vid][vehicle_damage][3] = 0;
			
			UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);
			
			SaveVehicle(vid);
		}
	}

	CleanPlayerAction(playerid);
}

stock action_OnPlayerDisconnect(playerid, reason)
{
	#pragma unused reason
	CancelPlayerAction(playerid);
}

stock action_Process(playerid)
{
	if( pInfo[playerid][player_afk] ) return;
	if( !action_DistanceCheck(playerid) ) return;

	new reduce_time_left = false;

	switch(pAction[playerid][action_type])
	{
		case ACTION_TYPE_CAR_REPAIR:
		{
			reduce_time_left = action_ProcessCarRepair(playerid);
		}
	}

	if( reduce_time_left ) 
	{
		pAction[playerid][action_time_left]--;

		if( pAction[playerid][action_time_left] == 0 )
		{
			FinishPlayerAction(playerid);
		}
	}
}

stock action_DistanceCheck(playerid)
{
	if( !pAction[playerid][action_type] ) return true;
	if( !pAction[playerid][action_distance_check] ) return true;

	if( pAction[playerid][action_distance_warn_times] == ACTION_MAX_DISTANCE_WARNS ) 
	{
		SendClientMessage(playerid, COLOR_LIGHTER_RED, "Wykonywana przez Ciebie akcja zosta³a anulowana ze wzglêdu na oddalenie siê od miejsca jej wykonywania.");
		CancelPlayerAction(playerid);
		return false;
	}

	if( GetPlayerDistanceFromPoint(playerid, pAction[playerid][action_source_pos][0], pAction[playerid][action_source_pos][1], pAction[playerid][action_source_pos][2]) > pAction[playerid][action_max_distance] )
	{
		pAction[playerid][action_distance_warn_times]++;

		Alert(playerid, ALERT_TYPE_INFO, "Wróc do miejsca wykonywania akcji, w innym wypadku zostanie ona przerwana.");
		return false;
	}

	return true;
}

stock OnActionCancel(playerid, type)
{
	switch(type)
	{
		case ACTION_TYPE_CAR_REPAIR:
		{
			new vid = pAction[playerid][action_extraid];

			Vehicle[vid][vehicle_state] = 0;
		}
	}
}

stock action_ProcessCarRepair(playerid)
{
	new vid = pAction[playerid][action_extraid];

	new Float:as = floatdiv(pAction[playerid][action_time]-pAction[playerid][action_time_left], pAction[playerid][action_time]);
	new str[40];
	format(str, sizeof(str), "Naprawianie pojazdu\n%d%", floatround(as*100));
	
	UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_state_label], COLOR_PINK, str);

	return true;
}