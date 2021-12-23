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

class FWOC
{
private:
	float	momentum;
	float	momentumSaved;
	float	momentumIncrement;
	bool 	fastWalkEnabled;

public:
	FWOC()
	{
		fastWalkEnabled = true;
		momentum = 1.f;
		momentumSaved = momentum;
		momentumIncrement = 0.25f;
	}

	//Method for finding the magnitude of an analogue stick using its x and y coordinates.
	float GetStickMagnitude(float x, float y)
	{
		float px = BUILTIN::POW(x - 127.f, 2);
		float py = BUILTIN::POW(y - 127.f, 2);
		float radius = BUILTIN::SQRT(px + py);

		return min(radius, 127);
	}

	//Method for getting a fraction of a variable according to the magnitude of an analogue stick input.
	float GetFractionByMagnitude(float x, float magnitude)
	{
		return x * (magnitude / 127.f);
	}

	//Main method for determining the player's move blend ratio
	void doFWOC(Ped player)
	{
		float	moveBlendRatio			= TASK::GET_PED_DESIRED_MOVE_BLEND_RATIO(player);
		bool	sprintPressed			= PAD::IS_CONTROL_JUST_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		bool	sprintHold				= PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		bool	sprintReleased			= PAD::IS_CONTROL_JUST_RELEASED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		float	x						= BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE(0, KEY::LS_X));
		float	y						= BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE(0, KEY::LS_Y));
		float	lsMag					= GetStickMagnitude(x, y);
		float	target_moveBlendRatio	= moveBlendRatio;
		bool	lsMax					= false;
		float	desiredMBR = moveBlendRatio;

		/*
		*	Momentum is the current desired maximum speed if the player pushes the movement input all the way.
		*/

		lsMax = lsMag > 125;

		if (sprintPressed)
		{
			if (desiredMBR <= 1.5f)
			{

			}
			else if (lsMax || momentum >= 1.f) // if the stick is pushed all the way or the player is already past walking speed, increment momentum
			{
				// Momentum is clipped between default walking speed and max running speed, and increased.
				momentum = max(1.f, momentum);
				momentum = min(momentum + momentumIncrement, 3.f);
			}
			else //player is walking slow, increase speed to first increment.
			{
				momentum = max(1.f + momentumIncrement, momentum);
			}
		}

		//Sprint is held: speed is set to a fraction of the current momentum determined by magnitude of the stick input.
		if (momentum > 1.f)
		{

		}
		
		if (sprintHold)
		{
			target_moveBlendRatio = GetFractionByMagnitude(momentum, lsMag);
		}
		else if (sprintReleased) //Sprint just got released: momentum is set to the current moveblendratio 
		{
			momentumSaved = momentum; //the previous momentum is saved so the 
			target_moveBlendRatio = momentumSaved;
			momentum = max(1.f, BUILTIN::CEIL(GetFractionByMagnitude(target_moveBlendRatio, lsMag) * 4.f) / 4.f);
		}
		else
		{
			if (lsMax)
				momentumSaved = momentum;
			target_moveBlendRatio = min(momentum, GetFractionByMagnitude(momentumSaved, lsMag));
			momentum = max(1.f, BUILTIN::CEIL(target_moveBlendRatio * 4.f) / 4.f);
		}

		if ((momentum > 1.f && moveBlendRatio >= 0.15f) || sprintHold)
		{
			PED::SET_PED_MIN_MOVE_BLEND_RATIO(player, target_moveBlendRatio);
			PED::SET_PED_MAX_MOVE_BLEND_RATIO(player, target_moveBlendRatio);
		}
		
		/*
		*/
		char buffer[32];
		//snprintf(buffer, 32, "ratio: %f\rRADIUS: %F", move_blend_ratio, ls_radius );
		//snprintf(buffer, 32, "%f\r%f\r%f\r%f", u, moveBlendRatio, target_moveBlendRatio, temp);
		snprintf(buffer, 32, "          desired: %f", desiredMBR);
		DrawText(buffer, 0.6, 0.6);
		snprintf(buffer, 32, "    max speed: %f", momentum);
		DrawText(buffer, 0.6, 0.65);
		snprintf(buffer, 32, "current speed: %f", moveBlendRatio);
		DrawText(buffer, 0.6, 0.7);
	}

};

FWOC fwoc;

void update()
{
	Player	player = PLAYER::PLAYER_ID();
	Ped		playerPed = PLAYER::PLAYER_PED_ID();

	// check if player ped exists and control is on (e.g. not in a cutscene)
	if (ENTITY::DOES_ENTITY_EXIST(playerPed) || PLAYER::IS_PLAYER_CONTROL_ON(player))
		fwoc.doFWOC(playerPed);

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