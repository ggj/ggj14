#ifndef _GAME_H_
#define _GAME_H_

#include <api/rocket/RocketInterface.h>

#include "../defines.h"
#include "../manager/physicsmanager.h"
#include "../manager/worldmanager.h"
#include "../states/game/pause_state.h"
#include "../states/gameover_state.h"
#include "../states/game/run_state.h"
#include "../entity/playerentity.h"
#include "../entity/optimistplayerentity.h"
#include "../entity/realistplayerentity.h"
#include "../entity/pessimistplayerentity.h"
#include "../controller/cameracontroller.h"
#include "../manager/soundmanager.h"

using namespace Seed::RocketGui;

class GameScene;

extern SceneNode *gScene;
extern PhysicsManager *gPhysics;
extern SoundManager *gSoundManager;
extern WorldManager *gWorldManager;
extern GameScene *gGameScene;

class GameScene : public IEventInputKeyboardListener,
				  public IRocketEventListener
{
	SEED_DISABLE_COPY(GameScene)

	public:
		GameScene(SceneNode *parent, Camera *mainCamera, const String &sceneFile);
		virtual ~GameScene();

		virtual bool Initialize();
		virtual bool Update(f32 dt);
		virtual bool Shutdown();

		void Pause();
		void Resume();

		void FogReveal(const vec3 &pos, u32 radius);

		void OnJobCompleted(FileLoader *job);
		void OnJobAborted();

		// IEventInputKeyboardListener
		virtual bool OnInputKeyboardRelease(const EventInputKeyboard *ev) override;

		// IRocketEventListener
		virtual void OnGuiEvent(Rocket::Core::Event &ev, const Rocket::Core::String &script) override;

		// Load
		virtual void LoadMapColliders();

		void RemoveLife();
		void ChangePlayer(const String currentPlayer);
		void ChangeLevel();

		void UseKey(u32 key);

	private:
		PlayerEntity	 *pPlayer;
		PlayerEntity	 *pPlayerRealist;
		PlayerEntity	 *pPlayerPessimist;
		PlayerEntity	 *pPlayerOptimist;
		Camera			 *pCamera;
		CameraController clCamera;
		SceneNode		*pParentScene;
		SceneNode		cScene;
		Music			musThemeRealist;
		Music			musThemePessimist;
		Music			musThemeOptimist;
		Music			*musCur;
		GameMap			*pGameMap;
		GameMap			*pFogMap;
		MapLayerTiled	*pFog;
		u32				iTileSize;
		Texture			*pTilesetOptimist;
		Texture			*pTilesetPessimist;
		Texture			*pTilesetRealist;

		WorldManager	clWorldManager;
		PhysicsManager	clPhysicsManager;
		SoundManager	clSoundManager;

		// State Machine
		StateMachine		cFlow;

		// State Machine State
		RunState			cRun;
		PauseState			cPause;
		GameOverState		cGameOver;
		StateMachineState	cMenu;

		// State Machine Event
		StateMachineEvent	cOnPause;
		StateMachineEvent	cOnRun;
		StateMachineEvent	cOnMenu;
		StateMachineEvent	cOnGameOver;

		// State Machine Transition
		StateMachineTransition cRunToPause;
		StateMachineTransition cPauseToRun;
		StateMachineTransition cPauseToMenu;
		StateMachineTransition cRunToGameOver;
		StateMachineTransition cGameOverToMenu;

		String sSceneFile;
		String strNextLevel;

		f32 fTimeToNextLevel;

		Image *pGameOverImg;

		// Lerp camera
		vec3	vCameraFrom;
		vec3	vCameraCurrent;
		vec3	vCameraTo;
		f32		fElapsed;
		bool	bRequiredKeys[3];
		bool	bPaused;
		bool	bInitialized;
		bool	bMoveCamera;
		bool	bChangeLevel;
};

#endif // _GAMEFLOW_H_
