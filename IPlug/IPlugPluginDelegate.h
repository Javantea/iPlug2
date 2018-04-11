#pragma once

#include <random>

#include "ptrlist.h"

#include "IPlugDelegate.h"
#include "IPlugParameter.h"
#include "IPlugStructs.h"

/** This is the class that owns parameter objects, and has methods for serialization of state
 *  It provides a base interface for remote editors as well as the main plug-in, because we may have state/preset management in remote editors,
 *  depending on the arrangement/separation we have chosen
 *  It needn't be a "plug-in" that implements this interface, it can also be used for other things
 *  An example use case: you would like to pop up a custom preferences window with a few simple checkboxes.
 *  You should be able to do that with a new graphics context and something implementing this interface in order to send/receive values
 *  to/from your new UI.
 *
 *  Note on method names: "FromUI" in a method name, means that that method is called by the UI class. Likewise "ToUI" means
 *  that the method is delivering something wait for it... to the UI.
 *  The words "FromDelegate" in a method name mean that method is called from the class that implements the IDelegate interface,
 *  which is usually your plug-in base class. A parameter value is a floating point number linked to an integer parameter index.
 *  A parameter object is an instance of the IParam class as defined in IPlugParameter.h, owned by IPlugBase.
 *  A parameter object is also referred to as a "param", in method names such as IPlugBase::GetParam(int paramIdx) and IControl::GetParam(). */
class IPluginDelegate : public IDelegate
{
public:
  IPluginDelegate(int nParams, int nPresets);
  virtual ~IPluginDelegate();
  
#pragma mark - Plug-in properties
  /** @return the name of the plug-in as a CString */
  const char* GetPluginName() const { return mPluginName.Get(); }
  
  /** Get the plug-in version number
   * @param decimal Sets the output format
   * @return Effect version in VVVVRRMM (if \p decimal is \c true) or Hexadecimal 0xVVVVRRMM (if \p decimal is \c false) format */
  int GetPluginVersion(bool decimal) const;
  
  /** Gets the plug-in version as a string
   * @param str WDL_String to write to
   * The output format is vX.M.m, where X - version, M - major, m - minor
   * @note If \c _DEBUG is defined, \c D is appended to the version string
   * @note If \c TRACER_BUILD is defined, \c T is appended to the version string*/
  void GetPluginVersionStr(WDL_String& str) const;
  
  /** Get the manufacturer name as a CString */
  const char* GetMfrName() const { return mMfrName.Get(); }
  
  /** Get the product name as a CString. A shipping product may contain multiple plug-ins, hence this. Not used in all APIs */
  const char* GetProductName() const { return mProductName.Get(); }
  
  /** @return The plug-in's unique four character ID as an integer */
  int GetUniqueID() const { return mUniqueID; }
  
  /** @return The plug-in manufacturer's unique four character ID as an integer */
  int GetMfrID() const { return mMfrID; }
  
  /** @return The host if it has been identified, see EHost enum for a list of possible hosts, implemented in the API class for VST2 and AUv2 */
  virtual EHost GetHost() { return mHost; }
  
  /** Get the host version number as an integer
   * @param decimal \c true indicates decimal format = VVVVRRMM, otherwise hexadecimal 0xVVVVRRMM.
   * @return The host version number as an integer. */
  int GetHostVersion(bool decimal); //
  
  /** Get the host version number as a string
   * @param str string into which to write the host version */
  void GetHostVersionStr(WDL_String& str);
  
  /** @return The The plug-in API, see EAPI enum for a list of possible APIs */
  EAPI GetAPI() const { return mAPI; }
  
  /** @return  Returns a CString describing the plug-in API, e.g. "VST2" */
  const char* GetAPIStr() const;
  
  /** @return  Returns a CString either "x86" or "x64" or "WASM" describing the binary architecture */
  const char* GetArchStr() const;
  
  /** @brief Used to get the build date of the plug-in and architecture/api details in one string
   * @note since the implementation is in IPlugBase.cpp, you may want to touch that file as part of your build script to force recompilation
   * @param str WDL_String will be set with the Plugin name, architecture, api, build date, build time*/
  void GetBuildInfoStr(WDL_String& str) const;
  
