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


// For LFP, check out getCurrentSample(int& chan) function in spike sorter
// remove middleman step that is spike info data, just place into spike data directly
#include <stdio.h>
#include "EvntTrigAvg.h"
#include "EvntTrigAvgCanvas.h"
#include "HistogramLib/HistogramLib.h"
class EvntTrigAvg;

EvntTrigAvg::EvntTrigAvg()
    : GenericProcessor("Evnt Trig Avg")

{
    
    //std::cout<<"UINT64_MAX: " << UINT64_MAX<<"\n";
    setProcessorType (PROCESSOR_TYPE_FILTER);
    windowSize = getDefaultSampleRate(); // 1 sec
    binSize = getDefaultSampleRate()/100; // 10 milliseconds
    //spikeInfoBuffer.resize(UINT64_MAX);
    //ttlTimestampBuffer.resize(UINT64_MAX);
    //spikeData.resize(UINT64_MAX);
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
    else if(parameterIndex == 2 && binSize != newValue/(getSampleRate()/1000)){
        binSize = newValue/(getSampleRate()/1000);
        changed = true;
    }
    else if(parameterIndex == 3 && windowSize != newValue/(getSampleRate()/1000)){
        windowSize = newValue/(getSampleRate()/1000);
        changed = true;
    }
    else if (parameterIndex == 4)
        changed = true;
    // If anything was changed, delete all data and start over
    if (changed){
        spikeData.clear();
        spikeData.resize(getTotalSpikeChannels());
        minMaxMean.clear();
    }
    
}

void EvntTrigAvg::updateSettings(){
    electrodeMap.clear();
    electrodeMap = createElectrodeMap();
    electrodeLabels.clear();
    electrodeLabels = createElectrodeLabels();
    
}

bool EvntTrigAvg::enable(){
    return true;
}

bool EvntTrigAvg::disable(){
    return true;
}


void EvntTrigAvg::process(AudioSampleBuffer& buffer){
    checkForEvents(true);
    //std::cout<<"timestamp at end of buffer: " << buffer.getNumSamples() + getTimestamp(0)<<"\n";
   // if(ttlTimestampBuffer.size() > 0)
   //     std::cout<<"end of window " << ttlTimestampBuffer[lastTTLCalculated+1] + windowSize/2 <<"\n";
    //std::cout<<buffer.getNumChannels()<<"\n";
    if(buffer.getNumChannels() != numChannels)
        numChannels = buffer.getNumChannels();
    //std::cout<<"last TTL calc: " << lastTTLCalculated << "\n";
    if(ttlTimestampBuffer.size() > lastTTLCalculated && buffer.getNumSamples() + getTimestamp(0) >= ttlTimestampBuffer[lastTTLCalculated+1] + windowSize/2){ // collected all data to calculate spike timings in reference to TTL, and haven't already calculated this
        // != std::distance(ttlTimestampBuffer.begin(),ttlTimestampBuffer.end())
       // std::cout<<"in long if \n";
        recalc = true;
        
    }
    if(recalc){
        /*
        // Dont think i need this if statement
        if(spikeInfoBuffer.size()>0){
            for(unsigned long i = 0 ; i < spikeInfoBuffer.size() ; i++){ //for each spike, add to spike data
                if(spikeInfoBuffer[i].sortedID>spikeData.size()){
                    spikeData.resize(spikeInfoBuffer[i].sortedID);
                }
                spikeData[spikeInfoBuffer[i].sortedID-1].push_back(spikeInfoBuffer[i].timestamp);
            }
            spikeInfoBuffer.clear();
            */
        histogramData=processSpikeData(spikeData, ttlTimestampBuffer);
        lastTTLCalculated+=1;
        recalc=false;
        readHistoData=true;
    }
    
    
}

void EvntTrigAvg::handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int sampleNum){
    //std::cout<<"recieved event on: " << event.getChannel() << "\n";
    if (triggerEvent < 0) return;
    /*
    else if (Event::getEventType(event) == EventChannel::TTL && event.getChannel() == triggerChannel){
        ttlTimestampBuffer.push_back(Event::getTimestamp(event));
    }
    */
    else if (eventInfo->getChannelType() == EventChannel::TTL && eventInfo == eventChannelArray[triggerEvent])
    {
        TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, eventInfo);
        if (ttl->getChannel() == triggerChannel)
            ttlTimestampBuffer.push_back(Event::getTimestamp(event));
    }
}

