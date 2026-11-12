#pragma once
#include <Geode/Geode.hpp>
#include <optional>
#include <string>
#include <unordered_map>

struct OnlineDemonData {
    int placement = 0;
    int total = 0;
    std::optional<double> length;
    std::optional<double> framePerfects;
    std::optional<double> timing;
    std::optional<double> precision;
    std::optional<double> visibility;
    std::optional<double> awkwardness;
    std::optional<double> gameMode;
};

class DataManager {
public:
    static DataManager& get();

    void initialize();
    void refresh(bool force = false);

    std::optional<OnlineDemonData> get(int levelID) const;
    bool ready() const;

private:
    bool loadCache();
    bool saveCache() const;
    void apply(matjson::Value const& value);

    std::unordered_map<int, OnlineDemonData> m_levels;
    bool m_ready = false;
    int64_t m_lastRefresh = 0;
};
