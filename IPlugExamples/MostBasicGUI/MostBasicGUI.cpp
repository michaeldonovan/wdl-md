#include "MostBasicGUI.h"
#include "IPlug_include_in_plug_src.h"
#include "resource.h"

MostBasicGUI::MostBasicGUI(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{  
  // add a parameter with type, name, default, min, max, step, unit label
  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
  
  // create graphics context
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  
  // add a coloured background
  pGraphics->AttachPanelBackground(&COLOR_RED);
  
  // add a PNG image-based knob control, link to parameter and position on gui
  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, kGain, &knob));
  
  // attach graphics context
  AttachGraphics(pGraphics);

  // create factory presets
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

// process a block of audio
void MostBasicGUI::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
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
void MostBasicGUI::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->Value() / 100.;
      break;
    default:
      break;
  }
}