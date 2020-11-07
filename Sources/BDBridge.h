//
//  BDBridge.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_BRIDGE_H_
#define __Boardeerlands_BRIDGE_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class Bridge : public RN::Entity
	{
	public:
		Bridge();
		~Bridge();

		void MoveTo(RN::Vector3 position, float time);
		
		void Update(float delta) override;

	private:
		float _interpolationFactor;
		RN::Vector3 _startPosition;
		RN::Vector3 _targetPosition;
		float _interpolationTime;
		
		RNDeclareMeta(Bridge)
	};
}

#endif /* defined(__Boardeerlands_BRIDGE_H_) */
