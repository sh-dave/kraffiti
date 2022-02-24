#include <stdio.h>

typedef struct rgb {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} rgb_t;

typedef struct hsl {
	int h;
	float s;
	float l;
} hsl_t;

float max1( float a, float b ) { return a >= b ? a : b; }
float min1( float a, float b ) { return a <= b ? a : b; }
float clamp01( float v ) { return v < 0 ? 0 : v > 1 ? 1 : v; }

float hue_to_rgb( float v1, float v2, float vh ) {
	if (vh < 0) vh += 1;
	if (vh > 1) vh -= 1;
	if (6 * vh < 1) return (v1 + (v2 - v1) * 6 * vh);
	if (2 * vh < 1) return v2;
	if (3 * vh < 2) return (v1 + (v2 - v1) * ((2.f / 3.f) - vh) * 6);
	return v1;
}

void hsl_to_rgb( hsl_t *hsl, rgb_t *rgb ) {
	float r, g, b;

	if (hsl->s == 0) {
		r = g = b = hsl->l;
	} else {
		float hue = hsl->h / 360.f;
		float v2 = hsl->l < 0.5 ? hsl->l * (1 + hsl->s) : hsl->l + hsl->s - (hsl->l * hsl->s);
		float v1 = 2 * hsl->l - v2;
		r = hue_to_rgb(v1, v2, hue + (1.f / 3.f));
		g = hue_to_rgb(v1, v2, hue);
		b = hue_to_rgb(v1, v2, hue - (1.f / 3.f));
	}

	rgb->r = r * 255;
	rgb->g = g * 255;
	rgb->b = b * 255;
}

void rgb_to_hsl( rgb_t *rgb, hsl_t *hsl ) {
	float r = rgb->r / 255.f;
	float g = rgb->g / 255.f;
	float b = rgb->b / 255.f;
	float min = min1(min1(r, g), b);
	float max = max1(max1(r, g), b);
	float delta = max - min;
	float h = 0;
	float s = 0;
	float l = (max + min) / 2;

	if (delta == 0) {
		h = s = 0;
	} else {
		s = l <= 0.5 ? (delta / (max + min)) : (delta / (2 - max - min));

		if (max == r) {
			h = ((g - b) / 6) / delta;
		} else if (max == g) {
			h = (1.f / 3.f) + ((b - r) / 6) / delta;
		} else {
			h = (2.f / 3.f) + ((r - g) / 6) / delta;
		}
	}

	if (h < 0) { h += 1; }
	if (h > 1) { h -= 1; }

	hsl->h = h * 360;
	hsl->s = s;
	hsl->l = l;
}

rgb_t * lighten_image( rgb_t *rgb, float amount ) {
	hsl_t hsl;

	rgb_to_hsl(rgb, &hsl);
	hsl.l += amount / 100;
	hsl.l = clamp01(hsl.l);
	hsl_to_rgb(&hsl, rgb);
	return rgb;
}
