#include "MidiMessages.h"
#include "IPlug_include_in_plug_src.h"
#include "resource.h"

// A bad midi gate effect

MidiMessages::MidiMessages(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.), mMult(0.) 
{  
  // add a parameter with type, name, default, min, max, step, unit label
  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");

  // create factory presets
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

// process a block of audio
void MidiMessages::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  // process each sample
  for (int s = 0; s < nFrames; s++)
  {
    while (!mMidiQueue.Empty())
    {
      IMidiMsg* pMsg = mMidiQueue.Peek();
      if (pMsg->mOffset > s) break;
      int status = pMsg->StatusMsg();
      
      switch (status)
      {
        case IMidiMsg::kNoteOn:
        case IMidiMsg::kNoteOff:
        {
          int velocity = pMsg->Velocity();
          // Note On
          if (status == IMidiMsg::kNoteOn && velocity)
          {
            mMult = 1.;
          }
          // Note Off
          else // if (status == IMidiMsg::kNoteOff || !velocity)
          {
            mMult = 0.;
          }
          break;
        }
      }
      
      mMidiQueue.Remove();
    }

    out1[s] = in1[s] * mMult * mGain;
    out2[s] = in2[s] * mMult * mGain;
  }
  
  mMidiQueue.Flush(nFrames);
}

// handle parameter changes
void MidiMessages::OnParamChange(int paramIdx)
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

void MidiMessages::ProcessMidiMsg(IMidiMsg* pMsg)
{
  int status = pMsg->StatusMsg();
  
  // filter only note messages
  switch (status)
  {
    case IMidiMsg::kNoteOn:
    case IMidiMsg::kNoteOff:
      break;
    default:
      return; // if !note message, nothing gets added to the queue
  }
  
  mMidiQueue.Add(pMsg);
}