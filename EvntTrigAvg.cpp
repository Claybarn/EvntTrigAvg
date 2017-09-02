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

#include <stdio.h>
#include "EvntTrigAvg.h"
#include "EvntTrigAvgCanvas.h"
//#include "HistogramLib/HistogramLib.h"
class EvntTrigAvg;

EvntTrigAvg::EvntTrigAvg()
    : GenericProcessor("Evnt Trig Avg")

{
    setProcessorType (PROCESSOR_TYPE_FILTER);
    windowSize = getDefaultSampleRate(); // 1 sec in samples
    binSize = getDefaultSampleRate()/100; // 10 milliseconds in samples
    updateSettings();
}

EvntTrigAvg::~EvntTrigAvg()
{
    
}

void EvntTrigAvg::setParameter(int parameterIndex, float newValue){
    bool changed = false;
    if (parameterIndex == 0 && triggerEvent != static_cast<int>(newValue)){
        triggerEvent = static_cast<int>(newValue);
        changed = true;
    }
    else if (parameterIndex == 1 && triggerChannel != static_cast<int>(newValue)){
        triggerChannel = static_cast<int>(newValue);
        changed = true;
    }
    else if(parameterIndex == 2 && binSize != newValue*(getSampleRate()/1000)){
        binSize = newValue*(getSampleRate()/1000);

        changed = true;
    }
    else if(parameterIndex == 3 && windowSize != newValue*(getSampleRate()/1000)){
        windowSize = newValue*(getSampleRate()/1000);
        changed = true;
    }
    else if (parameterIndex == 4)
        changed = true;
    
    // If anything was changed, delete all data and start over
    if (changed){
        minMaxMean.clear();
        spikeData.clear();
        ttlTimestampBuffer.clear();
        histogramData.clear();
        lastTTLCalculated=0;
        updateSettings();
    }
}

void EvntTrigAvg::updateSettings(){
    histogramData.clear();
    initializeHistogramArray();
    minMaxMean.clear();
    initializeMinMaxMean();
    electrodeMap.clear();
    electrodeMap = createElectrodeMap();
    electrodeLabels.clear();
    electrodeLabels = createElectrodeLabels();
    if(spikeData.size()!=getTotalSpikeChannels())
        spikeData.resize(getTotalSpikeChannels());
    electrodeSortedId.clear();
    if(electrodeSortedId.size()!=getTotalSpikeChannels())
        electrodeSortedId.resize(getTotalSpikeChannels());
    for(int electrodeIt = 0 ; electrodeIt < spikeData.size() ; electrodeIt++){
        electrodeSortedId[electrodeIt].push_back(0);
        if(spikeData[electrodeIt].size()<1)
            spikeData[electrodeIt].resize(1);
    }
}
void EvntTrigAvg::initializeHistogramArray(){
    const ScopedLock myScopedLock(mut);
    histogramData.resize(getTotalSpikeChannels());
    for(int electrodeIt = 0 ; electrodeIt < histogramData.size(); electrodeIt++){
        histogramData[electrodeIt].resize(1);
        for(int sortedIdIt = 0 ; sortedIdIt < histogramData[electrodeIt].size(); sortedIdIt++){
            if(histogramData[electrodeIt][sortedIdIt]!=NULL)
                delete histogramData[electrodeIt][sortedIdIt];
            histogramData[electrodeIt][sortedIdIt] = new uint64[1000]{0};
        }
    }
}

void EvntTrigAvg::initializeMinMaxMean(){
    minMaxMean.resize(getTotalSpikeChannels());
    for(int electrodeIt = 0 ; electrodeIt < getTotalSpikeChannels() ; electrodeIt++){
        minMaxMean[electrodeIt].push_back(new float[3]{0,0,0});
    }
}

bool EvntTrigAvg::enable(){
    return true;
}

bool EvntTrigAvg::disable(){
    return true;
}


void EvntTrigAvg::process(AudioSampleBuffer& buffer){
    
    checkForEvents(true);// see if got any spikes
    
    if(buffer.getNumChannels() != numChannels) // dont know if i use this
        numChannels = buffer.getNumChannels();
    if(ttlTimestampBuffer.size() > lastTTLCalculated && buffer.getNumSamples() + getTimestamp(0) >= ttlTimestampBuffer[lastTTLCalculated+1] + windowSize/2){ // if need to recalc
        recalc = true;
    }
    if(recalc){ // triggered after window time has expiered
        //process the data
        processSpikeData(spikeData, ttlTimestampBuffer);
        
        //clear the data
        for(int channelIterator = 0 ; channelIterator < spikeData.size() ; channelIterator++){
            for(int sortedIdIterator = 0 ; sortedIdIterator < spikeData[channelIterator].size() ; sortedIdIterator++){
                spikeData[channelIterator][sortedIdIterator].clear();
            }
        }
        
        //advance the TTL that needs to be calculated
        lastTTLCalculated+=1;
        //just recalculated, don't need to again until next ttl window has expired
        recalc=false;
        // tell canvas there was a change to the histogram
        readHistoData=true;
        }
}

