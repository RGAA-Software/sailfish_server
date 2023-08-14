#pragma once

#include <string>
#include <memory>
#include <Windows.h>
#include <mmeapi.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

namespace rgaa 
{

	class IAudioFileSaver {
	public:

		virtual int WriteData(char* data, uint32_t data_length) = 0;
		virtual void Close() = 0;

	};

	typedef std::shared_ptr<IAudioFileSaver> AudioFileSaverPtr;


	class WAVAudioFileSaver : public IAudioFileSaver {
	public:
		WAVAudioFileSaver(const std::wstring& path);
		~WAVAudioFileSaver();

		int WriteData(char* data, uint32_t data_length) override;
		void Close() override;

		HRESULT WriteWaveHeader(LPCWAVEFORMATEX pwfx, MMCKINFO* pckRIFF, MMCKINFO* pckData);
		HRESULT FinishWaveFile(MMCKINFO* pckRIFF, MMCKINFO* pckData);

	private:

		HMMIO hFile = NULL;

	};

	typedef std::shared_ptr<WAVAudioFileSaver> WAVAudioFileSaverPtr;

}