#define GymType:: gym_type_
#define gym_type_BENCH 1
#define gym_type_DUMBBELL 2

#define GYM_TRAIN_DELAY 15*60*60

stock GymStart(playerid, objectid, type)
{
	if( gettime() - pInfo[playerid][player_last_train] < GYM_TRAIN_DELAY ) return Alert(playerid, ALERT_TYPE_NEGATIVE, sprintf("Kolejne cwiczenie mozesz zaczac dopiero za %dh.", floatround(((15*60*60)-(gettime() - pInfo[playerid][player_last_train]))/3600)));
	if( pInfo[playerid][player_is_gym_training] ) return Alert(playerid, ALERT_TYPE_NEGATIVE, "Rozpoczales juz cwiczenie na silowni.");
	if( pInfo[playerid][player_health] < 50.0 ) return Alert(playerid, ALERT_TYPE_NEGATIVE, "Aby rozpoczac cwiczenie musisz miec wiecej niz ~r~50 HP~w~.");
	if( Object[objectid][object_is_used] ) return Alert(playerid, ALERT_TYPE_NEGATIVE, "Na tym przyrz¹dzie ktoœ ju¿ æwiczy.");

	TogglePlayerControllable(playerid, 0);

	new Float:pos[3], Float:rot;
	GetPointInAngleOfObject(objectid, pos[0], pos[1], pos[2], 1.0, 180.0);
	Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_R_Z, rot);

	SetPlayerPos(playerid, pos[0], pos[1], pos[2]+0.5);
	SetPlayerFacingAngle(playerid, rot);

	GetPointInAngleOfObject(objectid, pos[0], pos[1], pos[2], 3.5, 170.0);
	SetPlayerCameraPos(playerid, pos[0], pos[1], pos[2]+3.0);

	Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_X, pos[0]);
	Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_Y, pos[1]);
	Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_Z, pos[2]);
	SetPlayerCameraLookAt(playerid, pos[0], pos[1], pos[2]+1.0);

	pInfo[playerid][player_is_gym_training] = true;
	pInfo[playerid][player_gym_object] = objectid;
	pInfo[playerid][player_gym_reps] = 0;

	Object[objectid][object_is_used] = true;

	if( type == GymType::BENCH )
	{
		InfoboxRight(playerid, 3, "Uzyto klawisz ~y~~k~~CONVERSATION_YES~~w~: Znaleziono ~y~laweczke~w~, automatycznie rozpoczeto interakcje.");
		ApplyAnimation( playerid, "benchpress", "gym_bp_geton", 1, 0, 0, 0, 1, 0, 1 );
	}
	else if( type == GymType::DUMBBELL )
	{
		InfoboxRight(playerid, 3, "Uzyto klawisz ~y~~k~~CONVERSATION_YES~~w~: Znaleziono ~y~hantle~w~, automatycznie rozpoczeto interakcje.");
		ApplyAnimation(playerid, "FREEWEIGHTS", "gym_free_pickup", 4.0, 0, 0, 0, 1, 0, 1);
	}

	defer GymTrainStart[3800](playerid);

	return 1;
}

stock GymStop(playerid)
{
	switch(Object[pInfo[playerid][player_gym_object]][object_type])
	{
		case OBJECT_TYPE_GYM_BENCH:
		{
			ApplyAnimation( playerid, "benchpress", "gym_bp_getoff", 1, 0, 0, 0, 1, 0, 1 );
			PlayerTextDrawHide(playerid, pInfo[playerid][gymTd]);

			pInfo[playerid][player_gym_can_workout] = false;
			defer BenchBeforeStop[2800](playerid);
			defer BenchStop[3800](playerid);
		}
	}

	Object[pInfo[playerid][player_gym_object]][object_is_used] = false;
}

stock Gym_OnPlayerKey(playerid, newkeys, oldkeys)
{
	if(pInfo[playerid][player_is_gym_training])
	{
		if( PRESSED(KEY_HANDBRAKE) || PRESSED(KEY_FIRE) )
		{
			if( !pInfo[playerid][player_gym_can_workout] ) return;
			GymStop(playerid);
		}

		if( PRESSED(KEY_JUMP) )
		{
			if( !pInfo[playerid][player_gym_can_workout] ) return;

			if(pInfo[playerid][player_gym_rep_next_start] > GetTickCount())
			{
				GameTextForPlayer(playerid, "~r~zbyt wczesnie na kolejne potworzenie", 1000, 3);
				return;
			}

			pInfo[playerid][player_gym_is_pushing] = true;

			ApplyAnimation( playerid, "benchpress", "gym_bp_up_A", 1, 0, 0, 0, 1, 0, 1 );
			pInfo[playerid][player_gym_rep_start_tick] = GetTickCount();
		}

		if( RELEASED(KEY_JUMP) )
		{
			if( !pInfo[playerid][player_gym_can_workout] ) return;
			if( !pInfo[playerid][player_gym_is_pushing] ) return;
			if( GetTickCount() - pInfo[playerid][player_gym_rep_start_tick] < 2100 )
			{
				GameTextForPlayer(playerid, "~r~powotrzenie nie skonczone", 1000, 3);
			}
			else
			{
				// wycisnieta do konca
				Gym_OnPlayerRepFinish(playerid);
			}

			pInfo[playerid][player_gym_is_pushing] = false;

			pInfo[playerid][player_gym_rep_next_start] = GetTickCount() + 1300;
			ApplyAnimation( playerid, "benchpress", "gym_bp_down", 1, 0, 0, 0, 1, 0, 1 );
		}
	}
}

