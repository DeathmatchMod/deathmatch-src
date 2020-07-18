//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for DMO Game
//
// $NoKeywords: $
//=============================================================================//

#ifndef DMO_PLAYER_H
#define DMO_PLAYER_H
#pragma once


#include "player.h"
#include "server_class.h"
#include "dmo_playeranimstate.h"
#include "dmo_shareddefs.h"

class CWeaponDMOBase;
//=============================================================================
// >> DMO Game player
//=============================================================================
class CDMOPlayer : public CBasePlayer
{
public:
	DECLARE_CLASS( CDMOPlayer, CBasePlayer );
	DECLARE_SERVERCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CDMOPlayer();
	~CDMOPlayer();

	static CDMOPlayer *CreatePlayer( const char *className, edict_t *ed );
	static CDMOPlayer* Instance( int iEnt );

	// This passes the event to the client's and server's CPlayerAnimState.
	void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );

	virtual void FlashlightTurnOn( void );
	virtual void FlashlightTurnOff( void );
	virtual int FlashlightIsOn( void );

	virtual void PreThink();
	virtual void PostThink();
	virtual void Spawn();
	virtual void InitialSpawn();
	virtual void Precache();
	virtual void Event_Killed( const CTakeDamageInfo &info );
	virtual void LeaveVehicle( const Vector &vecExitPoint, const QAngle &vecExitAngles );
	
	CWeaponDMOBase* GetActiveDMOWeapon() const;
	virtual void	CreateViewModel( int viewmodelindex = 0 );

	virtual void	CheatImpulseCommands( int iImpulse );

	CNetworkVar( int, m_iThrowGrenadeCounter );	// used to trigger grenade throw animations.
	CNetworkQAngle( m_angEyeAngles );	// Copied from EyeAngles() so we can send it to the client.
	CNetworkVar( int, m_iShotsFired );	// number of shots fired recently

	// Tracks our ragdoll entity.
	CNetworkHandle( CBaseEntity, m_hRagdoll );	// networked entity handle 

// In shared code.
public:
	// IDMOPlayerAnimState overrides.
	virtual CWeaponDMOBase* DMOAnim_GetActiveWeapon();
	virtual bool DMOAnim_CanMove();
	

	void FireBullet( 
		Vector vecSrc, 
		const QAngle &shootAngles, 
		float vecSpread, 
		int iDamage, 
		int iBulletType,
		CBaseEntity *pevAttacker,
		bool bDoEffects,
		float x,
		float y );

private:

	void CreateRagdollEntity();

	CDMOPlayerAnimState *m_PlayerAnimState;
};


inline CDMOPlayer *ToDMOPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

#ifdef _DEBUG
	Assert( dynamic_cast<CDMOPlayer*>( pEntity ) != 0 );
#endif
	return static_cast< CDMOPlayer* >( pEntity );
}


#endif	// DMO_PLAYER_H
