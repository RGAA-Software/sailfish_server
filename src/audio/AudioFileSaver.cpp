#include "AudioFileSaver.h"

#include "rgaa_common/RFile.h"

#include <string>
#include <iostream>

namespace rgaa 
{

	WAVAudioFileSaver::WAVAudioFileSaver(const std::wstring& path) {
        // Create file
        MMIOINFO mi = { 0 };
        hFile = mmioOpenW(
            // some flags cause mmioOpen write to this buffer
            // but not any that we're using
            (LPWSTR)path.c_str(),
            &mi,
            MMIO_WRITE | MMIO_CREATE
        );

        if (NULL == hFile) {
            wprintf(L"mmioOpen(\"%ls\", ...) failed. wErrorRet == %u", path.c_str(), GetLastError());
        }

        //file = File::OpenForWriteB("ttttt.pcm");
	}

	WAVAudioFileSaver::~WAVAudioFileSaver() {
        Close();
        CoUninitialize();
	}

	int WAVAudioFileSaver::WriteData(char* data, uint32_t data_length) {
        if (!hFile) {
            return -1;
        }
        if (data == nullptr || data_length <= 0) {
            return 0;
        }

        LONG bytes_written = mmioWrite(hFile, const_cast<PCHAR>(data), data_length);
        if (bytes_written != data_length) {
            printf("Save WAV miss data.");
        }
        return 0;
	}

	void WAVAudioFileSaver::Close() {
        mmioClose(hFile, 0);
	}

    HRESULT WAVAudioFileSaver::WriteWaveHeader(LPCWAVEFORMATEX pwfx, MMCKINFO* pckRIFF, MMCKINFO* pckData) {
#if 1
        MMRESULT result;

        // make a RIFF/WAVE chunk
        pckRIFF->ckid = MAKEFOURCC('R', 'I', 'F', 'F');
        pckRIFF->fccType = MAKEFOURCC('W', 'A', 'V', 'E');

        result = mmioCreateChunk(hFile, pckRIFF, MMIO_CREATERIFF);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioCreateChunk(\"RIFF/WAVE\") failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // make a 'fmt ' chunk (within the RIFF/WAVE chunk)
        MMCKINFO chunk;
        chunk.ckid = MAKEFOURCC('f', 'm', 't', ' ');
        result = mmioCreateChunk(hFile, &chunk, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioCreateChunk(\"fmt \") failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // write the WAVEFORMATEX data to it
        LONG lBytesInWfx = sizeof(WAVEFORMATEX) + pwfx->cbSize;
        LONG lBytesWritten =
            mmioWrite(
                hFile,
                reinterpret_cast<PCHAR>(const_cast<LPWAVEFORMATEX>(pwfx)),
                lBytesInWfx
            );
        if (lBytesWritten != lBytesInWfx) {
            wprintf(L"mmioWrite(fmt data) wrote %u bytes; expected %u bytes", lBytesWritten, lBytesInWfx);
            return E_FAIL;
        }

        // ascend from the 'fmt ' chunk
        result = mmioAscend(hFile, &chunk, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioAscend(\"fmt \" failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // make a 'fact' chunk whose data is (DWORD)0
        chunk.ckid = MAKEFOURCC('f', 'a', 'c', 't');
        result = mmioCreateChunk(hFile, &chunk, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioCreateChunk(\"fmt \") failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // write (DWORD)0 to it
        // this is cleaned up later
        DWORD frames = 0;
        lBytesWritten = mmioWrite(hFile, reinterpret_cast<PCHAR>(&frames), sizeof(frames));
        if (lBytesWritten != sizeof(frames)) {
            wprintf(L"mmioWrite(fact data) wrote %u bytes; expected %u bytes", lBytesWritten, (UINT32)sizeof(frames));
            return E_FAIL;
        }

        // ascend from the 'fact' chunk
        result = mmioAscend(hFile, &chunk, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioAscend(\"fact\" failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // make a 'data' chunk and leave the data pointer there
        pckData->ckid = MAKEFOURCC('d', 'a', 't', 'a');
        result = mmioCreateChunk(hFile, pckData, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioCreateChunk(\"data\") failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }
#endif
        return S_OK;
    }

    HRESULT WAVAudioFileSaver::FinishWaveFile(MMCKINFO* pckRIFF, MMCKINFO* pckData) {
#if 1
        MMRESULT result;

        result = mmioAscend(hFile, pckData, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioAscend(\"data\" failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        result = mmioAscend(hFile, pckRIFF, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioAscend(\"RIFF/WAVE\" failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }
#endif 
        return S_OK;
    }

}