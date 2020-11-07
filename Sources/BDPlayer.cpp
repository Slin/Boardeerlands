//
//  BDPlayer.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDPlayer.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(Player, RN::SceneNode)
	
	Player::Player(RN::SceneNode *camera) :
		_camera(camera->Retain()), 
		_stickIsPressed(false), 
		_didSnapTurn(false), 
		_isActivating(false), 
		_didActivate(false)
	{
		AddChild(camera);

		RN::PhysXMaterial *physicsMaterial = new RN::PhysXMaterial();
		_controller = new RN::PhysXKinematicController(0.2f, 1.1f, physicsMaterial->Autorelease(), 0.0f);
		_controller->SetPositionOffset(_controller->GetFeetOffset());
		_controller->SetCollisionFilter(Types::CollisionType::CollisionPlayerController, Types::CollisionType::CollisionAll & ~Types::CollisionType::CollisionPlayerBody);
		AddAttachment(_controller);

		RN::PhysXCapsuleShape *bodyShape = RN::PhysXCapsuleShape::WithRadius(0.3f, 1.1f, physicsMaterial);
		_physicsBody = new RN::PhysXDynamicBody(bodyShape, 100.0f);
		_physicsBody->SetPositionOffset(_controller->GetFeetOffset());
		_physicsBody->SetRotationOffset(RN::Vector3(0.0f, 0.0f, 90.0f));
		_physicsBody->SetCollisionFilter(Types::CollisionType::CollisionPlayerBody, Types::CollisionType::CollisionObject);

		_physicsBodyNode = new RN::SceneNode();
                _physicsBodyNode->AddAttachment(_physicsBody->Autorelease());
		World::GetSharedInstance()->AddNode(_physicsBodyNode);

		RN::VRCamera *vrCamera = _camera->Downcast<RN::VRCamera>();
		if(vrCamera)
		{
			_previousHeadPosition = vrCamera->GetHead()->GetPosition();
			if(vrCamera->GetOrigin() == RN::VRWindow::Origin::Head)
				_camera->SetPosition(RN::Vector3(0.0f, 1.6f, 0.0f));
			else
				_camera->SetPosition(RN::Vector3(0.0f, 0.0f, 0.0f));
		}
		else
		{
			_camera->SetPosition(RN::Vector3(0.0f, 1.6f, 0.0f));
		}
	}
	
	Player::~Player()
	{
		SafeRelease(_camera);
		SafeRelease(_physicsBodyNode);
	}
	
	void Player::Update(float delta)
	{
		RN::SceneNode::Update(delta);

		if(delta == 0.0f) return;

		RN::VRCamera *vrCamera = _camera->Downcast<RN::VRCamera>();
		RN::InputManager *manager = RN::InputManager::GetSharedInstance();

		RN::Vector3 rotation(0.0);

		if(!vrCamera)
		{
			rotation.x = manager->GetMouseDelta().x;
			rotation.y = manager->GetMouseDelta().y;
			rotation = -rotation;
		}

		bool active = (int)manager->IsControlToggling(RNCSTR("E")) || manager->IsControlToggling(RNCSTR("Button 1"));
		if(vrCamera)
		{
			for(int i = 0; i < 2; i++)
			{
				RN::VRControllerTrackingState controllerState = vrCamera->GetControllerTrackingState(i);
				if(controllerState.active && controllerState.indexTrigger > 0.5f)
				{
					active = true;
				}
			}
		}

		if(!active) _didActivate = false;
		if(!_didActivate) _isActivating = active;
		else _isActivating = false;
		
		RN::Vector3 stickTranslation;

		if(vrCamera)
		{
			RN::VRControllerTrackingState leftController = vrCamera->GetControllerTrackingState(0);
			RN::VRControllerTrackingState rightController = vrCamera->GetControllerTrackingState(1);

			if(std::abs(rightController.thumbstick.x) > 0.25f || (rightController.button[RN::VRControllerTrackingState::Button::Pad] && std::abs(rightController.trackpad.x) > 0.25f))
			{
				if(!_didSnapTurn)
				{
					if(rightController.thumbstick.x > 0.25f || (rightController.button[RN::VRControllerTrackingState::Button::Pad] && rightController.trackpad.x > 0.25f))
					{
						_camera->SetRotation(_cameraRotation - RN::Vector3(45.0f, 0.0f, 0.0f));
						_didSnapTurn = true;
					}
					else if(rightController.thumbstick.x < -0.25f || (rightController.button[RN::VRControllerTrackingState::Button::Pad] && rightController.trackpad.x < -0.25f))
					{
						_camera->SetRotation(_cameraRotation + RN::Vector3(45.0f, 0.0f, 0.0f));
						_didSnapTurn = true;
					}
				}
			}
			else
			{
				_didSnapTurn = false;
			}
			
			RN::Vector3 controllerRotation = leftController.rotation.GetEulerAngle();
			controllerRotation.y = 0.0f;
			stickTranslation += (_cameraRotation * RN::Quaternion(controllerRotation)).GetRotatedVector(RN::Vector3(leftController.thumbstick.x, 0.0f, -leftController.thumbstick.y));
			stickTranslation.y = 0.0f;
			stickTranslation.Normalize(1.5f * delta);
		}
		
		_camera->Rotate(rotation * delta * 15.0f);
		if(_camera->GetEulerAngle().y > 85.0f)
		{
			_camera->SetWorldRotation(RN::Vector3(_camera->GetEulerAngle().x, 85.0f, _camera->GetEulerAngle().z));
		}
		if(_camera->GetEulerAngle().y < -85.0f)
		{
			_camera->SetWorldRotation(RN::Vector3(_camera->GetEulerAngle().x, -85.0f, _camera->GetEulerAngle().z));
		}
		_cameraRotation = _camera->GetRotation();
		

		RN::Vector3 globalTranslation;
		if(vrCamera)
		{
			RN::Vector3 localMovement = vrCamera->GetHead()->GetPosition() - _previousHeadPosition;
			_previousHeadPosition = vrCamera->GetHead()->GetPosition();
			localMovement.y = 0.0f;

			globalTranslation += _camera->GetWorldRotation().GetRotatedVector(localMovement) + stickTranslation;
		}
		else
		{
			RN::Vector3 translation{};
			if(manager->IsControlToggling(RNSTR("W")))
				translation -= RN::Vector3(0.f,0.f, 1.0f);
			if(manager->IsControlToggling(RNSTR("S")))
				translation += RN::Vector3(0.f, 0.f, 1.0f);
			if(manager->IsControlToggling(RNSTR("A")))
				translation -= RN::Vector3(1.0f, 0.f, 0.f);
			if(manager->IsControlToggling(RNSTR("D")))
				translation += RN::Vector3(1.0f, 0.f, 0.f);
			
			RN::Vector3 cameraRotation = _camera->GetWorldRotation().GetEulerAngle();
			cameraRotation.y = 0.0f;
			translation = RN::Quaternion(cameraRotation).GetRotatedVector(translation);
			translation.y = 0.0f;
			translation.Normalize(2.5f * delta);

			globalTranslation = translation;
		}
		
		_controller->Move(globalTranslation, delta);
		_controller->Move(RN::Vector3(0.0f, -2.0f * delta, 0.0f), delta);
		
		if(vrCamera)
		{
			RN::Vector3 localMovement = vrCamera->GetWorldPosition() - vrCamera->GetHead()->GetWorldPosition();

			if(vrCamera->GetOrigin() == RN::VRWindow::Origin::Head)
				localMovement.y = 1.8f;
			else
				localMovement.y = 0.0f;

			vrCamera->SetWorldPosition(localMovement + GetWorldPosition());
		}
		else
		{
			_camera->SetPosition(RN::Vector3(0.0f, 1.8f, 0.0f));
		}

		RN::Vector3 movementVelocity = GetWorldPosition() - _physicsBody->GetWorldPosition();
		movementVelocity /= delta;
		_physicsBody->SetEnableSleeping(false);
		_physicsBody->SetLinearVelocity(movementVelocity);
	}
	
	bool Player::IsActivatePressed()
	{
		return _isActivating;
	}

	void Player::DidActivate()
	{
		_didActivate = true;
	}
}
