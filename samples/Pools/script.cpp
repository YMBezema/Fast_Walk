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
	float	mbrMax;
	float	mbrMin;
	float	mbrMaxSaved;
	float	mbrIncrement;
	bool	changeSpeed;

public:
	FWOC()
	{
		mbrMax = 1.f;
		mbrMin = 0.f;
		mbrMaxSaved = mbrMax;
		mbrIncrement = 0.5f;
		changeSpeed = false;
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
	void doFWOC(Player playerId, Ped player)
	{
		float	moveBlendRatio = TASK::GET_PED_DESIRED_MOVE_BLEND_RATIO(player);
		float	x = BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE(0, KEY::LS_X));
		float	y = BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE(0, KEY::LS_Y));
		bool	sprintPressed = PAD::IS_CONTROL_JUST_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		bool	sprintHeld = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		//		float	deadzone = 10.f;
		//		bool	isStickInput = IsStickInput(x, y, deadzone);
		float	mbrMin = 0.f;
		bool	sprintReleased = PAD::IS_CONTROL_JUST_RELEASED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		float	lsMag = GetStickMagnitude(x, y);
		bool	lsMax = lsMag > 125;
		float	desiredMBR = moveBlendRatio;
		bool	controlFlag = false;
		//bool	isAiming = PLAYER::IS_PLAYER_FREE_AIMING(playerId);
		bool	isInCombat = PED::IS_PED_IN_COMBAT(player, 0);
//		int		deadeyeAttributePoints = ATTRIBUTE::GET_MAX_ATTRIBUTE_RANK(player, 2);
		int		deadeyeCoreValue = ATTRIBUTE::_GET_ATTRIBUTE_CORE_VALUE(player, 2);

		if(!isInCombat && !ATTRIBUTE::_IS_ATTRIBUTE_OVERPOWERED(player, 2))
			PLAYER::_0x2498035289B5688F(playerId, deadeyeCoreValue);


		//reset max speed if player reaches standstil
		if (moveBlendRatio == 0 || (moveBlendRatio < 1.0 && !sprintHeld))
			mbrMax = 1.f;

		//Handle maxMBR when sprint pressed
		if (sprintPressed)
		{
			if (lsMax) // if the stick is pushed all the way increment momentum
			{
				mbrMax = min(mbrMax + mbrIncrement, 3.f);
			}
			else //player is walking slow, increase speed to first increment.
			{
				mbrMax = max(1.f + mbrIncrement, mbrMax);
			}
		}


		//Sprint is held: speed is set to a fraction of the current momentum determined by magnitude of the stick input.
		if (sprintHeld || sprintPressed)
		{
			mbrMin = GetFractionByMagnitude(mbrMax, lsMag);
		}
		else if (sprintReleased) //Sprint just got released: momentum is set to the current moveblendratio 
		{
			mbrMaxSaved = mbrMax; //the previous momentum is saved so the 
			mbrMin = mbrMaxSaved;
			mbrMax = max(1.f, BUILTIN::CEIL(GetFractionByMagnitude(mbrMin, lsMag) * 2.f) / 2.f);
		}
		else if (IsStickInput(x, y, 1))
		{
			if (lsMax) //reset saved momentum if stick is pressed all the way again.
				mbrMaxSaved = mbrMax;
			mbrMin = min(mbrMax, GetFractionByMagnitude(mbrMaxSaved, lsMag));
			mbrMax = max(1.f, BUILTIN::CEIL(mbrMin * 2.f) / 2.f);
		}






		if (moveBlendRatio > 1.f && !(moveBlendRatio == 1.35f))
			PED::SET_PED_MIN_MOVE_BLEND_RATIO(player, mbrMin);
		//if(moveBlendRatio >= 1.f && mbrMax != 1.f)
		PED::SET_PED_MAX_MOVE_BLEND_RATIO(player, mbrMax);



		/*
		char buffer[32];
		//		snprintf(buffer, 32, "ratio: %f\rRADIUS: %F", move_blend_ratio, ls_radius );
		snprintf(buffer, 32, "%i\r%i", deadeyeAttributePoints, deadeyeCoreValue);
		DrawText(buffer, 0.01, 0.01);
		snprintf(buffer, 32, "%f", moveBlendRatio );
		DrawText(buffer, 0.01, 0.2);
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
		fwoc.doFWOC(player, playerPed);

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