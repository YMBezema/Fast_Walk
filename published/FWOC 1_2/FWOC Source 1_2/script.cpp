/*
	THIS FILE IS A PART OF RDR 2 SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2019
*/

#include "script.h"
#include <math.h>


void DrawText(const char* text, float x, float y)
{
	//HUD::SET_TEXT_CENTRE(true);
	const char* literalString = MISC::VAR_STRING(10, "LITERAL_STRING", text);
	HUD::_DISPLAY_TEXT(literalString, x, y);

}
//Method for finding the radius of a stick using x and y coordinates.
float GetStickRadius(int x, int y)
{
	float px		= pow(x - 127, 2);
	float py		= pow(y - 127, 2);
	float radius	= pow(px + py, 0.5);

	return min(radius, 127.f);
}

void update()
{
	Player	player				= PLAYER::PLAYER_ID();
	Ped		playerPed			= PLAYER::PLAYER_PED_ID();
	int		x_input, y_input;	//Stick registers input values from 0 - 254, where the radius is 127
	float	ls_radius;
	float	target_move_blend_ratio, move_blend_ratio;
	bool	sprint_pressed;

	// check if player ped exists and control is on (e.g. not in a cutscene)
	if (!ENTITY::DOES_ENTITY_EXIST(playerPed) || !PLAYER::IS_PLAYER_CONTROL_ON(player))
		return;

	move_blend_ratio	= TASK::GET_PED_DESIRED_MOVE_BLEND_RATIO(playerPed);
	x_input				=  PAD::GET_CONTROL_VALUE( 0, 4216773979);
	y_input				=  PAD::GET_CONTROL_VALUE( 0, 152139984 );
	sprint_pressed		=  PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));

	ls_radius = GetStickRadius(x_input, y_input);
	//Check to see if player is not trying to run
	if ( !sprint_pressed && ls_radius >= 5 && move_blend_ratio < 2.0f)
	{
		
		target_move_blend_ratio = 1.5f * (ls_radius / 127.f);

		PED::SET_PED_MIN_MOVE_BLEND_RATIO(playerPed, target_move_blend_ratio);
		PED::SET_PED_MAX_MOVE_BLEND_RATIO(playerPed, target_move_blend_ratio);
	}
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