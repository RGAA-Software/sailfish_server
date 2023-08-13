#pragma once

// Capture method from VNC

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;
typedef signed char S8;
typedef signed short S16;
typedef signed int S32;

class U8Array {
public:
	U8Array() : buf(0) {}
	U8Array(U8* a) : buf(a) {} // note: assumes ownership
	U8Array(int len) : buf(new U8[len]) {}
	~U8Array() { delete[] buf; }

	// Get the buffer pointer & clear it (i.e. caller takes ownership)
	U8* takeBuf() { U8* tmp = buf; buf = 0; return tmp; }

	U8* buf;
};

class U16Array {
public:
	U16Array() : buf(0) {}
	U16Array(U16* a) : buf(a) {} // note: assumes ownership
	U16Array(int len) : buf(new U16[len]) {}
	~U16Array() { delete[] buf; }
	U16* takeBuf() { U16* tmp = buf; buf = 0; return tmp; }
	U16* buf;
};

class U32Array {
public:
	U32Array() : buf(0) {}
	U32Array(U32* a) : buf(a) {} // note: assumes ownership
	U32Array(int len) : buf(new U32[len]) {}
	~U32Array() { delete[] buf; }
	U32* takeBuf() { U32* tmp = buf; buf = 0; return tmp; }
	U32* buf;
};

class S32Array {
public:
	S32Array() : buf(0) {}
	S32Array(S32* a) : buf(a) {} // note: assumes ownership
	S32Array(int len) : buf(new S32[len]) {}
	~S32Array() { delete[] buf; }
	S32* takeBuf() { S32* tmp = buf; buf = 0; return tmp; }
	S32* buf;
};

class BitmapDC {
public:
	BitmapDC(HDC hdc, HBITMAP hbitmap) {
		dc = CreateCompatibleDC(hdc);
		if (!dc) {
			return;
		}
		oldBitmap = (HBITMAP)SelectObject(dc, hbitmap);
	}
	~BitmapDC() {
		SelectObject(dc, oldBitmap);
		if (dc) {
			DeleteDC(dc);
		}
	}

	operator HDC() const { return dc; }

public:
	HBITMAP oldBitmap;

	HDC dc;
};

#ifdef __GNUC__
#define clz(x) __builtin_clz(x)
#define ctz(x) __builtin_ctz(x)
#else
static uint32_t popcnt(uint32_t x)
{
	x -= ((x >> 1) & 0x55555555);
	x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
	x = (((x >> 4) + x) & 0x0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);
	return x & 0x0000003f;
}
static uint32_t clz(uint32_t x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return 32 - popcnt(x);
}
static uint32_t ctz(uint32_t x)
{
	return popcnt((x & -x) - 1);
}

static uint32_t ffs(uint32_t x)
{
	if (x == 0)
		return 0;
	return ctz((unsigned int)x) + 1;
}
#endif

struct IconInfo : public ICONINFO {
	IconInfo(HICON icon) {
		if (!GetIconInfo(icon, this)) {
			std::cerr << "GetIconInfo error : " << GetLastError() << std::endl;
		}
	}
	~IconInfo() {
		if (hbmColor)
			DeleteObject(hbmColor);
		if (hbmMask)
			DeleteObject(hbmMask);
	}
};

struct Point {
	Point() : x(0), y(0) {}
	Point(int x_, int y_) : x(x_), y(y_) {}
	inline Point negate() const { return Point(-x, -y); }
	inline bool equals(const Point& p) const { return x == p.x && y == p.y; }
	inline Point translate(const Point& p) const { return Point(x + p.x, y + p.y); }
	inline Point subtract(const Point& p) const { return Point(x - p.x, y - p.y); }
	int x, y;
};

