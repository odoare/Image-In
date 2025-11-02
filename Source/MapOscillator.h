/*
  ==============================================================================

    MapOscillator.h
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#pragma once

#include "ImageBuffer.h"
#include "ReaderBase.h"
#include "EllipseReader.h"
#include "BitmapDataManager.h"

struct GlobalParameters;
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

    void prepareToPlay (double sampleRate);
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int startSample, int numSamples, BitmapDataManager& bitmapDataManager, const juce::AudioBuffer<float>& modulatorBuffer);
    void rebuildReaders (const juce::Array<ReaderBase::Type>& types);
    void updateParameters (const GlobalParameters& params, int readerIndex);
    EllipseReader* addEllipseReader();
    void removeReader (int index);
    int getNumReaders() const;
    ReaderBase* getReader (int index);
    const juce::OwnedArray<ReaderBase>& getReaders() const { return readers; }

private:
    juce::OwnedArray<ReaderBase> readers;
    juce::AudioBuffer<float> readerBuffer;
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapOscillator)
};
