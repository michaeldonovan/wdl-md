#ifndef _ICONTROL_
#define _ICONTROL_

#include "IPlugBase.h"
#include "IGraphics.h"
#include <valarray>
#include "../IPlug/DSP/DSP.h"
#include "../Cairo/include/cairo.h"

// A control is anything on the GUI, it could be a static bitmap, or
// something that moves or changes.  The control could manipulate
// bitmaps or do run-time vector drawing, or whatever.
//
// Some controls respond to mouse actions, either by moving a bitmap,
// transforming a bitmap, or cycling through a set of bitmaps.
// Other controls are readouts only.

#define DEFAULT_TEXT_ENTRY_LEN 7

class IControl
{
public:
  // If paramIdx is > -1, this control will be associated with a plugin parameter.
  IControl(IPlugBase* pPlug, IRECT pR, int paramIdx = -1, IChannelBlend blendMethod = IChannelBlend::kBlendNone)
    : mPlug(pPlug), mRECT(pR), mTargetRECT(pR), mParamIdx(paramIdx), mValue(0.0), mDefaultValue(-1.0),
      mBlend(blendMethod), mDirty(true), mHide(false), mGrayed(false), mDisablePrompt(true), mDblAsSingleClick(false),
      mClampLo(0.0), mClampHi(1.0), mMOWhenGreyed(false), mTextEntryLength(DEFAULT_TEXT_ENTRY_LEN), 
      mValDisplayControl(0), mNameDisplayControl(0), mTooltip(NULL) {}

  virtual ~IControl() {}

  virtual void OnMouseDown(int x, int y, IMouseMod* pMod);
  virtual void OnMouseUp(int x, int y, IMouseMod* pMod) {}
  virtual void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod) {}
  virtual void OnMouseDblClick(int x, int y, IMouseMod* pMod);
  virtual void OnMouseWheel(int x, int y, IMouseMod* pMod, int d);
  virtual bool OnKeyDown(int x, int y, int key) { return false; }

  // For efficiency, mouseovers/mouseouts are ignored unless you call IGraphics::HandleMouseOver.
  virtual void OnMouseOver(int x, int y, IMouseMod* pMod) {}
  virtual void OnMouseOut() {}

  // By default, mouse double click has its own handler.  A control can set mDblAsSingleClick to true to change,
  // which maps double click to single click for this control (and also causes the mouse to be
  // captured by the control on double click).
  bool MouseDblAsSingleClick() { return mDblAsSingleClick; }

  virtual bool Draw(IGraphics* pGraphics) = 0;

  // Ask the IGraphics object to open an edit box so the user can enter a value for this control.
  void PromptUserInput();
  void PromptUserInput(IRECT* pTextRect);
  
  inline void SetTooltip(const char* tooltip) { mTooltip = tooltip; }
  inline const char* GetTooltip() const { return mTooltip; }

  int ParamIdx() { return mParamIdx; }
  IParam *GetParam() { return mPlug->GetParam(mParamIdx); }
  virtual void SetValueFromPlug(double value);
  void SetValueFromUserInput(double value);
  double GetValue() { return mValue; }

  IText* GetText() { return &mText; }
  int GetTextEntryLength() { return mTextEntryLength; }
  void SetText(IText* txt) { mText = *txt; }
  IRECT* GetRECT() { return &mRECT; }       // The draw area for this control.
  IRECT* GetTargetRECT() { return &mTargetRECT; } // The mouse target area (default = draw area).
  void SetTargetArea(IRECT pR) { mTargetRECT = pR; }
  virtual void TextFromTextEntry( const char* txt ) { return; } // does nothing by default

  virtual void Hide(bool hide);
  bool IsHidden() const { return mHide; }

  virtual void GrayOut(bool gray);
  bool IsGrayed() { return mGrayed; }

  bool GetMOWhenGrayed() { return mMOWhenGreyed; }

  // Override if you want the control to be hit only if a visible part of it is hit, or whatever.
  virtual bool IsHit(int x, int y) { return mTargetRECT.Contains(x, y); }

  void SetBlendMethod(IChannelBlend::EBlendMethod blendMethod) { mBlend = IChannelBlend(blendMethod); }
  
  void SetValDisplayControl(IControl* pValDisplayControl) { mValDisplayControl = pValDisplayControl; }
  void SetNameDisplayControl(IControl* pNameDisplayControl) { mNameDisplayControl = pNameDisplayControl; }

  virtual void SetDirty(bool pushParamToPlug = true);
  virtual void SetClean();
  virtual bool IsDirty() { return mDirty; }
  void Clamp(double lo, double hi) { mClampLo = lo; mClampHi = hi; }
  void DisablePrompt(bool disable) { mDisablePrompt = disable; }  // Disables the right-click manual value entry.

  // Sometimes a control changes its state as part of its Draw method.
  // Redraw() prevents the control from being cleaned immediately after drawing.
  void Redraw() { mRedraw = true; }

  // This is an idle call from the GUI thread, as opposed to
  // IPlugBase::OnIdle which is called from the audio processing thread.
  // Only active if USE_IDLE_CALLS is defined.
  virtual void OnGUIIdle() {}
  
  // a struct that contain a parameter index and normalized value
  struct AuxParam 
  {
    double mValue;
    int mParamIdx;
    
    AuxParam(int idx) : mParamIdx(idx)
    {
      assert(idx > -1); // no negative params please
    }
  };
  
  // return a pointer to the AuxParam instance at idx in the mAuxParams array
  AuxParam* GetAuxParam(int idx);
  // return the index of the auxillary parameter that holds the paramIdx
  int AuxParamIdx(int paramIdx);
  // add an auxilliary parameter linked to paramIdx
  void AddAuxParam(int paramIdx);
  virtual void SetAuxParamValueFromPlug(int auxParamIdx, double value); // can override if nessecary
  void SetAllAuxParamsFromGUI();
  int NAuxParams() { return mAuxParams.GetSize(); }
  
