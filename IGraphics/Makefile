#CXXFLAGS=-I. -I../IPlug -I../Dependencies/IGraphics/STB -I../IPlug/Extras -DAPP_API -I../WDL -I../IGraphics -IPlatforms -IControls -I../Dependencies/IGraphics/NanoSVG/src -DIGRAPHICS_NANOVG -IDrawing -I../Dependencies/IGraphics/NanoVG/src -I../WDL/swell -DNOMINMAX -DIGRAPHICS_GL2 -I../Dependencies/IGraphics/xcbt -I../Dependencies/IGraphics/glad_GLX/include -ggdb -Wall -fPIC -fsanitize=address
# FIXME: Each one needs their own.
CXXFLAGS = -I../IPlug -I. -DVST3_API -DIPLUG_DSP -I../IPlug/VST3 -I../WDL -I../IGraphics -I../IPlug/Extras -IPlatforms -IControls -I../Dependencies/IGraphics/STB -I../Dependencies/IGraphics/NanoSVG/src -DIGRAPHICS_NANOVG -IDrawing -I../Dependencies/IGraphics/NanoVG/src -I../WDL/swell -DNOMINMAX -DIGRAPHICS_GL2 -I../Dependencies/IGraphics/xcbt -I../Dependencies/IGraphics/glad_GLX/include -I/usr/include/rtaudio -I/usr/include/rtmidi -fPIC -DIPLUG_EDITOR=1 -ggdb -O2 -Wall -I../IPlug/Extras/Synth/ -I../Dependencies/IPlug/VST3_SDK/ -DDEVELOPMENT -I$(VST3_SDK)/base/source -fsanitize=address

SOURCES = ./IGraphics.cpp ./Platforms/IGraphicsLinux.cpp ./IControl.cpp ./IGraphicsEditorDelegate.cpp ./Controls/ITextEntryControl.cpp ./Controls/IPopupMenuControl.cpp ./Controls/IControls.cpp

GL2=1

ifdef FLEXBOX
SOURCES += ./Extras/IGraphicsFlexBox.cpp
CXXFLAGS += -I../Dependencies/IGraphics/yoga/yoga/
endif

# These are included in the IGraphicsX
#ifdef NANOVG
#SOURCES += ./Drawing/IGraphicsNanoVG.cpp
#endif

#ifdef SKIA
#SOURCES += ./Drawing/IGraphicsSkia.cpp
#endif

#ifdef CANVAS
#SOURCES += ./Drawing/IGraphicsCanvas.cpp
#endif

ifdef WEB
SOURCES += ./Platforms/IGraphicsWeb.cpp
endif

ifdef GL2
CXXFLAGS += -I../Dependencies/IGraphics/glad_GL2/src -I../Dependencies/IGraphics/glad_GL2/include
endif
ifdef GL3
CXXFLAGS += -I../Dependencies/IGraphics/glad_GL3/src -I../Dependencies/IGraphics/glad_GL3/include
endif

OBJS = $(subst .cpp,.o,$(SOURCES))

all: $(OBJS)

./Platforms/IGraphicsLinux.o: ./Platforms/IGraphicsLinux.cpp ./Platforms/IGraphicsLinux.h

clean:
	rm -f $(OBJS)
