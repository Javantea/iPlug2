CXXFLAGS=-I. -I../IPlug -I../Dependencies/IGraphics/STB -I../IPlug/Extras -DAPP_API -I../WDL -I../IGraphics -I../IGraphics/Platforms -I../IGraphics/Controls -I../Dependencies/IGraphics/NanoSVG/src -DIGRAPHICS_NANOVG -I../IGraphics/Drawing -I../Dependencies/IGraphics/NanoVG/src -I../WDL/swell -DNOMINMAX -DIGRAPHICS_GL2 -I../Dependencies/IGraphics/xcbt -I../Dependencies/IGraphics/glad_GLX/include

SOURCES = ./IGraphics.cpp ./Platforms/IGraphicsLinux.cpp ./IControl.cpp ./IGraphicsEditorDelegate.cpp ./Controls/ITextEntryControl.cpp ./Controls/IPopupMenuControl.cpp ./Controls/IControls.cpp

ifdef FLEXBOX
SOURCES += ./Extras/IGraphicsFlexBox.cpp
CXXFLAGS += -I../Dependencies/IGraphics/yoga/yoga/
endif

ifdef NANOVG
SOURCES += ./Drawing/IGraphicsNanoVG.cpp
endif

ifdef SKIA
SOURCES += ./Drawing/IGraphicsSkia.cpp
endif

ifdef CANVAS
SOURCES += ./Drawing/IGraphicsCanvas.cpp
endif

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
