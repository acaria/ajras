#pragma once

#include "cocos2d.h"
#include "ui/CocosGui.h"
#include "Constants.h"

//Lib
#include "Dir.h"
#include "CsActionInterval.h"
#include "SweptAABB.h"
#include "V2.h"
#include "Any.h"
#include "Callback.h"
#include "Event.h"
#include "ScreenLog.h"
#include "Singleton.h"
#include "Jsonxx.h"
#include "ECS.h"
#include "ECSGroup.h"
#include "Misc.h"
#include "DataGrid.h"

//Model
#include "AnimationData.h"
#include "ProfileData.h"
#include "ProfileModel.h"
#include "BehaviourNodes.h"
#include "BehaviourData.h"
#include "BehaviourModel.h"
#include "BlockInfo.h"
#include "ObjectInfo.h"
#include "GateInfo.h"
#include "RoomShape.h"
#include "MapShape.h"
#include "RoomModel.h"
#include "RoomData.h"
#include "MapData.h"

//System
#include "InputComponent.h"
#include "AIComponent.h"
#include "CategoryComponent.h"
#include "RenderComponent.h"
#include "GateComponent.h"
#include "WarpComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "PositionComponent.h"
#include "MeleeComponent.h"
#include "OrientationComponent.h"
#include "VelocityComponent.h"
#include "Components.h"
#include "BaseTickSystem.h"
#include "SysHelper.h"
#include "MoveSystem.h"
#include "CollisionSystem.h"
#include "RenderSystem.h"
#include "ControlSystem.h"
#include "TargetSystem.h"
#include "InputSystem.h"
#include "TransitSystem.h"
#include "AISystem.h"
#include "MeleeSystem.h"

//View
#include "TickScheduler.h"
#include "InterfaceLayer.h"
#include "EventScene.h"
#include "BaseScene.h"
#include "MainMenuScene.h"
#include "GameScene.h"
#include "RoomLayer.h"

//Controller
#include "TickCtrl.h"
#include "BaseMediator.h"
#include "BaseSceneMediator.h"
#include "MainMenuMediator.h"
#include "GameMediator.h"
#include "SceneManager.h"
#include "RoomSystemCtrl.h"
#include "FloorSystemCtrl.h"
#include "GameCtrl.h"

#include "AppDelegate.h"

