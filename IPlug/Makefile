CXXFLAGS=-I. -DAPP_API -I./APP -I../WDL -I../IGraphics -I./Extras -I../IGraphics/Platforms -I../IGraphics/Controls -I../Dependencies/IGraphics/NanoSVG/src -DIGRAPHICS_NANOVG -I../IGraphics/Drawing -I../Dependencies/IGraphics/NanoVG/src -I../WDL/swell -DNOMINMAX -DIGRAPHICS_GL2 -I../Dependencies/IGraphics/xcbt

SOURCES = ./IPlugPluginBase.cpp ./IPlugPaths.cpp ./Extras/OSC/IPlugOSC.cpp ./Extras/OSC/IPlugOSC_msg.cpp ./Extras/Synth/MidiSynth.cpp ./Extras/Synth/VoiceAllocator.cpp ./Extras/HIIR/PolyphaseIIR2Designer.cpp ./IPlugTimer.cpp ./IPlugParameter.cpp ./IPlugAPIBase.cpp ./IPlugProcessor.cpp

ifdef WebView
SOURCES += ./Extras/WebView/IPlugWebView.cpp ./Extras/WebView/IPlugWebViewEditorDelegate.cpp ./Extras/WebView/IPlugWebView_win.cpp
endif

ifdef VST3
SOURCES += ./VST3/IPlugVST3_ProcessorBase.cpp ./VST3/IPlugVST3_Controller.cpp ./VST3/IPlugVST3_Processor.cpp ./VST3/IPlugVST3.cpp
endif

ifdef WEB
SOURCES += ./WEB/IPlugWeb.cpp ./WEB/IPlugWAM.cpp
endif

ifdef APP
SOURCES += ./APP/IPlugAPP_host.cpp ./APP/IPlugAPP_main.cpp ./APP/IPlugAPP.cpp ./APP/IPlugAPP_dialog.cpp
endif

ifdef VST2
SOURCES += ./VST2/IPlugVST2.cpp
endif

ifdef CLAP
SOURCES += ./CLAP/IPlugCLAP.cpp
endif

ifdef REAPER
SOURCES += ./ReaperExt/ReaperExtBase.cpp
endif

ifdef AU
SOURCES += ./AUv2/IPlugAU.cpp
endif

OBJS = $(subst .cpp,.o,$(SOURCES))

all: $(OBJS)
