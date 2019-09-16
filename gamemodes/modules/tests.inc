//Automatic tests

static tests[][8] =
{
	"Areas",
};

stock StartTests()
{
	CheckAreas();
}

stock AT_Error(type, str[])
{
	printf("[AT-ERR] [%s] %s", tests[type], str);
}

stock CheckAreas()
{
	new Cache:result = mysql_query(m_sql, "SELECT * FROM samp_areas");

	if(cache_get_rows() < 3600) AT_Error(0, "There are not enough zones in the database.");

	cache_delete(result);
}