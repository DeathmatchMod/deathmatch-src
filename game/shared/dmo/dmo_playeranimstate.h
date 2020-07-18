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
#include "Multiplayer/multiplayer_animstate.h"


#ifdef CLIENT_DLL
	class C_DMOPlayer;
	#define CDMOPlayer C_DMOPlayer
#else
	class CDMOPlayer;
#endif





// ------------------------------------------------------------------------------------------------ //
// CDMOPlayerAnimState declaration.
// ------------------------------------------------------------------------------------------------ //

class CDMOPlayerAnimState : public CMultiPlayerAnimState
{
public:
	DECLARE_CLASS(CDMOPlayerAnimState, CMultiPlayerAnimState);
	CDMOPlayerAnimState(CBasePlayer* pPlayer, MultiPlayerMovementData_t& movementData) : CMultiPlayerAnimState(pPlayer, movementData) { }

	void InitDMO(CDMOPlayer* pPlayer);
	virtual Activity TranslateActivity(Activity actDesired);
private:
	CDMOPlayer* m_pPlayer;
};



CDMOPlayerAnimState* CreatePlayerAnimState( CDMOPlayer* pPlayer );

// If this is set, then the game code needs to make sure to send player animation events
// to the local player if he's the one being watched.
extern ConVar cl_showanimstate;


#endif // DMO_PLAYERANIMSTATE_H
