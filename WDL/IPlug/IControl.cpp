#include "IControl.h"
#include "math.h"
#include "Log.h"
#include <string>

const float GRAYED_ALPHA = 0.25f;

void IControl::SetValueFromPlug(double value)
{
  if (mDefaultValue < 0.0)
  {
    mDefaultValue = mValue = value;
  }

  if (mValue != value)
  {
    mValue = value;
    SetDirty(false);
    Redraw();
  }
}

void IControl::SetValueFromUserInput(double value)
{
  if (mValue != value)
  {
    mValue = value;
    SetDirty();
    Redraw();
  }
}

void IControl::SetDirty(bool pushParamToPlug)
{
  mValue = BOUNDED(mValue, mClampLo, mClampHi);
  mDirty = true;
  if (pushParamToPlug && mPlug && mParamIdx >= 0)
  {
    mPlug->SetParameterFromGUI(mParamIdx, mValue);
    IParam* pParam = mPlug->GetParam(mParamIdx);
    
    if (mValDisplayControl) 
    {
      WDL_String plusLabel;
      char str[32];
      pParam->GetDisplayForHost(str);
      plusLabel.Set(str, 32);
      plusLabel.Append(" ", 32);
      plusLabel.Append(pParam->GetLabelForHost(), 32);
      
      ((ITextControl*)mValDisplayControl)->SetTextFromPlug(plusLabel.Get());
    }
    
    if (mNameDisplayControl) 
    {
      ((ITextControl*)mNameDisplayControl)->SetTextFromPlug((char*) pParam->GetNameForHost());
    }
  }
}

void IControl::SetClean()
{
  mDirty = mRedraw;
  mRedraw = false;
}

void IControl::Hide(bool hide)
{
  mHide = hide;
  mRedraw = true;
  SetDirty(false);
}

void IControl::GrayOut(bool gray)
{
  mGrayed = gray;
  mBlend.mWeight = (gray ? GRAYED_ALPHA : 1.0f);
  SetDirty(false);
}

void IControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{
  #ifdef PROTOOLS
  if (pMod->A && mDefaultValue >= 0.0)
  {
    mValue = mDefaultValue;
    SetDirty();
  }
  #endif
  
  if (pMod->R) {
		PromptUserInput();
	}
}

void IControl::OnMouseDblClick(int x, int y, IMouseMod* pMod)
{
  #ifdef PROTOOLS
  PromptUserInput();
  #else
  if (mDefaultValue >= 0.0)
  {
    mValue = mDefaultValue;
    SetDirty();
  }
  #endif
}

void IControl::OnMouseWheel(int x, int y, IMouseMod* pMod, int d)
{
  #ifdef PROTOOLS
  if (pMod->C)
  {
    mValue += 0.001 * d;
  }
  #else
  if (pMod->C || pMod->S)
  {
    mValue += 0.001 * d;
  }
  #endif
  else
  {
    mValue += 0.01 * d;
  }
  
  SetDirty();
}

#define PARAM_EDIT_W 30
#define PARAM_EDIT_H 16

void IControl::PromptUserInput()
{
  if (mParamIdx >= 0 && !mDisablePrompt)
  {
    if (mPlug->GetParam(mParamIdx)->GetNDisplayTexts()) // popup menu
    {
      mPlug->GetGUI()->PromptUserInput(this, mPlug->GetParam(mParamIdx), &mRECT );
    }
    else // text entry
    {
      int cX = (int) mRECT.MW();
      int cY = (int) mRECT.MH();
      int halfW = int(float(PARAM_EDIT_W)/2.f);
      int halfH = int(float(PARAM_EDIT_H)/2.f);

      IRECT txtRECT = IRECT(cX - halfW, cY - halfH, cX + halfW,cY + halfH);
      mPlug->GetGUI()->PromptUserInput(this, mPlug->GetParam(mParamIdx), &txtRECT );
    }

    Redraw();
  }
}

void IControl::PromptUserInput(IRECT* pTextRect)
{
  if (mParamIdx >= 0 && !mDisablePrompt)
  {
    mPlug->GetGUI()->PromptUserInput(this, mPlug->GetParam(mParamIdx), pTextRect);
    Redraw();
  }
}

IControl::AuxParam* IControl::GetAuxParam(int idx)
{
  assert(idx > -1 && idx < mAuxParams.GetSize());
  return mAuxParams.Get() + idx;
}

int IControl::AuxParamIdx(int paramIdx)
{
  for (int i=0;i<mAuxParams.GetSize();i++)
  {
    if(GetAuxParam(i)->mParamIdx == paramIdx)
      return i;
  }
  
  return -1;
}

void IControl::AddAuxParam(int paramIdx)
{
  mAuxParams.Add(AuxParam(paramIdx));
}

void IControl::SetAuxParamValueFromPlug(int auxParamIdx, double value)
{
  AuxParam* auxParam = GetAuxParam(auxParamIdx);
  
  if (auxParam->mValue != value)
  {
    auxParam->mValue = value;
    SetDirty(false);
    Redraw();
  }
}

void IControl::SetAllAuxParamsFromGUI()
{
  for (int i=0;i<mAuxParams.GetSize();i++)
  {
    AuxParam* auxParam = GetAuxParam(i);
    mPlug->SetParameterFromGUI(auxParam->mParamIdx, auxParam->mValue);
  }
}

bool IPanelControl::Draw(IGraphics* pGraphics)
{
  pGraphics->FillIRect(&mColor, &mRECT, &mBlend);
  return true;
}

bool IBitmapControl::Draw(IGraphics* pGraphics)
{
  int i = 1;
  if (mBitmap.N > 1)
  {
    i = 1 + int(0.5 + mValue * (double) (mBitmap.N - 1));
    i = BOUNDED(i, 1, mBitmap.N);
  }
  return pGraphics->DrawBitmap(&mBitmap, &mRECT, i, &mBlend);
}

void ISwitchControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{
  if (mBitmap.N > 1)
  {
    mValue += 1.0 / (double) (mBitmap.N - 1);
  }
  else
  {
    mValue += 1.0;
  }

  if (mValue > 1.001)
  {
    mValue = 0.0;
  }
  SetDirty();
}

void ISwitchControl::OnMouseDblClick(int x, int y, IMouseMod* pMod)
{
  OnMouseDown(x, y, pMod);
}

void ISwitchPopUpControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{
  PromptUserInput();

  SetDirty();
}

ISwitchFramesControl::ISwitchFramesControl(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap, bool imagesAreHorizontal, IChannelBlend::EBlendMethod blendMethod)
  : ISwitchControl(pPlug, x, y, paramIdx, pBitmap, blendMethod)
{
  mDisablePrompt = false;
  
  for(int i = 0; i < pBitmap->N; i++)
  {
    if (imagesAreHorizontal)
      mRECTs.Add(mRECT.SubRectHorizontal(pBitmap->N, i)); 
    else
      mRECTs.Add(mRECT.SubRectVertical(pBitmap->N, i)); 
  }
}

void ISwitchFramesControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{
  int n = mRECTs.GetSize();
  
  for (int i = 0; i < n; i++) 
  {
    if (mRECTs.Get()[i].Contains(x, y)) 
    {
      mValue = (double) i / (double) (n - 1);
      break;
    }
  }
  
  SetDirty();
}

IInvisibleSwitchControl::IInvisibleSwitchControl(IPlugBase* pPlug, IRECT pR, int paramIdx)
  :   IControl(pPlug, pR, paramIdx, IChannelBlend::kBlendClobber)
{
  mDisablePrompt = true;
}

void IInvisibleSwitchControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{
  if (mValue < 0.5)
  {
    mValue = 1.0;
  }
  else
  {
    mValue = 0.0;
  }
  SetDirty();
}

IRadioButtonsControl::IRadioButtonsControl(IPlugBase* pPlug, IRECT pR, int paramIdx, int nButtons,
    IBitmap* pBitmap, EDirection direction, bool reverse)
  :   IControl(pPlug, pR, paramIdx), mBitmap(*pBitmap)
{
  mRECTs.Resize(nButtons);
  int h = int((double) pBitmap->H / (double) pBitmap->N);
  
  if (reverse) 
  {
    if (direction == kHorizontal)
    {
      int dX = int((double) (pR.W() - nButtons * pBitmap->W) / (double) (nButtons - 1));
      int x = mRECT.R - pBitmap->W - dX;
      int y = mRECT.T;
      
      for (int i = 0; i < nButtons; ++i)
      {
        mRECTs.Get()[i] = IRECT(x, y, x + pBitmap->W, y + h);
        x -= pBitmap->W + dX;
      }
    }
    else
    {
      int dY = int((double) (pR.H() - nButtons * h) /  (double) (nButtons - 1));
      int x = mRECT.L;
      int y = mRECT.B - h - dY;
      
      for (int i = 0; i < nButtons; ++i)
      {
        mRECTs.Get()[i] = IRECT(x, y, x + pBitmap->W, y + h);
        y -= h + dY;
      }
    }
    
  }
  else
  {
    int x = mRECT.L, y = mRECT.T;
    
    if (direction == kHorizontal)
    {
      int dX = int((double) (pR.W() - nButtons * pBitmap->W) / (double) (nButtons - 1));
      for (int i = 0; i < nButtons; ++i)
      {
        mRECTs.Get()[i] = IRECT(x, y, x + pBitmap->W, y + h);
        x += pBitmap->W + dX;
      }
    }
    else
    {
      int dY = int((double) (pR.H() - nButtons * h) /  (double) (nButtons - 1));
      for (int i = 0; i < nButtons; ++i)
      {
        mRECTs.Get()[i] = IRECT(x, y, x + pBitmap->W, y + h);
        y += h + dY;
      }
    }
  }
}

void IRadioButtonsControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{
  #ifdef PROTOOLS
  if (pMod->A) 
  {
    if (mDefaultValue >= 0.0)
    {
      mValue = mDefaultValue;
      SetDirty();
      return;
    }
  }
  else
  #endif
  if (pMod->R)
  {
    PromptUserInput();
    return;
  }

  int i, n = mRECTs.GetSize();

  for (i = 0; i < n; ++i)
  {
    if (mRECTs.Get()[i].Contains(x, y))
    {
      mValue = (double) i / (double) (n - 1);
      break;
    }
  }

  SetDirty();
}

bool IRadioButtonsControl::Draw(IGraphics* pGraphics)
{
  int i, n = mRECTs.GetSize();
  int active = int(0.5 + mValue * (double) (n - 1));
  active = BOUNDED(active, 0, n - 1);
  for (i = 0; i < n; ++i)
  {
    if (i == active)
    {
      pGraphics->DrawBitmap(&mBitmap, &mRECTs.Get()[i], 2, &mBlend);
    }
    else
    {
      pGraphics->DrawBitmap(&mBitmap, &mRECTs.Get()[i], 1, &mBlend);
    }
  }
  return true;
}

void IContactControl::OnMouseUp(int x, int y, IMouseMod* pMod)
{
  mValue = 0.0;
  SetDirty();
}

IFaderControl::IFaderControl(IPlugBase* pPlug, int x, int y, int len, int paramIdx, IBitmap* pBitmap, EDirection direction, bool onlyHandle)
  : IControl(pPlug, IRECT(), paramIdx), mLen(len), mBitmap(*pBitmap), mDirection(direction), mOnlyHandle(onlyHandle)
{
  if (direction == kVertical)
  {
    mHandleHeadroom = mBitmap.H;
    mRECT = mTargetRECT = IRECT(x, y, x + mBitmap.W, y + len);
  }
  else
  {
    mHandleHeadroom = mBitmap.W;
    mRECT = mTargetRECT = IRECT(x, y, x + len, y + mBitmap.H);
  }
}

IRECT IFaderControl::GetHandleRECT(double value) const
{
  if (value < 0.0)
  {
    value = mValue;
  }
  IRECT r(mRECT.L, mRECT.T, mRECT.L + mBitmap.W, mRECT.T + mBitmap.H);
  if (mDirection == kVertical)
  {
    int offs = int((1.0 - value) * (double) (mLen - mHandleHeadroom));
    r.T += offs;
    r.B += offs;
  }
  else
  {
    int offs = int(value * (double) (mLen - mHandleHeadroom));
    r.L += offs;
    r.R += offs;
  }
  return r;
}

void IFaderControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{
  #ifdef PROTOOLS
  if (pMod->A) 
  {
    if (mDefaultValue >= 0.0)
    {
      mValue = mDefaultValue;
      SetDirty();
      return;
    }
  }
  else
  #endif
  if (pMod->R)
  {
    PromptUserInput();
    return;
  }

  return SnapToMouse(x, y);
}

void IFaderControl::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod)
{
  return SnapToMouse(x, y);
}

void IFaderControl::SnapToMouse(int x, int y)
{
  if (mDirection == kVertical)
  {
    mValue = 1.0 - (double) (y - mRECT.T - mHandleHeadroom / 2) / (double) (mLen - mHandleHeadroom);
  }
  else
  {
    mValue = (double) (x - mRECT.L - mHandleHeadroom / 2) / (double) (mLen - mHandleHeadroom);
  }
  SetDirty();
}

bool IFaderControl::Draw(IGraphics* pGraphics)
{
  IRECT r = GetHandleRECT();
  return pGraphics->DrawBitmap(&mBitmap, &r, 1, &mBlend);
}

bool IFaderControl::IsHit(int x, int y) 
{
  if(mOnlyHandle)
  {
    IRECT r = GetHandleRECT();
    return r.Contains(x, y); 
  }
  else 
  {
    return mTargetRECT.Contains(x, y); 
  }
}

