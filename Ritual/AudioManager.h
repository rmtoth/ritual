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
	void PlaySound(std::string aPath, bool loop = false);
	void Update();
private:
	struct SPlayingSound
	{
		HSTREAM mStream;
		bool mLoop;
	};
	std::vector<SPlayingSound> myStreams;
	std::vector<HSAMPLE> myPlayingSounds;
};