stock Gym_OnPlayerRepFinish(playerid)
{
	pInfo[playerid][player_gym_reps]++;

	if(pInfo[playerid][player_gym_reps] == 100)
	{
		if(pInfo[playerid][player_karnet])
		{
			new sila = 5;
			if(pInfo[playerid][player_suplement_type] == SUPLEMENT_TYPE_MOREPOWER) sila += 2;
			SendGuiInformation(playerid, "Informacja", sprintf("Trening si³owy zosta³ pomyœlnie ukoñczony. Twoja si³a wzros³a o %dJ.\nNastêpny trening mo¿esz rozpocz¹æ za 15h.", sila));
			pInfo[playerid][player_karnet] = false;
			pInfo[playerid][player_strength] += sila;
			pInfo[playerid][player_last_train] = gettime();
			pInfo[playerid][player_suplement_type] = -1;

			mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_strength = %d, char_last_train = %d WHERE char_uid = %d", pInfo[playerid][player_strength], pInfo[playerid][player_last_train], pInfo[playerid][player_id]));			
		}

		SetPlayerHealth(playerid, 35);

		GymStop(playerid);
	}

	UpdateGymTextDraw(playerid);
}

stock Gym_OnPlayerUseKarnet(playerid)
{
	if(pInfo[playerid][player_is_gym_training] && pInfo[playerid][player_gym_can_workout]) UpdateGymTextDraw(playerid);
}

stock Gym_OnPlayerUseSuple(playerid)
{
	if(pInfo[playerid][player_is_gym_training] && pInfo[playerid][player_gym_can_workout]) UpdateGymTextDraw(playerid);
}

stock UpdateGymTextDraw(playerid)
{
	new karnet[200], suple[100];
	
	// TODO HERE
	if(!pInfo[playerid][player_karnet]) format(karnet, sizeof(karnet), "~n~~n~~r~Nie posiadasz~w~ aktywnego karnetu silowego. Po wykonaniu 100 powtorzen Twoja sila nie wzrosnie.");
	else format(karnet, sizeof(karnet), "~n~~n~~g~Posiadasz~w~ aktywny karnet silowy. Po wykonaniu 100 powtorzen Twoja sila wzrosnie.");
	
	if(pInfo[playerid][player_suplement_type] == SUPLEMENT_TYPE_MOREPOWER)
	{
		format(suple, sizeof(suple), "~n~~n~~g~Aktywny suplement~w~: wiekszy przyrost (+2J)");
	}

	PlayerTextDrawSetString(playerid, pInfo[playerid][gymTd], sprintf("Powtorzen: ~y~%d~n~~n~~w~Przytrzymaj LSHIFT az do calkowitego wykonania powtorzenia 100 razy, aby zwiekszyc sile.~n~~n~Niektore narkotyki zwiekszaja szybkosc zdobywania sily, a inne czestotliwosc treningow.%s%s", pInfo[playerid][player_gym_reps], karnet, suple));
}

timer GymTrainStart[100](playerid)
{
	new slot = GetPlayerFreeAttachSlot(playerid);

	if( Object[pInfo[playerid][player_gym_object]][object_type] == OBJECT_TYPE_GYM_BENCH )
	{
		if(slot != -1)
		{
			SetPlayerAttachedObject(playerid, slot, 2913, 6);
			pInfo[playerid][player_gym_attach_slot] = slot;
		}
	}

	pInfo[playerid][player_gym_can_workout] = true;
	UpdateGymTextDraw(playerid);
	PlayerTextDrawShow(playerid, pInfo[playerid][gymTd]);
}

timer BenchBeforeStop[100](playerid)
{
	if(pInfo[playerid][player_gym_attach_slot] > -1) RemovePlayerAttachedObject(playerid, pInfo[playerid][player_gym_attach_slot]);
}

timer BenchStop[100](playerid)
{
	TogglePlayerControllable(playerid, 1);
	SetCameraBehindPlayer(playerid);

	pInfo[playerid][player_is_gym_training] = false;
	pInfo[playerid][player_gym_object] = -1;
}