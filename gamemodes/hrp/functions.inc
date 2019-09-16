stock LoadConfiguration()
{
	Code_ExTimer_Begin(LoadConfiguration);
	
	if( !dini_Exists(CONFIG_FILEPATH) ) 
	{
		dini_Create(CONFIG_FILEPATH);
		
		dini_Set(CONFIG_FILEPATH, "mysql_hostname", "127.0.0.1");
		dini_Set(CONFIG_FILEPATH, "mysql_username", "admin");
		dini_Set(CONFIG_FILEPATH, "mysql_password", "admin");
		dini_Set(CONFIG_FILEPATH, "mysql_database", "red");
		dini_Set(CONFIG_FILEPATH, "run_mode", "2");
		dini_Set(CONFIG_FILEPATH, "remote_download_url", "");
	}
	
	dini_Get(CONFIG_FILEPATH, "mysql_hostname", Setting[setting_mysql_hostname]);
	dini_Get(CONFIG_FILEPATH, "mysql_username", Setting[setting_mysql_username]);
	dini_Get(CONFIG_FILEPATH, "mysql_password", Setting[setting_mysql_password]);
	dini_Get(CONFIG_FILEPATH, "mysql_database", Setting[setting_mysql_database]);
	dini_Get(CONFIG_FILEPATH, "remote_download_url", Setting[setting_remote_download_url]);

	if( strfind(Setting[setting_remote_download_url], "%s") == -1 ) {
		Setting[setting_remote_download_url][0] = EOS;
	}

	Setting[setting_run_mode] = dini_Int(CONFIG_FILEPATH, "run_mode");
	Setting[setting_server_hour] = -1;
	Setting[setting_server_weather] = 1;
	
	SetGameModeText("honest "HRP_HUMAN_VERSION"");
	
	printf("[honest] Wczytano konfiguracjê serwera [czas wykonania: %d ms]", Code_ExTimer_End(LoadConfiguration));
}

stock CreateLoggers()
{
	new y, m, d;
	getdate(y,m,d);

	logFile = CreateLog(sprintf("players/%02d%02d%02d", y, m, d), INFO, false);
}

stock ConnectMysql()
{
	Code_ExTimer_Begin(ConnectMysql);
	
	new MySQLOpt:options = mysql_init_options();
 	
 	mysql_log(ERROR | WARNING); //logs everything (errors, warnings and debug messages)
	mysql_set_option(options, AUTO_RECONNECT, true); //disable auto-reconnect
	mysql_set_option(options, POOL_SIZE, 5); //disable connection pool (and thus mysql_pquery)
	 
	g_sql = mysql_connect(Setting[setting_mysql_hostname], Setting[setting_mysql_username], Setting[setting_mysql_password], Setting[setting_mysql_database], options);

	if( mysql_errno() != 0 )
	{
		new err[100];
		mysql_error(err);

		printf("[honest] B³¹d po³¹czenia z baz¹ danych [czas trwania: %d ms]", Code_ExTimer_End(ConnectMysql));
		printf("[honest] %s", err);
		return false;
	}

	mysql_set_charset("cp1250");
	
	printf("[honest] Po³¹czano z baz¹ danych %s@%s [czas trwania: %d ms]", Setting[setting_mysql_username], Setting[setting_mysql_hostname], Code_ExTimer_End(ConnectMysql));
	return true;
}

stock LoadGlobalSpawns()
{
	new Cache:result;
	result = mysql_query(g_sql, "SELECT * FROM crp_gspawns");

	for(new i; i < cache_get_rows(); i++)
	{
		if( i >= MAX_GLOBAL_SPAWNS ) break;
				
		GlobalSpawn[i][gspawn_id] = cache_get_int(i, "gspawn_uid");
		GlobalSpawn[i][gspawn_pos][0] = cache_get_float(i, "gspawn_posx");
		GlobalSpawn[i][gspawn_pos][1] = cache_get_float(i, "gspawn_posy");
		GlobalSpawn[i][gspawn_pos][2] = cache_get_float(i, "gspawn_posz");
		GlobalSpawn[i][gspawn_pos][3] = cache_get_float(i, "gspawn_posa");
		
		Iter_Add(GlobalSpawns, i);
		
		// Textdraw na spawnie
		GlobalSpawn[i][gspawn_label] = CreateDynamic3DTextLabel("", COLOR_WHITE, GlobalSpawn[i][gspawn_pos][0], GlobalSpawn[i][gspawn_pos][1], GlobalSpawn[i][gspawn_pos][2], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, 0, 0, 0);
		Streamer_RemoveArrayData(STREAMER_TYPE_3D_TEXT_LABEL, GlobalSpawn[i][gspawn_label], E_STREAMER_PLAYER_ID, 0);
		
		new str[50];
		format(str, sizeof(str), "Globalny spawn (ID: %d)", i);
		UpdateDynamic3DTextLabelText(GlobalSpawn[i][gspawn_label], 0xFFFFFF80, str);
	}
	
	printf("[honest] Proces wczytywania globalnych spawnów zakoñczony pomyœlnie [wczytanych globalnych spawnów: %d]", Iter_Count(GlobalSpawns));

	cache_delete(result);
}

