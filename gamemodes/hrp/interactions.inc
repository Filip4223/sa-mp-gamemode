stock InteractionRequest(playerid)
{
	if( GetTickCount() - pInfo[playerid][player_last_intact_press] < 1200) return;

	pInfo[playerid][player_last_intact_press] = GetTickCount();

	new pretender_object = -1, pretender_actor = -1, pretender_vehicle = -1, Float:pretender_distance[3] = {2.0, 2.0, 2.0}, Float:distance;
	new Float:x, Float:y, Float:z;
	
    // getting look at pos
    new Float:fPX, Float:fPY, Float:fPZ, Float:fVX, Float:fVY, Float:fVZ, Float:fScale = 2.0;
    GetPlayerPos(playerid, fPX, fPY, fPZ);
    GetPlayerCameraFrontVector(playerid, fVX, fVY, fVZ);

    x = fPX + floatmul(fVX, fScale);
    y = fPY + floatmul(fVY, fScale);
    z = fPZ + 1.0 + floatmul(fVZ, fScale);
    // --------

    new objects[20];
    Streamer_GetAllVisibleItems(playerid, STREAMER_TYPE_OBJECT, objects);

    for(new o_id;o_id<sizeof(objects);o_id++)
    {
        if( objects[o_id] == INVALID_STREAMER_ID ) break;

        Streamer_GetDistanceToItem(x, y, z, STREAMER_TYPE_OBJECT, objects[o_id], distance);
        if( distance > 2.0 ) break;

        if( IsObjectInteractable(objects[o_id]) && GetPlayerVirtualWorld(playerid) == Object[objects[o_id]][object_vw] )
        {
            pretender_object = objects[o_id];
            pretender_distance[0] = distance;

            break;
        }
    }

    new actors[10];
    Streamer_GetAllVisibleItems(playerid, STREAMER_TYPE_ACTOR, actors);

    for(new a_id;a_id<sizeof(actors);a_id++)
    {
        if( actors[a_id] == INVALID_STREAMER_ID ) break;

        Streamer_GetDistanceToItem(x, y, z, STREAMER_TYPE_ACTOR, actors[a_id], distance);
        if( distance > 3.0 ) break;

        if( GetPlayerVirtualWorld(playerid) == Actor[actors[a_id]][actor_virtual] )
        {
            pretender_actor = actors[a_id];
            pretender_distance[1] = distance;

            break;
        }
    }

    new Float:vehicle_pos[3];
    foreach(new vid : Vehicles)
    {
        GetVehiclePos(vid, vehicle_pos[0], vehicle_pos[1], vehicle_pos[2]);
        distance = GetDistanceBetweenPoints(vehicle_pos[0], vehicle_pos[1], vehicle_pos[2], x, y, z);
        if( distance > 2.0 ) continue;

        if( GetVehicleVirtualWorld(vid) == GetPlayerVirtualWorld(playerid) ) 
        {
            if( distance <= pretender_distance[2] ) 
            {
                pretender_vehicle = vid;
                pretender_distance[2] = distance;
            }
        }
    }

    // now we should check which one is closer
    new pretender_type, pretender_id = -1, Float:pretender_dist = 5.0;
    if( pretender_object > -1 )
    {
        pretender_type = STREAMER_TYPE_OBJECT;
        pretender_id = pretender_object;
        pretender_dist = pretender_distance[0];
    }

    if( pretender_actor > -1 && pretender_distance[1] < pretender_dist )
    {
        pretender_type = STREAMER_TYPE_ACTOR;
        pretender_id = pretender_actor;
        pretender_dist = pretender_distance[1];
    }

    if( pretender_vehicle > -1 && pretender_distance[2] < pretender_dist )
    {
        pretender_type = 555;
        pretender_id = pretender_vehicle;
        pretender_dist = pretender_distance[2];
    }

    if( pretender_id > -1 )
    {
    	ProcessInteraction(playerid, pretender_type, pretender_id);
    }
    else
    {
        Alert(playerid, ALERT_TYPE_NEGATIVE, "W poblizu nie ma dostepnych interakcji");
    }
}

stock IsObjectInteractable(objectid)
{
	new otype = Object[objectid][object_type];
	if( otype == OBJECT_TYPE_GYM_BENCH || otype == OBJECT_TYPE_GYM_DUMBBELL || otype == OBJECT_TYPE_ATM || otype == OBJECT_TYPE_BUS ) return true;

	return false;
}

