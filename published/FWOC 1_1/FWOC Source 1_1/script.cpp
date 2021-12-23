/*
	THIS FILE IS A PART OF RDR 2 SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2019
*/

#include "script.h"
#include <string>
#include <vector>
#include <math.h>
#include <fstream>

//int sprint_hash;

/* DEBUG - Display text
*/
void DrawText(const char* text, float x, float y)
{
	//HUD::SET_TEXT_CENTRE(true);
	const char* literalString = MISC::VAR_STRING(10, "LITERAL_STRING", text);
	HUD::_DISPLAY_TEXT(literalString, x, y);

}

/*
//method for checking if file exists
bool does_file_exist(const char* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

void SetSprintHash()
{
	switch ((int)GetPrivateProfileInt("PadSprintButton", "ButtonID", 0, ".\\FastWalk.ini"))
	{
	case 0:
		sprint_hash = MISC::GET_HASH_KEY("INPUT_FRONTEND_ACCEPT");
		break;
	case 1:
		sprint_hash = MISC::GET_HASH_KEY("INPUT_FRONTEND_RB");
		break;
	case 2:
		sprint_hash = MISC::GET_HASH_KEY("INPUT_FRONTEND_LS");
		break;
	default:
		return;
		break;
	}
}
*/

//Method for finding the radius of a stick using x and y coordinates.
int GetStickRadius(int x, int y)
{
	int px = pow(x - 127, 2);
	int py = pow(y - 127, 2);
	int radius = pow(px + py, 0.5);
	
	/* DEBUG - Display values on screen
	char buffer[32];
	snprintf(buffer, 32, "radius: %d", radius);
	DrawText(buffer, 0.6, 0.6);
	*/

	return radius;
}

void update()
{
	Player	player				= PLAYER::PLAYER_ID();
	Ped		playerPed			= PLAYER::PLAYER_PED_ID();
	int		fastWalkThreshold	= 115; //max = 127
	int		x_input, y_input;	//Stick registers input values from 0 - 254, where the radius is 127
	int		ls_radius;
	float	move_blend_ratio;
	bool	sprint_pressed;

	// check if player ped exists and control is on (e.g. not in a cutscene)
	if (!ENTITY::DOES_ENTITY_EXIST(playerPed) || !PLAYER::IS_PLAYER_CONTROL_ON(player))
		return;

	move_blend_ratio	= TASK::GET_PED_DESIRED_MOVE_BLEND_RATIO(playerPed);
	x_input				=  PAD::GET_CONTROL_VALUE( 0, 4216773979);
	y_input				=  PAD::GET_CONTROL_VALUE( 0, 152139984 );
	sprint_pressed		=  PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));

	//Check to see if player is not trying to run
	if ( !sprint_pressed && move_blend_ratio >= 1 && move_blend_ratio < 2)
	{
		ls_radius = GetStickRadius(x_input, y_input);

		if (ls_radius >=  fastWalkThreshold)
		{
			//Force fast walking 
			PED::SET_PED_MIN_MOVE_BLEND_RATIO(playerPed, 1.5f);
			PED::SET_PED_MAX_MOVE_BLEND_RATIO(playerPed, 1.5f);
		}
	}

	/* DEBUG - Display values on screen
	char buffer[32];
	snprintf(buffer, 32, "pressed: %f\r", move_blend_ratio);
	DrawText(buffer, 0.6, 0.6);
	*/
}


void main()
{	
	/*
	//checking if there is an ini file (used for drawing according information on screen)
	bool iniexists = false;
	iniexists = does_file_exist(".\\FastWalk.ini");

	//getting values from the ini or setting default values, if ini not there (default represent mod version "standard" of v1.41)
	if (iniexists)
		SetSprintHash();
	*/
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