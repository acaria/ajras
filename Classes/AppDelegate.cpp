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

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    
    if(!glview) {
        glview = GLViewImpl::create("ajras");
        director->setOpenGLView(glview);
    }

    Size designSize = Size(960, 640);
    Size resourceSize = Size();
    Size screenSize = glview->getFrameSize();
    
    std::vector<std::string> searchPaths;
    
    if (screenSize.height >= 640)
    {
        searchPaths.push_back("hd");
        searchPaths.push_back("sd");
        
        resourceSize = Size(960, 640);
    }
    else
    {
        if (screenSize.height > 320)
        {
            searchPaths.push_back("hd");
            searchPaths.push_back("sd");
            resourceSize = Size(960, 640);
        }
        else
        {
            searchPaths.push_back("sd");
            resourceSize = Size(480, 320);
        }
    }
    
    FileUtils::getInstance()->setSearchPaths(searchPaths);
    director->setContentScaleFactor(resourceSize.width / designSize.width);
    
    glview->setDesignResolutionSize(
                                    designSize.width,
                                    designSize.height,
                                    ResolutionPolicy::SHOW_ALL);


    CCLOG("\n{\n\tscreenSize: %.1f %.1f\n\tresourceSize: %.1f %.1f\n\tdesignSize: %.1f %.1f\n\tscale factor: %.1f\n}\n",
          screenSize.width, screenSize.height,
          resourceSize.width, resourceSize.height,
          designSize.width, designSize.height,
          resourceSize.width / designSize.width);
    
    director->setDisplayStats(kShowFPS);
    director->setAnimationInterval(kIdealInterval);
    director->setProjection(Director::Projection::_2D);
    
    GameCtrl::instance()->newSession();

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
