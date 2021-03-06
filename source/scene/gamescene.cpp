#include "gamescene.h"
#include "../gameflow.h"
#include "../manager/guimanager.h"
#include "../manager/worldmanager.h"

SceneNode *gScene = nullptr;
PhysicsManager *gPhysics = nullptr;
SoundManager *gSoundManager =nullptr;
WorldManager *gWorldManager = nullptr;
GameScene *gGameScene = nullptr;

GameScene::GameScene(SceneNode *parent, Camera *mainCamera, const String &sceneFile)
	: pPlayer(nullptr)
	, pPlayerRealist(nullptr)
	, pPlayerPessimist(nullptr)
	, pPlayerOptimist(nullptr)
	, pCamera(mainCamera)
	, clCamera()
	, pParentScene(parent)
	, cScene()
	, musCur(nullptr)
	, pGameMap(nullptr)
	, pFogMap(nullptr)
	, pFog(nullptr)
	, iTileSize(40) // READ FROM MAP - USED FOR FOG PIXEL TO TILE CONVERSION
	, pTilesetOptimist(nullptr)
	, pTilesetPessimist(nullptr)
	, pTilesetRealist(nullptr)
	, sSceneFile(sceneFile)
	, fTimeToNextLevel(0.0f)
	, pGameOverImg(nullptr)
	, vCameraFrom(0.0f, 0.0f, 0.0f)
	, vCameraCurrent(0.0f, 0.0f, 0.0f)
	, vCameraTo(0.0f, 0.0f, 0.0f)
	, fElapsed(0.0f)
	, bPaused(false)
	, bInitialized(false)
	, bMoveCamera(false)
	, bChangeLevel(false)
{
	gScene = &cScene;
	gPhysics = &clPhysicsManager;
	gSoundManager = &clSoundManager;
	gWorldManager = &clWorldManager;
	gGameScene = this;
	memset(&bRequiredKeys, 0x00, sizeof(bRequiredKeys));
}

GameScene::~GameScene()
{
	pTilesetOptimist->Release();
	pTilesetRealist->Release();
	pTilesetPessimist->Release();
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

		pTilesetOptimist = static_cast<Texture *>(pResourceManager->Get("textures/realist_ground_tileset.png", ITexture::GetTypeId()));
		pTilesetRealist = static_cast<Texture *>(pResourceManager->Get("textures/pessimist_ground_tileset.png", ITexture::GetTypeId()));
		pTilesetPessimist = static_cast<Texture *>(pResourceManager->Get("textures/optimist_ground_tileset.png", ITexture::GetTypeId()));

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
	gGui->SetPlayerName("Optimist");
	gGui->SetLevel(1);
	gGui->SetXP(100);
	gGui->SetAttackPower(15);
	gGui->SetGold(0);
	gGui->SetLife(22, 22);
	gGui->SetStamina(10, 10);

	return true;
}

void GameScene::UseKey(u32 key)
{
	if (!key)
		return;

	bRequiredKeys[key - 1] = true;

	if (bRequiredKeys[0] && bRequiredKeys[1] && bRequiredKeys[2])
		this->ChangeLevel();
}

