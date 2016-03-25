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