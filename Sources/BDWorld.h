//
//  BDWorld.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//

#ifndef __BD_WORLD_H_
#define __BD_WORLD_H_

#include <Rayne.h>

#include "RNPhysXWorld.h"

#include "BDTypes.h"
#include "BDCameraManager.h"

#include "BDPlayer.h"
#include "BDBall.h"
#include "BDBox.h"
#include "BDBoxTrigger.h"
#include "BDWater.h"
#include "BDBridge.h"
#include "BDChain.h"
#include "BDBucket.h"
#include "BDBucketTrigger.h"
#include "BDExitDoor.h"
#include "BDPlayerTrigger.h"

namespace BD
{
	class World : public RN::SceneBasic
	{
	public:
		static World *GetSharedInstance();
		static void Exit();

		World(RN::VRWindow *vrWindow);
		~World();

		RN::PhysXWorld *GetPhysicsWorld() const { return _physicsWorld; }
		RN::ShaderLibrary *GetShaderLibrary() const { return _shaderLibrary; }
		RN::VRCamera *GetVRCamera() const { return _cameraManager.GetVRCamera(); }
		RN::Camera *GetHeadCamera() const { return _cameraManager.GetHeadCamera(); }
		RN::Camera *GetPreviewCamera() const { return _cameraManager.GetPreviewCamera(); }
		CameraManager &GetCameraManager() { return _cameraManager; }
		Player *GetPlayer() const { return _player; }
		int GetCurrentLevelSection() const { return _currentLevelSection; }

		RN::Model *AssignShader(RN::Model *model, Types::MaterialType materialType) const;
		RN::Model *MakeDeepCopy(RN::Model *model) const;
		
		void AddLevelNode(RN::SceneNode *node);
		void RemoveLevelNode(RN::SceneNode *node);
		void RemoveAllLevelNodes();
		
		bool GetIsDash() const { return _isDash; }
		
		void LoadLevel();
		void UnlockLevelSection(int section);

	protected:
		void WillBecomeActive() override;
		void DidBecomeActive() override;
		void WillUpdate(float delta) override;

		RN::Entity *World::CreateLevelEntity(const RN::String *fileName);

		CameraManager _cameraManager;
		
		RN::Array *_levelNodes;

		RN::ShaderLibrary *_shaderLibrary;

		RN::VRWindow *_vrWindow;

		RN::PhysXWorld *_physicsWorld;

		Player *_player;

		RN::Entity *_levelPart[7];
		BoxTrigger *_boxTriggersSection2[4];
		BucketTrigger *_bucketTrigger;
		PlayerTrigger *_playerTrigger;
		
		bool _isPaused;
		bool _isDash;

		bool _isOPressed;

		int _currentLevelSection;

		static World *_sharedInstance;
	};
}


#endif /* __BD_WORLD_H_ */
