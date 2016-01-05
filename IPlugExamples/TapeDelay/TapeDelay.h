#ifndef __TAPEDELAY__
#define __TAPEDELAY__

#include "IPlug_include_in_plug_hdr.h"

class TapeDelay : public IPlug
{
public:
  TapeDelay(IPlugInstanceInfo instanceInfo);
  ~TapeDelay();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
};

#endif
