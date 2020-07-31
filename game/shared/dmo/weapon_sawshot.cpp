#include "cbase.h"
#include "dmo_weapon_base.h"
#include "beam_shared.h"
#include "in_buttons.h"

#if defined( CLIENT_DLL )

#define CWeaponSawshot C_WeaponSawshot
#include "c_dmo_player.h"

#else

#include "dmo_player.h"

#endif

#define SAWSHOT_BLADE "models/gibs/agibs.mdl"
#define SAWSHOT_PROJECTILE_SPEED 1024
#define SAWSHOT_PROJECTILE_RETRACT_SPEED 512
#define SAWSHOT_EXTEND_DAMAGE_PER_TOUCH 40
#define SAWSHOT_RETRACT_DAMAGE_PER_TOUCH 10
#define SAWSHOT_PROJECTILE_DELETE_DIST 40
#define SAWSHOT_PROJECTILE_REACH 128

// Sawblade Projectile

#ifdef DMO_SERVER

class CSawProjectile : public CBaseAnimating
{
public:
	DECLARE_CLASS(CSawProjectile, CBaseAnimating);
	
	static CSawProjectile* Create(const Vector& vecOrigin, const QAngle& angFireDirection, CBaseEntity* pOwner);


	virtual void Spawn();
	void ExtendTouch(CBaseEntity* pOther);
	void RetractTouch(CBaseEntity* pOther);
	void ReturnToSender();
	void ExtendThink();
	void RetractThink();

	bool m_bReturning;
	
};

CSawProjectile* CSawProjectile::Create(const Vector& vecOrigin, const QAngle& angFireDirection, CBaseEntity* pOwner)
{
	CSawProjectile* saw = static_cast<CSawProjectile*>(CBaseEntity::Create("projectile_saw", vecOrigin, angFireDirection, pOwner));
	
	return saw;
}

void CSawProjectile::Spawn()
{
	BaseClass::Spawn();

	SetModel(SAWSHOT_BLADE);

	UTIL_SetSize(this, -Vector(2, 2, 2), Vector(2, 2, 2));
	SetSolid(SOLID_BBOX);
	SetMoveType(MOVETYPE_FLY);
	

	SetTouch(&CSawProjectile::ExtendTouch);
	SetThink(&CSawProjectile::ExtendThink);
	SetNextThink(gpGlobals->curtime + 0.1f);
	m_bReturning = false;
}

void CSawProjectile::ExtendTouch(CBaseEntity* pOther)
{
	// Ignore triggers
	if (pOther->IsSolidFlagSet(FSOLID_TRIGGER))
		return;

	
	// Apply damage to whatever we hit if we can and return to sender if we're not on our way home
	if (pOther->IsWorld())
	{
		//CHECK IF GLASS!!
	}
	else
	{
		CTakeDamageInfo dmgInfo;
		dmgInfo.SetDamage(SAWSHOT_EXTEND_DAMAGE_PER_TOUCH);

		pOther->TakeDamage(dmgInfo);
	}


	// MAKE IT KEEP GOING INSTEAD OF RETRACTING IF WE BREAK IT!


	if (!pOther->IsAlive() || pOther->IsWorld())
	{
		Vector forward;
		GetVectors(&forward, nullptr, nullptr);
		const trace_t& trace = GetTouchTrace();
		float dot = -DotProduct(trace.plane.normal, forward);

		if (dot < 0.342f) // if the blade hits an angle of about 20 degrees, ricochet
		{
			forward += 2 * trace.plane.normal * dot;

			DebugDrawLine(trace.endpos, trace.endpos + trace.plane.normal * 10, 255, 0, 255, true, 1000);

			DebugDrawLine(trace.endpos, trace.endpos + forward * 10, 0, 0, 255, true, 1000);


			SetAbsVelocity(forward * SAWSHOT_PROJECTILE_SPEED);


			QAngle angles;
			VectorAngles(forward, angles);
			SetAbsAngles(angles);

			//EmitSound
		}
		else
		{
			ReturnToSender();
		}
	}
	else
	{
		ReturnToSender();
	}

}

