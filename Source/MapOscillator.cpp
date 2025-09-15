/*
  ==============================================================================

    MapOscillator.cpp
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "MapOscillator.h"
#include "LFO.h"

MapOscillator::MapOscillator()
{
}

MapOscillator::~MapOscillator()
{
}

void MapOscillator::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (auto* reader : readers)
        reader->prepareToPlay (sampleRate);

    readerBuffer.setSize (2, samplesPerBlock);
}

void MapOscillator::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    buffer.clear();

    if (readers.isEmpty())
        return;

    auto image = imageBuffer.getImage();
    if (! image.isValid())
        return;

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    if (readers.size() == 1)
    {
        readers[0]->processBlock (image, buffer, 0, numSamples);
    }
    else
    {
        readerBuffer.setSize (numChannels, numSamples, false, true, false);

        for (auto* reader : readers)
        {
            reader->processBlock (image, readerBuffer, 0, numSamples);

            for (int channel = 0; channel < numChannels; ++channel)
                buffer.addFrom (channel, 0, readerBuffer, channel, 0, numSamples);
        }
    }
}

LineReader* MapOscillator::addLineReader()
{
    // In a real application, you might want to notify listeners that a reader was added.
    auto* newReader = new LineReader();
    if (lfo != nullptr)
        newReader->setLFO (lfo);
    readers.add (newReader);
    return newReader;
}

CircleReader* MapOscillator::addCircleReader()
{
    // In a real application, you might want to notify listeners that a reader was added.
    auto* newReader = new CircleReader();
    if (lfo != nullptr)
        newReader->setLFO (lfo);
    readers.add (newReader);
    return newReader;
}

void MapOscillator::removeReader (int index)
{
    readers.remove (index);
}

int MapOscillator::getNumReaders() const { return readers.size(); }
ReaderBase* MapOscillator::getReader (int index) { return readers[index]; }

void MapOscillator::setLFO (LFO* lfoToUse)
{
    lfo = lfoToUse;
    for (auto* reader : readers)
        reader->setLFO (lfo);
}
