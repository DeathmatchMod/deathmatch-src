//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The TF Game rules object
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#ifndef DMO_GAMERULES_H
#define DMO_GAMERULES_H

#ifdef _WIN32
#pragma once
#endif


#include "teamplay_gamerules.h"
#include "convar.h"
#include "gamevars_shared.h"

#ifdef CLIENT_DLL
	#include "c_baseplayer.h"
#else
	#include "player.h"
#endif


#ifdef CLIENT_DLL
	#define CDMOGameRules C_DMOGameRules
	#define CDMOGameRulesProxy C_DMOGameRulesProxy
#endif


class CDMOGameRulesProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS( CDMOGameRulesProxy, CGameRulesProxy );
	DECLARE_NETWORKCLASS();
};


class CDMOGameRules : public CTeamplayRules
{
public:
	DECLARE_CLASS( CDMOGameRules, CTeamplayRules );

	virtual bool	ShouldCollide( int collisionGroup0, int collisionGroup1 );

	virtual int		PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual bool	IsTeamplay( void ) { return false;	}

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.
	
	CDMOGameRules();
	virtual ~CDMOGameRules();

	virtual bool ClientCommand( CBaseEntity *pEdict, const CCommand &args );
	virtual void RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore );
	virtual void Think();

	virtual const char *GetChatPrefix( bool bTeamOnly, CBasePlayer *pPlayer );

private:

	void RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore, bool bIgnoreWorld );


#endif
};

//-----------------------------------------------------------------------------
// Gets us at the team fortress game rules
//-----------------------------------------------------------------------------

inline CDMOGameRules* DMOGameRules()
{
	return static_cast<CDMOGameRules*>(g_pGameRules);
}


#endif // DMO_GAMERULES_H
