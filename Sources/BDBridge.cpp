//
//  BDBridge.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDBridge.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(Bridge, RN::Entity)
	
	Bridge::Bridge() : _interpolationTime(0.0f)
	{
		SetModel(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/stage/bridge.sgm")), Types::MaterialDefault));

		RN::PhysXMaterial *physicsMaterial = new RN::PhysXMaterial();
		RN::PhysXCompoundShape *shape = RN::PhysXCompoundShape::WithModel(GetModel(), physicsMaterial->Autorelease(), true, false);
		RN::PhysXStaticBody *body = RN::PhysXStaticBody::WithShape(shape);
		body->SetCollisionFilter(Types::CollisionLevel, Types::CollisionAll);
		AddAttachment(body);
	}
	
	Bridge::~Bridge()
	{

	}

	void Bridge::MoveTo(RN::Vector3 position, float time)
	{
		_startPosition = GetWorldPosition();
		_targetPosition = position;
		_interpolationFactor = 0.0f;
		_interpolationTime = time;

		RN::OpenALSource *source = new RN::OpenALSource(RN::AudioAsset::WithName(RNCSTR("audio/bridge moves.ogg")));
		source->SetSelfdestruct(true);
		AddChild(source->Autorelease());
		source->SetWorldPosition(RN::Vector3(0.0f, 0.0f, -7.0f));
		source->Play();
	}

	
	void Bridge::Update(float delta)
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
