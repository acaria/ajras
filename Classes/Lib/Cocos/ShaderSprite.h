#pragma once

template<class CSprite>
class ShaderSprite : public CSprite
{
    static_assert(std::is_base_of<cc::Sprite, CSprite>::value,
                  "must inherit from Sprite");
    using Base = CSprite;
    
public:
    ShaderSprite(const std::string& shaderName, bool withInputs)
    : shaderName(shaderName), withInputs(withInputs) {}
    
    virtual ~ShaderSprite(){}
    
    virtual void setTexture(cc::Texture2D *texture) override
    {
        Base::setTexture(texture);
        
        auto ps = cc::GLProgramState::getOrCreateWithGLProgram(
            cc::GLProgramCache::getInstance()->getGLProgram(this->shaderName));
        this->setGLProgramState(ps);
    }
    
    virtual void draw(cc::Renderer *renderer,
                      const cc::Mat4 &transform, uint32_t flags) override
    {
        if (withInputs && this->getGLProgramState())
        {
            this->customCmd.init(Base::_globalZOrder);
            this->customCmd.func = CC_CALLBACK_0(ShaderSprite::onDraw, this, transform, flags);
            renderer->addCommand(&this->customCmd);
        }
        
        Base::draw(renderer, transform, flags);
    }
    
    void onDraw(const cc::Mat4 &transform, uint32_t flags)
    {
        this->setCustomUniforms(this->getGLProgramState(), transform, flags);
    }
    
protected:
    virtual void setCustomUniforms(cc::GLProgramState* glState,
                                   const cc::Mat4 &transform,
                                   uint32_t flags) {};
    
private:
    cc::CustomCommand   customCmd;
    bool                withInputs;
    std::string         shaderName;
};


