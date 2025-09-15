/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MapSynthAudioProcessorEditor::MapSynthAudioProcessorEditor (MapSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), mapDisplayComponent (p.mapOscillator),
      lineReaderComponent(p), circleReaderComponent(p)
{
    addAndMakeVisible(mapDisplayComponent);
    addAndMakeVisible(lineReaderComponent);
    addAndMakeVisible(circleReaderComponent);
    
    addAndMakeVisible(globalControlsGroup);
    globalControlsGroup.setText("Global / LFO");

    addAndMakeVisible (frequencyKnob.slider);
    frequencyKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

    addAndMakeVisible (lfoFreqKnob.slider);
    lfoFreqKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

    addAndMakeVisible (lfo2FreqKnob.slider);
    lfo2FreqKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

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

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MapSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto controlsHeight = juce::jmin (180, bounds.getHeight() / 2);

    auto displayArea = bounds.removeFromTop (bounds.getHeight() - controlsHeight);
    int squareSize = juce::jmin (displayArea.getWidth(), displayArea.getHeight());
    auto squareBounds = juce::Rectangle<int> (squareSize, squareSize).withCentre (displayArea.getCentre());
    mapDisplayComponent.setBounds (squareBounds);

    auto controlsBounds = bounds;

    juce::FlexBox controlsFb;
    controlsFb.flexDirection = juce::FlexBox::Direction::row;
    controlsFb.items.add (juce::FlexItem (lineReaderComponent).withFlex (2.5f).withMargin(5.0f));
    controlsFb.items.add (juce::FlexItem (circleReaderComponent).withFlex (2.0f).withMargin(5.0f));
    
    controlsFb.items.add (juce::FlexItem (globalControlsGroup).withFlex (1.5f).withMargin(5.0f));
    
    controlsFb.performLayout (controlsBounds);

    auto globalContentBounds = globalControlsGroup.getBounds().reduced(5).withTrimmedTop(15);

    juce::FlexBox globalFb;
    globalFb.flexDirection = juce::FlexBox::Direction::column;

    juce::FlexBox knobBox;
    knobBox.flexDirection = juce::FlexBox::Direction::row;
    knobBox.items.add (juce::FlexItem (frequencyKnob.flex()).withFlex (1.0));
    knobBox.items.add (juce::FlexItem (lfoFreqKnob.flex()).withFlex (1.0));
    knobBox.items.add (juce::FlexItem (lfo2FreqKnob.flex()).withFlex (1.0));

    globalFb.items.add (juce::FlexItem(knobBox).withFlex(1.0));

    globalFb.performLayout(globalContentBounds);
}
