//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_te_basebeam.h"
#include "iviewrender_beams.h"
#include "util_shared.h"
#include "c_dmo_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SNIPER_BEAM_SPRITE "sprites/bluelaser1.vmt"
//-----------------------------------------------------------------------------
// Purpose: LaserSniperBeam TE
//-----------------------------------------------------------------------------
class C_TELaserSniperBeam : public C_TEBaseBeam
{
public:
	DECLARE_CLASS( C_TELaserSniperBeam, C_TEBaseBeam );
	DECLARE_CLIENTCLASS();

					C_TELaserSniperBeam( void );
	virtual			~C_TELaserSniperBeam( void );

	virtual void	PostDataUpdate( DataUpdateType_t updateType );

public:
	CNetworkHandle(CBasePlayer, m_hOwnerPlayer)
	CNetworkVar(int, m_iAttachmentIndex);
	CNetworkVector(m_vecEndPoint);
	CNetworkVar( float, m_fLife );
	CNetworkVar( float, m_fWidth);
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TELaserSniperBeam::C_TELaserSniperBeam( void )
{
	m_hOwnerPlayer = nullptr;
	m_iAttachmentIndex = 1;
	m_vecEndPoint.Init();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TELaserSniperBeam::~C_TELaserSniperBeam( void )
{
}

void TE_LaserSniperBeam( CBasePlayer* hOwnerPlayer, int iAttachmentIndex, const Vector end, float life, float width)
{
	Vector vecStartPos;


	CDMOPlayer* player = ToDMOPlayer(hOwnerPlayer);
	if (!player)
		return;
	player->GetRenderedWeaponModel()->GetAttachment(iAttachmentIndex, vecStartPos);
	//DebugDrawLine(vecStartPos, m_vecEndPoint, 255, 255, 0, false, 100);

	BeamInfo_t info;
	info.m_vecStart = vecStartPos;
	info.m_vecEnd = end;
	info.m_flLife = life;
	info.m_flWidth = width;
	info.m_flEndWidth = width;
	info.m_flFadeLength = 0;
	info.m_flAmplitude = 0;
	info.m_flBrightness = 200.0;
	info.m_flSpeed = 0;
	info.m_nStartFrame = 0;
	info.m_flFrameRate = 0;
	info.m_flRed = 255;
	info.m_flGreen = 255;
	info.m_flBlue = 255;
	info.m_pszModelName = SNIPER_BEAM_SPRITE;
	info.m_nSegments = 8;

	beams->CreateBeamPoints(info);
}

void TE_LaserSniperBeam(IRecipientFilter& filter,
	CBasePlayer* hOwnerPlayer, int iAttachmentIndex, const Vector end,
	float life, float width)
{
	TE_LaserSniperBeam(hOwnerPlayer, iAttachmentIndex, end, life, width);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : bool - 
//-----------------------------------------------------------------------------
void C_TELaserSniperBeam::PostDataUpdate( DataUpdateType_t updateType )
{
	TE_LaserSniperBeam(m_hOwnerPlayer, m_iAttachmentIndex, m_vecEndPoint, m_fLife, m_fWidth);
}

IMPLEMENT_CLIENTCLASS_EVENT_DT(C_TELaserSniperBeam, DT_TELaserSniperBeam, CTELaserSniperBeam)
	RecvPropEHandle(RECVINFO(m_hOwnerPlayer)),
	RecvPropInt(RECVINFO(m_iAttachmentIndex)),
	RecvPropVector(RECVINFO(m_vecEndPoint)),
	RecvPropFloat(RECVINFO(m_fLife)),
	RecvPropFloat(RECVINFO(m_fWidth)),
END_RECV_TABLE()

