#pragma once

#include "rgaa_common/RData.h"
#include "AudioFileSaver.h"

#include <memory>
#include <functional>
#include <mutex>

typedef std::function<void()> OnPrepareCallback;
typedef std::function<void(int samples, int channels, int bits)> OnFormatCallback;
typedef std::function<void(rgaa::DataPtr data)> OnDataCallback;
typedef std::function<void(rgaa::DataPtr left, rgaa::DataPtr right)> OnSplitDataCallback;
typedef std::function<void()> OnPauseCallback;
typedef std::function<void()> OnStopCallback;

namespace rgaa 
{

	class IAudioCapture {
	public:

		virtual int Prepare() = 0;
		virtual int StartRecording() = 0;
		virtual int Pause() = 0;
		virtual int Stop() = 0;

		void RegisterPrepareCallback(const OnPrepareCallback& cbk) {
			prepare_callback = cbk;
		}

		void RegisterFormatCallback(const OnFormatCallback& cbk) {
			format_callback = cbk;
		}

		void RegisterDataCallback(const OnDataCallback& cbk) {
			data_callback = cbk;
		}

		void RegisterSplitDataCallback(const OnSplitDataCallback& cbk) {
			split_data_callback = cbk;
		}

		void RegisterPauseCallback(const OnPauseCallback& cbk) {
			pause_callback = cbk;
		}

		void RegisterStopCallback(const OnStopCallback& cbk) {
			stop_callback = cbk;
		}

		void SetAudioFileSaver(AudioFileSaverPtr saver) {
			file_saver = saver;
		}

	protected:

		OnPrepareCallback prepare_callback{ nullptr };
		OnFormatCallback format_callback{ nullptr };
		OnDataCallback data_callback{ nullptr };
		OnSplitDataCallback split_data_callback{ nullptr };
		OnPauseCallback pause_callback{ nullptr };
		OnStopCallback stop_callback{ nullptr };

		AudioFileSaverPtr file_saver{ nullptr };

        std::mutex exit_mtx_;

	};

	typedef std::shared_ptr<IAudioCapture> AudioCapturePtr;
}