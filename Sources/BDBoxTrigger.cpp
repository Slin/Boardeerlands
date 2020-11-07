//
//  BDBoxTrigger.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDBoxTrigger.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(BoxTrigger, RN::SceneNode)
	
	BoxTrigger::BoxTrigger() : _isActive(false)
	{
		_boxes = new RN::Array();
		
		_boundingBox.minExtend.x = -0.5f;
		_boundingBox.minExtend.y = 0.0f;
		_boundingBox.minExtend.z = -0.5f;

		_boundingBox.maxExtend.x = 0.5f;
		_boundingBox.maxExtend.y = 1.0f;
		_boundingBox.maxExtend.z = 0.5f;
	}
	
	BoxTrigger::~BoxTrigger()
	{
		SafeRelease(_boxes);
	}

	void BoxTrigger::RegisterBox(Box *box)
	{
		_boxes->AddObject(box);
	}
	
	void BoxTrigger::Update(float delta)
	{
		RN::SceneNode::Update(delta);

		_isActive = false;
		_boxes->Enumerate<Box>([&](Box *box, size_t index, bool &stop){
			RN::Vector3 localBoxPosition = box->GetWorldPosition() - GetWorldPosition();
			if(_boundingBox.Contains(localBoxPosition))
			{
				_isActive = true;
			}
		});
	}
}
