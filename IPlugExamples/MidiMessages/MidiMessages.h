#ifndef __MIDIMESSAGES__
#define __MIDIMESSAGES__

#include "IPlug_include_in_plug_hdr.h"
#include "IMidiQueue.h"

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kNumParams
};

class MidiMessages : public IPlug
{
public:
  MidiMessages(IPlugInstanceInfo instanceInfo);
  ~MidiMessages() {};
  
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void OnParamChange(int paramIdx);
  void ProcessMidiMsg(IMidiMsg* pMsg);

private:
  double mGain;
  double mMult;
  IMidiQueue mMidiQueue;
};

#endif
