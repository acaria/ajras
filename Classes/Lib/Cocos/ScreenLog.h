#pragma once

#include <stdio.h>

#include <string>
#include <vector>

#define LL_FATAL    0x01
#define LL_ERROR    0x02
#define LL_WARNING  0x04
#define LL_INFO     0x08
#define LL_DEBUG    0x10
#define LL_TRACE    0x20

#define SCREENLOG_NUM_LINES             45
#define SCREENLOG_START_HEIGHT_PERCENT  0.001
#define SCREENLOG_PRINT_BUFFER_SIZE     8192
#define SCREENLOG_LAYER_LEVEL           1000

namespace lib
{

class screenLogMessage {
    friend class ScreenLog;
private:
    class ScreenLog* m_layer;
    int m_level;
    std::string m_text;
    double m_timestamp;
    cocos2d::Label* m_label;
    bool m_dirty;
    
private:
    screenLogMessage(ScreenLog* layer) {
        m_layer = layer;
        m_label = NULL;
        m_level = 0;
        m_timestamp = 0;
        m_dirty = true;
    }
    virtual ~screenLogMessage() { }
    
    void setLabelText(std::string msg);
    void createLabel();
    bool checkLabel();
};

class ScreenLog : public cocos2d::Layer
{
    friend class screenLogMessage;
private:
    std::string m_fontFile;
    int m_level;
    float m_timeout;
    std::vector<screenLogMessage*> m_messages;
    pthread_mutex_t m_contentMutex;
    
protected:
    char printBuffer[SCREENLOG_PRINT_BUFFER_SIZE];
    
public:
    
    ScreenLog();
    ~ScreenLog();
    
    void setFontFile(std::string file);
    void setLevelMask(int level);
    void setTimeoutSeconds(float seconds);
    void attachToScene(cocos2d::Scene* scene);
    
    screenLogMessage* log(int level, const char* msg, ...);
    bool setMessageText(screenLogMessage* slm, const char *p_str, ...);
    
    void update(float dt);
    void moveLabelsUp(int maxIndex);
    void reorderLabels();
    void clearEntries();
};

extern ScreenLog* g_screenLog;

class ScopeLock {
public:
    pthread_mutex_t* m_mutex;
    ScopeLock(pthread_mutex_t* m) {
        m_mutex = m;
        pthread_mutex_lock(m_mutex);
    }
    ~ScopeLock() {
        pthread_mutex_unlock(m_mutex);
    }
};

class ScopeLog {
public:
    std::string m_functionName;
    ScopeLog(std::string fn) {
        m_functionName = fn;
        g_screenLog->log(LL_TRACE, "Entered %s", m_functionName.c_str());
    }
    ~ScopeLog() {
        g_screenLog->log(LL_TRACE, "Exiting %s", m_functionName.c_str());
    }
};

}