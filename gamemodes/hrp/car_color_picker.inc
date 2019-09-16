  // car color picker

stock ShowCarColorPickerForPlayer(playerid)
{
	for(new i=0;i<4;i++) PlayerTextDrawShow(playerid, pInfo[playerid][CarColorPicker][i]);
	SelectTextDraw(playerid, 0x85000DFF);

	new vid = GetVehicleByUid(pInfo[playerid][player_carpaint_vuid]);

	pInfo[playerid][player_choosing_carcolor_cur] = 0;

	ChangeVehicleColor(vid, pInfo[playerid][player_choosing_carcolor_cur], Vehicle[vid][vehicle_color][1]);

	PlayerTextDrawSetString(playerid, pInfo[playerid][CarColorPicker][2], sprintf("~b~~h~~h~%d~n~~w~Aktualny", pInfo[playerid][player_choosing_carcolor_cur]));

	new Float:camerapos[3];
	GetPointInAngleOfVehicle(vid, camerapos[0], camerapos[1], camerapos[2], 6.0, 50.0);

	camerapos[2] += 1.0;

	new Float:cameralookat[3];
	GetVehiclePos(vid, cameralookat[0], cameralookat[1], cameralookat[2]);
	SetPlayerCameraPos(playerid, camerapos[0], camerapos[1], camerapos[2]);
	SetPlayerCameraLookAt(playerid, cameralookat[0], cameralookat[1], cameralookat[2]);
}

stock HideCarColorPickerForPlayer(playerid)
{
	for(new i=0;i<4;i++) PlayerTextDrawHide(playerid, pInfo[playerid][CarColorPicker][i]);
	CancelSelectTextDraw(playerid);

	SetCameraBehindPlayer(playerid);
}

stock CarColorPickerCheck(playerid, PlayerText:txdraw)
{
	if( pInfo[playerid][player_choosing_carcolor] )
	{
		new vid = GetVehicleByUid(pInfo[playerid][player_carpaint_vuid]);

		if( txdraw == pInfo[playerid][CarColorPicker][0] )
		{
			// poprzedni kolor
			if( pInfo[playerid][player_choosing_carcolor_cur] == 0 ) pInfo[playerid][player_choosing_carcolor_cur] = 254;
			else pInfo[playerid][player_choosing_carcolor_cur] -= 1;

			if( pInfo[playerid][player_choosen_carcolor][0] == -1 ) ChangeVehicleColor(vid, pInfo[playerid][player_choosing_carcolor_cur], Vehicle[vid][vehicle_color][1]);
			else ChangeVehicleColor(vid, pInfo[playerid][player_choosen_carcolor][0], pInfo[playerid][player_choosing_carcolor_cur]);

			PlayerTextDrawSetString(playerid, pInfo[playerid][CarColorPicker][2], sprintf("~b~~h~~h~%d~n~~w~Aktualny", pInfo[playerid][player_choosing_carcolor_cur]));
		}
		else if( txdraw == pInfo[playerid][CarColorPicker][1] )
		{
			// nastepny kolor
			if( pInfo[playerid][player_choosing_carcolor_cur] == 254 ) pInfo[playerid][player_choosing_carcolor_cur] = 0;
			else pInfo[playerid][player_choosing_carcolor_cur] += 1;

			if( pInfo[playerid][player_choosen_carcolor][0] == -1 ) ChangeVehicleColor(vid, pInfo[playerid][player_choosing_carcolor_cur], Vehicle[vid][vehicle_color][1]);
			else ChangeVehicleColor(vid, pInfo[playerid][player_choosen_carcolor][0], pInfo[playerid][player_choosing_carcolor_cur]);

			PlayerTextDrawSetString(playerid, pInfo[playerid][CarColorPicker][2], sprintf("~b~~h~~h~%d~n~~w~Aktualny", pInfo[playerid][player_choosing_carcolor_cur]));
		}
		else if( txdraw == pInfo[playerid][CarColorPicker][2] )
		{
			// kliknal na kolor:
			ShowPlayerDialog(playerid, DIALOG_MALOWANIE_CARCOLOR, DIALOG_STYLE_INPUT, "Podaj numer koloru", "W poni¿szym polu podaj numer koloru (0-254):", "Gotowe", "Anuluj");
		}
		else if( txdraw == pInfo[playerid][CarColorPicker][3] )
		{
			if( pInfo[playerid][player_choosen_carcolor][0] == -1 )
			{
				// drugi kolor bedzie wybieral
				pInfo[playerid][player_choosen_carcolor][0] = pInfo[playerid][player_choosing_carcolor_cur];

				ChangeVehicleColor(vid, pInfo[playerid][player_choosen_carcolor][0], pInfo[playerid][player_choosing_carcolor_cur]);

				SendPlayerInformation(playerid, "~g~Wybrales pierwszy kolor~n~~w~Teraz wybierz drugi kolor", 5000);
			}
			else
			{
				// wybral juz drugi, teraz oferta
				pInfo[playerid][player_choosen_carcolor][1] = pInfo[playerid][player_choosing_carcolor_cur];
				pInfo[playerid][player_choosing_carcolor] = false;
				HideCarColorPickerForPlayer(playerid);

				defer MakeResprayOffer[100](playerid, vid, pInfo[playerid][player_choosen_carcolor][0], pInfo[playerid][player_choosen_carcolor][1]);
			}
		}
	}

	return 1;
}