void EvntTrigAvg::handleSpike(const SpikeChannel* spikeInfo, const MidiMessage& event, int samplePosition){
    SpikeEventPtr newSpike = SpikeEvent::deserializeFromMessage(event, spikeInfo);
    int numSpikeChannels = getTotalSpikeChannels();
    std::cout<<"totalSpikeChannels: " << numSpikeChannels << "\n";
    for (int chanIterator = 0 ; chanIterator < numSpikeChannels ; chanIterator++){
        const SpikeChannel* chan = getSpikeChannel(chanIterator);
        std::cout<<"channel type: " << chan->getChannelType() << "\n";
        std::cout<<"number of channels : " << chan->getNumChannels() << "\n";
        for(int blah = 0 ; blah < chan->getSourceChannelInfo().size() ; blah++){
            std::cout<<"index of sourceChannelInfo: " << blah << "\n";
            std::cout<<"processorID: " <<chan->getSourceChannelInfo()[blah].processorID<<"\n";
            std::cout<< "subProcessorID: "<<chan->getSourceChannelInfo()[blah].subProcessorID<<"\n";
            std::cout<<"channelIDX: " <<chan->getSourceChannelInfo()[blah].channelIDX<<"\n";
            std::cout<<"\n";
        }
        std::cout<<"\n";
    }

    if (!newSpike)
        return;
    else if(newSpike->getSortedID()>0){
        if(newSpike->getSortedID()>spikeData.size()){
            spikeData.resize(newSpike->getSortedID());
        }
        
        // what do the indices in Array<sourceChannelInfo> mean?? Think mean channel of what electrode (ie tetrode will be length of 4, stereo length of 2
        Array<sourceChannelInfo> chanInfoArray = newSpike->getChannelInfo()->getSourceChannelInfo();
        // NEED TO FIX
        //spikeData[chanInfoArray[0].channelIDX][newSpike->getSortedID()].push_back(newSpike->getTimestamp());
        
    }
        /*
    const SpikeChannel* channelInfo = newSpike->getChannelInfo();
    Array<sourceChannelInfo> sourceChannelInfo = newSpike->getChannelInfo()->getSourceChannelInfo();
    //returns int getSpikeChannelIndex(<#int channelIdx#>, <#int processorID#>)
    for(int i = 0 ; i < sourceChannelInfo.size() ; i++){
        std::cout<<"i: " << i << "\n";
        std::cout<<"sorted ID: " << newSpike->getSortedID() << "\n";
        std::cout<<"processorID: " <<sourceChannelInfo[i].processorID<<"\n";
        std::cout<< "subProcessorID: "<<sourceChannelInfo[i].subProcessorID<<"\n";
        std::cout<<"channelIDX: " <<sourceChannelInfo[i].channelIDX<<"\n";
        std::cout<<"spikeChannelIndex: " << getSpikeChannelIndex(sourceChannelInfo[i].channelIDX, sourceChannelInfo[i].processorID)<<"\n";
        const SpikeChannel* var = getSpikeChannel(getSpikeChannelIndex(sourceChannelInfo[i].channelIDX, sourceChannelInfo[i].processorID));
        //std::cout<<"name: " << channelInfo->getC << "\n";
     
        
    }
     */

}



AudioProcessorEditor* EvntTrigAvg::createEditor(){
    editor = new EvntTrigAvgEditor (this, true);
    return editor;
}
void EvntTrigAvg::clearTTLTimestampBuffer(){
    ttlTimestampBuffer.clear();
}

void EvntTrigAvg::clearSpikeInfoBuffer(){
    spikeInfoBuffer.clear();
}



float EvntTrigAvg::getSampleRate(){
    return juce::AudioProcessor::getSampleRate();
}

// think relic
std::vector<EvntTrigAvg::spikeInfo> EvntTrigAvg::getSpikeInfoBuffer(){
    return spikeInfoBuffer;
}

std::vector<uint64> EvntTrigAvg::getTTLTimestampBuffer(){
    return ttlTimestampBuffer;
}
// think relic
unsigned long EvntTrigAvg::getSpikeInfoBufferSize(){
    return spikeInfoBuffer.size();
}

unsigned long EvntTrigAvg::getTTLTimestampBufferSize(){
    return ttlTimestampBuffer.size();
}

/** creates map to convert channelIDX to electrode type and number */
std::vector<int> EvntTrigAvg::createElectrodeMap(){
    std::vector<int> map;
    int numSpikeChannels = getTotalSpikeChannels();
    int electrodeCounter=0;
    for (int chanIt = 0 ; chanIt < numSpikeChannels ; chanIt++){
        const SpikeChannel* chan = getSpikeChannel(chanIt);
        // add to running count of each electrode
        map.resize(map.size()+chan->getNumChannels());
        Array<sourceChannelInfo> chanInfo = chan->getSourceChannelInfo();
        for (int subChanIt = 0 ; subChanIt < chan->getNumChannels() ; chanIt++){
            map[chanInfo[subChanIt].channelIDX] = electrodeCounter;
        }
        electrodeCounter+=1;
    }
    return map;
}


