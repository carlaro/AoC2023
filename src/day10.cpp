#include "help_func.hpp"
#include <algorithm>
#include <atomic>
#include <charconv>
#include <cmath>
#include <deque>
#include <iostream>
#include <limits>
#include <mutex>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

std::atomic<std::size_t> X;
std::atomic<std::size_t> Y;
std::atomic<bool> loc_found{ false };

std::size_t t_sum_1;
// std::mutex t_sum_1_mutex;

std::size_t t_sum_2;
// std::mutex t_sum_2_mutex;

void
find_start_loc(const std::vector<std::string>& input)
{
  for (std::size_t i = 0; i < input.size(); ++i) {
    std::size_t j = input[i].find('S');
    if (j < std::string::npos) {
      X.store(i);
      Y.store(j);
      loc_found.store(true);
      break;
    }
  }
}

inline bool
expand_up(const std::pair<std::size_t, std::size_t>& loc,
          const std::vector<std::string>& input)
{
  if (loc.first == 0) {
    return false;
  }
  return input[loc.first - 1][loc.second] == 'F' ||
         input[loc.first - 1][loc.second] == '7' ||
         input[loc.first - 1][loc.second] == '|';
}

inline bool
expand_right(const std::pair<std::size_t, std::size_t>& loc,
             const std::vector<std::string>& input)
{
  if (loc.second + 1 == input[loc.first].size()) {
    return false;
  }
  return input[loc.first][loc.second + 1] == 'J' ||
         input[loc.first][loc.second + 1] == '7' ||
         input[loc.first][loc.second + 1] == '-';
}

inline bool
expand_down(const std::pair<std::size_t, std::size_t>& loc,
            const std::vector<std::string>& input)
{
  if (loc.first + 1 == input.size()) {
    return false;
  }
  return input[loc.first + 1][loc.second] == 'J' ||
         input[loc.first + 1][loc.second] == 'L' ||
         input[loc.first + 1][loc.second] == '|';
}

inline bool
expand_left(const std::pair<std::size_t, std::size_t>& loc,
            const std::vector<std::string>& input)
{
  if (loc.second == 0) {
    return false;
  }
  return input[loc.first][loc.second - 1] == 'F' ||
         input[loc.first][loc.second - 1] == 'L' ||
         input[loc.first][loc.second - 1] == '-';
}

void
problem_1(const std::vector<std::string>& input)
{
  while (!loc_found.load(std::memory_order_relaxed)) {
    __builtin_ia32_pause();
  }
  std::pair<std::size_t, std::size_t> start_loc{
    X.load(std::memory_order_relaxed), Y.load(std::memory_order_relaxed)
  };

  std::deque<std::pair<std::pair<std::size_t, std::size_t>, std::size_t>> queue;
  std::size_t distance{};
  std::size_t max_distance{};

  bool visited[input.size()][input[0].size()]{};

  queue.emplace_back(start_loc, 0);
  while (!queue.empty()) {
    std::pair<std::size_t, std::size_t> current_elem{ queue.front().first };
    distance = queue.front().second;
    queue.pop_front();

    if (visited[current_elem.first][current_elem.second]) {
      continue;
    }

    if (distance > max_distance) {
      max_distance = distance;
    }

    switch (input[current_elem.first][current_elem.second]) {
      case 'S':
        if (expand_up(current_elem, input)) {
          queue.emplace_back(
            std::make_pair(current_elem.first - 1, current_elem.second),
            distance + 1);
        }

        if (expand_right(current_elem, input)) {
          queue.emplace_back(
            std::make_pair(current_elem.first, current_elem.second + 1),
            distance + 1);
        }

        if (expand_down(current_elem, input)) {
          queue.emplace_back(
            std::make_pair(current_elem.first + 1, current_elem.second),
            distance + 1);
        }

        if (expand_left(current_elem, input)) {
          queue.emplace_back(
            std::make_pair(current_elem.first, current_elem.second - 1),
            distance + 1);
        }
        break;
      case '|':
        queue.emplace_back(
          std::make_pair(current_elem.first - 1, current_elem.second),
          distance + 1);
        queue.emplace_back(
          std::make_pair(current_elem.first + 1, current_elem.second),
          distance + 1);
        break;
      case '-':
        queue.emplace_back(
          std::make_pair(current_elem.first, current_elem.second + 1),
          distance + 1);
        queue.emplace_back(
          std::make_pair(current_elem.first, current_elem.second - 1),
          distance + 1);
        break;
      case 'F':
        queue.emplace_back(
          std::make_pair(current_elem.first + 1, current_elem.second),
          distance + 1);
        queue.emplace_back(
          std::make_pair(current_elem.first, current_elem.second + 1),
          distance + 1);
        break;
      case '7':
        queue.emplace_back(
          std::make_pair(current_elem.first + 1, current_elem.second),
          distance + 1);
        queue.emplace_back(
          std::make_pair(current_elem.first, current_elem.second - 1),
          distance + 1);
        break;
      case 'L':
        queue.emplace_back(
          std::make_pair(current_elem.first - 1, current_elem.second),
          distance + 1);
        queue.emplace_back(
          std::make_pair(current_elem.first, current_elem.second + 1),
          distance + 1);
        break;
      case 'J':
        queue.emplace_back(
          std::make_pair(current_elem.first, current_elem.second - 1),
          distance + 1);
        queue.emplace_back(
          std::make_pair(current_elem.first - 1, current_elem.second),
          distance + 1);
        break;
      default:
        break;
    }

    visited[current_elem.first][current_elem.second] = true;
  }
  t_sum_1 = max_distance;
}