protected:
  int mTextEntryLength;
  IText mText;
  IPlugBase* mPlug;
  IRECT mRECT, mTargetRECT;
  int mParamIdx;
  
  WDL_TypedBuf<AuxParam> mAuxParams;
  double mValue, mDefaultValue, mClampLo, mClampHi;
  bool mDirty, mHide, mGrayed, mRedraw, mDisablePrompt, mClamped, mDblAsSingleClick, mMOWhenGreyed;
  IChannelBlend mBlend;
  IControl* mValDisplayControl;
  IControl* mNameDisplayControl;
  const char* mTooltip;
};

enum EDirection { kVertical, kHorizontal };

// Fills a rectangle with a colour
class IPanelControl : public IControl
{
public:
  IPanelControl(IPlugBase *pPlug, IRECT pR, const IColor* pColor)
    : IControl(pPlug, pR), mColor(*pColor) {}

  bool Draw(IGraphics* pGraphics);

protected:
  IColor mColor;
};

// Draws a bitmap, or one frame of a stacked bitmap depending on the current value.
class IBitmapControl : public IControl
{
public:
  IBitmapControl(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap,
                 IChannelBlend::EBlendMethod blendMethod = IChannelBlend::kBlendNone)
    : IControl(pPlug, IRECT(x, y, pBitmap), paramIdx, blendMethod), mBitmap(*pBitmap) {}

  IBitmapControl(IPlugBase* pPlug, int x, int y, IBitmap* pBitmap,
                 IChannelBlend::EBlendMethod blendMethod = IChannelBlend::kBlendNone)
    : IControl(pPlug, IRECT(x, y, pBitmap), -1, blendMethod), mBitmap(*pBitmap) {}

  virtual ~IBitmapControl() {}

  virtual bool Draw(IGraphics* pGraphics);

protected:
  IBitmap mBitmap;
};

