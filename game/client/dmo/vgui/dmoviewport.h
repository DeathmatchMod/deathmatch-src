//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef DMOVIEWPORT_H
#define DMOVIEWPORT_H


#include "dmo_shareddefs.h"
#include "baseviewport.h"


using namespace vgui;

namespace vgui 
{
	class Panel;
}

class DMOViewport : public CBaseViewport
{

private:
	DECLARE_CLASS_SIMPLE( DMOViewport, CBaseViewport );

public:

	IViewPortPanel* CreatePanelByName(const char *szPanelName);
	void CreateDefaultPanels( void );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
		
	int GetDeathMessageStartHeight( void );
};


#endif // DMOViewport_H
