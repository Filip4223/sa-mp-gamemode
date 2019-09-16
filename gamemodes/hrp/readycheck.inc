new RDC_Checks[][256] = {
	"SELECT true FROM crp_gspawns"
};

new RDC_ChecksName[][256] = {
	"There is at least one global spawn created"
};


/** @calledAt  @POST@ OnGameModeInit **/
stock hook_ReadyCheck()
{
	new 
		errorCount = 0,
		Cache:result;

	print("============ READY CHECK ============");
	
	for(new k;k<sizeof(RDC_Checks);k++)
	{
		result = mysql_query(g_sql, RDC_Checks[k]);

		if( !cache_get_rows() ) 
		{
			errorCount++;

			printf("|  FAIL    %s", RDC_ChecksName[k]);
		}
		else
		{
			printf("|  OK      %s", RDC_ChecksName[k]);
		}

		cache_delete(result);
	}

	if( errorCount ) {
		print("  ");
		printf("| Only %d/%d tests have passed%s", sizeof(RDC_Checks)-errorCount, sizeof(RDC_Checks), (Setting[setting_run_mode] == RunMode::PROD) ? (", gamemode cannot be started without dev mode enabled.") : (", still gamemode can be launched in dev mode."));
	}

	print("=====================================");

	if( errorCount && Setting[setting_run_mode] == RunMode::PROD ) {
		return SendRconCommand("exit");
	}
	return 1;
}