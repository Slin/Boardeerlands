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

		if(RN::InputManager::GetSharedInstance()->IsControlToggling(RNCSTR("O")))
		{
			if(!_isOPressed)
			{
				UnlockLevelSection(_currentLevelSection);
			}
			_isOPressed = true;
		}
		else
		{
			_isOPressed = false;
		}

		if(_currentLevelSection == 1)
		{
			if(_player->GetWorldPosition().y > 1.3f && _player->GetWorldPosition().z < -2.0f)
			{
				UnlockLevelSection(1);
			}
		}

		if(_currentLevelSection == 2)
		{
			bool canSwitch = true;
			for(int i = 0; i < 1; i++)
			{
				if(!_boxTriggersSection2[0]->GetIsActive()) canSwitch = false;
			}
			if(canSwitch)
			{
				UnlockLevelSection(2);
			}
		}

		if(_currentLevelSection == 4)
		{
			if(_player->GetWorldPosition().z < -6.0f)
			{
				UnlockLevelSection(4);
			}
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
                                        
					RN::Shader::Options *shaderOptions = RN::Shader::Options::WithMesh(lodStage->GetMeshAtIndex(i));
					shaderOptions->EnableDirectionalLights();
					shaderOptions->EnableDirectionalShadows();
					material->SetVertexShader(RN::Renderer::GetActiveRenderer()->GetDefaultShader(RN::Shader::Type::Vertex, shaderOptions));
					material->SetFragmentShader(RN::Renderer::GetActiveRenderer()->GetDefaultShader(RN::Shader::Type::Fragment, shaderOptions));
					break;
				}

				case Types::MaterialWater:
				{
					material->SetDepthWriteEnabled(true);
					material->SetDepthMode(RN::DepthMode::LessOrEqual);
					material->SetAlphaToCoverage(false);
					material->SetAmbientColor(RN::Color::White());
					material->SetCullMode(RN::CullMode::None);
					material->SetBlendOperation(RN::BlendOperation::Add, RN::BlendOperation::Add);

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
		RN::PhysXCompoundShape *shape = RN::PhysXCompoundShape::WithModel(model, physicsMaterial->Autorelease(), true);
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
				RemoveLevelNode(_levelPart[section]);
				break;

			case 3:
				_levelPart[3]->Downcast<Water>()->MoveTo(RN::Vector3(0.0f, -1.5, 0.0f), 5.0f);
				break;

			case 4:
				_levelPart[4]->Downcast<Bridge>()->MoveTo(RN::Vector3(0.0f, 0.0f, 0.0f), 5.0f);
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

		CreateLevelEntity(RNCSTR("models/stage/gamejam_level.sgm"));
		_levelPart[0] = CreateLevelEntity(RNCSTR("models/stage/wall_a.sgm"));
		_levelPart[1] = CreateLevelEntity(RNCSTR("models/stage/wall_b.sgm"));
		_levelPart[2] = CreateLevelEntity(RNCSTR("models/stage/wall_c.sgm"));

		_levelPart[3] = new Water();
		AddLevelNode(_levelPart[3]->Autorelease());
		_levelPart[3]->SetWorldPosition(RN::Vector3(0.0f, -0.2f, 0.0f));

		_levelPart[4] = new Bridge();
		AddLevelNode(_levelPart[4]->Autorelease());
		_levelPart[4]->SetWorldPosition(RN::Vector3(0.0f, 0.0f, 4.0f));

		Ball *ball = new Ball();
		AddLevelNode(ball->Autorelease());
		ball->SetWorldPosition(RN::Vector3(3.0f, 1.0f, -10.0f));

		Box *box = new Box();
		AddLevelNode(box->Autorelease());
		box->SetWorldPosition(RN::Vector3(0.0f, 2.0f, -2.0f));

		_boxTriggersSection2[0] = new BoxTrigger();
		AddLevelNode(_boxTriggersSection2[0]->Autorelease());
		_boxTriggersSection2[0]->SetWorldPosition(RN::Vector3(1.0f, 0.0f, -2.0f));
		_boxTriggersSection2[0]->RegisterBox(box);

		Chain *chain = new Chain(RN::Vector3(2.89f, -2.2f, -5.48f));
		AddLevelNode(chain->Autorelease());

		Bucket *smallBucket = new Bucket(false);
		AddLevelNode(smallBucket->Autorelease());
		smallBucket->SetWorldPosition(RN::Vector3(-2.0f, 1.0f, -10.0f));
		
		Bucket *bigBucket = new Bucket(true);
		AddLevelNode(bigBucket->Autorelease());
		bigBucket->SetWorldPosition(RN::Vector3(-3.0f, 1.0f, -10.0f));

		smallBucket->SetOtherBucket(bigBucket);
		bigBucket->SetOtherBucket(smallBucket);

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
