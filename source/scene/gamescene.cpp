#include "gamescene.h"
#include "../gameflow.h"
#include "../manager/guimanager.h"

SceneNode *gScene = nullptr;
PhysicsManager *gPhysics = nullptr;
SoundManager *gSoundManager =nullptr;
WorldManager *gWorldManager = nullptr;
GameScene *gGameScene = nullptr;

GameScene::GameScene(SceneNode *parent, Camera *mainCamera, const String &sceneFile)
	: pPlayer(nullptr)
	, pCamera(mainCamera)
	, clCamera()
	, pParentScene(parent)
	, cScene()
	, musCur(nullptr)
	, pGameMap(nullptr)
	, bPaused(false)
	, bInitialized(false)
	, sSceneFile(sceneFile)
	, fTimeToNextLevel(3.0f)
	, bChangeLevel(false)
	, pGameOverImg(nullptr)
	, vCameraFrom(0.0f, 0.0f, 0.0f)
	, vCameraCurrent(0.0f, 0.0f, 0.0f)
	, vCameraTo(0.0f, 0.0f, 0.0f)
	, fElapsed(0.0f)
	, bMoveCamera(false)
{
	gScene = &cScene;
	gPhysics = &clPhysicsManager;
	gSoundManager = &clSoundManager;
	gWorldManager = &clWorldManager;
	gGameScene = this;
}

GameScene::~GameScene()
{
	gScene = nullptr;
}

bool GameScene::Initialize()
{
	pParentScene->Add(&cScene);

	// Create the transitions
	cRunToPause.Initialize(&cRun, &cOnPause, &cPause);
	cPauseToRun.Initialize(&cPause, &cOnRun, &cRun);
	cPauseToMenu.Initialize(&cPause, &cOnMenu, &cMenu);
	cRunToGameOver.Initialize(&cRun, &cOnGameOver, &cGameOver);
	cGameOverToMenu.Initialize(&cGameOver, &cOnMenu, &cMenu);

	// Create the State Machine.
	cFlow.RegisterTransition(&cRunToPause);
	cFlow.RegisterTransition(&cPauseToRun);
	cFlow.RegisterTransition(&cPauseToMenu);
	cFlow.RegisterTransition(&cRunToGameOver);
	cFlow.RegisterTransition(&cGameOverToMenu);

	cFlow.Initialize(&cRun);

	auto cb = [&](Job *self)
	{
		auto job = static_cast<FileLoader *>(self);

		if(job->GetState() == eJobState::Completed)
		{
			OnJobCompleted(job);
		}
		else if(job->GetState() == eJobState::Aborted)
		{
			OnJobAborted();
		}
		sdDelete(self);
	};

	String f{"scenes/"};
	pJobManager->Add(sdNew(FileLoader(f + sSceneFile, cb)));

	RocketEventManager::AddListener(this);
	pInput->AddKeyboardListener(this);

	// Get the initial value from game data
	gGui->SetPlayerName("OptimistPlayer");
	gGui->SetLevel(1);
	gGui->SetXP(100);
	gGui->SetAttackPower(15);
	gGui->SetGold(0);
	gGui->SetLife(22);
	gGui->SetMana(10);

	return true;
}

bool GameScene::Update(f32 dt)
{
	if (!bInitialized)
		return true;

	cFlow.Update(dt);

	if (bPaused)
		return true;

	if (bMoveCamera)
	{
		fElapsed += dt;
		if (fElapsed > 1.0f)
			fElapsed = 1.0f;

		if (fElapsed < 1.0f)
		{
			auto A = ((1.f - fElapsed) * vCameraFrom);
			auto B = (fElapsed * vCameraTo);
			vCameraCurrent = A + B;
			clCamera.LookAt(vCameraCurrent);
		}
		else
		{
			clCamera.LookAt(pPlayer->GetSprite()->GetPosition());
			bMoveCamera = false;
		}
	}
	else
	{
		clPhysicsManager.Update(dt);
		clWorldManager.Update(dt);
		clCamera.LookAt(pPlayer->GetPosition());
	}

	if (bChangeLevel)
	{
		fTimeToNextLevel -= dt;
		if (fTimeToNextLevel <= 0)
		{
			gFlow->LoadSceneFile(strNextLevel);
		}
	}

	/*if (gGameData->GetLife() == 0)
	{
		pGameOverImg->SetVisible(true);
		pGameOverImg->SetPosition(pCamera->GetPosition() - Vector3f(-400.0f, -300.0f, 0.0f));
		pPlayer->GetSprite()->SetVisible(false);
		cFlow.OnEvent(&cOnGameOver, this);

		gGameData->SetLife(3);
	}*/

	return true;
}