void GameScene::FogReveal(const vec3 &pos, u32 radius)
{
	if (!pFog)
		return;

	auto px = pos.x - 20.f;
	auto py = pos.y - 20.f;
	auto sx = s32((px / iTileSize) + iTileSize / 2) - 4;
	auto sy = s32((py / iTileSize) + iTileSize / 2) - 4;
	auto r = s32(radius);
	for (auto y = -r; y <= r; y++)
	{
		for (auto x = -r; x <= r; x++)
		{
			pFog->SetTileAt(sx + x, sy + y, 5); // 5 == ultimo tile, transparente
		}
	}
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
		clPhysicsManager.RemoveBodies();
		clPhysicsManager.Update(dt);
		clWorldManager.Update(dt);
		clCamera.LookAt(pPlayer->GetPosition());
		this->FogReveal(pPlayerRealist->GetPosition(), 2);
		this->FogReveal(pPlayerPessimist->GetPosition(), 1);
		this->FogReveal(pPlayerOptimist->GetPosition(), 3);
	}

	if (bChangeLevel)
	{
		fTimeToNextLevel -= dt;
		if (fTimeToNextLevel <= 0)
		{
			gFlow->LoadSceneFile(strNextLevel);
		}
	}

	if (gGameData->sGamePlay.bIsGameOver == true)
	{
		pGameOverImg->SetVisible(true);
		pGameOverImg->SetPosition(pCamera->GetPosition() - vec3(-512.0f, -384.0f, 0.0f));

		pPlayer->Mute();
		pPlayer->GetSprite()->SetVisible(false);
		cFlow.OnEvent(&cOnGameOver, this);
		pFog->SetVisible(false);
		pGameMap->SetVisible(false);

		clWorldManager.Clear();

		gGameData->sGamePlay.bIsGameOver = false;
	}

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
	else if (k == eKey::F12)
	{
		if (pFogMap)
			pFogMap->SetVisible(!pFogMap->IsVisible());
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

	strNextLevel = pGameMap->GetProperty("NextLevel");

	MapLayerMetadata *game = pGameMap->GetLayerByName("Game")->AsMetadata();
	game->SetVisible(false);
	for (unsigned i = 0, len = game->Size(); i < len; ++i)
	{
		MetadataObject *placeHolder = static_cast<MetadataObject *>(game->GetChildAt(i));
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
		gGui->SelectHero("Optimist");
		gGui->SelectEnemy();
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

	{
		pFogMap = sdNew(GameMap);
		pFogMap->sName = "Fog";
		pFogMap->bMarkForDeletion = true;
		pFogMap->SetPosition(pGameMap->GetPosition());
		pFogMap->SetWidth(pGameMap->GetWidth());
		pFogMap->SetHeight(pGameMap->GetHeight());
		pFogMap->SetZ(-500);

		pFog = pGameMap->GetLayerByName("Background")->AsTiled()->Clone();
		pFog->sName = "Fog Layer";
		pFogMap->AddLayer(pFog);

		auto tex = static_cast<Texture *>(pResourceManager->Get("textures/fog_tileset.png", ITexture::GetTypeId()));
		auto set = pFog->GetTileSet();
		set->SetTexture(tex);
		pFog->SetTileSet(set); // Trigger mesh rebuild

		cScene.Add(pFogMap);
	}

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
		optimistPlayer->SetIsInputEnabled(false);
		realistPlayer->SetIsActive(true);
		realistPlayer->SetIsInputEnabled(true);
		pessimistPlayer->SetIsActive(false);
		pessimistPlayer->SetIsInputEnabled(false);
		gGui->SelectHero("Realist");

		// Change the terrain tileset
		auto tiles = pGameMap->GetLayerByName("Background")->AsTiled();
		auto set = tiles->GetTileSet();
		set->SetTexture(pTilesetOptimist);

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
		optimistPlayer->SetIsInputEnabled(false);
		realistPlayer->SetIsActive(false);
		realistPlayer->SetIsInputEnabled(false);
		pessimistPlayer->SetIsActive(true);
		pessimistPlayer->SetIsInputEnabled(true);
		gGui->SelectHero("Pessimist");

		// Change the terrain tileset
		auto tiles = pGameMap->GetLayerByName("Background")->AsTiled();
		auto set = tiles->GetTileSet();
		set->SetTexture(pTilesetRealist);

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
		optimistPlayer->SetIsInputEnabled(true);
		realistPlayer->SetIsActive(false);
		realistPlayer->SetIsInputEnabled(false);
		pessimistPlayer->SetIsActive(false);
		pessimistPlayer->SetIsInputEnabled(false);
		gGui->SelectHero("Optimist");

		// Change the terrain tileset
		auto tiles = pGameMap->GetLayerByName("Background")->AsTiled();
		auto set = tiles->GetTileSet();
		set->SetTexture(pTilesetPessimist);

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
	gGui->SetLife(pPlayer->GetLife(), pPlayer->GetLifeTotal());
	gGui->SetStamina(pPlayer->GetStamina(), pPlayer->GetStaminaTotal());

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

void GameScene::ChangeLevel()
{
	bChangeLevel = true;
}

void GameScene::RemoveLife()
{
}
