//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL1.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "dmo_player.h"
#include "dmo_gamerules.h"
#include "dmo_weapon_base.h"
#include "predicted_viewmodel.h"
#include "iservervehicle.h"
#include "viewport_panel_names.h"

extern int gEvilImpulse101;

ConVar sv_motd_unload_on_dismissal( "sv_motd_unload_on_dismissal", "0", 0, "If enabled, the MOTD contents will be unloaded when the player closes the MOTD." );



// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //

class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS( CTEPlayerAnimEvent, CBaseTempEntity );
	DECLARE_SERVERCLASS();

					CTEPlayerAnimEvent( const char *name ) : CBaseTempEntity( name )
					{
					}

	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

#define THROWGRENADE_COUNTER_BITS 3

IMPLEMENT_SERVERCLASS_ST_NOBASE( CTEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropInt( SENDINFO( m_iEvent ), Q_log2( PLAYERANIMEVENT_COUNT ) + 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_nData ), 32 )
END_SEND_TABLE()

static CTEPlayerAnimEvent g_TEPlayerAnimEvent( "PlayerAnimEvent" );

void TE_PlayerAnimEvent( CBasePlayer *pPlayer, PlayerAnimEvent_t event, int nData )
{
	CPVSFilter filter( (const Vector&)pPlayer->EyePosition() );
	
	g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
	g_TEPlayerAnimEvent.m_iEvent = event;
	g_TEPlayerAnimEvent.m_nData = nData;
	g_TEPlayerAnimEvent.Create( filter, 0 );
}

// -------------------------------------------------------------------------------- //
// Tables.
// -------------------------------------------------------------------------------- //
BEGIN_DATADESC( CDMOPlayer )
END_DATADESC()

LINK_ENTITY_TO_CLASS( player, CDMOPlayer );
PRECACHE_REGISTER(player);

BEGIN_SEND_TABLE_NOBASE( CDMOPlayer, DT_DMOLocalPlayerExclusive )
	SendPropInt( SENDINFO( m_iShotsFired ), 8, SPROP_UNSIGNED ),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST( CDMOPlayer, DT_DMOPlayer )
	SendPropExclude( "DT_BaseAnimating", "m_flPoseParameter" ),
	SendPropExclude( "DT_BaseAnimating", "m_flPlaybackRate" ),	
	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),
	SendPropExclude( "DT_BaseAnimatingOverlay", "overlay_vars" ),
	
	// playeranimstate and clientside animation takes care of these on the client
	SendPropExclude( "DT_ServerAnimationData" , "m_flCycle" ),	
	SendPropExclude( "DT_AnimTimeMustBeFirst" , "m_flAnimTime" ),

	// Data that only gets sent to the local player.
	SendPropDataTable( "sdklocaldata", 0, &REFERENCE_SEND_TABLE(DT_DMOLocalPlayerExclusive), SendProxy_SendLocalDataTable ),

	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 0), 11 ),
	SendPropAngle( SENDINFO_VECTORELEM(m_angEyeAngles, 1), 11 ),
	SendPropEHandle( SENDINFO( m_hRagdoll ) ),

	SendPropInt( SENDINFO( m_iThrowGrenadeCounter ), THROWGRENADE_COUNTER_BITS, SPROP_UNSIGNED ),
END_SEND_TABLE()

class CDMORagdoll : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS( CDMORagdoll, CBaseAnimatingOverlay );
	DECLARE_SERVERCLASS();

	// Transmit ragdolls to everyone.
	virtual int UpdateTransmitState()
	{
		return SetTransmitState( FL_EDICT_ALWAYS );
	}

public:
	// In case the client has the player entity, we transmit the player index.
	// In case the client doesn't have it, we transmit the player's model index, origin, and angles
	// so they can create a ragdoll in the right place.
	CNetworkHandle( CBaseEntity, m_hPlayer );	// networked entity handle 
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
};

