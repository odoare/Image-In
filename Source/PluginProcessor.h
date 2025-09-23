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
#include "ParameterStructs.h"
#include "SynthVoice.h"

// Number of voices for the synth
#define NUM_VOICES 4

struct VoiceDisplayState
{
    bool isActive = false;
    juce::Array<ReaderBase::DrawingInfo> readerInfos;
};


//==============================================================================
/**
*/
class MapSynthAudioProcessor  : public juce::AudioProcessor,
                                public juce::AudioProcessorValueTreeState::Listener
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

    void parameterChanged (const juce::String& parameterID, float newValue) override;

    ImageBuffer imageBuffer;
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameters()};
    LFO lfo;
    LFO lfo2;
    LFO lfo3;
    LFO lfo4;
    juce::AudioBuffer<float> lfoBuffer;
    GlobalParameters globalParams; // This now contains all parameter structs
    juce::Array<ReaderBase::Type> readerTypes;

    std::array<VoiceDisplayState, NUM_VOICES> voiceDisplayStates;
    juce::CriticalSection displayStateLock;

    // OpenGL state management
    void setUseOpenGL (bool shouldUseOpenGL);
    bool getUseOpenGL() const;
    juce::ChangeBroadcaster openGLStateBroadcaster;

    void addReader (ReaderBase::Type type);
    void removeReader (int index);

private:
    // Preset Management
    int currentProgram = 0;
    bool isLoadingPreset = false;
    bool useOpenGL = false;

    juce::Synthesiser synth;
    juce::LinearSmoothedValue<float> masterLevelSmoother;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();  
    void updateParameters();
    void updateVoices();


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapSynthAudioProcessor)
};
