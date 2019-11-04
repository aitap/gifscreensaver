#include <assert.h>

#include <windows.h>
#include <scrnsave.h>

#define STBI_ONLY_GIF
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct {
	unsigned char * pixels;
	int *delays, x, y, z;
} image = {0};

void get_background(const unsigned char * image, int w, int h, unsigned char background[4]) {
	// average the edge pixels
	unsigned long count = 0;
	double bg[4] = { 0 };
	for (int x = 0; x < w; ++x) {
		for (int c = 0; c < 4; ++c)
			bg[c] = 1./(count + 1.) * (count * bg[c] + image[4 * x + c]);
		++count;
	}
	for (int y = 1; y < (h - 1); ++y) {
		for (int c = 0; c < 4; ++c)
			bg[c] = 1./(count + 1.) * (count * bg[c] + image[4 * y * w + c]);
		++count;
		for (int c = 0; c < 4; ++c)
			bg[c] = 1./(count + 1.) * (count * bg[c] + image[4 * (y * w + (w - 1)) + c]);
		++count;
	}
	for (int x = 0; x < w; ++x) {
		for (int c = 0; c < 4; ++c)
			bg[c] = 1./(count + 1.) * (count * bg[c] + image[4 * (h - 1) * w + c]);
		++count;
	}
	for (int c = 0; c < 4; ++c)
		background[c] = bg[c];
	//printf("bg = %g %g %g %g\n", bg[0], bg[1], bg[2], bg[3]);
}

void prepare() {
	extern unsigned char binary_image_gif_start, binary_image_gif_end;
	int comp = 4;
	image.pixels = stbi_load_gif_from_memory(
		&binary_image_gif_start, &binary_image_gif_end - &binary_image_gif_start,
		&image.delays, &image.x, &image.y, &image.z, &comp, comp
	);
	assert(image.pixels && comp == 4);
}

void cleanup() {
	free(image.pixels);
	free(image.delays);
	image.pixels = NULL;
	image.delays = NULL;
}

LRESULT WINAPI ScreenSaverProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static UINT timer = 0;
	static int frame = 0;

	switch(msg) {
	case WM_CREATE: {
		//printf("WM_CREATE\n");
		RECT rc;
		BOOL ret = GetClientRect(hwnd, &rc);
		assert(ret);
		prepare();
		break;
	}
	case WM_ERASEBKGND: {
		//printf("WM_ERASEBKGND\n");
		unsigned char bg[4];
		get_background(image.pixels, image.x, image.y, bg);
		HBRUSH br = CreateSolidBrush(RGB(bg[0], bg[1], bg[2]));
		assert(br);
		HDC dc = GetDC(hwnd);
		assert(dc);
		RECT rc;
		BOOL ret = GetClientRect (hwnd, &rc);
		assert(ret);
		ret = !!FillRect(dc, &rc, br);
		assert(ret);
		ret = !!ReleaseDC(hwnd, dc);
		assert(ret);
		ret = DeleteObject(br);
		assert(ret);
		/* FALLTHRU */
	}
	case WM_TIMER: {
		// start the timer for the next frame
		timer = SetTimer(hwnd, timer, image.delays[frame], NULL);

		BITMAPINFO bi = {0};
		BITMAPINFOHEADER *bih = &bi.bmiHeader;
		bih->biSize = sizeof(BITMAPINFOHEADER);
		bih->biWidth = image.x, bih->biHeight = -image.y;
		bih->biPlanes = 1, bih->biBitCount = 32;
		bih->biCompression = BI_RGB, bih->biSizeImage = image.x * image.y * 4;
		bih->biClrUsed = bih->biClrImportant = 0;

		RECT rc;
		BOOL ret = GetClientRect(hwnd, &rc);
		assert(ret);
		HDC dc = GetDC(hwnd);
		assert(dc);
		int x, y;
		if ((long long)image.x * rc.bottom > (long long)image.y * rc.right) {
			// xy' > yx' => x y'/y > x' and y x'/x < y'
			x = rc.right;
			y = (int)((long)image.y * rc.right / image.x);
		} else {
			// xy' <= yx' => x y'/y <= x' and y x'/x >= y'
			x = (int)((long)image.x * rc.bottom / image.y);
			y = rc.bottom;
		}
		ret = !!SetStretchBltMode(dc, COLORONCOLOR);
		assert(ret);
		ret = !!StretchDIBits(
			dc,
			rc.right / 2 - x / 2, rc.bottom / 2 - y / 2, x, y,
			0, 0, image.x, image.y,
			image.pixels + image.x * image.y * 4 * frame,
			&bi, DIB_RGB_COLORS, SRCCOPY
		);
		assert(ret);
		ret = !!ReleaseDC(hwnd, dc);
		assert(ret);

		++frame;
		frame %= image.z;
		break;
	}
	case WM_DESTROY:
		//printf("WM_DESTROY\n");
		cleanup();
		KillTimer(hwnd, timer);
		break;
	default:
		return DefScreenSaverProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

WINBOOL WINAPI ScreenSaverConfigureDialog(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam) {
	return FALSE;
}

WINBOOL WINAPI RegisterDialogClasses(HANDLE hInst) {
	return TRUE;
}
