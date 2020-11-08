//
//  BDExitDoor.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_EXITDOOR_H_
#define __Boardeerlands_EXITDOOR_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class ExitDoor : public RN::Entity
	{
	public:
		ExitDoor();
		~ExitDoor();

		void MoveTo(RN::Vector3 position, float time);
		
		void Update(float delta) override;

	private:
		float _interpolationFactor;
		RN::Vector3 _startPosition;
		RN::Vector3 _targetPosition;
		float _interpolationTime;
		
		RNDeclareMeta(ExitDoor)
	};
}

#endif /* defined(__Boardeerlands_EXITDOOR_H_) */
