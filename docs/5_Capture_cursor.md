#### Since capturing the cursor is not a simple task due to the varying states of the cursor, I had learned the method from VNC's repository.
#### Please see src/capture/CursorCapture.h and src/capture/CursorCapture.cpp. It's very easy to use now.

#### If you want to use it, just call this method. The meaning of the params:
- hCursor : current cursor, from the Win32 API: GetCursorInfo(&cursor_info);
- bitmap: bitmap of the cursor, 32x32(arrow cursor or other...) or 34x34( I type ) in 1080P, bigger size in 4K or 4K+
- out_width: bitmap's width
- out_height: bitmap's height
- hotspot: size to adjust cursor's position

```c++
 void GetCursorBitmap(HCURSOR hCursor, std::string &bitmap, uint32_t &out_width, uint32_t &out_height, Point& hotspot) {
    ...
}
```