//
//  BDBucketTrigger.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_BUCKETTRIGGER_H_
#define __Boardeerlands_BUCKETTRIGGER_H_

#include <Rayne.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class Bucket;
	class BucketTrigger : public RN::SceneNode
	{
	public:
		BucketTrigger();
		~BucketTrigger();

		void RegisterBucket(Bucket *bucket);

		bool GetIsActive() const { return _isActive; }
		
		void Update(float delta) override;

	private:
		RN::AABB _boundingBox;
		RN::Array *_buckets;

		bool _isActive;

		RNDeclareMeta(BucketTrigger)
	};
}

#endif /* defined(__Boardeerlands_BUCKETTRIGGER_H_) */
