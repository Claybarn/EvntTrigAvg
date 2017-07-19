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
    // called when the component's tab becomes visible again
    resized();
}

void EvntTrigAvgCanvas::resized()
{
    viewport->setBounds(0,0,getWidth(),getHeight());
    display->setBounds(0,0,getWidth()-scrollBarThickness, getHeight());
    std::cout<<"canvas child components: " << getNumChildComponents() << "\n";
    repaint();
    //spikeDisplay->setBounds(0,0,getWidth()-140, spikeDisplay->getTotalHeight());
}

void EvntTrigAvgCanvas::paint(Graphics& g)
{
    /*
    
    //if(processor->shouldReadHistoData()){
        histoData = processor->getHistoData();
        minMaxMean = processor->getMinMaxMean();
        g.fillAll(Colours::darkgrey);
        int width=getWidth();
        int height=getHeight();
        g.setColour(Colours::snow);
        g.drawLine(width/8*3, border, width/8*3 , height-border, 1);
        //g.drawText(<#const juce::String &text#>, <#int x#>, <#int y#>, <#int width#>, <#int height#>, <#juce::Justification justificationType#>)
    g.drawText("Bin: ",width/12, 5, width/8, 20, juce::Justification::left);

    g.drawText("Min.", 9*width/12, 5, width/8, 20, juce::Justification::left);
    g.drawText("Max", 10*width/12, 5, width/8, 20, juce::Justification::left);
    g.drawText("Mean", 11*width/12, 5, width/8, 20, juce::Justification::left);
        int drawWidth = width-20-width/4;
        int drawHeight = 40;
        //int xOffset =
        for (int ID = 0 ; ID < histoData.size() ; ID++){
            int lineX= drawWidth/(histoData[ID].size()-1);
            int yFact = 2;

            for (int i = 1 ; i < histoData[ID].size() ; i++){
                g.setColour(channelColours[(ID+16)%16]);
                
            
                //std::cout<<"height offset: " << drawHeight << "\n";
                //std::cout<<"point 1, point 2: " <<histoData[ID][i-1]+drawHeight*(ID+1) << ", " << histoData[ID][i]+drawHeight*ID<<"\n";
                g.drawLine((i-1)*lineX+10,-yFact*histoData[ID][i-1]+drawHeight*(ID+1),(i)*lineX+10,-yFact*histoData[ID][i]+drawHeight*(ID+1));
                g.drawText(String(minMaxMean[ID][0]), 9*width/12, 5+drawHeight*(ID+1), width/8, 20, juce::Justification::left);
                g.drawText(String(minMaxMean[ID][1]), 10*width/12, 5+drawHeight*(ID+1), width/8, 20, juce::Justification::left);
                g.drawText(String(minMaxMean[ID][2]), 11*width/12, 5+drawHeight*(ID+1), width/8, 20, juce::Justification::left);
            }
        //}

        //update();
        }
    
       //int numberOfPoints = 2*processor->getWindowSize()/processor->getBinSize();
    //
    /*std::vector<std::vector<uint64>> histoData = processor->processSpikeData(spikeData, ttlTimestamps);
     

    repaint();
    */
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
    histoData = processor->getHistoData();
    minMaxMean = processor->getMinMaxMean();
    g.fillAll(Colours::darkgrey);
    int width=getWidth();
    int height=getHeight();
    g.setColour(Colours::snow);
    int yFact = -2;
    int xOffset= 20;
    int drawWidth = width-20-width/4;
    int drawHeight = 40;
    g.drawLine((xOffset+drawWidth)/2, border, (xOffset+drawWidth)/2 , height-border, 1);
    //g.drawText(<#const juce::String &text#>, <#int x#>, <#int y#>, <#int width#>, <#int height#>, <#juce::Justification justificationType#>)
    g.drawText("ID",5, 5, width/8, 20, juce::Justification::left);
    g.drawText("Trials: " + String(processor->getTTLTimestampBufferSize()),(xOffset+drawWidth)/2-50,5,100,20,Justification::centred);
    g.drawText("Min.", 9*width/12, 5, 50, 20, Justification::right);
    g.drawText("Max", 10*width/12, 5, 50, 20, Justification::right);
    g.drawText("Mean", 11*width/12, 5, 50, 20, Justification::right);
    
    
    //HistoGraph(EvntTrigAvgCanvas* e, Colour c,float stats[3], std::vector<float> histoFactors);
    //deleteAllChildren();
    
    for (int ID = 0 ; ID < histoData.size() ; ID++){
        int lineX= drawWidth/(histoData[ID].size()-1);
        
        for (int i = 1 ; i < histoData[ID].size() ; i++){
            g.setColour(channelColours[(ID+16)%16]);
            
            
            //std::cout<<"height offset: " << drawHeight << "\n";
            //std::cout<<"point 1, point 2: " <<histoData[ID][i-1]+drawHeight*(ID+1) << ", " << histoData[ID][i]+drawHeight*ID<<"\n";
            g.drawLine((i-1)*lineX+xOffset,yFact*histoData[ID][i-1]+drawHeight*(ID+1),(i)*lineX+xOffset,yFact*histoData[ID][i]+drawHeight*(ID+1));
            g.drawText(String(ID+1), 5, drawHeight*(ID+1)-10, 15, 20, juce::Justification::left);
            g.drawText(String(minMaxMean[ID][0]), 9*width/12, drawHeight*(ID+1)-10, 50, 20, juce::Justification::right);
            g.drawText(String(minMaxMean[ID][1]), 10*width/12, drawHeight*(ID+1)-10, 50, 20, juce::Justification::right);
            g.drawText(String(minMaxMean[ID][2]), 11*width/12, drawHeight*(ID+1)-10, 50, 20, juce::Justification::right);
        }
        /* not working well
        for (int ID = 1 ; ID < histoData.size() ; ID++){
        std::cout<<"adding histo graph \n";
        HistoGraph* histogram = new HistoGraph(canvas,channelColours[(ID+sizeof(channelColours))%(sizeof(channelColours))],minMaxMean[ID],histoData[ID]);
        //std::cout<<"real histo data size: " << histoData[ID].size() << "\n";
        histograms.add(histogram);
        addAndMakeVisible(histogram);
        std::cout<<"display child components: " << getNumChildComponents() << "\n";
        histogram->paint(g);
         */
    }
    /*
    //int xOffset =
    for (int ID = 0 ; ID < histoData.size() ; ID++){
        int lineX= drawWidth/(histoData[ID].size()-1);
        int yFact = 2;
        
        for (int i = 1 ; i < histoData[ID].size() ; i++){
            g.setColour(channelColours[(ID+16)%16]);
            
            
            //std::cout<<"height offset: " << drawHeight << "\n";
            //std::cout<<"point 1, point 2: " <<histoData[ID][i-1]+drawHeight*(ID+1) << ", " << histoData[ID][i]+drawHeight*ID<<"\n";
            g.drawLine((i-1)*lineX+10,-yFact*histoData[ID][i-1]+drawHeight*(ID+1),(i)*lineX+10,-yFact*histoData[ID][i]+drawHeight*(ID+1));
            g.drawText(String(minMaxMean[ID][0]), 9*width/12, 5+drawHeight*(ID+1), width/8, 20, juce::Justification::left);
            g.drawText(String(minMaxMean[ID][1]), 10*width/12, 5+drawHeight*(ID+1), width/8, 20, juce::Justification::left);
            g.drawText(String(minMaxMean[ID][2]), 11*width/12, 5+drawHeight*(ID+1), width/8, 20, juce::Justification::left);
        }
     
        
        //update();
    }
    */
    //int numberOfPoints = 2*processor->getWindowSize()/processor->getBinSize();
    //
    /*std::vector<std::vector<uint64>> histoData = processor->processSpikeData(spikeData, ttlTimestamps);
     
     */
    repaint();

}


