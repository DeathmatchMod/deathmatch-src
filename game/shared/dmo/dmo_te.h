#ifndef DMO_TE_H
#define DMO_TE_H

#include "baseplayer_shared.h"

void TE_LaserSniperBeam(IRecipientFilter& filter,
	CBasePlayer* hOwnerPlayer, int iAttachmentIndex, const Vector end,
	float life, float width);

#endif