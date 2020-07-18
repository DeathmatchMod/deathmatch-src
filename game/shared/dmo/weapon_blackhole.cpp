//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "cbase.h"
#include "dmo_weapon_base.h"

#if defined( CLIENT_DLL )

	#define CWeaponBlackhole C_WeaponBlackhole
	#include "c_dmo_player.h"

#else

	#include "dmo_player.h"

#endif


class CWeaponBlackhole : public CWeaponDMOBase
{
public:
	DECLARE_CLASS( CWeaponBlackhole, CWeaponDMOBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();


	CWeaponBlackhole();

	virtual DMOWeaponID GetWeaponID( void ) const		{ return DMO_WEAPON_PISTOL; }
	virtual int GetFireMode() const { return FM_SEMIAUTOMATIC; }

private:

	CWeaponBlackhole( const CWeaponBlackhole & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponBlackhole, DT_WeaponBlackhole )

BEGIN_NETWORK_TABLE( CWeaponBlackhole, DT_WeaponBlackhole )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponBlackhole )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_blackhole, CWeaponBlackhole );
PRECACHE_WEAPON_REGISTER( weapon_blackhole );

acttable_t CWeaponBlackhole::m_acttable[] =
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_PISTOL,						false },
	{ ACT_MP_RUN,						ACT_HL2MP_RUN_PISTOL,						false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_PISTOL,				false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_PISTOL,				false },
	//{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,		false },
	//{ ACT_HL2MP_GESTURE_RELOAD,		ACT_HL2MP_GESTURE_RELOAD_PISTOL,			false },
	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_PISTOL,						false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_RANGE_ATTACK_PISTOL,					false },
};
IMPLEMENT_ACTTABLE(CWeaponBlackhole);

CWeaponBlackhole::CWeaponBlackhole()
{
}

