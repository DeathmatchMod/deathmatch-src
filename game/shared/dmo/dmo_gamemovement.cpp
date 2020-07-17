//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "gamemovement.h"
#include "dmo_gamerules.h"
#include "dmo_shareddefs.h"
#include "in_buttons.h"
#include "movevars_shared.h"


#ifdef CLIENT_DLL
	#include "c_dmo_player.h"
#else
	#include "dmo_player.h"
#endif


class CDMOGameMovement : public CGameMovement
{
public:
	DECLARE_CLASS( CDMOGameMovement, CGameMovement );

	CDMOGameMovement();
};


// Expose our interface.
static CDMOGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = ( IGameMovement * )&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameMovement, IGameMovement,INTERFACENAME_GAMEMOVEMENT, g_GameMovement );


// ---------------------------------------------------------------------------------------- //
// CDMOGameMovement.
// ---------------------------------------------------------------------------------------- //

CDMOGameMovement::CDMOGameMovement()
{
	//m_vecViewOffsetNormal = DMO_PLAYER_VIEW_OFFSET;
}

