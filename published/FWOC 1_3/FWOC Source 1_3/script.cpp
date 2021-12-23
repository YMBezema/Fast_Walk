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
void DrawText(const char* text, float x, float y)
{
	//HUD::SET_TEXT_CENTRE(true);
	const char* literalString = MISC::VAR_STRING(10, "LITERAL_STRING", text);
	HUD::_DISPLAY_TEXT(literalString, x, y);

}
*/

//Method for finding the radius of a stick using x and y coordinates.
float GetStickRadius(float x, float y)
{
	float px		= BUILTIN::POW( x - 127.f, 2);
	float py		= BUILTIN::POW( y - 127.f, 2);
	float radius	= BUILTIN::SQRT(px + py);

	return min(radius, 127);
}

void update()
{
	Player	player				= PLAYER::PLAYER_ID();
	Ped		playerPed			= PLAYER::PLAYER_PED_ID();
	float	x_input, y_input;	//Stick registers input values from 0 - 254, where the radius is 127
	float	ls_radius = 0.f;
	float	target_move_blend_ratio, move_blend_ratio = 0.f;
	bool	sprint_pressed = false;

	// check if player ped exists and control is on (e.g. not in a cutscene)
	if (!ENTITY::DOES_ENTITY_EXIST(playerPed) || !PLAYER::IS_PLAYER_CONTROL_ON(player))
		return;

	move_blend_ratio	= TASK::GET_PED_DESIRED_MOVE_BLEND_RATIO(playerPed);
	x_input				= BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE( 0, KEY::LS_X));
	y_input				= BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE( 0, KEY::LS_Y));
	sprint_pressed		= PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
	ls_radius			= GetStickRadius(x_input, y_input);

	//Check to see if player is not trying to run
	if ( !sprint_pressed && ls_radius >= 5 && move_blend_ratio > 0.2f && move_blend_ratio < 2.0f)
	{
		target_move_blend_ratio = 1.5f * (ls_radius / 127.f);

		PED::SET_PED_MIN_MOVE_BLEND_RATIO(playerPed, target_move_blend_ratio);
		PED::SET_PED_MAX_MOVE_BLEND_RATIO(playerPed, target_move_blend_ratio);
	}


	/* DEBUG - showing some of the variables on screen
	char buffer[32];
	//snprintf(buffer, 32, "ratio: %f\rRADIUS: %F", move_blend_ratio, ls_radius );
	snprintf(buffer, 32, "%f\r%f", move_blend_ratio, target_move_blend_ratio);
	DrawText(buffer, 0.6, 0.6);
	*/
}


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