LINK_ENTITY_TO_CLASS( sdk_ragdoll, CDMORagdoll );

IMPLEMENT_SERVERCLASS_ST_NOBASE( CDMORagdoll, DT_DMORagdoll )
	SendPropVector( SENDINFO(m_vecRagdollOrigin), -1,  SPROP_COORD ),
	SendPropEHandle( SENDINFO( m_hPlayer ) ),
	SendPropModelIndex( SENDINFO( m_nModelIndex ) ),
	SendPropInt		( SENDINFO(m_nForceBone), 8, 0 ),
	SendPropVector	( SENDINFO(m_vecForce), -1, SPROP_NOSCALE ),
	SendPropVector( SENDINFO( m_vecRagdollVelocity ) )
END_SEND_TABLE()


// -------------------------------------------------------------------------------- //

void cc_CreatePredictionError_f()
{
	CBaseEntity *pEnt = CBaseEntity::Instance( 1 );
	pEnt->SetAbsOrigin( pEnt->GetAbsOrigin() + Vector( 63, 0, 0 ) );
}

ConCommand cc_CreatePredictionError( "CreatePredictionError", cc_CreatePredictionError_f, "Create a prediction error", FCVAR_CHEAT );


CDMOPlayer::CDMOPlayer()
{
	m_PlayerAnimState = CreatePlayerAnimState( this );

	UseClientSideAnimation();
	m_angEyeAngles.Init();


	m_iThrowGrenadeCounter = 0;
}


CDMOPlayer::~CDMOPlayer()
{
	m_PlayerAnimState->Release();
}


CDMOPlayer *CDMOPlayer::CreatePlayer( const char *className, edict_t *ed )
{
	CDMOPlayer::s_PlayerEdict = ed;
	return (CDMOPlayer*)CreateEntityByName( className );
}

void CDMOPlayer::LeaveVehicle( const Vector &vecExitPoint, const QAngle &vecExitAngles )
{
	BaseClass::LeaveVehicle( vecExitPoint, vecExitAngles );

	//teleport physics shadow too
	// Vector newPos = GetAbsOrigin();
	// QAngle newAng = GetAbsAngles();

	// Teleport( &newPos, &newAng, &vec3_origin );
}

void CDMOPlayer::PreThink(void)
{
	// Riding a vehicle?
	if ( IsInAVehicle() )	
	{
		// make sure we update the client, check for timed damage and update suit even if we are in a vehicle
		UpdateClientData();		
		CheckTimeBasedDamage();

		// Allow the suit to recharge when in the vehicle.
		CheckSuitUpdate();
		
		WaterMove();	
		return;
	}

	BaseClass::PreThink();
}


void CDMOPlayer::PostThink()
{
	BaseClass::PostThink();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles( angles );
	
	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

    m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );
}


void CDMOPlayer::Precache()
{
	PrecacheModel( DMO_PLAYER_MODEL );

	BaseClass::Precache();
}

void CDMOPlayer::Spawn()
{
	SetModel( DMO_PLAYER_MODEL );
	SetMoveType( MOVETYPE_WALK );
	RemoveSolidFlags( FSOLID_NOT_SOLID );

	m_hRagdoll = NULL;
	
	BaseClass::Spawn();
}

void CDMOPlayer::InitialSpawn( void )
{
	BaseClass::InitialSpawn();

	const ConVar *hostname = cvar->FindVar( "hostname" );
	const char *title = (hostname) ? hostname->GetString() : "MESSAGE OF THE DAY";

	// open info panel on client showing MOTD:
	KeyValues *data = new KeyValues("data");
	data->SetString( "title", title );		// info panel title
	data->SetString( "type", "1" );			// show userdata from stringtable entry
	data->SetString( "msg",	"motd" );		// use this stringtable entry
	data->SetInt( "cmd", TEXTWINDOW_CMD_IMPULSE101 );// exec this command if panel closed
	data->SetBool( "unload", sv_motd_unload_on_dismissal.GetBool() );

	ShowViewPortPanel( PANEL_INFO, true, data );

	data->deleteThis();
}

void CDMOPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	// Note: since we're dead, it won't draw us on the client, but we don't set EF_NODRAW
	// because we still want to transmit to the clients in our PVS.

	BaseClass::Event_Killed( info );

	CreateRagdollEntity();
}

void CDMOPlayer::CreateRagdollEntity()
{
	// If we already have a ragdoll, don't make another one.
	CDMORagdoll *pRagdoll = dynamic_cast< CDMORagdoll* >( m_hRagdoll.Get() );

	if ( !pRagdoll )
	{
		// create a new one
		pRagdoll = dynamic_cast< CDMORagdoll* >( CreateEntityByName( "sdk_ragdoll" ) );
	}

	if ( pRagdoll )
	{
		pRagdoll->m_hPlayer = this;
		pRagdoll->m_vecRagdollOrigin = GetAbsOrigin();
		pRagdoll->m_vecRagdollVelocity = GetAbsVelocity();
		pRagdoll->m_nModelIndex = m_nModelIndex;
		pRagdoll->m_nForceBone = m_nForceBone;
		pRagdoll->m_vecForce = Vector(0,0,0);
	}

	// ragdolls will be removed on round restart automatically
	m_hRagdoll = pRagdoll;
}

void CDMOPlayer::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	if ( event == PLAYERANIMEVENT_GRENADE1_THROW )
	{
		// Grenade throwing has to synchronize exactly with the player's grenade weapon going away,
		// and events get delayed a bit, so we let CCSPlayerAnimState pickup the change to this
		// variable.
		m_iThrowGrenadeCounter = (m_iThrowGrenadeCounter+1) % (1<<THROWGRENADE_COUNTER_BITS);
	}
	else
	{
		m_PlayerAnimState->DoAnimationEvent( event, nData );
		TE_PlayerAnimEvent( this, event, nData );	// Send to any clients who can see this guy.
	}
}

CWeaponDMOBase* CDMOPlayer::GetActiveDMOWeapon() const
{
	return dynamic_cast< CWeaponDMOBase* >( GetActiveWeapon() );
}

void CDMOPlayer::CreateViewModel( int index /*=0*/ )
{
	Assert( index >= 0 && index < MAX_VIEWMODELS );

	if ( GetViewModel( index ) )
		return;

	CPredictedViewModel *vm = ( CPredictedViewModel * )CreateEntityByName( "predicted_viewmodel" );
	if ( vm )
	{
		vm->SetAbsOrigin( GetAbsOrigin() );
		vm->SetOwner( this );
		vm->SetIndex( index );
		DispatchSpawn( vm );
		vm->FollowEntity( this, false );
		m_hViewModel.Set( index, vm );
	}
}

void CDMOPlayer::CheatImpulseCommands( int iImpulse )
{
	if ( iImpulse != 101 )
	{
		BaseClass::CheatImpulseCommands( iImpulse );
		return ;
	}
	gEvilImpulse101 = true;

	EquipSuit();

	GiveNamedItem( "weapon_mp5" );
	GiveNamedItem( "weapon_grenade" );
	GiveNamedItem( "weapon_shotgun" );

	// Give the player everything!
	GiveAmmo( 90, AMMO_BULLETS );
	GiveAmmo( 3, AMMO_GRENADE );
	
	if ( GetHealth() < 100 )
	{
		TakeHealth( 25, DMG_GENERIC );
	}

	gEvilImpulse101		= false;
}


void CDMOPlayer::FlashlightTurnOn( void )
{
	AddEffects( EF_DIMLIGHT );
}

void CDMOPlayer::FlashlightTurnOff( void )
{
	RemoveEffects( EF_DIMLIGHT );
}

int CDMOPlayer::FlashlightIsOn( void )
{
	return IsEffectActive( EF_DIMLIGHT );
}
