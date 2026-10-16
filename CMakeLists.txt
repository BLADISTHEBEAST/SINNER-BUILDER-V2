cmake_minimum_required(VERSION 3.22)
project(SINNER_BUILDER VERSION 2.0.0)

# Download JUCE from https://juce.com/get-juce/ and place its folder beside this file,
# named JUCE, or set JUCE_DIR to the JUCE folder when configuring CMake.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE)
elseif (DEFINED JUCE_DIR AND EXISTS "${JUCE_DIR}/CMakeLists.txt")
    add_subdirectory("${JUCE_DIR}" JUCE)
else()
    message(FATAL_ERROR "JUCE not found. Put the JUCE folder beside this CMakeLists.txt, or configure with -DJUCE_DIR=path/to/JUCE")
endif()

juce_add_plugin(SINNER_BUILDER
    COMPANY_NAME "BEAST INTEL"
    BUNDLE_ID "com.beastintel.sinnerbuilder"
    PLUGIN_MANUFACTURER_CODE BxIn
    PLUGIN_CODE SnB2
    PRODUCT_NAME "SINNER BUILDER"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES Instrument Synth
)

target_sources(SINNER_BUILDER PRIVATE
    PluginProcessor.cpp
    PluginProcessor.h
    PluginEditor.cpp
    PluginEditor.h
)

target_compile_features(SINNER_BUILDER PRIVATE cxx_std_17)
target_compile_definitions(SINNER_BUILDER PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(SINNER_BUILDER PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
