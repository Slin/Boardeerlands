//
//  BDPlayer.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_PLAYER_H_
#define __Boardeerlands_PLAYER_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class Player : public RN::SceneNode
	{
	public:
		Player(RN::SceneNode *camera);
		~Player();
		
		void Update(float delta) override;
		
		bool IsActivatePressed();
		void DidActivate();

	private:
		RN::SceneNode *_camera;
		RN::PhysXKinematicController *_controller;

		RN::PhysXDynamicBody *_physicsBody;
                RN::SceneNode *_physicsBodyNode;

		RN::Vector3 _previousHeadPosition;

		RN::Quaternion _cameraRotation;
		bool _stickIsPressed;
		bool _didSnapTurn;

		bool _didActivate;
		bool _isActivating;
		
		RNDeclareMeta(Player)
	};
}

#endif /* defined(__Boardeerlands_PLAYER_H_) */
