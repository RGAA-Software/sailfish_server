## Sailfish Remote Controller

[简体中文](README_CN.md)

#### This is the server side of Sailfish Remote Controller, this project is NOT well tested, so, if you want to use or publish in product environment, you should debug it yourself !  
#### The project is NOT a universal remote controller app, please read the doc following. 

#### Why?
I want to control other computers in my local area network, some of the other computers have 2 monitors. I've known that VNC is a good choice, but I need to connect many computers at same time with high FPS.  
Some business products work very well, although multiple monitors function is chargeable, therefore I start to develop this project.

#### Here is a list of the used technologies that you may want to lean 
- Cpp for both server and client
- Qt for the UI
- FFmpeg for encoding and decoding
- OpenGL for client displaying
- Opus for audio encoding and decoding
- VCPKG for package manager
- DDA(Desktop Duplication API) for capturing image
- Libyuv for converting image's format
- Websocket for sending & receiving data packages
- ProtoBuf for serializing data
- LevelDB for saving simple config

### **[How to compile](#Compile)**

#### Features  
- Support H264 and H265, both hardware and software with ffmpeg
- Support multiple clients at same time
- Support 2 or more monitors
- Support high FPS
- Support clipboard for TEXT
- Support audio

#### Roadmap
- Relay mode
- File transfer
- Different monitors size
- Encode with NVENC directly
- Web client

### Here are Server's UI and instruction
#### Server main ui
- IP Accessible: Your computer's IP, one or more
- Port Listening: The port that server is listening on

![](images/main_ui.png)

#### Server settings ui
- Encoder: H264 or H265, choose Hxxx_nvenc first
- Capture Mode: Capture single monitor or all monitors at same time, I'm normally using 2 monitors.
- Running Mode: "Auto fps" means to capture an image when the content has changed while "Try fixing fps" will use a cached image.
- Multi clients: Enable multiple clients to connect at same time or not.
- Preserve time: After the duration in seconds, the image capture, audio capture ,etc, will be destroyed.

![](images/main_ui_settings.png)
#### Yes, A poem, for my children. 
#### I have some incurable illnesses, thanks to my angels, I can overcome these serious matters.
#### Life with you
- Dear angel and sweet monster
- You are pushed to me by the mysterious power with happiness
- cute face like pink marshmallow
- Small hands became magic sticks
- Rescue me from the hell of illness everyday

![](images/main_ui_poem.png)

### Here are Client's UI and instruction

#### Client main ui
- Double click or right click to start a stream
- Click the + button to add new stream
- Right click to edit or delete

![](images/client_main.png)
![](images/client_main_add.png)

#### Client settings ui
- fix the menu at top: true -> the menu will stay on title bar, false -> the menu will stay under title bar
- audio enabled: audio, see below
- clipboard enabled: clipboard, see below
- multiple monitors display mode: two separated windows or one combined window

![](images/client_main_settings.png)

#### Running & Debug
![](images/client_operator.png)
![](images/client_debug.png)

#### 2 Monitors at same time
![](images/client_2_separated.png)

#### Yes, a poem again
#### Together
- Birds build nest and sleep in
- Shining diamonds fall off on my bed
- Gave them back to the sprites
- I am gifted melliferous toffee today
- We will sit at the beach in the future
- Wait for free wind to full in our sleeves


![](images/client_poem.png)

### Compile