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
	
	Bucket::Bucket(bool isBig) : _isBig(isBig)
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
	}
}
