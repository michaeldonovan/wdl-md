#ifndef __REVERB__
#define __REVERB__

#include "IPlug_include_in_plug_hdr.h"

class Reverb : public IPlug
{
public:
  Reverb(IPlugInstanceInfo instanceInfo);
  ~Reverb();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
};

#endif
