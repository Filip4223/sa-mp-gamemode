timer InitialCamera[1000](playerid, i)
{
	if(pPlayer[playerid][player_logged]) return 1;

	switch(i)
	{
		case 0:
		{
			InterpolateCameraPos(playerid, 1443.469848, -728.517456, 129.343460, 1471.216674, -865.390441, 97.174324, 7000);
			InterpolateCameraLookAt(playerid, 1441.945068, -732.994750, 127.722167, 1467.913818, -861.694824, 96.516082, 7000);
		}

		case 1:
		{
			InterpolateCameraPos(playerid, 725.533081, -1781.127929, 4.444662, 731.236450, -1525.306518, 46.239845, 7000);
			InterpolateCameraLookAt(playerid, 725.603027, -1776.129760, 4.331390, 735.597167, -1523.008911, 47.079593, 7000);
		}

		case 2:
		{
			InterpolateCameraPos(playerid, 2168.215820, -1142.138305, 82.641250, 2143.734130, -1106.807739, 39.625846, 7000);
			InterpolateCameraLookAt(playerid, 2169.187011, -1138.041015, 79.945060, 2148.308593, -1105.203613, 38.400493, 7000);
		}

		case 3:
		{
			InterpolateCameraPos(playerid, 2397.820800, -1850.576538, 28.541212, 2272.034423, -1851.651611, 63.161571, 7000);
			InterpolateCameraLookAt(playerid, 2392.835205, -1850.745849, 28.202836, 2268.568359, -1848.102172, 63.784946, 7000);			
		}

		case 4:
		{
			InterpolateCameraPos(playerid, 1511.942871, -1621.133911, 40.115947, 1523.758178, -1702.485595, 21.976514, 7000);
			InterpolateCameraLookAt(playerid, 1514.560791, -1625.043090, 38.423492, 1527.371826, -1699.062255, 21.504880, 7000);
		}

		case 5:
		{
			InterpolateCameraPos(playerid, 456.152160, -1989.773071, 40.127368, 333.636871, -2007.313964, 15.076543, 7000);
			InterpolateCameraLookAt(playerid, 452.238830, -1992.759887, 39.252979, 338.261138, -2009.183471, 15.424237, 7000);
		}
	}

	i++;

	defer InitialCamera[7000](playerid, i);

	return 1;
}