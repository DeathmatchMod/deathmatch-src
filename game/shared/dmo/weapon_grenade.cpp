//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "dmo_weapon_base.h"
#include "gamerules.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "weapon_grenade.h"


#ifdef CLIENT_DLL
	
#else

	#include "dmo_player.h"
	#include "items.h"
	#include "dmo_basegrenade_projectile.h"

#endif


#define GRENADE_TIMER	3.0f //Seconds

IMPLEMENT_NETWORKCLASS_ALIASED( DMOGrenade, DT_DMOGrenade )

BEGIN_NETWORK_TABLE(CDMOGrenade, DT_DMOGrenade)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CDMOGrenade )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_grenade, CDMOGrenade );
PRECACHE_WEAPON_REGISTER( weapon_grenade );

acttable_t CDMOGrenade::m_acttable[] =
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_GRENADE,						false },
	{ ACT_MP_RUN,						ACT_HL2MP_RUN_GRENADE,						false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_GRENADE,				false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_GRENADE,				false },
	//{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_GRENADE,		false },
	//{ ACT_HL2MP_GESTURE_RELOAD,		ACT_HL2MP_GESTURE_RELOAD_GRENADE,			false },
	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_GRENADE,						false },
	{ ACT_MP_ATTACK_STAND_GRENADE,		ACT_HL2MP_GESTURE_RANGE_ATTACK_GRENADE,		false },
};
IMPLEMENT_ACTTABLE(CDMOGrenade);



#ifdef GAME_DLL

#define GRENADE_MODEL "models/Weapons/w_grenade.mdl"

class CGrenadeProjectile : public CBaseGrenadeProjectile
{
public:
	DECLARE_CLASS( CGrenadeProjectile, CBaseGrenadeProjectile );


	// Overrides.
public:
	virtual void Spawn()
	{
		SetModel( GRENADE_MODEL );
		BaseClass::Spawn();
	}

	virtual void Precache()
	{
		PrecacheModel( GRENADE_MODEL );
		BaseClass::Precache();
	}

	// Grenade stuff.
public:

	static CGrenadeProjectile* Create( 
		const Vector &position, 
		const QAngle &angles, 
		const Vector &velocity, 
		const AngularImpulse &angVelocity, 
		CBaseCombatCharacter *pOwner, 
		float timer )
	{
		CGrenadeProjectile *pGrenade = (CGrenadeProjectile*)CBaseEntity::Create( "grenade_projectile", position, angles, pOwner );

		// Set the timer for 1 second less than requested. We're going to issue a SOUND_DANGER
		// one second before detonation.

		pGrenade->SetDetonateTimerLength( 1.5 );
		pGrenade->SetAbsVelocity( velocity );
		pGrenade->SetupInitialTransmittedGrenadeVelocity( velocity );
		pGrenade->SetThrower( pOwner ); 

		pGrenade->SetGravity( BaseClass::GetGrenadeGravity() );
		pGrenade->SetFriction( BaseClass::GetGrenadeFriction() );
		pGrenade->SetElasticity( BaseClass::GetGrenadeElasticity() );

		pGrenade->m_flDamage = 100;
		pGrenade->m_DmgRadius = pGrenade->m_flDamage * 3.5f;
		pGrenade->ChangeTeam( pOwner->GetTeamNumber() );
		pGrenade->ApplyLocalAngularVelocityImpulse( angVelocity );	

		// make NPCs afaid of it while in the air
		pGrenade->SetThink( &CGrenadeProjectile::DangerSoundThink );
		pGrenade->SetNextThink( gpGlobals->curtime );

		return pGrenade;
	}
};

LINK_ENTITY_TO_CLASS( grenade_projectile, CGrenadeProjectile );
PRECACHE_WEAPON_REGISTER( grenade_projectile );

BEGIN_DATADESC( CDMOGrenade )
END_DATADESC()

void CDMOGrenade::EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer )
{
	CGrenadeProjectile::Create( vecSrc, vecAngles, vecVel, angImpulse, pPlayer, GRENADE_TIMER );
}
	
#endif

