#include "cbase.h"
#include "dmo_weapon_base.h"
#include "beam_shared.h"
#include "dmo_te.h"

#if defined( CLIENT_DLL )

	#define CWeaponLaserSniper C_WeaponLaserSniper
	#include "c_dmo_player.h"

#else

	#include "dmo_player.h"

#endif

#define SNIPER_BEAM_SPRITE "sprites/bluelaser1.vmt"
#define SNIPER_BEAM_WIDTH 5.0f
#define SNIPER_BEAM_LIVE_TIME 0.75f

class CWeaponLaserSniper : public CWeaponDMOBase
{
public:
	DECLARE_CLASS( CWeaponLaserSniper, CWeaponDMOBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();
	

	CWeaponLaserSniper();

	virtual void Precache();

	virtual DMOWeaponID GetWeaponID( ) const { return DMO_WEAPON_LASERSNIPER; }
	virtual void PrimaryAttack(void);

};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponLaserSniper, DT_WeaponLaserSniper )

BEGIN_NETWORK_TABLE( CWeaponLaserSniper, DT_WeaponLaserSniper )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponLaserSniper )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_lasersniper, CWeaponLaserSniper );
PRECACHE_WEAPON_REGISTER( weapon_lasersniper );

acttable_t CWeaponLaserSniper::m_acttable[] =
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_AR2,						false },
	{ ACT_MP_RUN,						ACT_HL2MP_RUN_AR2,						false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_AR2,				false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_AR2,				false },
	//{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_AR2,		false },
	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_AR2,			false },
	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_AR2,						false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_AR2,		false },
};

IMPLEMENT_ACTTABLE(CWeaponLaserSniper);

CWeaponLaserSniper::CWeaponLaserSniper()
{
}

void CWeaponLaserSniper::Precache()
{
	BaseClass::Precache();
	PrecacheModel(SNIPER_BEAM_SPRITE);
}

void CWeaponLaserSniper::PrimaryAttack(void)
{
	CDMOPlayer* pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;


	Vector vecAimDir = pPlayer->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT);
	Vector vecStartPos = pPlayer->Weapon_ShootPosition();
	Vector vecEndPos = vecStartPos + vecAimDir * MAX_TRACE_LENGTH;

	trace_t tr;

	UTIL_TraceLine(vecStartPos, vecEndPos, MASK_SOLID, pPlayer, COLLISION_GROUP_PLAYER, &tr);

	if (tr.DidHit())
	{


		CBroadcastRecipientFilter filter;
		filter.RemoveRecipient(pPlayer);
		// MUST BE RAN ON OWNING CLIENT, BUT NOT SENT TO OWNER!!!!!!
		TE_LaserSniperBeam(filter, pPlayer, 1, tr.endpos, SNIPER_BEAM_LIVE_TIME, SNIPER_BEAM_WIDTH);

#ifdef DMO_SERVER
		if (tr.DidHitNonWorldEntity())
		{
			CTakeDamageInfo dmgInfo;
			dmgInfo.SetAttacker(pPlayer);
			dmgInfo.SetWeapon(this);

			dmgInfo.SetDamage(GetDMOWpnData().m_iDamage);
			dmgInfo.SetDamageType(DMG_DISSOLVE | DMG_ENERGYBEAM);

			tr.m_pEnt->TakeDamage(dmgInfo);
		}
#endif
	}

	m_flNextPrimaryAttack = gpGlobals->curtime + 1;
}

