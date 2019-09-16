stock GetStringValue(id, playerid)
{
	new language = GetPlayerLanguage(playerid);

	new string[512] = "UNDEFINED";

	switch(id)
	{
		case STRING_WELCOME:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "Welcome to "SCRIPT_NAME", %s!", BeautifyName(playerid));
				case LAN_PL: format(string, sizeof(string), "Witaj na serwerze "SCRIPT_NAME", %s!", BeautifyName(playerid));
			}
		}
		//Login
		case STRING_LOGIN_HEADER:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), SCRIPT_MAIN_COLOR""SCRIPT_NAME" > {FFFFFF}Login");
				case LAN_PL: format(string, sizeof(string), SCRIPT_MAIN_COLOR""SCRIPT_NAME" > {FFFFFF}Logowanie");
			}
		}	

		case STRING_LOGIN:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "{EA5959}Welcome to "SCRIPT_NAME_SMALL", polish RolePlay server.\n\n{A9C4E4}Your character {FFFFFF}%s {A9C4E4}has been found in the database.\n1.\tIf the account is yours, enter your password.\n2.\tYou entered the wrong name? Enter it again below and click {FFFFFF}'Change'{A9C4E4}.", BeautifyName(playerid));
				case LAN_PL: format(string, sizeof(string), "{EA5959}Witaj na "SCRIPT_NAME_SMALL", polskim serwerze Role Play.\n\n{A9C4E4}Twoja postaæ {FFFFFF}%s {A9C4E4}zosta³a odnaleziona w bazie danych.\n1.\tJeœli konto nale¿y do Ciebie, wpisz swoje has³o.\n2.\tWpisa³eœ z³¹ nazwê? Wpisz j¹ jeszcze raz klikaj¹c {FFFFFF}'Zmieñ nick'{A9C4E4}.", BeautifyName(playerid));
			}
		}
		case STRING_LOGIN_NO_ACC:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "{EA5959}Welcome to "SCRIPT_NAME_SMALL", polish RolePlay server.\n\n{A9C4E4}Your character {FFFFFF}%s {A9C4E4}has been not found in the database.\n1.\tDo not have an account? Create account on "SCRIPT_NAME_SMALL".\n2.\tYou entered the wrong name? Enter it again by clicking {FFFFFF}'Change nickname'{A9C4E4}.", BeautifyName(playerid));
				case LAN_PL: format(string, sizeof(string), "{EA5959}Witaj na "SCRIPT_NAME_SMALL", polskim serwerze Role Play.\n\n{A9C4E4}Twoja postaæ {FFFFFF}%s {A9C4E4}nie zosta³a odnaleziona w bazie danych.\n1.\tNie posiadasz konta? Za³ó¿ je na "SCRIPT_NAME_SMALL".\n2.\tWpisa³eœ z³¹ nazwê? Wpisz j¹ jeszcze raz w polu poni¿ej i naciœnij {FFFFFF}'Zmieñ'{A9C4E4}.", BeautifyName(playerid));
			}			
		}
		case STRING_LOGIN_BTN_1:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "Login");
				case LAN_PL: format(string, sizeof(string), "Zaloguj");
			}
		}
		case STRING_LOGIN_BTN_2:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "Change");
				case LAN_PL: format(string, sizeof(string), "Zmieñ");
			}
		}
		case STRING_BAD_PASSWORD:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "~r~BAD PASSWORD!");
				case LAN_PL: format(string, sizeof(string), "~r~BLEDNE HASLO!");
			}
		}
		case STRING_LOGIN_BTN_CHANGE:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "Change");
				case LAN_PL: format(string, sizeof(string), "Zmieñ");
			}
		}
		case STRING_CHANGENAME:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "{A9C4E4}Enter new nickname below.");
				case LAN_PL: format(string, sizeof(string), "{A9C4E4}WprowadŸ poni¿ej nowy nick.");
			}
		}
		case STRING_NICK_ERROR:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "~r~Bad nickname!");
				case LAN_PL: format(string, sizeof(string), "~r~Zly nick!");
			}
		}
		case STRING_COMMAND_ERROR:
		{
			switch(language)
			{
				case LAN_ENG: format(string, sizeof(string), "You do not have permission to use this command!");
				case LAN_PL: format(string, sizeof(string), "Nie masz do tego uprawnieñ!");				
			}
		}
	}

	return _:string;
}

stock GetPlayerLanguage(playerid)
{
	return pPlayer[playerid][player_language];
}