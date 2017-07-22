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

/*
 
 //hand-built palette
 channelColours.add(Colour(224,185,36));
 channelColours.add(Colour(214,210,182));
 channelColours.add(Colour(243,119,33));
 channelColours.add(Colour(186,157,168));
 channelColours.add(Colour(237,37,36));
 channelColours.add(Colour(179,122,79));
 channelColours.add(Colour(217,46,171));
 channelColours.add(Colour(217, 139,196));
 channelColours.add(Colour(101,31,255));
 channelColours.add(Colour(141,111,181));
 channelColours.add(Colour(48,117,255));
 channelColours.add(Colour(184,198,224));
 channelColours.add(Colour(116,227,156));
 channelColours.add(Colour(150,158,155));
 channelColours.add(Colour(82,173,0));
 channelColours.add(Colour(125,99,32));
*/

/*
 
 for (int i = 0; i < numChans; i++)
 {
 //std::cout << "Adding new display for channel " << i << std::endl;
 
 LfpChannelDisplay* lfpChan = new LfpChannelDisplay(canvas, this, options, i);
 
 //lfpChan->setColour(channelColours[i % channelColours.size()]);
 lfpChan->setRange(range[options->getChannelType(i)]);
 lfpChan->setChannelHeight(canvas->getChannelHeight());
 
 addAndMakeVisible(lfpChan);
 
 channels.add(lfpChan);
 
 LfpChannelDisplayInfo* lfpInfo = new LfpChannelDisplayInfo(canvas, this, options, i);
 
 //lfpInfo->setColour(channelColours[i % channelColours.size()]);
 lfpInfo->setRange(range[options->getChannelType(i)]);
 lfpInfo->setChannelHeight(canvas->getChannelHeight());
 
 addAndMakeVisible(lfpInfo);
 
 channelInfo.add(lfpInfo);
 
 savedChannelState.add(true);
 
 totalHeight += lfpChan->getChannelHeight();
 
 }
 
*/




#include "EvntTrigAvgCanvas.h"


EvntTrigAvgCanvas::EvntTrigAvgCanvas(EvntTrigAvg* n) :
    processor(n)
{
    
    viewport = new Viewport();
    scrollBarThickness = viewport->getScrollBarThickness();
    
    clearHisto = new UtilityButton("CLEAR", Font("Default", 12, Font::plain));
    clearHisto->addListener(this);
    clearHisto->setRadius(3.0f);
    clearHisto->setBounds(80,5,65,15);
    clearHisto->setClickingTogglesState(false);
    addAndMakeVisible(clearHisto);
/*
    addUnitButton = new UtilityButton("New box unit", Font("Small Text", 13, Font::plain));
    addUnitButton->setRadius(3.0f);
    addUnitButton->addListener(this);
    addAndMakeVisible(addUnitButton);
*/
    

    setWantsKeyboardFocus(true);
    
    update();

    processor = n;
    display = new EvntTrigAvgDisplay(this, viewport, n);
    viewport->setScrollBarsShown(true, false);
    viewport->setViewedComponent(display,false);
    addAndMakeVisible(viewport);
    
    }

EvntTrigAvgCanvas::~EvntTrigAvgCanvas()
{

}

void EvntTrigAvgCanvas::beginAnimation()
{
    std::cout << "EvntTrigAvgCanvas beginning animation." << std::endl;

    startCallbacks();
}

void EvntTrigAvgCanvas::endAnimation()
{
    std::cout << "EvntTrigAvgCanvas ending animation." << std::endl;

    stopCallbacks();
}

void EvntTrigAvgCanvas::update()
{
    repaint();
}


void EvntTrigAvgCanvas::refreshState()
{
    resized();
}

void EvntTrigAvgCanvas::resized()
{
    
    int xOffset= 40;
    int yOffset = 50;
    int drawWidth = getWidth()-20-getWidth()/4;
    viewport->setBounds(0,yOffset,getWidth(),getHeight());
    display->setBounds(0,100,getWidth(), getHeight()-2*yOffset);
    repaint();
}

