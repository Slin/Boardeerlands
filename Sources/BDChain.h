//
//  BDChain.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_CHAIN_H_
#define __Boardeerlands_CHAIN_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class Chain : public RN::SceneNode
	{
	public:
		Chain(RN::Vector3 position, RN::Vector3 targetPosition);
		~Chain();
		
		void Update(float delta) override;

	private:
		RN::Entity *_stoepselEntity;
		RN::Array *_chainElements;
		RN::Array *_constraints;

		RN::Vector3 _initialPosition;
		
		RNDeclareMeta(Chain)
	};
}

#endif /* defined(__Boardeerlands_CHAIN_H_) */
