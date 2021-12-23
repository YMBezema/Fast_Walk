/*
	THIS FILE IS A PART OF RDR 2 SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2019
*/

#include "script.h"
#include <string>
#include <vector>
#include <math.h>

/* DEBUG - Display text
void DrawText(const char* text, float x, float y)
{
	//HUD::SET_TEXT_CENTRE(true);
	const char* literalString = MISC::VAR_STRING(10, "LITERAL_STRING", text);
	HUD::_DISPLAY_TEXT(literalString, x, y);

}
*/
typedef struct 
{
    float x, y;
} VECTOR;

void normalize(VECTOR* p)
{
	float w = sqrt( p->x * p->x + p->y * p->y + p->z * p->z );
    p->x /= w;
    p->y /= w;
    p->z /= w;
}


float GetStickRadiusNormalized(float x, float y)
{
	//normalize
	struct input = {x, y};

	normalize(struct);
	
	/* DEBUG - Display values on screen
	*/
	char buffer[32];
	snprintf(buffer, 32, "x_n: %f\r y_n: %f", struct->x, struct->y);
	DrawText(buffer, 0.6, 0.6);

	return radius;
}


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
	int		fastWalkThreshold	= 10;
	int		x_input, y_input;	//Stick registers input values from 0 - 254, where the radius is 127
	int		ls_radius;
	float	move_blend_ratio;
	bool	a_pressed;

	// check if player ped exists and control is on (e.g. not in a cutscene)
	if (!ENTITY::DOES_ENTITY_EXIST(playerPed) || !PLAYER::IS_PLAYER_CONTROL_ON(player))
		return;

	move_blend_ratio	= TASK::GET_PED_DESIRED_MOVE_BLEND_RATIO(playerPed);
	x_input				=  PAD::GET_CONTROL_VALUE( 0, 4216773979);
	y_input				=  PAD::GET_CONTROL_VALUE( 0, 152139984 );
	a_pressed			=  PAD::IS_CONTROL_PRESSED(0, 3350541322);

	//Check to see if player is not trying to run
	if ( !a_pressed && move_blend_ratio < 2 && move_blend_ratio >= 1)
	{
		ls_radius = GetStickRadius(x_input, y_input);

		if (ls_radius <= fastWalkThreshold || ls_radius >= 127 - fastWalkThreshold)
		{
			//Force fast walking 
			PED::SET_PED_MIN_MOVE_BLEND_RATIO(playerPed, 1.5f);
			PED::SET_PED_MAX_MOVE_BLEND_RATIO(playerPed, 1.5f);
		}
	}

	/* DEBUG - Display values on screen
	char buffer[32];
	snprintf(buffer, 32, "ratio: %f\rx: %d\ry: %d", move_blend_ratio, x_input, y_input);
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