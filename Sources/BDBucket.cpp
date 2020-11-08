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

		if(isBig)
		{
			_waterEntity[0] = new RN::Entity(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/bucket_big/bucket_big_water_1.sgm")), Types::MaterialWater));
			AddChild(_waterEntity[0]->Autorelease());

			_waterEntity[1] = new RN::Entity(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/bucket_big/bucket_big_water_2.sgm")), Types::MaterialWater));
			AddChild(_waterEntity[1]->Autorelease());

			_waterEntity[2] = new RN::Entity(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/bucket_big/bucket_big_water_3.sgm")), Types::MaterialWater));
			AddChild(_waterEntity[2]->Autorelease());

			_waterEntity[3] = new RN::Entity(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/bucket_big/bucket_big_water_4.sgm")), Types::MaterialWater));
			AddChild(_waterEntity[3]->Autorelease());

			_waterEntity[4] = new RN::Entity(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/bucket_big/bucket_big_water_5.sgm")), Types::MaterialWater));
			AddChild(_waterEntity[4]->Autorelease());
		}
		else
		{
			_waterEntity[0] = new RN::Entity(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/bucket_small/bucket_small_water_1.sgm")), Types::MaterialWater));
			AddChild(_waterEntity[0]->Autorelease());

			_waterEntity[1] = new RN::Entity(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/bucket_small/bucket_small_water_2.sgm")), Types::MaterialWater));
			AddChild(_waterEntity[1]->Autorelease());

			_waterEntity[2] = new RN::Entity(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/bucket_small/bucket_small_water_3.sgm")), Types::MaterialWater));
			AddChild(_waterEntity[2]->Autorelease());

			_waterEntity[3] = nullptr;
			_waterEntity[4] = nullptr;
		}
		
		
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

		if(GetWorldPosition().y < -1.5f)
		{
			_fillAmount = _isBig?5:3;
		}

		if(GetUp().y < 0.0f)
		{
			if(_otherBucket && _otherBucket->GetWorldPosition().y < GetWorldPosition().y && _otherBucket->GetUp().y > 0.0f && RN::Vector3(_otherBucket->GetWorldPosition().x - GetWorldPosition().x, 0.0f, _otherBucket->GetWorldPosition().z - GetWorldPosition().z).GetLength() < 0.5f)
			{
				int otherCanTake = (_isBig ? 3 : 5) - _otherBucket->_fillAmount;
				int fillDiff = std::min(otherCanTake, _fillAmount);
				_otherBucket->_fillAmount += fillDiff;
				_fillAmount -= fillDiff;
			}
			else
			{
				_fillAmount = 0.0f;
			}
		}

		for(int i = 0; i < 5; i++)
		{
			if(_waterEntity[i])
				_waterEntity[i]->AddFlags(RN::Entity::Flags::Hidden);
		}

		if(_fillAmount > 0)
		{
			_waterEntity[_fillAmount-1]->RemoveFlags(RN::Entity::Flags::Hidden);
		}
	}
}