// A switch.  Click to cycle through the bitmap states.
class ISwitchControl : public IBitmapControl
{
public:
  ISwitchControl(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap,
                 IChannelBlend::EBlendMethod blendMethod = IChannelBlend::kBlendNone)
    : IBitmapControl(pPlug, x, y, paramIdx, pBitmap, blendMethod) {}
  ~ISwitchControl() {}

  void OnMouseDblClick(int x, int y, IMouseMod* pMod);
  void OnMouseDown(int x, int y, IMouseMod* pMod);
};

// Like ISwitchControl except it puts up a popup menu instead of cycling through states on click
class ISwitchPopUpControl : public ISwitchControl
{
public:
  ISwitchPopUpControl(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap,
                 IChannelBlend::EBlendMethod blendMethod = IChannelBlend::kBlendNone)
  : ISwitchControl(pPlug, x, y, paramIdx, pBitmap, blendMethod)
  {
    mDisablePrompt = false;
  }
  
  ~ISwitchPopUpControl() {}
  
  void OnMouseDown(int x, int y, IMouseMod* pMod);
};

// A switch where each frame of the bitmap contains images for multiple button states. The Control's mRect will be divided into clickable areas.
class ISwitchFramesControl : public ISwitchControl
{
public:
  ISwitchFramesControl(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap, bool imagesAreHorizontal = false,
                       IChannelBlend::EBlendMethod blendMethod = IChannelBlend::kBlendNone);
  
  ~ISwitchFramesControl() {}
  
  void OnMouseDown(int x, int y, IMouseMod* pMod);
  
protected:
  WDL_TypedBuf<IRECT> mRECTs;
};

// On/off switch that has a target area only.
class IInvisibleSwitchControl : public IControl
{
public:
  IInvisibleSwitchControl(IPlugBase* pPlug, IRECT pR, int paramIdx);
  ~IInvisibleSwitchControl() {}

  void OnMouseDown(int x, int y, IMouseMod* pMod);

  virtual bool Draw(IGraphics* pGraphics) { return true; }
};

// A set of buttons that maps to a single selection.  Bitmap has 2 states, off and on.
class IRadioButtonsControl : public IControl
{
public:
  IRadioButtonsControl(IPlugBase* pPlug, IRECT pR, int paramIdx, int nButtons, IBitmap* pBitmap,
                       EDirection direction = kVertical, bool reverse = false);
  ~IRadioButtonsControl() {}

  void OnMouseDown(int x, int y, IMouseMod* pMod);
  bool Draw(IGraphics* pGraphics);

protected:
  WDL_TypedBuf<IRECT> mRECTs;
  IBitmap mBitmap;
};

// A switch that reverts to 0.0 when released.
class IContactControl : public ISwitchControl
{
public:
  IContactControl(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap)
    : ISwitchControl(pPlug, x, y, paramIdx, pBitmap) {}
  ~IContactControl() {}

  void OnMouseUp(int x, int y, IMouseMod* pMod);
};

// A fader. The bitmap snaps to a mouse click or drag.
class IFaderControl : public IControl
{
public:
  IFaderControl(IPlugBase* pPlug, int x, int y, int len, int paramIdx, IBitmap* pBitmap,
                EDirection direction = kVertical, bool onlyHandle = false);
  ~IFaderControl() {}

  int GetLength() const { return mLen; }
  // Size of the handle in pixels.
  int GetHandleHeadroom() const { return mHandleHeadroom; }
  // Size of the handle in terms of the control value.
  double GetHandleValueHeadroom() const { return (double) mHandleHeadroom / (double) mLen; }
  // Where is the handle right now?
  IRECT GetHandleRECT(double value = -1.0) const;

  virtual void OnMouseDown(int x, int y, IMouseMod* pMod);
  virtual void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod);

  virtual bool Draw(IGraphics* pGraphics);
  
  virtual bool IsHit(int x, int y);

protected:
  virtual void SnapToMouse(int x, int y);
  int mLen, mHandleHeadroom;
  IBitmap mBitmap;
  EDirection mDirection;
  bool mOnlyHandle; // if true only by clicking on the handle do you click the slider
};

