SOURCES += source/main.cpp \
	source/gameflow.cpp \
	source/controller/cameracontroller.cpp \
	source/data/gamedata.cpp \
	source/entity/playerentity.cpp \
	source/entity/entityfactory.cpp \
	source/entity/entity.cpp \
	source/entity/spriteentity.cpp \
	source/entity/itementity.cpp \
	source/entity/heartentity.cpp \
	source/entity/triggerentity.cpp \
	source/entity/deathentity.cpp \
	source/entity/teleportentity.cpp \
	source/manager/guimanager.cpp \
	source/manager/worldmanager.cpp \
	source/manager/physicsmanager.cpp \
	source/manager/soundmanager.cpp \
	source/scene/gamescene.cpp \
	source/states/credits_state.cpp \
	source/states/mainmenu_state.cpp \
	source/states/options_state.cpp \
	source/states/game_state.cpp \
	source/states/gameover_state.cpp \
	source/states/game/run_state.cpp \
	source/states/game/pause_state.cpp \
	source/util/triggercounter.cpp \
	source/util/collisionsensor.cpp \
	source/entity/optimistplayerentity.cpp \
	source/entity/pessimistplayerentity.cpp \
	source/entity/realistplayerentity.cpp \
	source/entity/enemyentity.cpp \
	source/entity/healthpotionentity.cpp \
	source/entity/staminapotionentity.cpp \
	source/entity/goldentity.cpp

HEADERS += \
	source/defines.h \
	source/gameflow.h \
	source/controller/cameracontroller.h \
	source/data/gamedata.h \
	source/entity/playerentity.h \
	source/entity/entityfactory.h \
	source/entity/entity.h \
	source/entity/spriteentity.h \
	source/entity/itementity.h \
	source/entity/heartentity.h \
	source/entity/triggerentity.h \
	source/entity/deathentity.h \
	source/entity/teleportentity.h \
	source/manager/guimanager.h \
	source/manager/worldmanager.h \
	source/manager/physicsmanager.h \
	source/manager/soundmanager.h \
	source/scene/gamescene.h \
	source/states/mainmenu_state.h \
	source/states/credits_state.h \
	source/states/options_state.h \
	source/states/game_state.h \
	source/states/gameover_state.h \
	source/states/game/run_state.h \
	source/states/game/pause_state.h \
	source/util/sounds.h \
	source/util/triggercounter.h \
	source/util/collisionsensor.h \
	source/entity/optimistplayerentity.h \
	source/entity/pessimistplayerentity.h \
	source/entity/realistplayerentity.h \
	source/entity/enemyentity.h \
	source/entity/healthpotionentity.h \
	source/entity/staminapotionentity.h \
	source/entity/goldentity.h

OTHER_FILES_CONFIG = \
#Configs
	resources/configs/app.config \
	resources/configs/game.config

OTHER_FILES_FONT = \
#Fonts
	resources/fonts/Delicious-Roman.otf \
	resources/fonts/Delicious-Italic.otf \
	resources/fonts/Delicious-Bold.otf \
	resources/fonts/Delicious-BoldItalic.otf

OTHER_FILES_SCENE = \
#Scenes
	resources/scenes/empty.scene \
	resources/scenes/dungeon.scene \
	resources/scenes/dungeon2.scene \
	resources/scenes/dungeon3.scene

OTHER_FILES_TEXTURE = \
#Textures
	resources/textures/rocket_invader.png \
	resources/textures/realist_player_idle_frame1.png \
	resources/textures/realist_player_run_frame2.png \
	resources/textures/realist_player_run_frame1.png \
	resources/textures/pessimist_player_idle_frame1.png \
	resources/textures/pessimist_player_run_frame2.png \
	resources/textures/pessimist_player_run_frame1.png \
	resources/textures/optimist_player_idle_frame1.png \
	resources/textures/optimist_player_run_frame2.png \
	resources/textures/optimist_player_run_frame1.png \
	resources/textures/heart.png \
	resources/textures/menu_background.png \
	resources/textures/background_gameover.png \
	resources/textures/death_idle_frame1.png \
	resources/textures/optimist_ground_tileset.png \
	resources/textures/realist_ground_tileset.png \
	resources/textures/pessimist_ground_tileset.png \
	resources/textures/optimist_enemy1_idle_frame1.png \
	resources/textures/health_potion.png \
	resources/textures/stamina_potion.png \
	resources/textures/gold.png \
	resources/textures/ballon_optimist.png \
	resources/textures/ballon_pessimist.png \
	resources/textures/ballon_realist.png \
	resources/textures/fog_tileset.png \
	resources/textures/pessimist_enemy_tileset.png \
	resources/textures/realist_enemy_tileset.png \
	resources/textures/optimist_enemy_tileset.png \

#Gui
OTHER_FILES_GUI_STYLE = \
	#Styles
	resources/gui/styles/common.rcss \
	resources/gui/styles/window.rcss \
	resources/gui/styles/window2.rcss \
	resources/gui/styles/game.rcss

OTHER_FILES_GUI_VIEW = \
	#Views
	resources/gui/views/mainmenu.rml \
	resources/gui/views/window.rml \
	resources/gui/views/gameplay.rml \
	resources/gui/views/credits.rml \
	resources/gui/views/options.rml \
	resources/gui/views/game.rml \
	resources/gui/views/gamepause.rml \
	resources/gui/views/gameover.rml

OTHER_FILES_SOUND = \
#Sounds
	resources/sounds/optimist_theme.ogg \
	resources/sounds/pessimist_theme.ogg \
	resources/sounds/realist_theme.ogg \
	resources/sounds/sfx.ogg \
	resources/sounds/wakeup.ogg \
	resources/sounds/powerup.ogg \
	resources/sounds/Pickup_Coin3.ogg \
	resources/sounds/damage.ogg \
	resources/sounds/scottwills_time.ogg \
	resources/sounds/metal_game_over.ogg \
	resources/sounds/jump.ogg \
	resources/sounds/teleport.ogg

#Maps
OTHER_FILES_MAPS = \
	resources/dungeon.json \
	resources/dungeon2.json \
	resources/dungeon3.json

OTHER_FILES += $${OTHER_FILES_CONFIG} \
	$${OTHER_FILES_FONT} \
	$${OTHER_FILES_SCENE} \
	$${OTHER_FILES_TEXTURE} \
	$${OTHER_FILES_GUI_STYLE} \
	$${OTHER_FILES_GUI_VIEW} \
	$${OTHER_FILES_SOUND} \
	$${OTHER_FILES_MAPS} \

include("app.pri")
