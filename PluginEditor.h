#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SinnerBuilderAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit SinnerBuilderAudioProcessorEditor (SinnerBuilderAudioProcessor&);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SinnerBuilderAudioProcessor& processor;
    juce::GenericAudioProcessorEditor controls;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SinnerBuilderAudioProcessorEditor)
};