  /** @return \c true if the plug-in is meant to have a UI, as defined in config.h */
  bool HasUI() const { return mHasUI; }
  
  /** @return The default width of the plug-in UI in pixels, if defined in config.h */
  int Width() const { return mWidth; }
  
  /** @return The default height of the plug-in UI in pixels, if defined in config.h */
  int Height() const { return mHeight; }
  
  /** Override this method when not using IGraphics in order to return a platform view handle e.g. NSView, UIView, HWND */
  virtual void* OpenWindow(void* pHandle) { return nullptr; }
  
  /** Override this method when not using IGraphics if you need to free resources etc when the window closes */
  virtual void CloseWindow() {};
  
#pragma mark - Parameters
  
  /** Get a pointer to one of the delegate's IParam objects
   * @param paramIdx The index of the parameter object to be got
   * @return A pointer to the IParam object at paramIdx */
  IParam* GetParam(int paramIdx) override { return mParams.Get(paramIdx); }
  
  /** @return Returns the number of parameters that belong to the plug-in. */
  int NParams() const { return mParams.GetSize(); }
  
  /** @return The number of unique parameter groups identified */
  int NParamGroups() { return mParamGroups.GetSize(); }
  
  /** Called to add a parameter group name, when a unique group name is discovered
   * @param name CString for the unique group name
   * @return Number of parameter groups */
  int AddParamGroup(const char* name) { mParamGroups.Add(name); return NParamGroups(); }
  
  /** Get the parameter group name as a particular index
   * @param idx The index to return
   * @return CString for the unique group name */
  const char* GetParamGroupName(int idx) { return mParamGroups.Get(idx); }
  
#pragma mark - Parameter Change
  /** Override this method to do something when a parameter changes.
   * THIS METHOD **CAN BE** CALLED BY THE HIGH PRIORITY AUDIO THREAD
   * @param paramIdx The index of the parameter that changed
   * @param source One of the EParamSource options to indicate where the parameter change came from. */
  virtual void OnParamChange(int paramIdx, EParamSource source);
  
  /** Another version of the OnParamChange method without an EParamSource, for backwards compatibility / simplicity. */
  virtual void OnParamChange(int paramIdx) {}
  
#pragma mark - State Serialization
  /** @return \c true if the plug-in has been set up to do state chunks, via config.h */
  bool DoesStateChunks() const { return mStateChunks; }
  
  /** Serializes the current double precision floating point, non-normalised values (IParam::mValue) of all parameters, into a binary byte chunk.
   * @param chunk The output chunk to serialize to. Will append data if the chunk has already been started.
   * @return \c true if the serialization was successful */
  bool SerializeParams(IByteChunk& chunk);
  
  /** Unserializes double precision floating point, non-normalised values from a byte chunk into mParams.
   * @param chunk The incoming chunk where parameter values are stored to unserialize
   * @param startPos The start position in the chunk where parameter values are stored
   * @return The new chunk position (endPos) */
  int UnserializeParams(const IByteChunk& chunk, int startPos);
  
  /** Override this method to serialize custom state data, if your plugin does state chunks.
   * @param chunk The output bytechunk where data can be serialized
   * @return \c true if serialization was successful*/
  virtual bool SerializeState(IByteChunk& chunk) { TRACE; return SerializeParams(chunk); }
  
  /** Override this method to unserialize custom state data, if your plugin does state chunks.
   * Implementations should call UnserializeParams() after custom data is unserialized
   * @param chunk The incoming chunk containing the state data.
   * @param startPos The position in the chunk where the data starts
   * @return The new chunk position (endPos)*/
  virtual int UnserializeState(const IByteChunk& chunk, int startPos) { TRACE; return UnserializeParams(chunk, startPos); }
  
  /** This is called by API classes after restoring state and by IPresetDelegate::RestorePreset(). Typically used to update user interface, where parameter values have changed.
   * If you need to do something when state is restored you can override it */
  virtual void OnRestoreState() {};
  
#pragma mark - Preset Manipulation - NO-OPs
  
