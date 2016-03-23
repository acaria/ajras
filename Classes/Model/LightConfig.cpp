#include "LightConfig.h"

void LightConfig::initSpot(SpotType type, const SpotInfo& info)
{
    this->defaultCfg[type] = info;
}

void LightConfig::initAmbiant(const cc::Color3B& ambiantColor)
{
    this->ambiantColor = ambiantColor;
}