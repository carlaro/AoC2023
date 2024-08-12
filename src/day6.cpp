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
#include "help_func.hpp"

[[nodiscard]] int calc_valid_strats(double time, double distance) {
    if(time*time - 4.0*distance < 0) {
        return 0;
    }
    double upper_bound = std::floor(time*0.5 + std::sqrt(time*time*0.25 - distance));
    double lower_bound = std::ceil(time*0.5 - std::sqrt(time*time*0.25 - distance));

    if(upper_bound*(time - upper_bound) == distance) {
        return upper_bound - lower_bound - 1;
    }
    return upper_bound - lower_bound + 1;
}

[[nodiscard]] std::uint64_t problem_1(std::vector<std::string>& input){
    auto t_vec = help_func::split(input[0], ":");
    auto time_strings = help_func::split(t_vec[1], " ");
    std::vector<double> times;
    auto d_vec = help_func::split(input[1], ":");
    auto distance_strings = help_func::split(d_vec[1], " ");
    std::vector<double> distances;

    for(auto i : time_strings) {
        times.push_back(std::stod(i));
    }

    for(auto i : distance_strings) {
        distances.push_back(std::stod(i));
    }

    int retval = 1;
    for(std::size_t i = 0; i < times.size(); ++i) {
        retval *= calc_valid_strats(times[i], distances[i]);
    }

    return retval;
}

[[nodiscard]] std::uint64_t problem_2(std::vector<std::string>& input){
    auto t_vec = help_func::split(input[0], ":");
    auto time_strings = help_func::split(t_vec[1], " ");
    auto d_vec = help_func::split(input[1], ":");
    auto distance_strings = help_func::split(d_vec[1], " ");

    std::string ts{};
    std::string ds{};
    for(size_t i = 0; i < time_strings.size(); ++i) {
        ts += time_strings[i];
        ds += distance_strings[i];
    }

    double time = std::stod(ts);
    double distance = std::stod(ds);
    double retval = calc_valid_strats(time, distance);

    return retval;
}

int main(){
    help_func::Timer t;
    std::vector<std::string> input =help_func::read_input("../data/input-day-6.txt");
    // std::vector<std::string> input =help_func::read_input("small_data.txt");

    std::uint64_t answer_1 = problem_1(input);
    std::uint64_t answer_2 = problem_2(input);

    std::cout << "Day 6:" << answer_1  << ", " << answer_2 << " ";
}