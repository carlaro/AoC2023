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

std::uint64_t t_sum_1;
std::mutex t_sum_1_mutex;

std::uint64_t t_sum_2;
std::mutex t_sum_2_mutex;

std::size_t
calc_distance(const std::pair<std::size_t, std::size_t>& g1,
              const std::pair<std::size_t, std::size_t>& g2,
              const std::vector<bool>& cols_ptr,
              const std::vector<bool>& rows_ptr)
{
  std::uint64_t low_x = (g1.first < g2.first) ? g1.first : g2.first;
  std::uint64_t high_x = (g1.first > g2.first) ? g1.first : g2.first;

  std::uint64_t low_y = (g1.second < g2.second) ? g1.second : g2.second;
  std::uint64_t high_y = (g1.second > g2.second) ? g1.second : g2.second;

  std::uint64_t open_cols{};
  for (std::size_t i = low_y + 1; i < high_y; ++i) {
    if (!cols_ptr[i]) {
      ++open_cols;
    }
  }

  std::uint64_t open_rows{};
  for (std::size_t i = low_x + 1; i < high_x; ++i) {
    if (!rows_ptr[i]) {
      ++open_rows;
    }
  }

  std::uint64_t distance =
    high_y - low_y + high_x - low_x + open_cols + open_rows;

  return distance;
}

std::size_t
calc_distance_2(const std::pair<std::size_t, std::size_t>& g1,
                const std::pair<std::size_t, std::size_t>& g2,
                const std::vector<bool>& cols_ptr,
                const std::vector<bool>& rows_ptr)
{
  std::uint64_t low_x = (g1.first < g2.first) ? g1.first : g2.first;
  std::uint64_t high_x = (g1.first > g2.first) ? g1.first : g2.first;

  std::uint64_t low_y = (g1.second < g2.second) ? g1.second : g2.second;
  std::uint64_t high_y = (g1.second > g2.second) ? g1.second : g2.second;

  std::uint64_t open_cols{};
  for (std::size_t i = low_y + 1; i < high_y; ++i) {
    if (!cols_ptr[i]) {
      ++open_cols;
    }
  }

  std::uint64_t open_rows{};
  for (std::size_t i = low_x + 1; i < high_x; ++i) {
    if (!rows_ptr[i]) {
      ++open_rows;
    }
  }

  std::uint64_t distance =
    high_y - low_y + high_x - low_x + open_cols * 999999 + open_rows * 999999;

  return distance;
}

void
calculation_thread(
  const std::vector<std::pair<std::size_t, std::size_t>>& g_vec,
  const std::vector<std::pair<std::size_t, std::size_t>>& i_vec,
  std::size_t index,
  std::size_t n_threads,
  std::size_t d_size,
  const std::vector<bool>& cols_ptr,
  const std::vector<bool>& rows_ptr)
{

  std::size_t upper_bound =
    (index + 1) * d_size > i_vec.size() ? i_vec.size() : (index + 1) * d_size;

  std::size_t local_sum{};
  for (std::size_t i = index * d_size; i < upper_bound; ++i) {
    local_sum += calc_distance(
      g_vec[i_vec[i].first], g_vec[i_vec[i].second], cols_ptr, rows_ptr);
  }
  {
    std::lock_guard<std::mutex> lg(t_sum_1_mutex);
    // std::cout << "My index is " << index << " and I processed the pairs
    // between " << index*d_size << " and " << upper_bound << "\n";
    t_sum_1 += local_sum;
  }
}

void
calculation_thread_2(
  const std::vector<std::pair<std::size_t, std::size_t>>& g_vec,
  const std::vector<std::pair<std::size_t, std::size_t>>& i_vec,
  std::size_t index,
  std::size_t n_threads,
  std::size_t d_size,
  const std::vector<bool>& cols_ptr,
  const std::vector<bool>& rows_ptr)
{

  std::size_t upper_bound =
    (index + 1) * d_size > i_vec.size() ? i_vec.size() : (index + 1) * d_size;

  std::uint64_t local_sum{};
  for (std::size_t i = index * d_size; i < upper_bound; ++i) {
    local_sum += calc_distance_2(
      g_vec[i_vec[i].first], g_vec[i_vec[i].second], cols_ptr, rows_ptr);
  }
  {
    std::lock_guard<std::mutex> lg(t_sum_2_mutex);
    // std::cout << "My index is " << index << " and I processed the pairs
    // between " << index*d_size << " and " << upper_bound << "\n";
    t_sum_2 += local_sum;
  }
}

