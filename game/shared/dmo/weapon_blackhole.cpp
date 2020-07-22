//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "cbase.h"
#include "dmo_weapon_base.h"

#if defined( CLIENT_DLL )

	#define CWeaponBlackhole C_WeaponBlackhole
	#define CBlackhole C_Blackhole
	#include "c_dmo_player.h"

#else

	#include "dmo_player.h"

#endif

#include "debugoverlay_shared.h"
#include "beam_shared.h"

#ifdef CLIENT_DLL
#include "beamdraw.h"
#include "iviewrender_beams.h"
#endif

ConVar sv_blackhole_accel("sv_blackhole_accel", "50000", FCVAR_ARCHIVE);
ConVar sv_blackhole_speed("sv_blackhole_speed", "400", FCVAR_ARCHIVE);
ConVar sv_blackhole_lifetime("sv_blackhole_lifetime", "10", FCVAR_ARCHIVE);

//=====================================================================================//
// CBlackhole
class CBlackhole : public CBaseEntity
{
public:
	DECLARE_CLASS(CBlackhole, CBaseEntity);
	DECLARE_NETWORKCLASS();

	virtual void Spawn() override;
	virtual void Think() override;

	CBlackhole();

	float m_fRadius;
	float m_fAccel;
	float m_fTotalLifetime;
	CBasePlayer* m_pOwner;
};

LINK_ENTITY_TO_CLASS(point_blackhole, CBlackhole);

IMPLEMENT_NETWORKCLASS_ALIASED(Blackhole, DT_CBlackhole)
BEGIN_NETWORK_TABLE(CBlackhole, DT_CBlackhole)
END_NETWORK_TABLE()

void CBlackhole::Think()
{
	BaseClass::Think();
	this->SetNextThink(gpGlobals->curtime + gpGlobals->frametime);
	CBaseEntity* pEntList[64];
	int num = UTIL_EntitiesInSphere(pEntList, 64, this->GetAbsOrigin(), m_fRadius, 0);
	for(int i = 0; i < num; i++)
	{
		if(pEntList[i]->entindex() == this->entindex() ||
			pEntList[i]->entindex() == m_pOwner->entindex()) continue;
		if(pEntList[i]->IsPlayer())
		{
			ToBasePlayer(pEntList[i])->SetLocalOrigin(Vector(123456, 123456, 123456)); // idk lol
		}
		else
		{
			Vector _r;
			RandomVectorInUnitSphere(&_r);
			pEntList[i]->ApplyAbsVelocityImpulse(_r * (m_fAccel * gpGlobals->frametime));
			NDebugOverlay::Sphere(this->GetAbsOrigin(), m_fRadius, 255, 0, 0, false, 0.0f);
		}
	}

	m_fTotalLifetime += gpGlobals->frametime;
	if(m_fTotalLifetime > sv_blackhole_lifetime.GetFloat()) this->Remove();
}

void CBlackhole::Spawn()
{
	BaseClass::Spawn();
	this->SetNextThink(gpGlobals->curtime + gpGlobals->frametime);
	this->SetMoveType(MoveType_t::MOVETYPE_NOCLIP);
}

CBlackhole::CBlackhole() :
	m_fRadius(64),
	m_fAccel(50000.0f),
	m_fTotalLifetime(0.0f)
{

}

//=====================================================================================//


//=====================================================================================//
// CWeaponBlackhole
class CWeaponBlackhole : public CWeaponDMOBase
{
public:
	DECLARE_CLASS( CWeaponBlackhole, CWeaponDMOBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();


	CWeaponBlackhole();

	virtual DMOWeaponID GetWeaponID() const override { return DMO_WEAPON_PISTOL; }
	virtual int GetFireMode() const { return FM_SEMIAUTOMATIC; }

	virtual void PrimaryAttack() override;
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

void CWeaponBlackhole::PrimaryAttack()
{
	CDMOPlayer* pPlayer = GetPlayerOwner();

	pPlayer->ViewPunch(QAngle(RandomInt(-10, 10), RandomInt(-20, 20), RandomInt(-10, 10)));
	pPlayer->SetNextAttack(gpGlobals->curtime + 1.0f);
#ifdef GAME_DLL
	CBlackhole* ent = (CBlackhole*)this->Create("point_blackhole", this->GetAbsOrigin() + Vector(0, 0, 50), QAngle(0,0,0), this);
	ent->m_pOwner = pPlayer;
	ent->m_fAccel = sv_blackhole_accel.GetFloat();
	ent->SetAbsVelocity(pPlayer->GetAutoaimVector(1.0f) * sv_blackhole_speed.GetFloat());
#endif
}
//=====================================================================================//
