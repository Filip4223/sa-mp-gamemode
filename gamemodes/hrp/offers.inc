stock SetOffer(sellerid, buyerid, type, price, extraid = -1)
{
	if( sellerid == buyerid && GetPlayerPermissions(sellerid, 5) )
	{
		SendGuiInformation(sellerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz wysy³aæ ofert samemu sobie.");
		return 0;
	}
	
	/*if( GetPlayerOnlineTime(buyerid) < 18000 && sellerid != INVALID_PLAYER_ID && price > 0 )
	{
		SendGuiInformation(sellerid, "Wyst¹pi³ b³¹d", "Ten gracz ma przegrane mniej ni¿ 5h.");
		return 0;
	}*/

	if( sellerid != INVALID_PLAYER_ID )
	{
		if( pOffer[sellerid][offer_type] > 0 )
		{
			SendGuiInformation(sellerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz sk³adaæ kilku ofert na raz, poczekaj do zakoñczenia poprzedniej oferty.");
			return 0;
		}
		
		/*if( GetPlayerOnlineTime(sellerid) < 18000 && price > 0 )
		{
			SendGuiInformation(sellerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz sk³adaæ ofert poniewa¿ masz przegrane mniej ni¿ 5h.");
			return 0;
		}*/
	}
	
	if( pOffer[buyerid][offer_type] > 0 )
	{
		SendGuiInformation(sellerid, "Wyst¹pi³ b³¹d", "Gracz, do którego chcia³eœ wys³aæ ofertê aktualnie otrzyma³ ju¿ jedn¹.");
		return 0;
	}
	
	if( pInfo[buyerid][player_afk] )
	{
		SendGuiInformation(sellerid, "Wyst¹pi³ b³¹d", "Gracz, do którego chcia³eœ wys³aæ ofertê aktualnie jest AFK.");
		return 0;
	}
	
	switch( type )
	{
		case OFFER_TYPE_VCARD:
		{
			new itemid = GetPlayerUsedItem(buyerid, ITEM_TYPE_PHONE);
			if( GetPlayerUsedItem(buyerid, ITEM_TYPE_PHONE) == -1 )
			{
				SendGuiInformation(sellerid, "Wyst¹pi³ b³¹d", "Gracz, do którego chcia³eœ wys³aæ ofertê nie posiada w³¹czonego telefonu.");
				return 0;
			}
			
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT contact_uid FROM `crp_contacts` WHERE `contact_owner` = %d AND `contact_number` = %d AND `contact_deleted` = 0", Item[itemid][item_uid], Item[extraid][item_value1]));
			
			new rows = cache_get_rows();

			cache_delete(result);

			if( rows > 0 )
			{
				SendGuiInformation(sellerid, "Wyst¹pi³ b³¹d", "Gracz, do którego chcia³eœ wys³aæ ofertê posiada ju¿ Twoj¹ wizytówkê w telefonie.");
				return 0;
			}
		}

		case OFFER_TYPE_VEH_SELL: 
		{
		 	InfoboxCenter(buyerid, 15, sprintf("~n~ ~y~UID: ~w~%d~n~ ~y~Model: ~w~ %d~n~ ~y~Rodzaj paliwa: ~w~%s~n~ ~y~Stan techniczny: ~w~%.1f HP~n~ ~y~Przebieg: ~w~%d km~n~~n~", Vehicle[extraid][vehicle_uid], Vehicle[extraid][vehicle_model], VehicleFuelTypes[Vehicle[extraid][vehicle_fuel_type]], Vehicle[extraid][vehicle_health], floatround(Vehicle[extraid][vehicle_mileage], floatround_floor)));
		}
	}
	
	pOffer[buyerid][offer_type] = type;
	pOffer[buyerid][offer_sellerid] = sellerid;
	pOffer[buyerid][offer_buyerid] = buyerid;
	pOffer[buyerid][offer_price] = price;
	pOffer[buyerid][offer_extraid] = extraid;
	pOffer[buyerid][offer_start] = gettime();
	
	if( sellerid != INVALID_PLAYER_ID )
	{
		pOffer[sellerid][offer_type] = type;
		pOffer[sellerid][offer_sellerid] = sellerid;
		pOffer[sellerid][offer_buyerid] = buyerid;
		pOffer[sellerid][offer_price] = price;
		pOffer[sellerid][offer_extraid] = extraid;
		
		pOffer[sellerid][offer_start] = gettime();
	}
	
	return 1;
}

stock ShowPlayerOffer(playerid, fromid, header[], name[], price, bool:show_details = false, bool:high_perm = false)
{
	new tmp[MAX_PLAYER_NAME+1];
	if( fromid != INVALID_PLAYER_ID )
	{
		strcopy(tmp, pInfo[fromid][player_name], MAX_PLAYER_NAME+1);
	}
	else strcopy(tmp, "System");

	HidePlayerDialog(playerid);
	
	if(pInfo[playerid][player_interface])
	{
		PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][1], tmp);
		PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][2], sprintf("%d", price));
		PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][3], name);

		switch(pOffer[playerid][offer_type])
		{
			case OFFER_TYPE_VCARD: 			PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:vcard");
			case OFFER_TYPE_DOCUMENT: 		PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:id");
			case OFFER_TYPE_LSN_REKLAMA: 	PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:mic");
			case OFFER_TYPE_LSN_WYWIAD: 	PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:mic");
			case OFFER_TYPE_PETROL:			PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:petrol");
			case OFFER_TYPE_ITEM: 			PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:thing");
			case OFFER_TYPE_ITEM_PODAJ:		PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:thing");
			case OFFER_TYPE_SALON_VEH: 		PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:veh");
			case OFFER_TYPE_EMS_LECZENIE: 	PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:amb");
			case OFFER_TYPE_YO: 			PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:yo");
			case OFFER_TYPE_NAPRAWA: 		PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:repair");
			case OFFER_TYPE_VEH_SELL:		PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:veh");
			case OFFER_TYPE_MALOWANIE: 		PlayerTextDrawSetString(playerid, pInfo[playerid][OfferTD][4], "mdl-2009:spray");
		}

		defer SetOfferTextDrawColor[2](playerid, -255);
	}
	else
	{
		new str[350];
		format(str, sizeof(str), HEX_COLOR_LIGHTER_YELLOW"%s%s : %s\n\n"HEX_COLOR_SAMP"Koszt: $%d. Czy akceptujesz tê ofertê?\n", str, header, name, price);
		if(high_perm)
		{
			strcat(str, HEX_COLOR_LIGHTER_RED"\tUWAGA! Ta transakcja wymaga podwy¿szonych uprawnieñ.\n");
			strcat(str, HEX_COLOR_LIGHTER_RED"\tWpisz \x22;potwierdzam\x22; w poni¿szym okienku, inaczej zostanie anulowana.\n");
			strcat(str, HEX_COLOR_LIGHTER_RED"\tUPEWNIJ SIÊ, ¿e wiesz co akceptujesz!\n");
			ShowPlayerDialog(playerid, DIALOG_OFFER_HIGH, DIALOG_STYLE_INPUT, sprintf("Oferta od %s:", tmp), str, "Tak", "Nie");
		}
		else ShowPlayerDialog(playerid, DIALOG_OFFER, DIALOG_STYLE_MSGBOX, sprintf("Oferta od %s:", tmp), str, "Tak", "Nie");
	}


	if( fromid != INVALID_PLAYER_ID )
	{
		// From log
		PlayerLog(sprintf("Made an offer %s for $%d to %s (%s: %s)", OfferTypeLogLink(pOffer[fromid][offer_type]), price, PlayerLogLink(pInfo[playerid][player_id]), header, name), pInfo[fromid][player_id], "offer");

		GameTextForPlayer(fromid, "~y~oferta wyslana", 3000, 3);
	}
	
	PlayerLog(sprintf("Received an offer %s for $%d from %s (%s: %s)", OfferTypeLogLink(pOffer[playerid][offer_type]), price, PlayerLogLink(fromid == INVALID_PLAYER_ID ? -1 : pInfo[fromid][player_id]), header, name), pInfo[playerid][player_id], "offer");
	return 1;
}