const double DEFAULT_GEARING = 4.0;

// Parent for knobs, to handle mouse action and ballistics.
class IKnobControl : public IControl
{
public:
  IKnobControl(IPlugBase* pPlug, IRECT pR, int paramIdx, EDirection direction = kVertical,
               double gearing = DEFAULT_GEARING)
    : IControl(pPlug, pR, paramIdx), mDirection(direction), mGearing(gearing) {}
  virtual ~IKnobControl() {}

  void SetGearing(double gearing) { mGearing = gearing; }
  virtual void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod);

protected:
  EDirection mDirection;
  double mGearing;
};

// A knob that is just a line.
class IKnobLineControl : public IKnobControl
{
public:
  IKnobLineControl(IPlugBase* pPlug, IRECT pR, int paramIdx,
                   const IColor* pColor, double innerRadius = 0.0, double outerRadius = 0.0,
                   double minAngle = -0.75 * PI, double maxAngle = 0.75 * PI,
                   EDirection direction = kVertical, double gearing = DEFAULT_GEARING);
  ~IKnobLineControl() {}

  bool Draw(IGraphics* pGraphics);

protected:
  IColor mColor;
  float mMinAngle, mMaxAngle, mInnerRadius, mOuterRadius;
};

// A rotating knob.  The bitmap rotates with any mouse drag.
class IKnobRotaterControl : public IKnobControl
{
public:
  IKnobRotaterControl(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap,
                      double minAngle = -0.75 * PI, double maxAngle = 0.75 * PI, int yOffsetZeroDeg = 0,
                      EDirection direction = kVertical, double gearing = DEFAULT_GEARING)
    : IKnobControl(pPlug, IRECT(x, y, pBitmap), paramIdx, direction, gearing),
      mBitmap(*pBitmap), mMinAngle(minAngle), mMaxAngle(maxAngle), mYOffset(yOffsetZeroDeg) {}
  ~IKnobRotaterControl() {}

  bool Draw(IGraphics* pGraphics);

protected:
  IBitmap mBitmap;
  double mMinAngle, mMaxAngle;
  int mYOffset;
};

// A multibitmap knob.  The bitmap cycles through states as the mouse drags.
class IKnobMultiControl : public IKnobControl
{
public:
  IKnobMultiControl(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap,
                    EDirection direction = kVertical, double gearing = DEFAULT_GEARING)
    : IKnobControl(pPlug, IRECT(x, y, pBitmap), paramIdx, direction, gearing), mBitmap(*pBitmap) {}
  ~IKnobMultiControl() {}

  bool Draw(IGraphics* pGraphics);

protected:
  IBitmap mBitmap;
};


// A knob that consists of a static base, a rotating mask, and a rotating top.
// The bitmaps are assumed to be symmetrical and identical sizes.
class IKnobRotatingMaskControl : public IKnobControl
{
public:
  IKnobRotatingMaskControl(IPlugBase* pPlug, int x, int y, int paramIdx,
                           IBitmap* pBase, IBitmap* pMask, IBitmap* pTop,
                           double minAngle = -0.75 * PI, double maxAngle = 0.75 * PI,
                           EDirection direction = kVertical, double gearing = DEFAULT_GEARING)
    : IKnobControl(pPlug, IRECT(x, y, pBase), paramIdx, direction, gearing),
      mBase(*pBase), mMask(*pMask), mTop(*pTop), mMinAngle(minAngle), mMaxAngle(maxAngle) {}
  ~IKnobRotatingMaskControl() {}

  bool Draw(IGraphics* pGraphics);

protected:
  IBitmap mBase, mMask, mTop;
  double mMinAngle, mMaxAngle;
};

// Bitmap shows when value = 0, then toggles its target area to the whole bitmap
// and waits for another click to hide itself.
class IBitmapOverlayControl : public ISwitchControl
{
public:
  IBitmapOverlayControl(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap, IRECT pTargetArea)
    : ISwitchControl(pPlug, x, y, paramIdx, pBitmap), mTargetArea(pTargetArea) {}