void IKnobControl::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod)
{
  double gearing = mGearing;
  
  #ifdef PROTOOLS
    #ifdef OS_WIN
      if (pMod->C) gearing *= 10.0;
    #else
      if (pMod->R) gearing *= 10.0;
    #endif
  #else
    if (pMod->C || pMod->S) gearing *= 10.0;
  #endif
  
  if (mDirection == kVertical)
  {
    mValue += (double) dY / (double) (mRECT.T - mRECT.B) / gearing;
  }
  else
  {
    mValue += (double) dX / (double) (mRECT.R - mRECT.L) / gearing;
  }

  SetDirty();
}

IKnobLineControl::IKnobLineControl(IPlugBase* pPlug, IRECT pR, int paramIdx,
                                   const IColor* pColor, double innerRadius, double outerRadius,
                                   double minAngle, double maxAngle, EDirection direction, double gearing)
  :   IKnobControl(pPlug, pR, paramIdx, direction, gearing),
      mColor(*pColor)
{
  mMinAngle = (float) minAngle;
  mMaxAngle = (float) maxAngle;
  mInnerRadius = (float) innerRadius;
  mOuterRadius = (float) outerRadius;
  if (mOuterRadius == 0.0f)
  {
    mOuterRadius = 0.5f * (float) pR.W();
  }
  mBlend = IChannelBlend(IChannelBlend::kBlendClobber);
}

bool IKnobLineControl::Draw(IGraphics* pGraphics)
{
  double v = mMinAngle + mValue * (mMaxAngle - mMinAngle);
  float sinV = (float) sin(v);
  float cosV = (float) cos(v);
  float cx = mRECT.MW(), cy = mRECT.MH();
  float x1 = cx + mInnerRadius * sinV, y1 = cy - mInnerRadius * cosV;
  float x2 = cx + mOuterRadius * sinV, y2 = cy - mOuterRadius * cosV;
  return pGraphics->DrawLine(&mColor, x1, y1, x2, y2, &mBlend, true);
}

bool IKnobRotaterControl::Draw(IGraphics* pGraphics)
{
  int cX = (mRECT.L + mRECT.R) / 2;
  int cY = (mRECT.T + mRECT.B) / 2;
  double angle = mMinAngle + mValue * (mMaxAngle - mMinAngle);
  return pGraphics->DrawRotatedBitmap(&mBitmap, cX, cY, angle, mYOffset, &mBlend);
}

// Same as IBitmapControl::Draw.
bool IKnobMultiControl::Draw(IGraphics* pGraphics)
{
  int i = 1 + int(0.5 + mValue * (double) (mBitmap.N - 1));
  i = BOUNDED(i, 1, mBitmap.N);
  return pGraphics->DrawBitmap(&mBitmap, &mRECT, i, &mBlend);
}





bool IKnobRotatingMaskControl::Draw(IGraphics* pGraphics)
{
  double angle = mMinAngle + mValue * (mMaxAngle - mMinAngle);
  return pGraphics->DrawRotatedMask(&mBase, &mMask, &mTop, mRECT.L, mRECT.T, angle, &mBlend);
}

bool IBitmapOverlayControl::Draw(IGraphics* pGraphics)
{
  if (mValue < 0.5)
  {
    mTargetRECT = mTargetArea;
    return true;  // Don't draw anything.
  }
  else
  {
    mTargetRECT = mRECT;
    return IBitmapControl::Draw(pGraphics);
  }
}

void ITextControl::SetTextFromPlug(char* str)
{
  if (strcmp(mStr.Get(), str))
  {
    SetDirty(false);
    mStr.Set(str);
  }
}

bool ITextControl::Draw(IGraphics* pGraphics)
{
  char* cStr = mStr.Get();
  if (CSTR_NOT_EMPTY(cStr))
  {
    return pGraphics->DrawIText(&mText, cStr, &mRECT);
  }
  return true;
}

ICaptionControl::ICaptionControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IText* pText, bool showParamLabel)
  :   ITextControl(pPlug, pR, pText), mShowParamLabel(showParamLabel)
{
  mParamIdx = paramIdx;
}

void ICaptionControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{

    PromptUserInput();
  
}

void ICaptionControl::OnMouseDblClick(int x, int y, IMouseMod* pMod)
{
  PromptUserInput();
}

bool ICaptionControl::Draw(IGraphics* pGraphics)
{
  IParam* pParam = mPlug->GetParam(mParamIdx);
  char cStr[32];
  pParam->GetDisplayForHost(cStr);
  mStr.Set(cStr);

  if (mShowParamLabel)
  {
    mStr.Append(" ");
    mStr.Append(pParam->GetLabelForHost());
  }

  return ITextControl::Draw(pGraphics);
}

IURLControl::IURLControl(IPlugBase* pPlug, IRECT pR, const char* url, const char* backupURL, const char* errMsgOnFailure)
  : IControl(pPlug, pR)
{
  memset(mURL, 0, MAX_URL_LEN);
  memset(mBackupURL, 0, MAX_URL_LEN);
  memset(mErrMsg, 0, MAX_NET_ERR_MSG_LEN);

  if (CSTR_NOT_EMPTY(url))
  {
    strcpy(mURL, url);
  }
  if (CSTR_NOT_EMPTY(backupURL))
  {
    strcpy(mBackupURL, backupURL);
  }
  if (CSTR_NOT_EMPTY(errMsgOnFailure))
  {
    strcpy(mErrMsg, errMsgOnFailure);
  }
}

void IURLControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{
  bool opened = false;

  if (CSTR_NOT_EMPTY(mURL))
  {
    opened = mPlug->GetGUI()->OpenURL(mURL, mErrMsg);
  }

  if (!opened && CSTR_NOT_EMPTY(mBackupURL))
  {
    opened = mPlug->GetGUI()->OpenURL(mBackupURL, mErrMsg);
  }
}

void IFileSelectorControl::OnMouseDown(int x, int y, IMouseMod* pMod)
{
  if (mPlug && mPlug->GetGUI())
  {
    mState = kFSSelecting;
    SetDirty(false);

    mPlug->GetGUI()->PromptForFile(&mFile, mFileAction, &mDir, mExtensions.Get());
    mValue += 1.0;
    if (mValue > 1.0)
    {
      mValue = 0.0;
    }
    mState = kFSDone;
    SetDirty();
  }
}

bool IFileSelectorControl::Draw(IGraphics* pGraphics)
{
  if (mState == kFSSelecting)
  {
    pGraphics->DrawBitmap(&mBitmap, &mRECT, 0, 0);
  }
  return true;
}

void IFileSelectorControl::GetLastSelectedFileForPlug(WDL_String* pStr)
{
  pStr->Set(mFile.Get());
}

void IFileSelectorControl::SetLastSelectedFileFromPlug(char* file)
{
  mFile.Set(file);
}

bool IFileSelectorControl::IsDirty()
{
  if (mDirty)
  {
    return true;
  }

  if (mState == kFSDone)
  {
    mState = kFSNone;
    return true;
  }
  return false;
}


