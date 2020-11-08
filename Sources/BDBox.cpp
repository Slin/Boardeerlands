//
//  BDBox.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDBox.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(Box, RN::Entity)
	
	Box::Box() : _isFirstFrame(true)
	{
		RN::Dictionary *loadOptions = new RN::Dictionary();
		loadOptions->SetObjectForKey(RN::Number::WithBool(true), RNCSTR("enableDirectionalLights"));
		loadOptions->SetObjectForKey(RN::Number::WithBool(true), RNCSTR("enableDirectionalShadows"));
		loadOptions->Autorelease();
		SetModel(RN::Model::WithName(RNCSTR("models/box.sgm"), loadOptions));
		
		RN::PhysXMaterial *physicsMaterial = new RN::PhysXMaterial();
		_physicsBody = new RN::PhysXDynamicBody(RN::PhysXBoxShape::WithHalfExtents(RN::Vector3(0.35f, 0.35f, 0.35f), physicsMaterial->Autorelease()), 10.0f);
		_physicsBody->SetCollisionFilter(Types::CollisionType::CollisionObject, Types::CollisionType::CollisionAll);
		AddAttachment(_physicsBody->Autorelease());

		_physicsBody->SetEnableSleeping(false);
	}
	
	Box::~Box()
	{

	}
	
	void Box::Update(float delta)
	{
		RN::SceneNode::Update(delta);

		if(_isFirstFrame)
		{
			_isFirstFrame = false;
			_spawnPosition = GetWorldPosition();
			_spawnRotation = GetWorldRotation();
		}

		if(GetWorldPosition().y < 0.0f)
		{
			SetWorldPosition(_spawnPosition);
			SetWorldRotation(_spawnRotation);
		}
	}
}
