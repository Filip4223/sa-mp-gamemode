stock ShowHint(playerid, message[])
{
	if( pInfo[playerid][player_show_hints] )InfoboxCenter(playerid, 5, sprintf("%s~n~~n~~b~~h~~h~Pamietaj, ze podpowiedzi mozesz wylaczyc w dowolnym momencie pod /stats.", message));

	return 1;
}

stock InfoboxCenter(playerid, time, message[])
{
	ShowInfobox(playerid, InfoBox::CENTER, time, message);

	return 1;
}

stock InfoboxRight(playerid, time, message[])
{
	ShowInfobox(playerid, InfoBox::RIGHT, time, message);

	return 1;
}

stock ShowInfobox(playerid, type, time, message[])
{
	replacePolishChars(message);

	time = time * 1000;

	switch(type)
	{
		case InfoBox::RIGHT_BIG:
		{
            PlayerTextDrawSetString(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], message);
            PlayerTextDrawShow(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid]);
		}

		case InfoBox::RIGHT:
		{
			PlayerTextDrawSetString(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], message);
    		PlayerTextDrawShow(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid]);
		}

		case InfoBox::CENTER:
		{
			PlayerTextDrawSetString(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], message);
    		PlayerTextDrawShow(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);
		}

		case InfoBox::CENTER_SMALL:
		{
			PlayerTextDrawSetString(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid], message);
			PlayerTextDrawShow(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid]);
		}

		case InfoBox::UNDER_HUD:
		{
			PlayerTextDrawSetString(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], message);
			PlayerTextDrawShow(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid]);
		}
	}

	InfoBoxChecksum[type][playerid]++;
	defer HideInfobox[time](playerid, type, InfoBoxChecksum[type][playerid]);

	return 1;
}

timer HideInfobox[100](playerid, type, checksum)
{
	if( InfoBoxChecksum[type][playerid] != checksum ) return 1;

	InfoBoxChecksum[type][playerid] = 0;

	switch(type)
	{
		case InfoBox::RIGHT_BIG:
		{
            PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid]);
		}

		case InfoBox::RIGHT:
		{
    		PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid]);
		}

		case InfoBox::CENTER:
		{
    		PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);
		}

		case InfoBox::CENTER_SMALL:
		{
			PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid]);
		}

		case InfoBox::UNDER_HUD:
		{
			PlayerTextDrawHide(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid]);
		}
	}
	return 1;
}

