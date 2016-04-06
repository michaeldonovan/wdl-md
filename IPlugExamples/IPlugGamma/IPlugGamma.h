#ifndef __IPLUGGAMMA__
#define __IPLUGGAMMA__

#include "IPlug_include_in_plug_hdr.h"

#include "Gamma.h"
#include "Noise.h"
#include "Oscillator.h"

using namespace gam;

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kFreqL,
  kFreqR,
  kNumParams
};

class IPlugGamma : public IPlug
{
public:
  IPlugGamma(IPlugInstanceInfo instanceInfo);
  ~IPlugGamma();
  
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void OnParamChange(int paramIdx);

private:
  double mGain;
  
  NoisePink<> mNoise;
  Sine<> mOsc1;
  Sine<> mOsc2;
};

#endif
