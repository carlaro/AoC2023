#include "help_func.hpp"
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

[[nodiscard]] int
problem_1(std::vector<std::string> const& input)
{
  std::unordered_map<std::string, int> boundaries{ { "red", 12 },
                                                   { "green", 13 },
                                                   { "blue", 14 } };
  int sum{};

  for (std::string const& s : input) {
    auto split_vec = help_func::split(s, ": ");

    std::string game_string = split_vec[0];
    std::size_t from{ s.find(" ") };
    std::size_t to{ s.find(":") };
    std::string game_num = game_string.substr(from + 1, to - from - 1);

    std::string shown_string = split_vec[1];
    auto sub_games = help_func::split(shown_string, ";");

    bool game_possible = true;

    for (auto sub_game : sub_games) {

      help_func::trim(sub_game);
      auto outcomes = help_func::split(sub_game, ",");

      for (auto outcome : outcomes) {
        help_func::trim(outcome);
        auto split_outcome = help_func::split(outcome, " ");
        std::string key = split_outcome[1];
        int value = std::stoi(split_outcome[0]);

        if (value > boundaries[key]) {
          game_possible = false;
        }
      }
    }
    if (game_possible) {
      sum += std::stoi(game_num);
    }
  }
  return sum;
}

[[nodiscard]] int
problem_2(std::vector<std::string> const& input)
{
  std::unordered_map<std::string, int> smallest_occurence;
  int sum{};

  for (std::string const& s : input) {
    auto split_vec = help_func::split(s, ": ");

    smallest_occurence["red"] = 0;
    smallest_occurence["green"] = 0;
    smallest_occurence["blue"] = 0;

    std::string game_string = split_vec[0];
    std::size_t from{ s.find(" ") };
    std::size_t to{ s.find(":") };
    std::string game_num = game_string.substr(from + 1, to - from - 1);

    std::string shown_string = split_vec[1];
    auto sub_games = help_func::split(shown_string, ";");

    for (auto sub_game : sub_games) {

      help_func::trim(sub_game);
      auto outcomes = help_func::split(sub_game, ",");

      for (auto outcome : outcomes) {
        help_func::trim(outcome);
        auto split_outcome = help_func::split(outcome, " ");
        std::string key = split_outcome[1];
        int value = std::stoi(split_outcome[0]);

        if (value > smallest_occurence[key]) {
          smallest_occurence[key] = value;
        }
      }
    }
    sum += smallest_occurence["red"] * smallest_occurence["green"] *
           smallest_occurence["blue"];
  }
  return sum;
}

int
main()
{
  help_func::Timer t;
  std::vector<std::string> input =
    help_func::read_input("../data/input-day-2.txt");

  int answer_1 = problem_1(input);
  int answer_2 = problem_2(input);

  std::cout << "Day 2:" << answer_1 << ", " << answer_2 << " ";
}