stock CreatePlayerTextdraws(playerid)
{	
	pInfo[playerid][informationTd] = CreatePlayerTextDraw(playerid, 459.000000, 283.000000, " ");
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][informationTd], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][informationTd], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][informationTd], 0.170000, 1.100000);
	PlayerTextDrawColor(playerid, pInfo[playerid][informationTd], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][informationTd], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][informationTd], 1);
	PlayerTextDrawUseBox(playerid, pInfo[playerid][informationTd], 1);
	PlayerTextDrawBoxColor(playerid, pInfo[playerid][informationTd], 128);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][informationTd], 606.000000, 30.000000);

	pInfo[playerid][gymTd] = CreatePlayerTextDraw(playerid, 490.000000, 294.000000, " ");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][gymTd], 2);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][gymTd], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][gymTd], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][gymTd], 0.180000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][gymTd], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][gymTd], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][gymTd], 1);
	PlayerTextDrawUseBox(playerid, pInfo[playerid][gymTd], 1);
	PlayerTextDrawBoxColor(playerid, pInfo[playerid][gymTd], 153);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][gymTd], 0.000000, 134.000000);

	pInfo[playerid][Dashboard] = CreatePlayerTextDraw(playerid, 319.666473, 277.940765, "");
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][Dashboard], 0.261333, 1.077333);
	PlayerTextDrawAlignment(playerid, pInfo[playerid][Dashboard], 2);
	PlayerTextDrawColor(playerid, pInfo[playerid][Dashboard], -1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][Dashboard], 0);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][Dashboard], 1);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][Dashboard], 102);
	PlayerTextDrawFont(playerid, pInfo[playerid][Dashboard], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][Dashboard], 1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][Dashboard], 0);
	
	pInfo[playerid][DoorInfo][0] = CreatePlayerTextDraw(playerid, 470.000000, 341.000000, "~n~");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][DoorInfo][0], 2);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][DoorInfo][0], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][DoorInfo][0], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][DoorInfo][0], 0.500000, 6.900000);
	PlayerTextDrawColor(playerid, pInfo[playerid][DoorInfo][0], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][DoorInfo][0], 0);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][DoorInfo][0], 1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][DoorInfo][0], 1);
	PlayerTextDrawUseBox(playerid, pInfo[playerid][DoorInfo][0], 1);
	PlayerTextDrawBoxColor(playerid, pInfo[playerid][DoorInfo][0], 112);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][DoorInfo][0], 0.000000, 170.000000);

	pInfo[playerid][DoorInfo][1] = CreatePlayerTextDraw(playerid, 466.000000, 346.000000, "Los Santos Police Department~n~~n~~w~Wejscie: ~g~$5");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][DoorInfo][1], 2);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][DoorInfo][1], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][DoorInfo][1], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][DoorInfo][1], 0.230000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][DoorInfo][1], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][DoorInfo][1], 0);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][DoorInfo][1], 1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][DoorInfo][1], 1);

	pInfo[playerid][DoorInfo][2] = CreatePlayerTextDraw(playerid, 470.000000, 384.000000, "~y~Aby wejsc, wcisnij jednoczesnie~n~~w~[~b~~h~LALT + SPACE~w~]");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][DoorInfo][2], 2);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][DoorInfo][2], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][DoorInfo][2], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][DoorInfo][2], 0.230000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][DoorInfo][2], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][DoorInfo][2], 0);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][DoorInfo][2], 1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][DoorInfo][2], 1);
	
	pInfo[playerid][AreaInfo] = CreatePlayerTextDraw(playerid, 548.000000, 48.000000, "#area: ~y~432");
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][AreaInfo], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][AreaInfo], 3);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][AreaInfo], 0.290000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][AreaInfo], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][AreaInfo], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][AreaInfo], 1);
	
	pInfo[playerid][leftTime] = CreatePlayerTextDraw(playerid, 580.000000, 320.000000, " ");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][leftTime], 3);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][leftTime], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][leftTime], 3);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][leftTime], 0.610000, 2.400000);
	PlayerTextDrawColor(playerid, pInfo[playerid][leftTime], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][leftTime], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][leftTime], 1);
	
	CreateGroupPlayerTextdraws(playerid);
	
	pInfo[playerid][GroupDutyTag] = CreatePlayerTextDraw(playerid, 8.000000, 428.000000, "GOV");
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][GroupDutyTag], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][GroupDutyTag], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][GroupDutyTag], 0.230000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][GroupDutyTag], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][GroupDutyTag], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][GroupDutyTag], 1);
		
	pInfo[playerid][DashboardPane][0] = CreatePlayerTextDraw(playerid, 202.000000, 340.000000, "~n~");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][DashboardPane][0], 2);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][DashboardPane][0], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][DashboardPane][0], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][DashboardPane][0], 0.500000, 14.000000);
	PlayerTextDrawColor(playerid, pInfo[playerid][DashboardPane][0], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][DashboardPane][0], 0);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][DashboardPane][0], 1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][DashboardPane][0], 1);
	PlayerTextDrawUseBox(playerid, pInfo[playerid][DashboardPane][0], 1);
	PlayerTextDrawBoxColor(playerid, pInfo[playerid][DashboardPane][0], 0x9E9EAD20);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][DashboardPane][0], 730.000000, -950.000000);

	pInfo[playerid][DashboardPane][1] = CreatePlayerTextDraw(playerid, 152.000000, 349.000000, "Sluzba: ~b~grupa                    ~y~Ganton Gym (UID: 249)~n~~n~~w~Rozpoczeto: ~g~14:24               ~w~Tytul: ~r~Lider");
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][DashboardPane][1], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][DashboardPane][1], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][DashboardPane][1], 0.290000, 1.000000);
	PlayerTextDrawColor(playerid, pInfo[playerid][DashboardPane][1], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][DashboardPane][1], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][DashboardPane][1], 1);
	
	pInfo[playerid][vehicleFuelInfo] = CreatePlayerTextDraw(playerid, 244.000000, 330.000000, "~y~PALIWO: ~g~51/57 L");
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][vehicleFuelInfo], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][vehicleFuelInfo], 2);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][vehicleFuelInfo], 0.479999, 1.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][vehicleFuelInfo], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][vehicleFuelInfo], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][vehicleFuelInfo], 1);
	
	/*
	OOOOOOOOOO  FFFFFFFFFFF  EEEEEEE  RRRRRRRR    TTTTTTTTT  YY     YY
	OO      OO  FFFFFFFFFFF  EE       RR    RR        TT      YY   YY
	OO      OO  FF           EEEEEEE  RR    RR        TT       YY YY
	OO      OO  FFFFFFFFFFF  EEEEEEE  RRRRRR          TT         Y
	OO      OO  FF           EE       RR    RR        TT         Y
	0000000000  FF           EEEEEEE  RR      RR      TT         Y
	*/
	pInfo[playerid][OfferTD][0] = CreatePlayerTextDraw(playerid, 240.666702, 304.333221, "mdl-2009:oferta");
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][OfferTD][0], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][OfferTD][0], 167.000000, 80.000000);
	PlayerTextDrawAlignment(playerid, pInfo[playerid][OfferTD][0], 1);
	PlayerTextDrawColor(playerid, pInfo[playerid][OfferTD][0], -1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][0], 0);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][OfferTD][0], 0);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][OfferTD][0], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][OfferTD][0], 4);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][OfferTD][0], 0);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][0], 0);

	pInfo[playerid][OfferTD][1] = CreatePlayerTextDraw(playerid, 297.666503, 319.007415, "Vincent_Dabrasco");
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][OfferTD][1], 0.182332, 0.716444);
	PlayerTextDrawAlignment(playerid, pInfo[playerid][OfferTD][1], 1);
	PlayerTextDrawColor(playerid, pInfo[playerid][OfferTD][1], -1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][1], 0);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][OfferTD][1], 0);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][OfferTD][1], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][OfferTD][1], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][OfferTD][1], 1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][1], 0);

	pInfo[playerid][OfferTD][2] = CreatePlayerTextDraw(playerid, 270.333282, 344.311187, "155");
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][OfferTD][2], 0.530667, 2.130961);
	PlayerTextDrawAlignment(playerid, pInfo[playerid][OfferTD][2], 1);
	PlayerTextDrawColor(playerid, pInfo[playerid][OfferTD][2], -1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][2], 0);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][OfferTD][2], 1);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][OfferTD][2], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][OfferTD][2], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][OfferTD][2], 1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][2], 0);

	pInfo[playerid][OfferTD][3] = CreatePlayerTextDraw(playerid, 258.333068, 332.611145, "Akceptacja vCard");
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][OfferTD][3], 0.149665, 0.641776);
	PlayerTextDrawAlignment(playerid, pInfo[playerid][OfferTD][3], 1);
	PlayerTextDrawColor(playerid, pInfo[playerid][OfferTD][3], -1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][3], 0);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][OfferTD][3], 1);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][OfferTD][3], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][OfferTD][3], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][OfferTD][3], 1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][3], 0);

	pInfo[playerid][OfferTD][4] = CreatePlayerTextDraw(playerid, 362.333343, 319.266754, "mdl-2009:yo");
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][OfferTD][4], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][OfferTD][4], 25.000000, 25.000000);
	PlayerTextDrawAlignment(playerid, pInfo[playerid][OfferTD][4], 1);
	PlayerTextDrawColor(playerid, pInfo[playerid][OfferTD][4], -1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][4], 0);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][OfferTD][4], 0);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][OfferTD][4], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][OfferTD][4], 4);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][OfferTD][4], 0);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][OfferTD][4], 0);


	
	pInfo[playerid][BusInfo] = CreatePlayerTextDraw(playerid, 312.000000, 330.000000, " ");
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][BusInfo], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][BusInfo], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][BusInfo], 0.230000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][BusInfo], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][BusInfo], 0);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][BusInfo], 1);
	PlayerTextDrawSetShadow(playerid, pInfo[playerid][BusInfo], 1);
	PlayerTextDrawUseBox(playerid, pInfo[playerid][BusInfo], 1);
	PlayerTextDrawBoxColor(playerid, pInfo[playerid][BusInfo], 96);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][BusInfo], 611.000000, 0.000000);

	pInfo[playerid][CarColorPicker][0] = CreatePlayerTextDraw(playerid, 246.000000, 351.000000, "Poprzedni kolor");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][CarColorPicker][0], 2);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][CarColorPicker][0], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][CarColorPicker][0], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][CarColorPicker][0], 0.180000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][CarColorPicker][0], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][CarColorPicker][0], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][CarColorPicker][0], 1);
	PlayerTextDrawUseBox(playerid, pInfo[playerid][CarColorPicker][0], 1);
	PlayerTextDrawBoxColor(playerid, pInfo[playerid][CarColorPicker][0], 96);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][CarColorPicker][0], 10.000000, 71.000000);
	PlayerTextDrawSetSelectable(playerid, pInfo[playerid][CarColorPicker][0], true);

	pInfo[playerid][CarColorPicker][1] = CreatePlayerTextDraw(playerid, 389.000000, 351.000000, "Nastepny kolor");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][CarColorPicker][1], 2);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][CarColorPicker][1], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][CarColorPicker][1], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][CarColorPicker][1], 0.180000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][CarColorPicker][1], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][CarColorPicker][1], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][CarColorPicker][1], 1);
	PlayerTextDrawUseBox(playerid, pInfo[playerid][CarColorPicker][1], 1);
	PlayerTextDrawBoxColor(playerid, pInfo[playerid][CarColorPicker][1], 96);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][CarColorPicker][1], 10.000000, 71.000000);
	PlayerTextDrawSetSelectable(playerid, pInfo[playerid][CarColorPicker][1], true);

	pInfo[playerid][CarColorPicker][2] = CreatePlayerTextDraw(playerid, 317.000000, 347.000000, "~b~~h~~h~135~n~~w~Aktualny");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][CarColorPicker][2], 2);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][CarColorPicker][2], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][CarColorPicker][2], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][CarColorPicker][2], 0.180000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][CarColorPicker][2], -16776961);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][CarColorPicker][2], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][CarColorPicker][2], 1);
	PlayerTextDrawUseBox(playerid, pInfo[playerid][CarColorPicker][2], 1);
	PlayerTextDrawBoxColor(playerid, pInfo[playerid][CarColorPicker][2], 96);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][CarColorPicker][2], 20.000000, 47.000000);
	PlayerTextDrawSetSelectable(playerid, pInfo[playerid][CarColorPicker][2], true);

	pInfo[playerid][CarColorPicker][3] = CreatePlayerTextDraw(playerid, 317.000000, 370.000000, "Wybierz");
	PlayerTextDrawAlignment(playerid, pInfo[playerid][CarColorPicker][3], 2);
	PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][CarColorPicker][3], 255);
	PlayerTextDrawFont(playerid, pInfo[playerid][CarColorPicker][3], 1);
	PlayerTextDrawLetterSize(playerid, pInfo[playerid][CarColorPicker][3], 0.180000, 0.899999);
	PlayerTextDrawColor(playerid, pInfo[playerid][CarColorPicker][3], -1);
	PlayerTextDrawSetOutline(playerid, pInfo[playerid][CarColorPicker][3], 1);
	PlayerTextDrawSetProportional(playerid, pInfo[playerid][CarColorPicker][3], 1);
	PlayerTextDrawUseBox(playerid, pInfo[playerid][CarColorPicker][3], 1);
	PlayerTextDrawBoxColor(playerid, pInfo[playerid][CarColorPicker][3], 96);
	PlayerTextDrawTextSize(playerid, pInfo[playerid][CarColorPicker][3], 10.000000, 47.000000);
	PlayerTextDrawSetSelectable(playerid, pInfo[playerid][CarColorPicker][3], true);

	InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid] = CreatePlayerTextDraw(playerid, 430.000000, 133.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], 255);
    PlayerTextDrawFont(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], 1);
    PlayerTextDrawLetterSize(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], 0.290000, 1.200000);
    PlayerTextDrawColor(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], -1);
    PlayerTextDrawSetOutline(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], 1);
    PlayerTextDrawSetProportional(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], 1);
    PlayerTextDrawUseBox(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], 1);
    PlayerTextDrawBoxColor(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], 68); 
    PlayerTextDrawTextSize(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid], 608.000000, 10.000000);

    InfoBoxTextdraw[InfoBox::RIGHT][playerid] = CreatePlayerTextDraw(playerid, 459.000000, 283.000000, "_");
	PlayerTextDrawBackgroundColor(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], 255);
	PlayerTextDrawFont(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], 1);
	PlayerTextDrawLetterSize(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], 0.270000, 1.100000);
	PlayerTextDrawColor(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], -1);
	PlayerTextDrawSetOutline(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], -1);
	PlayerTextDrawSetProportional(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], 1);
	PlayerTextDrawUseBox(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], 1);
	PlayerTextDrawBoxColor(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], 128);
	PlayerTextDrawTextSize(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], 606.000000, 30.000000);
	PlayerTextDrawSetShadow(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid], 1);

	InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid] = CreatePlayerTextDraw(playerid, 499.000000, 104.000000, "Brak");
	PlayerTextDrawBackgroundColor(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], 255);
	PlayerTextDrawFont(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], 1);
	PlayerTextDrawLetterSize(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], 0.209999, 0.799998);
	PlayerTextDrawColor(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], -1);
	PlayerTextDrawSetOutline(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], 0);
	PlayerTextDrawSetProportional(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], 1);
	PlayerTextDrawSetShadow(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], 1);
	PlayerTextDrawUseBox(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], 1);
	PlayerTextDrawBoxColor(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], 122);
	PlayerTextDrawTextSize(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid], 605.000000, 248.000000);

	InfoBoxTextdraw[InfoBox::CENTER][playerid] = CreatePlayerTextDraw(playerid, 320.000000,350.000000,"_");
	PlayerTextDrawAlignment(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], 2);
	PlayerTextDrawBackgroundColor(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], 21);
	PlayerTextDrawFont(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], 1);
	PlayerTextDrawLetterSize(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], 0.270000, 1.100000);
	PlayerTextDrawColor(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], -1);
	PlayerTextDrawSetOutline(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], 1);
	PlayerTextDrawSetProportional(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], 1);
	PlayerTextDrawSetShadow(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid], 1);

	InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid] = CreatePlayerTextDraw(playerid, 321.000000, 339.000000, "");
	PlayerTextDrawBackgroundColor(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid], 255);
	PlayerTextDrawFont(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid], 1);
	PlayerTextDrawLetterSize(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid], 0.270000, 1.100000);
	PlayerTextDrawColor(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid], -1);
	PlayerTextDrawSetOutline(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid], 0);
	PlayerTextDrawSetProportional(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid], 1);
	PlayerTextDrawSetShadow(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid], 1);
	PlayerTextDrawAlignment(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid], 2);

	for(new i = 0; i < 5; i++)
	{

		SmallAlert[i][playerid][0] = CreatePlayerTextDraw(playerid, 4.999957, 295.207763-(31.0*i), "mdl-2005:notka");
		PlayerTextDrawLetterSize(playerid, SmallAlert[i][playerid][0], 0.000000, 0.000000);
		PlayerTextDrawTextSize(playerid, SmallAlert[i][playerid][0], 144.000000, 37.000000);
		PlayerTextDrawAlignment(playerid, SmallAlert[i][playerid][0], 1);
		PlayerTextDrawColor(playerid, SmallAlert[i][playerid][0], -1);
		PlayerTextDrawSetShadow(playerid, SmallAlert[i][playerid][0], 0);
		PlayerTextDrawSetOutline(playerid, SmallAlert[i][playerid][0], 0);
		PlayerTextDrawBackgroundColor(playerid, SmallAlert[i][playerid][0], 255);
		PlayerTextDrawFont(playerid, SmallAlert[i][playerid][0], 4);
		PlayerTextDrawSetProportional(playerid, SmallAlert[i][playerid][0], 0);
		PlayerTextDrawSetShadow(playerid, SmallAlert[i][playerid][0], 0);

		SmallAlert[i][playerid][1] = CreatePlayerTextDraw(playerid, 35.666633, 305.318389-(31.0*i), "Powiadomienie");
		PlayerTextDrawLetterSize(playerid, SmallAlert[i][playerid][1], 0.152666, 0.592000);
		PlayerTextDrawAlignment(playerid, SmallAlert[i][playerid][1], 1);
		PlayerTextDrawColor(playerid, SmallAlert[i][playerid][1], -1523963137);
		PlayerTextDrawSetShadow(playerid, SmallAlert[i][playerid][1], 0);
		PlayerTextDrawSetOutline(playerid, SmallAlert[i][playerid][1], 1);
		PlayerTextDrawBackgroundColor(playerid, SmallAlert[i][playerid][1], 51);
		PlayerTextDrawFont(playerid, SmallAlert[i][playerid][1], 1);
		PlayerTextDrawSetProportional(playerid, SmallAlert[i][playerid][1], 1);
		PlayerTextDrawSetShadow(playerid, SmallAlert[i][playerid][1], 0);

		SmallAlert[i][playerid][2] = CreatePlayerTextDraw(playerid, 35.333446, 311.540557-(31.0*i), "Nie_mozesz_uzyc_tej_funkcji_z_powodu_za_niskich~n~uprawnien!");
		PlayerTextDrawBackgroundColor(playerid, SmallAlert[i][playerid][2], 51);
		PlayerTextDrawFont(playerid, SmallAlert[i][playerid][2], 1);
		PlayerTextDrawLetterSize(playerid, SmallAlert[i][playerid][2], 0.108998, 0.521480);
		PlayerTextDrawColor(playerid, SmallAlert[i][playerid][2], -1);
		PlayerTextDrawSetOutline(playerid, SmallAlert[i][playerid][2], -1);
		PlayerTextDrawSetProportional(playerid, SmallAlert[i][playerid][2], 1);	
		PlayerTextDrawUseBox(playerid, SmallAlert[i][playerid][2], 1);
		PlayerTextDrawBoxColor(playerid, SmallAlert[i][playerid][2], -255);
		PlayerTextDrawSetShadow(playerid, SmallAlert[i][playerid][2], 0);
		PlayerTextDrawTextSize(playerid, SmallAlert[i][playerid][2], 606.000000, 30.000000);

		//PlayerTextDrawAlignment(playerid, SmallAlert[i][playerid][2], 1);

	}
}