stock OnPlayerOfferResponse(playerid, response)
{
	for(new i;i<6;i++) PlayerTextDrawHide(playerid, pInfo[playerid][OfferTD][i]);
	
	new sellerid = pOffer[playerid][offer_sellerid];
	
	if( !response )
	{
		if( sellerid != INVALID_PLAYER_ID ) GameTextForPlayer(sellerid, "~w~oferta ~r~odrzucona", 3000, 3);
		if( playerid != INVALID_PLAYER_ID ) GameTextForPlayer(playerid, "~w~oferta ~r~odrzucona", 3000, 3);
		
		OnPlayerOfferRejected(playerid, pOffer[playerid][offer_type]);
		
		for(new x=0; e_player_offer:x != e_player_offer; x++)
		{
			pOffer[playerid][e_player_offer:x] = 0;
			if( sellerid != INVALID_PLAYER_ID ) pOffer[sellerid][e_player_offer:x] = 0;
		}
	}
	else
	{
		pOffer[playerid][offer_accepted] = true;
		if( pOffer[playerid][offer_price] == 0 ) OnPlayerOfferFinish(playerid, 0, 1);
		else
		{
			ShowPlayerPayment(playerid);
		}
	}
	return 1;
}

stock OnPlayerOfferRejected(buyerid, type)
{
	new sellerid = pOffer[buyerid][offer_sellerid];

	if(sellerid != INVALID_PLAYER_ID && buyerid != INVALID_PLAYER_ID)
	{
		// From log
		PlayerLog(sprintf("Offer made to %s was rejected", PlayerLogLink(pInfo[buyerid][player_id])), pInfo[sellerid][player_id], "offer");

		// To log
		PlayerLog(sprintf("Rejected offer from %s", PlayerLogLink(pInfo[sellerid][player_id])), pInfo[buyerid][player_id], "offer");
	}

	switch( type )
	{
		case OFFER_TYPE_VEH_SELL:
		{
			new vid = pOffer[buyerid][offer_extraid];
			Vehicle[vid][vehicle_state] = 0;
		}
	}
}

