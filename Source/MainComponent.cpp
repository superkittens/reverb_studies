/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() :    Thread("Background Thread")
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
    formatManager.registerBasicFormats();
    
    addAndMakeVisible(openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this]{ openButtonClicked(); };
    
    addAndMakeVisible(clearButton);
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this]{ clearButtonClicked(); };
    
    startThread();
    
    roomSizeSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    roomSizeSlider.setRange(0.01, 0.98);
    roomSizeSlider.setValue(0.5);
    roomSizeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    addAndMakeVisible(roomSizeSlider);
    
    for (auto i = 0; i < 3; ++i)
    {
        apL[i].setDelayLength(apN[i]);
        apR[i].setDelayLength(apN[i]);
        apL[i].setGain(apGain);
        apR[i].setGain(apGain);
    }
    
    for (auto i = 0; i < 4; ++i)
    {
        fbL[i].setDelayLength(fbN[i]);
        fbR[i].setDelayLength(fbN[i]);
        fbL[i].am = fbAm[i];
        fbR[i].am = fbAm[i];
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    stopThread(4000);
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    ReferenceCountedBuffer::Ptr retainedCurrentBuffer(currentBuffer);
    
    if (retainedCurrentBuffer == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    auto *currentAudioSampleBuffer = retainedCurrentBuffer->getAudioSampleBuffer();
    auto position = retainedCurrentBuffer->position;
    
    auto outputSamplesRemaining = bufferToFill.numSamples;
    auto outputSamplesOffset = 0;
    
    while (outputSamplesRemaining > 0)
    {
        auto bufferSamplesRemaining = currentAudioSampleBuffer->getNumSamples() - position;
        auto samplesThisTime = jmin(bufferSamplesRemaining, outputSamplesRemaining);
        
        for (auto i = 0; i < samplesThisTime; ++i)
        {
            float left = currentAudioSampleBuffer->getSample(0, position);
            float right = currentAudioSampleBuffer->getSample(1, position);
            
            for (auto i = 0; i < 3; ++i)
            {
                left = apL[i].tick(left);
                right = apR[i].tick(right);
            }
            
            float outLeft = 0;
            float outRight = 0;
            
            for (auto i = 0; i < 4; ++i)
            {
                outLeft += fbL[i].tick(left);
                outRight += fbR[i].tick(right);
            }

            *(bufferToFill.buffer->getWritePointer(0) + outputSamplesOffset) = outLeft;
            *(bufferToFill.buffer->getWritePointer(1) + outputSamplesOffset) = outRight;

            outputSamplesRemaining -= 1;
            outputSamplesOffset += 1;
            position += 1;
        }
        
        if (position == currentAudioSampleBuffer->getNumSamples())
            position = 0;
    }
    
    retainedCurrentBuffer->position = position;
    
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    roomSizeSlider.setBounds(40, 40, 150, 150);
    openButton.setBounds(10, 10, 50, 20);
    clearButton.setBounds(10, 40, 50, 20);
}


void MainComponent::run()
{
    while (!threadShouldExit())
    {
        checkForPathToOpen();
        checkForBuffersToFree();
        wait(500);
    }
}


void MainComponent::checkForBuffersToFree()
{
    for (auto i = buffers.size(); --i >= 0; )
    {
        ReferenceCountedBuffer::Ptr buffer(buffers.getUnchecked(i));
        if (buffer->getReferenceCount() == 2)
            buffers.remove(i);
    }
}


void MainComponent::openButtonClicked()
{
    FileChooser chooser("Select File to Play", {}, "*.wav;*.aif");
    
    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        auto path = file.getFullPathName();
        chosenPath.swapWith(path);
        notify();
    }
}


void MainComponent::clearButtonClicked()
{
    currentBuffer = nullptr;
}


void MainComponent::checkForPathToOpen()
{
    String pathToOpen;
    pathToOpen.swapWith(chosenPath);
    
    if (pathToOpen.isNotEmpty())
    {
        File file(pathToOpen);
        std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
        
        if (reader.get() != nullptr)
        {
            ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(), reader->numChannels, (int)reader->lengthInSamples);
            
            reader->read(newBuffer->getAudioSampleBuffer(), 0, (int)reader->lengthInSamples, 0, true, true);
            currentBuffer = newBuffer;
            buffers.add(newBuffer);
        }
    }
}
