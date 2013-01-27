#ifndef _GAME_H_
#define _GAME_H_

#include <api/rocket/RocketInterface.h>

#include "../defines.h"
#include "physicsmanager.h"
#include "WorldManager.h"
#include "../states/game_pause_state.h"
#include "../states/game_run_state.h"
#include "PlayerEntity.h"
#include "cameracontroller.h"
#include "SoundManager.h"

using namespace Seed::RocketGui;

extern SceneNode *gScene;
extern PhysicsManager *gPhysics;
extern SoundManager *gSoundManager;
extern WorldManager *gWorldManager;

class GameScene : public IEventInputKeyboardListener,
				  public IEventJobListener,
				  public IRocketEventListener
{
	public:
		GameScene(SceneNode *parent, Camera *mainCamera);
		virtual ~GameScene();

		virtual bool Initialize();
		virtual bool Update(f32 dt);
		virtual bool Shutdown();

		void Pause();
		void Resume();

		// IEventInputKeyboardListener
		virtual void OnInputKeyboardRelease(const EventInputKeyboard *ev);

		// IEventJobListener
		virtual void OnJobCompleted(const EventJob *ev);
		virtual void OnJobAborted(const EventJob *ev);

		// IRocketEventListener
		virtual void OnGuiEvent(Rocket::Core::Event &ev, const Rocket::Core::String &script);

		// Load
		virtual void LoadMapColliders();

	private:
		SEED_DISABLE_COPY(GameScene);

		PlayerEntity	 *pPlayer;
		Camera			 *pCamera;
		CameraController clCamera;
		SceneNode		*pParentScene;
		SceneNode		cScene;
		Music			musTheme;
		GameMap			*pGameMap;
		bool			bPaused;
		bool			bInitialized;

		WorldManager	clWorldManager;
		PhysicsManager	clPhysicsManager;
		SoundManager	clSoundManager;

		// State Machine
		StateMachine		cFlow;
		GameRunState		cRun;
		GamePauseState		cPause;
		StateMachineState	cMenu;
		StateMachineEvent	cOnPause;
		StateMachineEvent	cOnRun;
		StateMachineEvent	cOnMenu;
		StateMachineTransition cRunToPause;
		StateMachineTransition cPauseToRun;
		StateMachineTransition cPauseToMenu;
};

#endif // _GAMEFLOW_H_
