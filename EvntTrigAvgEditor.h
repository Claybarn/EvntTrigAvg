/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef EvntTrigAvgCANVAS_H_
#define EvntTrigAvgCANVAS_H_

#include <VisualizerWindowHeaders.h>
#include "EvntTrigAvg.h"

#include <vector>

/**

 
*/

class EvntTrigAvgDisplay;
class Timescale;
class GraphUnit;
class LabelDisplay;
class HistoGraph;
class StatDisplay;


class EvntTrigAvgCanvas : public Visualizer, public Button::Listener

{
public:
    EvntTrigAvgCanvas(EvntTrigAvg* n);
    ~EvntTrigAvgCanvas();

    void paint(Graphics& g);

    void refresh();

    
    void beginAnimation();
    void endAnimation();

    void refreshState();

    void setParameter(int, float) {}
    void setParameter(int, int, int, float) {}

    void update();

    void resized();

    bool keyPressed(const KeyPress& key);

    void buttonClicked(Button* button);

    void startRecording() { }; // unused
    void stopRecording() { }; // unused
    void setBin(int bin_);
    void setBinSize(int binSize_);
    void setData(int data_);
    EvntTrigAvg* processor;

   
   // ScopedPointer<UtilityButton>

private:
    
    std::vector<std::vector<uint64*>> histoData;
    std::vector<std::vector<float>> minMaxMean;
    void removeUnitOrBox();
    ScopedPointer<Viewport> viewport;
    ScopedPointer<EvntTrigAvgDisplay> display;
    ScopedPointer<UtilityButton> clearHisto;
    //  SpikeObject spike;
    int scrollBarThickness;
    int border = 20;
    int triggerChannel = -1;
    unsigned long spikeBufferSize = 0;
    unsigned long ttlBufferSize = 0;
    int bin = 0;
    int binSize = 0;
    int data = 0;
    Timescale* scale;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EvntTrigAvgCanvas);

};
///////////////////////////////////////////////////////////////////////////////////

class EvntTrigAvgDisplay : public Component
{
    
public:
    EvntTrigAvgDisplay(EvntTrigAvgCanvas* c, Viewport* v, EvntTrigAvg* p);
    ~EvntTrigAvgDisplay();
    void visibleAreaChanged (const Rectangle<int>& newVisibleArea);
    
    /** Callback method that is called when the viewed component is added, removed or swapped. */
    void viewedComponentChanged (Component* newComponent);
    void resized();
    void paint(Graphics &g);
    void refresh();
    int getNumGraphs();
private:
    CriticalSection mut;
    EvntTrigAvg* processor;
    EvntTrigAvgCanvas* canvas;
    Viewport* viewport;
    std::vector<GraphUnit*> graphs;
    juce::Colour channelColours[16];
    std::vector<std::vector<uint64*>> histoData;
    std::vector<std::vector<float*>> minMaxMean;
    int border = 20;
    
};

//---------------------------

class Timescale : public Component
{
public:
    Timescale(int windowSize_, uint64 sampleRate_, int data = 0, int bin = 0 ,int binSize = 0 );
    ~Timescale();
    void paint(Graphics& g);
    void resized();
    void update(int windowSize, uint64 sR);
    void setBin(int bin_);
    void setData(int data_);
    void setBinSize(int binSize_);
private:
    int windowSize;
    uint64 sampleRate;
    int bin = 0;
    int binSize = 0;
    int data = 0;
    int lowerBin = 0;
    int upperBin = 0;
    
};


//---------------------------


class GraphUnit : public Component
{
public:
    GraphUnit(EvntTrigAvg* processor_,EvntTrigAvgCanvas* canvas_,juce::Colour color_, String name_, float* stats_, uint64*data_);
    ~GraphUnit();
    void paint(Graphics& g);
    void resized();
private:
    LabelDisplay* LD;
    HistoGraph* HG;
    StatDisplay* SD;
    Colour color;
};

//---------------------------

class LabelDisplay : public Component
{
public:
    LabelDisplay(Colour color,String n);
    ~LabelDisplay();
    void paint(Graphics& g);
    void resized();
private:
    Colour color;
    String name;
};

//---------------------------

class HistoGraph : public Component
{
    
public:
    HistoGraph(EvntTrigAvgCanvas* canvas_,juce::Colour color_, int bins_, int max_, uint64* histoData_);
    ~HistoGraph();
    
    void paint(Graphics& g);
    void resized();
    
    void select();
    void deselect();
    void clear();
    
    void mouseMove(const MouseEvent &event);
    
    //void buttonClicked(Button* button);
    
private:
    int bins = 0;
    Colour color;
    int max;
    uint64* histoData;
    int valueY=0;
    EvntTrigAvgCanvas* canvas;
    
};

//---------------------------

class StatDisplay : public Component
{
public:
    StatDisplay(juce::Colour c, float* s);
    ~StatDisplay();
    void paint(Graphics& g);
    void resized();
private:
    Colour color;
    float* stats;
    
};


#endif  // EvntTrigAvgCANVAS_H_
