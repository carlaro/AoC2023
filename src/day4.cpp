#include <string>
#include <string_view>
#include <iostream>
#include <unordered_map>
#include <limits>
#include <thread>
#include <charconv>
#include <cmath>
#include "help_func.hpp"

[[nodiscard]] int problem_1(std::vector<std::string>& input){

    int sum{};

    for(auto &line: input){

        int res{};
        auto sub_string = help_func::split(line, ":");
        help_func::trim(sub_string[1]);
        auto target_and_result = help_func::split(sub_string[1], "|");
        auto target = target_and_result[0];
        auto result = target_and_result[1];

        help_func::trim(target);
        help_func::trim(result);

        auto target_numbers_as_strings = help_func::split(target, " ");
        auto result_numbers_as_strings = help_func::split(result, " ");

        for(auto &s : target_numbers_as_strings) {
            help_func::trim(s);
        }
        for(auto &s : result_numbers_as_strings) {
            help_func::trim(s);
        }

        std::vector<int> target_nums;
        std::vector<int> result_nums;

        for(const std::string & s : target_numbers_as_strings){    
            target_nums.push_back(std::stoi(s));
        }
        for(const std::string & s : result_numbers_as_strings){
            result_nums.push_back(std::stoi(s));
        }

        for(int const &i : target_nums)
        {
            if(std::find(result_nums.begin(), result_nums.end(), i) != result_nums.end())
            {
                res += 1;
            }
        }

        sum += std::pow(2, res - 1);
    }

    return sum;
}

[[nodiscard]] std::uint64_t problem_2(std::vector<std::string>& input){

    std::vector<std::uint64_t> number_of_winning_cards;

    for(auto &line: input){
        int res{};
        auto sub_string = help_func::split(line, ":");
        help_func::trim(sub_string[1]);
        auto target_and_result = help_func::split(sub_string[1], "|");
        auto target = target_and_result[0];
        auto result = target_and_result[1];
        help_func::trim(target);
        help_func::trim(result);
        auto target_numbers_as_strings = help_func::split(target, " ");
        auto result_numbers_as_strings = help_func::split(result, " ");
        for(auto &s : target_numbers_as_strings) {
            help_func::trim(s);
        }
        for(auto &s : result_numbers_as_strings) {
            help_func::trim(s);
        }
        std::vector<int> target_nums;
        std::vector<int> result_nums;
        for(const std::string & s : target_numbers_as_strings){    
            target_nums.push_back(std::stoi(s));
        }
        for(const std::string & s : result_numbers_as_strings){
            result_nums.push_back(std::stoi(s));
        }
        for(int const &i : target_nums) {
            if(std::find(result_nums.begin(), result_nums.end(), i) != result_nums.end()) {
                res += 1;
            }
        }
        number_of_winning_cards.push_back(res);
    }

    std::size_t number_of_tickets = input.size();
    std::vector<std::uint64_t> dynamic_programming_vector(number_of_tickets, 1);

    for(std::size_t i = 0; i < number_of_tickets; ++i) {
        std::uint64_t copies_below = number_of_winning_cards[i];
        for(std::size_t j = 0; j < copies_below; ++j){
            dynamic_programming_vector[i + j + 1] += dynamic_programming_vector[i];
        }
    }

    std::uint64_t sum{};
    for(auto i :  dynamic_programming_vector) {
        sum += i;
    }

    return sum;
}

int main(){
    help_func::Timer t;
    std::vector<std::string> input =help_func::read_input("../data/input-day-4.txt");
    // std::vector<std::string> input =help_func::read_input("small_data.txt");

    int answer_1 = problem_1(input);
    std::uint64_t answer_2 = problem_2(input);

    std::cout << "Day 4:" << answer_1  << ", " << answer_2 << " ";
}