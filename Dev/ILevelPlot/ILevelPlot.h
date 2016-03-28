#ifndef __ILEVELPLOT__
#define __ILEVELPLOT__

#include "IPlug_include_in_plug_hdr.h"
#include "CParamSmooth.h"
#include "PeakFollower.h"
#include "IPopupMenuControl.h"


class ILevelPlot : public IPlug
{
public:
  ILevelPlot(IPlugInstanceInfo instanceInfo);
  ~ILevelPlot();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
};

#endif
