//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "Multiplayer/multiplayer_animstate.h"
#include "tier0/vprof.h"
#include "animation.h"
#include "studio.h"
#include "apparent_velocity_helper.h"
#include "utldict.h"

#include "dmo_playeranimstate.h"
#include "dmo_weapon_base.h"
#include "dmo_weapon_basegrenade.h"

#ifdef CLIENT_DLL
	#include "c_dmo_player.h"
	#include "bone_setup.h"
	#include "interpolatedvar.h"
#else
	#include "dmo_player.h"
#endif

#define ANIM_TOPSPEED_WALK			100
#define ANIM_TOPSPEED_RUN			250
#define ANIM_TOPSPEED_RUN_CROUCH	85

#define DEFAULT_IDLE_NAME "idle_upper_"
#define DEFAULT_CROUCH_IDLE_NAME "crouch_idle_upper_"
#define DEFAULT_CROUCH_WALK_NAME "crouch_walk_upper_"
#define DEFAULT_WALK_NAME "walk_upper_"
#define DEFAULT_RUN_NAME "run_upper_"

#define DEFAULT_FIRE_IDLE_NAME "idle_shoot_"
#define DEFAULT_FIRE_CROUCH_NAME "crouch_idle_shoot_"
#define DEFAULT_FIRE_CROUCH_WALK_NAME "crouch_walk_shoot_"
#define DEFAULT_FIRE_WALK_NAME "walk_shoot_"
#define DEFAULT_FIRE_RUN_NAME "run_shoot_"


#define FIRESEQUENCE_LAYER		(AIMSEQUENCE_LAYER+NUM_AIMSEQUENCE_LAYERS)
#define RELOADSEQUENCE_LAYER	(FIRESEQUENCE_LAYER + 1)
#define GRENADESEQUENCE_LAYER	(RELOADSEQUENCE_LAYER + 1)
#define NUM_LAYERS_WANTED		(GRENADESEQUENCE_LAYER + 1)




CDMOPlayerAnimState* CreatePlayerAnimState( CDMOPlayer* pPlayer )
{
	MultiPlayerMovementData_t movementData;
	movementData.m_flBodyYawRate = 720;
	movementData.m_flRunSpeed = DMO_DEFAULT_PLAYER_RUNSPEED;
	movementData.m_flSprintSpeed = DMO_DEFAULT_PLAYER_SPRINTSPEED;
	movementData.m_flWalkSpeed = DMO_DEFAULT_PLAYER_RUNSPEED;

	CDMOPlayerAnimState *pRet = new CDMOPlayerAnimState(pPlayer, movementData);
	pRet->InitDMO(pPlayer);
	return pRet;
}

// ------------------------------------------------------------------------------------------------ //
// CDMOPlayerAnimState implementation.
// ------------------------------------------------------------------------------------------------ //


Activity CDMOPlayerAnimState::TranslateActivity(Activity actDesired)
{
	Activity translatedAct = BaseClass::TranslateActivity(actDesired);
	if (m_pPlayer->GetActiveDMOWeapon())
	{
		translatedAct = m_pPlayer->GetActiveDMOWeapon()->ActivityOverride(translatedAct, false);
	}

	return translatedAct;
}

void CDMOPlayerAnimState::InitDMO( CDMOPlayer* pPlayer )
{
	m_pPlayer = static_cast<CDMOPlayer*>(pPlayer);

}

