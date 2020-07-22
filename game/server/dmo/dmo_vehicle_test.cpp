#include "cbase.h"
#include "engine/IEngineSound.h"
#include "in_buttons.h"
#include "ammodef.h"
#include "IEffects.h"
#include "beam_shared.h"
#include "soundenvelope.h"
#include "decals.h"
#include "soundent.h"
#include "te_effect_dispatch.h"
#include "ndebugoverlay.h"
#include "movevars_shared.h"
#include "bone_setup.h"
#include "ai_basenpc.h"
#include "ai_hint.h"
#include "globalstate.h"
#include "dmo_player.h"
#include "eventqueue.h"
#include "rumble_shared.h"
#include "haptics/haptic_utils.h"
#include "vehicle_base.h"

// memdbgon must be the last include file in a .cpp file!!!
// memdbgon is also dumb lmao!
#include "tier0/memdbgon.h"

class CPropTestVehicle : public CPropVehicleDriveable
{
public:
	DECLARE_CLASS( CPropTestVehicle, CPropVehicleDriveable );

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPropTestVehicle( void );

	// CPropVehicle
	void	ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMoveData ) override;
	void	DriveVehicle( float flFrameTime, CUserCmd *ucmd, int iButtonsDown, int iButtonsReleased ) override;
	void	SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move ) override;
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void	DampenEyePosition( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles ) override;
	virtual bool	AllowBlockedExit( CBasePlayer *pPlayer, int nRole ) { return false; }
	bool	CanExitVehicle( CBaseEntity *pEntity ) override;
	bool	IsVehicleBodyInWater() override { return false; }

	// Passengers do not directly receive damage from blasts or radiation damage
	bool PassengerShouldReceiveDamage( CTakeDamageInfo &info ) override
	{
		if ( GetServerVehicle() && GetServerVehicle()->IsPassengerExiting() )
			return false;

		if ( info.GetDamageType() & DMG_VEHICLE )
			return true;

		return (info.GetDamageType() & (DMG_RADIATION|DMG_BLAST) ) == 0;
	}

	// CBaseEntity
	void			Think(void) override;
	void			Precache( void ) override;
	void			Spawn( void ) override;
	void			Activate( void ) override;

	void	CreateServerVehicle( void ) override;
	Vector	BodyTarget( const Vector &posSrc, bool bNoisy = true ) override;
	void	TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator ) override;
	int	OnTakeDamage( const CTakeDamageInfo &info ) override;
	virtual float	PassengerDamageModifier( const CTakeDamageInfo &info );

	virtual void	EnterVehicle( CBaseCombatCharacter *pPassenger ) override;
	virtual void	ExitVehicle( int nRole ) override;

	void			AimGunAt( Vector *endPos, float flInterval );

	// NPC Driving
	bool			NPC_HasPrimaryWeapon( void ) { return true; }
	void			NPC_AimPrimaryWeapon( Vector vecTarget );

	const char		*GetTracerType( void ) override { return "AR2Tracer"; }
	void			DoImpactEffect( trace_t &tr, int nDamageType ) override;

	bool HeadlightIsOn( void ) { return m_bHeadlightIsOn; }
	void HeadlightTurnOn( void ) { m_bHeadlightIsOn = true; }
	void HeadlightTurnOff( void ) { m_bHeadlightIsOn = false; }

private:

	//void		FireCannon( void );
	//void		ChargeCannon( void );
	//void		FireChargedCannon( void );

	//void		DrawBeam( const Vector &startPos, const Vector &endPos, float width );
	//void		StopChargeSound( void );
	//void		GetCannonAim( Vector *resultDir );

	//void		InitWaterData( void );
	//void		CheckWaterLevel( void );
	//void		CreateSplash( const Vector &vecPosition );
	//void		CreateRipple( const Vector &vecPosition );

	//void		CreateDangerSounds( void );

	//void		ComputePDControllerCoefficients( float *pCoefficientsOut, float flFrequency, float flDampening, float flDeltaTime );
	//void		DampenForwardMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime );
	//void		DampenUpMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime );

	//void		JeepSeagullThink( void );
	//void		SpawnPerchedSeagull( void );
	//void		AddSeagullPoop( const Vector &vecOrigin );

	//void		InputShowHudHint( inputdata_t &inputdata );
	//void		InputStartRemoveTauCannon( inputdata_t &inputdata );
	//void		InputFinishRemoveTauCannon( inputdata_t &inputdata );

