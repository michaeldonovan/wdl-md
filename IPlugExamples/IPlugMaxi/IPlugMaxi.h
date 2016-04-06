#ifndef __IPLUGMAXI__
#define __IPLUGMAXI__

#include "IPlug_include_in_plug_hdr.h"
#include "maximilian.h"

// copied from maximillian "replicant" example 
// doesn't seem to work properly on windows :-(
const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kNumParams
};

class IPlugMaxi : public IPlug
{
public:
  IPlugMaxi(IPlugInstanceInfo instanceInfo);
  ~IPlugMaxi() {};
  
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void OnParamChange(int paramIdx);

private:
  double mGain;
  
  maxiOsc sound, bass, timer, mod, lead, lead2, leadmod; //here are the synth bits
  maxiEnv envelope, leadenvelope; //some envelopes
  maxiFilter filter, filter2; //some filters
  maxiDelayline delay; //a delay
  convert mtof; //a method for converting midi notes to frequency
  
  int trigger, trigger2, newnote; // some control variables
  float currentPitch, leadPitch; // the final pitch variable
  int currentCount, lastCount, playHead, currentChord; // some other control variables
  double bassout, leadout, delayout; // some variables to hold the data and pass it around

};

#endif
