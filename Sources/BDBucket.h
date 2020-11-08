//
//  BDBucket.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_BUCKET_H_
#define __Boardeerlands_BUCKET_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class Bucket : public RN::Entity
	{
	public:
		Bucket(bool isBig);
		~Bucket();
		
		void Update(float delta) override;
		void SetOtherBucket(Bucket *bucket) { _otherBucket = bucket; }

		int GetFillAmount() const { return _fillAmount; }

	private:
		RN::Entity *_waterEntity;
		RN::PhysXDynamicBody *_physicsBody;
		bool _isBig;

		int _fillAmount;
		Bucket *_otherBucket;
		
		RNDeclareMeta(Bucket)
	};
}

#endif /* defined(__Boardeerlands_BUCKET_H_) */
