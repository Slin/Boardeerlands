//
//  BDTypes.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//

#ifndef __BD_TYPES_H_
#define __BD_TYPES_H_

#include "Rayne.h"

namespace BD
{
	namespace Types
	{
		enum CollisionType
		{
			CollisionLevel = 1 << 0,

			CollisionAll = 0xffffffff
		};
		
		enum MaterialType
		{
			MaterialDefault
		};
	}
}

#endif /* defined(__BD_TYPES_H_) */
