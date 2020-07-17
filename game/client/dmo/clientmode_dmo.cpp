//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//
#include "cbase.h"
#include "hud.h"
#include "clientmode_dmo.h"
#include "cdll_client_int.h"
#include "iinput.h"
#include "vgui/ISurface.h"
#include "vgui/IPanel.h"
#include <vgui_controls/AnimationController.h>
#include "ivmodemanager.h"
#include "buymenu.h"
#include "filesystem.h"
#include "vgui/IVGui.h"
#include "hud_chat.h"
#include "view_shared.h"
#include "view.h"
#include "ivrenderview.h"
#include "model_types.h"
#include "iefx.h"
#include "dlight.h"
#include <imapoverview.h>
#include "c_playerresource.h"
#include <KeyValues.h>
#include "text_message.h"
#include "panelmetaclassmgr.h"


ConVar default_fov( "default_fov", "90", FCVAR_CHEAT );

IClientMode *g_pClientMode = NULL;


// --------------------------------------------------------------------------------- //
// CDMOModeManager.
// --------------------------------------------------------------------------------- //

class CDMOModeManager : public IVModeManager
{
public:
	virtual void	Init();
	virtual void	SwitchMode( bool commander, bool force ) {}
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
	virtual void	ActivateMouse( bool isactive ) {}
};

static CDMOModeManager g_ModeManager;
IVModeManager *modemanager = ( IVModeManager * )&g_ModeManager;

// --------------------------------------------------------------------------------- //
// CDMOModeManager implementation.
// --------------------------------------------------------------------------------- //

#define SCREEN_FILE		"scripts/vgui_screens.txt"

void CDMOModeManager::Init()
{
	g_pClientMode = GetClientModeNormal();
	
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void CDMOModeManager::LevelInit( const char *newmap )
{
	g_pClientMode->LevelInit( newmap );
}

void CDMOModeManager::LevelShutdown( void )
{
	g_pClientMode->LevelShutdown();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ClientModeDMONormal::ClientModeDMONormal()
{
}

//-----------------------------------------------------------------------------
// Purpose: If you don't know what a destructor is by now, you are probably going to get fired
//-----------------------------------------------------------------------------
ClientModeDMONormal::~ClientModeDMONormal()
{
}


void ClientModeDMONormal::InitViewport()
{
	m_pViewport = new DMOViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
}

ClientModeDMONormal g_ClientModeNormal;

IClientMode *GetClientModeNormal()
{
	return &g_ClientModeNormal;
}


ClientModeDMONormal* GetClientModeDMONormal()
{
	Assert( dynamic_cast< ClientModeDMONormal* >( GetClientModeNormal() ) );

	return static_cast< ClientModeDMONormal* >( GetClientModeNormal() );
}

float ClientModeDMONormal::GetViewModelFOV( void )
{
	return 74.0f;
}

int ClientModeDMONormal::GetDeathMessageStartHeight( void )
{
	return m_pViewport->GetDeathMessageStartHeight();
}

void ClientModeDMONormal::PostRenderVGui()
{
}




