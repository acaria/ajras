#include "ShaderManager.h"

void ShaderManager::load()
{
    std::list<std::tuple<std::string, std::string, std::string>> list = {
        {"test",        "shaders/test.vsh",         "shaders/test.fsh"},
        {"blend",       "shaders/pass.vsh",         "shaders/blend.fsh"},
        {"greyscale",   "shaders/pass.vsh",         "shaders/greyscale.fsh"},
        {"light",       "shaders/pass.vsh",         "shaders/light.fsh"},
        {"bump",        "shaders/pass.vsh",         "shaders/bump.fsh"}
    };
    auto fu = cc::FileUtils::getInstance();
    
    for (auto& t : list)
    {
        auto vStr = fu->getStringFromFile(fu->fullPathForFilename(std::get<1>(t)));
        GLchar* vSrc = (GLchar*)vStr.c_str();
        
        auto fStr = fu->getStringFromFile(fu->fullPathForFilename(std::get<2>(t)));
        GLchar* fSrc = (GLchar*)fStr.c_str();
        
        auto program = cc::GLProgram::createWithByteArrays(vSrc, fSrc);
        if (program != nullptr)
        {
            cc::GLProgramCache::getInstance()->addGLProgram(program, std::get<0>(t));
            Log("preload shader: %s", std::get<0>(t).c_str());
        }
    }
}

void ShaderManager::initLightConfig(const LightConfig &config)
{
    this->lightConfig = config;
    
    auto gls = cc::GLProgramCache::getInstance()->getGLProgram("light");
    if (gls == nullptr)
    {
        Log("invalid gl program for light config");
        return;
    }
    
    auto ps = cc::GLProgramState::getOrCreateWithGLProgram(gls);
    if (ps == nullptr)
    {
        Log("invalid gl state for light config");
        return;
    }
    
    auto ambiantColor = config.ambiantColor;
    ps->setUniformVec3("u_ambientColor",
                       cc::Vec3(ambiantColor.r, ambiantColor.g, ambiantColor.b) / 255.0f);
}