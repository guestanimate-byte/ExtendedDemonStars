#pragma once
#include <Geode/Geode.hpp>
#include <optional>

struct DifficultyMetrics {
    double length = 0.0;          // 0..1
    double framePerfects = 0.0;   // 0..1
    double timing = 0.0;          // 0..1
    double precision = 0.0;       // 0..1
    double visibility = 0.0;      // 0..1
    double awkwardness = 0.0;     // 0..1
    double gameMode = 0.0;        // 0..1
};

struct DemonPlacement {
    int placement = 0;
    int total = 0;
};

struct StarResult {
    int stars = 0;
    double score = 0.0;
    std::optional<int> aredlPlacement;
};

namespace StarCalculator {
    bool isDemon(GJGameLevel* level);
    int demonTier(GJGameLevel* level);
    DifficultyMetrics analyzeLevel(GJGameLevel* level);
    double weightedDifficulty(DifficultyMetrics const& m);
    int calculateStars(GJGameLevel* level, std::optional<DemonPlacement> placement);
}