stock LoadObjects()
{
	new loaded = LoadObject();
	
	printf("[honest] Proces wczytywania obiektów zakoñczony pomyœlnie [wczytanych obiektów: %d]", loaded);
}

stock LoadGroups()
{
	new Cache:result;
	result = mysql_query(g_sql, "SELECT * FROM crp_groups ORDER BY group_owner ASC");
	for(new i;i<cache_get_rows();i++)
	{
		Group[i][group_uid] = cache_get_int(i, "group_uid");
		Group[i][group_type] = cache_get_int(i, "group_type");
		Group[i][group_parent_uid] = cache_get_int(i, "group_owner");
		Group[i][group_flags] = cache_get_int(i, "group_flags");

		new gid = GetGroupByUid(Group[i][group_parent_uid]);
		if( gid != -1 )
		{
			Group[i][group_flags] = Group[gid][group_flags];
		}
		
		Group[i][group_bank_money] = cache_get_int(i, "group_cash");
		cache_get(i, "group_name", Group[i][group_name]);
		
		Group[i][group_activity_points] = cache_get_int(i, "group_activity");
		Group[i][group_value1] = cache_get_int(i, "group_value1");
		Group[i][group_value2] = cache_get_int(i, "group_value2");
		
		Group[i][group_ooc] = cache_get_bool(i, "group_ooc");
		
		if(gettime()-cache_get_int(i, "group_last_payment") > 1209600 && Group[i][group_type] == GROUP_TYPE_PD || Group[i][group_type] == GROUP_TYPE_FBI)
		{
			//Op³ata za ubezpieczenie pracowników co 2 tygodnie
			mysql_tquery(g_sql, sprintf("SELECT COUNT(*) as amount FROM crp_char_groups WHERE group_belongs = %d", Group[i][group_uid]), "OnGroupPayment", "i", Group[i][group_uid]);
		}

		Iter_Add(Groups, i);
	}

	printf("[honest] Proces wczytywania grup zakoñczony pomyœlnie [wczytanych grup: %d]", cache_get_rows());

	cache_delete(result);
}

stock LoadDoors()
{
	new loaded = LoadDoor();
	
	printf("[honest] Proces wczytywania drzwi zakoñczony pomyœlnie [wczytanych drzwi: %d]", loaded);
}

stock LoadAreas()
{
	new loaded = LoadArea();
	
	printf("[honest] Proces wczytywania stref zakoñczony pomyœlnie [wczytanych stref: %d]", loaded);
}

stock LoadLabels()
{
	new loaded = LoadLabel();
	
	printf("[honest] Proces wczytywania tekstów 3d zakoñczony pomyœlnie [wczytanych tekstów 3d: %d]", loaded);
}

stock LoadVehicles()
{
	new loaded = LoadVehicle(sprintf("WHERE `vehicle_ownertype` = %d OR vehicle_spawn_restart = 1", VEHICLE_OWNER_TYPE_GROUP));
	
	printf("[honest] Proces wczytywania pojazdów zakoñczony pomyœlnie [wczytanych pojazdów: %d]", loaded);
}

stock LoadItems()
{
	new loaded = LoadItem(sprintf("WHERE `item_ownertype` = %d OR `item_ownertype` = %d OR `item_ownertype` = %d OR `item_ownertype` = %d OR `item_ownertype` = %d", ITEM_OWNER_TYPE_GROUND, ITEM_OWNER_TYPE_DOOR, ITEM_OWNER_TYPE_DOOR_WAREHOUSE, ITEM_OWNER_TYPE_VEHICLE, ITEM_OWNER_TYPE_VEHICLE_COMPONENT));
	
	printf("[honest] Proces wczytywania przedmiotów zakoñczony pomyœlnie [wczytanych przedmiotów: %d]", loaded);
}

stock LoadSkins()
{
	new Cache:result;
	result = mysql_query(g_sql, "SELECT * FROM crp_skins WHERE skin_group = 0 AND skin_price > 0");

	for(new i;i<cache_get_rows();i++)
	{
		Skin[i][skin_value] = cache_get_int(i, "skin_id");
		Skin[i][skin_price] = cache_get_int(i, "skin_price");
		
		if( cache_get_int(i, "skin_sex") == 0 ) Iter_Add(Skins[0], i);
		else Iter_Add(Skins[1], i);
	}

	printf("[honest] Proces wczytywania skinów zakoñczony pomyœlnie [wczytanych skinów: %d]", cache_get_rows());

	cache_delete(result);
}

stock LoadBuses()
{
	new Cache:result;
	result = mysql_query(g_sql, "SELECT * FROM crp_buses");
	for(new i;i<cache_get_rows();i++)
	{
		Bus[i][bus_uid] = cache_get_int(i, "bus_uid");
		Bus[i][bus_objectid] = INVALID_OBJECT_ID;
		Bus[i][bus_ratio] = cache_get_float(i, "bus_ratio");
		cache_get(i, "bus_name", Bus[i][bus_name]);
		
		Iter_Add(Buses, i);
	}

	printf("[honest] Proces wczytywania przystanków zakoñczony pomyœlnie [wczytanych przystanków: %d]", cache_get_rows());

	cache_delete(result);
}

