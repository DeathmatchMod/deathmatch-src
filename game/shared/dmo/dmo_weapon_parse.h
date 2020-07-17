//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef DMO_WEAPON_PARSE_H
#define DMO_WEAPON_PARSE_H
#ifdef _WIN32
#pragma once
#endif


#include "weapon_parse.h"
#include "networkvar.h"


//--------------------------------------------------------------------------------------------------------
class CDMOWeaponInfo : public FileWeaponInfo_t
{
public:
	DECLARE_CLASS_GAMEROOT( CDMOWeaponInfo, FileWeaponInfo_t );
	
	CDMOWeaponInfo();
	
	virtual void Parse( ::KeyValues *pKeyValuesData, const char *szWeaponName );

	char m_szAnimExtension[16];		// string used to generate player animations with this weapon

	// Parameters for FX_FireBullets:
	int		m_iDamage;
	int		m_iBullets;
	float	m_flCycleTime;
};


#endif // DMO_WEAPON_PARSE_H
