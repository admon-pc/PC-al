#pragma once
#ifndef _AL_AUDIOENG_H_
#define _AL_AUDIOENG_H_

#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>

class alAudioEngine
{
protected:
	alAudioBufferInfo m_audioDeviceInfo;
public:
	alAudioEngine() {}
	virtual ~alAudioEngine() {}
	alAudioBufferInfo GetDeviceInfo() { return m_audioDeviceInfo; }

	virtual bool Initialize() = 0;
	
	struct queue_data
	{
		uint32_t m_cmd = 0;
	};

	enum
	{
		// this will exit main loop and thread function will go to the end
		queueCMD_quit = 1,

		// this will deactivate working with mixers
		queueCMD_stop, 
		// this will resume
		queueCMD_resume,
	};
	virtual void AddCommand(const queue_data&) = 0;
};


#endif


