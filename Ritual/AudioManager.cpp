#include "AudioManager.h"
#include <Windows.h>

#define MUS_PATH L"assets/sound/music.mp3"

CAudioManager::CAudioManager()
{
	myID = 0;
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
	for (auto iterator = myStreams.begin(); iterator != myStreams.end(); )
	{
		HSTREAM& stream = iterator->second.mStream;
		QWORD filePos =  BASS_StreamGetFilePosition(stream, BASS_FILEPOS_CURRENT);
		QWORD totalPos = BASS_StreamGetFilePosition(stream, BASS_FILEPOS_END);
		if (filePos >= totalPos)
		{
			if (iterator->second.mLoop == true)
			{
				BASS_ChannelPlay(stream, FALSE);
				break;
			}
			BASS_StreamFree(stream);
			myStreams.erase(stream);
		}
		else
		{
			iterator++;
		}
	}
}

void CAudioManager::SetVolume(int aID, float aVolume)
{
	std::map<int, SPlayingSound>::iterator it = myStreams.find(aID);
	if (it != myStreams.end())
	{
		SPlayingSound& sound = it->second;
		BASS_ChannelSetAttribute(sound.mStream, BASS_ATTRIB_VOL, aVolume);
	}
}

int CAudioManager::PlaySound(std::string aPath, bool loop)
{
	HSTREAM streamHandle; // Handle for open stream
	streamHandle = BASS_StreamCreateFile(FALSE, aPath.c_str(), 0, 0, 0);
	BASS_ChannelPlay(streamHandle, FALSE);
	SPlayingSound sound;
	sound.mLoop = loop;
	sound.mStream = streamHandle;
	int id = myID;
	myStreams[id] = sound;
	myID++;
	return id;
}