stock LSPDDB_DisplayMainPage(playerid)
{
	DynamicGui_Init(playerid);

	new string[512];
	strcat(string, HEX_COLOR_WHITE"Los Santos Emergency Database\n"); //header

	strcat(string, HEX_COLOR_GREY"Witaj w bazie danych s�u�b porz�dkowych Los Santos.\n");
	DynamicGui_AddBlankRow(playerid);

	strcat(string, HEX_COLOR_GREY"Aplikacja umo�liwia dok�adne prowadzenie dokumentacji.\n");
	DynamicGui_AddBlankRow(playerid);

	strcat(string, HEX_COLOR_GREY"Na chwil� obecn� umo�liwiamy przechowywanie informacji o\n");
	DynamicGui_AddBlankRow(playerid);

	strcat(string, HEX_COLOR_GREY"biznesach, pojazdach oraz osobach prywatnych.\n");
	DynamicGui_AddBlankRow(playerid);

	strcat(string, HEX_COLOR_WHITE"-\n");
	DynamicGui_AddBlankRow(playerid);

	strcat(string, HEX_COLOR_SAMP"Aby przej�� dalej wybierz jedn� z opcji poni�ej.\n");
	DynamicGui_AddBlankRow(playerid);

	strcat(string, HEX_COLOR_WHITE"{8380df}� {ffffff}Baza danych os�b prywatnych.\n");
	DynamicGui_AddRow(playerid, DG_FILES_SEARCH_CHAR);

	strcat(string, HEX_COLOR_WHITE"{8380df}� {ffffff}Baza danych pojazd�w.\n");
	DynamicGui_AddRow(playerid, DG_FILES_SEARCH_VEH);

	strcat(string, HEX_COLOR_WHITE"{8380df}� {ffffff}Baza danych grup.\n");
	DynamicGui_AddRow(playerid, DG_FILES_SEARCH_GROUP);

	return ShowPlayerDialog(playerid, DIALOG_FILES, DIALOG_STYLE_TABLIST_HEADERS, "Kartoteka s�u�b porz�dkowych", string, "Wybierz", "Zamknij");
}