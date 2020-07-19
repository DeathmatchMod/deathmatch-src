//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "basetempentity.h"
#include "te_basebeam.h"
#include "dmo_te.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


class CTELaserSniperBeam : public CBaseTempEntity
{
public:
	DECLARE_CLASS( CTELaserSniperBeam , CBaseTempEntity);
	DECLARE_SERVERCLASS();

					CTELaserSniperBeam ( const char *name );
	virtual			~CTELaserSniperBeam( void );

	virtual void	Test( const Vector& current_origin, const QAngle& current_angles );
	

public:
	CNetworkHandle( CBasePlayer, m_hOwnerPlayer)
	CNetworkVar( int, m_iAttachmentIndex );
	CNetworkVector( m_vecEndPoint );
	CNetworkVar( float, m_fLife );
	CNetworkVar( float, m_fWidth);

};

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *name - 
//-----------------------------------------------------------------------------
CTELaserSniperBeam::CTELaserSniperBeam( const char *name ) :
	CBaseTempEntity( name )
{
	m_hOwnerPlayer = nullptr;
	m_iAttachmentIndex = 1;
	m_vecEndPoint.Init();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTELaserSniperBeam ::~CTELaserSniperBeam( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *current_origin - 
//			*current_angles - 
//-----------------------------------------------------------------------------
void CTELaserSniperBeam::Test( const Vector& current_origin, const QAngle& current_angles )
{
	assert(false);
}

IMPLEMENT_SERVERCLASS_ST(CTELaserSniperBeam, DT_TELaserSniperBeam)
	SendPropEHandle( SENDINFO(m_hOwnerPlayer) ),
	SendPropInt( SENDINFO(m_iAttachmentIndex), 24, SPROP_UNSIGNED ),
	SendPropVector( SENDINFO(m_vecEndPoint), -1, SPROP_COORD ),
	SendPropFloat( SENDINFO( m_fLife ) ),
	SendPropFloat( SENDINFO( m_fWidth ) ),
END_SEND_TABLE()


// Singleton to fire TEBeamEntPoint objects
static CTELaserSniperBeam  g_TELaserSniperBeam( "LaserSniperBeam" );

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :
//-----------------------------------------------------------------------------
void TE_LaserSniperBeam( IRecipientFilter& filter,
	CBasePlayer* hOwnerPlayer, int iAttachmentIndex, const Vector end,
	float life, float width )
{
	g_TELaserSniperBeam.m_hOwnerPlayer = hOwnerPlayer;
	g_TELaserSniperBeam.m_iAttachmentIndex = iAttachmentIndex;
	g_TELaserSniperBeam.m_vecEndPoint = end;
	
	g_TELaserSniperBeam.m_fLife			= life;
	g_TELaserSniperBeam.m_fWidth		= width;
	
	// Send it over the wire
	g_TELaserSniperBeam.Create( filter );
}