stock CreateTextdraws()
{
	BlackScreen = TextDrawCreate(700.000000, -31.000000, "~n~");
	TextDrawLetterSize(BlackScreen, 0.500000, 57.000000);
	TextDrawUseBox(BlackScreen, 1);
	TextDrawBoxColor(BlackScreen, 255);
	TextDrawTextSize(BlackScreen, -70.000000, 0.000000);
	
	PlayerVip = TextDrawCreate(532.000000, 420.000000, "Gracz Premium");
	TextDrawBackgroundColor(PlayerVip, -154914561);
	TextDrawFont(PlayerVip, 2);
	TextDrawLetterSize(PlayerVip, 0.259999, 0.899999);
	TextDrawColor(PlayerVip, -1);
	TextDrawSetOutline(PlayerVip, 1);
	TextDrawSetProportional(PlayerVip, 1);
	
	vehicleInfo = TextDrawCreate(438.000000, 366.000000, "Aby uruchomic silnik, wcisnij ~y~LCTRL ~w~+ ~y~LALT~w~.~n~Klawisz ~y~LMB ~w~kontroluje swiatla w pojezdzie.");
	TextDrawBackgroundColor(vehicleInfo, 255);
	TextDrawFont(vehicleInfo, 1);
	TextDrawLetterSize(vehicleInfo, 0.220000, 0.899999);
	TextDrawColor(vehicleInfo, -1);
	TextDrawSetOutline(vehicleInfo, 0);
	TextDrawSetProportional(vehicleInfo, 1);
	TextDrawSetShadow(vehicleInfo, 1);
	TextDrawUseBox(vehicleInfo, 1);
	TextDrawBoxColor(vehicleInfo, 128);
	TextDrawTextSize(vehicleInfo, 611.000000, 0.000000);
	
	vehicleEngineStarting = TextDrawCreate(325.000000, 328.000000, "trwa odpalanie silnika...");
 	TextDrawAlignment(vehicleEngineStarting, 2);
 	TextDrawBackgroundColor(vehicleEngineStarting, 255);
 	TextDrawFont(vehicleEngineStarting, 2);
 	TextDrawLetterSize(vehicleEngineStarting, 0.479999, 1.799998);
 	TextDrawColor(vehicleEngineStarting, -1);
 	TextDrawSetOutline(vehicleEngineStarting, 1);
 	TextDrawSetProportional(vehicleEngineStarting, 1);
 	TextDrawSetShadow(vehicleEngineStarting, 1);
	
	CreateGroupStaticTextdraws();
	
	// PENALTIES
	PenaltiesTextDraw = TextDrawCreate(38.000003, 250.562973, "");
	TextDrawLetterSize(PenaltiesTextDraw, 0.183666, 0.981926);
	TextDrawAlignment(PenaltiesTextDraw, 1);
	TextDrawColor(PenaltiesTextDraw, -1);
	TextDrawSetShadow(PenaltiesTextDraw, 1);
	TextDrawSetOutline(PenaltiesTextDraw, 0);
	TextDrawBackgroundColor(PenaltiesTextDraw, 255);
	TextDrawFont(PenaltiesTextDraw, 1);
	TextDrawSetProportional(PenaltiesTextDraw, 1);
	TextDrawSetShadow(PenaltiesTextDraw, 1);
		
	LSNtd = TextDrawCreate(1.000000, 439.000000, "				    		    ~>~ ~w~Brak sygnalu nadawania.");
	TextDrawBackgroundColor(LSNtd, 153);
	TextDrawFont(LSNtd, 1);
	TextDrawLetterSize(LSNtd, 0.209998, 0.799997);
	TextDrawColor(LSNtd, -1);
	TextDrawSetOutline(LSNtd, 1);
	TextDrawSetProportional(LSNtd, 1);
	TextDrawUseBox(LSNtd, 1);
	TextDrawBoxColor(LSNtd, 85);
	TextDrawTextSize(LSNtd, 720.000000, 170.000000);
	
	LSNtd2 = TextDrawCreate(6.333347, 434.681640, "mdl-2009:sannews");
	TextDrawLetterSize(LSNtd2, 0.000000, 0.000000);
	TextDrawTextSize(LSNtd2, 33.000000, 15.000000);
	TextDrawAlignment(LSNtd2, 1);
	TextDrawColor(LSNtd2, -1);
	TextDrawSetShadow(LSNtd2, 0);
	TextDrawSetOutline(LSNtd2, 0);
	TextDrawBackgroundColor(LSNtd2, 255);
	TextDrawFont(LSNtd2, 4);
	TextDrawSetProportional(LSNtd2, 0);
	TextDrawSetShadow(LSNtd2, 0);

	ZegarekTD = TextDrawCreate(575.000000, 49.000000, "12:11");
	TextDrawBackgroundColor(ZegarekTD, 255);
	TextDrawFont(ZegarekTD, 3);
	TextDrawLetterSize(ZegarekTD, 0.340000, 1.100000);
	TextDrawColor(ZegarekTD, -1);
	TextDrawSetOutline(ZegarekTD, 1);
	TextDrawSetProportional(ZegarekTD, 1);

	HonestLogoLogin = TextDrawCreate(244.000000, 109.000000, "mdl-3000:honest_logo");
	TextDrawFont(HonestLogoLogin, 4);
	TextDrawLetterSize(HonestLogoLogin, 0.600000, 2.000000);
	TextDrawTextSize(HonestLogoLogin, 125.500000, 67.500000);
	TextDrawSetOutline(HonestLogoLogin, 1);
	TextDrawSetShadow(HonestLogoLogin, 0);
	TextDrawAlignment(HonestLogoLogin, 1);
	TextDrawColor(HonestLogoLogin, -1);
	TextDrawBackgroundColor(HonestLogoLogin, 255);
	TextDrawBoxColor(HonestLogoLogin, 50);
	TextDrawUseBox(HonestLogoLogin, 1);
	TextDrawSetProportional(HonestLogoLogin, 1);
	TextDrawSetSelectable(HonestLogoLogin, 0);

	HonestLogoAboveHud = TextDrawCreate(595.000000, 424.000000, "mdl-3000:honest_logo");
	TextDrawFont(HonestLogoAboveHud, 4);
	TextDrawLetterSize(HonestLogoAboveHud, 0.600000, 2.000000);
	TextDrawTextSize(HonestLogoAboveHud, 41.500000, 22.500000);
	TextDrawSetOutline(HonestLogoAboveHud, 1);
	TextDrawSetShadow(HonestLogoAboveHud, 0);
	TextDrawAlignment(HonestLogoAboveHud, 1);
	TextDrawColor(HonestLogoAboveHud, -1);
	TextDrawBackgroundColor(HonestLogoAboveHud, 255);
	TextDrawBoxColor(HonestLogoAboveHud, 50);
	TextDrawUseBox(HonestLogoAboveHud, 1);
	TextDrawSetProportional(HonestLogoAboveHud, 1);
	TextDrawSetSelectable(HonestLogoAboveHud, 0);

	Panorama[0] = TextDrawCreate(320.000000, 337.000000, "~n~~n~~n~~n~~n~~n~~n~~n~~n~~n~~n~~n~~n~~n~~n~~n~");
	TextDrawAlignment(Panorama[0], 2);
	TextDrawBackgroundColor(Panorama[0], 255);
	TextDrawFont(Panorama[0], 0);
	TextDrawLetterSize(Panorama[0], 1.000000, 3.300000);
	TextDrawColor(Panorama[0], -1);
	TextDrawSetOutline(Panorama[0], 0);
	TextDrawSetProportional(Panorama[0], 1);
	TextDrawSetShadow(Panorama[0], 1);
	TextDrawUseBox(Panorama[0], 1);
	TextDrawBoxColor(Panorama[0], 255);
	TextDrawTextSize(Panorama[0], 0.000000, 640.000000);

	Panorama[1] = TextDrawCreate(650.000000, 0.000000, "~n~~n~~n~~n~~n~~n~~n~~n~~n~~n~~n~");
	TextDrawBackgroundColor(Panorama[1], 255);
	TextDrawFont(Panorama[1], 1);
	TextDrawLetterSize(Panorama[1], 0.500000, 1.000000);
	TextDrawColor(Panorama[1], -1);
	TextDrawSetOutline(Panorama[1], 0);
	TextDrawSetProportional(Panorama[1], 1);
	TextDrawSetShadow(Panorama[1], 1);
	TextDrawUseBox(Panorama[1], 1);
	TextDrawBoxColor(Panorama[1], 255);
	TextDrawTextSize(Panorama[1], -10.000000, 10.000000);

}

