//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_DMOBASE_H
#define WEAPON_DMOBASE_H
#ifdef _WIN32
#pragma once
#endif

#include "Multiplayer/multiplayer_animstate.h"
#include "dmo_weapon_parse.h"
#include "dmo_shareddefs.h"

#ifdef CLIENT_DLL
	#include "c_dmo_player.h"
#else
	#include "dmo_player.h"
#endif

#if defined( CLIENT_DLL )
	#define CWeaponDMOBase C_WeaponDMOBase
#endif

class CDMOPlayer;

// These are the names of the ammo types that the weapon script files reference.
#define AMMO_BULLETS			"AMMO_BULLETS"
#define AMMO_ROCKETS			"AMMO_ROCKETS"
#define AMMO_GRENADE			"AMMO_GRENADE"
#define AMMO_CELLS				"Cells"


typedef enum
{
	Primary_Mode = 0,
	Secondary_Mode,
} DMOWeaponMode;

const char *WeaponIDToAlias( int id );

class CWeaponDMOBase : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponDMOBase, CBaseCombatWeapon );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CWeaponDMOBase();

	#ifdef GAME_DLL
		DECLARE_DATADESC();
	#endif

	// All predicted weapons need to implement and return true
	virtual bool	IsPredicted() const { return true; }
	virtual DMOWeaponID GetWeaponID( void ) const { return WEAPON_NONE; }
	
	// Get DMO weapon specific weapon data.
	CDMOWeaponInfo const	&GetDMOWpnData() const;

	// Get a pointer to the player that owns this weapon
	CDMOPlayer* GetPlayerOwner() const;

	// override to play custom empty sounds
	virtual bool PlayEmptySound();

#ifdef GAME_DLL
	virtual void SendReloadEvents();
#endif

private:
	CWeaponDMOBase( const CWeaponDMOBase & );
};


#endif // WEAPON_DMOBASE_H
