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
	void PlaySound(std::string aPath);
	void Update();
private:
	std::vector<HSTREAM> myStreams;
	std::vector<HSAMPLE> myPlayingSounds;
};

