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

    [[nodiscard]] std::vector<std::string> split(const std::string &s, std::string delim, bool trim_empty = true) {
        std::vector<std::string> s_vec{};
        std::size_t search_from{};
        std::size_t found_at = s.find(delim, search_from);
        std::size_t offset = delim.size();

        while(found_at < s.npos) {
            std::size_t distance = found_at - search_from;
            if(distance == 0 && trim_empty)
            {
                search_from += offset;
                found_at = s.find(delim, search_from);
                continue;
            }
            s_vec.push_back(s.substr(search_from, distance));
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

    void ltrim_char(std::string& s, char c){
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [c](auto c1) {return c != c1;}));
    }

    void rtrim_char(std::string& s, char c){
        s.erase(std::find_if(s.rbegin(), s.rend(), [c](auto c1) {return c != c1;}).base(), s.end());
    }

    void trim_char(std::string& s, char c) {
        help_func::ltrim_char(s, c);
        help_func::rtrim_char(s, c);
    }

    [[nodiscard]] constexpr double gcd(double a, double b) {
    double d = (a > b) ? b : a;
    double n = (a > b) ? a : b;

    double q = std::floor(n / d);
    double r = n - d*q;
    while(r != 0) {
        n = d;
        d = r;
        q = std::floor(n / d);
        r = n - d*q;
    }
    return d;
}

    [[nodiscard]] constexpr double lcm(double a, double b) {
        if (std::numeric_limits<double>::max() / a < b) {
            throw std::overflow_error("calculation will overflow!");
        }

        return a*b/gcd(a,b);
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