stock ProcessInteraction(playerid, type, itemid)
{
    if( type == STREAMER_TYPE_OBJECT )
    {
    	switch(Object[itemid][object_type])
    	{
    		case OBJECT_TYPE_GYM_BENCH:
    		{
    			GymStart(playerid, itemid, GymType::BENCH);
    		}

            case OBJECT_TYPE_GYM_DUMBBELL:
            {
                GymStart(playerid, itemid, GymType::DUMBBELL);
            }

            case OBJECT_TYPE_ATM:
            {
                cmd_bankomat(playerid, "");
            }

            case OBJECT_TYPE_BUS:
            {
                cmd_bus(playerid, "");
            }
    	}
    }
    else if( type == STREAMER_TYPE_ACTOR )
    {
        switch(Actor[itemid][actor_type])
        {
            case ACTOR_TYPE_GOV:
            {
                DynamicGui_Init(playerid);

                new str[512];
                strcat(str, "{8380df}Wybierz dokument, który chcesz wyrobiæ z listy poni¿ej.\n");
                DynamicGui_AddBlankRow(playerid);

                strcat(str, "{AFAFAF}-\n");
                DynamicGui_AddBlankRow(playerid);

                strcat(str, "{333333} » {FFFFFF}1.      Wyrób prawo jazdy\n");
                DynamicGui_AddRow(playerid, DG_GOV_DRIVER_LICENSE);

                strcat(str, "{333333} » {FFFFFF}2.      Wyrób dowód osobisty\n");
                DynamicGui_AddRow(playerid, DG_GOV_ID);

                if(pInfo[playerid][player_job] == 0) {
                    strcat(str, "{333333} » {FFFFFF}3.      Zatrudnij siê w pracy dorywczej.\n");
                    DynamicGui_AddRow(playerid, DG_GOV_JOB);
                }
                else {
                    strcat(str, "{333333} » {FFFFFF}3.      Zwolnij siê z pracy dorywczej.\n");
                    DynamicGui_AddRow(playerid, DG_GOV_JOB);
                }

                strcat(str, "{333333} » {FFFFFF}4.      Zap³aæ czynsz za budynek.\n");
                DynamicGui_AddRow(playerid, DG_GOV_RENT);

                strcat(str, "{333333} » {FFFFFF}5.      Zarejestruj dzia³alnoœæ gospodarcz¹.\n");
                DynamicGui_AddRow(playerid, DG_GOV_BUSINESS);

                ShowPlayerDialog(playerid, DIALOG_GOV, DIALOG_STYLE_TABLIST, "Wyrabianie dokumentów", str, "Wybierz", "Anuluj");
            }
            case ACTOR_TYPE_INSURANCE:
            {
                if(IsPlayerInPublicGroup(playerid))
                {
                    SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Ubezpieczenie funkcjonariuszy s³u¿b porz¹dkowych w wersji {FFFFFF}PRO "HEX_COLOR_SAMP"pokrywa urz¹d miasta.");
                    return;
                }

                DynamicGui_Init(playerid);

                new str[256];
                strcat(str, "{8380df}Zaznacz ubezpieczenie, które chcesz zakupiæ.\n");
                DynamicGui_AddBlankRow(playerid);

                strcat(str, "{AFAFAF}-\n");
                DynamicGui_AddBlankRow(playerid);

                if(pInfo[playerid][player_insurance] == INSURANCE_TYPE_NONE)
                {
                    strcat(str, "{333333} » {FFFFFF}1.      Ubezpieczenie SMART\n");
                    DynamicGui_AddRow(playerid, DG_INSURANCE_SMART);

                    strcat(str, "{333333} » {FFFFFF}2.      Ubezpieczenie STANDARD\n");
                    DynamicGui_AddRow(playerid, DG_INSURANCE_STD);

                    strcat(str, "{333333} » {FFFFFF}2.      Ubezpieczenie PRO\n");
                    DynamicGui_AddRow(playerid, DG_INSURANCE_PRO);
                }
                else
                {
                    strcat(str, "{333333} » {FFFFFF}1.      Zrezygnuj z ubezpieczenia\n");
                    DynamicGui_AddRow(playerid, DG_INSURANCE_CANCEL);                   
                }
                ShowPlayerDialog(playerid, DIALOG_INSURANCE, DIALOG_STYLE_TABLIST, "Ubezpieczenie", str, "Wybierz", "Anuluj");
            }
        }
    }
    else if( type == 555 )
    {
        InfoboxRight(playerid, 2, "Akcja z pojazdem");
    }
}

stock GetClosestObjectType(playerid, object_mod)
{
    new object_id,
        Float:prevdist = 5.0, OID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;

    GetPlayerPos(playerid, PosX, PosY, PosZ);
     
    return OID;
}