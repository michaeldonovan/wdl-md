class IBezierControl : public IControl
{
public:
  IBezierControl(IPlugBase *pPlug, IRECT pR, int paramA, int paramB)
  : IControl(pPlug, pR)
  {
    AddAuxParam(paramA);
    AddAuxParam(paramB);
  }
  
  bool Draw(IGraphics* pGraphics)
  {
    double xpos = GetAuxParam(0)->mValue * mRECT.W();
    double ypos = GetAuxParam(1)->mValue * mRECT.H();
    
    LICE_DrawQBezier(pGraphics->GetDrawBitmap(), mRECT.MW(), mRECT.T, xpos + 5, ypos + 5, mRECT.MW(), mRECT.B, LICE_RGBA(0,0,0,255),  0.5);
    LICE_DrawQBezier(pGraphics->GetDrawBitmap(), mRECT.MW(), mRECT.T, xpos, ypos, mRECT.MW(), mRECT.B, LICE_RGBA(255,255,255,255));

    return true;
  }
  
  void OnMouseDown(int x, int y, IMouseMod* pMod)
  {
    return SnapToMouse(x, y);
  }

  void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod)
  {
    return SnapToMouse(x, y);
  }
  
  void SnapToMouse(int x, int y)
  {
    GetAuxParam(0)->mValue = BOUNDED((double)x / (double)mRECT.W(), 0, 1);
    GetAuxParam(1)->mValue = BOUNDED((double)y / (double)mRECT.H(), 0, 1);
    
    SetDirty();
  }

  void SetDirty(bool pushParamToPlug = true)
  {
    mDirty = true;
    
    if (pushParamToPlug && mPlug)
    {
      SetAllAuxParamsFromGUI();
    }
  }
};