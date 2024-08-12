#include <string>
#include <string_view>
#include <iostream>
#include <unordered_map>
#include <limits>
#include <thread>
#include <charconv>
#include <cmath>
#include <algorithm>
#include <ranges>
#include <stdexcept>
#include "help_func.hpp"

[[nodiscard]] constexpr double calc_gcd(double a, double b) {
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

[[nodiscard]] constexpr double calc_lcm(double a, double b) {
    if (std::numeric_limits<double>::max() / a < b) {
        throw std::overflow_error("calculation will overflow!");
    }

    return a*b/calc_gcd(a,b);
}

[[nodiscard]] std::uint64_t problem_1(std::vector<std::string>& input){
    std::string instructions = input[0];

    std::unordered_map<std::string, std::pair<std::string, std::string>> graph; 

    for(auto &row : input | std::views::drop(2)) {
        auto parts = help_func::split(row, "=");
        std::string key = parts[0];
        help_func::trim(key);
        help_func::trim(parts[1]);
        help_func::ltrim_char(parts[1],'(');
        help_func::rtrim_char(parts[1],')');
        auto right_and_left = help_func::split(parts[1], ",");
        std::string left = right_and_left[0];
        std::string right = right_and_left[1];

        help_func::trim(left);
        help_func::trim(right);

        graph[key] = {left, right};
    }

    std::string loc = "AAA";
    auto instruction_it = instructions.begin();
    int steps = 0;

    while(loc != "ZZZ") 
    {
        if(instructions.end() == instruction_it) {
            instruction_it = instructions.begin();
        }

        if(*instruction_it == 'L') {
            loc = graph[loc].first;
            ++instruction_it;
            ++steps;
            continue;
        }

        loc = graph[loc].second;
        ++instruction_it;
        ++steps;
    }

    return steps;
}

[[nodiscard]] std::uint64_t problem_2(std::vector<std::string>& input){
        std::string instructions = input[0];

    std::unordered_map<std::string, std::pair<std::string, std::string>> graph; 

    std::vector<std::string> keys_ending_with_A;

    for(auto &row : input | std::views::drop(2)) {
        auto parts = help_func::split(row, "=");
        std::string key = parts[0];
        help_func::trim(key);
        help_func::trim(parts[1]);
        help_func::ltrim_char(parts[1],'(');
        help_func::rtrim_char(parts[1],')');
        auto right_and_left = help_func::split(parts[1], ",");
        std::string left = right_and_left[0];
        std::string right = right_and_left[1];
        help_func::trim(left);
        help_func::trim(right);
        graph[key] = {left, right};
        if(key[2] == 'A') {
            keys_ending_with_A.push_back(key);
        }
    }

    std::vector<std::uint64_t> cyclicity;
    cyclicity.reserve(keys_ending_with_A.size());
    for(auto &key : keys_ending_with_A) 
    {
        auto instruction_it = instructions.begin();
        std::string loc = key;
        std::uint64_t steps{};
        while(loc[2] != 'Z') {

            if(instructions.end() == instruction_it) {
                instruction_it = instructions.begin();
            }

            if(*instruction_it == 'L') {
                loc = graph[loc].first;
                ++instruction_it;
                ++steps;
                continue;
            }

            loc = graph[loc].second;
            ++instruction_it;
            ++steps;

        }
        cyclicity.push_back(steps);
    }

    double base = cyclicity[0];
    for(auto e : cyclicity | std::views::drop(1)) {
        base = calc_lcm(base, e);
    }

    return base;
}

int main(){
    help_func::Timer t;
    std::vector<std::string> input =help_func::read_input("../data/input-day-8.txt");
    // std::vector<std::string> input =help_func::read_input("small_data.txt");

    std::uint64_t answer_1 = problem_1(input);
    std::uint64_t answer_2 = problem_2(input);

    std::cout << "Day 8:" << answer_1  << ", " << answer_2 << " ";
}