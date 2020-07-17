//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "vgui_int.h"
#include "ienginevgui.h"
#include "vgui_rootpanel_dmo.h"
#include "vgui/IVGui.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

C_DMORootPanel *g_pRootPanel = NULL;


//-----------------------------------------------------------------------------
// Global functions.
//-----------------------------------------------------------------------------
void VGUI_CreateClientDLLRootPanel( void )
{
	g_pRootPanel = new C_DMORootPanel( enginevgui->GetPanel( PANEL_CLIENTDLL ) );
}

void VGUI_DestroyClientDLLRootPanel( void )
{
	delete g_pRootPanel;
	g_pRootPanel = NULL;
}

vgui::VPANEL VGui_GetClientDLLRootPanel( void )
{
	return g_pRootPanel->GetVPanel();
}


//-----------------------------------------------------------------------------
// C_DMORootPanel implementation.
//-----------------------------------------------------------------------------
C_DMORootPanel::C_DMORootPanel( vgui::VPANEL parent )
	: BaseClass( NULL, "DMO Root Panel" )
{
	SetParent( parent );
	SetPaintEnabled( false );
	SetPaintBorderEnabled( false );
	SetPaintBackgroundEnabled( false );

	// This panel does post child painting
	SetPostChildPaintEnabled( true );

	// Make it screen sized
	SetBounds( 0, 0, ScreenWidth(), ScreenHeight() );

	// Ask for OnTick messages
	vgui::ivgui()->AddTickSignal( GetVPanel() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_DMORootPanel::~C_DMORootPanel( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_DMORootPanel::PostChildPaint()
{
	BaseClass::PostChildPaint();

	// Draw all panel effects
	RenderPanelEffects();
}

//-----------------------------------------------------------------------------
// Purpose: For each panel effect, check if it wants to draw and draw it on
//  this panel/surface if so
//-----------------------------------------------------------------------------
void C_DMORootPanel::RenderPanelEffects( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_DMORootPanel::OnTick( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Reset effects on level load/shutdown
//-----------------------------------------------------------------------------
void C_DMORootPanel::LevelInit( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_DMORootPanel::LevelShutdown( void )
{
}

