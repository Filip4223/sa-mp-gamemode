stock GetGroupByUid(groupUid)
{
	foreach (new g : Groups)
	{
		if( Group[g][group_uid] == groupUid ) return g;
	}
	
	return -1;
}

stock IsPlayerInAnyGroup(playerid)
{
	if( pGroup[playerid][0][pg_id] > -1 ) return 1;
	
	return 0;
}

stock IsCorrectGroup(gid)
{
	if( !(gid >= 0 && gid < MAX_GROUPS) ) return 0;
	if( !Iter_Contains(Groups, gid) ) return 0;
	
	return 1;
}

function OnGroupPayment(gid)
{
	mysql_tquery(g_sql, sprintf("UPDATE crp_groups SET group_cash = group_cash - %d WHERE group_uid = %d", abs(cache_get_int(0, "amount")*750), gid));
}

stock GetPlayerGroupSlot(playerid, gid)
{
	for(new i;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] == gid ) return i;
	}
	return -1;
}

stock GetPlayerDutySlot(playerid)
{
	for(new i;i<5;i++)
	{
		if( pGroup[playerid][i][pg_duty] ) return i;
	}
	
	return -1;
}

stock GetPlayerGroupFreeSlot(playerid)
{
	for(new i;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] == -1 ) return i;
	}
	
	return -1;
}

stock IsPlayerInGroupPlace(playerid, gid, bool:in_areas = false, bool:everywhere = true)
{
	if( everywhere && GroupHasFlag(gid, GROUP_FLAG_DUTY_ANYPLACE) ) return 1;
	
	if( in_areas )
	{
		new a_id = GetPlayerArea(playerid, AREA_TYPE_NORMAL, AREA_OWNER_TYPE_GROUP);
		if( a_id > -1 ) 
		{
			if( Area[a_id][area_owner] == Group[gid][group_uid] ) return 1;
		}
	}
	
	if( GroupHasFlag(gid, GROUP_FLAG_DUTY_INVEHICLE) )
	{
		new vid = GetPlayerVehicleID(playerid);
		if( vid > 0 )
		{
			if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_GROUP && Vehicle[vid][vehicle_owner] == Group[gid][group_uid] ) return 1;
		}
	}
	
	new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	if( d_id > -1 )
	{
		if( Door[d_id][door_owner_type] == DOOR_OWNER_TYPE_GROUP && Door[d_id][door_owner] == Group[gid][group_uid] ) return 1;
	}
	
	return 0;
}

stock GetGroupDotation(gid)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT group_dotation FROM `crp_groups` WHERE `group_uid` = %d", Group[gid][group_uid]));

	new dotation = cache_get_int(0, "group_dotation");
	
	cache_delete(result);
	
	return dotation;
}

stock _:GetGroupTag(gid)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT group_tag FROM `crp_groups` WHERE `group_uid` = %d", Group[gid][group_uid]));

	new tag[15];
	cache_get(0, "group_tag", tag);
	
	cache_delete(result);
	
	return tag;
}

stock GetGroupColor(gid)
{
	new Cache:result;
	result = mysql_query(g_sql, sprintf("SELECT group_color FROM `crp_groups` WHERE `group_uid` = %d", Group[gid][group_uid]));
	
	new color = cache_get_int(0, "group_color");
	
	cache_delete(result);
	
	return color;
}

stock GetPlayerSubgroupByOwner(playerid, g_uid)
{
	for(new i;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] != -1 )
		{
			if( Group[pGroup[playerid][i][pg_id]][group_parent_uid] == g_uid ) return i;
		}
	}
	
	return -1;
}

stock GiveGroupMoney(gid, amount)
{
	Group[gid][group_bank_money] += amount;
	mysql_pquery(g_sql, sprintf("UPDATE `crp_groups` SET `group_cash` = %d WHERE `group_uid` = %d", Group[gid][group_bank_money], Group[gid][group_uid]));
}

stock GiveGroupPoints(gid, amount)
{
	Group[gid][group_activity_points] += amount;
	mysql_pquery(g_sql, sprintf("UPDATE `crp_groups` SET `group_activity` = %d WHERE `group_uid` = %d", Group[gid][group_activity_points], Group[gid][group_uid]));
}

