TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG += glfw

INCLUDEPATH += ../seed/include ../seed/contrib

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
	source/entity/hostageentity.cpp \
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
	source/util/collisionsensor.cpp

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
	source/entity/hostageentity.h \
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
	source/util/collisionsensor.h

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
	resources/scenes/game_level1.scene \
	resources/scenes/game_level4.scene \
	resources/scenes/game_level2.scene \
	resources/scenes/game_level3.scene \
	resources/scenes/game_level5.scene

OTHER_FILES_TEXTURE = \
#Textures
	resources/textures/rocket_invader.png \
	resources/textures/player_idle_frame2.png \
	resources/textures/player_idle_frame1.png \
	resources/textures/player_run_frame8.png \
	resources/textures/player_run_frame7.png \
	resources/textures/player_run_frame6.png \
	resources/textures/player_run_frame5.png \
	resources/textures/player_run_frame4.png \
	resources/textures/player_run_frame3.png \
	resources/textures/player_run_frame2.png \
	resources/textures/player_run_frame1.png \
	resources/textures/player_jump_frame4.png \
	resources/textures/player_jump_frame3.png \
	resources/textures/player_jump_frame2.png \
	resources/textures/player_jump_frame1.png \
	resources/textures/heart.png \
	resources/textures/player_land_frame3.png \
	resources/textures/player_land_frame2.png \
	resources/textures/player_land_frame1.png \
	resources/textures/death_idle_frame2.png \
	resources/textures/death_idle_frame1.png \
	resources/textures/hostage_idle_frame1.png \
	resources/textures/menu_background.png \
	resources/textures/hostage_death_frame5.png \
	resources/textures/hostage_death_frame4.png \
	resources/textures/hostage_death_frame3.png \
	resources/textures/hostage_death_frame2.png \
	resources/textures/hostage_death_frame1.png \
	resources/textures/background_gameover.png \
	resources/textures/village_tileset.png \
	resources/textures/instructions.png

#Gui
OTHER_FILES_GUI_STYLE = \
	#Styles
	resources/gui/styles/common.rcss \
	resources/gui/styles/window.rcss \
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
	resources/sounds/sfx.ogg \
	resources/sounds/theme.ogg \
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
	resources/level5.json \
	resources/level4.json \
	resources/level3.json \
	resources/level2.json \
	resources/level1.json

OTHER_FILES += $${OTHER_FILES_CONFIG} \
	$${OTHER_FILES_FONT} \
	$${OTHER_FILES_SCENE} \
	$${OTHER_FILES_TEXTURE} \
	$${OTHER_FILES_GUI_STYLE} \
	$${OTHER_FILES_GUI_VIEW} \
	$${OTHER_FILES_SOUND} \
	$${OTHER_FILES_MAPS}

CONFIG(debug, debug|release) {
	DESTDIR = bin
	DEFINES += DEBUG
	LIBS += -L../seed/lib/debug
} else {
	DESTDIR = bin
	DEFINES += RELEASE
	LIBS += -L../seed/lib/release
}

unix:!macx {
	DEFINES += LINUX
	LIBS += -lseed -lseedcontrib -lGL -lopenal
	QMAKE_CXXFLAGS += -std=c++0x

	sdl {
		LIBS += -lSDL -lSDL_image
	}
}

macx {
	DEFINES += LINUX
	INCLUDEPATH += ../seed/contrib/osx/
	LIBS += -lseed -lseedcontrib -framework OpenAL -framework OpenGL -framework Cocoa -framework IOKit
	CONFIG -= sdl
	CONFIG += glfw

	#Configs
	APP_CONFIG_FILES.files = $$OTHER_FILES_CONFIG
	APP_CONFIG_FILES.path = Contents/Resources/configs
	#Fonts
	APP_FONT_FILES.files = $$OTHER_FILES_FONT
	APP_FONT_FILES.path = Contents/Resources/fonts
	#Scenes
	APP_SCENE_FILES.files = $$OTHER_FILES_SCENE
	APP_SCENE_FILES.path = Contents/Resources/scenes
	#Textures
	APP_TEXTURE_FILES.files = $$OTHER_FILES_TEXTURE
	APP_TEXTURE_FILES.path = Contents/Resources/textures
	#Gui
		#Styles
		APP_GUI_STYLE_FILES.files = $$OTHER_FILES_GUI_STYLE
		APP_GUI_STYLE_FILES.path = Contents/Resources/gui/styles
		#Views
		APP_GUI_VIEW_FILES.files = $$OTHER_FILES_GUI_VIEW
		APP_GUI_VIEW_FILES.path = Contents/Resources/gui/views
	#Sounds
	APP_SOUND_FILES.files = $$OTHER_FILES_SOUND
	APP_SOUND_FILES.path = Contents/Resources/sounds
	#Maps
	APP_MAPS_FILES.files = $$OTHER_FILES_MAPS
	APP_MAPS_FILES.path = Contents/Resources/

	QMAKE_BUNDLE_DATA += APP_CONFIG_FILES APP_FONT_FILES APP_SCENE_FILES \
			APP_TEXTURE_FILES APP_GUI_STYLE_FILES APP_GUI_VIEW_FILES \
			APP_SOUND_FILES APP_MAPS_FILES
}

win32 {
	LIBS += -L../seed/contrib/windows/ -lseed -lseedcontrib -mwindows -lmingw32 -lopengl32 -lopenal32
	INCLUDEPATH += ../seed/contrib/windows/
	CONFIG -= glfw
	CONFIG += sdl
	sdl {
		DEFINES += WIN32 main=SDL_main
		LIBS += -lSDLmain -lSDL -lSDL_image -lgdi32
	}
}

glfw {
	DEFINES += BUILD_GLFW
} else:sdl {
	DEFINES += BUILD_SDL
}
