//
//  ILevelPlot.h
//  LevelPlot
//
//  Created by Michael Donovan on 3/23/16.
//  All rights reserved
//

#ifndef ILevelPlot_h
#define ILevelPlot_h

#include "IControl.h"
#include <vector>

class ILevelPlot : public IControl
{
public:
    ILevelPlot(IPlugBase* pPlug, IRECT pR, IColor cBackground, IColor cFill, IColor cHighlight, int paramIdx) : IControl(pPlug, pR), mColorBG(cBackground), mColorFill(cFill), mColorHighlight(cHighlight), mParam(paramIdx){
        width = static_cast<int>(mRECT.W());
    }
    
    ~ILevelPlot(){}
    
    bool Draw(IGraphics* pGraphics){
        //Fill background
     //   pGraphcs->FillIRECT(&mColorBG, &mRECT);
        
    }
    
    bool IsDirty(){
        return true;
    }
    
protected:
    IColor mColorBG, mColorFill, mColorHighlight;
    int mParam, width;
    std::vector<double> mVals;
};

#endif /* ILevelPlot_h */