stock SendGroupOOC(playerid, slot, text[], bool:to_subs = false)
{
	slot -= 1;
	if( pGroup[playerid][slot][pg_id] == -1 ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	if( isnull(text) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_OOC) ) return SendGuiInformation(playerid, "Informacja", "Ta grupa nie posiada uprawnieñ do u¿ywania czatu ooc.");
	if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_CHAT) ) return GameTextForPlayer(playerid, "~r~Nie masz uprawnien do czatu", 4000, 3);
	if( !Group[gid][group_ooc] ) return SendGuiInformation(playerid, "Informacja", "Lider tej grupy zablokowa³ mo¿liwoœæ pisania na czacie ooc.");
	
	new string[160];
	format(string, sizeof(string), "%s", BeautifyString(text, true, false, false));
	
	PlayerLog(sprintf("[ooc:group] %s %s", string, GroupLogLink(Group[gid][group_uid])), pInfo[playerid][player_id], "chat");

	new str[250], gcolor = GetGroupColor(gid);
	foreach(new p : Player)
	{
		new pslot = GetPlayerGroupSlot(p, gid);
		if( to_subs && pslot == -1 )
		{
			if( Group[gid][group_parent_uid] == 0 ) pslot = GetPlayerSubgroupByOwner(p, Group[gid][group_uid]);
			else 
			{
				pslot = GetPlayerGroupSlot(p, GetGroupByUid(Group[gid][group_parent_uid]));
				if( pslot == -1 ) pslot = GetPlayerSubgroupByOwner(p, Group[gid][group_parent_uid]);
			}
		}
		if( pslot == -1 ) continue;
		
		switch( pslot )
		{
			case 0: if( PlayerHasTog(p, TOG_G1) ) continue;
			case 1: if( PlayerHasTog(p, TOG_G2) ) continue;
			case 2: if( PlayerHasTog(p, TOG_G3) ) continue;
			case 3: if( PlayerHasTog(p, TOG_G4) ) continue;
			case 4: if( PlayerHasTog(p, TOG_G5) ) continue;
		}
		
		if( to_subs ) format(str, sizeof(str), "@@%d %s (( %d. %s: %s ))", ((pGroup[p][pslot][pg_id] == gid) ? (pslot+1) : (0)), GetGroupTag(gid), playerid, pInfo[playerid][player_name], string);
		else format(str, sizeof(str), "@%d %s (( %s: %s ))", pslot+1, GetGroupTag(gid), pInfo[playerid][player_name], string);

		extended_SendClientMessage(p, gcolor, str);
	}
	
	pInfo[playerid][player_last_group_slot_chat] = slot+1;
	
	return 1;
}

stock SendGroupIC(playerid, slot, text[])
{
	slot -= 1;
	if( pGroup[playerid][slot][pg_id] == -1 ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	if( isnull(text) ) return PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
	
	
	new gid = pGroup[playerid][slot][pg_id];
	if( !GroupHasFlag(gid, GROUP_FLAG_IC) ) return SendGuiInformation(playerid, "Informacja", "Ta grupa nie posiada uprawnieñ do u¿ywania czatu ooc.");
	if( !WorkerHasFlag(playerid, slot, WORKER_FLAG_CHAT) ) return GameTextForPlayer(playerid, "~r~Nie masz uprawnien do czatu", 4000, 3);
	
	new string[160];
	format(string, sizeof(string), BeautifyString(text, true, true, false));
	
	new str[200], gcolor = GetGroupColor(gid);
	foreach(new p : Player)
	{
		new pslot = GetPlayerGroupSlot(p, gid);
		if( pslot == -1 ) continue;
		
		switch( pslot )
		{
			case 0: if( PlayerHasTog(playerid, TOG_G1) ) continue;
			case 1: if( PlayerHasTog(playerid, TOG_G2) ) continue;
			case 2: if( PlayerHasTog(playerid, TOG_G3) ) continue;
			case 3: if( PlayerHasTog(playerid, TOG_G4) ) continue;
			case 4: if( PlayerHasTog(playerid, TOG_G5) ) continue;
		}
		
		format(str, sizeof(str), "!%d %s ** %s: %s**", pslot+1, GetGroupTag(gid), pInfo[playerid][player_name], string);
		extended_SendClientMessage(p, gcolor, str);
	}
	
	PlayerLog(sprintf("[ic:group] %s %s", text, GroupLogLink(Group[gid][group_uid])), pInfo[playerid][player_id], "chat");

	ProxMessage(playerid, text, PROX_RADIO);
	pInfo[playerid][player_last_group_slot_chat] = slot+1;
	
	return 1;
}

stock ShowGroupsList(playerid)
{
	TextDrawShowForPlayer(playerid, GroupsListStaticHeader);
	for(new i=0;i<5;i++)
	{
		if( pGroup[playerid][i][pg_id] > -1 )
		{
			new gid = pGroup[playerid][i][pg_id];
			if( pGroup[playerid][i][pg_duty] ) PlayerTextDrawSetString(playerid, pInfo[playerid][GroupsListRow][i], sprintf("~g~%d       %s (%d)", i+1, Group[gid][group_name], Group[gid][group_uid]));
			else PlayerTextDrawSetString(playerid, pInfo[playerid][GroupsListRow][i], sprintf("%d       %s (%d)", i+1, Group[gid][group_name], Group[gid][group_uid]));
			PlayerTextDrawShow(playerid, pInfo[playerid][GroupsListRow][i]);
			for(new y=0;y<5;y++) PlayerTextDrawShow(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+y]);
		}
	}
	
	SelectTextDraw(playerid, 0x750000FF);
	
	pInfo[playerid][player_group_list_showed] = true;
}