IKnobMultiControlText::IKnobMultiControlText(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap, IText* pText, bool showParamLabel, int offset)
: IKnobMultiControl(pPlug, x, y, paramIdx, pBitmap), mBitmap(*pBitmap), mShowParamLabel(showParamLabel)
{
    mRECT = IRECT(mRECT.L, mRECT.T, mRECT.R, mRECT.B+10 + offset);
    mText = *pText;
    mTextRECT = IRECT(mRECT.L, mRECT.B-20, mRECT.R, mRECT.B);
    mImgRECT = IRECT(mRECT.L, mRECT.T, &mBitmap);
    mDisablePrompt = false;
}



IKnobMultiControlText::~IKnobMultiControlText() {}

bool IKnobMultiControlText::Draw(IGraphics* pGraphics)
{
    int i = 1 + int(0.5 + mValue * (double) (mBitmap.N - 1));
    i = BOUNDED(i, 1, mBitmap.N);
    pGraphics->DrawBitmap(&mBitmap, &mImgRECT, i, &mBlend);
    //pGraphics->FillIRect(&COLOR_WHITE, &mTextRECT);
    
    char disp[20];
    mPlug->GetParam(mParamIdx)->GetDisplayForHost(disp);
    
    std::string str(disp);
    
    if (CSTR_NOT_EMPTY(disp))
    {
        if (mShowParamLabel)
        {
            str += " ";
            str += mPlug->GetParam(mParamIdx)->GetLabelForHost();
        }
        const char* cstr = str.c_str();
        return pGraphics->DrawIText(&mText, (char*)cstr, &mTextRECT);
    }
    return true;
}

void IKnobMultiControlText::OnMouseDown(int x, int y, IMouseMod* pMod)
{
    if (mTextRECT.Contains(x, y)) PromptUserInput(&mTextRECT);
#ifdef RTAS_API
    else if (pMod->A)
    {
        if (mDefaultValue >= 0.0)
        {
            mValue = mDefaultValue;
            SetDirty();
        }
    }
#endif
    else
    {
        OnMouseDrag(x, y, 0, 0, pMod);
    }
}

void IKnobMultiControlText::OnMouseDblClick(int x, int y, IMouseMod* pMod)
{
#ifdef RTAS_API
    PromptUserInput(&mTextRECT);
#else
    if (mDefaultValue >= 0.0)
    {
        mValue = mDefaultValue;
        SetDirty();
    }
#endif
}

void IKnobMultiControlText::setCaptionOffset(int offset){
    mTextRECT = IRECT(mTextRECT.L, mTextRECT.B + offset, mTextRECT.R, mTextRECT.B + offset);
    SetDirty();
}




IFaderControlText::IFaderControlText(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap, IText* pText, bool showParamLabel, int offset)
: IKnobMultiControl(pPlug, x, y, paramIdx, pBitmap), mBitmap(*pBitmap), mShowParamLabel(showParamLabel)
{
    mRECT = IRECT(mRECT.L, mRECT.T, mRECT.R, mRECT.B+10 + offset);
    mText = *pText;
    mTextRECT = IRECT(mRECT.L, mRECT.B-20, mRECT.R, mRECT.B);
    mImgRECT = IRECT(mRECT.L, mRECT.T, &mBitmap);
    mDisablePrompt = false;
}

IFaderControlText::~IFaderControlText() {}

bool IFaderControlText::Draw(IGraphics* pGraphics)
{
    int i = 1 + int(0.5 + mValue * (double) (mBitmap.N - 1));
    i = BOUNDED(i, 1, mBitmap.N);
    pGraphics->DrawBitmap(&mBitmap, &mImgRECT, i, &mBlend);
    //pGraphics->FillIRect(&COLOR_WHITE, &mTextRECT);
    
    char disp[20];
    mPlug->GetParam(mParamIdx)->GetDisplayForHost(disp);
    
    std::string str(disp);
    
    if (CSTR_NOT_EMPTY(disp))
    {
        if (mShowParamLabel)
        {
            str += " ";
            str += mPlug->GetParam(mParamIdx)->GetLabelForHost();
        }
        const char* cstr = str.c_str();
        return pGraphics->DrawIText(&mText, (char*)cstr, &mTextRECT);
    }
    return true;
}


void IFaderControlText::OnMouseDown(int x, int y, IMouseMod* pMod)
{
    if (mTextRECT.Contains(x, y)) PromptUserInput(&mTextRECT);
#ifdef PROTOOLS
    if (pMod->A)
    {
        if (mDefaultValue >= 0.0)
        {
            mValue = mDefaultValue;
            SetDirty();
            return;
        }
    }
    else
#endif
        if (pMod->R)
        {
            PromptUserInput();
            return;
        }
    
    if(mImgRECT.Contains(x, y)) SnapToMouse(x, y);
}



void IFaderControlText::OnMouseDblClick(int x, int y, IMouseMod* pMod)
{
#ifdef RTAS_API
    PromptUserInput(&mTextRECT);
#else
    if (mDefaultValue >= 0.0)
    {
        mValue = mDefaultValue;
        SetDirty();
    }
#endif
}


void IFaderControlText::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod)
{
    return SnapToMouse(x, y);
}


void IFaderControlText::SnapToMouse(int x, int y)
{
    mValue = 1.0 - (double) (y - mImgRECT.T) / (double) mImgRECT.H();
    
    SetDirty();
}

void IFaderControlText::setCaptionOffset(int offset){
    mTextRECT = IRECT(mTextRECT.L, mTextRECT.B + offset, mTextRECT.R, mTextRECT.B + offset);
    SetDirty();
}



ICairoPlotControl::ICairoPlotControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IColor* fillColor, IColor* lineColor, bool fillEnable) : IControl(pPlug, pR), mColorFill(fillColor), mColorLine(lineColor), mFill(fillEnable), mRange(1), mLineWeight(2.), mRetina(false)
{
    mWidth = mRECT.W();
    mHeight = mRECT.H();
    
    mVals = new valarray<double>(0., mWidth);
    
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mWidth, mHeight);
    cr = cairo_create(surface);
}

