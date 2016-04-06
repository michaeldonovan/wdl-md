#ifndef __PARAMETERTYPES__
#define __PARAMETERTYPES__

#include "IPlug_include_in_plug_hdr.h"

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kMode,
  kEnable,
  kCount,
  kNumParams
};

class ParameterTypes : public IPlug
{
public:
  ParameterTypes(IPlugInstanceInfo instanceInfo);
  ~ParameterTypes() {};
  
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void OnParamChange(int paramIdx);

private:
  double mGain;
};

#endif
