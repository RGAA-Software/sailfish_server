#### We use WASAPI to capture audio
Please see src/audio/WASAPIAudioCapture.h and src/audio/WASAPIAudioCapture.cpp  
Reference to [MSDN -> Capturing a Stream](https://learn.microsoft.com/en-us/windows/win32/coreaudio/capturing-a-stream)  
There is one thing to mention: the WAVEFORMATEX should be set to S16LE, please see WASAPIAudioCapture.cpp

#### The meaning of params:
```c++

int WASAPIAudioCapture::StartRecording() {
    ....
    WAVEFORMATEX waveFormat;
    // PCM format
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    // 2 channels
    waveFormat.nChannels = 2;
    // 16bit 
    waveFormat.wBitsPerSample = 16;
    // nChannels * wBitsPerSample / 8
    waveFormat.nBlockAlign = 4;
    // sample rate
    waveFormat.nSamplesPerSec = 48000;
    // nSamplesPerSec * nBlockAlign
    waveFormat.nAvgBytesPerSec = 192000;
    // ignore
    waveFormat.cbSize = 0;
    ....
}

```

#### Encode(by opus), see the wrapper at src/encoder/AudioEncoder.cpp

```c++
std::vector<std::shared_ptr<Data>> AudioEncoder::Encode(const std::shared_ptr<Data>& data, int frame_size) {
    std::vector<DataPtr> frames;
    std::vector<opus_int16> audio_data(frame_size * this->channels);
    
    memcpy((char*)audio_data.data(), data->DataAddr(), data->Size());
    auto encoded_frame = encoder->Encode(audio_data, frame_size);
    
    for (auto& piece : encoded_frame) {
        frames.push_back(Data::Make((char*)piece.data(), (int)piece.size()));
    }

    return frames;
}
```

