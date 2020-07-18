//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "in_buttons.h"
#include "takedamageinfo.h"
#include "dmo_weapon_base.h"
#include "ammodef.h"


#if defined( CLIENT_DLL )

	#include "c_dmo_player.h"

#else

	#include "dmo_player.h"

#endif


// ----------------------------------------------------------------------------- //
// Global functions.
// ----------------------------------------------------------------------------- //



// ----------------------------------------------------------------------------- //
// CWeaponDMOBase tables.
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponDMOBase, DT_WeaponDMOBase )

BEGIN_NETWORK_TABLE( CWeaponDMOBase, DT_WeaponDMOBase )
#ifdef CLIENT_DLL
  
#else
	// world weapon models have no animations
  	SendPropExclude( "DT_AnimTimeMustBeFirst", "m_flAnimTime" ),
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponDMOBase )
	DEFINE_PRED_FIELD( m_flTimeWeaponIdle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_NOERRORCHECK ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_dmo_base, CWeaponDMOBase );


#ifdef GAME_DLL

	BEGIN_DATADESC( CWeaponDMOBase )

		// New weapon Think and Touch Functions go here..

	END_DATADESC()

#endif

// ----------------------------------------------------------------------------- //
// CWeaponCSBase implementation. 
// ----------------------------------------------------------------------------- //
CWeaponDMOBase::CWeaponDMOBase()
{
	SetPredictionEligible( true );

	AddSolidFlags( FSOLID_TRIGGER ); // Nothing collides with these but it gets touches.
}

const CDMOWeaponInfo &CWeaponDMOBase::GetDMOWpnData() const
{
	const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
	const CDMOWeaponInfo *pDMOInfo;

	#ifdef _DEBUG
		pDMOInfo = dynamic_cast< const CDMOWeaponInfo* >( pWeaponInfo );
		Assert( pDMOInfo );
	#else
		pDMOInfo = static_cast< const CDMOWeaponInfo* >( pWeaponInfo );
	#endif

	return *pDMOInfo;
}

bool CWeaponDMOBase::PlayEmptySound()
{
	CPASAttenuationFilter filter( this );
	filter.UsePredictionRules();

	EmitSound( filter, entindex(), "Default.ClipEmpty_Rifle" );
	
	return 0;
}

CDMOPlayer* CWeaponDMOBase::GetPlayerOwner() const
{
	return dynamic_cast< CDMOPlayer* >( GetOwner() );
}

#ifdef GAME_DLL

void CWeaponDMOBase::SendReloadEvents()
{
	CDMOPlayer *pPlayer = dynamic_cast< CDMOPlayer* >( GetOwner() );
	if ( !pPlayer )
		return;

	// Send a message to any clients that have this entity to play the reload.
	CPASFilter filter( pPlayer->GetAbsOrigin() );
	filter.RemoveRecipient( pPlayer );

	UserMessageBegin( filter, "ReloadEffect" );
	WRITE_SHORT( pPlayer->entindex() );
	MessageEnd();

	// Make the player play his reload animation.
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );
}

#endif