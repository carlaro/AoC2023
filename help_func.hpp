#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>
#include <functional>
#include <algorithm>

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

    [[nodiscard]] std::vector<std::string> split(std::string s, std::string delim) {
        std::vector<std::string> s_vec{};
        std::size_t search_from{};
        std::size_t found_at = s.find(delim, search_from);
        std::size_t offset = delim.size();

        while(found_at < s.npos) {
            s_vec.push_back(s.substr(search_from, found_at - search_from));
            search_from = found_at + offset;
            found_at = s.find(delim, search_from);
        }
        s_vec.push_back(s.substr(search_from));

        return s_vec;
    }

    void ltrim(std::string& s){
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](auto c) {return !std::isspace(c);}));
    }

    void rtrim(std::string& s){
        s.erase(std::find_if(s.rbegin(), s.rend(), [](auto c) {return !std::isspace(c);}).base(), s.end());
    }
    
    void trim(std::string& s) {
        help_func::ltrim(s);
        help_func::rtrim(s);
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