void EvntTrigAvgCanvas::paint(Graphics& g)
{
    g.fillAll(Colours::black);
    int width=getWidth();
    int height=getHeight();
    int drawWidth = width-20-width/4;
    int xOffset= 20;
    g.setColour(Colours::snow);
    g.drawText("Electrode",5, 5, width/8, 20, juce::Justification::left);
    g.drawText("Trials: " + String(processor->getTTLTimestampBufferSize()),(xOffset+drawWidth)/2-50,5,100,20,Justification::centred);
    g.drawText("Min.", 9*width/12, 5, 50, 20, Justification::right);
    g.drawText("Max", 10*width/12, 5, 50, 20, Justification::right);
    g.drawText("Mean", 11*width/12, 5, 50, 20, Justification::right);
}

void EvntTrigAvgCanvas::refresh()
{
    // called every 10 Hz
    display->refresh(); // dont know if this ever gets called

    repaint();
}

bool EvntTrigAvgCanvas::keyPressed(const KeyPress& key)
{

/*
    KeyPress c = KeyPress::createFromDescription("c");
    KeyPress e = KeyPress::createFromDescription("escape");
    KeyPress d = KeyPress::createFromDescription("delete");

    if (key.isKeyCode(c.getKeyCode())) // C
    {
        spikeDisplay->clear();

        std::cout << "Clearing display" << std::endl;
        return true;
    }
    else  if (key.isKeyCode(e.getKeyCode()))   // ESC
    {
        spikeDisplay->setPolygonMode(false);
        return true;
    }
    else  if (key.isKeyCode(d.getKeyCode()))   // Delete
    {
        removeUnitOrBox();
        return true;
    }
*/
    return false;
 

}

void EvntTrigAvgCanvas::buttonClicked(Button* button)
{
    if (button == clearHisto){
        histoData.clear();
        minMaxMean.clear();
        processor->setParameter(4,0);
    }
     repaint();
}


//-----------------------------------------------------------------------------------------------------------------//

EvntTrigAvgDisplay::EvntTrigAvgDisplay(EvntTrigAvgCanvas* c, Viewport* v, EvntTrigAvg* p){
    processor=p;
    canvas=c;
    viewport=v;
    
    channelColours[0]=Colour(224,185,36);
    channelColours[1]=Colour(214,210,182);
    channelColours[2]=Colour(243,119,33);
    channelColours[3]=Colour(186,157,168);
    channelColours[4]=Colour(237,37,36);
    channelColours[5]=Colour(179,122,79);
    channelColours[6]=Colour(217,46,171);
    channelColours[7]=Colour(217, 139,196);
    channelColours[8]=Colour(101,31,255);
    channelColours[9]= Colour(141,111,181);
    channelColours[10]=Colour(48,117,255);
    channelColours[11]=Colour(184,198,224);
    channelColours[12]=Colour(116,227,156);
    channelColours[13]=Colour(150,158,155);
    channelColours[14]=Colour(82,173,0);
    channelColours[15]=Colour(125,99,32);

}

EvntTrigAvgDisplay::~EvntTrigAvgDisplay(){
    deleteAllChildren();
}

void EvntTrigAvgDisplay::visibleAreaChanged (const Rectangle<int>& newVisibleArea){
    
}

void EvntTrigAvgDisplay::viewedComponentChanged (Component* newComponent){
    
}

void EvntTrigAvgDisplay::resized()
{
    int width = getWidth();
    for(int i = 0 ; i < histograms.size() ; i++){
        histograms[i]->setBounds(20, 40*(i+1), width-20-width/4, 40);
        histograms[i]->resized();
    }
}