stock LoadCustomModels()
{
    new models[2], Cache:result;
    result = mysql_query(g_sql, "SELECT * FROM crp_custom_models WHERE type = 'object'");
    
    new dff[64], txd[64], type[10];
    
    for(new i;i<cache_get_rows();i++)
    {
    	cache_get(i, "type", type);
    	cache_get(i, "dff", dff);
    	cache_get(i, "txd", txd);

    	if( !strcmp(type, "skin") ) 
    	{
    		AddCharModel(cache_get_int(i, "template"), cache_get_int(i, "gameid"), dff, txd);
    		models[0]++;
    	}
    	else if( !strcmp(type, "object") )
    	{
    		AddSimpleModel(-1, cache_get_int(i, "template"), cache_get_int(i, "gameid"), dff, txd);
    		models[1]++;
    	}
    }

    cache_delete(result);

    printf("[honest] Wczytano %d modeli (%d skinów, %d obiektów)", models[0]+models[1], models[0], models[1]);
}

stock LoadActors()
{
	new loaded = LoadActor();
	
	printf("[honest] Proces wczytywania aktorów zakoñczony pomyœlnie [wczytanych aktorów: %d]", loaded);
}

stock ProcessPayday()
{
	new str[600];
	strcat(str, "UPDATE crp_characters c SET c.char_bankcash = c.char_bankcash + (SELECT sum(cg.group_payment) as cash FROM crp_char_groups cg LEFT JOIN crp_groups cgr ON cg.group_belongs = cgr.group_uid WHERE cg.char_uid = c.char_uid AND cg.group_payday = 1 AND cgr.group_activity >= 500 GROUP BY cg.char_uid)");
	strcat(str, " WHERE (SELECT sum(cg.group_payment) as cash FROM crp_char_groups cg LEFT JOIN crp_groups cgr ON cg.group_belongs = cgr.group_uid WHERE cg.char_uid = c.char_uid AND cg.group_payday = 1 AND cgr.group_activity >= 500 GROUP BY cg.char_uid) > 0");
	mysql_tquery(g_sql, str);
	mysql_tquery(g_sql, "UPDATE crp_char_groups SET group_payday = 0");
	mysql_tquery(g_sql, "UPDATE crp_groups SET group_activity = 0");
}

stock getBlockTypeName(block_type, dest[])
{
	for(new i;i<sizeof(BlockBit);i++)
	{
		if(BlockBit[i] == block_type)
		{
			strcopy(dest, BlockName[i]);
			return;
		}
	}

	strcopy(dest, "BEZ_NAZWY");
	return;
}

task ServerTask[500]()
{
	if( players_task_last_execution == 0 ) players_task_last_execution = gettime();
	
	if( Setting[setting_lsn_ad_finish_time] > 0 && Setting[setting_lsn_ad_finish_time] <= gettime() )
	{
		Setting[setting_lsn_ad_finish_time] = 0;
		TextDrawSetString(LSNtd, "				    		    ~>~ ~w~Brak sygnalu nadawania.");
	}

	new hour, minute;
	gettime(hour, minute);
	
	SetWeather(2);

	if( Setting[setting_server_hour] == -1 )
	{
		hour += 1;
		if( hour > 24 ) hour = hour-24;
		SetWorldTime(hour);
	}
	
	gettime(hour, minute);

	if(hour != knocks_bells && minute == 0)
	{
		knocks_bells = hour;
		if(hour == 0) hour = 24;
		SendClientMessageToAll(0x9B91ECFF, sprintf("* Z ratusza wydobywa siê dŸwiêk dzwonów wybijaj¹cych %d godzinê.", hour));
	}

	if(hour == 0 && minute == 0 && log_nextday)
	{
		log_nextday = !log_nextday;

		new y, m, d;
		getdate(y, m, d);

		DestroyLog(logFile);
		CreateLoggers();
	}

	if( hour == 3 && minute == 0 )
	{
		ProcessPayday();
	
		printf("[nocny restart] %dh %dmin.", hour, minute);
		
		foreach(new p : Player)
		{
			SavePlayer(p);
		}
		
		foreach(new v : Vehicles)
		{
			SaveVehicle(v);
		}
		
		SendRconCommand("changemode hrp");
	}
	
	// -- firedep.inc -- //
	ProcessRandomFires();

	if( gettime() - antycrasher_task_last_execution >= 5 )
	{
		antycrasher_task_last_execution = gettime();
		mysql_pquery(g_sql, sprintf("UPDATE anty_crasher SET `update` = %d WHERE id = 1", gettime()));

		// dezaktywujemy blokady ktore przeminely
		mysql_pquery(g_sql, sprintf("UPDATE crp_penalties SET penalty_deactivate = 1 WHERE penalty_type = %d AND penalty_end != 0 AND penalty_end <= %d", PENALTY_TYPE_BLOCK, gettime()));
	}
}