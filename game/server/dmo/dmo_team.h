//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Team management class. Contains all the details for a specific team
//
// $NoKeywords: $
//=============================================================================//

#ifndef DMO_TEAM_H
#define DMO_TEAM_H

#ifdef _WIN32
#pragma once
#endif


#include "utlvector.h"
#include "team.h"


//-----------------------------------------------------------------------------
// Purpose: Team Manager
//-----------------------------------------------------------------------------
class CDMOTeam : public CTeam
{
	DECLARE_CLASS( CDMOTeam, CTeam );
	DECLARE_SERVERCLASS();

public:

	// Initialization
	virtual void Init( const char *pName, int iNumber );
};


extern CDMOTeam *GetGlobalDMOTeam( int iIndex );


#endif // TF_TEAM_H
