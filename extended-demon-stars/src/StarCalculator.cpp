#include "StarCalculator.hpp"
#include <algorithm>
#include <cmath>

using namespace geode::prelude;

namespace {
    constexpr double W_LENGTH       = 0.24;
    constexpr double W_FRAME        = 0.20;
    constexpr double W_TIMING       = 0.16;
    constexpr double W_PRECISION    = 0.13;
    constexpr double W_VISIBILITY   = 0.11;
    constexpr double W_AWKWARDNESS = 0.09;
    constexpr double W_GAMEMODE     = 0.07;

    double clamp01(double x) {
        return std::clamp(x, 0.0, 1.0);
    }

    // This is intentionally deterministic and conservative. A future
    // analyzer can replace individual metrics without changing the public
    // star-calculation interface.
    double normalizeLength(GJGameLevel* level) {
        if (!level) return 0.0;
        // Geometry Dash's raw level object does not expose a single universal
        // "human length" field, so use object count as a stable first-pass
        // proxy. The online data file can override this for known levels.
        auto objects = static_cast<double>(level->m_objectCount);
        return clamp01(std::log1p(objects) / std::log1p(10000.0));
    }
}

namespace StarCalculator {

bool isDemon(GJGameLevel* level) {
    return level && static_cast<int>(level->m_demon) != 0;
}

int demonTier(GJGameLevel* level) {
    if (!isDemon(level)) return 0;

    // Geometry Dash's demon difficulty enum is used for the native
    // Easy/Medium/Hard/Insane/Extreme categories.
    switch (static_cast<int>(level->m_demonDifficulty)) {
        case 0: return 1; // Easy
        case 1: return 2; // Medium
        case 2: return 3; // Hard
        case 3: return 4; // Insane
        case 4: return 5; // Extreme
        default: return 1;
    }
}

DifficultyMetrics analyzeLevel(GJGameLevel* level) {
    DifficultyMetrics m{};
    if (!level) return m;

    m.length = normalizeLength(level);

    // The base analyzer intentionally starts these values at neutral values.
    // Known-level overrides from data.json/AREDL can supply measured metrics.
    // This prevents guessing that every level's gamemode/visibility/etc.
    // difficulty can be perfectly inferred from only rating metadata.
    m.framePerfects = 0.0;
    m.timing = 0.0;
    m.precision = 0.0;
    m.visibility = 0.0;
    m.awkwardness = 0.0;
    m.gameMode = 0.0;

    return m;
}

double weightedDifficulty(DifficultyMetrics const& m) {
    return
        clamp01(m.length)       * W_LENGTH +
        clamp01(m.framePerfects)* W_FRAME +
        clamp01(m.timing)      * W_TIMING +
        clamp01(m.precision)   * W_PRECISION +
        clamp01(m.visibility)  * W_VISIBILITY +
        clamp01(m.awkwardness) * W_AWKWARDNESS +
        clamp01(m.gameMode)    * W_GAMEMODE;
}

int calculateStars(GJGameLevel* level, std::optional<DemonPlacement> placement) {
    if (!isDemon(level)) return 0;

    // Fixed tier boundaries requested by the project.
    constexpr int easyMin = 100;
    constexpr int easyMax = 199;
    constexpr int mediumMin = 200;
    constexpr int mediumMax = 399;
    constexpr int hardMin = 400;
    constexpr int hardMax = 699;
    constexpr int insaneMin = 700;
    constexpr int insaneMax = 999;
    constexpr int extremeMin = 1000;

    auto tier = demonTier(level);
    auto difficulty = weightedDifficulty(analyzeLevel(level));

    if (tier == 1) return easyMin + static_cast<int>(difficulty * (easyMax - easyMin));
    if (tier == 2) return mediumMin + static_cast<int>(difficulty * (mediumMax - mediumMin));
    if (tier == 3) return hardMin + static_cast<int>(difficulty * (hardMax - hardMin));
    if (tier == 4) return insaneMin + static_cast<int>(difficulty * (insaneMax - insaneMin));

    // Extreme: AREDL placement is the ordering signal, while the weighted
    // difficulty affects the exact value. The exponent creates the requested
    // increasingly steep spacing toward #1.
    if (placement && placement->placement > 0 && placement->total > 1) {
        double p = static_cast<double>(placement->placement - 1) /
                   static_cast<double>(placement->total - 1);
        double closenessToTop = 1.0 - clamp01(p);

        // Exponential top-end curve. These are calibration constants, not
        // claims about objective human difficulty.
        constexpr double exponent = 3.2;
        constexpr double range = 250000.0;

        auto curve = std::pow(closenessToTop, exponent);
        auto measured = 0.65 + 0.35 * difficulty;
        return extremeMin + static_cast<int>(range * curve * measured);
    }

    // Unknown Extreme Demon: stay above 1000 until AREDL data arrives.
    return extremeMin + static_cast<int>(difficulty * 999.0);
}

} // namespace StarCalculator
