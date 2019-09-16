// =======================================================================================
// fly.inc
//
// Author: Norck
//
// msn:		paul_norck@hotmail.com
// icq:		44-055-21
// skype:	paul_norck
//
// Credits: Creator of SetPlayerLookAt function. Sorry, I can't remember their name
//
// you are allowed to edit this script
// you are not allowed to sell this script
//
// Please, don't remove the credits
// =======================================================================================

stock StartFly(playerid)
{
	new Float:x,Float:y,Float:z;
	GetPlayerPos(playerid,x,y,z);
	SetPlayerPos(playerid,x,y,z+5.0);
	ApplyAnimation(playerid,"PARACHUTE","PARA_steerR",6.1,1,1,1,1,0,1);
	Fly(playerid);

	GameTextForPlayer(playerid,"~r~TRYB LATANIA~n~\
		~y~~k~~GO_FORWARD~ ~w~- uruchamia latanie ~n~\
		~y~~k~~PED_SPRINT~ ~w~- lataj szybciej~n~\
		~y~~k~~SNEAK_ABOUT~ ~w~- lataj wolniej",5000,3);
}

timer Fly[30](playerid)
{
	if( !IsPlayerConnected(playerid) ) return 1;

	new k, ud,lr;
	GetPlayerKeys(playerid,k,ud,lr);
	new Float:v_x,Float:v_y,Float:v_z,
		Float:x,Float:y,Float:z;
	if(ud < 0)	// forward
	{
		GetPlayerCameraFrontVector(playerid,v_x,v_y,v_z);
		GetPlayerPos(playerid, x, y, z);
	}

	if(k & KEY_WALK)	// slow
	{
		v_x /=5.0;
		v_y /=5.0;
		v_z /=5.0;
	}
	if(k & KEY_SPRINT)	// fast
	{
		v_x *=4.0;
		v_y *=4.0;
		v_z *=4.0;
	}
	if(v_z == 0.0) 
		v_z = 0.025;
	SetPlayerVelocity(playerid,v_x,v_y,v_z);
	if(v_x == 0 && v_y == 0)
	{	
		if(GetPlayerAnimationIndex(playerid) == 959)	
			ApplyAnimation(playerid,"PARACHUTE","PARA_steerR",6.1,1,1,1,1,0,1);
	}
	else 
	{
		SetPlayerLookAt(playerid, x + v_x,  y + v_y);
		if(GetPlayerAnimationIndex(playerid) != 959)
			ApplyAnimation(playerid,"PARACHUTE","FALL_SkyDive_Accel",6.1,1,1,1,1,0,1);
	}

	if( pInfo[playerid][player_admin_fly] ) defer Fly[100](playerid);
	return 1;
}

stock StopFly(playerid)
{
	new Float:x,Float:y,Float:z;
	GetPlayerPos(playerid,x,y,z);
	SetPlayerPos(playerid,x,y,z);
}

stock SetPlayerLookAt(playerid, Float:X, Float:Y)
{
    new Float:Px, Float:Py, Float: Pa;
    GetPlayerPos(playerid, Px, Py, Pa);
    Pa = floatabs(atan((Y-Py)/(X-Px)));
    if (X <= Px && Y >= Py) Pa = floatsub(180, Pa);
    else if (X < Px && Y < Py) Pa = floatadd(Pa, 180);
    else if (X >= Px && Y <= Py) Pa = floatsub(360.0, Pa);
    Pa = floatsub(Pa, 90.0);
    if (Pa >= 360.0) Pa = floatsub(Pa, 360.0);
    SetPlayerFacingAngle(playerid, Pa);
}