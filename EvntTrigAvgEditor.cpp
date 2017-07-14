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

#include "EvntTrigAvgEditor.h"
#include "EvntTrigAvgCanvas.h"
#include "EvntTrigAvg.h"

#include <stdio.h>



EvntTrigAvgEditor::EvntTrigAvgEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors=true)
    : VisualizerEditor(parentNode, 300, useDefaultParameterEditors), evntTrigAvgCanvas(nullptr)

{
    tabText = "Evnt Trig Avg";
    desiredWidth = 200;

    processor = (EvntTrigAvg*) getProcessor();

    triggerChannel = new ComboBox("triggerChannel");
    triggerChannel->addListener(this);
    triggerChannel->setBounds(100,30,80,20);
    for (int i = 0; i < 16; i++)
        triggerChannel->addItem(String(i+1),i+2); // start numbering at one for user-visible channels
    triggerChannel->setTooltip("Select TTL channel to align data to");
    triggerChannel->setSelectedId(4);
    addAndMakeVisible(triggerChannel);
    
    binSize = new Label("binSize","bin size");
    binSize->setFont(Font("Default", 12, Font::plain));
    binSize->setEditable(true);
    binSize->setBounds(100,60,80,20);
    binSize->setColour(Label::textColourId, Colours::white);
    binSize->setTooltip("Set the bin size of the histogram in milliseconds");
    binSize->setText(String(10),dontSendNotification);
    addAndMakeVisible(binSize);

    windowSize = new Label("windowSize","windowSize");
    windowSize->setFont(Font("Default", 12, Font::plain));
    windowSize->setEditable(true);
    windowSize->setBounds(100,90,80,20);
    windowSize->addListener(this);
    windowSize->setColour(Label::textColourId, Colours::white);
    windowSize->setTooltip("Set the window size of the histogram in milliseconds");
    windowSize->setText(String(1000),dontSendNotification);
    addAndMakeVisible(windowSize);
    
    channelLabel = new Label("channelLabel","channel label");
    channelLabel->setFont(Font("Default", 12, Font::plain));
    channelLabel->setEditable(false);
    channelLabel->setBounds(10,30,80,20);
    channelLabel->setColour(Label::textColourId, Colours::white);
    channelLabel->setText("Channel: ",dontSendNotification);
    addAndMakeVisible(channelLabel);
    
    binLabel = new Label("binLabel","bin label");
    binLabel->setFont(Font("Default", 12, Font::plain));
    binLabel->setEditable(false);
    binLabel->setBounds(10,60,80,20);
    binLabel->setColour(Label::textColourId, Colours::white);
    binLabel->setText("Bin Size (ms): ",dontSendNotification);
    addAndMakeVisible(binLabel);

    windowLabel = new Label("binLabel","bin label");
    windowLabel->setFont(Font("Default", 12, Font::plain));
    windowLabel->setEditable(false);
    windowLabel->setBounds(10,90,90,20);
    windowLabel->setColour(Label::textColourId, Colours::white);
    windowLabel->setText("Window Size (ms): ",dontSendNotification);
    addAndMakeVisible(windowLabel);

                      
}

Visualizer* EvntTrigAvgEditor::createNewCanvas()
{

    EvntTrigAvg* processor = (EvntTrigAvg*) getProcessor();
    evntTrigAvgCanvas = new EvntTrigAvgCanvas(processor);
    //ActionListener* listener = (ActionListener*) EvntTrigAvgCanvas;
    //getUIComponent()->registerAnimatedComponent(listener);
    return evntTrigAvgCanvas;
}


EvntTrigAvgEditor::~EvntTrigAvgEditor()
{

}

void EvntTrigAvgEditor::sliderEvent(Slider* slider)
{
    if (canvas!= nullptr)
        canvas->repaint();

}


void EvntTrigAvgEditor::buttonEvent(Button* button)
{
}

void EvntTrigAvgEditor::labelTextChanged(Label* label)
{
    
    if (label == binSize)
    {
        processor->setParameter(1,label->getText().getIntValue());
    }
    else if (label == windowSize)
    {
        processor->setParameter(2,label->getText().getIntValue());
    }
    
}


void EvntTrigAvgEditor::comboBoxChanged(ComboBox* comboBox)
{
    EvntTrigAvg* processor = (EvntTrigAvg*) getProcessor();
    std::cout<<"input channel changed to: " << comboBox->getText().getIntValue()-1 << "\n";
    processor->setParameter(0,comboBox->getText().getIntValue()-1);
}

void EvntTrigAvgEditor::channelChanged (int chan, bool newState){
    
}
