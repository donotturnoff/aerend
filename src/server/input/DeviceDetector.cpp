#include "DeviceDetector.h"
#include "AerendServer.h"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace aerend {

DeviceDetector::DeviceDetector() {
    thread = std::thread{&DeviceDetector::run, this};
}

DeviceDetector::~DeviceDetector() {
    running = false;
    cv.notify_one();
    thread.join();
}

void DeviceDetector::run() {
    while (running) {
        /* Load all USB device information */
        std::ifstream input{"/proc/bus/input/devices"};
        std::unordered_map<char, std::string> props;
        for (std::string line; getline(input, line); ) {
            if (line == "") {
                process(props);
                props.clear();
            } else {
                props[line[0]] = line.substr(line.find(": ")+2);
            }
        }

        /* Remove disconnected mice */
        auto& ih{AerendServer::the().ih()};
        for (const auto& uid_path : mouse_uid_paths) {
            auto uid{uid_path.first};
            auto path{uid_path.second};
            if (new_mouse_uid_paths.find(uid) == new_mouse_uid_paths.end() || new_mouse_uid_paths[uid] != path) {
                try {
                    ih.rm_device<Mouse>(uid);
                } catch (InputException& e) {
                    std::cerr << "DeviceDetector: failed to remove mouse: " << e.what() << std::endl;
                }
            }
        }
        /* Add new mice */
        for (const auto& uid_path : new_mouse_uid_paths) {
            auto uid{uid_path.first};
            auto path{uid_path.second};
            if (mouse_uid_paths.find(uid) == mouse_uid_paths.end()) {
                try {
                    ih.add_device<Mouse>(uid, path);
                } catch (InputException& e) {
                    std::cerr << "DeviceDetector: failed to add mouse: " << e.what() << std::endl;
                }
            }
        }
        /* Remove disconnected keyboards */
        for (const auto& uid_path : keyboard_uid_paths) {
            auto uid{uid_path.first};
            auto path{uid_path.second};
            if (new_keyboard_uid_paths.find(uid) == new_keyboard_uid_paths.end() || new_keyboard_uid_paths[uid] != path) {
                try {
                    ih.rm_device<Keyboard>(uid);
                } catch (InputException& e) {
                    std::cerr << "DeviceDetector: failed to remove keyboard: " << e.what() << std::endl;
                }
            }
        }
        /* Add new keyboards */
        for (const auto& uid_path : new_keyboard_uid_paths) {
            auto uid{uid_path.first};
            auto path{uid_path.second};
            if (keyboard_uid_paths.find(uid) == keyboard_uid_paths.end()) {
                try {
                    ih.add_device<Keyboard>(uid, path);
                } catch (InputException& e) {
                    std::cerr << "DeviceDetector: failed to add keyboard: " << e.what() << std::endl;
                }
            }
        }
        keyboard_uid_paths = new_keyboard_uid_paths;
        mouse_uid_paths = new_mouse_uid_paths;
        new_keyboard_uid_paths.clear();
        new_mouse_uid_paths.clear();

        /* Sleep for a second */
        std::unique_lock<std::mutex> lock{mtx};
        cv.wait_for(lock, std::chrono::seconds{1}, [&]{ return !running; });
    }
}

/* Case insensitive substring search */
bool find_ci(const std::string& haystack, const std::string& needle) {
    auto it{std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](char c1, char c2) { return std::toupper(c1) == std::toupper(c2); }
    )};
    return (it != haystack.end());
}

void DeviceDetector::process(std::unordered_map<char, std::string> props) {
    std::string name{props['N']};
    std::string handlers{props['H']};
    std::string uid{name};
    auto handler_to_end{handlers.substr(handlers.find("event"))};
    auto space{handler_to_end.find(" ")};
    auto ev_handler{handler_to_end.substr(0, space)};
    if (find_ci(handlers, "kbd")) {
        new_keyboard_uid_paths[uid] = "/dev/input/" + ev_handler;
    } else if (find_ci(handlers, "mouse")) {
        new_mouse_uid_paths[uid] = "/dev/input/" + ev_handler;
    }
}

}
