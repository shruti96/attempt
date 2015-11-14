

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

class MainContentComponent : public AudioAppComponent, public ChangeListener,
	public Button::Listener
{
public:
    //==============================================================================
	MainContentComponent() : state(Stopped)
    {

		addAndMakeVisible(&openButton);
		openButton.setButtonText("Open...");
		openButton.addListener(this);

		addAndMakeVisible(&playButton);
		playButton.setButtonText("Play");
		playButton.addListener(this);
		playButton.setColour(TextButton::buttonColourId, Colours::green);
		playButton.setEnabled(false);

		addAndMakeVisible(&stopButton);
		stopButton.setButtonText("Stop");
		stopButton.addListener(this);
		stopButton.setColour(TextButton::buttonColourId, Colours::red);
		stopButton.setEnabled(false);

        setSize (500, 400);
		formatManager.registerBasicFormats();     
		transportSource.addChangeListener(this);  
        // specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    //=======================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
		transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {

		if (readerSource == nullptr)
		{
			bufferToFill.clearActiveBufferRegion();
			return;
		}

		transportSource.getNextAudioBlock(bufferToFill);
       
    }

    void releaseResources() override
    {
		transportSource.releaseResources();
        
    }

    //=======================================================================
    void paint (Graphics& g) override
    {
		
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::hotpink);

		String text = "Welcome \n\nPlay your audio ";
	
		g.setFont(Font(22.0f));
		g.setColour(Colours::white);

		g.drawMultiLineText(text,100, 50, 800);
	//	g.drawText(text, 10, 10, 300, 10, Justification::centred);
		g.fillRect(10, 100, getWidth()-20, 20);
        // You can add your drawing code here!



		
    }
	

	void resized() override
	{
		openButton.setBounds(10, 300, getWidth() - 20, 20);
		playButton.setBounds(10, 330, getWidth() - 20, 20);
		stopButton.setBounds(10, 360, getWidth() - 20, 20);
	}

	void changeListenerCallback(ChangeBroadcaster* source) override
	{
		if (source == &transportSource)
		{
			if (transportSource.isPlaying())
				changeState(Playing);
			else
				changeState(Stopped);
		}
	}

	void buttonClicked(Button* button) override
	{
		if (button == &openButton)  openButtonClicked();
		if (button == &playButton)  playButtonClicked();
		if (button == &stopButton)  stopButtonClicked();
	}

private:

	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
		Stopping
	};

	void changeState(TransportState newState)
	{
		if (state != newState)
		{
			state = newState;

			switch (state)
			{
			case Stopped:                           // [3]
				stopButton.setEnabled(false);
				playButton.setEnabled(true);
				transportSource.setPosition(0.0);
				break;

			case Starting:                          // [4]
				playButton.setEnabled(false);
				transportSource.start();
				break;

			case Playing:                           // [5]
				stopButton.setEnabled(true);
				break;

			case Stopping:                          // [6]
				transportSource.stop();
				break;
			}
		}
	}

	void openButtonClicked()
	{
		FileChooser chooser("Select a Wave file to play...",
			File::nonexistent,
			"*.wav");                                        // [7]

		if (chooser.browseForFileToOpen())                                    // [8]
		{
			File file(chooser.getResult());                                  // [9]
			AudioFormatReader* reader = formatManager.createReaderFor(file); // [10]

			if (reader != nullptr)
			{
				ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true); // [11]
				transportSource.setSource(newSource, 0, nullptr, reader->sampleRate);                         // [12]
				playButton.setEnabled(true);                                                                  // [13]
				readerSource = newSource.release();                                                            // [14]
			}
		}
	}

	void playButtonClicked()
	{
		changeState(Starting);
	}

	void stopButtonClicked()
	{
		changeState(Stopping);
	}

	//==========================================================================
	TextButton openButton;
	TextButton playButton;
	TextButton stopButton;

	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;
	TransportState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
