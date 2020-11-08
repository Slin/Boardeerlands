//
//  BDBucketTrigger.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDBucketTrigger.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(BucketTrigger, RN::SceneNode)
	
	BucketTrigger::BucketTrigger()
	{
		_buckets = new RN::Array();
		
		_boundingBox.minExtend.x = -0.3f;
		_boundingBox.minExtend.y = 0.0f;
		_boundingBox.minExtend.z = -0.3f;

		_boundingBox.maxExtend.x = 0.3f;
		_boundingBox.maxExtend.y = 1.0f;
		_boundingBox.maxExtend.z = 0.3f;
	}
	
	BucketTrigger::~BucketTrigger()
	{
		SafeRelease(_buckets);
	}

	void BucketTrigger::RegisterBucket(Bucket *bucket)
	{
		_buckets->AddObject(bucket);
	}
	
	void BucketTrigger::Update(float delta)
	{
		RN::SceneNode::Update(delta);

		_buckets->Enumerate<Bucket>([&](Bucket *bucket, size_t index, bool &stop){
			RN::Vector3 localBoxPosition = bucket->GetWorldPosition() - GetWorldPosition();
			if(_boundingBox.Contains(localBoxPosition) && bucket->GetFillAmount() == 4)
			{
				World::GetSharedInstance()->UnlockLevelSection(4);
			}
		});
	}
}
