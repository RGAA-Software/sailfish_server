#### Replay mouse events

- use Win32 API [SendInput] to generate a event
- please see src/controller/MouseReplayer.cpp

```c++
void MouseReplayer::ReplayGlobalAbsolute(const std::shared_ptr<NetMessage>& msg) {

    // information from client
    auto& info = msg->mouse_info();
    // which button: Left,Right,Middle
    MouseKey key = info.key();
    bool pressed = info.pressed();
    bool released = info.released();
    // X Y coordinates, 0~1.0
    float x = info.mouse_x();
    float y = info.mouse_y();
    float dx = info.mouse_dx();
    float dy = info.mouse_dy();
    // Middel button, positive or negative number
    int scroll = info.middle_scroll();

    int cx_screen = ::GetSystemMetrics(SM_CXSCREEN);
    int cy_screen = ::GetSystemMetrics(SM_CYSCREEN);

    // Very Important!!!
    // Transfer position to 0~65535

    int real_x = (int)(kMaxNDCValue * x);
    int real_y = (int)(kMaxNDCValue * y);

    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = real_x;
    input.mi.dy = real_y;
    input.mi.mouseData = 0;
    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    if (scroll == 0) {
        input.mi.dwFlags |= MOUSEEVENTF_MOVE;
    }
    else {
        input.mi.dwFlags |= MOUSEEVENTF_WHEEL;
        input.mi.mouseData = scroll;
    }

    if (pressed) {
        if (key == MouseKey::kLeft) {
            input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
        }
        else if (key == MouseKey::kRight) {
            input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
        }
        else if (key == MouseKey::kMiddle) {
            input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
        }
    }
    else if (released) {
        if (key == MouseKey::kLeft) {
            input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
        }
        else if (key == MouseKey::kRight) {
            input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
        }
        else if (key == MouseKey::kMiddle) {
            input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
        }
    }
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    SendInput(1, &input, sizeof(INPUT));
}

```

#### Replay keyboard events
- use Win32 API [SendInput] to generate event
- please see src/controller/KeyboardReplayer.cpp

```c++
void KeyboardReplayer::ReplayKeyboardGlobal(const  std::shared_ptr<NetMessage>& msg) {
    auto& info = msg->keyboard_info();
    int vk = info.vk();
    bool pressed = info.pressed();
    int scancode = info.scancode();
    bool caps_lock = info.caps_lock();
    bool num_lock = info.num_lock();

    INPUT m_InPut;
    memset(&m_InPut, 0, sizeof(INPUT));
    m_InPut.type = INPUT_KEYBOARD;
    m_InPut.ki.wVk = vk;
    // to send text 
    m_InPut.ki.dwFlags = KEYEVENTF_UNICODE;  //KEYEVENTF_SCANCODE;
    m_InPut.ki.wScan = 0; 
    m_InPut.ki.dwExtraInfo = 0;
    m_InPut.ki.time = 0;

    if (!pressed) {
        m_InPut.ki.dwFlags |= KEYEVENTF_KEYUP;
    }
    if (IsSystemKey(vk)) {
        m_InPut.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }

    if (SendInput(1, &m_InPut, sizeof(INPUT)) != 1) {
        LOGE( "SendInput error : {0:x}", GetLastError());
    }
}
```