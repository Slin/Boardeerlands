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

	World::World(RN::VRWindow *vrWindow) : _vrWindow(nullptr), _physicsWorld(nullptr), _isPaused(false), _isDash(false), _shaderLibrary(nullptr), _currentLevelSection(0), _isShowingInfo(true), _currentIntroScreen(0)
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
		//_physicsWorld->SetSubsteps(1);
		AddAttachment(_physicsWorld->Autorelease());

		LoadLevel();

		_player = new Player(_cameraManager.GetHeadSceneNode());
		AddNode(_player->Autorelease());
		_player->Update(0.0f);

		PlayIntro();
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
		if(headsetState == RN::VRHMDTrackingState::Mode::Paused || _isShowingInfo)
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
		
		if(RN::InputManager::GetSharedInstance()->IsControlToggling(RNCSTR("E")))
		{
			if(!_isEPressed)
			{
				if(_currentIntroScreen < 10 && _isShowingInfo)
				{
					_currentIntroScreen += 1;
					if(_currentIntroScreen == 1)
					{
						_titleEntity->GetModel()->GetLODStage(0)->GetMaterialAtIndex(0)->RemoveAllTextures();
						_titleEntity->GetModel()->GetLODStage(0)->GetMaterialAtIndex(0)->AddTexture(RN::Texture::WithName(RNCSTR("introoutro/intro_1.png")));
					}
					else if(_currentIntroScreen == 2)
					{
						_titleEntity->GetModel()->GetLODStage(0)->GetMaterialAtIndex(0)->RemoveAllTextures();
						_titleEntity->GetModel()->GetLODStage(0)->GetMaterialAtIndex(0)->AddTexture(RN::Texture::WithName(RNCSTR("introoutro/intro_2.png")));
					}
					else
					{
						_isShowingInfo = false;
						_player->DidActivate();
						RemoveNode(_titleEntity);
					}
				}
				else if (_currentIntroScreen >= 10 && _isShowingInfo)
				{
					Exit();
				}
			}
			
			_isEPressed = true;
		}
		else
		{
			_isEPressed = false;
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
			if(_player->GetWorldPosition().y > 1.3f && (std::abs(_player->GetWorldPosition().x) > 2.0f || std::abs(_player->GetWorldPosition().z) > 2.0f))
			{
				UnlockLevelSection(1);
			}
		}

		if(_currentLevelSection == 2)
		{
			bool canSwitch = true;
			for(int i = 0; i < 4; i++)
			{
				if(!_boxTriggersSection2[i]->GetIsActive()) canSwitch = false;
			}
			if(canSwitch)
			{
				UnlockLevelSection(2);
			}
		}

		if(_player->GetWorldPosition().z > 9.5f && !_isShowingInfo)
		{
			PlayOutro();
		}
	}


	void World::PlayOutro()
	{
		_currentIntroScreen = 10;
		_isShowingInfo = true;

		_titleEntity = new RN::Entity(RN::Model::WithName(RNCSTR("introoutro/title.sgm")));
		_titleEntity->GetModel()->GetLODStage(0)->GetMaterialAtIndex(0)->RemoveAllTextures();
		_titleEntity->GetModel()->GetLODStage(0)->GetMaterialAtIndex(0)->AddTexture(RN::Texture::WithName(RNCSTR("introoutro/outro.png")));
		AddNode(_titleEntity->Autorelease());
		_titleEntity->SetWorldPosition(_cameraManager.GetHeadCamera()->GetWorldPosition() + _cameraManager.GetHeadCamera()->GetForward() * 0.95f + _cameraManager.GetHeadCamera()->GetUp() * 0.0f);
		_titleEntity->SetWorldRotation(_cameraManager.GetHeadCamera()->GetWorldRotation());
	}

	void World::PlayIntro()
	{
		_currentIntroScreen = 0;
		_isShowingInfo = true;

		_titleEntity = new RN::Entity(RN::Model::WithName(RNCSTR("introoutro/title.sgm")));
		AddNode(_titleEntity->Autorelease());
		_titleEntity->SetWorldPosition(_cameraManager.GetHeadCamera()->GetWorldPosition() + _cameraManager.GetHeadCamera()->GetForward() * 0.95f + _cameraManager.GetHeadCamera()->GetUp() * 0.22f);
		_titleEntity->SetWorldRotation(_cameraManager.GetHeadCamera()->GetWorldRotation());
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

			case 5:
				RemoveLevelNode(_levelPart[section]);
				break;
			case 6:
				_levelPart[6]->Downcast<ExitDoor>()->MoveTo(RN::Vector3(2.5f, 0.0f, 0.0f), 5.0f);
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

		_levelPart[5] = CreateLevelEntity(RNCSTR("models/stage/wall_d.sgm"));

		_levelPart[6] = new ExitDoor();
		AddLevelNode(_levelPart[6]->Autorelease());

		Ball *ball = new Ball();
		AddLevelNode(ball->Autorelease());
		ball->SetWorldPosition(RN::Vector3(3.0f, 2.0f, -10.0f));

		Box *box1 = new Box();
		AddLevelNode(box1->Autorelease());
		box1->SetWorldPosition(RN::Vector3(1.0f, 2.0f, -1.7f));

		Box *box2 = new Box();
		AddLevelNode(box2->Autorelease());
		box2->SetWorldPosition(RN::Vector3(-1.0f, 2.0f, -1.7f));

		Box *box3 = new Box();
		AddLevelNode(box3->Autorelease());
		box3->SetWorldPosition(RN::Vector3(-1.0f, 2.0f, 1.7f));

		Box *box4 = new Box();
		AddLevelNode(box4->Autorelease());
		box4->SetWorldPosition(RN::Vector3(-1.7f, 2.0f, 1.7f));

		_boxTriggersSection2[0] = new BoxTrigger();
		AddLevelNode(_boxTriggersSection2[0]->Autorelease());
		_boxTriggersSection2[0]->SetWorldPosition(RN::Vector3(3.5f, 0.0f, 3.5f));
		_boxTriggersSection2[0]->RegisterBox(box1);
		_boxTriggersSection2[0]->RegisterBox(box2);
		_boxTriggersSection2[0]->RegisterBox(box3);
		_boxTriggersSection2[0]->RegisterBox(box4);

		_boxTriggersSection2[1] = new BoxTrigger();
		AddLevelNode(_boxTriggersSection2[1]->Autorelease());
		_boxTriggersSection2[1]->SetWorldPosition(RN::Vector3(-3.5f, 0.0f, 3.5f));
		_boxTriggersSection2[1]->RegisterBox(box1);
		_boxTriggersSection2[1]->RegisterBox(box2);
		_boxTriggersSection2[1]->RegisterBox(box3);
		_boxTriggersSection2[1]->RegisterBox(box4);

		_boxTriggersSection2[2] = new BoxTrigger();
		AddLevelNode(_boxTriggersSection2[2]->Autorelease());
		_boxTriggersSection2[2]->SetWorldPosition(RN::Vector3(3.5f, 0.0f, -3.5f));
		_boxTriggersSection2[2]->RegisterBox(box1);
		_boxTriggersSection2[2]->RegisterBox(box2);
		_boxTriggersSection2[2]->RegisterBox(box3);
		_boxTriggersSection2[2]->RegisterBox(box4);

		_boxTriggersSection2[3] = new BoxTrigger();
		AddLevelNode(_boxTriggersSection2[3]->Autorelease());
		_boxTriggersSection2[3]->SetWorldPosition(RN::Vector3(-3.5f, 0.0f, -3.5f));
		_boxTriggersSection2[3]->RegisterBox(box1);
		_boxTriggersSection2[3]->RegisterBox(box2);
		_boxTriggersSection2[3]->RegisterBox(box3);
		_boxTriggersSection2[3]->RegisterBox(box4);

		Chain *chain = new Chain(RN::Vector3(2.89f, -2.2f, -5.48f), RN::Vector3(4.3f, 1.1f, -5.0f));
		AddLevelNode(chain->Autorelease());

		Bucket *smallBucket = new Bucket(false);
		AddLevelNode(smallBucket->Autorelease());
		smallBucket->SetWorldPosition(RN::Vector3(-1.6f, 2.0f, -10.1f));
		smallBucket->SetWorldRotation(RN::Vector3(180.0f, 0.0f, 0.0f));
		
		Bucket *bigBucket = new Bucket(true);
		AddLevelNode(bigBucket->Autorelease());
		bigBucket->SetWorldPosition(RN::Vector3(-1.9f, 2.0f, -10.45f));
		bigBucket->SetWorldRotation(RN::Vector3(180.0f, 0.0f, 0.0f));

		smallBucket->SetOtherBucket(bigBucket);
		bigBucket->SetOtherBucket(smallBucket);

		_bucketTrigger = new BucketTrigger();
		AddLevelNode(_bucketTrigger->Autorelease());
		_bucketTrigger->SetWorldPosition(RN::Vector3(-1.9f, 1.0f, -10.45f));
		_bucketTrigger->RegisterBucket(smallBucket);
		_bucketTrigger->RegisterBucket(bigBucket);

		_playerTrigger = new PlayerTrigger();
		AddLevelNode(_playerTrigger->Autorelease());
		_playerTrigger->SetWorldPosition(RN::Vector3(0.0f, 0.0f, 6.5f));

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