void EvntTrigAvg::handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int sampleNum){
    if (triggerEvent < 0) return;
    else if (eventInfo->getChannelType() == EventChannel::TTL && eventInfo == eventChannelArray[triggerEvent])
    {// if TTL from right channel
        TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, eventInfo);
        if (ttl->getChannel() == triggerChannel)
            ttlTimestampBuffer.push_back(Event::getTimestamp(event)); // add timestamp of TTL to buffer
    }
}

void EvntTrigAvg::handleSpike(const SpikeChannel* spikeInfo, const MidiMessage& event, int samplePosition){
    SpikeEventPtr newSpike = SpikeEvent::deserializeFromMessage(event, spikeInfo);
    if (!newSpike)
        return;
    else {
        // extract information from spike
        
        const SpikeChannel* chan = newSpike->getChannelInfo();
        Array<sourceChannelInfo> chanInfo = chan->getSourceChannelInfo(); // has to stay Array
        int chanIDX = chanInfo[0].channelIDX;
        int sortedID = newSpike->getSortedID();
        int electrode = electrodeMap[chanIDX];
        if(sortedID!=0 && sortedID>idIndex.size()){ // respond to new sortedID
            idIndex.push_back(spikeData[electrode].size());// update map of what sorted ID is on what electrode
        }
            
        bool newID = true;
        for(int i = 0 ; i < electrodeSortedId[chanIDX].size() ; i++){
           if(sortedID == electrodeSortedId[chanIDX][i])
               newID=false;
        }
        if(newID){
            electrodeSortedId[chanIDX].push_back(sortedID);
            minMaxMean[electrode].push_back(new float[3]{0});
            histogramData[electrode].push_back(new uint64[1000]{0});
            for(int i = 0 ; i < electrodeSortedId.size() ; i++)
                std::cout<<electrodeSortedId[chanIDX][i]<<"\n"; //electrodeSortedId messed up
            spikeData[electrode].resize(spikeData[electrode].size()+1);
            }
        
        int relativeSortedID = 0;
        if (sortedID>0)
            relativeSortedID = idIndex[sortedID-1];
        spikeData[electrode][0].push_back(newSpike->getTimestamp());
        if (sortedID>0)
            spikeData[electrode][relativeSortedID].push_back(newSpike->getTimestamp());
    }
}

AudioProcessorEditor* EvntTrigAvg::createEditor(){
    editor = new EvntTrigAvgEditor (this, true);
    return editor;
}

void EvntTrigAvg::clearTTLTimestampBuffer(){
    ttlTimestampBuffer.clear();
}

float EvntTrigAvg::getSampleRate(){
    return juce::AudioProcessor::getSampleRate();
}

std::vector<uint64> EvntTrigAvg::getTTLTimestampBuffer(){
    return ttlTimestampBuffer;
}

int EvntTrigAvg::getLastTTLCalculated(){
    return lastTTLCalculated;
}

/** creates map to convert channelIDX to electrode number */
std::vector<int> EvntTrigAvg::createElectrodeMap(){
    std::vector<int> map;
    int numSpikeChannels = getTotalSpikeChannels();
    int electrodeCounter=0;
    for (int chanIt = 0 ; chanIt < numSpikeChannels ; chanIt++){
        const SpikeChannel* chan = getSpikeChannel(chanIt);
        // add to running count of each electrode
        map.resize(map.size()+chan->getNumChannels());
        Array<sourceChannelInfo> chanInfo = chan->getSourceChannelInfo(); // must stay Array data type
        for (int subChanIt = 0 ; subChanIt < chan->getNumChannels() ; subChanIt++){
            map[chanInfo[subChanIt].channelIDX]=electrodeCounter;
        }
        electrodeCounter+=1;
    }
    return map;
}

std::vector<String> EvntTrigAvg::createElectrodeLabels(){
    std::vector<String> map;
    int numSpikeChannels = getTotalSpikeChannels();
    map.resize(numSpikeChannels);
    String electrodeNames[3]{"Si ","St ","TT "};
    int electrodeCounter[3]{0};
    for (int chanIt = 0 ; chanIt < numSpikeChannels ; chanIt++){
        const SpikeChannel* chan = getSpikeChannel(chanIt);
        // add to running count of each electrode
        int chanType = chan->getChannelType();
        electrodeCounter[chanType]+=1;
        map[chanIt]=electrodeNames[chanType]+String(electrodeCounter[chanType]);
    }
    return map;
}

