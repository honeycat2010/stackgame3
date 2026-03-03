#include "core/RenderLayout.h"

ViewportRect CalculateLetterboxViewport(const int windowWidth, const int windowHeight,
                                        const int logicalWidth, const int logicalHeight) {
    ViewportRect rect{};

    if (windowWidth <= 0 || windowHeight <= 0 || logicalWidth <= 0 || logicalHeight <= 0) {
        return rect;
    }

    const double windowAspect = static_cast<double>(windowWidth) / static_cast<double>(windowHeight);
    const double logicalAspect = static_cast<double>(logicalWidth) / static_cast<double>(logicalHeight);

    if (windowAspect > logicalAspect) {
        rect.height = windowHeight;
        rect.width = static_cast<int>(windowHeight * logicalAspect);
        rect.x = (windowWidth - rect.width) / 2;
        rect.y = 0;
    } else {
        rect.width = windowWidth;
        rect.height = static_cast<int>(windowWidth / logicalAspect);
        rect.x = 0;
        rect.y = (windowHeight - rect.height) / 2;
    }

    return rect;
}