ICairoPlotControl::~ICairoPlotControl(){
    delete mVals;
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

void ICairoPlotControl::setFillEnable(bool b){
    mFill = b;
}

void ICairoPlotControl::setLineColor(IColor* color){
    mColorLine.setFromIColor(color);
}

void ICairoPlotControl::setFillColor(IColor* color){
    mColorFill.setFromIColor(color);
}

void ICairoPlotControl::setAAquality(int quality){
    switch(quality){
        case kNone:
            cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
            break;
        case kFast:
            cairo_set_antialias(cr, CAIRO_ANTIALIAS_FAST);
            break;
        case kGood:
            cairo_set_antialias(cr, CAIRO_ANTIALIAS_GOOD);
            break;
        case kBest:
            cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
            break;
    }
}

void ICairoPlotControl::setLineWeight(double w){
    mLineWeight = w;
}

void ICairoPlotControl::setRange(double range){
    mRange = range;
}

void ICairoPlotControl::plotVals(valarray<double>* vals, bool normalize){
    double scalar;
    
    delete mVals;
    
    mVals = vals;
    
    if (normalize) {
        scalar = 1. / mVals->max();
    }
    else{
        scalar = 1. / mRange;
    }
    
    if(scalar != 1.) *mVals *= scalar;
    
    SetDirty(true);
}

void ICairoPlotControl::checkChangeDPI(IGraphics* pGraphics){
#ifdef IPLUG_RETINA_SUPPORT
    if(pGraphics->IsRetina() != mRetina){   //Check if Retina state has changed
        mRetina = pGraphics->IsRetina();    //Update retina state
        if(mRetina){
            mWidth = mRECT.W() * 2;
            mHeight = mRECT.H() * 2;
        }
        else{
            mWidth = mRECT.W();
            mHeight = mRECT.H();
        }
    }
#else
    mWidth = mRECT.W();
    mHeight = mRECT.H();
#endif
    
    cairo_surface_destroy(surface);
    cairo_destroy(cr);
    
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mWidth, mHeight);
    cr = cairo_create(surface);
    
}

