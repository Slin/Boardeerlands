//
//  BDWater.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_WATER_H_
#define __Boardeerlands_WATER_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class Water : public RN::Entity
	{
	public:
		Water();
		~Water();

		void MoveTo(RN::Vector3 position, float time);
		
		void Update(float delta) override;

	private:
		float _interpolationFactor;
		RN::Vector3 _startPosition;
		RN::Vector3 _targetPosition;
		float _interpolationTime;
		
		RNDeclareMeta(Water)
	};
}

#endif /* defined(__Boardeerlands_WATER_H_) */
