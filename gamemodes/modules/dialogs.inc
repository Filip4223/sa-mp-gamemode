public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	new text[256];
	format(text, sizeof(text), inputtext);
	mysql_escape_string(text, text);

	switch(dialogid)
	{
		case DIALOG_LOGIN:
		{
			if(!response) return ShowPlayerDialog(playerid, DIALOG_CHANGENAME, DIALOG_STYLE_INPUT, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_CHANGENAME, playerid), GetStringValue(STRING_LOGIN_BTN_CHANGE, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));

			mysql_tquery(m_sql, sprintf("SELECT `player_pass` FROM samp_players WHERE player_name = '%s'", PlayerName(playerid)), "AuthPlayer", "ds", playerid, MD5_Hash(text));
		}

		case DIALOG_LOGIN_ERROR:
		{
			if(!response) return Kick(playerid);

			Debug(sprintf("%d", strfind(text, "_", true)));

			if(strfind(text, "_", true) == -1 || strlen(text) <= 3)
			{
				GameTextForPlayer(playerid, GetStringValue(STRING_NICK_ERROR, playerid), 1000, 4);
				return ShowPlayerDialog(playerid, DIALOG_LOGIN_ERROR, DIALOG_STYLE_INPUT, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_LOGIN_NO_ACC, playerid), GetStringValue(STRING_LOGIN_BTN_CHANGE, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));
			}

			if(!SetPlayerName(playerid, text))
			{
				GameTextForPlayer(playerid, GetStringValue(STRING_NICK_ERROR, playerid), 1000, 4);
				return ShowPlayerDialog(playerid, DIALOG_LOGIN_ERROR, DIALOG_STYLE_INPUT, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_LOGIN_NO_ACC, playerid), GetStringValue(STRING_LOGIN_BTN_CHANGE, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));
			}
			
			new Cache:result;
			result = mysql_query(m_sql, sprintf("SELECT `player_uid` FROM samp_players WHERE player_name = '%s'", PlayerName(playerid)));

			if(cache_get_rows())
				ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_LOGIN, playerid), GetStringValue(STRING_LOGIN_BTN_1, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));		
			else
				ShowPlayerDialog(playerid, DIALOG_LOGIN_ERROR, DIALOG_STYLE_INPUT, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_LOGIN_NO_ACC, playerid), GetStringValue(STRING_LOGIN_BTN_CHANGE, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));
			
			cache_delete(result);
		}

		case DIALOG_CHANGENAME:
		{
			if(!response) return Kick(playerid);

			Debug(sprintf("%d", strfind(text, "_", true)));

			if(!SetPlayerName(playerid, text) || strfind(text, "_", true) == -1 || strlen(text) <= 3) return ShowPlayerDialog(playerid, DIALOG_CHANGENAME, DIALOG_STYLE_INPUT, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_CHANGENAME, playerid), GetStringValue(STRING_LOGIN_BTN_CHANGE, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));
		
			new Cache:result;
			result = mysql_query(m_sql, sprintf("SELECT `player_uid` FROM samp_players WHERE player_name = '%s'", PlayerName(playerid)));

			if(cache_get_rows())
				ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_LOGIN, playerid), GetStringValue(STRING_LOGIN_BTN_1, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));		
			else
				ShowPlayerDialog(playerid, DIALOG_LOGIN_ERROR, DIALOG_STYLE_INPUT, GetStringValue(STRING_LOGIN_HEADER, playerid), GetStringValue(STRING_LOGIN_NO_ACC, playerid), GetStringValue(STRING_LOGIN_BTN_CHANGE, playerid), GetStringValue(STRING_LOGIN_BTN_2, playerid));
			
			cache_delete(result);			
		}
	}

	return 1;
}