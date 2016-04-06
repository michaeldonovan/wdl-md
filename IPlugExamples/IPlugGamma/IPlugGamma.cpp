#include "IPlugGamma.h"
#include "IPlug_include_in_plug_src.h"
#include "resource.h"

// wrong frequency

IPlugGamma::IPlugGamma(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{ 
  // add a parameter with type, name, default, min, max, step, unit label
  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
  GetParam(kFreqL)->InitDouble("FreqL", 440., 50., 1000.0, 0.01, "Hz");
  GetParam(kFreqR)->InitDouble("FreqR", 440., 50., 1000.0, 0.01, "Hz");

  // create factory presets
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

IPlugGamma::~IPlugGamma()
{
}

// process a block of audio
void IPlugGamma::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
//  double* in1 = inputs[0];
//  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  // process each sample
  for (int s = 0; s < nFrames; s++)
  {
    out1[s] = (mOsc1() * 0.1) + mNoise() * mGain;
    out2[s] = (mOsc2() * 0.1) + mNoise() * mGain;
  }
}

// handle parameter changes
void IPlugGamma::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->Value() / 100.;
      break;
    case kFreqL:
      mOsc1.freq( GetParam(kFreqL)->Value() );
      break;
    case kFreqR:
      mOsc2.freq( GetParam(kFreqR)->Value() );
      break;
    default:
      break;
  }
}