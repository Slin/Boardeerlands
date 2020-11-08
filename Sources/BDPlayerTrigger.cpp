//
//  BDPlayerTrigger.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDPlayerTrigger.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(PlayerTrigger, RN::SceneNode)
	
	PlayerTrigger::PlayerTrigger()
	{
		_boundingBox.minExtend.x = -0.5f;
		_boundingBox.minExtend.y = -0.1f;
		_boundingBox.minExtend.z = -0.5f;

		_boundingBox.maxExtend.x = 0.5f;
		_boundingBox.maxExtend.y = 2.0f;
		_boundingBox.maxExtend.z = 0.5f;
	}
	
	PlayerTrigger::~PlayerTrigger()
	{
		
	}
	
	void PlayerTrigger::Update(float delta)
	{
		RN::SceneNode::Update(delta);

		RN::Vector3 localPlayerPosition = World::GetSharedInstance()->GetPlayer()->GetWorldPosition() - GetWorldPosition();
		if(_boundingBox.Contains(localPlayerPosition))
		{
			World::GetSharedInstance()->UnlockLevelSection(6);
		}
	}
}
