/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MapSynthAudioProcessor::MapSynthAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    mapOscillator.getImageBuffer().setImage (juce::ImageCache::getFromMemory (BinaryData::world_png, BinaryData::world_pngSize));
    mapOscillator.addLineReader();
    mapOscillator.addCircleReader();
}

MapSynthAudioProcessor::~MapSynthAudioProcessor()
{
}

//==============================================================================
const juce::String MapSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MapSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MapSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MapSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MapSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MapSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MapSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MapSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MapSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void MapSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MapSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mapOscillator.prepareToPlay (sampleRate, samplesPerBlock);
    lfo.prepareToPlay (sampleRate);
    mapOscillator.setLFO (&lfo);
}

void MapSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MapSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MapSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    lfo.setFrequency (apvts.getRawParameterValue ("LFOFreq")->load());    

    for (auto* readerBase : mapOscillator.getReaders())
    {
        if (auto* lineReader = dynamic_cast<LineReader*> (readerBase))
        {
            lineReader->setCentre (apvts.getRawParameterValue ("LineCX")->load(),
                                   apvts.getRawParameterValue ("LineCY")->load());
            lineReader->setLength (apvts.getRawParameterValue ("LineLength")->load());
            lineReader->setAngle (apvts.getRawParameterValue ("LineAngle")->load());
            lineReader->setFrequency (apvts.getRawParameterValue ("Frequency")->load());
            lineReader->setVolume (apvts.getRawParameterValue ("LineVolume")->load());

            lineReader->lfoAngleAmount = apvts.getRawParameterValue ("LFO_LineAngle_Amount")->load();
            lineReader->lfoLengthAmount = apvts.getRawParameterValue ("LFO_LineLength_Amount")->load();
        }
        else if (auto* circleReader = dynamic_cast<CircleReader*> (readerBase))
        {
            circleReader->setCentre (apvts.getRawParameterValue ("CX")->load(),
                                     apvts.getRawParameterValue ("CY")->load());
            circleReader->setRadius (apvts.getRawParameterValue ("R")->load());
            circleReader->setFrequency (apvts.getRawParameterValue ("Frequency")->load());
            circleReader->setVolume (apvts.getRawParameterValue ("CircleVolume")->load());

            circleReader->lfoCxAmount = apvts.getRawParameterValue ("LFO_CX_Amount")->load();
            circleReader->lfoRadiusAmount = apvts.getRawParameterValue ("LFO_R_Amount")->load();
        }
    }

    // The mapOscillator.processBlock will clear the buffer, so no need to do it here.
    mapOscillator.processBlock (buffer, midiMessages);
}

//==============================================================================
bool MapSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MapSynthAudioProcessor::createEditor()
{
    return new MapSynthAudioProcessorEditor (*this);
}

//==============================================================================
void MapSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MapSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MapSynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout MapSynthAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
        
    layout.add(std::make_unique<juce::AudioParameterFloat>("Level","Level",juce::NormalisableRange<float>(-60.f,12.f,1e-2f,1.f),0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineCX", "LineCX", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineCY", "LineCY", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineLength", "LineLength", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.6f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineAngle", "LineAngle", juce::NormalisableRange<float>(0.f, juce::MathConstants<float>::twoPi, .01f, 1.f), 0.0f));    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Frequency","Frequency",juce::NormalisableRange<float>(20.f,20000.f,1.f,1.f),440.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CX", "CX", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CY", "CY", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("R", "R", juce::NormalisableRange<float>(0.f, 0.5f, .01f, 1.f), 0.25f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineVolume", "LineVolume", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CircleVolume", "CircleVolume", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFOFreq", "LFO Freq", juce::NormalisableRange<float>(0.01f, 200.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_LineAngle_Amount", "LFO->Angle", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_LineLength_Amount", "LFO->Length", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_CX_Amount", "LFO->CX", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_R_Amount", "LFO->Radius", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));

    return layout;
}
