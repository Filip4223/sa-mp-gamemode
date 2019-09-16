const PLAYER_SYNC = 207;
const VEHICLE_SYNC = 200;
const PASSENGER_SYNC = 211;
const TRAILER_SYNC = 210;
const UNOCCUPIED_SYNC = 209;
const AIM_SYNC = 203;
const BULLET_SYNC = 206;

new Float:lastPlayerPos[MAX_PLAYERS][3];

#define DEBUG_PLAYER_SYNC true
#define DEBUG_UNOCCUPIED_SYNC true

IPacket:PLAYER_SYNC(playerid, BitStream:bs)
{
    new onFootData[PR_OnFootSync];

    BS_IgnoreBits(bs, 8);
    BS_ReadOnFootSync(bs, onFootData);

    if( !playerTeleportedByServer[playerid]  && !pInfo[playerid][player_admin_duty]) {
        if( lastPlayerPos[playerid][0] != onFootData[PR_position][0] && lastPlayerPos[playerid][1] != onFootData[PR_position][1] && lastPlayerPos[playerid][2] != onFootData[PR_position][2] ) {
            if( floatabs(lastPlayerPos[playerid][0] - onFootData[PR_position][0]) > 0.5 && floatabs(lastPlayerPos[playerid][1] - onFootData[PR_position][1]) > 0.5 ) {
                if( onFootData[PR_velocity][0] == 0 && onFootData[PR_velocity][1] == 0 && onFootData[PR_velocity][2] == 0 ) {
                    /*new str[80];
                    format(str, sizeof(str), "Teleport hack (DX: %.1f, DY: %.1f, V:%.0f/%.0f/%.0f)", floatabs(lastPlayerPos[playerid][0] - onFootData[PR_position][0]), floatabs(lastPlayerPos[playerid][1] - onFootData[PR_position][1]), onFootData[PR_velocity][0], onFootData[PR_velocity][1], onFootData[PR_velocity][2]);
                    AddPlayerPenalty(playerid, PENALTY_TYPE_KICK, INVALID_PLAYER_ID, 0, str);*/
                    return 1;
                }
            }
        }
    }


    new Float:vx = floatabs(lastPlayerPos[playerid][0]) - floatabs(onFootData[PR_position][0]);
    new Float:vy = floatabs(lastPlayerPos[playerid][1]) - floatabs(onFootData[PR_position][1]);
    new Float:vz = floatabs(lastPlayerPos[playerid][2]) - floatabs(onFootData[PR_position][2]);

    new Float:diff[3];
    diff[0] = vx - floatabs(onFootData[PR_velocity][0]);
    diff[1] = vy - floatabs(onFootData[PR_velocity][1]);
    diff[2] = vz - floatabs(onFootData[PR_velocity][2]);
    
    new Float:diffsum = floatabs(diff[0]) + floatabs(diff[1]) + floatabs(diff[2]);
    if(diffsum > 5.0 && !playerTeleportedByServer[playerid] && !pInfo[playerid][player_admin_duty]) pInfo[playerid][player_ac_warns]++;

    if(pInfo[playerid][player_ac_warns] > 5 && !pInfo[playerid][player_admin_duty])
    {
        pInfo[playerid][player_ac_warns] = 0;
        new str[64];
        format(str, sizeof(str), "AntyCheat: Rapid movement m1 ds%.2f dx%.2f dy%.2f dz%.2f", diffsum, floatabs(diff[0]), floatabs(diff[1]), floatabs(diff[2]));
        if( pInfo[playerid][player_aj] <= 0 ) AddPlayerPenalty(playerid, PENALTY_TYPE_AJ, INVALID_PLAYER_ID, 0, str, 30);
    }

    playerTeleportedByServer[playerid] = false;
    lastPlayerPos[playerid][0] = onFootData[PR_position][0];
    lastPlayerPos[playerid][1] = onFootData[PR_position][1];
    lastPlayerPos[playerid][2] = onFootData[PR_position][2];

    if( pInfo[playerid][player_admin_duty] && DEBUG_PLAYER_SYNC ) 
    {
        InfoboxCenter(playerid, 1, sprintf(
            "lrKey %d udKey %d keys %d ~n~position: %.2f %.2f %.2f quaternion %.2f %.2f %.2f %.2f health %d armour %d ~n~additionalKey %d weaponId %d specialAction %d ~n~velocity %.2f %.2f %.2f surfingOffsets %.2f %.2f %.2f ~n~surfingVehicleId %d animationId %d animationFlags",
            onFootData[PR_lrKey],
            onFootData[PR_udKey],
            onFootData[PR_keys],
            onFootData[PR_position][0],
            onFootData[PR_position][1],
            onFootData[PR_position][2],
            onFootData[PR_quaternion][0],
            onFootData[PR_quaternion][1],
            onFootData[PR_quaternion][2],
            onFootData[PR_quaternion][3],
            onFootData[PR_health],
            onFootData[PR_armour],
            onFootData[PR_additionalKey],
            onFootData[PR_weaponId],
            onFootData[PR_specialAction],
            onFootData[PR_velocity][0],
            onFootData[PR_velocity][1],
            onFootData[PR_velocity][2],
            onFootData[PR_surfingOffsets][0],
            onFootData[PR_surfingOffsets][1],
            onFootData[PR_surfingOffsets][2],
            onFootData[PR_surfingVehicleId],
            onFootData[PR_animationId],
            onFootData[PR_animationFlags]
            )
        );
    }
    return 1;
}

IPacket:UNOCCUPIED_SYNC(playerid, BitStream:bs)
{
    new unoccupiedData[PR_UnoccupiedSync];

    BS_IgnoreBits(bs, 8);
    BS_ReadUnoccupiedSync(bs, unoccupiedData);

    if( pInfo[playerid][player_admin_duty] && DEBUG_UNOCCUPIED_SYNC ) 
    {
        InfoboxCenter(playerid, 1, sprintf(
            "vehicleId %d seatId %d ~n~roll %.2f %.2f %.2f direction %.2f %.2f %.2f position %.2f %.2f %.2f ~n~velocity %.2f %.2f %.2f angularVelocity %.2f %.2f %.2f vehicleHealth %.2f",
            unoccupiedData[PR_vehicleId],
            unoccupiedData[PR_seatId],
            unoccupiedData[PR_roll][0],
            unoccupiedData[PR_roll][1],
            unoccupiedData[PR_roll][2],
            unoccupiedData[PR_direction][0],
            unoccupiedData[PR_direction][1],
            unoccupiedData[PR_direction][2],
            unoccupiedData[PR_position][0],
            unoccupiedData[PR_position][1],
            unoccupiedData[PR_position][2],
            unoccupiedData[PR_velocity][0],
            unoccupiedData[PR_velocity][1],
            unoccupiedData[PR_velocity][2],
            unoccupiedData[PR_angularVelocity][0],
            unoccupiedData[PR_angularVelocity][1],
            unoccupiedData[PR_angularVelocity][2],
            unoccupiedData[PR_vehicleHealth])
        );
    }
    return 1;
}
