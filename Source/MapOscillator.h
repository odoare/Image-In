/*
  ==============================================================================

    MapOscillator.h
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "ImageBuffer.h"
#include "ReaderBase.h"
#include "LineReader.h"
#include "CircleReader.h"

class LFO;

/**
    This class manages an ImageBuffer and a collection of LineReaders to
    generate a composite audio signal.
*/
class MapOscillator
{
public:
    MapOscillator();
    ~MapOscillator();

    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

    ImageBuffer& getImageBuffer() { return imageBuffer; }
    const ImageBuffer& getImageBuffer() const { return imageBuffer; }

    LineReader* addLineReader();
    CircleReader* addCircleReader();
    void removeReader (int index);
    int getNumReaders() const;
    ReaderBase* getReader (int index);
    const juce::OwnedArray<ReaderBase>& getReaders() const { return readers; }

    void setLFOs (LFO* lfo1, LFO* lfo2);

private:
    ImageBuffer imageBuffer;
    juce::OwnedArray<ReaderBase> readers;
    juce::AudioBuffer<float> readerBuffer;
    LFO* lfos[2] = { nullptr, nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapOscillator)
};