bool ICairoPlotControl::Draw(IGraphics* pGraphics){
    double mSpacing = (double)mWidth / mVals->size()  ;
    
    checkChangeDPI(pGraphics);
    
    cairo_save(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    cairo_restore(cr);
    

    
    //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mWidth, mHeight);
    //cr = cairo_create(surface);
    if(mRetina){
        cairo_set_line_width(cr, mLineWeight * 2);
    }
    else{
        cairo_set_line_width(cr, mLineWeight);
    }

    //Starting point in bottom left corner.
    cairo_move_to(cr, 0, mHeight);
    
    //Draw data points
    for (int i = 0, x = 0; x < mWidth && i < mVals->size(); i++, x += mSpacing) {
        cairo_line_to(cr, x, mVals->operator[](i));
    }
    
    //Endpoint in bottom right corner
    cairo_line_to(cr, mWidth, mHeight);
    
    cairo_close_path(cr);
    
    if(mFill){
        cairo_set_source_rgba(cr, mColorFill.R, mColorFill.G, mColorFill.B, mColorFill.A);
        
        cairo_path_t* path = cairo_copy_path(cr);
        
        cairo_fill(cr);
        
        
        cairo_append_path(cr, path);
        
        cairo_set_source_rgba(cr, mColorLine.R, mColorLine.G, mColorLine.B, mColorLine.A);
        
        cairo_stroke(cr);
        
        cairo_path_destroy(path);
    }
    else{
        cairo_set_source_rgba(cr, mColorLine.R, mColorLine.G, mColorLine.B, mColorLine.A);
        cairo_stroke(cr);
    }
    
    cairo_surface_flush(surface);
    
    unsigned int *data = (unsigned int*)cairo_image_surface_get_data(surface);
    //Bind to LICE
    LICE_WrapperBitmap WrapperBitmap = LICE_WrapperBitmap(data, mWidth, mHeight, mWidth, false);
    
    //Render
    //}
    IBitmap result;
#ifndef IPLUG_RETINA_SUPPORT
    result = IBitmap(&WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
    
#else
    result = IBitmap(&WrapperBitmap, &WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
#endif
    return pGraphics->DrawBitmap(&result, &this->mRECT);
    
}


//Accessors//
CColor ICairoPlotControl::getColorFill(){ return mColorFill; }
CColor ICairoPlotControl::getColorLine(){ return mColorLine; }
bool ICairoPlotControl::getFill(){ return mFill; }
int ICairoPlotControl::getWidth(){ return mWidth; }
int ICairoPlotControl::getHeight(){ return mHeight; }
double ICairoPlotControl::getRange() { return mRange; }


inline double ICairoPlotControl::scaleValue(double inValue, double inMin, double inMax, double outMin, double outMax){
    return ((outMax - outMin) * (inValue - inMin)) / (inMax - inMin) + outMin;
}

inline double ICairoPlotControl::percentToCoordinates(double value) {
    return mHeight - value * mHeight;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




ILevelPlotControl::ILevelPlotControl(IPlugBase* pPlug, IRECT pR, IColor* fillColor, IColor* lineColor, double timeScale, bool fillEnable, int paramIdx) : ICairoPlotControl(pPlug, pR, paramIdx, fillColor, lineColor, fillEnable), mTimeScale(timeScale), mBufferLength(0.), mYRange(-32), mStroke(true), mHeadroom(2), mReverseFill(false), mGradientFill(false)
{
    mRes = kHighRes;
    mXRes = mWidth/2.;
    mDrawVals = new valarray<double>(mHeight, mXRes);
    mBuffer = new valarray<double>(0., mTimeScale * mPlug->GetSampleRate() / (double)mXRes);
    setResolution(kHighRes);
    setLineWeight(2.);
}

ILevelPlotControl::~ILevelPlotControl(){
    delete mDrawVals;
    delete mBuffer;
}


void ILevelPlotControl::setReverseFill(bool rev){
    mReverseFill = rev;
}

void ILevelPlotControl::setResolution(int res){
    mRes = res;
    switch (mRes) {
        case kLowRes:
            mXRes = mWidth / 8.;
            break;
            
        case kMidRes:
            mXRes = mWidth / 4.;
            break;
            
        case kHighRes:
            mXRes = mWidth / 2.;
            break;
            
        case kMaxRes:
            
            mXRes = mWidth;
            break;
            
        default:
            mXRes = mWidth / 2.;
            break;
    }
    
    mBuffer->resize(mTimeScale * mPlug->GetSampleRate() / (double)mXRes, -48.);
    mBufferLength = 0;
    if(mReverseFill){
        mDrawVals->resize(mXRes, -2);
    }
    else{
        mDrawVals->resize(mXRes, mHeight);
    }
    mSpacing = mWidth / mXRes;
    
}

void ILevelPlotControl::setYRange(int yRangeDB){
    switch (yRangeDB) {
        case k16dB:
            mYRange = -16;
            break;
            
        case k32dB:
            mYRange = -32;
            break;
            
        case k48dB:
            mYRange = -48;
            break;
            
        default:
            break;
    }
}

void ILevelPlotControl::setStroke(bool stroke){
    mStroke = stroke;
}

void ILevelPlotControl::setGradientFill(bool grad){
    mGradientFill = grad;
}
void ILevelPlotControl::process(double sample){
    mBuffer->operator[](mBufferLength) = sample;
    mBufferLength++;
    
    if(mBufferLength >= mBuffer->size()){
        double average;
        
        *mDrawVals = mDrawVals->shift(1);
        
        average = mBuffer->sum() / (double)mBuffer->size();
        average = scaleValue(average, mYRange, 2, 0, 1);
        mDrawVals->operator[](mDrawVals->size() - 1) = percentToCoordinates(average);
        
        mBufferLength = 0;
    }
}

void ILevelPlotControl::checkChangeDPI(IGraphics* pGraphics){
#ifdef IPLUG_RETINA_SUPPORT
    if(pGraphics->IsRetina() != mRetina){   //Check if Retina state has changed
        mRetina = pGraphics->IsRetina();    //Update retina state
        if(mRetina){
            mWidth = mRECT.W() * 2;
            mHeight = mRECT.H() * 2;
        }
        else{
            mWidth = mRECT.W();
            mHeight = mRECT.H();
        }
        
        setResolution(mRes);
        
        cairo_surface_destroy(surface);
        cairo_destroy(cr);
        
        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mWidth, mHeight);
        cr = cairo_create(surface);
    }
#endif
}

bool ILevelPlotControl::Draw(IGraphics* pGraphics){
#ifdef IPLUG_RETINA_SUPPORT
    checkChangeDPI(pGraphics);
#endif
    cairo_save(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    cairo_restore(cr);
    
    //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mWidth, mHeight);
    //cr = cairo_create(surface);
    
    if(mRetina){
        cairo_set_line_width(cr, mLineWeight * 2);
    }
    else{
        cairo_set_line_width(cr, mLineWeight);
    }
    
    //        if(mGridLines){
    //            drawDBLines(cr);
    //        }
    
    //Starting point in bottom left corner.
    if(mReverseFill){
        cairo_move_to(cr, -8, -8);
    }
    else{
        cairo_move_to(cr, -4, mHeight+4);
    }
    
    //Draw data points
    for (int i = 0, x = 0; x < mWidth && i < mDrawVals->size(); i++) {
        cairo_line_to(cr, x, mDrawVals->operator[](i));
        x += mSpacing;
    }
    
    cairo_line_to(cr, mWidth+8, mDrawVals->operator[](mDrawVals->size()-1));
    //Endpoint in bottom right corner
    if(mReverseFill){
        cairo_line_to(cr, mWidth+8, -8);
    }
    else{
        cairo_line_to(cr, mWidth+8, mHeight+8);
    }
    
    cairo_close_path(cr);
    
    if(mFill && mStroke){
        cairo_path_t* path = cairo_copy_path(cr);
        
        if(mGradientFill){
            cairo_pattern_t* grad = cairo_pattern_create_linear(0, 0, 0, mHeight);
            
            cairo_pattern_add_color_stop_rgba(grad, .5, mColorFill.R, mColorFill.G, mColorFill.B, mColorFill.A);
            cairo_pattern_add_color_stop_rgba(grad, 1, mColorFill.R, mColorFill.G, mColorFill.B, .3);
            
            cairo_set_source(cr, grad);
            cairo_fill(cr);
            cairo_pattern_destroy(grad);
        }
        else{
            cairo_set_source_rgba(cr, mColorFill.R, mColorFill.G, mColorFill.B, mColorFill.A);
            cairo_fill(cr);
        }
        
        cairo_append_path(cr, path);
        
        cairo_set_source_rgba(cr, mColorLine.R, mColorLine.G, mColorLine.B, mColorLine.A);
        
        
        cairo_stroke(cr);
        
        cairo_path_destroy(path);
    }
    else if(mStroke){
        cairo_set_source_rgba(cr, mColorLine.R, mColorLine.G, mColorLine.B, mColorLine.A);
        cairo_stroke(cr);
    }
    else if(mFill){
        if(mGradientFill){
            cairo_pattern_t* grad = cairo_pattern_create_linear(0, 0, 0, mHeight);
            
            cairo_pattern_add_color_stop_rgba(grad, .75, mColorFill.R, mColorFill.G, mColorFill.B, mColorFill.A);
            cairo_pattern_add_color_stop_rgba(grad, 1, mColorFill.R, mColorFill.G, mColorFill.B, .3);
            
            cairo_set_source(cr, grad);
            cairo_fill(cr);
            
            cairo_pattern_destroy(grad);
        }
        else{
            cairo_set_source_rgba(cr, mColorFill.R, mColorFill.G, mColorFill.B, mColorFill.A);
            cairo_fill(cr);
        }
    }
    cairo_surface_flush(surface);
    
    unsigned int *data = (unsigned int*)cairo_image_surface_get_data(surface);
    //Bind to LICE
    LICE_WrapperBitmap WrapperBitmap = LICE_WrapperBitmap(data, mWidth, mHeight, mWidth, false);
    
    //Render
    //}
    IBitmap result;
#ifndef IPLUG_RETINA_SUPPORT
    result = IBitmap(&WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
#else
    result = IBitmap(&WrapperBitmap, &WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
#endif
    return pGraphics->DrawBitmap(&result, &this->mRECT);
}



IGRPlotControl::IGRPlotControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IColor* preFillColor, IColor* postFillColor, IColor* postLineColor, IColor* GRFillColor, IColor* GRLineColor, double timeScale) : ICairoPlotControl(pPlug, pR, paramIdx, postFillColor, postLineColor, true), mTimeScale(timeScale), mBufferLength(0.), mYRange(-32), mHeadroom(2), sr(mPlug->GetSampleRate()), mPreFillColor(preFillColor), mGRFillColor(GRFillColor), mGRLineColor(GRLineColor), mRes(2.), mGradientFill(true)
{
    mXRes = mWidth/2.;
    mDrawValsPre = new valarray<double>(mHeight, mXRes);
    mDrawValsPost = new valarray<double>(mHeight, mXRes);
    mDrawValsGR = new valarray<double>(mHeight, mXRes);
    
    mBufferPre = new valarray<double>(0., mTimeScale * sr / (double)mXRes);
    mBufferPost = new valarray<double>(0., mTimeScale * sr / (double)mXRes);
    mBufferGR = new valarray<double>(-2, mTimeScale * sr / (double)mXRes);

    setResolution(kHighRes);
    setLineWeight(2.);
}

IGRPlotControl::~IGRPlotControl(){
    delete mDrawValsPre;
    delete mDrawValsPost;
    delete mDrawValsGR;
    delete mBufferPre;
    delete mBufferPost;
}



void IGRPlotControl::setResolution(int res){
    mRes = res;
    switch (mRes) {
        case kLowRes:
            mXRes = mWidth / 8.;
            break;
        case kMidRes:
            mXRes = mWidth / 4.;
            break;
            
        case kHighRes:
            mXRes = mWidth / 2.;
            break;
            
        case kMaxRes:
            mXRes = mWidth;
            break;
            
        default:
            mXRes = mWidth / 2.;
            break;
    }
    
    if(mRetina)
        mXRes /= 2;
    
    mBufferPre->resize(mTimeScale * sr / (double)mXRes, -48.);
    mBufferPost->resize(mTimeScale * sr / (double)mXRes, -48.);
    mBufferGR->resize(mTimeScale * sr / (double)mXRes, 0);

    mDrawValsPre->resize(mXRes, mHeight);
    mDrawValsPost->resize(mXRes, mHeight);
    mDrawValsGR->resize(mXRes, -2);
    
    mBufferLength = 0;
    
    mSpacing = mWidth / mXRes;
}

void IGRPlotControl::setYRange(int yRangeDB){
    switch (yRangeDB) {
        case k16dB:
            mYRange = -16;
            break;
            
        case k32dB:
            mYRange = -32;
            break;
            
        case k48dB:
            mYRange = -48;
            break;
            
        default:
            break;
    }
}

void IGRPlotControl::setGradientFill(bool enabled){
    mGradientFill = enabled;
}

void IGRPlotControl::process(double sampleIn, double sampleOut, double sampleGR){
    mBufferPre->operator[](mBufferLength) = sampleIn;
    mBufferPost->operator[](mBufferLength) = sampleOut;
    mBufferGR->operator[](mBufferLength) = sampleGR;

    mBufferLength++;
    
    if(mBufferLength >= mBufferPre->size()){
        double averagePre, averagePost, averageGR;
        
        *mDrawValsPre = mDrawValsPre->shift(1);
        *mDrawValsPost = mDrawValsPost->shift(1);
        *mDrawValsGR = mDrawValsGR->shift(1);
        
        averagePre = mBufferPre->sum() / (double)mBufferPre->size();
        averagePost = mBufferPost->sum() / (double)mBufferPost->size();
        averageGR = mBufferGR->sum() / (double)mBufferGR->size();
        
        averagePre = scaleValue(averagePre, mYRange, mHeadroom, 0, 1);
        mDrawValsPre->operator[](mDrawValsPre->size() - 1) = percentToCoordinates(averagePre);
        
        averagePost = scaleValue(averagePost, mYRange, mHeadroom, 0, 1);
        mDrawValsPost->operator[](mDrawValsPost->size() - 1) = percentToCoordinates(averagePost);
        
        averageGR = scaleValue(averageGR, mYRange, mHeadroom, 0, 1);
        mDrawValsGR->operator[](mDrawValsGR->size() - 1) = percentToCoordinates(averageGR);
        
        
        mBufferLength = 0;
    }
}


void IGRPlotControl::checkChangeDPI(IGraphics* pGraphics){
#ifdef IPLUG_RETINA_SUPPORT
    if(pGraphics->IsRetina() != mRetina){   //Check if Retina state has changed
        mRetina = pGraphics->IsRetina();    //Update retina state
        if(mRetina){
            mWidth = mRECT.W() * 2;
            mHeight = mRECT.H() * 2;
        }
        else{
            mWidth = mRECT.W();
            mHeight = mRECT.H();
        }
        
        setResolution(mRes);
        
        cairo_surface_destroy(surface);
        cairo_destroy(cr);
        
        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mWidth, mHeight);
        cr = cairo_create(surface);
    }
#endif
}


bool IGRPlotControl::Draw(IGraphics* pGraphics){
#ifdef IPLUG_RETINA_SUPPORT
    checkChangeDPI(pGraphics);
#endif
    cairo_save(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    cairo_restore(cr);
    
    //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mWidth, mHeight);
    //cr = cairo_create(surface);
    
    if(mRetina){
        cairo_set_line_width(cr, mLineWeight * 2);
    }
    else{
        cairo_set_line_width(cr, mLineWeight);
    }
    ////////////////////////////////////////////////////////////////////////////////PRE
    
    //Starting point in bottom left corner.
    cairo_move_to(cr, -4, mHeight+4);
    
    //Draw data points
    for (int i = 0, x = 0; x < mWidth && i < mDrawValsPre->size(); i++) {
        cairo_line_to(cr, x, mDrawValsPre->operator[](i));
        x += mSpacing;
    }
    
    cairo_line_to(cr, mWidth+4, mDrawValsPre->operator[](mDrawValsPre->size()-1));
    //Endpoint in bottom right corner
    cairo_line_to(cr, mWidth+4, mHeight+4);
    
    cairo_close_path(cr);
    
    cairo_path_t* pathPre = cairo_copy_path(cr);
    
    ////////////////////////////////////////////////////////////////////////////////PRE
    
    ////////////////////////////////////////////////////////////////////////////////POST
    cairo_new_path(cr);

    
    //Starting point in bottom left corner.
    cairo_move_to(cr, -4, mHeight+4);
    
    //Draw data points
    for (int i = 0, x = 0; x < mWidth && i < mDrawValsPost->size(); i++) {
        cairo_line_to(cr, x, mDrawValsPost->operator[](i));
        x += mSpacing;
    }
    
    cairo_line_to(cr, mWidth+4, mDrawValsPre->operator[](mDrawValsPre->size()-1));
    //Endpoint in bottom right corner
    cairo_line_to(cr, mWidth+4, mHeight+4);
    
    cairo_close_path(cr);
    
    cairo_path_t* pathPost = cairo_copy_path(cr);
    
    
    ////////////////////////////////////////////////////////////////////////////////POST
    
    cairo_new_path(cr);

    
    //Starting point in top left corner.
    cairo_move_to(cr, -8, -8);
    
    //Draw data points
    for (int i = 0, x = 0; x < mWidth && i < mDrawValsGR->size(); i++) {
        cairo_line_to(cr, x, mDrawValsGR->operator[](i));
        x += mSpacing;
    }
    
    cairo_line_to(cr, mWidth+8, mDrawValsGR->operator[](mDrawValsGR->size()-1));
    
    //Endpoint in top right corner
    cairo_line_to(cr, mWidth+8, -8);
    
    cairo_close_path(cr);
    
    cairo_path_t* pathGR = cairo_copy_path(cr);
    
    ////////////////////////////////////////////////////////////////////////////////GR
    
    cairo_new_path(cr);
    cairo_append_path(cr, pathPre);
    cairo_set_source_rgba(cr, mPreFillColor.R, mPreFillColor.G, mPreFillColor.B, mPreFillColor.A);
    cairo_fill(cr);
    
    
    cairo_new_path(cr);
    cairo_append_path(cr, pathPost);
    
    if(mGradientFill){
        cairo_pattern_t* grad = cairo_pattern_create_linear(0, 0, 0, mHeight);
        
        cairo_pattern_add_color_stop_rgba(grad, .5, mColorFill.R, mColorFill.G, mColorFill.B, mColorFill.A);
        cairo_pattern_add_color_stop_rgba(grad, 1, mColorFill.R, mColorFill.G, mColorFill.B, 0.3);
        
        cairo_set_source(cr, grad);
        cairo_fill(cr);
        cairo_pattern_destroy(grad);
    }
    else{
        cairo_set_source_rgba(cr, mColorFill.R, mColorFill.G, mColorFill.B, mColorFill.A);
        cairo_fill(cr);
    }
    
    cairo_new_path(cr);

    cairo_append_path(cr, pathPost);
    cairo_set_source_rgba(cr, mColorLine.R, mColorLine.G, mColorLine.B, mColorLine.A);
    

    cairo_stroke(cr);
    

    if(mRetina){
        cairo_set_line_width(cr, mLineWeight * 2 + 2);
    }
    else{
        cairo_set_line_width(cr, mLineWeight + 1);
    }
    
    cairo_new_path(cr);

    cairo_append_path(cr, pathGR);
    cairo_set_source_rgba(cr, mGRLineColor.R, mGRLineColor.G, mGRLineColor.B, mGRLineColor.A);

    cairo_stroke(cr);
    
    cairo_path_destroy(pathPre);
    cairo_path_destroy(pathPost);
    cairo_path_destroy(pathGR);
    
    cairo_surface_flush(surface);
    
    unsigned int *data = (unsigned int*)cairo_image_surface_get_data(surface);
    //Bind to LICE
    LICE_WrapperBitmap WrapperBitmap = LICE_WrapperBitmap(data, mWidth, mHeight, mWidth, false);
    
    //Render
    //}
    IBitmap result;
#ifndef IPLUG_RETINA_SUPPORT
    result = IBitmap(&WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
    
#else
    result = IBitmap(&WrapperBitmap, &WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
#endif
    return pGraphics->DrawBitmap(&result, &this->mRECT);
}


ICompressorPlotControl::ICompressorPlotControl(IPlugBase* pPlug, IRECT pR, IColor* lineColor, IColor* fillColor, compressor* comp, int paramIdx) : ICairoPlotControl(pPlug, pR, paramIdx, fillColor, lineColor, false), mYRange(-32), mHeadroom(2.){
    setLineWeight(2.);
    mComp = comp;
}

void ICompressorPlotControl::calc(){
    double threshCoord = scaleValue(mComp->getThreshold(), mYRange, mHeadroom, 0, mWidth);
    
    x1  = scaleValue(mComp->getKneeBoundL(), mYRange, mHeadroom, 0, mWidth) ;
    y1 = mHeight - x1;
    
    xCP = threshCoord;
    yCP = mHeight - threshCoord;
    
    x2 = scaleValue(mComp->getKneeBoundU(), mYRange, mHeadroom, 0, mWidth);
    y2 = yCP - ((x2 - xCP) / mComp->getRatio());
    
    x3 = mWidth+2;
    
    y3 = yCP - ((mWidth + 2 - xCP) / mComp->getRatio());
    
    SetDirty();
}





bool ICompressorPlotControl::Draw(IGraphics* pGraphics){
    
#ifdef IPLUG_RETINA_SUPPORT
    checkChangeDPI(pGraphics);
#endif
    
    cairo_save(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    cairo_restore(cr);
    
    //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mWidth, mHeight);
    //cr = cairo_create(surface);
    
    if(mRetina){
        cairo_set_line_width(cr, mLineWeight * 2);
    }
    else{
        cairo_set_line_width(cr, mLineWeight);
    }
    
    //fill background
    cairo_set_source_rgba(cr, mColorFill.R, mColorFill.G, mColorFill.B, mColorFill.A);
    cairo_rectangle(cr, 0, 0, mWidth, mHeight);
    cairo_fill(cr);
    
    cairo_set_source_rgba(cr, mColorLine.R, mColorLine.G, mColorLine.B, mColorLine.A);
    
    //Starting point in bottom left corner.
    cairo_move_to(cr, -1, mHeight + 1);
    
    if(mComp->getKnee() > 0.){
        cairo_line_to(cr, x1, y1);
        cairo_curve_to(cr, xCP, yCP, xCP, yCP, x2, y2);
        cairo_line_to(cr, x3, y3);
    }
    else{
        cairo_line_to(cr, xCP, yCP);
        cairo_line_to(cr, x3, y3);
    }
    
    cairo_stroke(cr);
    
    cairo_surface_flush(surface);
    
    unsigned int *data = (unsigned int*)cairo_image_surface_get_data(surface);
    //Bind to LICE
    LICE_WrapperBitmap WrapperBitmap = LICE_WrapperBitmap(data, mWidth, mHeight, mWidth, false);
    
    //Render
    //
    IBitmap result;
#ifndef IPLUG_RETINA_SUPPORT
    result = IBitmap(&WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
#else
    result = IBitmap(&WrapperBitmap, &WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
#endif
    return pGraphics->DrawBitmap(&result, &this->mRECT);
}

IThresholdPlotControl::IThresholdPlotControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IColor* lineColor, compressor* comp) : ICairoPlotControl(pPlug, pR, paramIdx, (IColor*)&COLOR_BLACK, lineColor, false), mYRange(-32), mHeadroom(2)
{
    mComp = comp;
}



bool IThresholdPlotControl::Draw(IGraphics* pGraphics){
    
#ifdef IPLUG_RETINA_SUPPORT
    checkChangeDPI(pGraphics);
#endif
    
    cairo_save(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    cairo_restore(cr);
    
    //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mWidth, mHeight);
    //cr = cairo_create(surface);
    double dashes[] = {6.0,  /* ink */
        3.0,  /* skip */
        6.0,  /* ink */
        3.0   /* skip*/
    };
    
    if(mRetina){
        cairo_set_line_width(cr, mLineWeight * 2);
        for(int i=0; i<4; i++) dashes[i] *= 2;
    }
    else{
        cairo_set_line_width(cr, mLineWeight);
    }
    
    cairo_set_source_rgba(cr, mColorLine.R, mColorLine.G, mColorLine.B, mColorLine.A);
    

    int    ndash  = sizeof (dashes)/sizeof(dashes[0]);
    double offset = -5.0;
    
    cairo_set_dash (cr, dashes, ndash, offset);
    
    double threshCoord = scaleValue(mComp->getThreshold(), mYRange, mHeadroom, 0, mHeight);
    
    cairo_move_to(cr, threshCoord, 0);
    
    cairo_line_to(cr, threshCoord, mHeight);
    
    cairo_stroke(cr);
    
    cairo_move_to(cr, 0, mHeight - threshCoord);
    cairo_line_to(cr, mWidth, mHeight-threshCoord);
    
    cairo_stroke(cr);
    
    cairo_pattern_t* grad = cairo_pattern_create_linear(mHeight, 0, mHeight+5, 0);
    
    cairo_pattern_add_color_stop_rgba(grad, 0, .1, .1, .1, .4);
    cairo_pattern_add_color_stop_rgba(grad, 1, .1, .1, .1, 0);
    
    cairo_set_source(cr, grad);
    
    cairo_rectangle(cr, mHeight, 0, mWidth, mHeight);
    
    cairo_fill(cr);
    cairo_pattern_destroy(grad);
    
    cairo_surface_flush(surface);
    
    unsigned int *data = (unsigned int*)cairo_image_surface_get_data(surface);
    //Bind to LICE
    LICE_WrapperBitmap WrapperBitmap = LICE_WrapperBitmap(data, mWidth, mHeight, mWidth, false);
    
    //Render
    //}
#ifndef IPLUG_RETINA_SUPPORT
    IBitmap result(&WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
#else
    IBitmap result(&WrapperBitmap, &WrapperBitmap, WrapperBitmap.getWidth(), WrapperBitmap.getHeight());
#endif
    return pGraphics->DrawBitmap(&result, &this->mRECT);
}


