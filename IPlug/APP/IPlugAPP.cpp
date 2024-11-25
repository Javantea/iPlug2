/*
 ==============================================================================
 
 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers. 
 
 See LICENSE.txt for  more info.
 
 ==============================================================================
*/

#include "IPlugAPP.h"
#include "IPlugAPP_host.h"

#if defined OS_MAC || defined OS_LINUX
#include <IPlugSWELL.h>
#else
extern float GetScaleForHWND(HWND hWnd);
#endif
#ifdef OS_LINUX
const int TITLE_BAR_OFFSET = 17;
#endif

using namespace iplug;

extern HWND gHWND;

IPlugAPP::IPlugAPP(const InstanceInfo& info, const Config& config)
: IPlugAPIBase(config, kAPIAPP)
, IPlugProcessor(config, kAPIAPP)
{
  mAppHost = (IPlugAPPHost*) info.pAppHost;
  
  Trace(TRACELOC, "%s%s", config.pluginName, config.channelIOStr);

  SetChannelConnections(ERoute::kInput, 0, MaxNChannels(ERoute::kInput), true);
  SetChannelConnections(ERoute::kOutput, 0, MaxNChannels(ERoute::kOutput), true);

  SetBlockSize(DEFAULT_BLOCK_SIZE);
  
  CreateTimer();

#ifdef OS_LINUX
  // Every 50ms check to see if the main window needs to be resized.
  // This fixes basically all the issues related to resizing the window on Linux.
  mResizeTimer = std::unique_ptr<Timer>(Timer::Create([&](Timer& timer) {
    if (mNeedResize)
    {
      int viewWidth = GetEditorWidth();
      int viewHeight = GetEditorHeight();
      RECT r;
      GetWindowRect(gHWND, &r);
      SetWindowPos(gHWND, 0, r.left, r.bottom - viewHeight - TITLE_BAR_OFFSET, viewWidth, viewHeight + TITLE_BAR_OFFSET, 0);
      mNeedResize = false;
    }
  }, 50));
#endif
}

bool IPlugAPP::EditorResize(int viewWidth, int viewHeight)
{
  bool parentResized = false;
    
  if (viewWidth != GetEditorWidth() || viewHeight != GetEditorHeight())
  {
    #if defined OS_MAC || defined NO_IGRAPHICS 
    RECT rcClient, rcWindow;
    POINT ptDiff;
    
    GetClientRect(gHWND, &rcClient);
    GetWindowRect(gHWND, &rcWindow);
    
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    
    int flags = 0;
    
    #ifdef OS_WIN
    flags = SWP_NOMOVE;
    float ss = GetScaleForHWND(gHWND);
    #else
    float ss = 1.f;
    #endif
    
    SetWindowPos(gHWND, 0, rcWindow.left * ss,
                 (rcWindow.bottom - viewHeight - ptDiff.y) * ss,
                 (viewWidth + ptDiff.x) * ss,
                 (viewHeight + ptDiff.y) * ss, flags);
    parentResized = true;
    #endif
    
    SetEditorSize(viewWidth, viewHeight);
  }
  
  return parentResized;
}

bool IPlugAPP::SendMidiMsg(const IMidiMsg& msg)
{
  if (DoesMIDIOut() && mAppHost->mMidiOut)
  {
    //TODO: midi out channel
//    uint8_t status;
//
//    // if the midi channel out filter is set, reassign the status byte appropriately
//    if(mAppHost->mMidiOutChannel > -1)
//      status = mAppHost->mMidiOutChannel-1 | ((uint8_t) msg.StatusMsg() << 4) ;

    std::vector<uint8_t> message;
    message.push_back(msg.mStatus);
    message.push_back(msg.mData1);
    message.push_back(msg.mData2);

    mAppHost->mMidiOut->sendMessage(&message);
    
    return true;
  }

  return false;
}

bool IPlugAPP::SendSysEx(const ISysEx& msg)
{
  if (DoesMIDIOut() && mAppHost->mMidiOut)
  {
    //TODO: midi out channel
    std::vector<uint8_t> message;
    
    for (int i = 0; i < msg.mSize; i++)
    {
      message.push_back(msg.mData[i]);
    }
    
    mAppHost->mMidiOut->sendMessage(&message);
    return true;
  }
  
  return false;
}

void IPlugAPP::SendSysexMsgFromUI(const ISysEx& msg)
{
  SendSysEx(msg);
}

void IPlugAPP::AppProcess(double** inputs, double** outputs, int nFrames)
{
  SetChannelConnections(ERoute::kInput, 0, MaxNChannels(ERoute::kInput), !IsInstrument()); //TODO: go elsewhere - enable inputs
  SetChannelConnections(ERoute::kOutput, 0, MaxNChannels(ERoute::kOutput), true); //TODO: go elsewhere
  AttachBuffers(ERoute::kInput, 0, NChannelsConnected(ERoute::kInput), inputs, GetBlockSize());
  AttachBuffers(ERoute::kOutput, 0, NChannelsConnected(ERoute::kOutput), outputs, GetBlockSize());
  
  if(mMidiMsgsFromCallback.ElementsAvailable())
  {
    //std::cout << "mMidiMsgsFromCallback " << mMidiMsgsFromCallback.ElementsAvailable() << std::endl;
    IMidiMsg msg;
    
    while (mMidiMsgsFromCallback.Pop(msg))
    {
      // FIXME: When both input and output are connected to midi thru (yes this can happen) we get an issue here that needs to be resolved.
      // Detect and Fix!
      ProcessMidiMsg(msg);
      //std::cout << "processed " << ((int)msg.mStatus) << std::endl;
      mMidiMsgsFromProcessor.Push(msg); // queue incoming MIDI for UI
    }
  }
  
  if(mSysExMsgsFromCallback.ElementsAvailable())
  {
    SysExData data;
    
    while (mSysExMsgsFromCallback.Pop(data))
    {
      ISysEx msg { data.mOffset, data.mData, data.mSize };
      ProcessSysEx(msg);
      mSysExDataFromProcessor.Push(data); // queue incoming Sysex for UI
    }
  }
  
  if(mMidiMsgsFromEditor.ElementsAvailable())
  {
    IMidiMsg msg;

    while (mMidiMsgsFromEditor.Pop(msg))
    {
      ProcessMidiMsg(msg);
    }
  }

  //Do not handle Sysex messages here - SendSysexMsgFromUI overridden

  ENTER_PARAMS_MUTEX
  ProcessBuffers(0.0, GetBlockSize());
  LEAVE_PARAMS_MUTEX
}