std::vector<String> EvntTrigAvg::createElectrodeLabels(){
    std::vector<String> map;
    int numSpikeChannels = getTotalSpikeChannels();
    map.resize(numSpikeChannels);
    String electrodeNames[3]{"SI","ST","TT"};
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

 


/** pass data into createHistogramData() by sortedID */
// TODO modify so only new data is analyzed to save processing time/memory
std::vector<std::vector<uint64>> EvntTrigAvg::processSpikeData(std::vector<std::vector<std::vector<uint64>>> spikeData,std::vector<uint64> ttlData){
    //std::cout<<"processing spike data \n";
    std::vector<std::vector<uint64>> processedSpikeData;
    for (int channelIterator = 0 ; channelIterator < numChannels ; channelIterator++)
        for (int sortedIdIterator = 0 ; sortedIdIterator < spikeData.size() ; sortedIdIterator++){
            std::vector<uint64> toAdd = createHistogramData(spikeData[channelIterator][sortedIdIterator],ttlData);
            if(minMaxMean.size()<sortedIdIterator+1)
                minMaxMean.resize(sortedIdIterator+1);
            if(minMaxMean[sortedIdIterator].size()<3)
                minMaxMean[sortedIdIterator].resize((3));
            minMaxMean[sortedIdIterator][0]=findMin(toAdd);
            minMaxMean[sortedIdIterator][1]=findMax(toAdd);
            minMaxMean[sortedIdIterator][2]=findMean(toAdd);
            for (int i = 0 ; i < toAdd.size() ; i++){
                if (i >= processedSpikeData.size()){
                    processedSpikeData.resize(i+1);
                }
                processedSpikeData[sortedIdIterator].push_back(toAdd[i]);
            }
    }
    return processedSpikeData;
}


/** returns bin counts */
std::vector<uint64> EvntTrigAvg::createHistogramData(std::vector<uint64> spikeData, std::vector<uint64> ttlData){
    std::vector<uint64> histoData;
    uint64 numberOfBins = windowSize/binSize;
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


// Returns the bin a data point belongs to given the very first value covered by the bins, the very last value covered by then bins, bin size and the data point to bin, currently only works for positive numbers (can get around by adding minimum value


uint64 EvntTrigAvg::binDataPoint(uint64 startBin, uint64 endBin, uint64 binSize, uint64 dataPoint){
    uint64 binsInRange = (endBin-startBin)+1;
    uint64 binsToSearch = binsInRange/2;
    if (binsToSearch <= 1){
        return startBin;
    }
    else if (dataPoint < (startBin+binsToSearch)*binSize){ // if in first half of search range
        return binDataPoint(startBin,startBin+(binsToSearch-1),binSize,dataPoint);
    }
    else if (dataPoint >= (startBin+binsToSearch) * binSize){ // if in second half of search range
        return binDataPoint(startBin+(binsToSearch-1),endBin,binSize,dataPoint);
    }
    else{
        return NULL;
    }
}

std::vector<uint64> EvntTrigAvg::binCount(std::vector<uint64> binData, uint64 numberOfBins){
    std::vector<uint64> bins(numberOfBins,0); // initialize with 0
    for (int dataIterator = 0 ; dataIterator < binData.size() ; dataIterator++){
        bins[binData[dataIterator]]+=1;
    }
    return bins;
}

uint64 EvntTrigAvg::getBinSize(){
    return binSize;
}

uint64 EvntTrigAvg::getWindowSize(){
    return windowSize;
}



std::vector<std::vector<uint64>> EvntTrigAvg::getHistoData(){
        readHistoData=false;
        return histogramData;
}

bool EvntTrigAvg::shouldReadHistoData(){
    return readHistoData;
}

int EvntTrigAvg::findMin(std::vector<uint64> data){
    int min = INT_MAX;
    for (int i = 0 ; i < data.size() ; i++){
        if(data[i]<min){
            min=data[i];
        }
    }
    return min;
}

int EvntTrigAvg::findMax(std::vector<uint64> data){
    int max = INT_MIN;
    for (int i = 0 ; i < data.size() ; i++){
        int dataPoint = data[i];
        if(dataPoint>max){
            max=dataPoint;
        }
    }
    return max;
}

float EvntTrigAvg::findMean(std::vector<uint64> data){
    int runningSum=0;
    for(int i=0 ; i < data.size() ; i++){
        runningSum += data[i];
    }
    float mean = float(runningSum)/float(data.size());
    return mean;
}

std::vector<std::vector<float>> EvntTrigAvg::getMinMaxMean(){
    return minMaxMean;
}

std::vector<String> EvntTrigAvg::getElectrodeLabels(){
    return electrodeLabels;
}
