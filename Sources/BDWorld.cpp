//
//  BDWorld.cpp
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//

#include "BDWorld.h"

namespace BD
{
	World *World::_sharedInstance = nullptr;

	World *World::GetSharedInstance()
	{
		return _sharedInstance;
	}

	void World::Exit()
	{
		if(_sharedInstance)
		{
			_sharedInstance->RemoveAttachment(_sharedInstance->_physicsWorld);

#if RN_PLATFORM_ANDROID
			if(_sharedInstance->_vrWindow)
			{
				_sharedInstance->_vrWindow->Release(); //Reference from VRCamera
				_sharedInstance->_vrWindow->Release(); //Reference from World
				_sharedInstance->_vrWindow->Release(); //Reference from Application
				//_sharedInstance->_vrWindow->StopRendering();
			}
#endif
		}
		
		exit(0);
		//RN::Kernel::GetSharedInstance()->Exit();
	}

	World::World(RN::VRWindow *vrWindow) : _vrWindow(nullptr), _physicsWorld(nullptr), _isPaused(false), _isDash(false), _shaderLibrary(nullptr), _currentLevelSection(0)
	{
		_sharedInstance = this;

		if (vrWindow)
			_vrWindow = vrWindow->Retain();
		
		_levelNodes = new RN::Array();
	}

	World::~World()
	{
		
	}

	void World::WillBecomeActive()
	{
		RN::Scene::WillBecomeActive();

		if(!RN::Renderer::IsHeadless())
		{
			RN::Renderer *activeRenderer = RN::Renderer::GetActiveRenderer();
			_shaderLibrary = activeRenderer->CreateShaderLibraryWithFile(RNCSTR("shaders/Shaders.json"));
		}

		_cameraManager.Setup(_vrWindow);

		RN::String *pvdServerIP = nullptr;
#if RN_BUILD_DEBUG
	#if RN_PLATFORM_WINDOWS
		pvdServerIP = RNCSTR("localhost");
	#else
		pvdServerIP = RNCSTR("192.168.178.22");
	#endif
#endif
		_physicsWorld = new RN::PhysXWorld(RN::Vector3(0.0f, -9.81f, 0.0f), pvdServerIP);
		AddAttachment(_physicsWorld->Autorelease());

		LoadLevel();

		_player = new Player(_cameraManager.GetHeadSceneNode());
		AddNode(_player->Autorelease());
	}

	void World::DidBecomeActive()
	{
		RN::SceneBasic::DidBecomeActive();
		_cameraManager.SetCameraAmbientColor(RN::Color::White(), 1.0f, nullptr);
	}

	void World::WillUpdate(float delta)
	{
		RN::Scene::WillUpdate(delta);

		_isPaused = false;
		_isDash = false;
		RN::VRHMDTrackingState::Mode headsetState = _cameraManager.Update(delta);
		if(headsetState == RN::VRHMDTrackingState::Mode::Paused)
		{
			_isPaused = true;
			_isDash = true;
		}
		else if(headsetState == RN::VRHMDTrackingState::Mode::Disconnected)
		{
			Exit();
		}

		if(RN::InputManager::GetSharedInstance()->IsControlToggling(RNCSTR("ESC")))
		{
			Exit();
		}
	}

	RN::Model *World::AssignShader(RN::Model *model, Types::MaterialType materialType) const
	{
		if(RN::Renderer::IsHeadless()) return model;
		
		World *world = World::GetSharedInstance();
		RN::ShaderLibrary *shaderLibrary = world->GetShaderLibrary();

		RN::Model::LODStage *lodStage = model->GetLODStage(0);
		for(int i = 0; i < lodStage->GetCount(); i++)
		{
			RN::Material *material = lodStage->GetMaterialAtIndex(i);
			
			switch(materialType)
			{
				case Types::MaterialDefault:
				{
					material->SetDepthWriteEnabled(true);
					material->SetDepthMode(RN::DepthMode::LessOrEqual);
					material->SetAlphaToCoverage(false);
					material->SetAmbientColor(RN::Color::White());
					material->SetCullMode(RN::CullMode::None);
                                        
					RN::Shader::Options *shaderOptions = RN::Shader::Options::WithMesh(lodStage->GetMeshAtIndex(i));
					shaderOptions->EnableDirectionalLights();
					shaderOptions->EnableDirectionalShadows();
					material->SetVertexShader(RN::Renderer::GetActiveRenderer()->GetDefaultShader(RN::Shader::Type::Vertex, shaderOptions));
					material->SetFragmentShader(RN::Renderer::GetActiveRenderer()->GetDefaultShader(RN::Shader::Type::Fragment, shaderOptions));
					break;
				}
			}
		}

		return model;
	}

	RN::Model *World::MakeDeepCopy(RN::Model *model) const
	{
		RN::Model *result = model->Copy();
		
		RN::Model::LODStage *lodStage = result->GetLODStage(0);
		for(int i = 0; i < lodStage->GetCount(); i++)
		{
			RN::Material *material = lodStage->GetMaterialAtIndex(i)->Copy();
			lodStage->ReplaceMaterial(material->Autorelease(), i);
		}
		
		return result->Autorelease();
	}

	void World::AddLevelNode(RN::SceneNode *node)
	{
		_levelNodes->AddObject(node);
		AddNode(node);
	}

	void World::RemoveLevelNode(RN::SceneNode *node)
	{
		_levelNodes->RemoveObject(node);
		RemoveNode(node);
	}