bool GameScene::Shutdown()
{
	if (musCur)
		musCur->Unload();

	clWorldManager.Clear();

	pParentScene->Remove(&cScene);
	cScene.Unload();
	pParentScene = nullptr;

	pInput->RemoveKeyboardListener(this);
	RocketEventManager::RemoveListener(this);

	return true;
}

bool GameScene::OnInputKeyboardRelease(const EventInputKeyboard *ev)
{
	Key k = ev->GetKey();
	if (k == eKey::Escape)
	{
		if (bPaused)
			cFlow.OnEvent(&cOnRun, this);
		else
			cFlow.OnEvent(&cOnPause, this);
	}

	return true;
}

void GameScene::OnGuiEvent(Rocket::Core::Event &ev, const Rocket::Core::String &script)
{
	UNUSED(ev)
	if (script == "resume")
	{
		cFlow.OnEvent(&cOnRun, this);
	}
	if (script == "quit")
	{
		cFlow.OnEvent(&cOnMenu);
		gFlow->Menu();
	}
}

void GameScene::Pause()
{
	bPaused = true;
}

void GameScene::Resume()
{
	bPaused = false;
}

void GameScene::OnJobCompleted(FileLoader *job)
{
	Reader r(job->pFile);
	cScene.Load(r);
	Log("Scene Name: %s len %d", cScene.sName.c_str(), cScene.Size());

	// Validate the music to play
	if (gGameData->IsBgmEnabled() == true)
	{
		musThemeOptimist.Load("sounds/optimist_theme.ogg");
		musThemeOptimist.SetVolume(1.0f);
		musThemeRealist.Load("sounds/realist_theme.ogg");
		musThemeRealist.SetVolume(1.0f);
		musThemePessimist.Load("sounds/pessimist_theme.ogg");
		musThemePessimist.SetVolume(1.0f);
	}

	SceneNode *sounds = (SceneNode *)cScene.GetChildByName("Sounds");
	clSoundManager.Init(*sounds);

	SceneNode *sprites = (SceneNode *)cScene.GetChildByName("Sprites");
	pGameMap = (GameMap *)cScene.GetChildByName("Map");

	int hostageNum = 0;

	strNextLevel = pGameMap->GetProperty("NextLevel");

	MapLayerMetadata *game = pGameMap->GetLayerByName("Game")->AsMetadata();
	game->SetVisible(false);
	for (unsigned i = 0, len = game->Size(); i < len; ++i)
	{
		MetadataObject *placeHolder = static_cast<MetadataObject *>( game->GetChildAt(i));
		//const String &type = placeHolder->GetProperty("Type");
		//if (type == "Entity")
		{
			Entity* entity = clWorldManager.BuildEntity(*placeHolder, sprites);
			//Log("%s", entity->GetName().c_str());
			if (entity->GetClassName() == "RealistPlayer")
			{
				pPlayerRealist = static_cast<RealistPlayerEntity*>(entity);
			}
			if (entity->GetClassName() == "PessimistPlayer")
			{
				pPlayerPessimist = static_cast<PessimistPlayerEntity*>(entity);
			}
			if (entity->GetClassName() == "OptimistPlayer")
			{
				pPlayerOptimist = static_cast<OptimistPlayerEntity*>(entity);
			}
		}
	}

	// If the player is not set, the player will be optimist
	if (pPlayer == nullptr)
	{
		pPlayer = pPlayerOptimist;
		musCur = &musThemeOptimist;
		pSoundSystem->PlayMusic(musCur);
	}

	this->LoadMapColliders();

	clCamera.SetCamera(pCamera);
	clCamera.LookAt(pPlayer->GetSprite()->GetPosition());

	MapLayerTiled *bg = pGameMap->GetLayerByName("Background")->AsTiled();

	f32 hw = bg->GetWidth() * 0.5f;
	f32 hh = bg->GetHeight() * 0.5f;
	clCamera.SetArea(Rect4f(-hw, -hh, bg->GetWidth(), bg->GetHeight()));

	sprites->SetVisible(false);

	pGameOverImg = (Image *)cScene.GetChildByName("GameOverImage");
	pGameOverImg->SetVisible(false);

	bInitialized = true;
}

