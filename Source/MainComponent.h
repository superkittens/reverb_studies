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
    Slider reverbDamp;
    
    AudioFormatManager formatManager;
    String chosenPath;
    
    TextButton openButton;
    TextButton clearButton;
    
    ReferenceCountedArray<ReferenceCountedBuffer> buffers;
    ReferenceCountedBuffer::Ptr currentBuffer;
    
//    LBCF<float> lbLeft[8];
//    LBCF<float> lbRight[8];
//
//    LBCF<float> apLeft[4];
//    LBCF<float> apRight[4];

    APCF<float> apL[3];
    APCF<float> apR[3];
    FBCF<float> fbL[4];
    FBCF<float> fbR[4];
    
    size_t apN[3] = {347, 113, 37};
    float apGain = 0.7;
    size_t fbN[4] = {1687, 1601, 2053, 2251};
    float fbAm[4] = {-0.773, -0.802, -0.753, -0.733};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
