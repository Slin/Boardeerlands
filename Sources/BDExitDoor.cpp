//
//  BDExitDoor.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDExitDoor.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(ExitDoor, RN::Entity)
	
	ExitDoor::ExitDoor() : _interpolationTime(0.0f)
	{
		SetModel(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/stage/door.sgm")), Types::MaterialDefault));

		RN::PhysXMaterial *physicsMaterial = new RN::PhysXMaterial();
		RN::PhysXCompoundShape *shape = RN::PhysXCompoundShape::WithModel(GetModel(), physicsMaterial->Autorelease(), true, false);
		RN::PhysXStaticBody *body = RN::PhysXStaticBody::WithShape(shape);
		body->SetCollisionFilter(Types::CollisionLevel, Types::CollisionAll);
		AddAttachment(body);
	}
	
	ExitDoor::~ExitDoor()
	{

	}

	void ExitDoor::MoveTo(RN::Vector3 position, float time)
	{
		_startPosition = GetWorldPosition();
		_targetPosition = position;
		_interpolationFactor = 0.0f;
		_interpolationTime = time;
	}

	
	void ExitDoor::Update(float delta)
	{
		RN::SceneNode::Update(delta);

		if(_interpolationTime > 0.0f)
		{
			_interpolationFactor += delta / _interpolationTime;
			if(_interpolationFactor >= 1.0f)
			{
				_interpolationTime = 0.0f;
				_interpolationFactor = 1.0f;
			}

			SetWorldPosition(_startPosition.GetLerp(_targetPosition, _interpolationFactor));
		}
	}
}
