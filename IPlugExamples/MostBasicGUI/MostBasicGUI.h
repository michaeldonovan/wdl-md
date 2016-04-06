#ifndef __MOSTBASICGUI__
#define __MOSTBASICGUI__

#include "IPlug_include_in_plug_hdr.h"

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  kGainX = 100,
  kGainY = 100,
  kKnobFrames = 60
};

class MostBasicGUI : public IPlug
{
public:
  MostBasicGUI(IPlugInstanceInfo instanceInfo);
  ~MostBasicGUI() {};
  
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void OnParamChange(int paramIdx);

private:
  double mGain;
};

#endif
