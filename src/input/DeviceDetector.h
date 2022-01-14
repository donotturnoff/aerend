#ifndef DEVICE_DETECTOR_H
#define DEVICE_DETECTOR_H

#include "InputDevice.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <string>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace aerend {

class DeviceDetector {
public:
    DeviceDetector();
    ~DeviceDetector();
private:
    void run();
    void process(std::unordered_map<char, std::string>);
    std::unordered_map<std::string, std::string> mouse_uid_paths, keyboard_uid_paths, new_mouse_uid_paths, new_keyboard_uid_paths;
    std::thread thread;
    std::atomic<bool> running = true;
    std::mutex mtx;
    std::condition_variable cv;
};

}

#endif // DEVICE_DETECTOR_H

