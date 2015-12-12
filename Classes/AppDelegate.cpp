#include "AppDelegate.h"
#include "GameCtrl.h"

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    cc::GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = cc::Director::getInstance();
    auto glview = director->getOpenGLView();
    
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = cc::GLViewImpl::createWithRect("ajras", cc::Rect(0,0,1024, 768));
#else
        glview = cc::GLViewImpl::create("ajras");
#endif
        director->setOpenGLView(glview);
    }
    
    //retrieve device size
    cc::Size frameSize = glview->getFrameSize();

    //define design size
    glview->setDesignResolutionSize(def::designResolutionSize.width,
                                    def::designResolutionSize.height,
                                    ResolutionPolicy::NO_BORDER);
    
    std::vector<std::string> searchPaths;
    
    //define resource size
    cc::Size resourceSize;
    for(auto resource : def::resourceList)
    {
        if (frameSize.height > resource.select.height)
        {
            searchPaths.push_back(resource.directory);
            resourceSize = resource.size;
            break;
        }
    }
    director->setContentScaleFactor(resourceSize.height / def::designResolutionSize.height);
    cc::FileUtils::getInstance()->setSearchPaths(searchPaths);

    Log("\n{\n\tscreenSize: %.1f %.1f\n\tresourceSize: %.1f %.1f\n\tdesignSize: %.1f %.1f\n\tscale factor: %.1f\n\tframeRect: %.1f,%.1f,%.1f,%.1f}\n",
          frameSize.width, frameSize.height,
          resourceSize.width, resourceSize.height,
          def::designResolutionSize.width, def::designResolutionSize.height,
          director->getContentScaleFactor(),
          glview->getVisibleOrigin().x, glview->getVisibleOrigin().y, glview->getVisibleSize().width, glview->getVisibleSize().height);
    
    director->setDisplayStats(def::showFPS);
    director->setAnimationInterval(def::idealInterval);
    director->setProjection(cc::Director::Projection::_2D);
    
    GameCtrl::instance()->start();

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    cc::Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    cc::Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
