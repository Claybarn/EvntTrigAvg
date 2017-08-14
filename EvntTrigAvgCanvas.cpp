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
 */
 #include "EvntTrigAvgCanvas.h"


EvntTrigAvgCanvas::EvntTrigAvgCanvas(EvntTrigAvg* n) :
    processor(n)
{
    
    
    clearHisto = new UtilityButton("CLEAR", Font("Default", 12, Font::plain));
    clearHisto->addListener(this);
    clearHisto->setRadius(3.0f);
    clearHisto->setBounds(80,5,65,15);
    clearHisto->setClickingTogglesState(false);
    addAndMakeVisible(clearHisto);
    setWantsKeyboardFocus(true);
    
    viewport = new Viewport();
    viewport->setScrollBarsShown(true,true);
    scrollBarThickness = viewport->getScrollBarThickness();
    
    int yOffset = 50;
    processor = n;
    display = new EvntTrigAvgDisplay(this, viewport, n);
    display->setBounds(0,100,getWidth()-scrollBarThickness, getHeight()-2*yOffset);
    scale = new Timescale(processor->getWindowSize(),processor->getSampleRate());
    viewport->setViewedComponent(display,false);
    addAndMakeVisible(viewport);
    viewport->setBounds(0,100,getWidth(), getHeight()-2*yOffset);
    scale->setBounds(0, getHeight()-40, getWidth()-scrollBarThickness, 40);
    addAndMakeVisible(scale,false);
    
    update();
}

EvntTrigAvgCanvas::~EvntTrigAvgCanvas()
{
    removeAllChildren();
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
    viewport->setBounds(0,yOffset,getWidth(),getHeight()-2*yOffset);
    if (display->getNumGraphs()>0)
        display->setBounds(0,yOffset,getWidth()-scrollBarThickness,display->getNumGraphs()*40);
    else
        display->setBounds(0,yOffset,getWidth(),getHeight()-2*yOffset);
    scale->setBounds(20+30, getHeight()-40, getWidth()-20-getWidth()/4, 40);
    repaint();
}

void EvntTrigAvgCanvas::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
    
    int width=getWidth();
    int height=getHeight();
    int drawWidth = width-20-width/4;
    int xOffset= 20;
    int yOffset = 50;
    g.setColour(Colours::lightgrey);
    g.fillRoundedRectangle(getWidth()-scrollBarThickness, yOffset, scrollBarThickness, height-2*yOffset, 4.0);

    g.setColour(Colours::snow);
    
        g.drawText("Electrode",5, 5, width/8, 20, juce::Justification::left);
    g.drawText("Trials: " + String(processor->getLastTTLCalculated()),(xOffset+drawWidth)/2-50,5,100,20,Justification::centred);
    g.drawText("Min.", width-180-scrollBarThickness, 5, 60, 20, Justification::right);
    g.drawText("Max", width-120-scrollBarThickness, 5, 60, 20, Justification::right);
    g.drawText("Mean", width-60-scrollBarThickness, 5, 60, 20, Justification::right);
    removeChildComponent(scale);
    scale = new Timescale(processor->getWindowSize(),processor->getSampleRate());
    scale->setBounds(0, getHeight()-40, width-20, 40);
    addAndMakeVisible(scale,true);
    repaint();
}

void EvntTrigAvgCanvas::refresh()
{
    // called every 10 Hz
    display->refresh(); // dont know if this ever gets called

    repaint();
}

bool EvntTrigAvgCanvas::keyPressed(const KeyPress& key)
{
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
    addMouseListener(this, true);
    channelColours[0]=Colour(224,185,36);
    channelColours[1]=Colour(214,210,182);
    channelColours[2]=Colour(243,119,33);
    channelColours[3]=Colour(186,157,168);
    channelColours[4]=Colour(237,37,36);
    channelColours[5]=Colour(179,122,79);
    channelColours[6]=Colour(217,46,171);
    channelColours[7]=Colour(217, 139,196);
    channelColours[8]=Colour(101,31,255);
    channelColours[9]=Colour(141,111,181);
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
    for(int i = 0 ; i < graphs.size() ; i++){
        graphs[i]->setBounds(20, 40*(i+1), width-20-width/4, 40);
        graphs[i]->resized();
    }
}

void EvntTrigAvgDisplay::paint(Graphics &g){
    //g.fillAll(Colours::darkgrey);
    //g.fillAll(Colours::firebrick);
    histoData = processor->getHistoData();
    minMaxMean = processor->getMinMaxMean();
    int width=getWidth();
    int height=getHeight();
    g.setColour(Colours::snow);
    int yFact = -2;
    int xOffset= 20;
    int drawWidth = width-20-width/4;
    int drawHeight = 40;
    std::vector<String> labels = processor->getElectrodeLabels();
    deleteAllChildren();
    graphs.clear();
    int graphCount = 0;

    for (int channelIt = 0 ; channelIt < histoData.size() ; channelIt++){
        /*
        GraphUnit* graph = new GraphUnit(menus,channelColours[(channelIt+sizeof(channelColours))%(sizeof(channelColours))],labels[channelIt],minMaxMean[channelIt][0],histoData[channelIt][0]);
        graphs.add(graph);
        graph->setBounds(0, 40*(graphCount), width-20, 40);
        addAndMakeVisible(graph,false);
        graphCount += 1;
         */
        
        for(int sortedId = 0 ; sortedId < histoData[channelIt].size() ; sortedId++){
                //GraphUnit* graph = new GraphUnit(menus,channelColours[(channelIt+sizeof(channelColours))%(sizeof(channelColours))],"ID " + String(sortedId),minMaxMean[channelIt][sortedId],histoData[channelIt][sortedId]);
            GraphUnit* graph = new GraphUnit(menus,channelColours[(channelIt+sizeof(channelColours))%(sizeof(channelColours))],labels[channelIt],minMaxMean[channelIt][sortedId],histoData[channelIt][sortedId]);
            
                graphs.add(graph);
                graph->setBounds(0, 40*(graphCount), width-20, 40);
                addAndMakeVisible(graph,false);
                graphCount += 1;
        }
    }

    repaint();
}

