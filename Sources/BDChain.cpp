//
//  BDChain.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#include "BDChain.h"
#include "BDWorld.h"

namespace BD
{
	RNDefineMeta(Chain, RN::SceneNode)
	
	Chain::Chain(RN::Vector3 position)
	{
		_chainElements = new RN::Array();
		_constraints = new RN::Array();

		SetWorldPosition(position);
		
		RN::Dictionary *loadOptions = new RN::Dictionary();
		loadOptions->SetObjectForKey(RN::Number::WithBool(true), RNCSTR("enableDirectionalLights"));
		loadOptions->SetObjectForKey(RN::Number::WithBool(true), RNCSTR("enableDirectionalShadows"));
		loadOptions->Autorelease();
		RN::Model *chainModel = RN::Model::WithName(RNCSTR("models/kettenglied/kettenglied.sgm"), loadOptions);

		RN::Model *stoepselModel = RN::Model::WithName(RNCSTR("models/stoepsel/stoepsel.sgm"), loadOptions);

		RN::PhysXMaterial *physicsMaterial = new RN::PhysXMaterial();

		RN::Entity *stoepselEntity = new RN::Entity(stoepselModel);
		World::GetSharedInstance()->AddLevelNode(stoepselEntity->Autorelease());
		stoepselEntity->SetWorldPosition(GetWorldPosition());
		_chainElements->AddObject(stoepselEntity);

		RN::PhysXConvexHullShape *stoepselPhysicsShape = RN::PhysXConvexHullShape::WithMesh(stoepselModel->GetLODStage(0)->GetMeshAtIndex(0), physicsMaterial->Autorelease());
		RN::PhysXDynamicBody *stoepselPhysicsBody = new RN::PhysXDynamicBody(stoepselPhysicsShape, 0.5f);
		stoepselPhysicsBody->SetRotationOffset(RN::Vector3(0.0f, 0.0f, 0.0f));
		stoepselPhysicsBody->SetCollisionFilter(Types::CollisionType::CollisionObject, Types::CollisionType::CollisionAll);
		stoepselEntity->AddAttachment(stoepselPhysicsBody->Autorelease());

		RN::PhysXDynamicBody *_previousPhysicsBody = stoepselPhysicsBody;
		for(int i = 0; i < 10; i++)
		{
			RN::Entity *chainElementEntity = new RN::Entity(chainModel);
			World::GetSharedInstance()->AddLevelNode(chainElementEntity->Autorelease());
			chainElementEntity->SetWorldPosition(GetWorldPosition() + RN::Vector3(0.0f, 0.23f*i, 0.0f));
			chainElementEntity->SetWorldRotation(RN::Vector3(i*90.0f, 0.0f, -90.0f));
			_chainElements->AddObject(chainElementEntity);

			RN::PhysXBoxShape *physicsShape = RN::PhysXBoxShape::WithHalfExtents(RN::Vector3(0.15f, 0.03f, 0.08f), physicsMaterial->Autorelease());
			RN::PhysXDynamicBody *physicsBody = new RN::PhysXDynamicBody(physicsShape, 0.2f);
			physicsBody->SetRotationOffset(RN::Vector3(90.0f, 90.0f, 90.0f));
			physicsBody->SetCollisionFilter(Types::CollisionType::CollisionObject, Types::CollisionType::CollisionAll);
			chainElementEntity->AddAttachment(physicsBody->Autorelease());

			RN::PhysXD6Constraint *constraint = RN::PhysXD6Constraint::WithBodiesAndOffsets(_previousPhysicsBody, i == 0 ? RN::Vector3(0.0f, 0.1f, 0.0f) : RN::Vector3(-0.115f, 0.0f, 0.0f), RN::Quaternion(), physicsBody, RN::Vector3(0.115f, 0.0f, 0.0f), RN::Quaternion(RN::Vector3(0.0f, 90.0f, 0.0f)));
			constraint->SetMotion(RN::PhysXD6Constraint::MotionAxis::motionAngularX, RN::PhysXD6Constraint::MotionType::Limited);
			constraint->SetMotion(RN::PhysXD6Constraint::MotionAxis::motionAngularY, RN::PhysXD6Constraint::MotionType::Free);
			constraint->SetMotion(RN::PhysXD6Constraint::MotionAxis::motionAngularZ, RN::PhysXD6Constraint::MotionType::Free);
			constraint->SetAngularLimit(RN::Vector3(-M_PI_4, -M_PI_2, -M_PI_2), RN::Vector3(M_PI_4, M_PI_2, M_PI_2), 800.0f, 100.0f);
			_constraints->AddObject(constraint);

			_previousPhysicsBody = physicsBody;
		}
	}
	
	Chain::~Chain()
	{
		SafeRelease(_constraints);
		SafeRelease(_chainElements);
	}
	
	void Chain::Update(float delta)
	{
		RN::SceneNode::Update(delta);
	}
}
