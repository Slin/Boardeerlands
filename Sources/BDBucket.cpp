//
//  BDBucket.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDBucket.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(Bucket, RN::Entity)
	
	Bucket::Bucket(bool isBig) : _isBig(isBig), _fillAmount(0), _otherBucket(nullptr)
	{
		RN::Dictionary *loadOptions = new RN::Dictionary();
		loadOptions->SetObjectForKey(RN::Number::WithBool(true), RNCSTR("enableDirectionalLights"));
		loadOptions->SetObjectForKey(RN::Number::WithBool(true), RNCSTR("enableDirectionalShadows"));
		loadOptions->Autorelease();
		SetModel(RN::Model::WithName(isBig?RNCSTR("models/bucket_big/bucket_big.sgm"):RNCSTR("models/bucket_small/bucket_small.sgm"), loadOptions));
		
		RN::PhysXMaterial *physicsMaterial = new RN::PhysXMaterial();
		_physicsBody = new RN::PhysXDynamicBody(RN::PhysXConvexHullShape::WithMesh(GetModel()->GetLODStage(0)->GetMeshAtIndex(0), physicsMaterial->Autorelease()), 2.0f);
		_physicsBody->SetCollisionFilter(Types::CollisionType::CollisionObject, Types::CollisionType::CollisionAll);
		AddAttachment(_physicsBody->Autorelease());

		_physicsBody->SetEnableSleeping(false);
	}
	
	Bucket::~Bucket()
	{

	}
	
	void Bucket::Update(float delta)
	{
		RN::SceneNode::Update(delta);

		if(GetWorldPosition().y < -2.0f)
		{
			_fillAmount = _isBig?5:3;

			RNDebug("Fill amount: " << _fillAmount);
		}

		if(GetUp().y < 0.0f)
		{
			if(_otherBucket && _otherBucket->GetWorldPosition().y < GetWorldPosition().y && _otherBucket->GetUp().y > 0.0f && RN::Vector3(_otherBucket->GetWorldPosition().x - GetWorldPosition().x, 0.0f, _otherBucket->GetWorldPosition().z - GetWorldPosition().z).GetLength() < 0.5f)
			{
				int fillDiff = std::min(_fillAmount - _otherBucket->_fillAmount, (_isBig?3:5) - _otherBucket->_fillAmount);
				//RNDebug("Removing " << fillDiff << " from " << (_isBig ? "big" : "small") << " to get " << );
				_otherBucket->_fillAmount += fillDiff;
				_fillAmount -= fillDiff;
			}
			else
			{
				_fillAmount = 0.0f;
				RNDebug("Empty bucket");
			}
		}
	}
}