void EvntTrigAvgDisplay::refresh(){
    for (int i = 0 ; i < graphs.size() ; i++){
        graphs[i]->repaint();
    }
}

int EvntTrigAvgDisplay::getNumGraphs(){
    return graphs.size();
}
//--------------------------------------------------------------------
Timescale::Timescale(int wS, uint64 sR){
    windowSize = wS;
    sampleRate = sR;
}
Timescale::~Timescale(){
    
}

void Timescale::paint(Graphics& g){
    g.setColour(Colours::snow);
    // HG->setBounds(30,0,getWidth()-210,40);
    int histogramLen = getWidth()-210-22;
    int vertLineLen = 20;
    int textStart = vertLineLen+5;
    g.drawHorizontalLine(0, 30, histogramLen+30);
    
    g.drawVerticalLine(30, 0, vertLineLen);
    g.drawText(String(-1000.0*float(windowSize/2)/float(sampleRate)) + " ms", 0, textStart, 60, 10, Justification::centred);
    
    g.drawVerticalLine(histogramLen/4+30, 0, vertLineLen);
    g.drawText(String(-1000.0*float(windowSize/2)/2.0/float(sampleRate)) + " ms", histogramLen/4, textStart, 60, 10, Justification::centred);
    
    g.drawVerticalLine(histogramLen/2+30, 0, vertLineLen);
    g.drawText(" 0 ms",histogramLen/2, textStart, 60, 10, Justification::centred);
    
    
    g.drawVerticalLine(3*histogramLen/4+30, 0, vertLineLen);
    g.drawText(String(1000.0*float(windowSize/2)/2.0/float(sampleRate)) + " ms", 3*histogramLen/4, textStart, 60, 10, Justification::centred);
    
    
    g.drawVerticalLine(histogramLen+30, 0, vertLineLen);
    g.drawText(String(1000.0*float(windowSize/2)/float(sampleRate)) + " ms", histogramLen, textStart, 60, 10, Justification::centred);
    

}
void Timescale::resized(){
    
}
 
//--------------------------------------------------------------------


GraphUnit::GraphUnit(PopupMenu men, juce::Colour c, String n, std::vector<float> s, std::vector<uint64> f){
    color = c;
    LD = new LabelDisplay(c,n);
    LD->setBounds(0,0,30,40);
    addAndMakeVisible(LD,false);
    HG = new HistoGraph(men,c, s[1], f);
    HG->setBounds(30,0,getWidth()-210,40);
    addAndMakeVisible(HG,false);
    SD = new StatDisplay(c,s);
    SD->setBounds(getWidth()-180,0,180,40);
    addAndMakeVisible(SD,false);
}
GraphUnit::~GraphUnit(){
    deleteAllChildren();
}
void GraphUnit::paint(Graphics& g){
        //g.setOpacity(1);
}
void GraphUnit::resized(){
    LD->setBounds(0,0,30,40);
    SD->setBounds(getWidth()-180,0,180,40);
    HG->setBounds(30,0,getWidth()-210,40);
}

//----------------

LabelDisplay::LabelDisplay(juce::Colour c, String n){
    color = c;
    name = n;
}
LabelDisplay::~LabelDisplay(){
    
}
void LabelDisplay::paint(Graphics& g){
    g.setColour(color);
    g.drawText(name,0, 0, 30, 40, juce::Justification::left);
}
void LabelDisplay::resized(){
    
}

//----------------

HistoGraph::HistoGraph(PopupMenu men, juce::Colour c, int m, std::vector<uint64> f){
    //menu = &men;
    color = c;
    histoData = f;
    max = m;
    //setMouseClickGrabsKeyboardFocus(true);
}

HistoGraph::~HistoGraph(){
    
}

void HistoGraph::paint(Graphics& g){
    g.setColour(Colours::snow);
    g.setOpacity(0.5);
    g.drawVerticalLine(getWidth()/2,5, getHeight());

    g.setColour(color);
    for (int i = 1 ; i < histoData.size() ; i++){
        if(max!=0)
            g.drawLine((i-1)*getWidth()/histoData.size(),getHeight()-(histoData.operator[](i-1)*getHeight()/max),(i)*getWidth()/histoData.size(),getHeight()-(histoData.operator[](i)*getHeight()/max));
        else
            g.drawLine((i-1)*getWidth()/histoData.size(),getHeight()-(histoData.operator[](i-1)*getHeight()),(i)*getWidth()/histoData.size(),getHeight()-(histoData.operator[](i)*getHeight()));
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

void HistoGraph::mouseMove(const MouseEvent &event){
    int posX = event.getMouseDownScreenX();
    if(histoData.size()>0){
         int valueY = histoData[float(posX)/float(getWidth())*float(histoData.size())];
        menu.clear();
        menu.addItem(1, "Bin: " + String(int(float(posX)/float(getWidth())*float(histoData.size()))+1)+ " Counts: " + String(valueY),false);
        menu.showAt(this);
    
    }
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
    g.drawText(String(stats[0]),0, 0, 60, 40, juce::Justification::right);
    g.drawText(String(stats[1]),60, 0, 60, 40, juce::Justification::right);
    g.drawText(String(stats[2]),120, 0, 60, 40, juce::Justification::right);
    }

void StatDisplay::resized(){
    
}



