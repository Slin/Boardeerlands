//
//  BDBox.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_BOX_H_
#define __Boardeerlands_BOX_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class Box : public RN::Entity
	{
	public:
		Box();
		~Box();
		
		void Update(float delta) override;

	private:
		RN::PhysXDynamicBody *_physicsBody;
		RN::Vector3 _spawnPosition;
		RN::Quaternion _spawnRotation;
		bool _isFirstFrame;
		
		RNDeclareMeta(Box)
	};
}

#endif /* defined(__Boardeerlands_BOX_H_) */
