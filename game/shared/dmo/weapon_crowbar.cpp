//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "cbase.h"
#include "dmo_weapon_base.h"
#include "dmo_weapon_melee.h"

#if defined( CLIENT_DLL )

	#define CWeaponCrowbar C_WeaponCrowbar
	#include "c_dmo_player.h"

#else

	#include "dmo_player.h"

#endif


class CWeaponCrowbar : public CWeaponDMOMelee
{
public:
	DECLARE_CLASS( CWeaponCrowbar, CWeaponDMOMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();
	
	CWeaponCrowbar();

	virtual DMOWeaponID GetWeaponID( void ) const		{	return DMO_WEAPON_CROWBAR; }
	virtual float	GetRange( void )					{	return	64.0f;	}	//Tony; let the crowbar swing further.
	virtual bool CanWeaponBeDropped() const				{	return false; }

private:

	CWeaponCrowbar( const CWeaponCrowbar & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponCrowbar, DT_WeaponCrowbar )

BEGIN_NETWORK_TABLE( CWeaponCrowbar, DT_WeaponCrowbar )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponCrowbar )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_crowbar, CWeaponCrowbar );
PRECACHE_WEAPON_REGISTER( weapon_crowbar );



CWeaponCrowbar::CWeaponCrowbar()
{
}

//Tony; todo; add ACT_MP_PRONE* activities, so we have them.
acttable_t CWeaponCrowbar::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,					ACT_HL2MP_IDLE_MELEE,				false },
	{ ACT_MP_CROUCH_IDLE,					ACT_HL2MP_IDLE_CROUCH_MELEE,				false },
	{ ACT_MP_JUMP,							ACT_HL2MP_JUMP_MELEE,					false },

	{ ACT_MP_RUN,							ACT_HL2MP_RUN_MELEE,					false },
	//{ ACT_MP_WALK,							ACT_DOD_WALK_AIM_SPADE,					false },
	{ ACT_MP_CROUCHWALK,					ACT_HL2MP_WALK_CROUCH_MELEE,			false },
	//{ ACT_SPRINT,							ACT_DOD_SPRINT_AIM_SPADE,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,		ACT_RANGE_ATTACK_SLAM,			false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,		ACT_RANGE_ATTACK_SLAM,			false },
};
IMPLEMENT_ACTTABLE( CWeaponCrowbar );

