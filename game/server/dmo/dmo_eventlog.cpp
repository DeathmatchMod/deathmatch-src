//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "../EventLog.h"
#include "KeyValues.h"

class CDMOEventLog : public CEventLog
{
private:
	typedef CEventLog BaseClass;

public:
	virtual ~CDMOEventLog() {};

public:
	bool PrintEvent( IGameEvent * event )	// override virtual function
	{
		if ( BaseClass::PrintEvent( event ) )
		{
			return true;
		}
	
		if ( Q_strcmp(event->GetName(), "sdk_") == 0 )
		{
			return PrintDMOEvent( event );
		}

		return false;
	}

protected:

	bool PrintDMOEvent( IGameEvent * event )	// print Mod specific logs
	{
		//const char * name = event->GetName() + Q_strlen("sdk_"); // remove prefix
		return false;
	}

};

CDMOEventLog g_DMOEventLog;

//-----------------------------------------------------------------------------
// Singleton access
//-----------------------------------------------------------------------------
IGameSystem* GameLogSystem()
{
	return &g_DMOEventLog;
}