  IBitmapOverlayControl(IPlugBase* pPlug, int x, int y, IBitmap* pBitmap, IRECT pTargetArea)
    : ISwitchControl(pPlug, x, y, -1, pBitmap), mTargetArea(pTargetArea) {}

  ~IBitmapOverlayControl() {}

  bool Draw(IGraphics* pGraphics);

protected:
  IRECT mTargetArea;  // Keep this around to swap in & out.
};

// Output text to the screen.
class ITextControl : public IControl
{
public:
  ITextControl(IPlugBase* pPlug, IRECT pR, IText* pText, const char* str = "")
    : IControl(pPlug, pR)
  {
    mText = *pText;
    mStr.Set(str);
  }
  ~ITextControl() {}

  void SetTextFromPlug(char* str);
  void ClearTextFromPlug() { SetTextFromPlug( (char *) ""); }

  bool Draw(IGraphics* pGraphics);

protected:
  WDL_String mStr;
};

// If paramIdx is specified, the text is automatically set to the output
// of Param::GetDisplayForHost().  If showParamLabel = true, Param::GetLabelForHost() is appended.
class ICaptionControl : public ITextControl
{
public:
  ICaptionControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IText* pText, bool showParamLabel = true);
  ~ICaptionControl() {}

  virtual void OnMouseDown(int x, int y, IMouseMod* pMod);
  virtual void OnMouseDblClick(int x, int y, IMouseMod* pMod);

  bool Draw(IGraphics* pGraphics);

protected:
  bool mShowParamLabel;
};

#define MAX_URL_LEN 256
#define MAX_NET_ERR_MSG_LEN 1024

class IURLControl : public IControl
{
public:
  IURLControl(IPlugBase* pPlug, IRECT pR, const char* url, const char* backupURL = 0, const char* errMsgOnFailure = 0);
  ~IURLControl() {}

  void OnMouseDown(int x, int y, IMouseMod* pMod);
  bool Draw(IGraphics* pGraphics) { return true; }

protected:
  char mURL[MAX_URL_LEN], mBackupURL[MAX_URL_LEN], mErrMsg[MAX_NET_ERR_MSG_LEN];
};

// This is a weird control for a few reasons.
// - Although its numeric mValue is not meaningful, it needs to be associated with a plugin parameter
// so it can inform the plug when the file selection has changed. If the associated plugin parameter is
// declared after kNumParams in the EParams enum, the parameter will be a dummy for this purpose only.
// - Because it puts up a modal window, it needs to redraw itself twice when it's dirty,
// because moving the modal window will clear the first dirty state.
class IFileSelectorControl : public IControl
{
public:
  enum EFileSelectorState { kFSNone, kFSSelecting, kFSDone };

  IFileSelectorControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IBitmap* pBitmap,
                       EFileAction action, char* dir = "", char* extensions = "")     // extensions = "txt wav" for example.
    : IControl(pPlug, pR, paramIdx), mBitmap(*pBitmap),
      mFileAction(action), mDir(dir), mExtensions(extensions), mState(kFSNone) {}
  ~IFileSelectorControl() {}

  void OnMouseDown(int x, int y, IMouseMod* pMod);

  void GetLastSelectedFileForPlug(WDL_String* pStr);
  void SetLastSelectedFileFromPlug(char* file);

  bool Draw(IGraphics* pGraphics);
  bool IsDirty();

protected:
  IBitmap mBitmap;
  WDL_String mDir, mFile, mExtensions;
  EFileAction mFileAction;
  EFileSelectorState mState;
};


class IKnobMultiControlText : public IKnobMultiControl
{
private:
    IRECT mTextRECT, mImgRECT;
    IBitmap mBitmap;
    bool mShowParamLabel;
    
public:
    IKnobMultiControlText(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap, IText* pText, bool showParamLabel=true, int offset = 0);
    
    ~IKnobMultiControlText();
    
    bool Draw(IGraphics* pGraphics);
    
