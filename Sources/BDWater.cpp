//
//  BDWater.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDWater.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(Water, RN::Entity)
	
	Water::Water() : _interpolationTime(0.0f)
	{
		SetModel(World::GetSharedInstance()->AssignShader(RN::Model::WithName(RNCSTR("models/water.sgm")), Types::MaterialWater));
		AddFlags(RN::Entity::Flags::DrawLater);
	}
	
	Water::~Water()
	{

	}

	void Water::MoveTo(RN::Vector3 position, float time)
	{
		_startPosition = GetWorldPosition();
		_targetPosition = position;
		_interpolationFactor = 0.0f;
		_interpolationTime = time;
	}

	
	void Water::Update(float delta)
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
