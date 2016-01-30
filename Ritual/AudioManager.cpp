#include "AudioManager.h"
#include <Windows.h>

#define MUS_PATH L"assets/sound/music.mp3"

CAudioManager::CAudioManager()
{
}


CAudioManager::~CAudioManager()
{
}

int samc = 0;


int CAudioManager::Init()
{
	int device = -1; // Default Sounddevice
	int freq = 44100; // Sample rate (Hz)
	BASS_Init(device, freq, 0, 0, NULL);
	/* Load your soundfile and play it */

	return 0;
}

void CAudioManager::Update()
{
	for (int i = 0; i < myStreams.size(); i++)
	{
		HSTREAM& stream = myStreams[i];
		QWORD filePos =  BASS_StreamGetFilePosition(stream, BASS_FILEPOS_CURRENT);
		QWORD totalPos = BASS_StreamGetFilePosition(stream, BASS_FILEPOS_END);
		if (filePos >= totalPos)
		{
			BASS_StreamFree(stream);
			myStreams.erase(myStreams.begin() + i);
			break;
		}
	//	stram.
	}
}

void CAudioManager::PlaySound(std::string aPath)
{
	HSTREAM streamHandle; // Handle for open stream
	streamHandle = BASS_StreamCreateFile(FALSE, aPath.c_str(), 0, 0, 0);
	BASS_ChannelPlay(streamHandle, FALSE);
	myStreams.push_back(streamHandle);
}