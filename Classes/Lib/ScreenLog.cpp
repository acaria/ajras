#include "ScreenLog.h"
#include "Any.h"

double getTimeMillis() {
    timeval time;
    gettimeofday(&time, NULL);
    return (double)(time.tv_sec * 1000 + time.tv_usec / 1000);
}

ScreenLog::ScreenLog()
{
    pthread_mutexattr_t Attr;
    pthread_mutexattr_init(&Attr);
    pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
    
    pthread_mutex_init(&m_contentMutex, &Attr);
    
    m_timeout = 5000;//ms
    m_level = 0;
    
    cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(this, 10000, false);
}

ScreenLog::~ScreenLog()
{
    cocos2d::Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    
    {
        ScopeLock lock(&m_contentMutex);
        for (unsigned int i = 0; i < m_messages.size(); i++)
            delete m_messages[i];
    }
    
    pthread_mutex_destroy(&m_contentMutex);
}

void ScreenLog::setFontFile(std::string file)
{
    m_fontFile = file;
}

void ScreenLog::setLevelMask(int p_level)
{
    m_level = p_level;
}

void ScreenLog::setTimeoutSeconds(float t)
{
    m_timeout = t * 1000;
}

void ScreenLog::attachToScene(cocos2d::Scene* scene)
{
    if ( getParent() )
        getParent()->removeChild(this,false);
    if ( scene )
        scene->addChild(this,SCREENLOG_LAYER_LEVEL);
}

screenLogMessage* ScreenLog::log(int p_level, const char *p_str, ...)
{
    ScopeLock lock(&m_contentMutex);
    
    if (! p_str )
        return NULL;
    
    if (!(p_level & m_level))
        return NULL;
    
    va_list t_va;
    va_start (t_va, p_str);
    vsnprintf(this->printBuffer, SCREENLOG_PRINT_BUFFER_SIZE - 1, p_str, t_va);
    va_end (t_va);
    
    screenLogMessage *slm = new screenLogMessage(this);
    slm->m_level = p_level;
    slm->m_text = this->printBuffer;
    slm->m_timestamp = getTimeMillis();
    m_messages.push_back(slm);
    
    return slm;
}

void ScreenLog::setMessageText(screenLogMessage *slm, const char *p_str, ...)
{
    ScopeLock lock(&m_contentMutex);
    
    //loop through to find matching message, in case it has already gone
    bool messageStillExists = false;
    for (int i = 0; i < m_messages.size(); i++) {
        if (m_messages[i] == slm) {
            messageStillExists = true;
            break;
        }
    }
    if (!messageStillExists )
        return;
    
    va_list t_va;
    va_start (t_va, p_str);
    vsnprintf(this->printBuffer, SCREENLOG_PRINT_BUFFER_SIZE - 1, p_str, t_va);
    va_end (t_va);
    
    slm->setLabelText(this->printBuffer);
    slm->m_timestamp = getTimeMillis();
}

void ScreenLog::update(float dt)
{
    ScopeLock lock(&m_contentMutex);
    
    for (int i = 0; i < m_messages.size(); i++) {
        screenLogMessage* slm = m_messages[i];
        if ( slm->checkLabel() )
            moveLabelsUp(i);
    }
    
    double now = getTimeMillis();
    int c = 0;
    for (int i = m_messages.size()-1; i >= 0; i--) {
        screenLogMessage *slm = m_messages[i];
        if (now - slm->m_timestamp > m_timeout || c > (2*SCREENLOG_NUM_LINES)) {
            removeChild(slm->m_label,true);
            delete slm;
            m_messages.erase( m_messages.begin() + i );
        }
        c++;
    }
}

void ScreenLog::moveLabelsUp(int maxIndex)
{
    ScopeLock lock(&m_contentMutex);
    
    float screenHeightPixels = cocos2d::Director::getInstance()->getWinSize().height;
    float fontSize =  screenHeightPixels / (float)SCREENLOG_NUM_LINES - 1;
    
    if ( maxIndex >= m_messages.size() )
        maxIndex = m_messages.size();
    
    for (int i = 0; i < maxIndex; i++) {
        screenLogMessage* slm = m_messages[i];
        cocos2d::Point p = slm->m_label->getPosition();
        p.y += fontSize;
        slm->m_label->setPosition( p );
    }
}

void ScreenLog::clearEntries()
{
    ScopeLock lock(&m_contentMutex);
    
    for (unsigned int i = 0; i < m_messages.size(); i++)
        delete m_messages[i];
    m_messages.clear();
}

void screenLogMessage::setLabelText(std::string msg)
{
    // can be called from other threads, delay label creation to main thread to make sure OpenGL works
    ScopeLock lock(&m_layer->m_contentMutex);
    
    m_text = msg;
    m_dirty = true;
}

void screenLogMessage::createLabel()
{
    float screenHeightPixels = cocos2d::Director::getInstance()->getWinSize().height;
    float fontSize =  screenHeightPixels / (float)SCREENLOG_NUM_LINES - 1;
    m_label = cocos2d::Label::createWithSystemFont(m_text.c_str(), m_layer->m_fontFile, fontSize);
    m_label->setAnchorPoint(cocos2d::Point(0,0));
    
    switch ( m_level ) {
        case LL_TRACE:
        case LL_DEBUG:   m_label->setColor({255, 255, 255}); break; // white
        case LL_INFO:    m_label->setColor({32, 255, 32}); break; // green
        case LL_WARNING: m_label->setColor({255, 127, 32}); break; // orange
        default:         m_label->setColor({255, 32, 32}); break; // red
    }
    
    m_label->setPosition(2, SCREENLOG_START_HEIGHT_PERCENT * screenHeightPixels );
    m_layer->addChild(m_label);
}

//returns true if label was created for the first time (other labels should be moved upward)
bool screenLogMessage::checkLabel()
{
    if ( !m_label ) {
        createLabel();
        m_dirty = false;
        return true;
    }
    
    if (m_dirty) {
        cocos2d::Point originalPos = m_label->getPosition();
        m_layer->removeChild(m_label,true);
        m_label = NULL;
        createLabel();
        m_label->setPosition(originalPos);
        m_dirty = false;
    }
    
    return false;
}

