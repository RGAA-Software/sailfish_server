#### Client's function, see [This repository](https://github.com/RGAA-Software/sailfish_client_pc)

#### Please see src/sdk/FFmpegVideoDecoder.cpp, there is one thing to mention: 
- Selecting proper parameters to decode quickly

```c++
int FFmpegVideoDecoder::Init(int codec_type, int width, int height) {
    ...
    //how many threads are used by decoder
    codec_context->thread_count = (int)std::thread::hardware_concurrency()/2;
    // !!! important !!!
    // Decode one frame quickly 
    codec_context->thread_type = FF_THREAD_SLICE;
    ...
}
```

##### If you don't known the meaning of buffer's size, please see [Here](2_Convert_to_I420.md)