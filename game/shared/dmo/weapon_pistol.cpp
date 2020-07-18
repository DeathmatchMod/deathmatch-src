//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "cbase.h"
#include "dmo_weapon_base.h"

#if defined( CLIENT_DLL )

	#define CWeaponPistol C_WeaponPistol
	#include "c_dmo_player.h"

#else

	#include "dmo_player.h"

#endif


class CWeaponPistol : public CWeaponDMOBase
{
public:
	DECLARE_CLASS( CWeaponPistol, CWeaponDMOBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();
	

	CWeaponPistol();

	virtual DMOWeaponID GetWeaponID( void ) const		{ return DMO_WEAPON_PISTOL; }
	virtual int GetFireMode() const { return FM_SEMIAUTOMATIC; }

private:

	CWeaponPistol( const CWeaponPistol & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponPistol, DT_WeaponPistol )

BEGIN_NETWORK_TABLE( CWeaponPistol, DT_WeaponPistol )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponPistol )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_pistol, CWeaponPistol );
PRECACHE_WEAPON_REGISTER( weapon_pistol );

acttable_t CWeaponPistol::m_acttable[] =
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
IMPLEMENT_ACTTABLE(CWeaponPistol);

CWeaponPistol::CWeaponPistol()
{
}

