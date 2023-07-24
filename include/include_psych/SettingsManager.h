#pragma once
#include <RSettings.h>
#include <FileIO.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <constants.h>
using json = nlohmann::json;
class SettingsManager {
public:
    static Settings getSettings() {
        if (checkFileExists("/Settings/settings.json"))
        {
            return parseJson();
        }
        else {
            Settings settings;
            dumpJson(settings);
            return settings;
        }
    }
	static Settings parseJson()
	{
		Settings settings;
		std::ifstream f(pathResource + "/Settings/settings.json");
        json data = json::parse(f);
        settings.anti_aliasingMSAA = data["video"]["anti_aliasingMSAA"];
        settings.display_mode = data["video"]["display_mode"];
        settings.resolution[0] = data["video"]["resolution"][0];
        settings.resolution[1] = data["video"]["resolution"][1];
        for (auto control : data["controls"]["keyboard"].items())
        {
            settings.ControlToGLFWKeys[control.key()] = Keys::stringToGLFWKeys[control.value()];
            settings.GLFWKeysToControl[Keys::stringToGLFWKeys[control.value()]] = control.key();
        }
        for (auto control : data["controls"]["mouse"].items())
        {
            settings.ControlToGLFWMouseKeys[control.key()] = Keys::stringToGLFWKeys[control.value()];
            settings.GLFWMouseKeysToControl[Keys::stringToGLFWKeys[control.value()]] = control.key();
        }
        for (auto controlPair : settings.GLFWKeysToControl)
        {
            std::cout << controlPair.first << " : " << controlPair.second << std::endl;
        }
		return settings;
	}
	static void dumpJson(const Settings settings)
	{
        json data;
		auto video = data["video"];
		video["resolution"][0] = settings.resolution[0];video["resolution"][1] = settings.resolution[1];
		video["display_mode"] = (int)settings.display_mode;
		video["anti_aliasingMSAA"] = settings.anti_aliasingMSAA;
		auto controls = data["controls"];
        auto keyboardcontrols = controls["keyboard"];
        for (auto controlPair : settings.GLFWKeysToControl)
        {
            keyboardcontrols[controlPair.second] = Keys::GLFWKeysToString[controlPair.first];
        }
        auto Mousecontrols = controls["mouse"];
        for (auto controlPair : settings.GLFWMouseKeysToControl)
        {
            Mousecontrols[controlPair.second] = Keys::GLFWKeysToString[controlPair.first];
        }
        data["video"] = video;
        data["controls"]["keyboard"] = keyboardcontrols;
        data["controls"]["mouse"] = Mousecontrols;
        std::ofstream o(pathResource + "/Settings/settings.json");
        o << std::setw(4) << data << std::endl;
	}
};