stock CreateGroupPlayerTextdraws(playerid)
{
	new Float:baseY = 183.0;
	for(new i=0;i<5;i++)
	{
		pInfo[playerid][GroupsListRow][i] = CreatePlayerTextDraw(playerid, 104.000000, baseY+(i*14), "1       Pig Pen (669)");
		PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][GroupsListRow][i], 255);
		PlayerTextDrawFont(playerid, pInfo[playerid][GroupsListRow][i], 1);
		PlayerTextDrawLetterSize(playerid, pInfo[playerid][GroupsListRow][i], 0.290000, 1.000000);
		PlayerTextDrawColor(playerid, pInfo[playerid][GroupsListRow][i], -1);
		PlayerTextDrawSetOutline(playerid, pInfo[playerid][GroupsListRow][i], 1);
		PlayerTextDrawSetProportional(playerid, pInfo[playerid][GroupsListRow][i], 1);
		PlayerTextDrawUseBox(playerid, pInfo[playerid][GroupsListRow][i], 1);
		PlayerTextDrawBoxColor(playerid, pInfo[playerid][GroupsListRow][i], 112);
		PlayerTextDrawTextSize(playerid, pInfo[playerid][GroupsListRow][i], 537.000000, 0.000000);
	}
	
	baseY = 184.0;
	for(new i=0;i<5;i++)
	{
		pInfo[playerid][GroupsListStaticButtons][i*5] = CreatePlayerTextDraw(playerid, 344.000000, baseY+(playerid, i*14), "Info");
		PlayerTextDrawAlignment(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 2);
		PlayerTextDrawBackgroundColor(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 255);
		PlayerTextDrawFont(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 1);
		PlayerTextDrawLetterSize(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], -1);
		PlayerTextDrawSetOutline(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 1);
		PlayerTextDrawSetProportional(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 1);
		PlayerTextDrawUseBox(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 1);
		PlayerTextDrawBoxColor(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 112);
		PlayerTextDrawTextSize(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 12.0, 36.0);
		PlayerTextDrawSetSelectable(playerid,  pInfo[playerid][GroupsListStaticButtons][i*5], 1);

		pInfo[playerid][GroupsListStaticButtons][(i*5)+1] = CreatePlayerTextDraw(playerid, 387.000000, baseY+(playerid, i*14), "Pojazdy");
		PlayerTextDrawAlignment(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 2);
		PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 255);
		PlayerTextDrawFont(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 1);
		PlayerTextDrawLetterSize(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], -1);
		PlayerTextDrawSetOutline(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 1);
		PlayerTextDrawSetProportional(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 1);
		PlayerTextDrawUseBox(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 1);
		PlayerTextDrawBoxColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 112);
		PlayerTextDrawTextSize(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+1], 1);

		pInfo[playerid][GroupsListStaticButtons][(i*5)+2] = CreatePlayerTextDraw(playerid, 430.000000, baseY+(playerid, i*14), "Sluzba");
		PlayerTextDrawAlignment(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 2);
		PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 255);
		PlayerTextDrawFont(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 1);
		PlayerTextDrawLetterSize(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], -1);
		PlayerTextDrawSetOutline(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 1);
		PlayerTextDrawSetProportional(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 1);
		PlayerTextDrawUseBox(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 1);
		PlayerTextDrawBoxColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 112);
		PlayerTextDrawTextSize(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+2], 1);

		pInfo[playerid][GroupsListStaticButtons][(i*5)+3] = CreatePlayerTextDraw(playerid, 473.000000, baseY+(playerid, i*14), "Magazyn");
		PlayerTextDrawAlignment(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 2);
		PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 255);
		PlayerTextDrawFont(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 1);
		PlayerTextDrawLetterSize(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], -1);
		PlayerTextDrawSetOutline(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 1);
		PlayerTextDrawSetProportional(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 1);
		PlayerTextDrawUseBox(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 1);
		PlayerTextDrawBoxColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 112);
		PlayerTextDrawTextSize(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+3], 1);

		pInfo[playerid][GroupsListStaticButtons][(i*5)+4] = CreatePlayerTextDraw(playerid, 516.000000, baseY+(playerid, i*14), "Online");
		PlayerTextDrawAlignment(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 2);
		PlayerTextDrawBackgroundColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 255);
		PlayerTextDrawFont(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 1);
		PlayerTextDrawLetterSize(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], -1);
		PlayerTextDrawSetOutline(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 1);
		PlayerTextDrawSetProportional(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 1);
		PlayerTextDrawUseBox(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 1);
		PlayerTextDrawBoxColor(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 112);
		PlayerTextDrawTextSize(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, pInfo[playerid][GroupsListStaticButtons][(i*5)+4], 1);
	}
}

