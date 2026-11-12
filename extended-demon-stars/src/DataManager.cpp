#include "DataManager.hpp"
#include <Geode/utils/web.hpp>
#include <filesystem>

using namespace geode::prelude;

namespace {
    constexpr char const* DATA_URL =
        "https://raw.githubusercontent.com/YOUR_USERNAME/extended-demon-stars/main/data/levels.json";
    constexpr char const* CACHE_FILE = "levels-cache.json";
}

DataManager& DataManager::get() {
    static DataManager instance;
    return instance;
}

void DataManager::initialize() {
    loadCache();
    refresh(false);
}

bool DataManager::loadCache() {
    auto path = Mod::get()->getSaveDir() / CACHE_FILE;
    if (!std::filesystem::exists(path)) return false;

    auto parsed = matjson::parse(file::readString(path).unwrapOr(""));
    if (parsed.isErr()) return false;

    apply(parsed.unwrap());
    return true;
}

bool DataManager::saveCache() const {
    matjson::Value root = matjson::Value::object();
    for (auto const& [id, d] : m_levels) {
        matjson::Value v = matjson::Value::object();
        v["placement"] = d.placement;
        v["total"] = d.total;
        if (d.length) v["length"] = *d.length;
        if (d.framePerfects) v["framePerfects"] = *d.framePerfects;
        if (d.timing) v["timing"] = *d.timing;
        if (d.precision) v["precision"] = *d.precision;
        if (d.visibility) v["visibility"] = *d.visibility;
        if (d.awkwardness) v["awkwardness"] = *d.awkwardness;
        if (d.gameMode) v["gameMode"] = *d.gameMode;
        root[std::to_string(id)] = v;
    }

    auto result = file::writeString(Mod::get()->getSaveDir() / CACHE_FILE, root.dump());
    return result.isOk();
}

void DataManager::apply(matjson::Value const& root) {
    if (!root.isObject()) return;

    for (auto const& [key, value] : root) {
        auto id = numFromString<int>(key);
        if (!id) continue;

        OnlineDemonData d{};
        d.placement = value["placement"].asInt().unwrapOr(0);
        d.total = value["total"].asInt().unwrapOr(0);

        if (value.contains("length")) d.length = value["length"].asDouble().unwrapOr(0.0);
        if (value.contains("framePerfects")) d.framePerfects = value["framePerfects"].asDouble().unwrapOr(0.0);
        if (value.contains("timing")) d.timing = value["timing"].asDouble().unwrapOr(0.0);
        if (value.contains("precision")) d.precision = value["precision"].asDouble().unwrapOr(0.0);
        if (value.contains("visibility")) d.visibility = value["visibility"].asDouble().unwrapOr(0.0);
        if (value.contains("awkwardness")) d.awkwardness = value["awkwardness"].asDouble().unwrapOr(0.0);
        if (value.contains("gameMode")) d.gameMode = value["gameMode"].asDouble().unwrapOr(0.0);

        m_levels[*id] = d;
    }

    m_ready = true;
}

void DataManager::refresh(bool force) {
    if (!force && m_ready) {
        auto hours = Mod::get()->getSettingValue<int64_t>("refresh-hours");
        auto now = std::time(nullptr);
        if (now - m_lastRefresh < hours * 3600) return;
    }

    m_lastRefresh = std::time(nullptr);

    web::AsyncWebRequest()
        .fetch(DATA_URL)
        .json()
        .then([this](matjson::Value const& json) {
            apply(json);
            saveCache();
            log::info("Extended Demon Stars data updated");
        })
        .expect([this](std::string const& error) {
            log::warn("Extended Demon Stars update failed: {}", error);
        });
}

std::optional<OnlineDemonData> DataManager::get(int levelID) const {
    auto it = m_levels.find(levelID);
    if (it == m_levels.end()) return std::nullopt;
    return it->second;
}

bool DataManager::ready() const {
    return m_ready;
}