	void World::RemoveAllLevelNodes()
	{
		_levelNodes->Enumerate<RN::SceneNode>([&](RN::SceneNode *node, size_t index, bool &stop){
			RemoveNode(node);
		});
		
		_levelNodes->RemoveAllObjects();
	}

	RN::Entity *World::CreateLevelEntity(const RN::String *fileName)
	{
		RN::Model *model = AssignShader(RN::Model::WithName(fileName), Types::MaterialDefault);
		RN::Entity *entity = new RN::Entity(model);
		AddLevelNode(entity->Autorelease());

		RN::PhysXMaterial *physicsMaterial = new RN::PhysXMaterial();
		RN::PhysXCompoundShape *shape = RN::PhysXCompoundShape::WithModel(model, physicsMaterial->Autorelease(), true, false);
		RN::PhysXStaticBody *body = RN::PhysXStaticBody::WithShape(shape);
		body->SetCollisionFilter(Types::CollisionLevel, Types::CollisionAll);
		entity->AddAttachment(body);

		return entity;
	}

	void World::UnlockLevelSection(int section)
	{
		if(_currentLevelSection != section) return;
		_currentLevelSection += 1;
		
		switch(section)
		{
		case 0:
		case 1:
		case 2:
			//RemoveLevelNode(_levelPart[section]);
			break;
		}
	}

	void World::LoadLevel()
	{
		RemoveAllLevelNodes();

		RN::Light *sunLight = new RN::Light(RN::Light::Type::DirectionalLight);
		sunLight->SetWorldRotation(RN::Vector3(45.0f + 90, -45.0f, 0.0f));
		AddLevelNode(sunLight);

		RN::ShadowParameter shadowParameter(_cameraManager.GetHeadCamera(), 1024);
		shadowParameter.distanceBlendFactor = 0.05f;
		shadowParameter.splits[0].maxDistance = 2.5f;
		shadowParameter.splits[0].biasFactor = 2.0f;
		shadowParameter.splits[0].biasUnits = 128.0f;
		shadowParameter.splits[1].maxDistance = 10.0f;
		shadowParameter.splits[1].biasFactor = 2.0f;
		shadowParameter.splits[1].biasUnits = 128.0f;
		shadowParameter.splits[2].maxDistance = 20.0f;
		shadowParameter.splits[2].biasFactor = 3.0f;
		shadowParameter.splits[2].biasUnits = 256.0f;
		shadowParameter.splits[3].maxDistance = 50.0f;
		shadowParameter.splits[3].biasFactor = 3.0f;
		shadowParameter.splits[3].biasUnits = 256.0f;
		sunLight->ActivateShadows(shadowParameter);

		CreateLevelEntity(RNCSTR("models/stage/gamejam_level_floor.sgm"));
		//_levelPart[0] = CreateLevelEntity(RNCSTR("models/stage/gamejam_level_wall_01.sgm"));
		//_levelPart[1] = CreateLevelEntity(RNCSTR("models/stage/gamejam_level_wall_02.sgm"));
		//_levelPart[2] = CreateLevelEntity(RNCSTR("models/stage/gamejam_level_wall_03.sgm"));
		//_levelPart[3] = CreateLevelEntity(RNCSTR("models/stage/gamejam_level_wall_04.sgm"));

		_levelPart[4] = CreateLevelEntity(RNCSTR("models/stage/gamejam_level_bridge.sgm"));

		Ball *ball = new Ball();
		AddLevelNode(ball->Autorelease());
		ball->SetWorldPosition(RN::Vector3(3.0f, 1.0f, -10.0f));

		Box *box = new Box();
		AddLevelNode(box->Autorelease());
		box->SetWorldPosition(RN::Vector3(0.0f, 2.0f, -2.0f));

		if(!RN::Renderer::IsHeadless())
		{
			RN::Model *skyModel = RN::Model::WithName(RNCSTR("models/template/sky.sgm"));
			RN::Material *skyMaterial = RN::Material::WithShaders(nullptr, nullptr);
			skyMaterial->SetDepthMode(RN::DepthMode::LessOrEqual);

			RN::Shader::Options *skyShaderOptions = RN::Shader::Options::WithMesh(skyModel->GetLODStage(0)->GetMeshAtIndex(0));
			skyShaderOptions->AddDefine(RNCSTR("RN_SKY"), RNCSTR("1"));
			skyMaterial->SetVertexShader(RN::Renderer::GetActiveRenderer()->GetDefaultShader(RN::Shader::Type::Vertex, skyShaderOptions, RN::Shader::UsageHint::Default));
			skyMaterial->SetFragmentShader(RN::Renderer::GetActiveRenderer()->GetDefaultShader(RN::Shader::Type::Fragment, skyShaderOptions, RN::Shader::UsageHint::Default));
			skyMaterial->SetVertexShader(RN::Renderer::GetActiveRenderer()->GetDefaultShader(RN::Shader::Type::Vertex, skyShaderOptions, RN::Shader::UsageHint::Depth), RN::Shader::UsageHint::Depth);
			skyMaterial->SetFragmentShader(RN::Renderer::GetActiveRenderer()->GetDefaultShader(RN::Shader::Type::Fragment, skyShaderOptions, RN::Shader::UsageHint::Depth), RN::Shader::UsageHint::Depth);
			skyModel->GetLODStage(0)->ReplaceMaterial(skyMaterial, 0);
			
			RN::Entity *skyEntity = new RN::Entity(skyModel);
			skyEntity->SetScale(RN::Vector3(10.0f));
			skyEntity->AddFlags(RN::Entity::Flags::DrawLate);
			AddLevelNode(skyEntity->Autorelease());
		}
	}
}