  /** Gets the number of factory presets. NOTE: some hosts don't like 0 presets, so even if you don't support factory presets, this method should return 1
   * @return The number of factory presets */
  virtual int NPresets() { return 1; }
  
  /** This method should update the current preset with current values
   * NOTE: This is only relevant for VST2 plug-ins, which is the only format to have the notion of banks?
   * @param name CString name of the modified preset */
  virtual void ModifyCurrentPreset(const char* name = 0) { };
  
  /** Restore a preset by index. This should also update mCurrentPresetIdx
   * @param idx The index of the preset to restore
   * @return \c true on success */
  virtual bool RestorePreset(int idx) { mCurrentPresetIdx = idx; return true; }
  
  /** Restore a preset by name
   * @param CString name of the preset to restore
   * @return \c true on success */
  virtual bool RestorePreset(const char* name) { return true; }
  
  /** Get the name a preset
   * @param idx The index of the preset whose name to get
   * @return CString preset name */
  virtual const char* GetPresetName(int idx) { return "-"; }
  
  /** Get the index of the current, active preset
   * @return The index of the current preset */
  int GetCurrentPresetIdx() const { return mCurrentPresetIdx; }
  
  /** Set the index of the current, active preset
   * @param idx The index of the current preset */
  void SetCurrentPresetIdx(int idx) { assert(idx > -1 && idx < NPresets()); mCurrentPresetIdx = idx; }
  
  /** Implemented by the API class, called by the UI (etc) when the plug-in initiates a program/preset change (not applicable to all APIs) */
  virtual void InformHostOfProgramChange() {};
  
#pragma mark - Parameter manipulation
  
  /** Initialise this delegate from another one
   * @param delegate The delegate to clone */
  void InitFromDelegate(IPluginDelegate& delegate);
  
  /** Initialise a range of parameters simultaneously. This mirrors the arguments available in IParam::InitDouble, for maximum flexibility
   * @param startIdx The index of the first parameter to initialise
   * @param endIdx The index of the last parameter to initialise
   * @param countStart An integer representing the start of the count in the format string. If the first parameter should have "0" in its name, set this to 0
   * @param nameFmtStr A limited format string where %i can be used to get the index + countStart, in the range of parameters specified
   * @param defaultVal A default real value for the parameter
   * @param minVal A minimum real value for the parameter
   * @param maxVal A Maximum real value for the parameter
   * @param step The parameter step
   * @param label A CString label for the parameter e.g. "decibels"
   * @param flags Any flags, see IParam::EFlags
   * @param group A CString group name for the parameter, e.g. "envelope"
   * @param shape A IParam::Shape class to determine how the parameter shape should be skewed
   * @param unit An IParam::EParamUnit which can be used in audiounit plug-ins to specify certain kinds of parameter
   * @param displayFunc An IParam::DisplayFunc lambda function to specify a custom display function */
  void InitParamRange(int startIdx, int endIdx, int countStart, const char* nameFmtStr, double defaultVal, double minVal, double maxVal, double step, const char* label = "", int flags = 0, const char* group = "", IParam::Shape* shape = nullptr, IParam::EParamUnit unit = IParam::kUnitCustom, IParam::DisplayFunc displayFunc = nullptr);
  
  /** Clone a range of parameters, optionally doing a string substitution on the parameter name.
   * @param cloneStartIdx The index of the first parameter to clone
   * @param cloneEndIdx The index of the last parameter to clone
   * @param startIdx The start of the cloned range
   * @param searchStr A CString to search for in the input parameter name
   * @param replaceStr A CString to replace searchStr in the output parameter name
   * @param newGroup If the new parameter should have a different group, update here */
  void CloneParamRange(int cloneStartIdx, int cloneEndIdx, int startIdx, const char* searchStr = "", const char* replaceStr = "", const char* newGroup = "");
  
