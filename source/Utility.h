#pragma once
#include "common.h"

#define DEFAULT_SCREEN_WIDTH 640.0f
#define DEFAULT_SCREEN_HEIGHT 480.0f
#define DEFAULT_SCREEN_ASPECT_RATIO (DEFAULT_SCREEN_WIDTH / DEFAULT_SCREEN_HEIGHT)
#define SCREEN_ASPECT_RATIO (SCREEN_WIDTH / SCREEN_HEIGHT)

static float ScaleX(float x) {
    float f = ((x) * (float)SCREEN_WIDTH / DEFAULT_SCREEN_WIDTH) * DEFAULT_SCREEN_ASPECT_RATIO / SCREEN_ASPECT_RATIO;
    return f;
}

static float ScaleXKeepCentered(float x) {
    float f = ((SCREEN_WIDTH == DEFAULT_SCREEN_WIDTH) ? (x) : (SCREEN_WIDTH - ScaleX(DEFAULT_SCREEN_WIDTH)) / 2 + ScaleX((x)));
    return f;
}

static float ScaleY(float y) {
    float f = ((y) * (float)SCREEN_HEIGHT / DEFAULT_SCREEN_HEIGHT);
    return f;
}

static float ScaleW(float w) {
    float f = ((w) * (float)SCREEN_WIDTH / DEFAULT_SCREEN_WIDTH) * DEFAULT_SCREEN_ASPECT_RATIO / SCREEN_ASPECT_RATIO;
    return f;
}

static float ScaleH(float h) {
    float f = ((h) * (float)SCREEN_HEIGHT / DEFAULT_SCREEN_HEIGHT);
    return f;
}

#define SCREEN_SCALE_X(x) (ScaleX(x))
#define SCREEN_SCALE_X_CENTER(x) (ScaleXKeepCentered(x))
#define SCREEN_SCALE_Y(y) (ScaleY(y))
#define SCREEN_SCALE_FROM_RIGHT(x) (SCREEN_WIDTH - SCREEN_SCALE_X(x))
#define SCREEN_SCALE_FROM_BOTTOM(y) (SCREEN_HEIGHT - SCREEN_SCALE_Y(y))
