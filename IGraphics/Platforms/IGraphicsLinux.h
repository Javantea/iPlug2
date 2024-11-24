/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

#include "IGraphics_select.h"
#include <memory>
#include <xcbt.h>

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

/** IGraphics platform class for linux
*   @ingroup PlatformClasses
*/
class IGraphicsLinux final : public IGRAPHICS_DRAW_CLASS
{
  class Font;
public:
  IGraphicsLinux(IGEditorDelegate& dlg, int w, int h, int fps, float scale);
  virtual ~IGraphicsLinux();

  void* OpenWindow(void* pWindow) override;
  void CloseWindow() override;
  bool WindowIsOpen() override { return (mPlugWnd); }
  void PlatformResize(bool parentHasResized) override;

  void HideMouseCursor(bool hide, bool lock) override;
  //void ShowMouseCursor() override;
  void GetMouseLocation(float& x, float&y) const override;
  void MoveMouseCursor(float x, float y) override;
  ECursor SetMouseCursor(ECursor cursorType) override;

  EMsgBoxResult ShowMessageBox(const char* str, const char* title, EMsgBoxType type, IMsgBoxCompletionHandlerFunc completionHandler) override;
  void ForceEndUserEdit() override;

  const char* GetPlatformAPIStr() override;

  void UpdateTooltips() override;

  bool RevealPathInExplorerOrFinder(WDL_String& path, bool select) override;
  void PromptForFile(WDL_String& fileName, WDL_String& path, EFileAction action, const char* ext, IFileDialogCompletionHandlerFunc completionHandler) override;
  void PromptForDirectory(WDL_String& dir, IFileDialogCompletionHandlerFunc completionHandler) override;
  bool PromptForColor(IColor& color, const char* str, IColorPickerHandlerFunc func) override;

  bool OpenURL(const char* url, const char* msgWindowTitle, const char* confirmMsg, const char* errMsgOnFailure) override;

  static int GetUserOSVersion();
  bool GetTextFromClipboard(WDL_String& str) override;
  bool SetTextInClipboard(const char* str) override;
  void* GetWindow() override { return (void *)(intptr_t)mPlugWnd; }

  PlatformFontPtr LoadPlatformFont(const char* fontID, const char* fileNameOrResID) override;
  PlatformFontPtr LoadPlatformFont(const char* fontID, const char* fontName, ETextStyle style) override;
  PlatformFontPtr LoadPlatformFont(const char* fontID, void* pData, int dataSize) override;
  void SetIntegration(void* mainLoop);

protected:
  IPopupMenu* CreatePlatformPopupMenu(IPopupMenu& menu, const IRECT bounds, bool& isAsync) override;
  void CreatePlatformTextEntry(int paramIdx, const IText& text, const IRECT& bounds, int length, const char* str) override;
  void RequestFocus();

private:
  void Paint();
  void DrawResize();
  IMouseInfo GetMouseInfo(int16_t x, int16_t y, int16_t state);
  void TimerHandler(int timerID);
  IMouseInfo GetMouseInfoDeltas(float& dX, float& dY, int16_t x, int16_t y, int16_t state);
  static void TimerHandlerProxy(xcbt x, int timer_id, IGraphicsLinux* pGraphics) { pGraphics->TimerHandler(timer_id); }
  void WindowHandler(xcb_generic_event_t* evt);
  static void WindowHandlerProxy(xcbt_window xw, xcb_generic_event_t* evt, IGraphicsLinux* pGraphics) { pGraphics->WindowHandler(evt); }

  xcbt mX = NULL;
  xcbt_embed* mEmbed = NULL;
  xcbt_window mPlugWnd = NULL;
  xcbt_window_handler mBaseWindowHandler;
  void* mBaseWindowData;

  void CachePlatformFont(const char* fontID, const PlatformFontPtr& font) override;
  uint32_t GetUserDblClickTimeout();

  /** Double-click timeout in milliseconds */
  uint32_t mDblClickTimeout = 400;
  xcb_timestamp_t mLastLeftClickStamp; // it will be not zero in case there is a chance for double click

  IVec2 mMouseLockPos;
  bool mMouseVisible;
};

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
