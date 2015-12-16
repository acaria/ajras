#pragma once

#include "BaseScene.h"

class TestScene : public BaseScene<TestScene>
{
public:
    virtual bool init() override;
    virtual ~TestScene() {}
};