timer MakeResprayOffer[50](playerid, vid, color1, color2)
{
	new senderid = GetPlayerByUid(pInfo[playerid][player_offered_carpaint]);
	if( senderid == INVALID_PLAYER_ID )
	{
		SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz, który z³o¿y³ Ci ofertê przemalowania pojazdu nie jest ju¿ pod³¹czony do serwera.");
	}
	else
	{
		if( GetDistanceBetweenPlayers(playerid, senderid) > 20 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz, który z³o¿y³ Ci ofertê przemalowania pojazdu jest zbyt daleko.");
		new gslot = GetPlayerDutySlot(senderid);
		if( gslot == -1 ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz, który z³o¿y³ Ci ofertê przemalowania pojazdu zszed³e ze s³u¿by w firmie.");
		new gid = pGroup[senderid][gslot][pg_id];
		if( Group[gid][group_type] != GROUP_TYPE_WARSZTAT ) return SendGuiInformation(playerid, "Wyst¹pi³ b³¹d", "Gracz, który z³o¿y³ Ci ofertê przemalowania pojazdu zszed³e ze s³u¿by w firmie.");

		SendClientMessage(senderid, COLOR_GOLD, "Gracz, któremu oferowa³eœ malowanie wybra³ nowy kolor pojazdu. Poczekaj na akceptacjê przez niego oferty.");

		ChangeVehicleColor(vid, color1, color2);
		
		new str[200];
		format(str, sizeof(str), "Otrzyma³eœ ofertê malowania pojazdu od gracza %s.\nUID pojazdu:\t\t%d\nKolor:\t\t\t%d:%d\nCena:\t\t\t$500\n\n"HEX_COLOR_LIGHTER_RED"Aby potwierdziæ zakup wpisz w poni¿szym polu 'Potwierdzam':", 
		pInfo[senderid][player_name], pInfo[playerid][player_carpaint_vuid], color1, color2);
		
		new resp = SetOffer(senderid, playerid, OFFER_TYPE_MALOWANIE, 300, gid);	
		if( resp ) ShowPlayerOffer(playerid, senderid, "Pojazd", sprintf("Malowanie pojazdu o UID %d (kolor: %d:%d)", pInfo[playerid][player_carpaint_vuid], pInfo[playerid][player_choosen_carcolor][0], pInfo[playerid][player_choosen_carcolor][1]), 300);
	}

	return 1;
}