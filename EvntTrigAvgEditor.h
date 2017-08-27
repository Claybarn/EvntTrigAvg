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

#ifndef __EvntTrigAvgEDITOR_H_F0BD2DD9__
#define __EvntTrigAvgEDITOR_H_F0BD2DD9__

#include <VisualizerEditorHeaders.h>


class EvntTrigAvgCanvas;
class EvntTrigAvg;
/**

*/

class EvntTrigAvgEditor : public VisualizerEditor,
    public Label::Listener,
    public ComboBox::Listener
{
    
public:
    
    EvntTrigAvgEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    virtual ~EvntTrigAvgEditor();
    void buttonEvent(Button* button);
    void labelTextChanged(Label* label);
    void comboBoxChanged(ComboBox* comboBox);
    void sliderEvent(Slider* slider);
    void channelChanged (int chan, bool newState) override;
    void updateSettings();
    Visualizer* createNewCanvas();
    
    EvntTrigAvgCanvas* evntTrigAvgCanvas;
    
    
    /*
    void checkSettings();
    void setThresholdValue(int chan, double threshold);
    //void updateAdvancerList();
    void refreshElectrodeList(int selected = 0);
    void setSelectedElectrode(int i);
    int getSelectedElectrode();
    void drawElectrodeButtons(int);
    void removeElectrode(int index);
    void editElectrode(int index, int chan, int newChan);
*/
    
private:
    struct EventSources
    {
        unsigned int eventIndex;
        unsigned int channel;
    };
    std::vector<EventSources> eventSourceArray;
    
    //  ComboBox* electrodeTypes;
    EvntTrigAvg* processor;
    ScopedPointer<ComboBox> triggerChannel;
    ScopedPointer<Label> binSize, windowSize, channelLabel, binLabel, windowLabel;
    Font font;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EvntTrigAvgEditor);

};




#endif  // __EvntTrigAvgEDITOR_H_F0BD2DD9__