/*
void LfpDisplay::setNumChannels(int numChannels)
{
    numChans = numChannels;
    
    deleteAllChildren();
    
    channels.clear();
    channelInfo.clear();
    
    totalHeight = 0;
    
    for (int i = 0; i < numChans; i++)
    {
        
        //std::cout << "Adding new display for channel " << i << std::endl;
        
        LfpChannelDisplay* lfpChan = new LfpChannelDisplay(canvas, this, i);
        
        //lfpChan->setColour(channelColours[i % channelColours.size()]);
        lfpChan->setRange(range[canvas->getChannelType(i)]);
        lfpChan->setChannelHeight(canvas->getChannelHeight());
        
        addAndMakeVisible(lfpChan);
        
        channels.add(lfpChan);
        
        LfpChannelDisplayInfo* lfpInfo = new LfpChannelDisplayInfo(canvas, this, i);
        
        //lfpInfo->setColour(channelColours[i % channelColours.size()]);
        lfpInfo->setRange(range[canvas->getChannelType(i)]);
        lfpInfo->setChannelHeight(canvas->getChannelHeight());
        
        addAndMakeVisible(lfpInfo);
        
        channelInfo.add(lfpInfo);
        
        savedChannelState.add(true);
        
        totalHeight += lfpChan->getChannelHeight();
        
    }
    
    setColors();
    
    //std::cout << "TOTAL HEIGHT = " << totalHeight << std::endl;
    
    // // this doesn't seem to do anything:
    //canvas->fullredraw = true;
    //refresh();
    
}
*/

void EvntTrigAvgDisplay::refresh(){
    for (int i = 0 ; i < histograms.size() ; i++){
        histograms[i]->repaint();
    }
}

//--------------------------------------------------------------------



HistoGraph::HistoGraph(EvntTrigAvgCanvas* e, juce::Colour c, std::vector<float> s, std::vector<uint64> f){
    canvas = e;
    color = c;
    histoData = f;
    stats = s;
}

HistoGraph::~HistoGraph(){
    
}

void HistoGraph::paint(Graphics& g){
    int width = getWidth();
    int height = getHeight();
    
    g.setColour(color);
    for (int i = 1 ; i < histoData.size() ; i++){
        std::cout<<"start x: "<< (i-1)*width/histoData.size() << "start y: " << histoData.operator[](i-1)<< "end x: " << (i)*width/histoData.size()<< "end y:" << histoData.operator[](i)<< "\n:";
        g.drawLine((i-1)*width/histoData.size(),histoData.operator[](i-1),(i)*width/histoData.size(),histoData.operator[](i));
    }
    auto size = stats.size();
    if (size>0){
        //g.drawText(String(stats->operator[](0)), 9*width/12,0, width/8, 20, juce::Justification::left);
        //g.drawText(String(stats->operator[](1)), 10*width/12,0, width/8, 20, juce::Justification::left);
        //g.drawText(String(stats->operator[](2)), 11*width/12, 0, width/8, 20, juce::Justification::left);
        //std::cout<<"inside if with stats size = " << size << "\n";
    }
    //std::cout<<"painted histograph \n";
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