void GameScene::ChangePlayer(const String currentPlayer)
{
	pSoundSystem->StopMusic(10.0f);
	bMoveCamera = true;

	OptimistPlayerEntity *optimistPlayer = static_cast<OptimistPlayerEntity *>(gWorldManager->FindEntityByClassName("OptimistPlayer"));
	RealistPlayerEntity *realistPlayer = static_cast<RealistPlayerEntity *>(gWorldManager->FindEntityByClassName("RealistPlayer"));
	PessimistPlayerEntity *pessimistPlayer = static_cast<PessimistPlayerEntity *>(gWorldManager->FindEntityByClassName("PessimistPlayer"));

	Log("Current player: %s", currentPlayer.c_str());

	// otim -> real -> pess
	if (pPlayer == optimistPlayer)
	{
		optimistPlayer->SetIsActive(false);
		realistPlayer->SetIsActive(true);
		pessimistPlayer->SetIsActive(false);

		auto tex = static_cast<Texture *>(pResourceManager->Get("textures/realist_ground_tileset.png", ITexture::GetTypeId()));
		auto tiles = pGameMap->GetLayerByName("Background")->AsTiled();
		auto set = tiles->GetTileSet();
		set->SetTexture(tex);

		//Lerp camera
		vCameraFrom = optimistPlayer->GetSprite()->GetPosition();
		vCameraTo = realistPlayer->GetSprite()->GetPosition();
		fElapsed = 0.0f;

		pPlayer = realistPlayer;
		musCur = &musThemeRealist;
	}
	else if (pPlayer == realistPlayer)
	{
		optimistPlayer->SetIsActive(false);
		realistPlayer->SetIsActive(false);
		pessimistPlayer->SetIsActive(true);

		auto tex = static_cast<Texture *>(pResourceManager->Get("textures/pessimist_ground_tileset.png", ITexture::GetTypeId()));
		auto tiles = pGameMap->GetLayerByName("Background")->AsTiled();
		auto set = tiles->GetTileSet();
		set->SetTexture(tex);

		//Lerp camera
		vCameraFrom = realistPlayer->GetSprite()->GetPosition();
		vCameraTo = pessimistPlayer->GetSprite()->GetPosition();
		fElapsed = 0.0f;

		pPlayer = pessimistPlayer;
		musCur = &musThemePessimist;
	}
	else if (pPlayer == pessimistPlayer)
	{
		optimistPlayer->SetIsActive(true);
		realistPlayer->SetIsActive(false);
		pessimistPlayer->SetIsActive(false);

		auto tex = static_cast<Texture *>(pResourceManager->Get("textures/optimist_ground_tileset.png", ITexture::GetTypeId()));
		auto tiles = pGameMap->GetLayerByName("Background")->AsTiled();
		auto set = tiles->GetTileSet();
		set->SetTexture(tex);

		//Lerp camera
		vCameraFrom = pessimistPlayer->GetSprite()->GetPosition();
		vCameraTo = optimistPlayer->GetSprite()->GetPosition();
		fElapsed = 0.0f;

		pPlayer = optimistPlayer;
		musCur = &musThemeOptimist;
	}

	// Set the name of the player to UI
	gGui->SetPlayerName(pPlayer->GetDisplayName());
	gGui->SetLevel(pPlayer->GetLevel());
	gGui->SetXP(pPlayer->GetXP());
	gGui->SetLife(pPlayer->GetLife());
	gGui->SetMana(pPlayer->GetMana());

	pSoundSystem->PlayMusic(musCur, 10.0f);
}

void GameScene::OnJobAborted()
{
}

void GameScene::LoadMapColliders()
{
	MapLayerMetadata *game = pGameMap->GetLayerByName("Colliders")->AsMetadata();
	game->SetVisible(false);
	for (unsigned i = 0, len = game->Size(); i < len; ++i)
	{
		MetadataObject *placeHolder = static_cast<MetadataObject *>( game->GetChildAt(i));

		clPhysicsManager.CreateStaticBody(placeHolder);
	}
}

void GameScene::RemoveLife()
{
	gGui->RemoveLife();
}
