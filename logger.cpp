#include "pch.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Logger::Logger()
    :start(std::chrono::high_resolution_clock::now())
{

}
const std::string Logger::AddressToHexString(PDWORD_PTR value)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(sizeof(PDWORD_PTR)) << std::hex << value;
    return ss.str();
}
const std::string Logger::BytesInHex(const BYTE* bytes, INT len)
{
    std::stringstream ss;
    for (INT i=0;i<len;i++)
    {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)bytes[i];
    }
    return ss.str();
}
Logger& Logger::getInstance()
{
    static Logger sInstance;
    return sInstance;
}

void Logger::print(const std::string& message)
{
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = now - start;
    std::cout << '[' << std::fixed << duration.count() << "] " << message << std::endl;
}