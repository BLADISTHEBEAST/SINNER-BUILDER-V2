#include "PluginEditor.h"

SinnerBuilderAudioProcessorEditor::SinnerBuilderAudioProcessorEditor (SinnerBuilderAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), controls (p)
{
    addAndMakeVisible (controls);
    setSize (420, 640);
}

void SinnerBuilderAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff101311));
    g.setColour (juce::Colour (0xffb8ff36));
    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawText ("SINNER BUILDER", 16, 10, getWidth() - 32, 34, juce::Justification::centredLeft);
    g.setColour (juce::Colour (0xffa6b39d));
    g.setFont (juce::FontOptions (10.0f));
    g.drawText ("BEAST INTEL  /  SELECT ONE ENGINE  /  V2.0",
                18, 43, getWidth() - 36, 18, juce::Justification::centredLeft);
}

void SinnerBuilderAudioProcessorEditor::resized()
{
    controls.setBounds (10, 68, getWidth() - 20, getHeight() - 78);
}
