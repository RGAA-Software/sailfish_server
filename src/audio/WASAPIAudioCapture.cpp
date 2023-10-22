#include "WASAPIAudioCapture.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RTime.h"

#pragma comment(lib, "Winmm.lib")

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
                  if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
                  if ((punk) != NULL)  \
                    { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

//class MyAudioSink
//{
//public:
//	HRESULT CopyData(BYTE* pData, UINT32 NumFrames, BOOL* pDone, WAVEFORMATEX* pwfx, HMMIO hFile);
//};
//
//HRESULT WriteWaveHeader(HMMIO hFile, LPCWAVEFORMATEX pwfx, MMCKINFO* pckRIFF, MMCKINFO* pckData);
//HRESULT FinishWaveFile(HMMIO hFile, MMCKINFO* pckRIFF, MMCKINFO* pckData);
//HRESULT RecordAudioStream(MyAudioSink* pMySink);

namespace rgaa 
{

	AudioCapturePtr WASAPIAudioCapture::Create() {
		return std::make_shared<WASAPIAudioCapture>();
	}

	WASAPIAudioCapture::WASAPIAudioCapture() {

	}

	WASAPIAudioCapture::~WASAPIAudioCapture() { 
	}

	int WASAPIAudioCapture::Prepare() {
		HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		return SUCCEEDED(hr) ? 0 : -1;
	}

	int WASAPIAudioCapture::StartRecording() {
        HRESULT hr;
        REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
        REFERENCE_TIME hnsActualDuration;
        UINT32 bufferFrameCount;
        UINT32 numFramesAvailable;
        IMMDeviceEnumerator* pEnumerator = NULL;
        IMMDevice* pDevice = NULL;
        IAudioClient* pAudioClient = NULL;
        IAudioCaptureClient* pCaptureClient = NULL;
        WAVEFORMATEX* pwfx = NULL;
        UINT32 packetLength = 0;

        BYTE* pData;
        DWORD flags;

        MMCKINFO ckRIFF = { 0 };
        MMCKINFO ckData = { 0 };

        hr = CoCreateInstance(
            CLSID_MMDeviceEnumerator, NULL,
            CLSCTX_ALL, IID_IMMDeviceEnumerator,
            (void**)&pEnumerator);
        EXIT_ON_ERROR(hr)

        hr = pEnumerator->GetDefaultAudioEndpoint(
            eRender, eConsole, &pDevice);
        EXIT_ON_ERROR(hr)

        hr = pDevice->Activate(
            IID_IAudioClient, CLSCTX_ALL,
            NULL, (void**)&pAudioClient);
        EXIT_ON_ERROR(hr)

        //hr = pAudioClient->GetMixFormat(&pwfx);
        //EXIT_ON_ERROR(hr)

        WAVEFORMATEX waveFormat;
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 2;
        waveFormat.wBitsPerSample = 16;
        waveFormat.nBlockAlign = 4;
        waveFormat.nSamplesPerSec = 48000;
        waveFormat.nAvgBytesPerSec = 192000;
        waveFormat.cbSize = 0;

        if (format_callback) {
            format_callback(waveFormat.nSamplesPerSec, waveFormat.nChannels, waveFormat.wBitsPerSample);
        }

        pwfx = &waveFormat;

   
        hr = pAudioClient->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_STREAMFLAGS_LOOPBACK,
            hnsRequestedDuration,
            0,
            pwfx,
            NULL);
        EXIT_ON_ERROR(hr)

        // Get the size of the allocated buffer.
        hr = pAudioClient->GetBufferSize(&bufferFrameCount);
        EXIT_ON_ERROR(hr)

        hr = pAudioClient->GetService(
            IID_IAudioCaptureClient,
            (void**)&pCaptureClient);
        EXIT_ON_ERROR(hr)
        if (file_saver) {
            hr = std::static_pointer_cast<WAVAudioFileSaver>(file_saver)->WriteWaveHeader(pwfx, &ckRIFF, &ckData);
        }
            
        if (FAILED(hr)) {
            // WriteWaveHeader does its own logging
            return hr;
        }

        // Calculate the actual duration of the allocated buffer.
        hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

        hr = pAudioClient->Start();  // Start recording.
        EXIT_ON_ERROR(hr)

        // Each loop fills about half of the shared buffer.
        while (bDone == FALSE)
        {
            // Sleep for half the buffer duration.
            //Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);
     
            Sleep(16);

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr)

            while (packetLength != 0)
            {
                if (bDone == TRUE) {
                    std::cout << "Exit inner loop" << std::endl;
                    break;
                }

                auto begin = GetCurrentTimestamp();
                // Get the available data in the shared buffer.
                hr = pCaptureClient->GetBuffer(
                    &pData,
                    &numFramesAvailable,
                    &flags, NULL, NULL);
                EXIT_ON_ERROR(hr)

                if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
                {
                    pData = NULL;  // Tell CopyData to write silence.
                }

                LONG bytes_to_write = numFramesAvailable * pwfx->nBlockAlign;
                if (pData && bytes_to_write > 0) {
                    std::lock_guard<std::mutex> guard(exit_mtx_);
                    if (data_callback && !bDone) {
                        auto data = Data::Make((char*)pData, bytes_to_write);
                        data_callback(data);
                    }

                    if (split_data_callback && !bDone) {
                        auto left_data = Data::Make(nullptr, bytes_to_write / 2);
                        auto right_data = Data::Make(nullptr, bytes_to_write / 2);
                        //for (int i = 0; i < bytes_to_write; i += 8) {
                        //    memcpy((left_data->DataAddr() + i / 8 * 4), ((char*)pData + i), 4);
                        //    memcpy((right_data->DataAddr() + i / 8 * 4), ((char*)pData + i + 4), 4);
                        //}

                        for (int i = 0; i < bytes_to_write; i += 4) {
                            memcpy((left_data->DataAddr() + i / 4 * 2), ((char*)pData + i), 2);
                            memcpy((right_data->DataAddr() + i / 4 * 2), ((char*)pData + i + 2), 2);
                        }

                        //LOGI("l : {0:d}, r : {1:d}", left_data->Size(), right_data->Size());
                        split_data_callback(std::move(left_data), std::move(right_data));
                    }   
                }

                if (file_saver && pData && bytes_to_write > 0) {
                    file_saver->WriteData((char*)pData, bytes_to_write);
                }
                    
                EXIT_ON_ERROR(hr)

                hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
                EXIT_ON_ERROR(hr)

                hr = pCaptureClient->GetNextPacketSize(&packetLength);
                EXIT_ON_ERROR(hr)
            }
        }

        hr = pAudioClient->Stop();  // Stop recording.
        std::cout << "stopped recording .." << hr << " " << GetLastError() << std::endl;
        EXIT_ON_ERROR(hr)
        if (file_saver) {
            hr = std::static_pointer_cast<WAVAudioFileSaver>(file_saver)->FinishWaveFile(&ckData, &ckRIFF);
        }

    Exit:
        LOGI("WASAPIAudioCapture Release device .");
        SAFE_RELEASE(pCaptureClient)
        SAFE_RELEASE(pAudioClient)
        SAFE_RELEASE(pDevice)
        SAFE_RELEASE(pEnumerator)

        released = true;

        return hr;
	}

	int WASAPIAudioCapture::Pause() {
        return 0;
	}

	int WASAPIAudioCapture::Stop() {
        std::lock_guard<std::mutex> guard(exit_mtx_);
        bDone = TRUE;
        int wait_times = 0;
        while(!released && wait_times < 5) {
            wait_times++;
            LOGI("WASAPI wait exit : {}", wait_times);
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
        LOGI("WASAPI audio capture released.");
        return 0;
	}


}