//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef DMO_BOT_TEMP_H
#define DMO_BOT_TEMP_H
#ifdef _WIN32
#pragma once
#endif


// If iTeam or iClass is -1, then a team or class is randomly chosen.
CBasePlayer *BotPutInServer( bool bFrozen, int iTeam, int iClass );

void Bot_RunAll();


#endif // DMO_BOT_TEMP_H
