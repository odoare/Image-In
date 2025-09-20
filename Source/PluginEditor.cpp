/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MapSynthAudioProcessorEditor::MapSynthAudioProcessorEditor (MapSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), 
      audioProcessor (p), 
      mapDisplayComponent (p),
      lineReaderComponent(p), 
      circleReaderComponent(p),
      lfoFreqKnob (p.apvts, "LFOFreq", juce::Colours::hotpink),
      lfo2FreqKnob (p.apvts, "LFO2Freq", juce::Colours::hotpink),
      attackKnob (p.apvts, "Attack", juce::Colours::limegreen),
      decayKnob (p.apvts, "Decay", juce::Colours::limegreen),
      sustainKnob (p.apvts, "Sustain", juce::Colours::limegreen),
      releaseKnob (p.apvts, "Release", juce::Colours::limegreen)
      ,
      attack2Knob (p.apvts, "Attack2", juce::Colours::cyan),
      decay2Knob (p.apvts, "Decay2", juce::Colours::cyan),
      sustain2Knob (p.apvts, "Sustain2", juce::Colours::cyan),
      release2Knob (p.apvts, "Release2", juce::Colours::cyan)
{
    addAndMakeVisible(mapDisplayComponent);
    addAndMakeVisible(lineReaderComponent);
    addAndMakeVisible(circleReaderComponent);
    
    addAndMakeVisible(globalControlsGroup);
    globalControlsGroup.setText("Global / LFO");

    addAndMakeVisible (lfoFreqKnob.slider);
    lfoFreqKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

    addAndMakeVisible (lfo2FreqKnob.slider);
    lfo2FreqKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

    addAndMakeVisible (attackKnob.slider);
    attackKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
    addAndMakeVisible (decayKnob.slider);
    decayKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
    addAndMakeVisible (sustainKnob.slider);
    sustainKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
    addAndMakeVisible (releaseKnob.slider);
    releaseKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

    addAndMakeVisible (attack2Knob.slider);
    attack2Knob.slider.setLookAndFeel (&fxmeLookAndFeel);
    addAndMakeVisible (decay2Knob.slider);
    decay2Knob.slider.setLookAndFeel (&fxmeLookAndFeel);
    addAndMakeVisible (sustain2Knob.slider);
    sustain2Knob.slider.setLookAndFeel (&fxmeLookAndFeel);
    addAndMakeVisible (release2Knob.slider);
    release2Knob.slider.setLookAndFeel (&fxmeLookAndFeel);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable(true, true);    
    setSize (800, 600);
}

MapSynthAudioProcessorEditor::~MapSynthAudioProcessorEditor()
{
}

//==============================================================================
void MapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).darker (0.2f));
}

void MapSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();

    juce::FlexBox mainFb;
    mainFb.flexDirection = juce::FlexBox::Direction::column;

    auto controlsHeight = juce::jmin (240, bounds.getHeight() / 2);

    juce::FlexBox controlsFb;
    controlsFb.flexDirection = juce::FlexBox::Direction::row;
    controlsFb.items.add (juce::FlexItem (lineReaderComponent).withFlex (2.5f).withMargin(5.0f));
    controlsFb.items.add (juce::FlexItem (circleReaderComponent).withFlex (2.0f).withMargin(5.0f));
    controlsFb.items.add (juce::FlexItem (globalControlsGroup).withFlex (1.5f).withMargin(5.0f));

    mainFb.items.add (juce::FlexItem (mapDisplayComponent).withFlex (1.0f).withMargin (5.0f));
    mainFb.items.add (juce::FlexItem (controlsFb).withHeight (controlsHeight));

    mainFb.performLayout (bounds);

    auto globalContentBounds = globalControlsGroup.getBounds().reduced(5).withTrimmedTop(15);

    juce::FlexBox globalFb;
    globalFb.flexDirection = juce::FlexBox::Direction::column;

    juce::FlexBox knobBox;
    knobBox.flexDirection = juce::FlexBox::Direction::row;
    knobBox.items.add (juce::FlexItem (lfoFreqKnob.flex()).withFlex (1.0));
    knobBox.items.add (juce::FlexItem (lfo2FreqKnob.flex()).withFlex (1.0));

    juce::FlexBox adsrBox;
    adsrBox.flexDirection = juce::FlexBox::Direction::row;
    adsrBox.items.add (juce::FlexItem (attackKnob.flex()).withFlex (1.0));
    adsrBox.items.add (juce::FlexItem (decayKnob.flex()).withFlex (1.0));
    adsrBox.items.add (juce::FlexItem (sustainKnob.flex()).withFlex (1.0));
    adsrBox.items.add (juce::FlexItem (releaseKnob.flex()).withFlex (1.0));

    juce::FlexBox adsr2Box;
    adsr2Box.flexDirection = juce::FlexBox::Direction::row;
    adsr2Box.items.add (juce::FlexItem (attack2Knob.flex()).withFlex (1.0));
    adsr2Box.items.add (juce::FlexItem (decay2Knob.flex()).withFlex (1.0));
    adsr2Box.items.add (juce::FlexItem (sustain2Knob.flex()).withFlex (1.0));
    adsr2Box.items.add (juce::FlexItem (release2Knob.flex()).withFlex (1.0));

    globalFb.items.add (juce::FlexItem(knobBox).withFlex(1.0));
    globalFb.items.add (juce::FlexItem(adsrBox).withFlex(1.0));
    globalFb.items.add (juce::FlexItem(adsr2Box).withFlex(1.0));

    globalFb.performLayout(globalContentBounds);
}