stock HideGroupsList(playerid)
{
	TextDrawHideForPlayer(playerid, GroupsListStaticHeader);
	for(new i=0;i<5;i++)
	{
		PlayerTextDrawHide(playerid, pInfo[playerid][GroupsListRow][i]);
		for(new y=0;y<5;y++) PlayerTextDrawHide(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+y]);
	}
	
	CancelSelectTextDraw(playerid);
	
	pInfo[playerid][player_group_list_showed] = false;
}

stock ShowGroupDutyPane(playerid, slot)
{
	new hour, minute, second;
	gettime(hour, minute, second);
	
	new gid = pGroup[playerid][slot][pg_id];
	
	PlayerTextDrawSetString(playerid, pInfo[playerid][DashboardPane][1], sprintf("Sluzba: ~b~grupa                    ~y~%s (UID: %d)~n~~n~~w~Rozpoczeto: ~g~%02d:%02d               ~w~Ranga: ~r~%s", Group[gid][group_name], Group[gid][group_uid], hour, minute, pGroup[playerid][slot][pg_rank_title]));
	PlayerTextDrawShow(playerid, pInfo[playerid][DashboardPane][0]);
	PlayerTextDrawShow(playerid, pInfo[playerid][DashboardPane][1]);
	
	defer HideGroupDutyPane[4000](playerid);
}

function OnGroupProductsListLoaded(playerid)
{
	if( cache_get_rows() )
	{
		DynamicGui_Init(playerid);
		
		new str[1024], listname[100];
		format(str, sizeof(str), "ID\tNazwa produktu\tCena szt.\n");

		for(new i;i<cache_get_rows();i++)
		{
			cache_get(i, "product_listname", listname);
			
			format(str, sizeof(str), "%s%d\t%s\t$%d\n", str, cache_get_int(i, "product_uid"), listname, cache_get_int(i, "product_price"));
			DynamicGui_AddRow(playerid, DG_ORDER_PRODUCTS_CATEGORY_ITEM, cache_get_int(i, "product_uid"));
		}
		
		ShowPlayerDialog(playerid, DIALOG_ORDER_PRODUCTS, DIALOG_STYLE_TABLIST_HEADERS, "Produkty dostêpne dla tej grupy:", str, "Wybierz", "Zamknij");
	}
	else
	{
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
		if( pInfo[playerid][player_phone_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_phone_object_index]);
	
		SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ta grupa nie posiada ¿adnych dostêpnych produktów do zamawiania.");
	}
}

function OnGroupProductsOrderDataLoaded(playerid, amount)
{
	new price = cache_get_int(0, "product_price"), 
		type = cache_get_int(0, "product_type"), 
		model = cache_get_int(0, "product_model"), 
		value1 = cache_get_int(0, "product_value1"), 
		value2 = cache_get_int(0, "product_value2");
	
	new did = GetDoorByUid(GetPlayerVirtualWorld(playerid));
	new gid = GetGroupByUid(Door[did][door_owner]); 

	if( Group[gid][group_bank_money] < amount * price ) return ShowPlayerDialog(playerid, DIALOG_ORDER_PRODUCTS_ITEM_AMOUNT, DIALOG_STYLE_INPUT, "Zamawianie produktów » Iloœæ", sprintf("W poni¿szym polu podaj iloœæ jak¹ chcesz zamówiæ:\n\n"HEX_COLOR_LIGHTER_RED"Na koncie nie ma wystarczaj¹co pieniêdzy ($%d)", amount * price), "Dalej", "Anuluj");
	
	GiveGroupMoney(gid, -(amount * price));
	
	// TODO Obs³uga paczek, praca dorwycza kuriera ;)
	new Cache:result;
	result = mysql_query(g_sql, sprintf("INSERT INTO `crp_packages` VALUES (null, %d)", Door[did][door_uid]));
	new package = cache_insert_id();
	cache_delete(result);

	new i_name[40];
	strcopy(i_name, pInfo[playerid][player_order_name], 40);
	SetPlayerCellPhoneVisuals(playerid, false);
	
	Item_Create(ITEM_OWNER_TYPE_DOOR_WAREHOUSE, Door[did][door_uid], type, value1, value2, i_name, model, amount, price);
	SendGuiInformation(playerid, "Zamówienie z³o¿one", sprintf("Twoje zamówienie zosta³o pomyœlnie z³o¿one i przyjête do realizacji.\n\n"HEX_COLOR_WHITE"Nr zamówienia: "HEX_COLOR_HONEST"\t%d\n"HEX_COLOR_WHITE"Produkt: "HEX_COLOR_HONEST"\t%s (x%d)\n"HEX_COLOR_WHITE"Wartoœæ: "HEX_COLOR_HONEST"\t$%d", package, i_name, amount, amount * price));

	return 1;
}