  /** Modify a range of parameters with a lamda function
   * @param startIdx The index of the first parameter to modify
   * @param endIdx The index of the last parameter to modify
   * @param func A lambda function to modify the parameter. Ideas: you could randomise the parameter value or reset to default, modify certain params based on their group */
  void ModifyParamValues(int startIdx, int endIdx, std::function<void(IParam& param)> func);
  
  /** Modify a parameter group simulataneously
   * @param paramGroup The name of the group to modify
   * @param param func A lambda function to modify the parameter. Ideas: you could randomise the parameter value or reset to default*/
  void ModifyParamValues(const char* paramGroup, std::function<void(IParam& param)> func);
  
  /** Copy a range of parameter values
   * @param startIdx The index of the first parameter value to copy
   * @param destIdx The index of the first destination parameter
   * @param nParams The number of parameters to copy */
  void CopyParamValues(int startIdx, int destIdx, int nParams);
  
  /** Copy a range of parameter values for a parameter group
   * @param inGroup The name of the group to copy from
   * @param outGroup The name of the group to copy to */
  void CopyParamValues(const char* inGroup, const char* outGroup);
  
  /** Randomise parameter values within a range. NOTE for more flexibility in terms of RNG etc, use ModifyParamValues()
   * @param startIdx The index of the first parameter to modify
   * @param endIdx The index of the last parameter to modify */
  void RandomiseParamValues(int startIdx, int endIdx);
  
  /** Randomise parameter values for a parameter group
   * @param paramGroup The name of the group to modify */
  void RandomiseParamValues(const char* paramGroup);
  
  /** Default parameter values within a range.
   * @param startIdx The index of the first parameter to modify
   * @param endIdx The index of the last parameter to modify */
  void DefaultParamValues(int startIdx, int endIdx);
  
  /** Default parameter values for a parameter group
   * @param paramGroup The name of the group to modify */
  void DefaultParamValues(const char* paramGroup);
  
#pragma mark - DELEGATION methods for sending values FROM the user interface
  
  /** Called by the user interface during a parameter change gesture, in order to notify the host of the new value (via a call in the API class)
   * If you override this method you should call the base class implementation to make sure OnParamChangeUI gets triggered
   * @param paramIdx The index of the parameter that is changing value
   * @param value The new normalised value of the parameter */
  virtual void SetParameterValueFromUI(int paramIdx, double normalizedValue) override { GetParam(paramIdx)->SetNormalized(normalizedValue); OnParamChangeUI(paramIdx); }

protected:
  int mCurrentPresetIdx = 0;
  /** \c true if the plug-in does opaque state chunks. If false the host will provide a default interface */
  bool mStateChunks = false;
  /** The name of this plug-in */
  WDL_String mPluginName;
  /** Product name: if the plug-in is part of collection of plug-ins it might be one product */
  WDL_String mProductName;
  /** Plug-in Manufacturer name */
  WDL_String mMfrName;
  /* Plug-in unique four char ID as an int */
  int mUniqueID;
  /* Manufacturer unique four char ID as an int */
  int mMfrID;
  /** Plug-in version number stored as 0xVVVVRRMM: V = version, R = revision, M = minor revision */
  int mVersion;
  /** Host version number stored as 0xVVVVRRMM: V = version, R = revision, M = minor revision */
  int mHostVersion = 0;
  /** Host that has been identified, see EHost enum */
  EHost mHost = kHostUninit;
  /** API of this instance */
  EAPI mAPI;
  
  /** \c true if the plug-in has a user interface. If false the host will provide a default interface */
  bool mHasUI = false;
  /** The default width of the plug-in UI if it has an interface. */
  int mWidth = 0;
  /** The default height of the plug-in UI if it has an interface. */
  int mHeight = 0;
  
  /** A list of unique cstrings found specified as "parameter groups" when defining IParams. These are used in various APIs to group parameters together in automation dialogues. */
  WDL_PtrList<const char> mParamGroups;
  /** A list of IParam objects. This list is populated in the delicate constructor depending on the number of parameters passed as an argument to IPLUG_CTOR in the plugin class implementation constructor */
  WDL_PtrList<IParam> mParams;
};