stock OnPlayerOfferFinish(playerid, type, response)
{
	new sellerid = pOffer[playerid][offer_sellerid];
	
	if( !response )
	{
		if( sellerid != INVALID_PLAYER_ID ) GameTextForPlayer(sellerid, "~w~oferta ~r~odrzucona", 3000, 3);
		if( playerid != INVALID_PLAYER_ID ) GameTextForPlayer(playerid, "~w~oferta ~r~odrzucona", 3000, 3);

		OnPlayerOfferRejected(playerid, pOffer[playerid][offer_type]);
	}
	else
	{
		if( sellerid != INVALID_PLAYER_ID ) 
		{			
			// From log
			PlayerLog(sprintf("Offer made to %s was accepted", PlayerLogLink(pInfo[playerid][player_id])), pInfo[sellerid][player_id], "offer");

			GameTextForPlayer(sellerid, "~w~oferta ~g~zaakceptowana", 3000, 3);
		}

		PlayerLog(sprintf("Accepted offer from %s", PlayerLogLink(sellerid == INVALID_PLAYER_ID ? -1 : pInfo[sellerid][player_id])), pInfo[playerid][player_id], "offer");
		GameTextForPlayer(playerid, "~w~oferta ~g~zaakceptowana", 3000, 3);

		switch( pOffer[playerid][offer_type] )
		{
			case OFFER_TYPE_VCARD:
			{
				new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
				if( itemid > -1 )
				{
					mysql_pquery(g_sql, sprintf("INSERT INTO `crp_contacts` VALUES (null, %d, '%s', %d, 0)", Item[pOffer[playerid][offer_extraid]][item_value1], pInfo[sellerid][player_name], Item[itemid][item_uid]));
					ProxMessage(sellerid, sprintf("wys³a³ wizytówkê do %s", pInfo[playerid][player_name]), PROX_AME);
				}	
			}
			
			case OFFER_TYPE_DOCUMENT:
			{
				pInfo[playerid][player_documents] += pOffer[playerid][offer_extraid];
				mysql_pquery(g_sql, sprintf("UPDATE `crp_characters` SET `char_documents` = %d WHERE `char_uid` = %d", pInfo[playerid][player_documents], pInfo[playerid][player_id]));
				
				ProxMessage(playerid, "odbiera od urzêdnika dokument.", PROX_AME);
			}
			
			case OFFER_TYPE_VEH_SELL:
			{
				if( type == 1 ) AddPlayerBankMoney(sellerid, pOffer[playerid][offer_price], "Sprzeda¿ pojazdu");
				else GivePlayerMoney(sellerid, pOffer[playerid][offer_price]);
				
				new vid = pOffer[playerid][offer_extraid];
				mysql_pquery(g_sql, sprintf("UPDATE `crp_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_PLAYER, pInfo[playerid][player_id], Vehicle[vid][vehicle_uid]));
				Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_PLAYER;
				Vehicle[vid][vehicle_owner] = pInfo[playerid][player_id];
				Vehicle[vid][vehicle_locked] = false;
				Vehicle[vid][vehicle_engine] = false;
				UpdateVehicleVisuals(vid);
				
				foreach(new p : Player)
				{
					if( GetPlayerVehicleID(p) == vid ) RemovePlayerFromVehicle(p);
				}

				SendGuiInformation(playerid, "Informacja", sprintf("Zakupi³eœ pojazd %s. Jest on teraz dostêpny pod /v. Nie zapomnij go jak najszybciej zaparkowaæ!", VehicleNames[Vehicle[vid][vehicle_model]-400]));
			}
			
			case OFFER_TYPE_LSN_REKLAMA:
			{
				GiveGroupMoney(pOffer[playerid][offer_extraid], pOffer[playerid][offer_price]);
				GiveGroupPoints(pOffer[playerid][offer_extraid], 3);
			}
			
			case OFFER_TYPE_LSN_WYWIAD:
			{
				pInfo[playerid][player_lsn_wywiad_starter] = sellerid;
				pInfo[playerid][player_lsn_wywiad_with] = INVALID_PLAYER_ID;
				
				pInfo[sellerid][player_lsn_wywiad_starter] = INVALID_PLAYER_ID;
				pInfo[sellerid][player_lsn_wywiad_with] = playerid;
			}
			
			case OFFER_TYPE_PETROL:
			{
				new vid = GetPlayerVehicleID(playerid);
				
				Vehicle[vid][vehicle_state] = VEHICLE_STATE_FUELING;
				Vehicle[vid][vehicle_state_end] = gettime() + floatround(1.5 * pOffer[playerid][offer_extraid], floatround_ceil);
				Vehicle[vid][vehicle_state_time] = Vehicle[vid][vehicle_state_end] - gettime();
				UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_state_label], COLOR_PINK, "Tankowanie pojazdu\n0%");
				
				Vehicle[vid][vehicle_fuel_current] = Vehicle[vid][vehicle_fuel_current] + float(pOffer[playerid][offer_extraid]);
				
				if( Vehicle[vid][vehicle_fuel_current] > float(VehicleFuelMax[GetVehicleModel(vid)-400]) ) Vehicle[vid][vehicle_fuel_current] = VehicleFuelMax[GetVehicleModel(vid)-400];
				
				SaveVehicle(vid);
			}
			
			case OFFER_TYPE_ITEM:
			{
				new itemid = pOffer[playerid][offer_extraid];
				
				if( Item[itemid][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[itemid][item_owner] != pInfo[sellerid][player_id] || Item[itemid][item_used] )
				{
					if( type == 1 ) AddPlayerBankMoney(playerid, pOffer[playerid][offer_price], "Zwrot p³atnoœci");
					else GivePlayerMoney(playerid, pOffer[playerid][offer_price]);
					
					SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz, który zaoferowa³ Ci ten przedmiot ju¿ go nie posiada.");
				}
				else
				{
					if( pInfo[playerid][player_debit] > 0 ) {
						SendPlayerInformation(playerid, "Najpierw splac swoj dlug.", 4000);
					}
					else {
						if( type == 1 ) AddPlayerBankMoney(sellerid, pOffer[playerid][offer_price], "Kupno przedmiotu");
						else GivePlayerMoney(sellerid, pOffer[playerid][offer_price]);
						
						Item[itemid][item_owner] = pInfo[playerid][player_id];
						mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_owner = %d WHERE item_uid = %d", Item[itemid][item_owner], Item[itemid][item_uid]));
						
						HidePlayerDialog(sellerid);
					}
				}
			}
			
			case OFFER_TYPE_ITEM_PODAJ:
			{
				new itemid = pOffer[playerid][offer_extraid];
				if( Item[itemid][item_uid] <= 0 || Item[itemid][item_amount] <= 0 )
				{
					if( type == 1 ) AddPlayerBankMoney(playerid, pOffer[playerid][offer_price], "Zwrot p³atnoœci");
					else GivePlayerMoney(playerid, pOffer[playerid][offer_price]);
					
					SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Przedmiot, który zosta³ Ci oferowany nie istnieje.");
				}
				else
				{
					new gid = GetGroupByUid(Door[GetDoorByUid(Item[itemid][item_owner])][door_owner]);
					GiveGroupPoints(gid, 3);
					GiveGroupMoney(gid, pOffer[playerid][offer_price]);
					
					new value1 = Item[itemid][item_value1];
					
					if( Item[itemid][item_type] == ITEM_TYPE_GYM_PASS ) value1 = Group[gid][group_uid];
					
					Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, Item[itemid][item_type], value1, Item[itemid][item_value2], Item[itemid][item_name], Item[itemid][item_model]);
					Item[itemid][item_amount] -= 1;
					
					if( Item[itemid][item_amount] > 0 ) mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_amount = %d WHERE item_uid = %d", Item[itemid][item_amount], Item[itemid][item_uid]));
					else DeleteItem(itemid, true);
				}
				
			}
			
			case OFFER_TYPE_SALON_VEH:
			{
				new model = pOffer[playerid][offer_extraid];
				
				pInfo[playerid][player_dialog_tmp1] = model;

				if( GetVehicleType(INVALID_VEHICLE_ID, model) == VEHICLE_TYPE_BIKE || GetVehicleType(INVALID_VEHICLE_ID, model) == VEHICLE_TYPE_MOTORBIKE || GetVehicleType(INVALID_VEHICLE_ID, model) == VEHICLE_TYPE_TRAILER ) 
				{
					if( GetVehicleType(INVALID_VEHICLE_ID, model) == VEHICLE_TYPE_BIKE || GetVehicleType(INVALID_VEHICLE_ID, model) == VEHICLE_TYPE_TRAILER ) pInfo[playerid][player_dialog_tmp2] = -1;
					else pInfo[playerid][player_dialog_tmp2] = 1;
					
					ShowPlayerDialog(playerid, DIALOG_CARS_SHOP_COLOR, DIALOG_STYLE_INPUT, "Wybór koloru pojazdu", "W poni¿sze pole wpisz kolor pojazdu w formacie kolor1:kolor2, np. 24:35.", "Wybierz", "Zamknij");
				}
				else ShowPlayerDialog(playerid, DIALOG_CARS_SHOP_FUELTYPE, DIALOG_STYLE_LIST, "Wybór rodzaju paliwa pojazdu", "Benzyna\nGaz\nDiesel", "Wybierz", "Zamknij");
			}
			
			case OFFER_TYPE_EMS_LECZENIE:
			{
				new gid = pOffer[playerid][offer_extraid];
				GiveGroupPoints(gid, 3);
				GiveGroupMoney(gid, pOffer[playerid][offer_price]);
				
				SetPlayerHealth(playerid, 100);
			}
			
			case OFFER_TYPE_YO:
			{
				new Float:gpos[4];
				GetPointInAngleOfPlayer(sellerid, gpos[0], gpos[1], gpos[2], 1.0, 0.0);
				
				SetPlayerPos(playerid, gpos[0], gpos[1], gpos[2]);
				SetPlayerFacingPlayer(sellerid, playerid);
				
				new animname[32];
				
				switch( pOffer[playerid][offer_extraid] )
				{
					case 1: strcopy(animname, "hndshkaa");
					case 2: strcopy(animname, "hndshkba");
					case 3: strcopy(animname, "hndshkca");
					case 4: strcopy(animname, "hndshkcb");
					case 5: strcopy(animname, "hndshkda");
					case 6: strcopy(animname, "hndshkea");
					case 7: strcopy(animname, "hndshkfa");
					case 8: strcopy(animname, "hndshkfa_swt");
				}
				
				ApplyAnimation(playerid, "GANGS", animname, 4.0, 0, 0, 0, 0, 0);
				ApplyAnimation(sellerid, "GANGS", animname, 4.0, 0, 0, 0, 0, 0);
			}
			
			case OFFER_TYPE_NAPRAWA:
			{
				new vid = GetVehicleByUid(pInfo[playerid][player_repair_vuid]);

				if( vid == INVALID_VEHICLE_ID )
				{
					if( type == 1 ) AddPlayerBankMoney(playerid, pOffer[playerid][offer_price], "Zwrot p³atnoœci");
					else GivePlayerMoney(playerid, pOffer[playerid][offer_price]);
					
					SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Pojazd, który mia³ byæ naprawiany zosta³ odspawnowany.");
				}
				else
				{
					pAction[sellerid][action_type] = ACTION_TYPE_CAR_REPAIR;
					pAction[sellerid][action_buyer_uid] = pInfo[playerid][player_id];
					pAction[sellerid][action_extraid] = vid;
					pAction[sellerid][action_price] = pOffer[playerid][offer_price];
					pAction[sellerid][action_payment_type] = type;
					pAction[sellerid][action_groupid] = pOffer[playerid][offer_extraid];

					pAction[sellerid][action_distance_check] = true;
					pAction[sellerid][action_max_distance] = 15.0;
					GetVehiclePos(vid, pAction[sellerid][action_source_pos][0], pAction[sellerid][action_source_pos][1], pAction[sellerid][action_source_pos][2]);
					
					pAction[sellerid][action_time] = 30 + floatround((1000.0 - Vehicle[vid][vehicle_health]) / 3);
					pAction[sellerid][action_time_left] = pAction[sellerid][action_time];

					Vehicle[vid][vehicle_state] = VEHICLE_STATE_REPAIRING;
					
					UpdateDynamic3DTextLabelText(Vehicle[vid][vehicle_state_label], COLOR_PINK, "Naprawianie pojazdu\n0%");
					
					SendGuiInformation(sellerid, "Informacja", "Rozpoczêto proces naprawiania pojazdu.\nW czasie jego trwania powinieneœ odgrywaæ akcjê role play za pomoc¹ /me, /do oraz animacji. Inne zachowania bêd¹ karane.");
				}
			}

			case OFFER_TYPE_MALOWANIE:
			{
				if( pInfo[playerid][player_debit] > 0 ) SendPlayerInformation(playerid, "Najpierw splac swoj dlug.", 4000);
				else
				{
					if( type == 1 ) AddPlayerBankMoney(playerid, -pOffer[playerid][offer_price], "Malowanie pojazdu");
					else GivePlayerMoney(playerid, -pOffer[playerid][offer_price]);

					GiveGroupMoney(pOffer[sellerid][offer_extraid], pOffer[playerid][offer_price]);


					SendClientMessage(sellerid, COLOR_GOLD, "Oferta malowania pojazdu zosta³a zaakceptowana. Teraz zacznij malowaæ pojazd, a¿ wskaŸnik postêpu nad pojazdem osi¹gnie 100%.");
					SendClientMessage(playerid, COLOR_GOLD, "Twój pojazd bêdzie teraz przemalowywany. Postêp mo¿esz œledziæ patrz¹c na wskaŸnik nad pojazdem.");
					new vid = GetVehicleByUid(pInfo[playerid][player_carpaint_vuid]);

					Vehicle[vid][vehicle_state] = VEHICLE_STATE_PAINT;
					Vehicle[vid][vehicle_state_time] = 2000;
					Vehicle[vid][vehicle_state_end] = gettime() + Vehicle[vid][vehicle_state_time];
					Vehicle[vid][vehicle_color][0] = pInfo[playerid][player_choosen_carcolor][0];
					Vehicle[vid][vehicle_color][1] = pInfo[playerid][player_choosen_carcolor][1];
					Vehicle[vid][vehicle_carpaint_seller] = pInfo[sellerid][player_id];
					Vehicle[vid][vehicle_carpaint_buyer] = pInfo[playerid][player_id];
				}
			}
		}
	}
	
	for(new x=0; e_player_offer:x != e_player_offer; x++)
	{
		pOffer[playerid][e_player_offer:x] = 0;
		if( sellerid > -1 && sellerid != INVALID_PLAYER_ID ) pOffer[sellerid][e_player_offer:x] = 0;
	}
}