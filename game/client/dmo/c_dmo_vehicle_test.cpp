#include "cbase.h"
#include "c_prop_vehicle.h"
#include "flashlighteffect.h"

class C_TestVehicle : public C_PropVehicleDriveable
{
public:
	C_TestVehicle();
	~C_TestVehicle();

	DECLARE_CLASS(C_TestVehicle, C_PropVehicleDriveable);
	DECLARE_INTERPOLATION();
	DECLARE_CLIENTCLASS();

	bool m_bHeadlightIsOn;
};

IMPLEMENT_CLIENTCLASS_DT( C_TestVehicle, DT_PropTestVehicle, CPropTestVehicle)
			RecvPropBool( RECVINFO( m_bHeadlightIsOn ) ),
END_RECV_TABLE()

C_TestVehicle::C_TestVehicle()
{

}

C_TestVehicle::~C_TestVehicle()
{

}

