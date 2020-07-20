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
#define SNIPER_BEAM_LIVE_TIME 0.45f

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
	CBaseEntity* lastEnt = pPlayer;

startFire:
	UTIL_TraceLine(vecStartPos, vecEndPos, MASK_SOLID, lastEnt, COLLISION_GROUP_PLAYER, &tr);

	if (tr.DidHit())
	{

		if (tr.DidHitNonWorldEntity())
		{

			
			// We have to get our true end point on both the client and the server, but we can only damage on the server... 
#ifdef DMO_SERVER
			int iDamage = GetDMOWpnData().m_iDamage;
			CTakeDamageInfo dmgInfo;
			dmgInfo.SetAttacker(pPlayer);
			dmgInfo.SetWeapon(this);

			dmgInfo.SetDamage(iDamage);
			dmgInfo.SetDamageType(DMG_ENERGYBEAM | DMG_ALWAYSGIB);
			
			//if (tr.m_pEnt->IsPlayer())
			//	dmgInfo.SetDamageType(DMG_ALWAYSGIB);

			tr.m_pEnt->DispatchTraceAttack(dmgInfo, vecAimDir, &tr); 

#endif
			
			// If we kill our hit ent, let's keep blasting through!
			if (tr.m_pEnt->IsCombatCharacter() && !tr.m_pEnt->IsAlive())
			{
				lastEnt = tr.m_pEnt;
				vecStartPos = tr.endpos;
				goto startFire;
			}
		}





		CBroadcastRecipientFilter filter;
		// We don't send to the owner so that this code can be ran on the client and the beam can come from the viewmodel's muzzle
		filter.RemoveRecipient(pPlayer);
		TE_LaserSniperBeam(filter, pPlayer, 1, tr.endpos, SNIPER_BEAM_LIVE_TIME, SNIPER_BEAM_WIDTH);

	}

	m_iClip1 -= 1;

	WeaponSound(SINGLE);

	pPlayer->SetAnimation(PLAYER_ATTACK1);
	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	float delayTime = gpGlobals->curtime + GetDMOWpnData().m_flCycleTime;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = delayTime;
	SetWeaponIdleTime(delayTime);
}

