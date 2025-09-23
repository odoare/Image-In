/*
  ==============================================================================

    MapOscillator.cpp
    Created: 12 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "MapOscillator.h"
#include "LFO.h"
#include "ParameterStructs.h"

MapOscillator::MapOscillator()
{
}

MapOscillator::~MapOscillator()
{
}

void MapOscillator::prepareToPlay (double sampleRate)
{
    currentSampleRate = sampleRate;
    for (auto* reader : readers)
        reader->prepareToPlay (sampleRate);
}

void MapOscillator::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/, int startSample, int numSamples, ImageBuffer& imageBuffer, const juce::AudioBuffer<float>& modulatorBuffer)
{
    if (readers.isEmpty())
        return;

    auto image = imageBuffer.getImage();
    if (! image.isValid())
        return;

    auto numChannels = buffer.getNumChannels();

    // If there's only one reader, we can process it directly.
    // Since the reader now uses addSample, this will correctly add its output.
    if (readers.size() == 1)
    {
        readers[0]->processBlock (image, buffer, startSample, numSamples, modulatorBuffer);
    }
    else
    {
        // For multiple readers, we use an intermediate buffer to sum their outputs.
        readerBuffer.setSize (numChannels, numSamples, false, true, true);
        readerBuffer.clear();

        // Each reader adds its output to the intermediate buffer.
        for (auto* reader : readers)
            reader->processBlock (image, readerBuffer, 0, numSamples, modulatorBuffer);

        // Finally, add the summed output to the main output buffer.
        for (int channel = 0; channel < numChannels; ++channel)
            buffer.addFrom (channel, startSample, readerBuffer, channel, 0, numSamples);
    }
}

void MapOscillator::rebuildReaders (const juce::Array<ReaderBase::Type>& types)
{
    readers.clear();

    for (int i = 0; i < 3; ++i)
    {
        auto* newReader = addEllipseReader();
        if (newReader != nullptr)
            newReader->prepareToPlay (currentSampleRate);
    }
}

void MapOscillator::updateParameters (const GlobalParameters& params)
{
    // Assuming the number of readers matches the number of parameter sets
    for (int i = 0; i < readers.size() && i < params.ellipses.size(); ++i)
    {
        if (auto* ellipseReader = dynamic_cast<EllipseReader*> (readers[i]))
            ellipseReader->updateParameters (params.ellipses[i]);
    }
}

EllipseReader* MapOscillator::addEllipseReader()
{
    // In a real application, you might want to notify listeners that a reader was added.
    auto* newReader = new EllipseReader();
    readers.add (newReader);
    return newReader;
}

void MapOscillator::removeReader (int index)
{
    readers.remove (index);
}

int MapOscillator::getNumReaders() const { return readers.size(); }
ReaderBase* MapOscillator::getReader (int index) { return readers[index]; }
