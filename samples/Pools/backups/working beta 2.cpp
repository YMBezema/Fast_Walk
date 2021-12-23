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
	bool 	fastWalkEnabled;

public:
	FWOC()
	{
		fastWalkEnabled = true;
		momentum = 1.f;
	}

	//Method for finding the radius of a stick using x and y coordinates.
	float GetStickRadius(float x, float y)
	{
		float px = BUILTIN::POW(x - 127.f, 2);
		float py = BUILTIN::POW(y - 127.f, 2);
		float radius = BUILTIN::SQRT(px + py);

		return min(radius, 127);
	}

	float GetMomentumFracture(float lsMag, float momentum, float lower = 0.f)
	{
		return lower + ((momentum - lower) * (lsMag / 127.f));
	}

	void doFWOC(Ped player)
	{
		float	target_moveBlendRatio;
		float moveBlendRatio = TASK::GET_PED_DESIRED_MOVE_BLEND_RATIO(player);
		float x = BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE(0, KEY::LS_X));
		float y = BUILTIN::TO_FLOAT(PAD::GET_CONTROL_VALUE(0, KEY::LS_Y));
		bool sprint = PAD::IS_CONTROL_JUST_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		bool sprintHold = PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_SPRINT"));
		float lsMag = GetStickRadius(x, y);
		float momentumFracture;
		bool changeSpeed = false;
		float c;

		target_moveBlendRatio = 1.f;
		momentumFracture = 0.f;

		//Calculate momentum
		//if sprint is pressed
		if (sprint)
		{
			//if player already at full walk, set momentum += 0.25
			if (moveBlendRatio > 1.f)
			{
				momentum = max(1.f, momentum);
				momentum = min(momentum + 0.25f, 3.f);
			}
			else
				momentum = 1.25f;

			/*
			if (momentum < 2.f && sprintHold)
				momentum = 2.f;
			*/
		}

		if (momentum > 1.f)
		{
			//Set movement speed according to ls input

			//target_moveBlendRatio = momentum;
			/*
			if (momentum >= 1.75f)
				c = max(1.75f, momentum - 1.75f);
			else
			*/
			c = 0.f;


			momentumFracture = GetMomentumFracture(lsMag, momentum);// +((momentum - c) * (lsMag / 127.f));

			if (!sprintHold)
			{
				target_moveBlendRatio = momentumFracture;
				momentum = max(1.f, BUILTIN::CEIL(target_moveBlendRatio * 4.f) / 4.f);
			}
			else 
			{
				//momentumFracture = momentum * (lsMag / 127.f);
				if (momentum >= 1.75f)
				{
					c = max(1.75f, momentum - 1.75f);
					momentumFracture = GetMomentumFracture(lsMag, momentum, c);
					target_moveBlendRatio = max(1.75, momentumFracture);
				}
				else
					target_moveBlendRatio = momentumFracture;
			}

		}

		if (momentum > 1.f)
		{
			PED::SET_PED_MIN_MOVE_BLEND_RATIO(player, target_moveBlendRatio);
			PED::SET_PED_MAX_MOVE_BLEND_RATIO(player, target_moveBlendRatio);
		}





		/*
		//if walking at max speed and giving stick input
		if (lsMag >= 5)
		{
			//if stick pressed roughly all the way
			if (moveBlendRatio >= 1.f && lsMag > 125)
			{
				if (sprint)
				{
					momentum = max(1.f, momentum);
					momentum = min(momentum + 0.25f, 3.f);
				}
				target_moveBlendRatio = momentum;
			}
			else if (moveBlendRatio > 1.f)
			{
				//momentumFracture = 1 + ((momentum - 1) * (lsMag / 127.f));
				momentumFracture = momentum * (lsMag / 127.f);

				if (!sprintHold)
					momentum = BUILTIN::CEIL(momentumFracture * 4.f) / 4.f;

				target_moveBlendRatio = momentumFracture;
			}
			else if (sprintHold && moveBlendRatio < 1.f)
				target_moveBlendRatio = 1.25f;
			else
				target_moveBlendRatio = 1.f;

			if (target_moveBlendRatio > 1.f)
			{
				PED::SET_PED_MIN_MOVE_BLEND_RATIO(player, target_moveBlendRatio);
				PED::SET_PED_MAX_MOVE_BLEND_RATIO(player, target_moveBlendRatio);
			}
		}
		else { momentum = 1.f; }
		*/

		char buffer[32];
		//snprintf(buffer, 32, "ratio: %f\rRADIUS: %F", move_blend_ratio, ls_radius );
		//snprintf(buffer, 32, "%f\r%f\r%f\r%f", u, moveBlendRatio, target_moveBlendRatio, temp);
		snprintf(buffer, 32, "%f\r%f\r%f", momentum, momentumFracture, moveBlendRatio);
		DrawText(buffer, 0.6, 0.6);
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