void CSawProjectile::ExtendThink()
{
	// we have to update the velocity constantly for some reason. annoying... 

	Vector forward;
	GetVectors(&forward, nullptr, nullptr);
	SetAbsVelocity(forward * SAWSHOT_PROJECTILE_SPEED);


	// Are we out of range of the owner yet?
	CDMOPlayer* owner = ToDMOPlayer(GetOwnerEntity());
	if (owner && owner->IsAlive())
	{
		Vector posDif = owner->EyePosition() - GetAbsOrigin();

		// We're out of rope! Let's retract.
		if (posDif.Length() >= SAWSHOT_PROJECTILE_REACH)
		{

			ReturnToSender();
			return;
		}
	}
	else
	{
		// No owner? Let's dip!
		UTIL_Remove(this);
	}


	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CSawProjectile::ReturnToSender()
{
	// Think once here since to get our angles and velocity right
	RetractThink();


	// Retract
	SetTouch(&CSawProjectile::RetractTouch);
	SetThink(&CSawProjectile::RetractThink);
	SetNextThink(gpGlobals->curtime + 0.05f);
}

void CSawProjectile::RetractTouch(CBaseEntity* pOther)
{
	// Ignore triggers
	if (pOther->IsSolidFlagSet(FSOLID_TRIGGER))
		return;


	// Apply damage to whatever we hit if we can and return to sender if we're not on our way home
	if (pOther->IsWorld())
	{

	}
	else
	{
		CTakeDamageInfo dmgInfo;
		dmgInfo.SetDamage(SAWSHOT_EXTEND_DAMAGE_PER_TOUCH);

		pOther->TakeDamage(dmgInfo);
	}

}

void CSawProjectile::RetractThink()
{
	// we have to update the velocity constantly for some reason. annoying... 

	CDMOPlayer* owner = ToDMOPlayer(GetOwnerEntity());
	if (owner && owner->IsAlive())
	{
		Vector posDif = owner->EyePosition() - GetAbsOrigin();
		float distToOwner = posDif.Length();

		if (distToOwner < SAWSHOT_PROJECTILE_DELETE_DIST)
		{
			// We're up close. Let's delete!
			UTIL_Remove(this);
			return;
		}

		Vector dirToOwner = (posDif).Normalized();

		float velRatio = fabs(distToOwner - SAWSHOT_PROJECTILE_REACH) / SAWSHOT_PROJECTILE_REACH;


		// This makes us use more and more of our target as we get closer
		//velRatio *= velRatio * velRatio;

		// ramp up to target slower
		velRatio = clamp(velRatio / 16, 0, 1);

		//DebugDrawLine(GetAbsOrigin(), GetAbsOrigin() + GetAbsVelocity(), 255 * velRatio, 0, 255 * (1 - velRatio), false, 1000);

		// While we get closer to the player, use more of the target velocity than the previous velocity
		SetAbsVelocity((dirToOwner * SAWSHOT_PROJECTILE_RETRACT_SPEED) * velRatio + GetAbsVelocity() * ( 1 - velRatio) );
		
		QAngle angles;
		VectorAngles(dirToOwner, angles);
		SetAbsAngles(angles);

		SetNextThink(gpGlobals->curtime + 0.05f);
	}
	else
	{
		// No owner? Let's dip!
		UTIL_Remove(this);
	}
}

LINK_ENTITY_TO_CLASS(projectile_saw, CSawProjectile);
#endif



// Sawshot Weapon


class CWeaponSawshot : public CWeaponDMOBase
{
public:
	DECLARE_CLASS(CWeaponSawshot, CWeaponDMOBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();


	CWeaponSawshot();

	virtual void Spawn();
	virtual void Precache();

	virtual void ItemPostFrame();

	virtual DMOWeaponID GetWeaponID() const { return DMO_WEAPON_SAWSHOT; }
	virtual void PrimaryAttack();
private:
	bool m_bWaitingForReturn;

};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponSawshot, DT_WeaponSawshot)

BEGIN_NETWORK_TABLE(CWeaponSawshot, DT_WeaponSawshot)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponSawshot)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_sawshot, CWeaponSawshot);
PRECACHE_WEAPON_REGISTER(weapon_sawshot);

acttable_t CWeaponSawshot::m_acttable[] =
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_PHYSGUN,						false },
	{ ACT_MP_RUN,						ACT_HL2MP_RUN_PHYSGUN,						false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_PHYSGUN,				false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_PHYSGUN,				false },
	//{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PHYSGUN,		false },
	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_PHYSGUN,			false },
	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_PHYSGUN,						false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PHYSGUN,		false },
};

IMPLEMENT_ACTTABLE(CWeaponSawshot);

CWeaponSawshot::CWeaponSawshot()
{
}

void CWeaponSawshot::Spawn()
{
	BaseClass::Spawn();

	m_bWaitingForReturn = false;
}

void CWeaponSawshot::Precache()
{
	BaseClass::Precache();
	
	PrecacheModel(SAWSHOT_BLADE);
#ifdef DMO_SERVER
	UTIL_PrecacheOther("projectile_saw");
#endif
}

void CWeaponSawshot::ItemPostFrame()
{
	CDMOPlayer* pOwner = ToDMOPlayer(GetOwner());
	if (!pOwner)
		return;

	//Track the duration of the fire
	//FIXME: What if we're calling ItemBusyFrame?
	m_fFireDuration = (pOwner->m_nButtons & IN_ATTACK) ? (m_fFireDuration + gpGlobals->frametime) : 0.0f;


	if ((pOwner->m_nButtons & IN_ATTACK) && (m_flNextPrimaryAttack <= gpGlobals->curtime))
	{
		
		PrimaryAttack();


#ifdef CLIENT_DLL
		pOwner->SetFiredWeapon(true);
#endif

	}
	else
	{
		WeaponIdle();
	}
}

void CWeaponSawshot::PrimaryAttack(void)
{
	CDMOPlayer* pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;


	QAngle angAimDir = pPlayer->EyeAngles();
	Vector vecStartPos = pPlayer->Weapon_ShootPosition();
	

#ifdef DMO_SERVER

	CSawProjectile::Create(pPlayer->Weapon_ShootPosition(), angAimDir, pPlayer);

#endif
	

	WeaponSound(SINGLE);

	pPlayer->SetAnimation(PLAYER_ATTACK1);
	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	float delayTime = gpGlobals->curtime + GetDMOWpnData().m_flCycleTime;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = delayTime;
	SetWeaponIdleTime(delayTime);
}

