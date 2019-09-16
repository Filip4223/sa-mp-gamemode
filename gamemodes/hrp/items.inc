stock LoadItem(limit[] = "", bool:return_id = false)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_items` %s", limit));  
	
	new i_id = -1, rows = cache_get_rows();
	
	for(new i;i<rows;i++)
	{
		i_id = Iter_Free(Items);
		Iter_Add(Items, i_id);
		
		Item[i_id][item_uid] = cache_get_int(i, "item_uid");
		Item[i_id][item_type] = cache_get_int(i, "item_type");
		Item[i_id][item_value1] = cache_get_int(i, "item_value1");
		Item[i_id][item_value2] = cache_get_int(i, "item_value2");
		Item[i_id][item_value3] = cache_get_int(i, "item_value3");
		Item[i_id][item_used] = cache_get_int(i, "item_used");
		Item[i_id][item_created] = cache_get_int(i, "item_created");
		Item[i_id][item_favorite] = cache_get_bool(i, "item_favorite");

		Item[i_id][item_owner_type] = cache_get_int(i, "item_ownertype");
		Item[i_id][item_owner] = cache_get_int(i, "item_owner");
		Item[i_id][item_group] = cache_get_int(i, "item_group");
		
		Item[i_id][item_price] = cache_get_int(i, "item_price");
		Item[i_id][item_amount] = cache_get_int(i, "item_amount");
		
		Item[i_id][item_model] = cache_get_int(i, "item_model");
		Item[i_id][item_x] = cache_get_float(i, "item_posx");
		Item[i_id][item_y] = cache_get_float(i, "item_posy");
		Item[i_id][item_z] = cache_get_float(i, "item_posz");
		Item[i_id][item_rx] = cache_get_float(i, "item_rotx");
		Item[i_id][item_ry] = cache_get_float(i, "item_roty");
		Item[i_id][item_rz] = cache_get_float(i, "item_rotz");
		Item[i_id][item_world] = cache_get_int(i, "item_world");
		Item[i_id][item_interior] = cache_get_int(i, "item_interior");
		
		cache_get(i, "item_name", Item[i_id][item_name]);

		if( Item[i_id][item_owner_type] == ITEM_OWNER_TYPE_GROUND ) Item_Drop(i_id);
	}
	
	cache_delete(result);

	if( return_id ) return i_id;
	return rows;
}

stock LoadPlayerBags(playerid) 
{
	foreach(new i : Items) {
		if(Item[i][item_type] == ITEM_TYPE_BAG && Item[i][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[i][item_owner] == pInfo[playerid][player_id]) {
			LoadItem(sprintf("WHERE `item_ownertype` = 9 AND `item_owner` = %d", Item[i][item_uid], pInfo[playerid][player_id]));
		}
	}
}

stock DeleteItem(itemid, bool:from_db = false)
{
	if( IsValidDynamicObject(Item[itemid][item_object]) ) DestroyDynamicObject(Item[itemid][item_object]);
	
	Iter_Remove(Items, itemid);
	
    if( from_db ) mysql_pquery(g_sql, sprintf("DELETE FROM `crp_items` WHERE `item_uid` = %d", Item[itemid][item_uid]));
    
	for(new z=0; e_items:z != e_items; z++)
    {
		Item[itemid][e_items:z] = 0;
    }
}

stock Item_Use(itemid, playerid = INVALID_PLAYER_ID)
{
	if( playerid != INVALID_PLAYER_ID && (Item[itemid][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[itemid][item_owner] != pInfo[playerid][player_id]) ) return 1;
	
	if(Item[itemid][item_selected]) return Alert(playerid, ALERT_TYPE_NEGATIVE, "Nie mozesz uzyc przedmiotu ktory aktualnie jest w zaznaczeniu.");

	PlayerLog(sprintf("Uses thing %s {V:{%d,%d,%d},TYPE:%d,USED:%d}", ItemLogLink(Item[itemid][item_uid]), Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_value3], Item[itemid][item_type], Item[itemid][item_used]), pInfo[playerid][player_id], "item");

	switch( Item[itemid][item_type] )
	{
		case ITEM_TYPE_WEAPON:
		{
			if( Item[itemid][item_used] )
			{
				new wslot = GetWeaponSlot(Item[itemid][item_value1]);
				Item[itemid][item_value2] = pWeapon[playerid][wslot][pw_ammo];

				// zapisywanie granatów
				if(wslot == 8) {
					new wpid, wpammo;
					GetPlayerWeaponData(playerid, wslot, wpid, wpammo);
					if(wpammo < Item[itemid][item_value2])
					{
						Item[itemid][item_value2] = wpammo;
					}
				}
				
				Item[itemid][item_used] = false;
				
				if( pWeapon[playerid][wslot][pw_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pWeapon[playerid][wslot][pw_object_index]);
				
				pWeapon[playerid][wslot][pw_itemid] = -1;
				pWeapon[playerid][wslot][pw_id] = -1;
				pWeapon[playerid][wslot][pw_ammo] = -1;
				pWeapon[playerid][wslot][pw_object_index] = -1;
				
				ResetPlayerWeapons(playerid);
				
				for(new i;i<13;i++)
				{
					if( pWeapon[playerid][i][pw_itemid] > -1 ) GivePlayerWeapon(playerid, pWeapon[playerid][i][pw_id], pWeapon[playerid][i][pw_ammo]);
				}
				
				pInfo[playerid][player_used_weapon] = -1;

				mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_value2` = %d WHERE `item_uid` = %d", Item[itemid][item_value2], Item[itemid][item_uid]));
			}
			else
			{
				new wslot = GetWeaponSlot(Item[itemid][item_value1]);
				
				// Sprawdzamy czy jest amunicja
				if( Item[itemid][item_value2] == 0 && GetWeaponType(Item[itemid][item_value1]) != WEAPON_TYPE_MELEE ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tej broni nie ma amunicji.");
				
				// Sprawdzamy czy czasem nie ma juz broni tego samego typu
				new wtype = GetWeaponType(Item[itemid][item_value1]);
				for(new i;i<13;i++)
				{
					if( pWeapon[playerid][i][pw_itemid] > -1 && GetWeaponType(pWeapon[playerid][i][pw_id]) == wtype ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Masz ju¿ wyci¹gniêt¹ inn¹ broñ tego samego typu.");
				}
				
				// Sprawdzamy czy czasem bron nie jest oflagowana
				if( Item[itemid][item_group] > 0 )
				{
					new gid = GetGroupByUid(Item[itemid][item_group]);
					if( GetPlayerGroupSlot(playerid, gid) == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do u¿ywania tej broni.");
				}
				
				pWeapon[playerid][wslot][pw_itemid] = itemid;
				pWeapon[playerid][wslot][pw_id] = Item[itemid][item_value1];
				pWeapon[playerid][wslot][pw_ammo] = Item[itemid][item_value2];
				GivePlayerWeapon(playerid, Item[itemid][item_value1], Item[itemid][item_value2]);
				
				Item[itemid][item_used] = true;

				pInfo[playerid][player_used_weapon] = itemid;
			}
		}
		
		case ITEM_TYPE_GLOVES:
		{
			if( Item[itemid][item_used] )
			{
				Item[itemid][item_used] = false;
				RemovePlayerStatus(playerid, PLAYER_STATUS_GLOVES);
				
				if( Item[itemid][item_value1] == 0 ) DeleteItem(itemid, true);
			}
			else
			{
				if( Item[itemid][item_value1] == 0 )
				{
					DeleteItem(itemid, true);
					return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz u¿yæ tego przedmiotu.");
				}
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_GLOVES) > -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Masz ju¿ u¿yty inny przedmiot tego typu.");
			
				AddPlayerStatus(playerid, PLAYER_STATUS_GLOVES);
				
				// Zabieramy jedno uzycie rekawiczek jesli nie jest nieskonczona (dla org. porzadkowych)
				if( Item[itemid][item_value1] != -1 )
				{
					Item[itemid][item_value1] -= 1;
					mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_value1` = %d WHERE `item_uid` = %d", Item[itemid][item_value1], Item[itemid][item_uid]));
				}
			}
		}
		
		case ITEM_TYPE_MASK:
		{
			if( Item[itemid][item_used] )
			{
				strcopy(pInfo[playerid][player_name], pInfo[playerid][player_duty_tmp], MAX_PLAYER_NAME+1);
				Item[itemid][item_used] = false;
				UpdatePlayerLabel(playerid);
				
				if( Item[itemid][item_value1] == 0 ) DeleteItem(itemid, true);
			}
			else
			{
				if( pInfo[playerid][player_admin_duty] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Podczas s³u¿by admina nie mo¿esz u¿ywaæ tego przedmiotu.");
				if( Item[itemid][item_value1] == 0 )
				{
					DeleteItem(itemid, true);
					return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz u¿yæ tego przedmiotu.");
				}
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASK) > -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Masz ju¿ u¿yty inny przedmiot tego typu.");
				
				new str[6];
				format(str, sizeof(str), "%d", pInfo[playerid][player_id]);
				strcopy(str, MD5_Hash(str), 6);
				strcopy(pInfo[playerid][player_duty_tmp], pInfo[playerid][player_name], MAX_PLAYER_NAME+1);
				strtoupper(str);
				
				Item[itemid][item_used] = true;
				
				format(pInfo[playerid][player_name], MAX_PLAYER_NAME+1, "Nieznajomy %s", str);
				UpdatePlayerLabel(playerid);
				
				// Zabieramy jedno uzycie maski jesli nie jest nieskonczona (dla org. porzadkowych)
				if( Item[itemid][item_value1] != -1 )
				{
					Item[itemid][item_value1] -= 1;
					mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_value1` = %d WHERE `item_uid` = %d", Item[itemid][item_value1], Item[itemid][item_uid]));
				}
			}
		}
		
		case ITEM_TYPE_AMMO:
		{
			if( !Item[itemid][item_used] )
			{
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
				new count, string[200];
				foreach(new item : Items)
				{
					if( Item[item][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[item][item_owner] == pInfo[playerid][player_id] )
					{
						if( Item[item][item_type] == ITEM_TYPE_WEAPON && Item[item][item_value1] == Item[itemid][item_value1] && !Item[item][item_used] )
						{
							format(string, sizeof(string), "%s%d\t\t%s\n", string, Item[item][item_uid], Item[item][item_name]);
							DynamicGui_AddRow(playerid, item);
							count++;
						}
					}
				}
				
				if( count == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz ¿adnych nieu¿ywanych broni do których pasuje ta amunicja.");
				else ShowPlayerDialog(playerid, DIALOG_USE_AMMO, DIALOG_STYLE_LIST, "Wybierz broñ do której za³adujesz amunicjê:", string, "Wybierz", "Zamknij");
			}
		}
		
		case ITEM_TYPE_PHONE:
		{
			if( Item[itemid][item_used] )
			{
				cmd_tel(playerid, "");
			}
			else
			{
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE) > -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Masz ju¿ w³¹czony inny telefon.");
			
				if( Item[itemid][item_value1] == 0 )
				{
					new str[7];
					format(str, sizeof(str), "%04d", pInfo[playerid][player_id]);
					
					new length = 6 - strlen(str);
					
					if( length == 2 ) format(str, sizeof(str), "%02d%s", rand(10, 99), str);
					else if( length == 1 ) format(str, sizeof(str), "%d%s", rand(1, 9), str);
					
					Item[itemid][item_value1] = strval(str);
					
					mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_value1` = %d WHERE `item_uid` = %d", Item[itemid][item_value1], Item[itemid][item_uid]));
				}
				
				Item[itemid][item_used] = true;
				
				mysql_pquery(g_sql, sprintf("UPDATE `crp_items` SET `item_used` = 1 WHERE `item_uid` = %d", Item[itemid][item_uid]));
				
				GameTextForPlayer(playerid, "~w~Telefon ~g~wlaczony", 3000, 3);
			}
		}
		
		case ITEM_TYPE_NOTEPAD:
		{
			pInfo[playerid][player_dialog_tmp1] = itemid;
			ShowPlayerDialog(playerid, DIALOG_NOTEPAD, DIALOG_STYLE_INPUT, "Tworzenie notatki", "W poni¿szym polu podaj treœc notatki:", "Gotowe", "Anuluj");
		}
		
		case ITEM_TYPE_CHIT:
		{
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT chit_text FROM `crp_chits` WHERE `chit_uid` = %d", Item[itemid][item_value1]));
			
			if( cache_get_rows() == 1 )
			{
				new text[200], date[30];
				cache_get(0, "chit_text", text);
				
				GetFormattedDate(Item[itemid][item_created], date);
				
				SendGuiInformation(playerid, "Treœæ karteczki", sprintf("%s\n\nData utworzenia: %s", text, date));
			}
			else
			{
				DeleteItem(itemid, true);
			}

			cache_delete(result);
		}
		
		case ITEM_TYPE_CLOTH:
		{
			if( Item[itemid][item_used] )
			{
				Item[itemid][item_used] = false;
				mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_used = 0 WHERE item_uid = %d", Item[itemid][item_uid]));
				
				pInfo[playerid][player_last_skin] = pInfo[playerid][player_skin];
				SetPlayerSkin(playerid, pInfo[playerid][player_last_skin]);
			}
			else
			{
				new iid = GetPlayerUsedItem(playerid, ITEM_TYPE_CLOTH);
				if( iid > -1 )
				{
					Item[iid][item_used] = false;
					mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_used = 0 WHERE item_uid = %d", Item[iid][item_uid]));
				}
				
				Item[itemid][item_used] = true;
				mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_used = 1 WHERE item_uid = %d", Item[itemid][item_uid]));
				
				pInfo[playerid][player_last_skin] = Item[itemid][item_value1];
				SetPlayerSkin(playerid, pInfo[playerid][player_last_skin]);
			}
		}
		
		case ITEM_TYPE_FOOD, ITEM_TYPE_DRINK:
		{			
			if( Item[itemid][item_value1] > 0 )
			{
				if( pInfo[playerid][player_health] < 30 ) return SendGuiInformation(playerid, "Informacja", "Stan Twojego zdrowia wymaga za¿ycia odpowiednich lekarstw aby przywróciæ pe³n¹ sprawnoœæ.\nDopóki nie zastosujesz odpowiednich leków to poziom Twojego zdrowia nie wzroœnie.");
				new waznosc = gettime() - Item[itemid][item_created];
				if( waznosc > 60*60 )
				{
					DeleteItem(itemid);
					SendGuiInformation(playerid, "Informacja", "Ten produkt jest ju¿ przeterminowany i nie mo¿esz go spo¿yæ wiêc wyrzucasz go do kosza.");
					return 1;
				}
				if( pInfo[playerid][player_taken_damage] > 0 && gettime() - pInfo[playerid][player_taken_damage] < 180 ) return SendGuiInformation(playerid, "Informacja", "Nie mo¿esz spo¿ywaæ jedzenia do 3 minut po otrzymaniu obra¿eñ.");
				
				pInfo[playerid][player_health] += Item[itemid][item_value1];
				if( pInfo[playerid][player_health] > 100 ) pInfo[playerid][player_health] = 100;
				
				SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
			}
			
			if( Item[itemid][item_type] == ITEM_TYPE_DRINK )
			{
				if( Item[itemid][item_value2] == 0 )
				{
					SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DRINK_SPRUNK);
				}
				else if( Item[itemid][item_value2] > 0 && Item[itemid][item_value2] < 5 )
				{
					SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DRINK_WINE);
					
					pInfo[playerid][player_drunk_time] = 30*60;
				}
				else
				{
					SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DRINK_BEER);
					
					pInfo[playerid][player_drunk_time] = 60*60;
				}
			}
			
			ProxMessage(playerid, sprintf("spo¿ywa %s.", Item[itemid][item_name]), PROX_AME);
			
			DeleteItem(itemid, true);
		}
		
		case ITEM_TYPE_PETY:
		{
			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_SMOKE_CIGGY);
			
			Item[itemid][item_value1] -= 1;
			
			if( Item[itemid][item_value1] <= 0 ) DeleteItem(itemid, true);
		}
		
		case ITEM_TYPE_ZEGAREK:
		{
			if( Item[itemid][item_used] )
			{
				TextDrawHideForPlayer(playerid, ZegarekTD);
				
				Item[itemid][item_used] = false;
				mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_used = 0 WHERE item_uid = %d", Item[itemid][item_uid]));
			}
			else
			{
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_ZEGAREK) > -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Masz ju¿ u¿yty inny przedmiot tego typu.");
				
				ShowZegarekTd(playerid);
				
				Item[itemid][item_used] = true;
				mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_used = 1 WHERE item_uid = %d", Item[itemid][item_uid]));
			}
		}
		
		case ITEM_TYPE_ACCESSORY:
		{
			if( Item[itemid][item_used] )
			{
				if( !IsPlayerAttachedObjectSlotUsed(playerid, Item[itemid][item_value2]) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Podczas u¿ywania przedmiotu wyst¹pi³ b³¹d.");
				
				RemovePlayerAttachedObject(playerid, Item[itemid][item_value2]);
			
				Item[itemid][item_used] = false;
				mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_used = 0 WHERE item_uid = %d", Item[itemid][item_uid]));
			}
			else
			{
				new count = GetPlayerUsedItemCount(playerid, ITEM_TYPE_ACCESSORY);
				if( IsPlayerVip(playerid) && count >= 6 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Jako posiadacz konta premium mo¿esz mieæ za³o¿one tylko 6 dodatków postaci jednoczeœnie.");
				else if( !IsPlayerVip(playerid) && count >= 2 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Jako i¿ nie posiadasz konta premium, mo¿esz mieæ za³o¿one tylko 2 dodatki postaci jednoczeœnie.\nZakup konto premium aby zwiêkszyæ limit a¿ do 6!");
				
				if( Item[itemid][item_value1] == 0 )
				{
					new Cache:result;
					result = mysql_query(g_sql, sprintf("INSERT INTO crp_access (access_uid, access_model, access_bone) VALUES (null, %d, %d)", Item[itemid][item_model], Item[itemid][item_value2]));
					
					Item[itemid][item_value1] = cache_insert_id();
					Item[itemid][item_value2] = 0;

					cache_delete(result);
					
					mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_value1 = %d, item_value2 = %d WHERE item_uid = %d", Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_uid]));
				}
				
				pInfo[playerid][player_dialog_tmp1] = itemid;
				
				ShowPlayerDialog(playerid, DIALOG_ACCESSORY, DIALOG_STYLE_MSGBOX, "Zak³adanie dodatku", "Czy chcesz dostosowaæ pozycjê dodatku postaci?", "Tak", "Nie");
			}
		}
		
		case ITEM_TYPE_GYM_PASS:
		{
			if( GetPlayerVirtualWorld(playerid) == 0 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w si³owni.");
	        new did = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	        if( did == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w si³owni.");
	        if( Door[did][door_owner_type] != DOOR_OWNER_TYPE_GROUP || Door[did][door_owner] != Item[itemid][item_value2]) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w si³owni do której nale¿y ten karnet.");
	        
	        Item[itemid][item_value1]--;
	        mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_value1 = %d WHERE item_uid = %d", Item[itemid][item_value1], Item[itemid][item_uid]));

	        if( Item[itemid][item_value1] <= 0 ) DeleteItem(itemid, true), Alert(playerid, ALERT_TYPE_SUCCESS, "U¿y³eœ karnetu na trening silowy, jest on aktywny do zakonczenia cwiczenia lub wyjscia z serwera.");
	        else Alert(playerid, ALERT_TYPE_SUCCESS, sprintf("U¿y³eœ karnetu na trening silowy (zosta³o na nim %d wejœæ), jest on aktywny do zakonczenia cwiczenia lub wyjscia z serwera.", Item[itemid][item_value1]));
	        pInfo[playerid][player_karnet] = true;

	        Gym_OnPlayerUseKarnet(playerid);
		}
		
		case ITEM_TYPE_MEDICINE:
		{
			if( pInfo[playerid][player_health] >= 30 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz u¿yæ lekarstwa, poniewa¿ Twój stan zdrowia tego nie wymaga.");
			
			pInfo[playerid][player_health] = 35;

			SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
			
			ProxMessage(playerid, sprintf("aplikuje lekarstwo %s.", Item[itemid][item_name]), PROX_AME);
			
			DeleteItem(itemid, true);
		}
		
		case ITEM_TYPE_EXAMINATION:
		{
			new slot = GetPlayerDutySlot(playerid);
			if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tego przedmiotu.");
			
			new gid = pGroup[playerid][slot][pg_id];
			if( Group[gid][group_type] != GROUP_TYPE_EMS && Group[gid][group_type] != GROUP_TYPE_FD ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tego przedmiotu.");

			DynamicGui_Init(playerid);
			new string[2048], count;
			
			new Float:p_pos[3];
			GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
			
			foreach(new p : Player)
			{
				if( !pInfo[p][player_logged] ) continue;
				//if( p == playerid ) continue; //TODO: odkomentowac
				if( pInfo[p][player_admin_spec] ) continue;
				if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 5.0 )
				{	
					format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);
					
					DynamicGui_AddRow(playerid, p);
					count++;
				}
			}
			
			if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W pobli¿u nie ma ¿adnych osób.");
			else ShowPlayerDialog(playerid, DIALOG_EXAMINATION, DIALOG_STYLE_LIST, "Osoby znajduj¹ce siê w pobli¿u:", string, "Wyœlij", "Zamknij");

			DeleteItem(itemid, true);
		}

		case ITEM_TYPE_ADRENALINE:
		{
			new slot = GetPlayerDutySlot(playerid);
			if( slot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tego przedmiotu.");
			
			new gid = pGroup[playerid][slot][pg_id];
			if( Group[gid][group_type] != GROUP_TYPE_EMS && Group[gid][group_type] != GROUP_TYPE_FD ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e u¿ywaæ tego przedmiotu.");

			DynamicGui_Init(playerid);
			new string[2048], count;
			
			new Float:p_pos[3];
			GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
			
			foreach(new p : Player)
			{
				if( !pInfo[p][player_logged] ) continue;
				//if( p == playerid ) continue; //TODO: usunac
				if( pInfo[p][player_admin_spec] ) continue;
				if( pInfo[p][player_damage] != DAMAGE_LEVEL_CRITICAL) continue;
				if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 5.0 )
				{	
					format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);
					
					DynamicGui_AddRow(playerid, p);
					count++;
				}
			}
			
			if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W pobli¿u nie ma ¿adnych osób spe³niaj¹cych wymogi.");
			else ShowPlayerDialog(playerid, DIALOG_ADRENALINE, DIALOG_STYLE_LIST, "Osoby znajduj¹ce siê w pobli¿u:", string, "Wyœlij", "Zamknij");

			DeleteItem(itemid, true);
		}

		case ITEM_TYPE_CORPSE:
		{
			new dslot = GetPlayerDutySlot(playerid);
			if( dslot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e przeprowadzaæ sekcje zw³ok.");
			
			new gid = pGroup[playerid][dslot][pg_id];
			if( gid == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e przeprowadzaæ sekcje zw³ok.");
			if( !GroupHasFlag(gid, GROUP_FLAG_EXAMINE_CORPSE) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie jesteœ na s³u¿bie w grupie, która mo¿e przeprowadzaæ sekcje zw³ok.");
			
			new did = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			if( did == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w budynku grupy, w której jesteœ na s³u¿bie.");
			if( Door[did][door_owner_type] != DOOR_OWNER_TYPE_GROUP || Door[did][door_owner] != Group[gid][group_uid] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie znajdujesz siê w budynku grupy, w której jesteœ na s³u¿bie.");
			
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT * FROM crp_corpses WHERE corpse_uid = %d", Item[itemid][item_value1]));
			
			new bool:corpse_exist = !!cache_get_rows(), cuid, examined;

			if( corpse_exist ) {
				cuid = cache_get_int(0, "corpse_uid");
				examined = cache_get_int(0, "corpse_examined");
			}

			cache_delete(result);

			if( corpse_exist ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Wyst¹pi³ nieoczekiwany b³¹d, powinieneœ zg³osiæ go do administracji.");
			else
			{
				if( examined > gettime() )
				{					
					new examine_date[50];
					GetRelativeDate(examined, examine_date);
					SendGuiInformation(playerid, "Informacja", sprintf("Zw³oki s¹ aktualnie badane przez specjalistów. Przewidywany czas zakoñczenia: %s.", examine_date));
				}
				else if( examined == 0 )
				{
					mysql_pquery(g_sql, sprintf("UPDATE crp_corpses SET corpse_examined = %d WHERE corpse_uid = %d", gettime() + 24*60*60, cuid));
					
					SendGuiInformation(playerid, "Informacja", "Zw³oki zosta³y poddane sekcji, która zakoñczy siê za 24h.\nPo tym czasie bêdziesz móg³ sprawdziæ szczegó³y stanu cia³a oraz opinie koronera.");
				}
				else
				{
					SendGuiInformation(playerid, "Informacja", "TEST");
				}
			}
		}

		case ITEM_TYPE_CD:
		{
			if( Item[itemid][item_value1] == 0 )
			{
				// Tworzymy plytke ;d
				
				pInfo[playerid][player_dialog_tmp1] = itemid;
				ShowPlayerDialog(playerid, DIALOG_CD_URL, DIALOG_STYLE_INPUT, "Tworzenie p³yty » Adres radia", "W poni¿szym polu podaj adres do internetowego radia: (ogg/vorbis;mp3;.pls)", "Dalej", "Anuluj");
			}
			else
			{
				if( GetPlayerState(playerid) != PLAYER_STATE_DRIVER ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Aby u¿yæ p³yty musisz byæ kierowc¹ pojazdu.");
				
				new vehicleid = GetPlayerVehicleID(playerid);
				foreach(new itid : Items)
				{
					if( Item[itid][item_owner_type] == ITEM_OWNER_TYPE_VEHICLE_COMPONENT && Item[itid][item_owner] == Vehicle[vehicleid][vehicle_uid] && Item[itid][item_type] == ITEM_TYPE_CD )
					{
						return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W radiu tego samochodu znajduje siê ju¿ inna p³yta. Aby j¹ wyci¹gn¹æ u¿yj komendy /v.");
					}
				}
				
				Item[itemid][item_owner_type] = ITEM_OWNER_TYPE_VEHICLE_COMPONENT;
				Item[itemid][item_owner] = Vehicle[vehicleid][vehicle_uid];
				
				mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_ownertype = %d, item_owner = %d WHERE item_uid = %d", Item[itemid][item_owner_type], Item[itemid][item_owner], Item[itemid][item_uid]));
				
				ProxMessage(playerid, "wk³ada p³ytê do radia.", PROX_AME);
				
				new url[120], Cache:result;
				result = mysql_query(g_sql, sprintf("SELECT audio_url FROM crp_audiourls WHERE audio_uid = %d", Item[itemid][item_value1]));

				cache_get(0, "audio_url", url);
				
				cache_delete(result);
				
				Vehicle[vehicleid][vehicle_radio] = true;
				
				foreach(new p : Player)
				{
					if( GetPlayerVehicleID(p) == vehicleid )
					{
						PlayAudioStreamForPlayer(p, url);
					}
				}
			}
		}

		case ITEM_TYPE_CUFFS:
		{
			if( !Item[itemid][item_used] )
			{
				// Jesli przedmiot nie jest aktywny
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_CUFFS) != -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Masz ju¿ u¿yte inne kajdanki.");

				pInfo[playerid][player_dialog_tmp1] = itemid;

				DynamicGui_Init(playerid);
				new string[2048], count;
				
				new Float:p_pos[3];
				GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
				
				foreach(new p : Player)
				{
					if( !pInfo[p][player_logged] ) continue;
					if( p == playerid ) continue;
					if( pInfo[p][player_admin_spec] ) continue;
					if( GetPlayerDistanceFromPoint(p, p_pos[0], p_pos[1], p_pos[2]) <= 5.0 )
					{
						//if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASK) > -1 ) format(string, sizeof(string), "%s##\t\t%s\n", string, pInfo[p][player_name]);
						//else format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);
						
						format(string, sizeof(string), "%s%d\t\t%s\n", string, p, pInfo[p][player_name]);
						
						DynamicGui_AddRow(playerid, p);
						count++;
					}
				}
				
				if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W pobli¿u nie ma ¿adnych osób.");
				else ShowPlayerDialog(playerid, DIALOG_HANDCUFFS_SELECT, DIALOG_STYLE_LIST, "Osoby znajduj¹ce siê w pobli¿u:", string, "Wyœlij", "Zamknij");
			}
			else
			{
				// jesli jest aktywny
				new targetid = Item[itemid][item_value1];

				pInfo[targetid][player_is_cuffed] = false;
				pInfo[targetid][player_cuff_targetid] = INVALID_PLAYER_ID;

				RemovePlayerAttachedObject(targetid, pInfo[targetid][player_cuff_oindex]);
				pInfo[targetid][player_cuff_oindex] = -1;

				SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);

				Item[itemid][item_used] = false;

				GameTextForPlayer(playerid, "~g~Odkules ~w~gracza", 3000, 3);
				GameTextForPlayer(targetid, "~w~Zostales ~g~odkuty", 3000, 3);
			}
		}

		case ITEM_TYPE_DRUGS:
		{
			OnPlayerUseDrugs(playerid, itemid);
		}

		case ITEM_TYPE_BAG:
		{
			new string[512], count = 0;

			format(string, sizeof(string), "ID\tNazwa\tParametry\n");

			DynamicGui_Init(playerid);

			foreach(new i : Items)
			{
				if(Item[i][item_owner_type] == ITEM_OWNER_TYPE_BAG && Item[i][item_owner] == Item[itemid][item_uid])
				{
					new name[32];
					format(name, sizeof(name), "%s", Item[i][item_name]);

					if(Item[i][item_type] == ITEM_TYPE_DRUGS) {
						format(name, sizeof(name), "%s (%dg)", Item[i][item_name], Item[i][item_amount]);
					}
					else if(Item[i][item_type] == ITEM_TYPE_DRUG_INGR) {
						format(name, sizeof(name), "%s (%dszt)", Item[i][item_name], Item[i][item_amount]);
					}
					
					format(string, sizeof(string), "%s%d\t%s\t{000000}%d,%d,%d\n", string, Item[i][item_uid], name, Item[i][item_type], Item[i][item_value1], Item[i][item_value1]);
					DynamicGui_AddRow(playerid, i);

					count++;					
				}
			}

			if(count == 0) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "W tej torbie nie ma przedmiotów.");
			else ShowPlayerDialog(playerid, DIALOG_BAG, DIALOG_STYLE_TABLIST_HEADERS, Item[itemid][item_name], string, "Wyci¹gnij", "Anuluj");
		}

		case ITEM_TYPE_BOOMBOX:
		{
			new Float:player_pos[4];
			GetPlayerPos(playerid, player_pos[0], player_pos[1], player_pos[2]);
			GetPlayerFacingAngle(playerid, player_pos[3]);

			if(IsPlayerInAnyDynamicArea(playerid))
			{
				if(CanPlayerUseArea(playerid, pInfo[playerid][player_area]))
				{
					if(!Item[itemid][item_used])
					{
						pInfo[playerid][player_dialog_tmp1] = itemid;
						ShowPlayerDialog(playerid, DIALOG_BOOMBOX, DIALOG_STYLE_INPUT, "WprowadŸ link", "Poni¿ej wprowadŸ link do muzyki w rozszerzeniu .pls lub .mp3.", "Od³ó¿", "Anuluj");
					}
					else
					{
						new Float:objectpos[3];
						GetDynamicObjectPos(pInfo[playerid][player_boombox_id], objectpos[0], objectpos[1], objectpos[2]);
						new Float:distance = GetPlayerDistanceFromPoint(playerid, objectpos[0], objectpos[1], objectpos[2]);

						if(distance < 3.0)
						{
							DestroyDynamicObject(pInfo[playerid][player_boombox_id]);
							pInfo[playerid][player_boombox_id] = 0;
							Item[itemid][item_used] = false;

							ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
							ProxMessage(playerid, "podnosi boombox z ziemi.", PROX_AME);

							Area[pInfo[playerid][player_area]][area_music_url][0] = EOS;
							Area[pInfo[playerid][player_area]][area_boombox_id] = 0;

							foreach(new p : Player)
							{
								if(pInfo[p][player_area] == pInfo[playerid][player_area] && GetPlayerVirtualWorld(playerid) == GetPlayerVirtualWorld(p))
								{
									StopAudioStreamForPlayer(p);
									SendClientMessageToAll(COLOR_YELLOW, sprintf("[D] stoping audiostream for p%d", p));
								}
							}
						}
						else Alert(playerid, ALERT_TYPE_NEGATIVE, "Musisz znajdowac sie ~r~blizej ~w~boomboxa!");
					}
				}
				else return Alert(playerid, ALERT_TYPE_NEGATIVE, "Aby odlozyc ~y~boomboxa ~w~na strefie musisz byc w grupie do ktorej ona nalezy.");
			}
			else
			{
				if(!Item[itemid][item_used])
				{
					pInfo[playerid][player_dialog_tmp1] = itemid;
					ShowPlayerDialog(playerid, DIALOG_BOOMBOX, DIALOG_STYLE_INPUT, "WprowadŸ link", "Poni¿ej wprowadŸ link do muzyki w rozszerzeniu .pls lub .mp3.", "Od³ó¿", "Anuluj");
				}
				else
				{
					new Float:objectpos[3];
					GetDynamicObjectPos(pInfo[playerid][player_boombox_id], objectpos[0], objectpos[1], objectpos[2]);

					new Float:distance = GetPlayerDistanceFromPoint(playerid, objectpos[0], objectpos[1], objectpos[2]);

					if(distance < 3.0)
					{
						DestroyDynamicObject(pInfo[playerid][player_boombox_id]);
						pInfo[playerid][player_boombox_id] = 0;
						Item[itemid][item_used] = false;

						ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
						ProxMessage(playerid, "podnosi boombox z ziemi.", PROX_AME);

						Area[pInfo[playerid][player_area]][area_music_url][0] = EOS;
						Area[pInfo[playerid][player_area]][area_boombox_id] = 0;

						foreach(new p : Player)
						{
							if(pInfo[p][player_area] == pInfo[playerid][player_area] && GetPlayerVirtualWorld(playerid) == GetPlayerVirtualWorld(p))
							{
								StopAudioStreamForPlayer(p);
							}
						}
					}
					else Alert(playerid, ALERT_TYPE_NEGATIVE, "Musisz znajdowac sie ~r~blizej ~w~boomboxa!");
				}
			}
		}

		default:
		{
			Alert(playerid, ALERT_TYPE_NEGATIVE, "Ten typ przedmiotu ~r~nie posiada ~w~skryptowej funkcji!");
		}
	}
	
	return 1;
}

stock GetItemByData(i_type, i_value1 = -1, i_value2 = -1)
{
	new item = -1, bool:item_good = false;
	foreach(new itemid : Items)
	{
		item_good = true;
		if( Item[itemid][item_type] == i_type )
		{
			if( i_value1 > -1 )
			{
				if( Item[itemid][item_value1] != i_value1 ) item_good = false;
			}
			
			if( i_value2 > -1 )
			{
				if( Item[itemid][item_value2] != i_value2 ) item_good = false;
			}
		}
		else item_good = false;
		
		if( item_good )
		{
			item = itemid;
			break;
		}
	}
	
	return item;
}

stock GetPlayerUsedItem(playerid, i_type)
{
	foreach(new itemid : Items)
	{
		if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[itemid][item_owner] == pInfo[playerid][player_id] && Item[itemid][item_type] == i_type && Item[itemid][item_used] ) return itemid;
	}
	
	return -1;
}

stock GetPlayerUsedItemCount(playerid, i_type)
{
	new i = 0;
	
	foreach(new itemid : Items)
	{
		if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[itemid][item_owner] == pInfo[playerid][player_id] && Item[itemid][item_type] == i_type && Item[itemid][item_used] ) i++;
	}
	
	return i;
}

stock GetPlayerItem(playerid, i_type)
{
	foreach(new itemid : Items)
	{
		if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[itemid][item_owner] == pInfo[playerid][player_id] && Item[itemid][item_type] == i_type ) return itemid;
	}
	
	return -1;
}


/* native Item_Drop(itemid, playerid = INVALID_PLAYER_ID)
 * @params integer, integer
 * @returns true
 */
stock Item_Drop(itemid, playerid = INVALID_PLAYER_ID, bool:more_items = false)
{
	if( playerid != INVALID_PLAYER_ID && (Item[itemid][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[itemid][item_owner] != pInfo[playerid][player_id]) ) return 1;
	
	if(Item[itemid][item_selected]) return Alert(playerid, ALERT_TYPE_NEGATIVE, "~r~Wystapil blad! ~w~Nie mozesz odlozyc przedmiotu ktory aktualnie jest w zaznaczeniu.");

	if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_GROUND )
	{
		Item[itemid][item_object] = CreateDynamicObject(Item[itemid][item_model], Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z], Item[itemid][item_rx], Item[itemid][item_ry], Item[itemid][item_rz], Item[itemid][item_world], Item[itemid][item_interior], -1, 100.0);
	}
	else if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_PLAYER )
	{
		if( Item[itemid][item_used] ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie mo¿esz od³o¿yæ przedmiotu, który jest u¿ywany");
		
		if( !IsPlayerInAnyVehicle(playerid) )
		{
			new Cache:result;
			result = mysql_query(g_sql, sprintf("SELECT * FROM `crp_items_proto` WHERE `model` = %d", Item[itemid][item_model]));
			
			if( cache_get_rows() )
			{
				if( playerid != INVALID_PLAYER_ID )
				{
					GetPointInAngleOfPlayer(playerid, Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z], 1.0, 0.0);
					
					Item[itemid][item_owner_type] = ITEM_OWNER_TYPE_GROUND;
					Item[itemid][item_owner] = 0;
					Item[itemid][item_z] = floatadd(Item[itemid][item_z], cache_get_float(0, "z"));
					Item[itemid][item_rx] = cache_get_float(0, "rx");
					Item[itemid][item_ry] = cache_get_float(0, "ry");
					Item[itemid][item_rz] = cache_get_float(0, "rz");
					Item[itemid][item_world] = GetPlayerVirtualWorld(playerid);
					Item[itemid][item_interior] = GetPlayerInterior(playerid);
					Item[itemid][item_selected] = false;
					Item[itemid][item_favorite] = false;
					
					Item[itemid][item_object] = CreateDynamicObject(Item[itemid][item_model], Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z], Item[itemid][item_rx], Item[itemid][item_ry], Item[itemid][item_rz], Item[itemid][item_world], Item[itemid][item_interior], -1, 100.0);
					
					new str[400];
					strcat(str, sprintf("UPDATE `crp_items` SET `item_ownertype` = %d, `item_owner` = 0, `item_posx` = %f, `item_posy` = %f, `item_posz` = %f,", Item[itemid][item_owner_type], Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z]));
					strcat(str, sprintf(" `item_rotx` = %f, `item_roty` = %f, `item_rotz` = %f, `item_world` = %d, `item_interior` = %d, `item_favorite` = %d  WHERE `item_uid` = %d", Item[itemid][item_rx], Item[itemid][item_ry], Item[itemid][item_rz], Item[itemid][item_world], Item[itemid][item_interior], Item[itemid][item_favorite], Item[itemid][item_uid]));
					mysql_tquery(g_sql, str);

					PlayerLog(sprintf("Drops thing %s {V:{%d,%d,%d},TYPE:%d,WORLD:%d,POS:{%.1f,%.1f,%.1f}}", ItemLogLink(Item[itemid][item_uid]), Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_value3], Item[itemid][item_type], Item[itemid][item_world], Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z]), pInfo[playerid][player_id], "item");

					if(!more_items)
					{
						Streamer_UpdateEx(playerid, Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z]);
						ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
						ProxMessage(playerid, "odk³ada coœ na ziemiê.", PROX_AME);
					}
				}
			}
			else
			{				
				if( playerid != INVALID_PLAYER_ID )
				{
					if(GetPlayerPermissions(playerid, 5)) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Podczas odk³adania przedmiotu wyst¹pi³ b³¹d.");
					
					GetPointInAngleOfPlayer(playerid, Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z], 1.0, 0.0);
					
					Item[itemid][item_object] = CreateDynamicObject(Item[itemid][item_model], Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z], Item[itemid][item_rx], Item[itemid][item_ry], Item[itemid][item_rz], Item[itemid][item_world], Item[itemid][item_interior], -1, 100.0);
					
					Streamer_UpdateEx(playerid, Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z]);

					pInfo[playerid][player_items_proto_create] = true;
					pInfo[playerid][player_items_proto_create_id] = itemid;
					
					defer DelayEditObject[250](playerid, Item[itemid][item_object]);
					
					Item[itemid][item_owner_type] = ITEM_OWNER_TYPE_GROUND;
					Item[itemid][item_owner] = 0;
					
					SendPlayerInformation(playerid, "Ten model ~r~nie posiada wzoru~w~ przedmiotu w bazie danych, ~y~utworz~w~ go teraz.", 4000);
				}
			}

			cache_delete(result);
		}
		else
		{
			new vid = GetPlayerVehicleID(playerid);
			if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie masz uprawnieñ do odk³adania przedmiotów w tym pojeŸdzie.");
			
			Item[itemid][item_owner_type] = ITEM_OWNER_TYPE_VEHICLE;
			Item[itemid][item_owner] = Vehicle[vid][vehicle_uid];
			
			mysql_tquery(g_sql, sprintf("UPDATE `crp_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_uid` = %d", Item[itemid][item_owner_type], Item[itemid][item_owner], Item[itemid][item_uid]));
			ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
			
			PlayerLog(sprintf("Drops thing %s {V:{%d,%d,%d},TYPE:%d} in vehicle %s", ItemLogLink(Item[itemid][item_uid]), Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_value3], Item[itemid][item_type], VehicleLogLink(Item[itemid][item_owner])), pInfo[playerid][player_id], "item");
			ProxMessage(playerid, "odk³ada coœ do pojazdu.", PROX_AME);
		}
	}
	
	return 1;
}

/* native Item_Pickup(itemid, playerid = INVALID_PLAYER_ID)
 * @params integer, integer
 * @returns true
 */
stock Item_Pickup(itemid, playerid = INVALID_PLAYER_ID)
{
	if( playerid != INVALID_PLAYER_ID && Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_PLAYER ) return 1;
	
	if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_GROUND )
	{
		DestroyDynamicObject(Item[itemid][item_object]);
		
		Item[itemid][item_object] = -1;
		
		Item[itemid][item_x] = 0.0;
		Item[itemid][item_y] = 0.0;
		Item[itemid][item_z] = 0.0;
		Item[itemid][item_rx] = 0.0;
		Item[itemid][item_ry] = 0.0;
		Item[itemid][item_rz] = 0.0;
		Item[itemid][item_world] = 0;
		Item[itemid][item_interior] = 0;

		PlayerLog(sprintf("Pickup thing %s {V:{%d,%d,%d},TYPE:%d,WORLD:%d,POS:{%.1f,%.1f,%.1f}} ", ItemLogLink(Item[itemid][item_uid]), Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_value3], Item[itemid][item_type], Item[itemid][item_world], Item[itemid][item_x], Item[itemid][item_y], Item[itemid][item_z]), pInfo[playerid][player_id], "item");
		ProxMessage(playerid, "podnosi coœ z ziemi.", PROX_AME);
	}
	else if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_VEHICLE )
	{
		PlayerLog(sprintf("Pickup thing %s {V:{%d,%d,%d},TYPE:%d} from vehicle %s", ItemLogLink(Item[itemid][item_uid]), Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_value3], Item[itemid][item_type], VehicleLogLink(Item[itemid][item_owner])), pInfo[playerid][player_id], "item");
		ProxMessage(playerid, "wyci¹ga coœ z pojazdu.", PROX_AME);
	}
	else if( Item[itemid][item_owner_type] == ITEM_OWNER_TYPE_DOOR )
	{
		PlayerLog(sprintf("Pickup thing %s {V:{%d,%d,%d},TYPE:%d} from door %s storage ", ItemLogLink(Item[itemid][item_uid]), Item[itemid][item_value1], Item[itemid][item_value2], Item[itemid][item_value3], Item[itemid][item_type], DoorLogLink(Item[itemid][item_owner])), pInfo[playerid][player_id], "item");

		ProxMessage(playerid, "wyci¹ga coœ ze schowka.", PROX_AME);
	}
	
	Item[itemid][item_owner_type] = ITEM_OWNER_TYPE_PLAYER;
	Item[itemid][item_owner] = pInfo[playerid][player_id];
	
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
	
	mysql_tquery(g_sql, sprintf("UPDATE `crp_items` SET `item_ownertype` = %d, `item_owner` = %d, `item_posx` = 0.0, `item_posy` = 0.0, `item_posz` = 0.0, `item_rotx` = 0.0, `item_roty` = 0.0, `item_rotz` = 0.0, `item_world` = 0, `item_interior` = 0  WHERE `item_uid` = %d", Item[itemid][item_owner_type], Item[itemid][item_owner], Item[itemid][item_uid]));
	
	return 1;
}

stock GetItemDefaultModel(i_type, i_val1, i_val2)
{
	#pragma unused i_val2

	new i_model;
	switch(i_type)
	{
		case 1:
		{
			switch(i_val1)
			{
				case 0: i_model = 0;
				case 1: i_model = 331;
				case 2: i_model = 333;
				case 3: i_model = 334;
				case 4: i_model = 335;
				case 5: i_model = 336;
				case 6: i_model = 337;
				case 7: i_model = 338;
				case 8: i_model = 339;
				case 9: i_model = 341;
				case 10: i_model = 321;
				case 11: i_model = 322;
				case 12: i_model = 323;
				case 13: i_model = 324;
				case 14: i_model = 325;
				case 15: i_model = 326;
				case 16: i_model = 342;
				case 17: i_model = 343;
				case 18: i_model = 344;
				case 22: i_model = 346;
				case 23: i_model = 347;
				case 24: i_model = 348;
				case 25: i_model = 349;
				case 26: i_model = 350;
				case 27: i_model = 351;
				case 28: i_model = 352;
				case 29: i_model = 353;
				case 30: i_model = 355;
				case 31: i_model = 356;
				case 32: i_model = 372;
				case 33: i_model = 357;
				case 34: i_model = 358;
				case 35: i_model = 359;
				case 36: i_model = 360;
				case 37: i_model = 361;
				case 38: i_model = 362;
				case 39: i_model = 363;
				case 40: i_model = 364;
				case 41: i_model = 365;
				case 42: i_model = 366;
				case 43: i_model = 367;
				case 44: i_model = 368;
				case 45: i_model = 369;
				case 46: i_model = 371;
			}
		}
		
		case 2: i_model = 2057;
		case 4: i_model = 18874;
		case 5: i_model = 2663;
		case 8: i_model = 2843;
		case 9: i_model = 1546;
		case 12: i_model = 2710;
		case 17: i_model = 1575;
	}

	return i_model;
}

/* native Item_Create(owner_type, owner_id, i_type, i_val1, i_val2, i_name[40], i_model)
 * @params integer, integer, integer, integer, integer, integer, string[40]
 * @returns true
 */
stock Item_Create(owner_type, owner_id, i_type, i_val1, i_val2, i_name[], i_model = 0, i_amount = 0, i_price = 0, i_group = 0)
{
	new itemid = -1;

	if(!i_model) i_model = GetItemDefaultModel(i_type, i_val1, i_val2);

	if( owner_type == ITEM_OWNER_TYPE_PLAYER )
	{
		new playerid = owner_id;
		if( owner_id > -1 )
		{
			if(i_type == ITEM_TYPE_DRUG_INGR && i_amount == 0) i_amount = 1;

			new Cache:result;
			result = mysql_query(g_sql, sprintf("INSERT INTO `crp_items` (`item_uid`,`item_model`,`item_ownertype`,`item_owner`,`item_type`,`item_value1`,`item_value2`,`item_name`,`item_created`,`item_amount`,`item_price`,`item_group`) VALUES (null, %d, %d, %d, %d, %d, %d,'%s', %d, %d, %d, %d)", i_model, owner_type, pInfo[playerid][player_id], i_type, i_val1, i_val2, i_name, gettime(), i_amount, i_price, i_group));
			
			new uid = cache_insert_id();

			cache_delete(result);

			itemid = LoadItem(sprintf("WHERE `item_uid` = %d", uid), true);
		}
	}
	else if( owner_type == ITEM_OWNER_TYPE_PACKAGE )
	{
		new Cache:result;
		result = mysql_query(g_sql, sprintf("INSERT INTO `crp_items` (`item_uid`,`item_model`,`item_ownertype`,`item_owner`,`item_type`,`item_value1`,`item_value2`,`item_name`,`item_created`,`item_amount`,`item_price`) VALUES (null, %d, %d, %d, %d, %d, %d,'%s', %d, %d, %d)", i_model, owner_type, owner_id, i_type, i_val1, i_val2, i_name, gettime(), i_amount, i_price));
		
		new uid = cache_insert_id();

		cache_delete(result);
		
		itemid = LoadItem(sprintf("WHERE `item_uid` = %d", uid), true);
	}
	else if( owner_type == ITEM_OWNER_TYPE_DOOR_WAREHOUSE )
	{
		new Cache:result;
		result = mysql_query(g_sql, sprintf("INSERT INTO `crp_items` (`item_uid`,`item_model`,`item_ownertype`,`item_owner`,`item_type`,`item_value1`,`item_value2`,`item_name`,`item_created`,`item_amount`,`item_price`) VALUES (null, %d, %d, %d, %d, %d, %d,'%s', %d, %d, %d)", i_model, owner_type, owner_id, i_type, i_val1, i_val2, i_name, gettime(), i_amount, i_price));
		
		new uid = cache_insert_id();

		cache_delete(result);
		
		itemid = LoadItem(sprintf("WHERE `item_uid` = %d", uid), true);
	}
	return itemid;
}

stock GetItemByUid(itemuid)
{
	foreach(new item : Items)
	{
		if( Item[item][item_uid] == itemuid ) return item;
	}
	
	return -1;
}

stock ShowPlayerFavoriteItems(playerid, bool:with_back = false)
{
	new string[512], count = 0;

	format(string, sizeof(string), "ID\tNazwa\tParametry\n");

	DynamicGui_Init(playerid);

	foreach(new i : Items)
	{
		if(Item[i][item_owner_type] == ITEM_OWNER_TYPE_PLAYER && Item[i][item_owner] == pInfo[playerid][player_id] && Item[i][item_favorite])
		{
			new prefix[10];
			if( Item[i][item_used] ) prefix = "{9E9E9E}";

			new name[150];
			format(name, sizeof(name), "%s", Item[i][item_name]);

			if(Item[i][item_type] == ITEM_TYPE_DRUGS) {
				format(name, sizeof(name), "%s (%dg)", Item[i][item_name], Item[i][item_amount]);
			}
			else if(Item[i][item_type] == ITEM_TYPE_DRUG_INGR) {
				format(name, sizeof(name), "%s (%dszt)", Item[i][item_name], Item[i][item_amount]);
			}
			
			format(string, sizeof(string), "%s%d\t%s%s\t%d,%d,%d\n", string, Item[i][item_uid], prefix, name, Item[i][item_type], Item[i][item_value1], Item[i][item_value1]);
			DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, i);

			count++;
		}
	}

	if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz ¿adnych ulubionych przedmiotów.");
	else
	{
		PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], " ~n~ ~n~ ~n~ ~n~Istnieje szybsza metoda na uzywanie przedmiotow. Skrot:~n~~b~~h~~h~/p [czesc nazwy przedmiotu]~n~~n~~y~Przyklady: ~h~/p desert    /p samsung");
		PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);

		new button_str[8];
		if(pInfo[playerid][player_items_selecting]) format(button_str, sizeof(button_str), "Dodaj");
		else format(button_str, sizeof(button_str), "U¿yj");

		if(with_back)
		{
			format(string, sizeof(string), "%s---\n", string);
			DynamicGui_AddBlankRow(playerid);

			format(string, sizeof(string), "%s"HEX_COLOR_SAMP"Wyœwietl\t"HEX_COLOR_SAMP"wszystkie swoje\t"HEX_COLOR_SAMP"przedmioty", string);
			DynamicGui_AddRow(playerid, DG_ITEMS_VIEW_ALL, 0);
		}

		ShowPlayerDialog(playerid, DIALOG_PLAYER_ITEMS, DIALOG_STYLE_TABLIST_HEADERS, "Dostêpne ulubione przedmioty", string, button_str, "Opcje");
	}
}

stock ShowPlayerItems(playerid)
{
	new count, string[2048];
	
	DynamicGui_Init(playerid);
	
	format(string, sizeof(string), "ID\tNazwa\tParametry\n");
	
	format(string, sizeof(string), "%s »\t%s Funkcja zaznaczania\t%s%s\n", string, (pInfo[playerid][player_items_selecting] == true) ? (HEX_COLOR_BROWN) : ("{FFFFFF}"), (pInfo[playerid][player_items_selecting] == true) ? (HEX_COLOR_BROWN) : ("{FFFFFF}"), (pInfo[playerid][player_selected_items] > 0) ? (sprintf("(zaznaczono %d przedmiotów)", pInfo[playerid][player_selected_items])) : (""));
	DynamicGui_AddRow(playerid, DG_ITEMS_SELECTING, -1);

	format(string, sizeof(string), "%s »\t Przedmioty w pobli¿u\n", string);
	DynamicGui_AddRow(playerid, DG_ITEMS_SEARCH, -1);

	format(string, sizeof(string), "%s »\t Ulubione przedmioty\n", string);
	DynamicGui_AddRow(playerid, DG_ITEMS_FAVORITES, -1);

	format(string, sizeof(string), "%s---\n", string);
	DynamicGui_AddBlankRow(playerid);

	foreach (new item : Items)
	{
		if( Item[item][item_owner_type] != ITEM_OWNER_TYPE_PLAYER || Item[item][item_owner] != pInfo[playerid][player_id] ) continue;
		
		new prefix[16];
		if( Item[item][item_used] ) prefix = HEX_COLOR_GREY;
		if( Item[item][item_favorite] ) prefix = HEX_COLOR_RED"[U] ";
		if( Item[item][item_selected]) prefix = HEX_COLOR_BROWN"[S] ";
		if( Item[item][item_favorite] && Item[item][item_used] ) prefix = HEX_COLOR_CREW_ADMIN_ROOT"[U] ";

		new name[150];
		format(name, sizeof(name), "%s", Item[item][item_name]);

		if(Item[item][item_type] == ITEM_TYPE_DRUGS) {
			format(name, sizeof(name), "%s (%dg)", Item[item][item_name], Item[item][item_amount]);
		}
		else if(Item[item][item_type] == ITEM_TYPE_DRUG_INGR) {
			format(name, sizeof(name), "%s (%dszt)", Item[item][item_name], Item[item][item_amount]);
		}
		
		format(string, sizeof(string), "%s%d\t%s%s\t%d,%d,%d,%d\n", string, Item[item][item_uid], prefix, name, Item[item][item_type], Item[item][item_value1], Item[item][item_value2], Item[item][item_value3]);
		DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, item);	
		count++;
	}
	
	if( count == 0 ) SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Nie posiadasz ¿adnych przedmiotów.");
	else
	{
		PlayerTextDrawSetString(playerid, pInfo[playerid][Dashboard], " ~n~ ~n~ ~n~ ~n~Istnieje szybsza metoda na uzywanie przedmiotow. Skrot:~n~~b~~h~~h~/p [czesc nazwy przedmiotu]~n~~n~~y~Przyklady: ~h~/p desert    /p samsung");
		PlayerTextDrawShow(playerid, pInfo[playerid][Dashboard]);

		new button_str[8];
		if(pInfo[playerid][player_items_selecting]) format(button_str, sizeof(button_str), "Dodaj");
		else format(button_str, sizeof(button_str), "U¿yj");

		ShowPlayerDialog(playerid, DIALOG_PLAYER_ITEMS, DIALOG_STYLE_TABLIST_HEADERS, "Dostêpne przedmioty", string, button_str, "Opcje");
	}
}

stock ShowPlayerMoreOptions(playerid, itemid)
{
	new selected_items_count = pInfo[playerid][player_selected_items];
	if(selected_items_count <= 1)
	{
		Item[itemid][item_selected] = false;
		pInfo[playerid][player_selected_items] = 0;
		pInfo[playerid][player_items_selecting] = false;
	}

	if(pInfo[playerid][player_items_selecting]) //if items are selected
	{
		DynamicGui_Init(playerid);

		new string[256];
		format(string, sizeof(string), "Zaznaczono:\t%d\n", selected_items_count);

		format(string, sizeof(string), "%s0\tInformacje o przedmiotach\n", string);
		DynamicGui_AddRow(playerid, DG_MULTIITEMS_MORE_INFO, 0);

		format(string, sizeof(string), "%s1\tOd³ó¿ przedmioty na ziemiê\n", string);
		DynamicGui_AddRow(playerid, DG_MULTIITEMS_MORE_DROPG, 0);

		if( GetPlayerVirtualWorld(playerid) > 0 )
		{
			new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			if( d_id > -1 )
			{
				if( CanPlayerUseDoor(playerid, d_id) )
				{
					format(string, sizeof(string), "%s2\tW³ó¿ do schowka w drzwiach\n", string);
					DynamicGui_AddRow(playerid, DG_MULTIITEMS_MORE_PUT_IN_DOOR, 0);
				}
			}
		}

		format(string, sizeof(string), "%s3\tDodaj do ulubionych\n", string);
		DynamicGui_AddRow(playerid, DG_MULTIITEMS_FAVORITES, 0);

		ShowPlayerDialog(playerid, DIALOG_ITEM_MORE, DIALOG_STYLE_TABLIST_HEADERS, "Zaznaczone przedmioty » Opcje", string, "Wybierz", "Zamknij");
	}
	else
	{
		// Show item more options
		
		DynamicGui_Init(playerid);
		new string[256];
		new lp = 1;

		
		format(string, sizeof(string), "%s0%d\tInformacje o przedmiocie\n", string, lp);
		DynamicGui_AddRow(playerid, DG_ITEMS_MORE_INFO, itemid);
		lp++;
		
		if( IsPlayerInAnyVehicle(playerid) ) format(string, sizeof(string), "%s0%d\tOd³ó¿ przedmiot do pojazdu\n", string, lp);
		else format(string, sizeof(string), "%s0%d\tOd³ó¿ przedmiot na ziemiê\n", string, lp);
		
		DynamicGui_AddRow(playerid, DG_ITEMS_MORE_DROPG, itemid);
		lp++;
		
		format(string, sizeof(string), "%s0%d\tSprzedaj graczowi\n", string, lp);
		DynamicGui_AddRow(playerid, DG_ITEMS_MORE_SELL, itemid);
		lp++;

		if(Item[itemid][item_type] == ITEM_TYPE_DRUGS || Item[itemid][item_type] == ITEM_TYPE_DRUG_INGR) {
			if( Item[itemid][item_amount] > 1 ) {
				format(string, sizeof(string), "%s0%d\tPodziel przedmiot\n", string, lp);
				DynamicGui_AddRow(playerid, DG_ITEMS_MORE_DRUGS_DIVIDE, itemid);
				lp++;
			}

			format(string, sizeof(string), "%s0%d\tPo³¹cz przedmiot\n", string, lp);
			DynamicGui_AddRow(playerid, DG_ITEMS_MORE_DRUGS_JOIN, itemid);
			lp++;

			DynamicGui_SetDialogValue(playerid, 5);
		}
		
		if( GetPlayerVirtualWorld(playerid) > 0 )
		{
			new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			if( d_id > -1 )
			{
				if( CanPlayerUseDoor(playerid, d_id) )
				{
					format(string, sizeof(string), "%s0%d\tW³ó¿ do schowka drzwi\n", string, lp);
					DynamicGui_AddRow(playerid, DG_ITEMS_MORE_PUT_IN_DOOR, itemid);
					lp++;
				}
			}
		}
		
		format(string, sizeof(string), "%s0%d\t%s ulubionych\n", string, lp, (Item[itemid][item_favorite]) ? ("Usuñ z") : ("Dodaj do"));
		DynamicGui_AddRow(playerid, DG_ITEMS_MORE_FAVORITE, itemid);
		lp++;

		if(Item[itemid][item_type] != ITEM_TYPE_BAG)
		{
			format(string, sizeof(string), "%s0%d\tW³ó¿ do torby", string, lp);
			DynamicGui_AddRow(playerid, DG_ITEMS_MORE_PUT_IN_BAG, itemid);
			lp++;
		}

		ShowPlayerDialog(playerid, DIALOG_ITEM_MORE, DIALOG_STYLE_LIST, sprintf("%s [UID: %d] » Opcje", Item[itemid][item_name], Item[itemid][item_uid]), string, "Wybierz", "Zamknij");
	}

	return 1;
}