//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_DMO_PLAYER_H
#define C_DMO_PLAYER_H
#ifdef _WIN32
#pragma once
#endif


#include "dmo_playeranimstate.h"
#include "c_baseplayer.h"
#include "dmo_shareddefs.h"
#include "baseparticleentity.h"

class C_WeaponDMOBase;
#define CWeaponDMOBase C_WeaponDMOBase
class C_DMOPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_DMOPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	C_DMOPlayer();
	~C_DMOPlayer();

	static C_DMOPlayer* GetLocalDMOPlayer();

	virtual const QAngle& GetRenderAngles();
	virtual void UpdateClientSideAnimation();
	virtual void PostDataUpdate( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );


// Called by shared code.
public:
	
	// IDMOPlayerAnimState overrides.
	virtual CWeaponDMOBase* DMOAnim_GetActiveWeapon();
	virtual bool DMOAnim_CanMove();

	void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
	bool ShouldDraw();

	CDMOPlayerAnimState *m_PlayerAnimState;

	QAngle	m_angEyeAngles;
	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;

	CNetworkVar( int, m_iThrowGrenadeCounter );	// used to trigger grenade throw animations.
	CNetworkVar( int, m_iShotsFired );	// number of shots fired recently

	EHANDLE	m_hRagdoll;

	CWeaponDMOBase *GetActiveDMOWeapon() const;

	C_BaseAnimating *BecomeRagdollOnClient();
	IRagdoll* C_DMOPlayer::GetRepresentativeRagdoll() const;

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
	C_DMOPlayer( const C_DMOPlayer & );
};


inline C_DMOPlayer* ToDMOPlayer( CBaseEntity *pPlayer )
{
	Assert( dynamic_cast< C_DMOPlayer* >( pPlayer ) != NULL );
	return static_cast< C_DMOPlayer* >( pPlayer );
}


#endif // C_DMO_PLAYER_H
