/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "DelayLine.h"
#include "CombFilters.h"
#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent, private Thread
{
    
    class ReferenceCountedBuffer : public ReferenceCountedObject
    {
    public:
        typedef ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;
        
        ReferenceCountedBuffer(const String &nameToUse, int numChannels, int numSamples) :  name(nameToUse),
        buffer(numChannels, numSamples)
        {
            DBG(String("Buffer named: ") + name + String("Num Channels: ") + String(numChannels) + String("Num Samples") + String(numSamples));
        }
        
        ~ReferenceCountedBuffer()
        {
            DBG(String("Buffer named: ") + name + String(" destroyed"));
        }
        
        AudioSampleBuffer *getAudioSampleBuffer()
        {
            return &buffer;
        }
        
        int position = 0;
        
    private:
        String name;
        AudioSampleBuffer buffer;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReferenceCountedBuffer)
    };
    
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void run() override;

private:
    //==============================================================================
    void checkForBuffersToFree();
    void checkForPathToOpen();
    void openButtonClicked();
    void clearButtonClicked();
    
    
    Slider roomSizeSlider;
    Slider reverbDampSlider;
    
    AudioFormatManager formatManager;
    String chosenPath;
    
    TextButton openButton;
    TextButton clearButton;
    
    ReferenceCountedArray<ReferenceCountedBuffer> buffers;
    ReferenceCountedBuffer::Ptr currentBuffer;
    
    LBCF<float> lb[2][8];
    APCF<float> ap[2][4];
    
    static constexpr float f = 0.84;
    static constexpr float d = 0.2;
    static constexpr float g = 0.5;
    
    size_t lbN[8] = {1557, 1617, 1491, 1422, 1277, 1356, 1188, 1116};
    size_t apN[4] = {225, 556, 441, 341};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
