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

#ifndef __EvntTrigAvg_H_3F920F95__
#define __EvntTrigAvg_H_3F920F95__

#include <ProcessorHeaders.h>
#include "EvntTrigAvgEditor.h"
#include <vector>

class EvntTrigAvgEditor;

/**


*/

class EvntTrigAvg : public GenericProcessor
{
public:

    // CONSTRUCTOR AND DESTRUCTOR //

    /** constructor */
    EvntTrigAvg();

    /** destructor */
    ~EvntTrigAvg();


    // PROCESSOR METHODS //

    void handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int sampleNum) override;
    void handleSpike(const SpikeChannel* channelInfo, const MidiMessage& event, int samplePosition) override;

    void process(AudioSampleBuffer& buffer) override;

    /** Used to alter parameters of data acquisition. */
    void setParameter(int parameterIndex, float newValue) override;

    /** Called whenever the signal chain is altered. */
    void updateSettings() override;

    /** Called prior to start of acquisition. */
    bool enable() override;

    /** Called after acquisition is finished. */
    bool disable() override;

    /** Creates the EvntTrigAvgEditor. */
    AudioProcessorEditor* createEditor() override;
    
    void clearTTLTimestampBuffer();
    float getSampleRate();
    std::vector<uint64> getTTLTimestampBuffer();
    int getLastTTLCalculated();
    uint64 getWindowSize();
    uint64 getBinSize();

    //create histogram data
    uint64* createHistogramData(std::vector<uint64> spikeData, std::vector<uint64> ttlData);
    uint64 binDataPoint(uint64 startBin, uint64 endBin, uint64 binSize, uint64 dataPoint);
    void processSpikeData(std::vector<std::vector<std::vector<uint64>>> spikeData,std::vector<uint64> ttlData);
    uint64* binCount(std::vector<uint64> binData, uint64 numberOfBins);
    std::vector<std::vector<uint64*>> getHistoData();
    bool shouldReadHistoData();
    int findMin(uint64* data_);
    int findMax(uint64* data_);
    float findMean(uint64* data_);
    std::vector<std::vector<float*>> getMinMaxMean();
    std::vector<int> createElectrodeMap();
    std::vector<String> createElectrodeLabels();
    std::vector<String> getElectrodeLabels();
    std::vector<std::vector<int>> getElectrodeSortedId();
private:
    CriticalSection mut;
    void initializeHistogramArray();
    void initializeMinMaxMean();
    std::atomic<int> triggerEvent;
    std::atomic<int> triggerChannel;

    uint64 bins[1000]; // allocate space for 1000 bins

    int numChannels = 0;
    bool readHistoData = false;
    bool recalc = false;
    int lastTTLCalculated = 0;
    uint64 windowSize;
    uint64 binSize;
    
    //TODO make a struct to move all the information in a more intuitive way
    
    std::vector<uint64> ttlTimestampBuffer;
    std::vector<std::vector<std::vector<uint64>>> spikeData;// channel.sortedID.spikeInstance.timestamp
    //std::vector<std::vector<std::vector<uint64>>> histogramData;
    void clearHistogramData(uint64 *);
    //TODO change above to below
    std::vector<std::vector<uint64 *>> histogramData;
    std::vector<std::vector<float*>> minMaxMean;
    std::vector<int> electrodeMap;
    std::vector<String> electrodeLabels;
    std::vector<int> idIndex; //sorted ID, electrode. used to match a sortedID with its electrode
    std::vector<std::vector<int>> electrodeSortedId; 
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EvntTrigAvg);

};



#endif  // __EvntTrigAvg_H_3F920F95__

