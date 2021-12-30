/*
	THIS FILE IS A PART OF RDR 2 SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2019
*/

#include "script.h"
#include <string>
#include <math.h>
#include <vector>

/* DEBUG - Method for drawing text on screen
*/
void DrawText(const char* text, float x, float y)
{
	//HUD::SET_TEXT_CENTRE(true);
	const char* literalString = MISC::VAR_STRING(10, "LITERAL_STRING", text);
	HUD::_DISPLAY_TEXT(literalString, x, y);

}

//Method for finding the radius of a stick using x and y coordinates.
float GetStickRadius(float x, float y)
{
	float px		= BUILTIN::POW( x - 127.f, 2);
	float py		= BUILTIN::POW( y - 127.f, 2);
	float radius	= BUILTIN::SQRT(px + py);

	return min(radius, 127);
}

bool fastWalkEnabled = true;


bool CheckMovement()
{
	bool w = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_MOVE_UP_ONLY"));
	bool a = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_MOVE_LEFT_ONLY"));
	bool s = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_MOVE_DOWN_ONLY"));
	bool d = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_MOVE_RIGHT_ONLY"));

	return w || a || s || d;
}

float	target_moveBlendRatio = 0.f;
float	u = 1.f;

float clip(float n, float lower, float upper) {
	return max(lower, (n, upper));
}

void update()
{
	Player	player				= PLAYER::PLAYER_ID();
	Ped		playerPed			= PLAYER::PLAYER_PED_ID();
	float	lsX, lsY;	//Stick registers input values from 0 - 254, where the radius is 127
	float	lsMagnitude = 0.f;
	float	moveBlendRatio = 0.f;
	bool	keySprint, keyPTT, isMoving = false;
	float temp;
	float test;

	temp = 0.f;


	// check if player ped exists and control is on (e.g. not in a cutscene)
	if (!ENTITY::DOES_ENTITY_EXIST(playerPed) || !PLAYER::IS_PLAYER_CONTROL_ON(player))
		return;

	lsX				= BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE( 0, KEY::LS_X));
	lsY				= BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE( 0, KEY::LS_Y));
	lsMagnitude		= 0.f;
	moveBlendRatio	= TASK::GET_PED_DESIRED_MOVE_BLEND_RATIO(playerPed);
	keySprint		= PAD::IS_CONTROL_JUST_RELEASED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
//	keySprint		= PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
	keyPTT			= PAD::IS_CONTROL_JUST_RELEASED(0, MISC::GET_HASH_KEY("INPUT_PUSH_TO_TALK"));

	lsMagnitude = GetStickRadius(lsX, lsY);

	//if walking at max speed and giving stick input
	if (moveBlendRatio >= 1.f && lsMagnitude >= 5)
	{
		//if stick pressed roughly all the way
		if (lsMagnitude > 125)
		{
			if (keySprint)
			{
				u = min(u + 0.25f, 3.f);
			}
		}
		else
		{
			//temp = 1.f + (3.f * ( (lsMagnitude * (2.f / 3.f)) / 127.f)); //get fraction of moveblend ratio between 1.0f and 3.f
			temp = 1 + ((u - 1) * (lsMagnitude / 127.f));
			u = BUILTIN::CEIL(temp * 4.f) / 4.f;
		}

		target_moveBlendRatio = u;
		PED::SET_PED_MIN_MOVE_BLEND_RATIO(playerPed, target_moveBlendRatio);
		PED::SET_PED_MAX_MOVE_BLEND_RATIO(playerPed, target_moveBlendRatio);
	}
	else
		u = 1.f;


	/* DEBUG - showing some of the variables on screen
	*/
	char buffer[32];
	//snprintf(buffer, 32, "ratio: %f\rRADIUS: %F", move_blend_ratio, ls_radius );
	//snprintf(buffer, 32, "%f\r%f\r%f\r%f", u, moveBlendRatio, target_moveBlendRatio, temp);
	snprintf(buffer, 32, "%f\r%f\r%f", u, target_moveBlendRatio, moveBlendRatio);
	DrawText(buffer, 0.6, 0.6);

	/*
	if( keyPTT )
		fastWalkEnabled = !fastWalkEnabled;


	//Check to see if player is not trying to run
	if ( !keySprint && moveBlendRatio > 0.2f && moveBlendRatio < 2.0f)
	{
		lsMagnitude = GetStickRadius(lsX, lsY);

		if ( lsMagnitude >= 5 && lsMagnitude < 127.f)
		{
			target_moveBlendRatio = 1.5f * (lsMagnitude / 127.f);

			PED::SET_PED_MIN_MOVE_BLEND_RATIO(playerPed, target_moveBlendRatio);
			PED::SET_PED_MAX_MOVE_BLEND_RATIO(playerPed, target_moveBlendRatio);
		}
		else 
		if (fastWalkEnabled && CheckMovement())
		{
			PED::SET_PED_MIN_MOVE_BLEND_RATIO(playerPed, 1.5f);
			PED::SET_PED_MAX_MOVE_BLEND_RATIO(playerPed, 1.5f);
		}
	}
	*/

}

class FWOC {

};


void main()
{	
	while (true)
	{
		update();
		WAIT(0);
	}
}

void ScriptMain()
{	
	srand(GetTickCount());
	main();
}