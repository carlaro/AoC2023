#include "help_func.hpp"
#include <algorithm>
#include <atomic>
#include <charconv>
#include <cmath>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <mutex>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

std::uint64_t answer_1{};
std::mutex answer_1_mutex;

std::uint64_t answer_2{};
std::mutex answer_2_mutex;

std::mutex input_mutex;

class input_queue
{
  const std::vector<std::string>& m_input;
  std::mutex& m_mutex;
  std::vector<std::string>::const_iterator m_it;
  std::vector<std::string>::const_iterator m_end;
  bool m_ready{ true };
  const size_t m_num;
  size_t m_lines_read{};

public:
  input_queue(const std::vector<std::string>& i, std::mutex& mutex)
    : m_input(i)
    , m_mutex(mutex)
    , m_it(m_input.begin())
    , m_end(m_input.end())
    , m_num(m_input.size())
  {
  }

  std::string get()
  {
    std::lock_guard<std::mutex> lg(m_mutex);
    if (m_it == m_end) {
      m_ready = false;
      return "";
    }
    return *m_it++;
  }

  bool ready() { return m_ready; }
};

std::uint64_t
possible_assignment(const std::string& s,
                    std::vector<int> t,
                    size_t t_ind,
                    size_t s_ind,
                    int count,
                    int blocks_started)
{
  while (s_ind < s.size()) {
    if (s[s_ind] == '.') {
      if (count == 0) {
        ++s_ind;
        continue;
      }
      if (count != t[t_ind]) {
        return 0;
      }
      ++t_ind;
      count = 0;
      ++s_ind;
      continue;
    }
    if (s[s_ind] == '#') {
      if (count == 0) {
        ++blocks_started;
      }
      if (count < t[t_ind]) {
        ++count;
        ++s_ind;
        continue;
      }
      return 0;
    }

    if (s[s_ind] == '?') {
      std::string s1 = std::string(s);
      s1[s_ind] = '#';
      std::uint64_t ans_left =
        possible_assignment(s1, t, t_ind, s_ind, count, blocks_started);
      std::string s2 = std::string(s);
      s2[s_ind] = '.';
      std::uint64_t ans_right =
        possible_assignment(s2, t, t_ind, s_ind, count, blocks_started);

      return ans_left + ans_right;
    }
  }
  if (t_ind < t.size()) {
    if (count != t[t_ind] || t_ind != t.size() - 1) {
      return 0;
    }
  }
  if (blocks_started > t.size()) {
    return 0;
  }
  return 1;
}

bool
check_if_group_fits(const std::string& record, int group, size_t from)
{
  if (from + group > record.size()) {
    return false;
  }
  for (size_t i = 0; i < group; ++i) {
    if (record[from + i] == '.') {
      return false;
    }
  }
  if (from + group == record.size()) {
    return true;
  }
  if (record[from + group] != '#') {
    return true;
  }
  return false;
}

std::uint64_t
possible_assignment_2(const std::string& record,
                      std::vector<int> groups,
                      std::unordered_map<std::string, std::uint64_t>& hashmap)
{
  std::string key = record;
  for (auto c : groups) {
    key += c;
  }

  if (hashmap.contains(key)) {
    return hashmap[key];
  }

  if (groups.empty()) {
    for (auto c : record) {
      if (c == '#') {
        return 0;
      }
    }
    return 1;
  }
  size_t springs_required = groups.size() - 1;

  for (auto& group : groups) {
    springs_required += group;
  }

  if (springs_required > record.size()) {
    return 0;
  }

  size_t record_index = 0;
  int group = groups[0];

  while (record[record_index] == '.') {
    ++record_index;
  }

  if (record.size() <= record_index) {
    return 0;
  }

  if (record[record_index] == '#') {
    bool fits = check_if_group_fits(record, group, record_index);
    if (fits) {
      if (record.size() == group + record_index) {
        if (groups.size() == 1) {
          return 1;
        }
        return 0;
      }
      std::string remaining_record = record.substr(record_index + group + 1);
      std::vector<int> remaining_groups =
        std::vector<int>(groups.begin() + 1, groups.end());
      std::uint64_t lval =
        possible_assignment_2(remaining_record, remaining_groups, hashmap);
      hashmap[key] = lval;
      return possible_assignment_2(remaining_record, remaining_groups, hashmap);
    }
    return 0;
  }

  if (record[record_index] == '?') {
    std::string s1 = record.substr(record_index);
    s1[0] = '#';
    std::uint64_t ans_left = possible_assignment_2(s1, groups, hashmap);
    std::string s2 = record.substr(record_index + 1);
    std::uint64_t ans_right = possible_assignment_2(s2, groups, hashmap);
    return ans_left + ans_right;
  }
  return 0;
}

void
problem_1_thread(input_queue& iq)
{
  std::uint64_t local_sum{};

  while (iq.ready()) {
    std::string s = iq.get();
    if (s.empty()) {
      continue;
    }

    auto string_vec = help_func::split(s, " ");
    auto temp_vec = help_func::split(string_vec[1], ",");
    std::vector<int> connected_broken_springs;
    for (auto& num : temp_vec) {
      connected_broken_springs.push_back(std::stoi(num));
    }

    std::uint64_t answer =
      possible_assignment(string_vec[0], connected_broken_springs, 0, 0, 0, 0);
    local_sum += answer;
  }
  {
    std::lock_guard<std::mutex> lg(answer_1_mutex);
    answer_1 += local_sum;
  }
}

void
problem_2_thread(input_queue& iq)
{
  std::uint64_t local_sum{};
  std::unordered_map<std::string, std::uint64_t> local_map;

  size_t line_count{};
  while (iq.ready()) {
    std::string s = iq.get();
    if (s.empty()) {
      continue;
    }

    auto string_vec = help_func::split(s, " ");
    std::string repeated_string =
      help_func::repeat_string(string_vec[0], 5, "?");
    std::string int_string = help_func::repeat_string(string_vec[1], 5, ",");
    auto temp_vec = help_func::split(int_string, ",");
    std::vector<int> connected_broken_springs;
    for (auto& num : temp_vec) {
      connected_broken_springs.push_back(std::stoi(num));
    }
    std::uint64_t score = possible_assignment_2(
      repeated_string, connected_broken_springs, local_map);
    local_sum += score;
    ++line_count;
  }
  {
    std::lock_guard<std::mutex> lg(answer_2_mutex);
    answer_2 += local_sum;
  }
}

void
problem_1(const std::vector<std::string>& input, std::size_t n_threads)
{
  input_queue iq(input, input_mutex);
  std::vector<std::thread> thread_vec;
  for (std::size_t i = 0; i < n_threads; ++i) {
    thread_vec.emplace_back(problem_1_thread, std::ref(iq));
  }
  for (auto& t : thread_vec) {
    t.join();
  }
}

void
problem_2(const std::vector<std::string>& input, std::size_t n_threads)
{
  input_queue iq(input, input_mutex);
  std::vector<std::thread> thread_vec;
  for (std::size_t i = 0; i < n_threads; ++i) {
    thread_vec.emplace_back(problem_2_thread, std::ref(iq));
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
    help_func::read_input("../data/input-day-12.txt");

  std::size_t n_t{ 16 };
  std::thread t1(problem_1, std::ref(input), n_t);
  std::thread t2(problem_2, std::ref(input), n_t);
  t1.join();
  t2.join();

  std::cout << "Day 12:" << answer_1 << ", " << answer_2 << " ";
}