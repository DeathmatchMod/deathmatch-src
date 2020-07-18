//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_GRENADE_H
#define WEAPON_GRENADE_H
#ifdef _WIN32
#pragma once
#endif

#include "dmo_weapon_basegrenade.h"


#ifdef CLIENT_DLL
	
	#define CDMOGrenade C_DMOGrenade

#endif

//-----------------------------------------------------------------------------
// Fragmentation grenades
//-----------------------------------------------------------------------------
class CDMOGrenade : public CBaseDMOGrenade
{
public:
	DECLARE_CLASS( CDMOGrenade, CBaseDMOGrenade );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

	CDMOGrenade() {}

	virtual DMOWeaponID GetWeaponID( void ) const		{ return DMO_WEAPON_GRENADE; }

#ifdef CLIENT_DLL

#else
	DECLARE_DATADESC();

	virtual void EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer );
	
#endif

	CDMOGrenade( const CDMOGrenade & ) {}
};


#endif // WEAPON_GRENADE_H
