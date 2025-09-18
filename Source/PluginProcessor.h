/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MapOscillator.h"
#include "LFO.h"
#include "SynthSound.h"

// Let's define the number of voices for our synth
#define NUM_VOICES 4

//==============================================================================
/**
*/
class MapSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MapSynthAudioProcessor();
    ~MapSynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    ImageBuffer imageBuffer;
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameters()};
    LFO lfo;
    LFO lfo2;
    juce::AudioBuffer<float> lfoBuffer;

private:
    juce::Synthesiser synth;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();  


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapSynthAudioProcessor)
};
