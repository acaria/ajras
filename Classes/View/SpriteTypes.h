#pragma once

class SpriteBlend : public cc::Sprite
{
public:
    //overrides
    virtual bool initWithTexture(cc::Texture2D* texture,
                                 const cc::Rect& rect,
                                 bool _rotated) override;
    virtual void draw(cc::Renderer *renderer,
                      const cc::Mat4 &transform, uint32_t flags) override;
    
    void onDraw();
    
private:
    cc::CustomCommand customCmd;
};

