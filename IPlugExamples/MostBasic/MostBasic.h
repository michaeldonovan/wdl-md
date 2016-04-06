#ifndef __MOSTBASIC__
#define __MOSTBASIC__

#include "IPlug_include_in_plug_hdr.h"

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kNumParams
};

class MostBasic : public IPlug
{
public:
  MostBasic(IPlugInstanceInfo instanceInfo);
  ~MostBasic() {};
  
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void OnParamChange(int paramIdx);

private:
  double mGain;
};

#endif
