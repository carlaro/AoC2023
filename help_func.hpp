#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>

namespace help_func{
    [[nodiscard]] std::vector<std::string> read_input(const char* f_name)
    {
        std::filesystem::path path{f_name};
        bool file_exists = std::filesystem::exists(path);
        if(file_exists)
        {
            std::vector<std::string> lines;
            std::string line;
            // uintmax_t file_size = std::filesystem::file_size(path);
            std::fstream ifs{path};

            while(std::getline(ifs, line))
            {
                lines.push_back(std::move(line));
            }

            return lines;
        }
        return std::vector<std::string>(0);
    }

    class Timer
    {
        std::chrono::time_point<std::chrono::steady_clock> m_start_time;

    public:
        Timer() : m_start_time(std::chrono::steady_clock::now())
        {
        }

        ~Timer()
        {
            std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - m_start_time).count() << '\n';
        }
    };
};