inline bool
vert_crossing(const char& c)
{
  return '7' == c || 'F' == c || '|' == c;
}

void
problem_2(const std::vector<std::string>& input)
{
  while (!loc_found.load(std::memory_order_relaxed)) {
    __builtin_ia32_pause();
  }
  std::pair<std::size_t, std::size_t> start_loc{
    X.load(std::memory_order_relaxed), Y.load(std::memory_order_relaxed)
  };

  std::deque<std::pair<std::size_t, std::size_t>> queue;

  bool visited[input.size()][input[0].size()]{};

  queue.emplace_back(start_loc);
  while (!queue.empty()) {
    std::pair<std::size_t, std::size_t> current_elem{ queue.front() };
    queue.pop_front();

    if (visited[current_elem.first][current_elem.second]) {
      continue;
    }

    switch (input[current_elem.first][current_elem.second]) {
      case 'S':
        if (expand_up(current_elem, input)) {
          queue.emplace_back(current_elem.first - 1, current_elem.second);
        }

        if (expand_right(current_elem, input)) {
          queue.emplace_back(current_elem.first, current_elem.second + 1);
        }

        if (expand_down(current_elem, input)) {
          queue.emplace_back(current_elem.first + 1, current_elem.second);
        }

        if (expand_left(current_elem, input)) {
          queue.emplace_back(current_elem.first, current_elem.second - 1);
        }
        break;
      case '|':
        queue.emplace_back(current_elem.first - 1, current_elem.second);
        queue.emplace_back(current_elem.first + 1, current_elem.second);
        break;
      case '-':
        queue.emplace_back(current_elem.first, current_elem.second + 1);
        queue.emplace_back(current_elem.first, current_elem.second - 1);
        break;
      case 'F':
        queue.emplace_back(current_elem.first + 1, current_elem.second);
        queue.emplace_back(current_elem.first, current_elem.second + 1);
        break;
      case '7':
        queue.emplace_back(current_elem.first + 1, current_elem.second);
        queue.emplace_back(current_elem.first, current_elem.second - 1);
        break;
      case 'L':
        queue.emplace_back(current_elem.first - 1, current_elem.second);
        queue.emplace_back(current_elem.first, current_elem.second + 1);
        break;
      case 'J':
        queue.emplace_back(current_elem.first, current_elem.second - 1);
        queue.emplace_back(current_elem.first - 1, current_elem.second);
        break;
      default:
        break;
    }

    visited[current_elem.first][current_elem.second] = true;
  }

  std::size_t parity_map[input.size()][input[0].size()]{};
  for (size_t i = 0; i < input.size(); ++i) {
    std::size_t parity{};
    for (size_t j = 0; j < input[0].size(); ++j) {
      if (visited[i][j]) {
        if (vert_crossing(input[i][j])) {
          parity += 1;
        }
      } else {
        parity_map[i][j] = parity;
      }
    }
  }

  std::size_t enclosed_tiles{};

  for (size_t i = 0; i < input.size(); ++i) {
    for (size_t j = 0; j < input[0].size(); ++j) {
      if (parity_map[i][j] % 2 == 1) {
        ++enclosed_tiles;
        // std::cout << "x";
      }
      // else {

      //     std::cout << ".";
      // }
    }
    // std::cout << "\n";
  }

  t_sum_2 = enclosed_tiles;
}

int
main()
{
  help_func::Timer t;
  std::vector<std::string> input =
    help_func::read_input("../data/input-day-10.txt");
  // std::vector<std::string> input =help_func::read_input("small_data.txt");

  // std::size_t n_t{20};
  std::thread t0(find_start_loc, std::ref(input));
  t0.detach();

  std::thread t1(problem_1, std::ref(input));
  std::thread t2(problem_2, std::ref(input));

  t1.join();
  t2.join();

  std::cout << "Day 10:" << t_sum_1 << ", " << t_sum_2 << " ";
}