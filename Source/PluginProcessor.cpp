/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthSound.h"
#include "SynthVoice.h"

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
    imageBuffer.setImage (juce::ImageCache::getFromMemory (BinaryData::world_png, BinaryData::world_pngSize));

    synth.addSound (new SynthSound());
    for (int i = 0; i < NUM_VOICES; ++i)
    {
        synth.addVoice (new SynthVoice (*this));
    }
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
    return 5.0;
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
    synth.setCurrentPlaybackSampleRate (sampleRate);

    lfoBuffer.setSize (2, samplesPerBlock);

    lfo.prepareToPlay (sampleRate);
    lfo2.prepareToPlay (sampleRate);
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
    lfo2.setFrequency (apvts.getRawParameterValue ("LFO2Freq")->load());

    // Process LFOs for the block
    auto* lfo1Data = lfoBuffer.getWritePointer (0);
    auto* lfo2Data = lfoBuffer.getWritePointer (1);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        lfo1Data[i] = lfo.process();
        lfo2Data[i] = lfo2.process();
    }

    buffer.clear();
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
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
    // Get the parameters' state from APVTS
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());

    // Get the source file for the current image
    auto imageFile = imageBuffer.getFile();

    if (imageFile.existsAsFile())
    {
        // If we have a valid file, save its path
        xml->setAttribute ("imagePath", imageFile.getFullPathName());
    }
    else
    {
        // Otherwise (e.g. for the default image), save the raw image data as a fallback
        auto image = imageBuffer.getImage();
        if (image.isValid())
        {
            juce::MemoryOutputStream memoryStream;
            juce::PNGImageFormat pngFormat;
            if (pngFormat.writeImageToStream (image, memoryStream))
            {
                xml->setAttribute ("imageData", memoryStream.getMemoryBlock().toBase64Encoding());
            }
        }
    }
    
    // Store the combined XML state in the memory block
    copyXmlToBinary (*xml, destData);
}

void MapSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore the state from XML
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName (apvts.state.getType()))
        {
            // First, try to restore from a file path
            if (xmlState->hasAttribute ("imagePath"))
            {
                auto imagePath = xmlState->getStringAttribute ("imagePath");
                if (! imageBuffer.setImage (juce::File (imagePath)))
                {
                    // If loading from path fails, do nothing, keep current/default image.
                    // You could show an error message here if you wanted.
                }
            }
            // If no path, try to restore from embedded data (for older presets or default state)
            else if (xmlState->hasAttribute ("imageData"))
            {
                auto base64Data = xmlState->getStringAttribute ("imageData");
                juce::MemoryBlock imageData;
                if (imageData.fromBase64Encoding (base64Data))
                {
                    juce::Image loadedImage = juce::ImageFileFormat::loadFrom (imageData.getData(), imageData.getSize());
                    imageBuffer.setImage (loadedImage);
                }
            }

            // Then, restore the parameters using the APVTS
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
    }
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
    layout.add(std::make_unique<juce::AudioParameterFloat>("CX", "CX", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CY", "CY", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("R", "R", juce::NormalisableRange<float>(0.f, 0.5f, .01f, 1.f), 0.25f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineVolume", "LineVolume", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CircleVolume", "CircleVolume", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFOFreq", "LFO 1 Freq", juce::NormalisableRange<float>(0.01f, 200.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO2Freq", "LFO 2 Freq", juce::NormalisableRange<float>(0.01f, 200.0f, 0.01f, 0.3f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Attack", "Attack", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Decay", "Decay", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Sustain", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Release", "Release", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.4f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_LineCX_Amount", "LFO->LineCX", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO_LineCX_Select", "LFO Select", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_LineCY_Amount", "LFO->LineCY", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO_LineCY_Select", "LFO Select", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_LineAngle_Amount", "LFO->Angle", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO_LineAngle_Select", "LFO Select", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_LineLength_Amount", "LFO->Length", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO_LineLength_Select", "LFO Select", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_CX_Amount", "LFO->CircleCX", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO_CX_Select", "LFO Select", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_CY_Amount", "LFO->CircleCY", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO_CY_Select", "LFO Select", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO_R_Amount", "LFO->Radius", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO_R_Select", "LFO Select", false));

    return layout;
}