    void OnMouseDown(int x, int y, IMouseMod* pMod);
    
    void OnMouseDblClick(int x, int y, IMouseMod* pMod);
    
    void setCaptionOffset(int offset);
};

/**
 *  An IKnobMultiControl with mouse snapping functionality for use as a fader.
 *
 */
class IFaderControlText : public IKnobMultiControl
{
private:
    IRECT mTextRECT, mImgRECT;
    IBitmap mBitmap;
    bool mShowParamLabel;
    
public:
    IFaderControlText(IPlugBase* pPlug, int x, int y, int paramIdx, IBitmap* pBitmap, IText* pText, bool showParamLabel=true, int offset = 0);
    
    ~IFaderControlText();
    
    bool Draw(IGraphics* pGraphics);
    
    void OnMouseDown(int x, int y, IMouseMod* pMod);

    void OnMouseDblClick(int x, int y, IMouseMod* pMod);
    
    void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod);

    void SnapToMouse(int x, int y);

    void setCaptionOffset(int offset);
};



using std::valarray;

/**
 *  A struct for storing color data for use with Cairo
 *  Can be initialized with a pointer to an IColor
 */
struct CColor{
public:
    double A, R, G, B;
    
    /**
     *  Constructor
     *  @param ic A pointer to an IColor
     */
    CColor(IColor* ic){
        A = ic->A / 255.;
        R = ic->R / 255.;
        G = ic->G / 255.;
        B = ic->B / 255.;
    }
    
    /**
     *  Constructor
     *  @param a Alpha value in range [0,1]
     *  @param r Red value in range [0,1]
     *  @param g Green value in range [0,1]
     *  @param b Blue value in range [0,1]
     */
    CColor(double a = 1., double r = 0., double g = 0., double b = 0.){
        A = a;
        R = r;
        G = g;
        B = b;
        Clamp();
    }
    
    bool operator==(const CColor& rhs) { return (rhs.A == A && rhs.R == R && rhs.G == G && rhs.B == B); }
    
    bool operator!=(const CColor& rhs) { return !operator==(rhs); }
    
    bool Empty() const { return A == 0 && R == 0 && G == 0 && B == 0; }
    
    void Clamp() { A = IPMIN(A, 1.); R = IPMIN(R, 1.); G = IPMIN(G, 1.); B = IPMIN(B, 1.); }
    
    void setFromIColor(IColor* ic){
        A = ic->A / 255.;
        R = ic->R / 255.;
        G = ic->G / 255.;
        B = ic->B / 255.;
    }
};


/**
 * An IControl that plots a set of data points using Cairo
 */
class ICairoPlotControl : public IControl
{
public:
    /**
     Antialiasing quality options
     */
    enum AAQuality{
        kNone,
        kFast,
        kGood,
        kBest
    };
    
    
    ICairoPlotControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IColor* fillColor, IColor* lineColor, bool fillEnable=true);
    
    ~ICairoPlotControl();
    
    /**
     *  Set whether or not a fill will be drawn under the plot points
     *
     *  @param b True=Enabled, False=Disabled
     */
    void setFillEnable(bool b);
    
    /**
     *  Set the color of the plot line
     *
     *  @param color A Pointer to an IColor
     */
    void setLineColor(IColor* color);
    
    /**
     *  Set the color of the fill
     *
     *  @param color A pointer to an IColor
     */
    void setFillColor(IColor* color);
    
    /**
     *  Set antialiasing quality
     *
     *  @see AAQualtiy
     *  @param quality An int in range [0,4]
     */
    void setAAquality(int quality);
    
    /**
     *  Set the line weight of the plot
     *
     *  @param w Line weight in pixels
     */
    void setLineWeight(double w);
    
    /**
     *  Set the Y-axis range
     *
     *  @param range The Y-axis range
     */
    void setRange(double range);
    
    /*
     *  Plot a set of values
     *  Points will be evenly spaced along the horizontal axis
     *
     *  @param vals Pointer to a valarray of doubles to be plotted
     *  @param normalize If true, values will be scaled so that the max value is at the top of the plot. Default = false
     */
    void plotVals(valarray<double>* vals, bool normalize=false);
    
    /**
     *  Draw the plot. To be called by IGraphics.
     *
     *  @param pGraphics Pointer to IGraphics
     *
     *  @return True if drawn
     */
    bool Draw(IGraphics* pGraphics);
    
    //Accessors//
    CColor getColorFill();
    CColor getColorLine();
    bool getFill();
    int getWidth();
    int getHeight();
    double getRange();
    
