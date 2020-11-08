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
	
	Chain::Chain(RN::Vector3 position, RN::Vector3 targetPosition) : _initialPosition(position)
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

		_stoepselEntity = new RN::Entity(stoepselModel);
		World::GetSharedInstance()->AddLevelNode(_stoepselEntity->Autorelease());
		_stoepselEntity->SetWorldPosition(GetWorldPosition());
		_chainElements->AddObject(_stoepselEntity);

		RN::PhysXConvexHullShape *stoepselPhysicsShape = RN::PhysXConvexHullShape::WithMesh(stoepselModel->GetLODStage(0)->GetMeshAtIndex(0), physicsMaterial->Autorelease());
		RN::PhysXDynamicBody *stoepselPhysicsBody = new RN::PhysXDynamicBody(stoepselPhysicsShape, 1.0f);
		stoepselPhysicsBody->SetSolverIterationCount(1.0f, 1.0f);
		stoepselPhysicsBody->SetRotationOffset(RN::Vector3(0.0f, 0.0f, 0.0f));
		stoepselPhysicsBody->SetCollisionFilter(Types::CollisionType::CollisionObject, Types::CollisionType::CollisionAll);
		_stoepselEntity->AddAttachment(stoepselPhysicsBody->Autorelease());

		RN::PhysXDynamicBody *_previousPhysicsBody = stoepselPhysicsBody;
		RN::Vector3 positionOffset = targetPosition - position;
		int stepsToTarget = positionOffset.GetLength() / 0.18f;
		positionOffset.Normalize(0.18f);
		RN::Vector3 elementPosition;
		int i = 0;
		for(i = 0; i < 25; i++)
		{
			elementPosition = GetWorldPosition() + positionOffset * i;
			if(i > stepsToTarget)
			{
				positionOffset = RN::Vector3(0.0f, 0.0f, 0.15f);
				elementPosition = targetPosition + positionOffset * (i - stepsToTarget);
			}
			
			RN::Entity *chainElementEntity = new RN::Entity(chainModel);
			World::GetSharedInstance()->AddLevelNode(chainElementEntity->Autorelease());
			chainElementEntity->SetWorldPosition(elementPosition);
			chainElementEntity->SetWorldRotation(RN::Vector3(i*90.0f, (i > stepsToTarget)?-90.0f:0.0f, -90.0f));
			_chainElements->AddObject(chainElementEntity);

			RN::PhysXBoxShape *physicsShape = RN::PhysXBoxShape::WithHalfExtents(RN::Vector3(0.15f, 0.03f, 0.08f), physicsMaterial->Autorelease());
			RN::PhysXDynamicBody *physicsBody = new RN::PhysXDynamicBody(physicsShape, 0.3f);
			physicsBody->SetSolverIterationCount(1.0f, 1.0f);
			physicsBody->SetRotationOffset(RN::Vector3(90.0f, 90.0f, 90.0f));
			physicsBody->SetCollisionFilter(Types::CollisionType::CollisionChain, Types::CollisionLevel);
			chainElementEntity->AddAttachment(physicsBody->Autorelease());

			RN::PhysXD6Constraint *constraint = RN::PhysXD6Constraint::WithBodiesAndOffsets(_previousPhysicsBody, i == 0 ? RN::Vector3(0.0f, 0.1f, 0.0f) : RN::Vector3(-0.115f, 0.0f, 0.0f), RN::Quaternion(), physicsBody, RN::Vector3(0.115f, 0.0f, 0.0f), RN::Quaternion(RN::Vector3(0.0f, 90.0f, 0.0f)));
			constraint->SetMotion(RN::PhysXD6Constraint::MotionAxis::motionAngularX, RN::PhysXD6Constraint::MotionType::Locked);
			constraint->SetMotion(RN::PhysXD6Constraint::MotionAxis::motionAngularY, RN::PhysXD6Constraint::MotionType::Free);
			constraint->SetMotion(RN::PhysXD6Constraint::MotionAxis::motionAngularZ, RN::PhysXD6Constraint::MotionType::Free);
			constraint->SetAngularLimit(RN::Vector3(-M_PI_4, -M_PI_2, -M_PI_2), RN::Vector3(M_PI_4, M_PI_2, M_PI_2), 100.0f, 10.0f);
			_constraints->AddObject(constraint);
			
			_previousPhysicsBody = physicsBody;
		}

		RN::Entity *handleEntity = new RN::Entity(RN::Model::WithName(RNCSTR("models/kettenglied/kettenglied_ende.sgm"), loadOptions));
		World::GetSharedInstance()->AddLevelNode(handleEntity->Autorelease());
		handleEntity->SetWorldPosition(targetPosition + RN::Vector3(-0.1f, -0.4f, 0.0f));
		handleEntity->SetWorldRotation(RN::Vector3(i*90.0f, 0.0f, -90.0f));
		_chainElements->AddObject(handleEntity);

		RN::PhysXCompoundShape *handlePhysicsShape = RN::PhysXCompoundShape::WithModel(handleEntity->GetModel(), physicsMaterial->Autorelease(), false);
		RN::PhysXDynamicBody *handlePhysicsBody = new RN::PhysXDynamicBody(handlePhysicsShape, 0.51f);
		handlePhysicsBody->SetSolverIterationCount(1.0f, 1.0f);
		handlePhysicsBody->SetRotationOffset(RN::Vector3(90.0f, 90.0f, 90.0f));
		handlePhysicsBody->SetCollisionFilter(Types::CollisionType::CollisionObject, Types::CollisionType::CollisionAll);
		handleEntity->AddAttachment(handlePhysicsBody->Autorelease());

		RN::PhysXD6Constraint *constraint = RN::PhysXD6Constraint::WithBodiesAndOffsets(_previousPhysicsBody, i == 0 ? RN::Vector3(0.0f, 0.1f, 0.0f) : RN::Vector3(-0.115f, 0.0f, 0.0f), RN::Quaternion(), handlePhysicsBody, RN::Vector3(0.115f, 0.0f, 0.0f), RN::Quaternion(RN::Vector3(0.0f, 90.0f, 0.0f)));
		constraint->SetMotion(RN::PhysXD6Constraint::MotionAxis::motionAngularX, RN::PhysXD6Constraint::MotionType::Locked);
		constraint->SetMotion(RN::PhysXD6Constraint::MotionAxis::motionAngularY, RN::PhysXD6Constraint::MotionType::Free);
		constraint->SetMotion(RN::PhysXD6Constraint::MotionAxis::motionAngularZ, RN::PhysXD6Constraint::MotionType::Free);
		constraint->SetAngularLimit(RN::Vector3(-M_PI_4, -M_PI_2, -M_PI_2), RN::Vector3(M_PI_4, M_PI_2, M_PI_2), 100.0f, 10.0f);
		_constraints->AddObject(constraint);
	}
	
	Chain::~Chain()
	{
		SafeRelease(_constraints);
		SafeRelease(_chainElements);
	}
	
	void Chain::Update(float delta)
	{
		RN::SceneNode::Update(delta);

		if(_stoepselEntity->GetWorldPosition().GetDistance(_initialPosition) > 0.3f)
		{
			World::GetSharedInstance()->UnlockLevelSection(3);
		}
	}
}
