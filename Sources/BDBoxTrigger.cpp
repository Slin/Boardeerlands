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
		
		_boundingBox.minExtend.x = -0.7f;
		_boundingBox.minExtend.y = 0.0f;
		_boundingBox.minExtend.z = -0.7f;

		_boundingBox.maxExtend.x = 0.7f;
		_boundingBox.maxExtend.y = 1.0f;
		_boundingBox.maxExtend.z = 0.7f;
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

		bool wasActive = _isActive;
		_isActive = false;
		_boxes->Enumerate<Box>([&](Box *box, size_t index, bool &stop){
			RN::Vector3 localBoxPosition = box->GetWorldPosition() - GetWorldPosition();
			if(_boundingBox.Contains(localBoxPosition))
			{
				_isActive = true;
			}
		});

		if(_isActive && !wasActive)
		{
			RN::OpenALSource *source = new RN::OpenALSource(RN::AudioAsset::WithName(RNCSTR("audio/button.ogg")));
			source->SetSelfdestruct(true);
			AddChild(source->Autorelease());
			source->Play();
		}
	}
}
