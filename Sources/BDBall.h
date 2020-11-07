//
//  BDBall.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_BALL_H_
#define __Boardeerlands_BALL_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class Ball : public RN::Entity
	{
	public:
		Ball();
		~Ball();
		
		void Update(float delta) override;

	private:
		RN::PhysXDynamicBody *_physicsBody;
		
		RNDeclareMeta(Ball)
	};
}

#endif /* defined(__Boardeerlands_BALL_H_) */
