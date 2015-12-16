#pragma once

template<class CSprite>
class ShaderLayer : public cc::Layer
{
    static_assert(std::is_base_of<cc::Sprite, CSprite>::value,
                  "must inherit from Sprite");
    using Base = cc::Layer;
    
public:
    ~ShaderLayer() {}
    
    virtual bool init() override
    {
        return false;
    }
    
    virtual bool init(const cc::Size size)
    {
        if (!Base::init())
            return false;
        
        this->renderTexture = cc::RenderTexture::create(size.width, size.height);
        this->addChild(renderTexture);
        
        this->renderSprite = new CSprite();
        this->renderSprite->setTexture(renderTexture->getSprite()->getTexture());
        
        assert(this->renderSprite->getGLProgramState() != nullptr);
        
        this->renderSprite->setTextureRect({0, 0,
            renderSprite->getTexture()->getContentSize().width,
            renderSprite->getTexture()->getContentSize().height});
        this->renderSprite->setPosition(cc::Point::ZERO);
        this->renderSprite->setAnchorPoint(cc::Point::ZERO);
        this->renderSprite->setFlippedY(true);
        this->renderSprite->setFlippedX(true);
        this->addChild(renderSprite);
        
        return true;
    }
    
    
    virtual void visit(cc::Renderer *renderer,
                       const cc::Mat4& parentTransform,
                       uint32_t parentFlags) override
    {
        renderTexture->beginWithClear(0, 0, 0, 0);
        for (auto child : getChildren())
        {
            if (child != renderTexture && child != renderSprite)
                child->visit(renderer, parentTransform, parentFlags);
        }
        renderTexture->end();
        
        renderSprite->visit(renderer, parentTransform, parentFlags);
    }
    
private:
    
    cc::RenderTexture*  renderTexture;
    CSprite*            renderSprite;
};