stock CreateGroupStaticTextdraws()
{
	GroupsListStaticHeader = TextDrawCreate(104.000000, 172.000000, "~y~SLOT           NAZWA GRUPY                                          OPCJE");
	TextDrawBackgroundColor(GroupsListStaticHeader, 255);
	TextDrawFont(GroupsListStaticHeader, 2);
	TextDrawLetterSize(GroupsListStaticHeader, 0.280000, 0.799999);
	TextDrawColor(GroupsListStaticHeader, -1);
	TextDrawSetOutline(GroupsListStaticHeader, 1);
	TextDrawSetProportional(GroupsListStaticHeader, 1);
}

stock DestroyPlayerTextdraws(playerid)
{
	for(new i=0;i<5;i++)
	{
		PlayerTextDrawDestroy(playerid, pInfo[playerid][GroupsListRow][i]);
	}
	
	for(new y=0;y<25;y++)
	{
		PlayerTextDrawDestroy(playerid, pInfo[playerid][GroupsListStaticButtons][y]);
	}
	
	PlayerTextDrawDestroy(playerid, pInfo[playerid][gymTd]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][informationTd]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][Dashboard]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][DoorInfo][0]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][DoorInfo][1]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][DoorInfo][2]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][AreaInfo]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][leftTime]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][GroupDutyTag]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][DashboardPane][0]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][DashboardPane][1]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][vehicleFuelInfo]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][OfferTD][0]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][OfferTD][1]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][OfferTD][2]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][OfferTD][3]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][OfferTD][4]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][OfferTD][5]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][BusInfo]);

	PlayerTextDrawDestroy(playerid, pInfo[playerid][CarColorPicker][0]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][CarColorPicker][1]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][CarColorPicker][2]);
	PlayerTextDrawDestroy(playerid, pInfo[playerid][CarColorPicker][3]);

	PlayerTextDrawDestroy(playerid, InfoBoxTextdraw[InfoBox::RIGHT_BIG][playerid]);
	PlayerTextDrawDestroy(playerid, InfoBoxTextdraw[InfoBox::RIGHT][playerid]);
	PlayerTextDrawDestroy(playerid, InfoBoxTextdraw[InfoBox::CENTER][playerid]);
	PlayerTextDrawDestroy(playerid, InfoBoxTextdraw[InfoBox::CENTER_SMALL][playerid]);
	PlayerTextDrawDestroy(playerid, InfoBoxTextdraw[InfoBox::UNDER_HUD][playerid]);
}