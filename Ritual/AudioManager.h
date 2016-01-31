#pragma once
#include <vector>
#include <map>

#include "audio/bass.h"
class CAudioManager
{
public:
	CAudioManager();
	~CAudioManager();
	int Init();
	int PlaySound(std::string aPath, bool loop = false);
	void SetVolume(int aID, float aVolume);

	void Update();
private:
	struct SPlayingSound
	{
		HSTREAM mStream;
		bool mLoop;
	};
	std::map<int, SPlayingSound> myStreams;
	std::vector<HSAMPLE> myPlayingSounds;
	int myID;
};

