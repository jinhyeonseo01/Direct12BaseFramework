#include "GraphicSetting.h"

int GraphicSetting::GetMSAACount()
{
    if(aaType == AAType::MSAA && aaActive && msaaSupportAble)
    {
        return std::max(std::min(msaaSupportMaxLevel, msaaLevel), 1);
    }
    return 1;
}

int GraphicSetting::GetMSAAQuality()
{
    if (aaType == AAType::MSAA && aaActive && msaaSupportAble)
    {
        return 1;
    }
    return 0;
}

BOOL GraphicSetting::GetMSAAActive()
{
    return aaType == AAType::MSAA && aaActive && msaaSupportAble;
}
