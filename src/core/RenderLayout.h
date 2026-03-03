#pragma once

struct ViewportRect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

ViewportRect CalculateLetterboxViewport(int windowWidth, int windowHeight, int logicalWidth, int logicalHeight);
