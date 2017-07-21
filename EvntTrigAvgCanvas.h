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
class HistoGraph;
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

    void startRecording() { } // unused
    void stopRecording() { } // unused
    
       EvntTrigAvg* processor;

   ;
   // ScopedPointer<UtilityButton>

private:
    std::vector<std::vector<uint64>> histoData;
    std::vector<std::vector<float>> minMaxMean;
    void removeUnitOrBox();
    ScopedPointer<Viewport> viewport;
    ScopedPointer<EvntTrigAvgDisplay> display;
    ScopedPointer<UtilityButton> clearHisto;
    //  SpikeObject spike;
    int scrollBarThickness;
    int border = 20;
    int triggerChannel = NULL;
    unsigned long spikeBufferSize = 0;
    unsigned long ttlBufferSize = 0;
    
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
private:
    EvntTrigAvg* processor;
    EvntTrigAvgCanvas* canvas;
    Viewport* viewport;
    Array<HistoGraph*> histograms;
    juce::Colour channelColours[16];
    std::vector<std::vector<uint64>> histoData;
    std::vector< std::vector<float>> minMaxMean;
    int border = 20;
};

class HistoGraph : public Component
{
    
public:
    HistoGraph(EvntTrigAvgCanvas* e, juce::Colour c,std::vector<float> s, std::vector<uint64> h);
    ~HistoGraph();
    
    void paint(Graphics& g);
    void resized();
    
    void select();
    void deselect();
    void clear();
    
    EvntTrigAvgCanvas* canvas;
    Colour color;
    std::vector<uint64> histoData;
    std::vector<float> stats;
    
    
    //void buttonClicked(Button* button);
    
private:
    
   
    String name;
    
    Font font;
    
   
    
};

#endif  // EvntTrigAvgCANVAS_H_
