#pragma once
#include <chrono>
#include <string>
class Logger {
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
public:
    static Logger& getInstance();
    static const std::string AddressToHexString(PDWORD_PTR value);
    static const std::string BytesInHex(const BYTE* bytes, INT len);
    void print(const std::string&);
private:
    Logger();
};