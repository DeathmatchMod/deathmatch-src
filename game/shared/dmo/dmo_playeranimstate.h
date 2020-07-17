//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef DMO_PLAYERANIMSTATE_H
#define DMO_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif


#include "convar.h"
#include "iplayeranimstate.h"
#include "base_playeranimstate.h"


#ifdef CLIENT_DLL
	class C_BaseAnimatingOverlay;
	class C_WeaponDMOBase;
	#define CBaseAnimatingOverlay C_BaseAnimatingOverlay
	#define CWeaponDMOBase C_WeaponDMOBase
	#define CDMOPlayer C_DMOPlayer
#else
	class CBaseAnimatingOverlay;
	class CWeaponDMOBase; 
	class CDMOPlayer;
#endif


// When moving this fast, he plays run anim.
#define ARBITRARY_RUN_SPEED		175.0f


enum PlayerAnimEvent_t
{
	PLAYERANIMEVENT_FIRE_GUN_PRIMARY=0,
	PLAYERANIMEVENT_FIRE_GUN_SECONDARY,
	PLAYERANIMEVENT_THROW_GRENADE,
	PLAYERANIMEVENT_JUMP,
	PLAYERANIMEVENT_RELOAD,
	
	PLAYERANIMEVENT_COUNT
};


class IDMOPlayerAnimState : virtual public IPlayerAnimState
{
public:
	// This is called by both the client and the server in the same way to trigger events for
	// players firing, jumping, throwing grenades, etc.
	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 ) = 0;
	
	// Returns true if we're playing the grenade prime or throw animation.
	virtual bool IsThrowingGrenade() = 0;
};


// This abstracts the differences between DMO players and hostages.
class IDMOPlayerAnimStateHelpers
{
public:
	virtual CWeaponDMOBase* DMOAnim_GetActiveWeapon() = 0;
	virtual bool DMOAnim_CanMove() = 0;
};


IDMOPlayerAnimState* CreatePlayerAnimState( CBaseAnimatingOverlay *pEntity, IDMOPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences );

// If this is set, then the game code needs to make sure to send player animation events
// to the local player if he's the one being watched.
extern ConVar cl_showanimstate;


#endif // DMO_PLAYERANIMSTATE_H
