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
#include "dmo_weapon_basegrenade.h"
#include "in_buttons.h"	


#ifdef CLIENT_DLL

	#include "c_dmo_player.h"

#else

	#include "dmo_player.h"
	#include "items.h"

#endif


#define GRENADE_TIMER	1.5f //Seconds


IMPLEMENT_NETWORKCLASS_ALIASED( BaseDMOGrenade, DT_BaseDMOGrenade )

BEGIN_NETWORK_TABLE(CBaseDMOGrenade, DT_BaseDMOGrenade)

#ifndef CLIENT_DLL
	SendPropBool( SENDINFO(m_bRedraw) ),
	SendPropBool( SENDINFO(m_bPinPulled) ),
	SendPropFloat( SENDINFO(m_fThrowTime), 0, SPROP_NOSCALE ),
#else
	RecvPropBool( RECVINFO(m_bRedraw) ),
	RecvPropBool( RECVINFO(m_bPinPulled) ),
	RecvPropFloat( RECVINFO(m_fThrowTime) ),
#endif

END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CBaseDMOGrenade )
	DEFINE_PRED_FIELD( m_bRedraw, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bRedraw, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_basesdkgrenade, CBaseDMOGrenade );


CBaseDMOGrenade::CBaseDMOGrenade()
{
	m_bRedraw = false;
	m_bPinPulled = false;
	m_fThrowTime = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseDMOGrenade::Precache()
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBaseDMOGrenade::Deploy()
{
	m_bRedraw = false;
	m_bPinPulled = false;
	m_fThrowTime = 0;

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseDMOGrenade::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	m_bRedraw = false;
	m_bPinPulled = false; // when this is holstered make sure the pin isn’t pulled.
	m_fThrowTime = 0;

#ifndef CLIENT_DLL
	// If they attempt to switch weapons before the throw animation is done, 
	// allow it, but kill the weapon if we have to.
	CDMOPlayer *pPlayer = GetPlayerOwner();

	if( pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0 )
	{
		CBaseCombatCharacter *pOwner = (CBaseCombatCharacter *)pPlayer;
		pOwner->Weapon_Drop( this );
		UTIL_Remove(this);
	}
#endif

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseDMOGrenade::PrimaryAttack()
{
	if ( m_bRedraw || m_bPinPulled )
		return;

	CDMOPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer || pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
		return;

	// The pull pin animation has to finish, then we wait until they aren't holding the primary
	// attack button, then throw the grenade.
	SendWeaponAnim( ACT_VM_PULLPIN );
	m_bPinPulled = true;
	
	// Don't let weapon idle interfere in the middle of a throw!
	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );

	m_flNextPrimaryAttack	= gpGlobals->curtime + SequenceDuration();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseDMOGrenade::SecondaryAttack()
{
	if ( m_bRedraw )
		return;

	CDMOPlayer *pPlayer = GetPlayerOwner();
	
	if ( pPlayer == NULL )
		return;

	//See if we're ducking
	if ( pPlayer->GetFlags() & FL_DUCKING )
	{
		//Send the weapon animation
		SendWeaponAnim( ACT_VM_SECONDARYATTACK );
	}
	else
	{
		//Send the weapon animation
		SendWeaponAnim( ACT_VM_HAULBACK );
	}

	// Don't let weapon idle interfere in the middle of a throw!
	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );

	m_flNextSecondaryAttack	= gpGlobals->curtime + SequenceDuration();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseDMOGrenade::Reload()
{
	if ( ( m_bRedraw ) && ( m_flNextPrimaryAttack <= gpGlobals->curtime ) && ( m_flNextSecondaryAttack <= gpGlobals->curtime ) )
	{
		//Redraw the weapon
		SendWeaponAnim( ACT_VM_DRAW );

		//Update our times
		m_flNextPrimaryAttack	= gpGlobals->curtime + SequenceDuration();
		m_flNextSecondaryAttack	= gpGlobals->curtime + SequenceDuration();

		SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseDMOGrenade::ItemPostFrame()
{
	CDMOPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	CBaseViewModel *vm = pPlayer->GetViewModel( m_nViewModelIndex );
	if ( !vm )
		return;

	// If they let go of the fire button, they want to throw the grenade.
	if ( m_bPinPulled && !(pPlayer->m_nButtons & IN_ATTACK) ) 
	{
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_GRENADE1_THROW );

		StartGrenadeThrow();
		
		DecrementAmmo( pPlayer );
	
		m_bPinPulled = false;
		SendWeaponAnim( ACT_VM_THROW );	
		SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );
	}
	else if ((m_fThrowTime > 0) && (m_fThrowTime < gpGlobals->curtime))
	{
		ThrowGrenade();
	}
	else if( m_bRedraw )
	{
		// Has the throw animation finished playing
		if( m_flTimeWeaponIdle < gpGlobals->curtime )
		{
#ifdef GAME_DLL
			// if we're officially out of grenades, ditch this weapon
			if( pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0 )
			{
				pPlayer->Weapon_Drop( this, NULL, NULL );
				UTIL_Remove(this);
			}
			else
			{
				pPlayer->SwitchToNextBestWeapon( this );
			}
#endif
			return;	//don't animate this grenade any more!
		}	
	}
	else if( !m_bRedraw )
	{
		BaseClass::ItemPostFrame();
	}
}



#ifdef CLIENT_DLL

	void CBaseDMOGrenade::DecrementAmmo( CBaseCombatCharacter *pOwner )
	{
	}

	void CBaseDMOGrenade::DropGrenade()
	{
		m_bRedraw = true;
		m_fThrowTime = 0.0f;
	}

	void CBaseDMOGrenade::ThrowGrenade()
	{
		m_bRedraw = true;
		m_fThrowTime = 0.0f;
	}

	void CBaseDMOGrenade::StartGrenadeThrow()
	{
		m_fThrowTime = gpGlobals->curtime + 0.1f;
	}

#else

	BEGIN_DATADESC( CBaseDMOGrenade )
		DEFINE_FIELD( m_bRedraw, FIELD_BOOLEAN ),
	END_DATADESC()

	int CBaseDMOGrenade::CapabilitiesGet()
	{
		return bits_CAP_WEAPON_RANGE_ATTACK1; 
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *pOwner - 
	//-----------------------------------------------------------------------------
	void CBaseDMOGrenade::DecrementAmmo( CBaseCombatCharacter *pOwner )
	{
		pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
	}

	void CBaseDMOGrenade::StartGrenadeThrow()
	{
		m_fThrowTime = gpGlobals->curtime + 0.1f;
	}

	void CBaseDMOGrenade::ThrowGrenade()
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
		if ( !pPlayer )
		{
			Assert( false );
			return;
		}

		QAngle angThrow = pPlayer->LocalEyeAngles();

		Vector vForward, vRight, vUp;

		if (angThrow.x < 90 )
			angThrow.x = -10 + angThrow.x * ((90 + 10) / 90.0);
		else
		{
			angThrow.x = 360.0f - angThrow.x;
			angThrow.x = -10 + angThrow.x * -((90 - 10) / 90.0);
		}

		float flVel = (90 - angThrow.x) * 6;

		if (flVel > 750)
			flVel = 750;

		AngleVectors( angThrow, &vForward, &vRight, &vUp );

		Vector vecSrc = pPlayer->GetAbsOrigin() + pPlayer->GetViewOffset();

		vecSrc += vForward * 16;
	
		Vector vecThrow = vForward * flVel + pPlayer->GetAbsVelocity();

		EmitGrenade( vecSrc, vec3_angle, vecThrow, AngularImpulse(600,random->RandomInt(-1200,1200),0), pPlayer );

		m_bRedraw = true;
		m_fThrowTime = 0.0f;
	}

	void CBaseDMOGrenade::DropGrenade()
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
		if ( !pPlayer )
		{
			Assert( false );
			return;
		}

		Vector vForward;
		pPlayer->EyeVectors( &vForward );
		Vector vecSrc = pPlayer->GetAbsOrigin() + pPlayer->GetViewOffset() + vForward * 16; 

		Vector vecVel = pPlayer->GetAbsVelocity();

		EmitGrenade( vecSrc, vec3_angle, vecVel, AngularImpulse(600,random->RandomInt(-1200,1200),0), pPlayer );

		m_bRedraw = true;
		m_fThrowTime = 0.0f;
	}

	void CBaseDMOGrenade::EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer )
	{
		Assert( 0 && "CBaseDMOGrenade::EmitGrenade should not be called. Make sure to implement this in your subclass!\n" );
	}

	bool CBaseDMOGrenade::AllowsAutoSwitchFrom( void ) const
	{
		return !m_bPinPulled;
	}

#endif