/** passes data into createHistogramData() by electrode and sorted ID */
// TODO modify so only new data is analyzed to save processing time/memory
void EvntTrigAvg::processSpikeData(std::vector<std::vector<std::vector<uint64>>> spikeData,std::vector<uint64> ttlData){
    const ScopedLock myScopedLock(mut);
    for (int channelIterator = 0 ; channelIterator < getTotalSpikeChannels() ; channelIterator++){
        for (int sortedIdIterator = 0 ; sortedIdIterator < spikeData[channelIterator].size() ; sortedIdIterator++){
             uint64* data = createHistogramData(spikeData[channelIterator][sortedIdIterator],ttlData);
            if(minMaxMean.size()<channelIterator+1)
                minMaxMean.resize(channelIterator+1);
            if(minMaxMean[channelIterator].size()<sortedIdIterator+1)
                minMaxMean[channelIterator].resize(channelIterator+1);
            for(int dataIterator = 0 ; dataIterator<windowSize/binSize ; dataIterator++){
                histogramData[channelIterator][sortedIdIterator][dataIterator] = histogramData[channelIterator][sortedIdIterator][dataIterator]+*(data + dataIterator);
            }
            minMaxMean[channelIterator][sortedIdIterator][0]= findMin(histogramData[channelIterator][sortedIdIterator]);
            minMaxMean[channelIterator][sortedIdIterator][1]= findMax(histogramData[channelIterator][sortedIdIterator]);
            minMaxMean[channelIterator][sortedIdIterator][2] = findMean(histogramData[channelIterator][sortedIdIterator]);

        }
    }
}

/** returns bin counts */
uint64* EvntTrigAvg::createHistogramData(std::vector<uint64> spikeData, std::vector<uint64> ttlData){
    uint64 numberOfBins = windowSize/binSize;
    std::vector<uint64> histoData;
    for(int ttlIterator = 0 ; ttlIterator < ttlData.size() ; ttlIterator++){
        for(int spikeIterator = 0 ; spikeIterator < spikeData.size() ; spikeIterator++){
            int relativeSpikeValue = int(spikeData[spikeIterator])-int(ttlData[ttlIterator]);
            if (relativeSpikeValue >= -int(windowSize)/2 && relativeSpikeValue <= int(windowSize)/2){
                uint64 bin = binDataPoint(0, numberOfBins, binSize, relativeSpikeValue+windowSize/2);
                histoData.push_back(bin);
            }
        }
    }
    return binCount(histoData,numberOfBins);
}


// Returns the bin a data point belongs to given the very first bin, the very last bin, bin size and the data point to bin, currently only works for positive numbers (can get around by adding minimum value to all values
uint64 EvntTrigAvg::binDataPoint(uint64 startBin, uint64 endBin, uint64 binSize, uint64 dataPoint){
    uint64 binsInRange = (endBin-startBin);
    uint64 binsToSearch = binsInRange/2;
    if (binsToSearch <= 1){
        
        if (dataPoint < (startBin+binsToSearch)*binSize){
            return startBin;
        }
        else if (dataPoint < (startBin+1+binsToSearch) * binSize){
            return startBin+1;
        }
        else{
            return startBin+2;
        }
    }

    else if (dataPoint < (startBin+binsToSearch)*binSize){ // if in first half of search range
        return binDataPoint(startBin,startBin+(binsToSearch),binSize,dataPoint);
    }
    else if (dataPoint >= (startBin+binsToSearch) * binSize){ // if in second half of search range
        return binDataPoint(startBin+(binsToSearch),endBin,binSize,dataPoint);
    }
    else{
        return NULL;
    }
}

uint64* EvntTrigAvg::binCount(std::vector<uint64> binData,uint64 numberOfBins){
    for (int i = 0 ; i < numberOfBins ; i++){
        bins[i]=0;
    }
    for (int dataIterator = 0 ; dataIterator < binData.size() ; dataIterator++){
        bins[binData[dataIterator]] = bins[binData[dataIterator]]+1;
    }
    return bins;
}

uint64 EvntTrigAvg::getBinSize(){
    return binSize;
}

uint64 EvntTrigAvg::getWindowSize(){
    return windowSize;
}

std::vector<std::vector<uint64*>> EvntTrigAvg::getHistoData(){
        readHistoData=false;
        return histogramData;
}

bool EvntTrigAvg::shouldReadHistoData(){
    return readHistoData;
}

int EvntTrigAvg::findMin(uint64* data_){
    //int min = INT_MAX;
    int min = 2147483647;
    for (int i = 0 ; i < windowSize/binSize ; i++){
        int data = data_[i];
        if(data<min){
            min=data;
        }
    }
    return min;
}

int EvntTrigAvg::findMax(uint64* data_){
    //int max = INT_MIN;
    int max =-2147483647;
    for (int i = 0 ; i < windowSize/binSize ; i++){
        int data = data_[i];
        if(data>max){
            max=data;
        }
    }
    return max;
}

float EvntTrigAvg::findMean(uint64* data_){
    int runningSum=0;
    for(int i=0 ; i < windowSize/binSize ; i++){
        runningSum += data_[i];
    }
    float mean = float(runningSum)/float(windowSize/binSize);
    return mean;
}

std::vector<std::vector<float*>> EvntTrigAvg::getMinMaxMean(){
    return minMaxMean;
}

std::vector<String> EvntTrigAvg::getElectrodeLabels(){
    return electrodeLabels;
}
std::vector<std::vector<int>> EvntTrigAvg::getElectrodeSortedId(){
    return electrodeSortedId;
}

void EvntTrigAvg::clearHistogramData(uint64 * dataptr){
    for(int i = 0 ; i < 1000 ; i++)
        dataptr[i] = 0;
}
