#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
    constexpr float twoPi = juce::MathConstants<float>::twoPi;

    class SinnerSound final : public juce::SynthesiserSound
    {
    public:
        bool appliesToNote (int) override { return true; }
        bool appliesToChannel (int) override { return true; }
    };

    class SinnerVoice final : public juce::SynthesiserVoice
    {
    public:
        explicit SinnerVoice (SinnerBuilderAudioProcessor::APVTS& state)
            : params (state)
        {
            adsr.setParameters ({ 0.01f, 0.25f, 0.7f, 0.25f });
        }

        bool canPlaySound (juce::SynthesiserSound* sound) override
        {
            return dynamic_cast<SinnerSound*> (sound) != nullptr;
        }

        void startNote (int midiNote, float velocity, juce::SynthesiserSound*, int) override
        {
            phase = 0.0f;
            modPhase = 0.0f;
            noteFrequency = juce::MidiMessage::getMidiNoteInHertz (midiNote);
            level = velocity;
            adsr.setParameters ({ get ("ATTACK"), get ("DECAY"), get ("SUSTAIN"), get ("RELEASE") });
            adsr.noteOn();
        }

        void stopNote (float, bool allowTailOff) override
        {
            adsr.noteOff();
            if (! allowTailOff)
                clearCurrentNote();
        }

        void pitchWheelMoved (int) override {}
        void controllerMoved (int, int) override {}

        void renderNextBlock (juce::AudioBuffer<float>& buffer, int start, int count) override
        {
            if (! isVoiceActive())
                return;

            auto* left = buffer.getWritePointer (0);
            auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;
            const double sr = getSampleRate();
            if (sr <= 0.0)
                return;

            const int engine = juce::roundToInt (get ("ENGINE")); // 0=SOUL, 1=MACHINE, 2=BEAST
            const float tune = get ("TUNE");
            const float tone = get ("TONE");
            const float metal = get ("METAL");
            const float noiseMix = get ("NOISE");
            const float drive = 1.0f + get ("DRIVE") * 14.0f;
            const float motion = get ("MOTION");
            const float volume = get ("OUTPUT") * 0.18f;
            const float freq = noteFrequency * std::pow (2.0f, tune / 12.0f);
            const float toneCut = 0.05f + tone * 0.95f;
            const float phaseStep = twoPi * freq / static_cast<float> (sr);
            const float modStep = twoPi * (0.08f + motion * 7.0f) / static_cast<float> (sr);
            const float metalAmt = metal * 0.75f;
            const float noiseAmt = noiseMix * 0.22f;

            for (int i = 0; i < count; ++i)
            {
                const float mod = std::sin (modPhase);
                const float fundamental = std::sin (phase);
                const float harmonicA = std::sin (phase * 2.0f + mod * motion * 2.0f);
                const float harmonicB = std::sin (phase * 3.01f + 0.7f);
                const float grit = random.nextFloat() * 2.0f - 1.0f;
                float sample = 0.0f;

                if (engine == 0) // SOUL: smoother, softer, atmospheric
                {
                    const float gentleMotion = std::sin (phase + mod * (0.15f + motion * 2.0f));
                    sample = fundamental * 0.78f + gentleMotion * 0.18f
                           + harmonicA * (0.04f + metalAmt * 0.12f)
                           + grit * noiseAmt * 0.12f;
                }
                else if (engine == 1) // MACHINE: inharmonic metallic partials
                {
                    const float metallicA = std::sin (phase * 2.71f + mod * motion * 3.0f);
                    const float metallicB = std::sin (phase * 4.13f + 0.4f);
                    sample = fundamental * 0.42f + harmonicA * (0.22f + metalAmt * 0.25f)
                           + metallicA * (0.18f + metalAmt * 0.25f)
                           + metallicB * metalAmt * 0.22f + grit * noiseAmt;
                }
                else // BEAST: denser harmonics, stronger drive
                {
                    sample = fundamental * 0.50f + harmonicA * (0.28f + metalAmt * 0.20f)
                           + harmonicB * (0.18f + metalAmt * 0.25f)
                           + grit * noiseAmt * 0.75f;
                }

                sample *= toneCut;
                const float engineDrive = engine == 2 ? drive * 1.35f : drive;
                sample = std::tanh (sample * engineDrive) / std::max (0.001f, std::tanh (engineDrive));
                sample *= adsr.getNextSample() * level * volume;

                left[start + i] += sample;
                if (right != nullptr)
                    right[start + i] += sample;

                phase += phaseStep;
                modPhase += modStep;
                if (phase >= twoPi) phase -= twoPi;
                if (modPhase >= twoPi) modPhase -= twoPi;
            }

            if (! adsr.isActive())
                clearCurrentNote();
        }

    private:
        float get (const char* id) const
        {
            if (auto* value = params.getRawParameterValue (id))
                return value->load();
            return 0.0f;
        }

        SinnerBuilderAudioProcessor::APVTS& params;
        juce::ADSR adsr;
        juce::Random random;
        float phase = 0.0f, modPhase = 0.0f, noteFrequency = 440.0f, level = 0.0f;
    };
}

SinnerBuilderAudioProcessor::APVTS::ParameterLayout SinnerBuilderAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    auto addFloat = [&layout] (const char* id, const char* name, float min, float max, float initial, float step = 0.0f)
    {
        layout.add (std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 }, name,
            juce::NormalisableRange<float> (min, max, step), initial));
    };

    layout.add (std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "ENGINE", 1 }, "Engine",
        juce::StringArray { "SOUL", "MACHINE", "BEAST" }, 1));

    addFloat ("TUNE", "Tune (semitones)", -12.0f, 12.0f, 0.0f, 0.1f);
    addFloat ("TONE", "Tone", 0.0f, 1.0f, 0.8f);
    addFloat ("METAL", "Metal", 0.0f, 1.0f, 0.45f);
    addFloat ("NOISE", "Noise", 0.0f, 1.0f, 0.08f);
    addFloat ("DRIVE", "Drive", 0.0f, 1.0f, 0.25f);
    addFloat ("MOTION", "Motion", 0.0f, 1.0f, 0.2f);
    addFloat ("ATTACK", "Attack", 0.001f, 2.0f, 0.01f);
    addFloat ("DECAY", "Decay", 0.01f, 3.0f, 0.25f);
    addFloat ("SUSTAIN", "Sustain", 0.0f, 1.0f, 0.7f);
    addFloat ("RELEASE", "Release", 0.01f, 4.0f, 0.25f);
    addFloat ("OUTPUT", "Output", 0.0f, 1.0f, 0.7f);
    return layout;
}

SinnerBuilderAudioProcessor::SinnerBuilderAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    for (int i = 0; i < 8; ++i)
        synth.addVoice (new SinnerVoice (parameters));
    synth.addSound (new SinnerSound());
}

void SinnerBuilderAudioProcessor::prepareToPlay (double sampleRate, int)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void SinnerBuilderAudioProcessor::releaseResources() {}

void SinnerBuilderAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        buffer.applyGain (channel, 0, buffer.getNumSamples(), 0.8f);
}

juce::AudioProcessorEditor* SinnerBuilderAudioProcessor::createEditor()
{
    return new SinnerBuilderAudioProcessorEditor (*this);
}

void SinnerBuilderAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, dest);
}

void SinnerBuilderAudioProcessor::setStateInformation (const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, size));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SinnerBuilderAudioProcessor();
}
