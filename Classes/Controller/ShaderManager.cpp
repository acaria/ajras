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

void ShaderManager::setLightParam(const def::shader::LightParam &param, float value)
{
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
    
    switch(param)
    {
        case def::shader::LightParam::brightness:
            ps->setUniformFloat("u_brightness", value);
            this->lightConfig.brightness = value;
            break;
        case def::shader::LightParam::cutOffRadius:
            ps->setUniformFloat("u_cutoffRadius", std::max(1.0f, value));
            this->lightConfig.cutOffRadius = value;
            break;
        case def::shader::LightParam::halfRadius:
            ps->setUniformFloat("u_halfRadius", std::max(0.01f, value));
            this->lightConfig.halfRadius = value;
            break;
        case def::shader::LightParam::lightColor:
        case def::shader::LightParam::ambiantColor:
            Log("invalid light param: float required");
            break;
        default:
            Log("invalid shader light param");
            break;
    }
}

void ShaderManager::setLightParam(const def::shader::LightParam &param, const cc::Color3B& value)
{
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
    
    switch(param)
    {
        case def::shader::LightParam::ambiantColor:
            ps->setUniformVec3("u_ambientColor", cc::Vec3(value.r, value.g, value.b) / 255.0f);
            this->lightConfig.ambiantColor = value;
            break;
        case def::shader::LightParam::lightColor:
            ps->setUniformVec3("u_lightColor", cc::Vec3(value.r, value.g, value.b) / 255.0f);
            this->lightConfig.lightColor = value;
            break;
        case def::shader::LightParam::brightness:
        case def::shader::LightParam::cutOffRadius:
        case def::shader::LightParam::halfRadius:
            Log("invalid light param: Color3B required");
            break;
        default:
            Log("invalid shader light param");
            break;
    }
}


const def::shader::LightConfig& ShaderManager::getLightConfig()
{
    return lightConfig;
}

void ShaderManager::setLightConfig(const def::shader::LightConfig &config)
{
    this->setLightConfig(config.lightColor,
                         config.ambiantColor,
                         config.brightness,
                         config.cutOffRadius,
                         config.halfRadius);
}

void ShaderManager::setLightConfig(const cc::Color3B &lightColor,
                                   const cc::Color3B &ambiantColor,
                                   float brightness,
                                   float cutOffRadius,
                                   float halfRadius)
{
    this->lightConfig.lightColor = lightColor;
    this->lightConfig.ambiantColor = ambiantColor;
    this->lightConfig.brightness = brightness;
    this->lightConfig.cutOffRadius = cutOffRadius;
    this->lightConfig.halfRadius = halfRadius;

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

    ps->setUniformVec3("u_lightColor",
                       cc::Vec3(lightColor.r, lightColor.g, lightColor.b) / 255.0f);
    ps->setUniformVec3("u_ambientColor",
                       cc::Vec3(ambiantColor.r, ambiantColor.g, ambiantColor.b) / 255.0f);
    ps->setUniformFloat("u_brightness", brightness);
    ps->setUniformFloat("u_cutoffRadius", std::max(1.0f, cutOffRadius));
    ps->setUniformFloat("u_halfRadius", std::max(0.01f, halfRadius));
}