static void GetCursorBitmap(HCURSOR hCursor, std::string& bitmap, uint32_t& out_width, uint32_t& out_height)
{
	if (hCursor == nullptr) {
		return;
	}
	
	try {

		int width, height;
		U8Array buffer;

		IconInfo iconInfo((HICON)hCursor);

		BITMAP maskInfo;
		if (!GetObject(iconInfo.hbmMask, sizeof(BITMAP), &maskInfo)) {
			std::cout << "GetObject, error : " << GetLastError() << std::endl;
			return;
		}

		if (maskInfo.bmPlanes != 1) {
			std::cerr << "bmPlanes : " << maskInfo.bmPlanes << std::endl;
			return;
		}
		if (maskInfo.bmBitsPixel != 1) {
			std::cerr << "bmBitsPixel : " << maskInfo.bmBitsPixel << std::endl;
			return;
		}

		width = maskInfo.bmWidth;
		height = maskInfo.bmHeight;
		if (!iconInfo.hbmColor)
			height /= 2;

		int buf_size = width * height * 4;
		buffer.buf = new U8[width * height * 4];

		Point hotspot = Point(iconInfo.xHotspot, iconInfo.yHotspot);

		if (iconInfo.hbmColor) {
			auto device = ::GetDC(NULL);

			BITMAPV5HEADER bi;
			BitmapDC dc(device, iconInfo.hbmColor);

			memset(&bi, 0, sizeof(BITMAPV5HEADER));

			bi.bV5Size = sizeof(BITMAPV5HEADER);
			bi.bV5Width = width;
			bi.bV5Height = -height; // Negative for top-down
			bi.bV5Planes = 1;
			bi.bV5BitCount = 32;
			bi.bV5Compression = BI_BITFIELDS;
			bi.bV5RedMask = 0x000000FF;
			bi.bV5GreenMask = 0x0000FF00;
			bi.bV5BlueMask = 0x00FF0000;
			bi.bV5AlphaMask = 0xFF000000;

			if (!GetDIBits(dc, iconInfo.hbmColor, 0, height,
				buffer.buf, (LPBITMAPINFO)&bi, DIB_RGB_COLORS)) {
				std::cerr << "GetDIBits error :" << GetLastError() << std::endl;
				return;
			}

			// We may not get the RGBA order we want, so shuffle things around
			int ridx, gidx, bidx, aidx;

			ridx = ffs(bi.bV5RedMask) / 8;
			gidx = ffs(bi.bV5GreenMask) / 8;
			bidx = ffs(bi.bV5BlueMask) / 8;
			// Usually not set properly
			aidx = 6 - ridx - gidx - bidx;

			if ((bi.bV5RedMask != ((unsigned)0xff << ridx * 8)) ||
				(bi.bV5GreenMask != ((unsigned)0xff << gidx * 8)) ||
				(bi.bV5BlueMask != ((unsigned)0xff << bidx * 8))) {
				std::cerr << "unsupported cursor color format" << std::endl;
				return;
			}

			U8* rwbuffer = buffer.buf;
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					U8 r, g, b, a;

					r = rwbuffer[ridx];
					g = rwbuffer[gidx];
					b = rwbuffer[bidx];
					a = rwbuffer[aidx];

					rwbuffer[0] = r;
					rwbuffer[1] = g;
					rwbuffer[2] = b;
					rwbuffer[3] = a;

					rwbuffer += 4;
				}
			}

			DeleteDC(device);
		}
		else {
			// B/W cursor

			U8Array mask(maskInfo.bmWidthBytes * maskInfo.bmHeight);
			U8* andMask = mask.buf;
			U8* xorMask = mask.buf + height * maskInfo.bmWidthBytes;

			if (!GetBitmapBits(iconInfo.hbmMask,
				maskInfo.bmWidthBytes * maskInfo.bmHeight, mask.buf)) {
				std::cerr << "GetBitmapBits error : " << GetLastError() << std::endl;
				return;
			}

			bool doOutline = false;
			U8* rwbuffer = buffer.buf;
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int byte = y * maskInfo.bmWidthBytes + x / 8;
					int bit = 7 - x % 8;

					if (!(andMask[byte] & (1 << bit))) {
						// Valid pixel, so make it opaque
						rwbuffer[3] = 0xff;

						// Black or white?
						if (xorMask[byte] & (1 << bit))
							rwbuffer[0] = rwbuffer[1] = rwbuffer[2] = 0xff;
						else
							rwbuffer[0] = rwbuffer[1] = rwbuffer[2] = 0;

					
					}
					else if (xorMask[byte] & (1 << bit)) {
						// Replace any XORed pixels with black, because RFB doesn't support
						// XORing of cursors.  XORing is used for the I-beam cursor, which is most
						// often used over a white background, but also sometimes over a black
						// background.  We set the XOR'd pixels to black, then draw a white outline
						// around the whole cursor.

						rwbuffer[0] = rwbuffer[1] = rwbuffer[2] = 0;
						rwbuffer[3] = 0xff;

						doOutline = true;
						
					}
					else {
						// Transparent pixel
						rwbuffer[0] = rwbuffer[1] = rwbuffer[2] = rwbuffer[3] = 0;
					}

					rwbuffer += 4;
				}
			}

			if (doOutline) {

				// The buffer needs to be slightly larger to make sure there
				// is room for the outline pixels
				U8Array outline((width + 2) * (height + 2) * 4);
				memset(outline.buf, 0, (width + 2) * (height + 2) * 4);

				// Pass 1, outline everything
				U8* in = buffer.buf;
				U8* out = outline.buf + width * 4 + 4;
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						// Visible pixel?
						if (in[3] > 0) {
							// Outline above...
							memset(out - (width + 2) * 4 - 4, 0xff, 4 * 3);
							// ...besides...
							memset(out - 4, 0xff, 4 * 3);
							// ...and above
							memset(out + (width + 2) * 4 - 4, 0xff, 4 * 3);
						}
						in += 4;
						out += 4;
					}
					// outline is slightly larger
					out += 2 * 4;
				}

				// Pass 2, overwrite with actual cursor
				in = buffer.buf;
				out = outline.buf + width * 4 + 4;
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						if (in[3] > 0)
							memcpy(out, in, 4);
						in += 4;
						out += 4;
					}
					out += 2 * 4;
				}

				width += 2;
				height += 2;
				hotspot.x += 1;
				hotspot.y += 1;

				buf_size = width * height * 4;

				delete[] buffer.buf;
				buffer.buf = outline.takeBuf();
			}
		}

		out_width = width;
		out_height = height;
		bitmap.resize(buf_size);
		memcpy((char*)bitmap.data(), buffer.buf, buf_size);
	}
	catch (std::exception& e) {
		std::cerr << "Capture Error : " << e.what() << std::endl;
	}
}