void
problem_1(const std::vector<std::string>& input, std::size_t n_threads)
{
  std::vector<std::pair<std::uint64_t, std::uint64_t>> galaxy_coords;

  std::vector<bool> occupied_cols(input[0].size(), {});
  std::vector<bool> occupied_rows(input.size(), {});

  std::uint64_t row_index{};
  for (const std::string& s : input) {
    std::uint64_t galaxay_location = s.find('#');
    while (galaxay_location != std::string::npos) {
      galaxy_coords.emplace_back(row_index, galaxay_location);
      occupied_cols[galaxay_location] = true;
      occupied_rows[row_index] = true;
      galaxay_location = s.find('#', galaxay_location + 1);
    }
    ++row_index;
  }

  std::vector<std::pair<std::size_t, std::size_t>> pair_vec;
  pair_vec.reserve(galaxy_coords.size() * galaxy_coords.size() / 2);

  for (std::size_t i = 0; i < galaxy_coords.size(); ++i) {
    for (std::size_t j = i + 1; j < galaxy_coords.size(); ++j) {
      pair_vec.emplace_back(i, j);
    }
  }

  std::vector<std::thread> thread_vec;
  std::size_t n_data_points = std::floor(pair_vec.size() / n_threads) + 1;

  for (std::size_t i = 0; i < n_threads; ++i) {
    thread_vec.emplace_back(calculation_thread,
                            std::ref(galaxy_coords),
                            std::ref(pair_vec),
                            i,
                            n_threads,
                            n_data_points,
                            std::ref(occupied_cols),
                            std::ref(occupied_rows));
  }

  for (auto& t : thread_vec) {
    t.join();
  }
}

void
problem_2(const std::vector<std::string>& input, std::size_t n_threads)
{
  std::vector<std::pair<std::uint64_t, std::uint64_t>> galaxy_coords;

  std::vector<bool> occupied_cols(input[0].size(), {});
  std::vector<bool> occupied_rows(input.size(), {});

  std::uint64_t row_index{};
  for (const std::string& s : input) {
    std::uint64_t galaxay_location = s.find('#');
    while (galaxay_location != std::string::npos) {
      galaxy_coords.emplace_back(row_index, galaxay_location);
      occupied_cols[galaxay_location] = true;
      occupied_rows[row_index] = true;
      galaxay_location = s.find('#', galaxay_location + 1);
    }
    ++row_index;
  }

  std::vector<std::pair<std::size_t, std::size_t>> pair_vec;
  pair_vec.reserve(galaxy_coords.size() * galaxy_coords.size() / 2);

  for (std::size_t i = 0; i < galaxy_coords.size(); ++i) {
    for (std::size_t j = i + 1; j < galaxy_coords.size(); ++j) {
      pair_vec.emplace_back(i, j);
    }
  }

  std::vector<std::thread> thread_vec;
  std::size_t n_data_points = std::floor(pair_vec.size() / n_threads) + 1;

  for (std::size_t i = 0; i < n_threads; ++i) {
    thread_vec.emplace_back(calculation_thread_2,
                            std::ref(galaxy_coords),
                            std::ref(pair_vec),
                            i,
                            n_threads,
                            n_data_points,
                            std::ref(occupied_cols),
                            std::ref(occupied_rows));
  }

  for (auto& t : thread_vec) {
    t.join();
  }
}

int
main()
{
  help_func::Timer t;
  std::vector<std::string> input =
    help_func::read_input("../data/input-day-11.txt");
  // std::vector<std::string> input =help_func::read_input("small_data.txt");

  std::size_t n_t{ 8 };

  std::thread t1(problem_1, std::ref(input), n_t);
  std::thread t2(problem_2, std::ref(input), n_t);

  t1.join();
  t2.join();

  std::cout << "Day 11:" << t_sum_1 << ", " << t_sum_2 << " ";
}