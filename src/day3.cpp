#include <string>
#include <string_view>
#include <iostream>
#include <unordered_map>
#include <limits>
#include <thread>
#include <charconv>
#include "help_func.hpp"

bool is_number(char const &c) {
    return c >= 48 && c <= 57;
}

bool is_symbol(char const &c) {
    return !is_number(c) && c != '.';
}

[[nodiscard]] int problem_1(std::vector<std::string>const & input) {
        const std::size_t max_row = input.size();
    const std::size_t max_col = input[0].size();

    int sum = 0;
    int term = 0;

    for(std::size_t i{}; i < max_row; ++i) {
        for(std::size_t j{}; j < max_col; ++j) {
            if(is_symbol(input[i][j])){
                for(int y = -1; y < 2; ++y){
                    for(int x = -1; x < 2; ++x) {
                        std::size_t row = i + y;
                        std::size_t col = j + x;

                        if( row < 0 || row >= max_row || col < 0 || col >= max_col) {
                            continue;
                        }

                        std::string_view number;

                        if(is_number(input[row][col])) {
                            auto s_it = input[row].begin() + col - 1;
                            while(is_number(*s_it) && s_it > input[row].begin())
                            {
                                --s_it;
                            }
                            if(*s_it == '.' || is_symbol(*s_it)) {
                                ++s_it;
                            }

                            auto e_it{s_it};
                            while(is_number(*e_it)) {
                                ++e_it;
                            }
                            std::size_t offset_end = e_it - input[row].begin();
                            std::size_t offset_beginning = s_it - input[row].begin();
                            std::size_t str_len = e_it - s_it;

                            number = std::string_view{s_it.base(), str_len};

                            std::from_chars(number.data(), number.data() + number.size(), term);

                            if(offset_end  - 1 < j || offset_beginning > j) { 
                                sum += term;
                                continue;
                            }

                            if(offset_end  - 1 == j || offset_beginning == j) {
                                sum += term;
                                x = 5;
                                continue;
                            }

                            if(offset_end -1 > j && offset_beginning < j) {
                                sum += term;
                                x = 5;
                                continue;
                            }
                        }
                    }
                }
            }
        }
        
    }
    return sum;
} 

[[nodiscard]] std::uint64_t problem_2(std::vector<std::string> const & input){
        const std::size_t max_row = input.size();
    const std::size_t max_col = input[0].size();

    std::uint64_t sum = 0;
    int term = 0;

    std::vector<int> gear_ratio{};

    for(std::size_t i{}; i < max_row; ++i) {
        for(std::size_t j{}; j < max_col; ++j) {
            if(input[i][j] == '*'){

                for(int y = -1; y < 2; ++y){
                    for(int x = -1; x < 2; ++x) {
                        std::size_t row = i + y;
                        std::size_t col = j + x;

                        if( row < 0 || row >= max_row || col < 0 || col >= max_col) {
                            continue;
                        }

                        std::string_view number;

                        if(is_number(input[row][col])) {
                            auto s_it = input[row].begin() + col - 1;
                            while(is_number(*s_it) && s_it > input[row].begin())
                            {
                                --s_it;
                            }
                            if(*s_it == '.' || is_symbol(*s_it)) {
                                ++s_it;
                            }

                            auto e_it{s_it};
                            while(is_number(*e_it)) {
                                ++e_it;
                            }
                            std::size_t offset_end = e_it - input[row].begin();
                            std::size_t offset_beginning = s_it - input[row].begin();
                            std::size_t str_len = e_it - s_it;

                            number = std::string_view{s_it.base(), str_len};

                            std::from_chars(number.data(), number.data() + number.size(), term);

                            if(offset_end  - 1 < j || offset_beginning > j) { 
                                gear_ratio.push_back(term);
                                continue;
                            }

                            if(offset_end  - 1 == j || offset_beginning == j) {
                                gear_ratio.push_back(term);
                                x = 5;
                                continue;
                            }

                            if(offset_end -1 > j && offset_beginning < j) {
                                gear_ratio.push_back(term);
                                x = 5;
                                continue;
                            }
                        }
                    }
                }
            

            if(gear_ratio.size() == 2)
            {
                sum += gear_ratio[0]*gear_ratio[1];
            }
            gear_ratio.clear();
            }
        }
        
    }
    return sum;
}

int main(){
    help_func::Timer t;
    std::vector<std::string> input =help_func::read_input("../data/input-day-3.txt");
    // std::vector<std::string> input =help_func::read_input("small_data.txt");

    int answer_1 = problem_1(input);
    std::uint64_t answer_2 = problem_2(input);

    std::cout << "Day 3:" << answer_1  << ", " << answer_2 << " ";
}