protected:
    CColor mColorFill;
    CColor mColorLine;
    bool mFill;
    int mWidth, mHeight;
    double mRange, mLineWeight;
    valarray<double>* mVals;
    cairo_surface_t *surface;
    cairo_t *cr;
    bool mRetina;
    
    
    
    /**
     *  Checks for changes in DPI state of display, updates cairo graphics context accordingly
     *
     *  @param pGraphics A pointer to IGraphics
     */
    virtual void checkChangeDPI(IGraphics* pGraphics);

    
    
    /**
     *  Scale a value from range [inMin, inMax] to [outMin, outMax]
     *
     *  @param inValue Value to be scaled
     *  @param inMin   Min of input range
     *  @param inMax   Max of input range
     *  @param outMin  Min of output range
     *  @param outMax  Max of output range
     *
     *  @return Scaled value
     */
    inline double scaleValue(double inValue, double inMin, double inMax, double outMin, double outMax);
    
    
    inline double percentToCoordinates(double value);
    
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class ILevelPlotControl : public ICairoPlotControl
{
public:
    /**
     *  Resolution settings, determines number of data points to be plotted
     *  kLowRes -> mRECT.W() / 8.
     *  kMidRes -> mRECT.W() / 4.
     *  kHighRes -> mRECT.W() / 2.
     *  kMaxRes -> mRECT.W()
     */
    enum kResolution{
        kLowRes,
        kMidRes,
        kHighRes,
        kMaxRes
    };
    
    
    /**
     *  Y-axis range settings. Determines min value of Y-axis
     */
    enum kYRange{
        k16dB,
        k32dB,
        k48dB
    };
    
    
    /**
     *  Constructor
     *
     *  @param pPlug        Pointer to IPlugBase
     *  @param pR           IRECT for the plot
     *  @param fillColor    Pointer to an IColor for plot fill
     *  @param lineColor    Pointer to an IColor for plot line
     *  @param timeScale    X-Axis range in seconds (Default = 5)
     *  @param paramIdx     Parameter index for IControl (Default = -1)
     */
    ILevelPlotControl(IPlugBase* pPlug, IRECT pR, IColor* fillColor, IColor* lineColor, double timeScale=5., bool fillEnable=true, int paramIdx=-1);
    
    ~ILevelPlotControl();
    
    /**
     *  If enabled, fill will be drawn above line instead of below
     *
     *  @param rev True = reverse fill
     */
    void setReverseFill(bool rev);
    
    /**
     *  Set the horizontal resolution of the plot (number of points to be plotted)
     *
     *  @see kResolution
     *  @param res Resolution value in range [0,3]
     */
    void setResolution(int res);
    
    /**
     *  Set the minimum value of the Y-Axis
     *
     *  @see   kYRange
     *  @param yRangeDB Range value
     */
    void setYRange(int yRangeDB);
    
    /**
     *  Set whether or not the plot line will be drawn
     *
     *  @param stroke True = enabled
     */
    void setStroke(bool stroke);
    
    /**
     *  If true, plot fill will be a vertical, linear gradient from mColorFill to transparent
     *
     *  @param grad True = gradient fill enabled
     */
    void setGradientFill(bool grad);
    
    /**
     *  Takes a sample of audio to be added to the plot
     *  For a smooth plot, sample should be processed by an envelope follower
     *  @param sample A sample of audio
     */
    void process(double sample);
    
    /**
     *  Draws the plot
     *
     *  @param pGraphics Pointer to IGraphics
     *
     *  @return True if drawn
     */
    bool Draw(IGraphics* pGraphics);
    
protected:
    double mTimeScale;
    int mBufferLength, mXRes, mRes, mSpacing, mYRange, mHeadroom;
    valarray<double> *mBuffer, *mDrawVals;
    bool mStroke, mReverseFill, mGradientFill;
    
    /**
     *  Checks for changes in DPI state of display, updates cairo graphics context accordingly
     *
     *  @param pGraphics A pointer to IGraphics
     */
    virtual void checkChangeDPI(IGraphics* pGraphics);

};


