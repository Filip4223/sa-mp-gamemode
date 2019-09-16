stock GetBusByUid(buid)
{
	foreach(new bid : Buses)
	{
		if( Bus[bid][bus_uid] == buid ) return bid;
	}
	
	return -1;
}

stock GetBusPos(bid, &Float:x, &Float:y, &Float:z)
{
	new objectid = Bus[bid][bus_objectid];
	
	GetDynamicObjectPos(objectid, x, y, z);
}

stock GetPointInAngleOfObject(objectid, &Float:x, &Float:y, &Float:z, Float:distance, Float:angle)
{
	new Float:current_angle;
	GetDynamicObjectPos(objectid, x, y, z);
	
	GetDynamicObjectRot(objectid, current_angle, current_angle, current_angle);

	new Float:a = current_angle + angle;
	
	if( a > 180 ) a = -180 + (a-180);
	if( a < -180 ) a = 180 + (a+180);
	
	x += (distance * floatsin(-a, degrees));
	y += (distance * floatcos(-a, degrees));
}

stock FindNearestBus(skipid, Float:x, Float:y, Float:z)
{
	new Float:pretender_distance = 10000.0, pretender_id = -1, Float:distance;
	foreach(new bid : Buses)
	{
		if( bid == skipid ) continue;
		Streamer_GetDistanceToItem(x, y, z, STREAMER_TYPE_OBJECT, Bus[bid][bus_objectid], distance, 2);
		
		if( distance < pretender_distance )
		{
			pretender_distance = distance;
			pretender_id = bid;
		}
	}
	
	return pretender_id;
}