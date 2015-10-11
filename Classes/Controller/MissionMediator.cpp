#include "MissionMediator.h"
#include "MissionScene.h"
#include "GameCtrl.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

void MissionMediator::onAddView(MissionScene &scene)
{
    floorSystemCtrl.load(&scene, GameCtrl::instance()->getData().curFloor());
    floorSystemCtrl.start();
    
    auto player = GameCtrl::instance()->getData().curPlayer();
    
    scene.interface->registerPlayer(player->ctrlIndex, [player](KeyCode code) {
        return player->KeyCode2KeyType(code);
    });
    
    //mediator events
    this->eventRegs.push_back(scene.onEnterAfterTransition.registerObserver([](){
        Log("MissionScene after");
    }));
    
    this->eventRegs.push_back(scene.onMenuCmd.registerObserver(
        [&scene](MissionScene::CmdTag tag) {
            switch(tag)
            {
                case MissionScene::CmdTag::QUIT:
                    GameCtrl::instance()->goToMainMenu();
                    break;
                default:
                    Log("unsupported ItemTag in MissionMediator");
                    break;
            }
    }));
    
    //view events
    this->eventRegs.push_back(scene.interface->getStick()->onTrigger.registerObserver(
        [this](cc::Vec2 pos){
            auto player = GameCtrl::instance()->getData().curPlayer();
            this->floorSystemCtrl.getCtrlSystem()->setStickDirection(player->ctrlIndex, pos);
    }));
    
    this->eventRegs.push_back(scene.interface->getStick()->onRelease.registerObserver(
        [this](){
            auto player = GameCtrl::instance()->getData().curPlayer();
            this->floorSystemCtrl.getCtrlSystem()->releaseStick(player->ctrlIndex);
    }));
    
    this->eventRegs.push_back(scene.interface->onKeyPressAction.registerObserver(
        [this](unsigned playerIndex, int flag){
            this->floorSystemCtrl.getCtrlSystem()->setKeyPressAction(playerIndex, flag);
    }));
    
    this->eventRegs.push_back(scene.interface->onKeyReleaseAction.registerObserver(
        [this](unsigned playerIndex, int flag){
            this->floorSystemCtrl.getCtrlSystem()->setKeyReleaseAction(playerIndex, flag);
    }));
    
    this->eventRegs.push_back(scene.interface->onSetActionMode.registerObserver(
        [this](ActionMode mode){
            this->floorSystemCtrl.getCtrlSystem()->setSelectionAction(mode);
    }));
    
    //touch
    auto tListener = cc::EventListenerTouchAllAtOnce::create();
    tListener->onTouchesBegan = [this, &scene](const std::vector<cc::Touch*>& touches,
                                   cocos2d::Event* event) {
        auto player = GameCtrl::instance()->getData().curPlayer();
        auto ctrlSystem = this->floorSystemCtrl.getCtrlSystem();
        
        for(auto touch : touches)
        {
            auto touchPos = touch->getLocation();
            
            if (kCanvasMissionRect.containsPoint(touchPos)) // frame zone
            {
                if (this->cameraID.size() < 2)
                {
                    this->cameraID[touch->getID()] = touchPos;
                    if (this->cameraID.size() > 1)
                    {
                        //multitouch on frame zone detected
                        continue;
                    }
                }

                ctrlSystem->setTouchFrameBegan(player->ctrlIndex,
                        scene.getCam()->getOrigin() - kCanvasMissionRect.origin,
                        touchPos);
            }
        }
    };

    tListener->onTouchesEnded = [this](const std::vector<cc::Touch*>& touches,
                                   cocos2d::Event* event) {
        auto player = GameCtrl::instance()->getData().curPlayer();
        auto ctrlSystem = this->floorSystemCtrl.getCtrlSystem();
        
        for(auto touch : touches)
        {
            //handle joystick
            /*if (lib::hasKey(joyID, player->ctrlIndex) && joyID[player->ctrlIndex] == touch->getID())
            {
                joyID.erase(player->ctrlIndex);
                joyPos.erase(player->ctrlIndex);
            }
            
            //handle action buttons
            if (this->view->interface->getActionBounds().containsPoint(touch->getStartLocation()))
            {
                auto diff = touch->getLocation() - touch->getStartLocation();
                ActionMode doAction = ActionMode::none;
                if (diff.x < -40)
                    doAction = this->view->interface->getPrevAction();
                else if (diff.x > 40)
                    doAction = this->view->interface->getNextAction();
                
                if (doAction != ActionMode::none)
                {
                    actionSelection = doAction;
                }
            }
            */
            
            //handle camera
            cameraID.erase(touch->getID());
        }
    };
    
    tListener->onTouchesMoved = [](const std::vector<cc::Touch*>& touches,
                                   cocos2d::Event* event) {
    };
 
    tListener->onTouchesCancelled = [](const std::vector<cc::Touch*>& touches,
                                       cocos2d::Event* event) {
    };
    
    scene.getEventDispatcher()->addEventListenerWithSceneGraphPriority(tListener, &scene);
}

void MissionMediator::onRemoveView(MissionScene &scene)
{
    scene.getEventDispatcher()->removeAllEventListeners();
}

void MissionMediator::onTick(double dt)
{
    floorSystemCtrl.tick(dt);
}

void MissionMediator::onAnimate(double dt, double tickPercent)
{
    floorSystemCtrl.animate(dt, tickPercent);
}