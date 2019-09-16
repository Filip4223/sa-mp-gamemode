stock OnPlayerUseDrugs(playerid, itemid) 
{
	new Float:quality = floatdiv(Item[itemid][item_value2], 100);
	#pragma unused quality

	new drugtime = 60;

	switch(Item[itemid][item_value1])
	{
		case DRUG_LSD:
		{

		}

		case DRUG_COCAINE:
		{

		}

		case DRUG_CRACK:
		{

		}

		case DRUG_META:
		{
			drugtime = 60 * 10;
		}

		case DRUG_MARIHUANA:
		{

		}

		case DRUG_HEROIN:
		{

		}

		case DRUG_AMFA:
		{
			
		}

		case DRUG_PROTEIN:
		{
			switch(Item[itemid][item_value3])
	        {
	            case SUPLEMENT_TYPE_TIME:
	            {
	                if( pInfo[playerid][player_is_gym_training] ) return Alert(playerid, ALERT_TYPE_NEGATIVE, "Tego suplementu nie mo¿esz u¿ywaæ w trakcie æwiczenia.");
	                if( gettime() - pInfo[playerid][player_last_train] >= GYM_TRAIN_DELAY ) return Alert(playerid, ALERT_TYPE_NEGATIVE, "Nie czujesz potrzeby za¿ywania tej substancji.");
					
					pInfo[playerid][player_last_train] -= ((60 * 60) * 3);
	                mysql_pquery(g_sql, sprintf("UPDATE crp_characters SET char_last_train = %d WHERE char_uid = %d", pInfo[playerid][player_last_train], pInfo[playerid][player_id]));
	                
	                Alert(playerid, ALERT_TYPE_SUCCESS, "Czujesz sie troche lepiej, nastepny trening bedziesz mogl zaczac 3 godziny szybciej.");
	            }

	            case SUPLEMENT_TYPE_MOREPOWER:
	            {
	                pInfo[playerid][player_suplement_type] = SUPLEMENT_TYPE_MOREPOWER;
	                Alert(playerid, ALERT_TYPE_SUCCESS, "Od teraz Twoje punkty sily zdobywane podczas cwiczenia zostana odrobine zwiekszone.");
	                Gym_OnPlayerUseSuple(playerid);
	            }
	        }
		}
	}

	if( Item[itemid][item_value1] != DRUG_PROTEIN )
	{
		pInfo[playerid][player_drug_used] = true;
		pInfo[playerid][player_drug_type] = Item[itemid][item_value1];
		pInfo[playerid][player_drug_start] = gettime();
		pInfo[playerid][player_drug_time] = drugtime;
	}

	ProxMessage(playerid, sprintf("za¿ywa %s", DrugTypes[Item[itemid][item_value1]]), PROX_AME);

	Item[itemid][item_amount] -= 1;

	if(Item[itemid][item_amount] <= 0) {
		DeleteItem(itemid, true);
	}
	else
	{
		mysql_pquery(g_sql, sprintf("UPDATE crp_items SET item_value1 = %d WHERE item_uid = %d", Item[itemid][item_value1], Item[itemid][item_uid]));
	}
	return 1;
}

stock OnPlayerDrugEffectEnd(playerid)
{

}