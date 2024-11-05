#include "help_func.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <iostream>
#include <limits>
#include <ranges>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

std::unordered_map<char, int> p1_map{ { '2', 2 },  { '3', 3 },  { '4', 4 },
                                      { '5', 5 },  { '6', 6 },  { '7', 7 },
                                      { '8', 8 },  { '9', 9 },  { 'T', 10 },
                                      { 'J', 11 }, { 'Q', 12 }, { 'K', 13 },
                                      { 'A', 14 } };

std::unordered_map<char, int> p2_map{ { '2', 2 }, { '3', 3 },  { '4', 4 },
                                      { '5', 5 }, { '6', 6 },  { '7', 7 },
                                      { '8', 8 }, { '9', 9 },  { 'T', 10 },
                                      { 'J', 1 }, { 'Q', 12 }, { 'K', 13 },
                                      { 'A', 14 } };

std::unordered_map<char, std::uint64_t> h2_map{};
std::unordered_map<char, std::uint64_t> h1_map{};

bool
compare_hand(std::pair<std::string, std::uint64_t>& h1,
             std::pair<std::string, std::uint64_t>& h2)
{

  h1_map.clear();
  h2_map.clear();

  for (const char& c : h1.first) {
    if (h1_map.contains(c)) {
      ++h1_map[c];
    } else {
      h1_map[c] = 1;
    }
  }

  for (const char& c : h2.first) {
    if (h2_map.contains(c)) {
      ++h2_map[c];
    } else {
      h2_map[c] = 1;
    }
  }

  if (h1_map.size() < h2_map.size()) {
    return false;
  }
  if (h1_map.size() > h2_map.size()) {
    return true;
  }

  std::pair<char, std::uint64_t> max_p_h1{};
  std::pair<char, std::uint64_t> max_p_h2{};
  for (auto p : h1_map) {
    if (p.second > max_p_h1.second) {
      max_p_h1 = p;
    }
  }
  for (auto p : h2_map) {
    if (p.second > max_p_h2.second) {
      max_p_h2 = p;
    }
  }

  if (max_p_h1.second > max_p_h2.second) {
    return false;
  }
  if (max_p_h1.second < max_p_h2.second) {
    return true;
  }

  std::size_t i = 0;
  while (i < h1.first.size()) {
    if (p1_map[h1.first[i]] > p1_map[h2.first[i]]) {
      return false;
    }
    if (p1_map[h1.first[i]] < p1_map[h2.first[i]]) {
      return true;
    }
    ++i;
  }

  return false;
}

bool
compare_hand_2(std::pair<std::string, std::uint64_t>& h1,
               std::pair<std::string, std::uint64_t>& h2)
{
  h1_map.clear();
  h2_map.clear();

  std::uint64_t h1j{};
  for (const char& c : h1.first) {
    if ('J' == c) {
      ++h1j;
      continue;
    }

    if (h1_map.contains(c)) {
      ++h1_map[c];
    } else {
      h1_map[c] = 1;
    }
  }

  std::uint64_t h2j{};
  for (const char& c : h2.first) {
    if ('J' == c) {
      ++h2j;
      continue;
    }

    if (h2_map.contains(c)) {
      ++h2_map[c];
    } else {
      h2_map[c] = 1;
    }
  }

  if (5 == h1j) {
    if (1 == h2_map.size() && h2j > 0) {
      return true;
    }
  }
  if (5 == h2j) {
    if (1 == h1_map.size() && h1j > 0) {
      return false;
    }
  }

  if (h1_map.size() < h2_map.size()) {
    return false;
  }
  if (h1_map.size() > h2_map.size()) {
    return true;
  }

  std::pair<char, std::uint64_t> max_p_h1{};
  std::pair<char, std::uint64_t> max_p_h2{};

  for (auto p : h1_map) {
    if (p.second > max_p_h1.second) {
      max_p_h1 = p;
    }
  }
  for (auto p : h2_map) {
    if (p.second > max_p_h2.second) {
      max_p_h2 = p;
    }
  }

  if (max_p_h1.second + h1j > max_p_h2.second + h2j) {
    return false;
  }
  if (max_p_h1.second + h1j < max_p_h2.second + h2j) {
    return true;
  }

  std::size_t i = 0;
  while (i < h1.first.size()) {
    if (p2_map[h1.first[i]] > p2_map[h2.first[i]]) {
      return false;
    }
    if (p2_map[h1.first[i]] < p2_map[h2.first[i]]) {
      return true;
    }
    ++i;
  }

  return false;
}

[[nodiscard]] std::uint64_t
problem_1(std::vector<std::string>& input)
{

  std::vector<std::pair<std::string, std::uint64_t>> hand_vec{};

  for (std::string s : input) {
    auto hand_and_bid = help_func::split(s, " ");
    hand_vec.emplace_back(hand_and_bid[0], std::stoull(hand_and_bid[1]));
  }

  std::ranges::sort(hand_vec, compare_hand);

  std::uint64_t out_val{ 0 };
  std::uint64_t i{ 1 };
  for (auto p : hand_vec) {
    out_val += p.second * i;
    ++i;
  }

  return out_val;
}

[[nodiscard]] std::uint64_t
problem_2(std::vector<std::string>& input)
{
  std::vector<std::pair<std::string, std::uint64_t>> hand_vec{};

  for (std::string s : input) {
    auto hand_and_bid = help_func::split(s, " ");
    hand_vec.emplace_back(hand_and_bid[0], std::stoull(hand_and_bid[1]));
  }

  std::ranges::sort(hand_vec, compare_hand_2);

  std::uint64_t out_val{ 0 };
  std::uint64_t i{ 1 };
  for (auto p : hand_vec) {
    out_val += p.second * i;
    ++i;
  }

  return out_val;
}

int
main()
{
  help_func::Timer t;
  std::vector<std::string> input =
    help_func::read_input("../data/input-day-7.txt");
  // std::vector<std::string> input =help_func::read_input("small_data.txt");

  std::uint64_t answer_1 = problem_1(input);
  std::uint64_t answer_2 = problem_2(input);

  std::cout << "Day 7:" << answer_1 << ", " << answer_2 << " ";
}