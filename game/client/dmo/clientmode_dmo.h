//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef DMO_CLIENTMODE_H
#define DMO_CLIENTMODE_H
#ifdef _WIN32
#pragma once
#endif

#include "clientmode_shared.h"
#include "dmoviewport.h"

class ClientModeDMONormal : public ClientModeShared 
{
DECLARE_CLASS( ClientModeDMONormal, ClientModeShared );

private:

// IClientMode overrides.
public:

					ClientModeDMONormal();
	virtual			~ClientModeDMONormal();

	virtual void	InitViewport();

	virtual float	GetViewModelFOV( void );

	int				GetDeathMessageStartHeight( void );

	virtual void	PostRenderVGui();

	
private:
	
	//	void	UpdateSpectatorMode( void );

};


extern IClientMode *GetClientModeNormal();
extern ClientModeDMONormal* GetClientModeDMONormal();


#endif // DMO_CLIENTMODE_H