class IGRPlotControl : public ICairoPlotControl{
public:
    enum kResolution{
        kLowRes,
        kMidRes,
        kHighRes,
        kMaxRes
    };
    
    enum kYRange{
        k16dB,
        k32dB,
        k48dB
    };
    
    IGRPlotControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IColor* preFillColor, IColor* postFillColor, IColor* postLineColor, IColor* GRFillColor, IColor* GRLineColor, double timeScale=5.);
    
    ~IGRPlotControl();
    
    void setResolution(int res);
    
    void setYRange(int yRangeDB);
    
    void setGradientFill(bool enabled);
    
    void process(double sampleIn, double sampleOut, double sampleGR);
    
    void checkChangeDPI(IGraphics* pGraphics);
    
    bool Draw(IGraphics* pGraphics);
    
protected:
    double mTimeScale, sr;
    int mBufferLength, mXRes, mSpacing, mYRange, mHeadroom, mRes;
    valarray<double> *mBufferPre, *mBufferPost, *mBufferGR, *mDrawValsPre, *mDrawValsPost, *mDrawValsGR;

    bool mGradientFill;
    
    CColor mPreFillColor, mGRLineColor, mGRFillColor;
    
    
};


/**
 *  An ICairoPlotControl class for plotting response curve of a compressor
 *  Designed to be overlayed on an ILevelPlotControl
 *
 *  @see compressor
 *  @see ICairoPlotControl
 *  @see ILevelPlotControl
 */
class ICompressorPlotControl : public ICairoPlotControl
{
public:
    /**
     *  Constructor
     *
     *  @param pPlug        Pointer to IPlugBase
     *  @param pR           IRECT
     *  @param lineColor    Pointer to an IColor
     *  @param fillColor    Pointer to an IColor
     *  @param comp         Pointer to a compressor
     *  @param paramIdx     Parameter index (Default = -1)
     */
    ICompressorPlotControl(IPlugBase* pPlug, IRECT pR, IColor* lineColor, IColor* fillColor, compressor* comp, int paramIdx=-1);
    
    /**
     *  Update the compressor response curve. Call when compressor settings are changed.
     */
    void calc();
    
    /**
     *  Draw the plot
     *
     *  @param pGraphics Pointer to IGraphics
     *
     *  @return True if drawn
     */
    bool Draw(IGraphics* pGraphics);
    
private:
    double mHeadroom;
    
    /**
     *  Coordinates of lower bound of knee
     */
    double x1, y1;
    
    /**
     *  Coordinates of control point for knee bezier curve
     */
    double xCP, yCP;
    
    /**
     *  Coordinates of upper bound of knee
     */
    double x2, y2;
    
    /**
     *  Coordinates where response curve exits mRECT
     */
    double x3, y3;
    int mYRange;
    compressor *mComp;
};


/**
 *  An ICairoPlotControl class for plotting the threshold of a compressor
 *  Designed to be overlayed on an ILevelPlotControl and ICompressorControl
 *
 *  @see compressor
 *  @see ICairoPlotControl
 *  @see ILevelPlotControl
 *  @see ICompressorControl
 */
class IThresholdPlotControl : public ICairoPlotControl
{
public:
    IThresholdPlotControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IColor* lineColor, compressor* comp);
    
    
    bool Draw(IGraphics* pGraphics);
    
private:
    int mYRange;
    double mHeadroom;
    compressor* mComp;
};

#endif
