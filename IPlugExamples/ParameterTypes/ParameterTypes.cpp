#include "ParameterTypes.h"
#include "IPlug_include_in_plug_src.h"
#include "resource.h"

ParameterTypes::ParameterTypes(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{  
  // add a floating point, continuous parameter, 
  //                          name,   default,  min,  max,    step, unit label
  GetParam(kGain)->InitDouble("Gain", 50.,      0.,   100.0,  0.01, "%");

  // add an enumerated list, discrete parameter, 
  //                        name,   default,  numitems
  GetParam(kMode)->InitEnum("Mode", 0,        4);
  
  GetParam(kMode)->SetDisplayText(0, "a"); // set display text for first item to "a"
  GetParam(kMode)->SetDisplayText(1, "b"); // ...
  GetParam(kMode)->SetDisplayText(2, "c"); // ...
  GetParam(kMode)->SetDisplayText(3, "d"); // ...
  
  // add a boolean, discreet parameter, 
  //                          name,     default, 
  GetParam(kEnable)->InitBool("Enable", 0);

  // add an integer, discreet parameter, 
  //                       name,      default,  min,  max,  unit label
  GetParam(kCount)->InitInt("Count",   50,       0,    100,  "%");
  
  // create factory presets
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

// process a block of audio
void ParameterTypes::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  // process each sample
  for (int s = 0; s < nFrames; s++)
  {
    out1[s] = in1[s] * mGain;
    out2[s] = in2[s] * mGain;
  }
}

// handle parameter changes
void ParameterTypes::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      DBGMSG( "value of gain : %f\n", GetParam(kGain)->Value() );
      break;
    case kMode:
      DBGMSG( "value of mode : %i\n", GetParam(kMode)->Int() );
      break;
    case kEnable:
      DBGMSG( "value of enable : %i\n", GetParam(kEnable)->Bool() );
      break;
    case kCount:
      DBGMSG( "value of count : %i\n", GetParam(kCount)->Int() );
      break;
    default:
      break;
  }
}