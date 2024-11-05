#include "help_func.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <iostream>
#include <limits>
#include <mutex>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

std::int64_t t_sum_1;
std::mutex t_sum_1_mutex;

std::int64_t t_sum_2;
std::mutex t_sum_2_mutex;

bool
all_zero(std::vector<std::int64_t>& vec)
{
  bool all_zeroes = true;
  for (auto i : vec) {
    if (0 != i) {
      all_zeroes = false;
      break;
    }
  }
  return all_zeroes;
}

void
problem_1_thread(const std::vector<std::string>& input,
                 const std::size_t n_row,
                 const std::size_t n_threads,
                 const std::size_t index)
{

  std::int64_t sum{};
  std::size_t upper_bound =
    (index == n_threads - 1) ? input.size() : (index + 1) * n_row;

  std::vector<std::vector<std::int64_t>> vector_stack;
  std::vector<std::int64_t> base_sequence;
  for (std::size_t i = index * n_row; i < upper_bound; ++i) {
    if (input[i].empty()) {
      continue;
    }

    std::vector<std::string> split_string = help_func::split(input[i], " ");

    base_sequence.clear();
    base_sequence.reserve(split_string.size());

    for (auto& s : split_string) {
      base_sequence.push_back(std::stoi(s));
    }
    vector_stack.clear();
    vector_stack.reserve(base_sequence.size() * base_sequence.size() * 0.5);
    vector_stack.push_back(std::move(base_sequence));

    std::size_t stack_ind{};
    while (!all_zero(vector_stack[stack_ind])) {
      vector_stack.emplace_back();
      std::vector<std::int64_t>& curr_vec = vector_stack[stack_ind];
      vector_stack[stack_ind + 1].reserve(curr_vec.size() - 1);
      for (std::size_t i = 1; i < curr_vec.size(); ++i) {
        vector_stack[stack_ind + 1].push_back(curr_vec[i] - curr_vec[i - 1]);
      }
      ++stack_ind;
    }

    for (auto& vec : vector_stack) {
      sum += vec.back();
    }
  }
  {
    std::lock_guard<std::mutex> lock(t_sum_1_mutex);
    t_sum_1 += sum;
  }
}

void
problem_1(const std::vector<std::string>& input, const std::size_t n_threads)
{

  std::size_t n_rows_per_thread = std::ceil(input.size() / n_threads);
  std::vector<std::thread> thread_vec;

  for (std::size_t i = 0; i < n_threads; ++i) {
    thread_vec.emplace_back(
      problem_1_thread, std::ref(input), n_rows_per_thread, n_threads, i);
  }

  for (auto& t : thread_vec) {
    t.join();
  }
}

void
problem_2_thread(const std::vector<std::string>& input,
                 const std::size_t n_row,
                 const std::size_t n_threads,
                 const std::size_t index)
{
  std::int64_t val{};
  std::size_t upper_bound =
    (index == n_threads - 1) ? input.size() : (index + 1) * n_row;

  std::vector<std::vector<std::int64_t>> vector_stack;
  std::vector<std::int64_t> base_sequence;
  for (std::size_t i = index * n_row; i < upper_bound; ++i) {
    if (input[i].empty()) {
      continue;
    }

    std::vector<std::string> split_string = help_func::split(input[i], " ");

    base_sequence.clear();
    base_sequence.reserve(split_string.size());

    for (auto& s : split_string) {
      base_sequence.push_back(std::stoi(s));
    }
    vector_stack.clear();
    vector_stack.reserve(base_sequence.size() * base_sequence.size() * 0.5);
    vector_stack.push_back(std::move(base_sequence));

    std::size_t stack_ind{};
    while (!all_zero(vector_stack[stack_ind])) {
      vector_stack.emplace_back();
      std::vector<std::int64_t>& curr_vec = vector_stack[stack_ind];
      vector_stack[stack_ind + 1].reserve(curr_vec.size() - 1);
      for (std::size_t i = 1; i < curr_vec.size(); ++i) {
        vector_stack[stack_ind + 1].push_back(curr_vec[i] - curr_vec[i - 1]);
      }
      ++stack_ind;
    }
    std::int64_t lval{};
    for (auto& vec : vector_stack | std::views::reverse) {

      lval = vec.front() - lval;
    }
    val += lval;
  }
  {
    std::lock_guard<std::mutex> lock(t_sum_2_mutex);
    t_sum_2 += val;
  }
}

void
problem_2(const std::vector<std::string>& input, std::size_t n_threads)
{
  std::size_t n_rows_per_thread = std::ceil(input.size() / n_threads);
  std::vector<std::thread> thread_vec;

  for (std::size_t i = 0; i < n_threads; ++i) {
    thread_vec.emplace_back(
      problem_2_thread, std::ref(input), n_rows_per_thread, n_threads, i);
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
    help_func::read_input("../data/input-day-9.txt");
  // std::vector<std::string> input =help_func::read_input("small_data.txt");

  std::size_t n_t{ 20 };

  std::thread t1(problem_1, std::ref(input), n_t);
  std::thread t2(problem_2, std::ref(input), n_t);

  t1.join();
  t2.join();

  std::cout << "Day 9:" << t_sum_1 << ", " << t_sum_2 << " ";
}