#pragma once
#include <RSettings.h>
class SettingsManager {
public:
    Settings getSettings();
    Settings parseJson();
    void dumpJson(const Settings settings);
};