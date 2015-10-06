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
    /*
    
     // initialize director
     auto director = Director::getInstance();
     auto glview = director->getOpenGLView();
     if(!glview) {
     #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
     glview = GLViewImpl::createWithRect("ajras", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
     #else
     glview = GLViewImpl::create("ajras");
     #endif
     director->setOpenGLView(glview);
     }
     
     // turn on display FPS
     director->setDisplayStats(true);
     
     // set FPS. the default value is 1.0/60 if you don't call this
     director->setAnimationInterval(1.0 / 60);
     
     // Set the design resolution
     glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
     Size frameSize = glview->getFrameSize();
     // if the frame's height is larger than the height of medium size.
     if (frameSize.height > mediumResolutionSize.height)
     {
     director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
     }
     // if the frame's height is larger than the height of small size.
     else if (frameSize.height > smallResolutionSize.height)
     {
     director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
     }
     // if the frame's height is smaller than the height of medium size.
     else
     {
     director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
     }
     
     register_all_packages();
     
     // create a scene. it's an autorelease object
     auto scene = HelloWorld::createScene();
     
     // run
     director->runWithScene(scene);
     
     return true;

    */
    
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
    
    glview->setDesignResolutionSize(designSize.width,
                                    designSize.height,
                                    ResolutionPolicy::SHOW_ALL);

    CCLOG("\n{\n\tscreenSize: %.1f %.1f\n\tresourceSize: %.1f %.1f\n\tdesignSize: %.1f %.1f\n\tscale factor: %.1f\n}\n",
          screenSize.width, screenSize.height,
          resourceSize.width, resourceSize.height,
          designSize.width, designSize.height,
          resourceSize.width / designSize.width);
    
    //glClearColor(143 / 255.0f,86 / 255.0f, 59 / 255.0f, 1.0f);
    
    director->setDisplayStats(kShowFPS);
    director->setAnimationInterval(kIdealInterval);
    director->setProjection(Director::Projection::_2D);
    
    GameCtrl::instance()->start();

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
