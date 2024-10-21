#include "GraphicSetting.h"

int GraphicSetting::GetMSAALevel()
{
    if(aaType == AAType::MSAA && aaActive && msaaSupportAble)
    {
        return std::max(std::min(msaaSupportMaxLevel, msaaLevel), 1);
    }
    return 1;
}

BOOL GraphicSetting::GetMSAAActive()
{
    return aaType == AAType::MSAA && aaActive && msaaSupportAble;
}
