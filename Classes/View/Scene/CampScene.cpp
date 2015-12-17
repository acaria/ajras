#include "CampScene.h"
#include "Defines.h"

CampScene::~CampScene()
{
    if (this->camera != nullptr)
        delete this->camera;
}

bool CampScene::init()
{
    if (!Base::init())
        return false;
    
    auto pOrigin = cc::Director::getInstance()->getVisibleOrigin();
    auto pSize = cc::Director::getInstance()->getVisibleSize();
    
    auto margin = def::canvasCampMargin;
    cc::Rect canvasRect = {pOrigin.x + margin.x,
                           pOrigin.y + margin.w,
                           pSize.width - margin.x - margin.y,
                           pSize.height - margin.w - margin.z};
    
    auto back = cc::LayerColor::create(cc::Color4B(143,86,59,255));
    this->addChild(back);
    
    this->canvas = cc::ui::Layout::create();
    this->canvas->setBackGroundColorType(cc::ui::HBox::BackGroundColorType::SOLID);
    this->canvas->setBackGroundColor(back->getColor());
    this->canvas->setLayoutType(cc::ui::Layout::Type::RELATIVE);
    this->canvas->setClippingEnabled(true);
    this->canvas->setPosition(canvasRect.origin);
    this->canvas->setSize(canvasRect.size);
    this->addChild(canvas);
    
    this->frame = cc::create<BufferedFrame>(canvasRect);
    this->frame->setActive(def::postProcessing);
    this->canvas->addChild(this->frame);
    
    this->interface = cc::create<CampInterface>();
    this->addChild(interface);

    this->camera = new GameCamera(this->frame, canvasRect);
    
    auto listener = cc::EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](cc::Touch* touch, cc::Event* event){
        return true;
    };
    
    listener->onTouchEnded = [this](cc::Touch* touch, cc::Event* event){
        if (touch->getLocation().x < 15 && touch->getLocation().y > 625)
        {
            this->onMenuCmd(CmdTag::QUIT);
            return true;
        }
        return false;
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void CampScene::setBgColor(cc::Color3B bgColor)
{
    this->canvas->setBackGroundColor(bgColor);
}

cc::Node* CampScene::getFrame()
{
    return this->frame;
}

GameCamera* CampScene::getCam()
{
    return this->camera;
}