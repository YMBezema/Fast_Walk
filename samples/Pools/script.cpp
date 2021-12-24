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

/*
* TASK::PED_HAS_USE_SCENARIO_TASK = might be nice for removing control while player is interacting (even necessary?)
*/

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
		fastWalkEnabled = false;
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

	bool IsStickInput(float x, float y, float deadzone)
	{
		x = x - 127;
		y = y - 127;

		if (x < 0)
			x = x * -1;
		

		if (y < 0)
			y = y * -1;


		return (x > deadzone) || (y > deadzone);
	}

	//Main method for determining the player's move blend ratio
	void doFWOC(Ped player)
	{
		float	moveBlendRatio			= TASK::GET_PED_DESIRED_MOVE_BLEND_RATIO(player);
		float	x						= BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE(0, KEY::LS_X));
		float	y						= BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE(0, KEY::LS_Y));
		bool	sprintPressed			= PAD::IS_CONTROL_JUST_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		bool	sprintHold				= PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		float	deadzone = 4.f;

		//check to avoid weird looking sprint behavior on slight stick inputs.
		if (!IsStickInput(x, y, deadzone) && sprintHold)
		{
			
			PED::SET_PED_MIN_MOVE_BLEND_RATIO(player, 0);
			PED::SET_PED_MAX_MOVE_BLEND_RATIO(player, 0);
			return;
		}

		float	target_moveBlendRatio	= 1.f;
		bool	sprintReleased			= PAD::IS_CONTROL_JUST_RELEASED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		float	lsMag					= GetStickMagnitude(x, y);
		bool	lsMax					= lsMag > 125;
		float	desiredMBR = moveBlendRatio;
		bool	controlFlag = false;
		

		//bool	task = PED::IS_PED_USING_ANY_SCENARIO(player);

		/*
		*	Momentum is the current desired maximum speed if the player pushes the movement input all the way.

		if (!fastWalkEnabled)
		{
			if (!sprintPressed && !(moveBlendRatio == 1.35f || moveBlendRatio == 1.5f))
			{
				fastWalkEnabled = true;
			}
		}
		*/

		if (moveBlendRatio == 0.f)
			momentum = 1.f;

		if (sprintPressed)
		{
			if (lsMax || momentum >= 1.f) // if the stick is pushed all the way or the player is already past walking speed, increment momentum
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
		if (sprintHold || sprintPressed)
		{
			target_moveBlendRatio = GetFractionByMagnitude(momentum, lsMag);
			controlFlag = true;
		}
		else if (sprintReleased) //Sprint just got released: momentum is set to the current moveblendratio 
		{
			momentumSaved = momentum; //the previous momentum is saved so the 
			target_moveBlendRatio = momentumSaved;
			momentum = max(1.f, BUILTIN::CEIL(GetFractionByMagnitude(target_moveBlendRatio, lsMag) * 4.f) / 4.f);
			controlFlag = true;
		}
		else if (IsStickInput(x, y, deadzone))
		{
			if (lsMax) //reset saved momentum if stick is pressed all the way again.
				momentumSaved = momentum;
			target_moveBlendRatio = min(momentum, GetFractionByMagnitude(momentumSaved, lsMag));
			momentum = max(1.f, BUILTIN::CEIL(target_moveBlendRatio * 4.f) / 4.f);
			controlFlag = true;
		}

	
		if (desiredMBR == 1.35f)
			controlFlag = false;
		
		//if (lsMag >= 3 && (controlFlag && ((momentum >= 1.f && moveBlendRatio >= 0.15f) || sprintHold)))

		if (controlFlag && (momentum > 1.f || sprintHold || moveBlendRatio >= 1.0f))
		{
			target_moveBlendRatio = max(0.15f, target_moveBlendRatio);
			PED::SET_PED_MIN_MOVE_BLEND_RATIO(player, target_moveBlendRatio);
			PED::SET_PED_MAX_MOVE_BLEND_RATIO(player, target_moveBlendRatio);
		}
		
		/*
		char buffer[32];
//		snprintf(buffer, 32, "ratio: %f\rRADIUS: %F", move_blend_ratio, ls_radius );
		snprintf(buffer, 32, "%f\r%f\r%f", moveBlendRatio, x, y);
		DrawText(buffer, 0.01, 0.01);
		snprintf(buffer, 32, "momentum: %f", momentum);
		DrawText(buffer, 0.6, 0.6);
		snprintf(buffer, 32, "    max speed: %f", momentum);
		DrawText(buffer, 0.6, 0.65);
		snprintf(buffer, 32, "current speed: %f", );
		DrawText(buffer, 0.6, 0.7);
		*/
	}

};

FWOC fwoc;

void update()
{
	Player	player = PLAYER::PLAYER_ID();
	Ped		playerPed = PLAYER::PLAYER_PED_ID();

	// check if player ped exists and control is on (e.g. not in a cutscene)
	if ((ENTITY::DOES_ENTITY_EXIST(playerPed) || PLAYER::IS_PLAYER_CONTROL_ON(player)) && !PED::IS_PED_USING_ANY_SCENARIO(playerPed))
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