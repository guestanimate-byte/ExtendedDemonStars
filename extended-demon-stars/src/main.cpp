#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include "StarCalculator.hpp"
#include "DataManager.hpp"

using namespace geode::prelude;

namespace {
    constexpr char const* LABEL_ID = "extended-demon-stars-label";

    bool shouldShow(GJGameLevel* level) {
        return Mod::get()->getSettingValue<bool>("enabled") &&
               StarCalculator::isDemon(level);
    }

    int starsFor(GJGameLevel* level) {
        auto online = DataManager::get().get(level->m_levelID.value());

        if (online && online->placement > 0 && online->total > 0) {
            return StarCalculator::calculateStars(
                level,
                DemonPlacement{online->placement, online->total}
            );
        }

        return StarCalculator::calculateStars(level, std::nullopt);
    }

    void addOrUpdateLabel(CCNode* parent, GJGameLevel* level) {
        if (!parent || !level) return;

        if (auto old = parent->getChildByID(LABEL_ID)) {
            old->removeFromParent();
        }

        if (!shouldShow(level)) return;

        auto stars = starsFor(level);

        auto label = CCLabelBMFont::create(
            fmt::format("{} ★", stars).c_str(),
            "goldFont.fnt"
        );
        if (!label) return;

        label->setID(LABEL_ID);
        label->setScale(.55f);

        auto size = parent->getContentSize();
        label->setPosition({size.width - 48.f, 18.f});
        parent->addChild(label, 9999);
    }
}

class $modify(ExtendedDemonStarsLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        addOrUpdateLabel(this, level);
        return true;
    }
};

class $modify(ExtendedDemonStarsLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        // The cell layout differs between GD versions and list types, so use
        // a small overlay instead of modifying RobTop's actual star value.
        addOrUpdateLabel(this, level);
    }
};

$on_mod(Loaded) {
    DataManager::get().initialize();
    log::info("Extended Demon Stars loaded");
}