void EvntTrigAvgDisplay::paint(Graphics &g){
    //if(processor->shouldReadHistoData()){
    g.fillAll(Colours::darkgrey);
    histoData = processor->getHistoData();
    minMaxMean = processor->getMinMaxMean();
    int width=getWidth();
    int height=getHeight();
    g.setColour(Colours::snow);
    int yFact = -2;
    int xOffset= 20;
    int drawWidth = width-20-width/4;
    int drawHeight = 40;
    g.drawLine((xOffset+drawWidth)/2, border, (xOffset+drawWidth)/2 , height-border, 1);
    std::vector<String> labels = processor->getElectrodeLabels();
    for (int electrode = 0 ; electrode < histoData.size() ; electrode++){
        for(int sortedID = 0 ; sortedID < histoData[electrode].size() ; sortedID++)
        {
            int lineX= drawWidth/(histoData[electrode][sortedID].size()-1);
            for (int i = 1 ; i < histoData[electrode][sortedID].size() ; i++){
                g.setColour(channelColours[(sortedID+16)%16]);
            }
            g.drawText(labels[electrode], 5, drawHeight*(electrode+1)-10, 15, 20, juce::Justification::left);
            g.drawText(String(minMaxMean[electrode][sortedID][0]), 9*width/12, drawHeight*(sortedID+1)-10, 50, 20, juce::Justification::right);
            g.drawText(String(minMaxMean[electrode][sortedID][1]), 10*width/12, drawHeight*(sortedID+1)-10, 50, 20, juce::Justification::right);
            g.drawText(String(minMaxMean[electrode][sortedID][2]), 11*width/12, drawHeight*(sortedID+1)-10, 50, 20, juce::Justification::right);
            deleteAllChildren();
            histograms.clear();
            for (int channelIt = 0 ; channelIt < histoData.size() ; channelIt++){
                for(int sortedId = 0 ; sortedId < histoData[channelIt].size() ; sortedId++){
                    HistoGraph* histogram = new HistoGraph(canvas,channelColours[(channelIt+sizeof(channelColours))%(sizeof(channelColours))],minMaxMean[channelIt][sortedId],histoData[channelIt][sortedId]);
                    histograms.add(histogram);
                    histogram->setBounds(20, 40*(channelIt+sortedId+1), width-20, 40);
                    addAndMakeVisible(histogram,false);
                }
            }
        }
    }
    repaint();
}

void EvntTrigAvgDisplay::refresh(){
    for (int i = 0 ; i < histograms.size() ; i++){
        histograms[i]->repaint();
    }
}

//--------------------------------------------------------------------

GraphUnit::GraphUnit(juce::Colour c, String n, std::vector<float> s, std::vector<uint64> f){
    color = c;
    LD = new LabelDisplay(c,n);
    LD->setBounds(0,0,20,40);
    addAndMakeVisible(LD,false);
    HG = new HistoGraph(c, f);
    HG->setBounds(0,0,20,40);
    addAndMakeVisible(HG,false);
    SD = new StatDisplay(c,s);
    SD->setBounds(getWidth()-60,0,60,40);
    addAndMakeVisible(SD,false);
}
GraphUnit::~GraphUnit(){
    deleteAllChildren();
}
void GraphUnit::paint(Graphics& g){
    g.setColour(Colours::snow);
    g.setOpacity(0.3);
    g.drawVerticalLine(getWidth()/2, getHeight(), 0);
    g.setOpacity(0);
}
void GraphUnit::resized(){
    LD->setBounds(0,0,20,40);
    HG->setBounds(0,0,20,40);
    SD->setBounds(getWidth()-60,0,60,40);


}
//----------------

LabelDisplay::LabelDisplay(juce::Colour c, String n){
    color = c;
    name = n;
}
LabelDisplay::~LabelDisplay(){
    
}
void LabelDisplay::paint(Graphics& g){
    
}
void LabelDisplay::resized(){
    
}

//----------------

HistoGraph::HistoGraph(juce::Colour c, std::vector<uint64> f){
    color = c;
    histoData = f;
}

HistoGraph::~HistoGraph(){
    
}

void HistoGraph::paint(Graphics& g){
    g.setColour(color);
    for (int i = 1 ; i < histoData.size() ; i++){
        g.drawLine((i-1)*getWidth()/histoData.size(),getHeight()-histoData.operator[](i-1),(i)*getWidth()/histoData.size(),getHeight()-histoData.operator[](i));
    }
}

void HistoGraph::resized(){
    repaint();
}

void HistoGraph::select(){
    
}

void HistoGraph::deselect(){
    
}

void HistoGraph::clear(){
    
}

//----------------

StatDisplay::StatDisplay(juce::Colour c, std::vector<float> s){
    color = c;
    stats = s;
}

StatDisplay::~StatDisplay(){
    
}

void StatDisplay::paint(Graphics& g){
    g.setColour(color);
    g.drawText(String(stats[0]),0, 0, 20, 40, juce::Justification::right);
    g.drawText(String(stats[1]),20, 0, 20, 40, juce::Justification::right);
    g.drawText(String(stats[2]),40, 0, 20, 40, juce::Justification::right);
    }

void StatDisplay::resized(){
    
}

