#include <SettingsManager.h>
#include <FileIO.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <constants.h>

Settings SettingsManager::getSettings() {
    if (checkFileExists(pathResource + "/Settings/settings.json"))
    {
        return parseJson();
    }
    else {
        std::cout << "didn't find settings.json, creating";
        Settings settings;
        dumpJson(settings);
        return settings;
    }
}
Settings SettingsManager::parseJson()
{
    using json = nlohmann::json;
    Settings settings;
    std::ifstream f(pathResource + "/Settings/settings.json");
    json data = json::parse(f);
    settings.anti_aliasingMSAA = data["video"]["anti_aliasingMSAA"];
    settings.display_mode = data["video"]["display_mode"];

    settings.resolution[0] = data["video"]["resolution"][0];
    settings.resolution[1] = data["video"]["resolution"][1];
    for (auto const control : data["controls"]["keyboard"].items())
    {
        // control - > "BACKWARD_KEY": "S"
        // ControlToGLFWKeys - > "BACKWARD_KEY" : (int)GLFW_KEY_S
        settings.ControlToGLFWKeys[control.key()] = Keys::stringToGLFWKeys[control.value()];
        // "BACKWARD_KEY": "S",
        settings.GLFWKeysToControl[Keys::stringToGLFWKeys[control.value()]] = control.key();
    }
    for (auto const control : data["controls"]["mouse"].items())
    {
        settings.ControlToGLFWMouseKeys[control.key()] = Keys::stringToGLFWKeys[control.value()];
        settings.GLFWMouseKeysToControl[Keys::stringToGLFWKeys[control.value()]] = control.key();
    }
    for (auto const controlPair : settings.GLFWKeysToControl)
    {
        std::cout << controlPair.first << " : " << controlPair.second << std::endl;
    }
    for (auto const controlPair : settings.GLFWMouseKeysToControl)
    {
        std::cout << controlPair.first << " : " << controlPair.second << std::endl;
    }
    return settings;
}
void  SettingsManager::dumpJson(const Settings settings)
{
    using json = nlohmann::json;
    json data;
    auto video = data["video"];
    video["resolution"][0] = settings.resolution[0]; video["resolution"][1] = settings.resolution[1];
    video["display_mode"] = (int)settings.display_mode;
    video["anti_aliasingMSAA"] = settings.anti_aliasingMSAA;
    auto controls = data["controls"];
    auto keyboardcontrols = controls["keyboard"];
    for (auto const controlPair : settings.GLFWKeysToControl)
    {
        keyboardcontrols[controlPair.second] = Keys::GLFWKeysToString[controlPair.first];
    }
    auto Mousecontrols = controls["mouse"];
    for (auto const controlPair : settings.GLFWMouseKeysToControl)
    {
        Mousecontrols[controlPair.second] = Keys::GLFWKeysToString[controlPair.first];
    }
    data["video"] = video;
    data["controls"]["keyboard"] = keyboardcontrols;
    data["controls"]["mouse"] = Mousecontrols;
    std::ofstream o(pathResource + "/Settings/settings.json");
    o << std::setw(4) << data << std::endl;
}

