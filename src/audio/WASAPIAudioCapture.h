#pragma once

#include "AudioCapture.h"

#include <Windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <ctime>
#include <mmeapi.h>
#include <iostream>
#include <mutex>
#include <condition_variable>

namespace rgaa 
{

	class WASAPIAudioCapture : public IAudioCapture {
	public:

		static AudioCapturePtr Create();

		WASAPIAudioCapture();
		~WASAPIAudioCapture();

		int Prepare() override;
		int StartRecording() override;
		int Pause() override;
		int Stop() override;
	
	private:
        bool released = false;
		BOOL bDone = FALSE;
	};

}