protected:

	virtual void HandleWater();
	bool		 CheckWater();

	CNetworkVar( bool, m_bHeadlightIsOn );
};

class CPropTestServerVehicle : public CFourWheelServerVehicle
{

};

IMPLEMENT_SERVERCLASS_ST( CPropTestVehicle, DT_PropTestVehicle )
			SendPropBool( SENDINFO( m_bHeadlightIsOn ) ),
END_SEND_TABLE();

BEGIN_DATADESC( CPropTestVehicle )

END_DATADESC()

LINK_ENTITY_TO_CLASS( prop_vehicle_test, CPropTestVehicle );


CPropTestVehicle::CPropTestVehicle()
{
	m_bHeadlightIsOn = false;
}

void CPropTestVehicle::ProcessMovement(CBasePlayer *pPlayer, CMoveData *pMoveData)
{
	CPropVehicleDriveable::ProcessMovement(pPlayer, pMoveData);
}

void CPropTestVehicle::DriveVehicle(float flFrameTime, CUserCmd *ucmd, int iButtonsDown, int iButtonsReleased)
{
	CPropVehicleDriveable::DriveVehicle(flFrameTime, ucmd, iButtonsDown, iButtonsReleased);
}

void CPropTestVehicle::SetupMove(CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move)
{
	CPropVehicleDriveable::SetupMove(player, ucmd, pHelper, move);
}

void CPropTestVehicle::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CPropVehicleDriveable::Use(pActivator, pCaller, useType, value);
}

void CPropTestVehicle::DampenEyePosition(Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles)
{

}

void CPropTestVehicle::Think()
{
	BaseClass::Think();
}

void CPropTestVehicle::Precache()
{
	BaseClass::Precache();
}

void CPropTestVehicle::Spawn()
{
	SetVehicleType(VEHICLE_TYPE_CAR_WHEELS);
	SetCollisionGroup(COLLISION_GROUP_VEHICLE);
	BaseClass::Spawn();
	AddSolidFlags(FSOLID_NOT_STANDABLE);
}

void CPropTestVehicle::Activate()
{
	CBaseProp::Activate();
}

bool CPropTestVehicle::CanExitVehicle(CBaseEntity *pEntity)
{
	return CPropVehicleDriveable::CanExitVehicle(pEntity);
}

void CPropTestVehicle::CreateServerVehicle(void)
{
	m_pServerVehicle = new CPropTestServerVehicle();
	m_pServerVehicle->SetVehicle( this );
}

Vector CPropTestVehicle::BodyTarget(const Vector &posSrc, bool bNoisy)
{
	return Vector(0,0,0);
}

void CPropTestVehicle::TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr,
                                   CDmgAccumulator *pAccumulator)
{
	CPropVehicleDriveable::TraceAttack(info, vecDir, ptr, pAccumulator);
}

int CPropTestVehicle::OnTakeDamage(const CTakeDamageInfo &info)
{
	return (int)info.GetDamage();
}

void CPropTestVehicle::EnterVehicle(CBaseCombatCharacter *pPassenger)
{

}

void CPropTestVehicle::ExitVehicle(int nRole)
{

}

float CPropTestVehicle::PassengerDamageModifier(const CTakeDamageInfo &info)
{
	return 0;
}

void CPropTestVehicle::AimGunAt(Vector *endPos, float flInterval)
{

}

void CPropTestVehicle::NPC_AimPrimaryWeapon(Vector vecTarget)
{

}

void CPropTestVehicle::DoImpactEffect(trace_t &tr, int nDamageType)
{
	CBaseEntity::DoImpactEffect(tr, nDamageType);
}

void CPropTestVehicle::HandleWater()
{

}

bool CPropTestVehicle::CheckWater()
{
	return false;
}
