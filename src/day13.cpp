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
#include <mutex>
#include <atomic>
#include <deque>
#include <iterator>
#include "help_func.hpp"

std::uint64_t answer_1{};
std::mutex answer_1_mutex;

std::uint64_t answer_2{};
std::mutex answer_2_mutex;

std::mutex input_mutex;

template<typename T>
class input_queue {
    const std::vector<T> &m_input;
    std::mutex& m_mutex;
    std::vector<T>::const_iterator m_it;
    std::vector<T>::const_iterator m_end;
    bool m_ready{true};
    const size_t m_num;
    size_t m_lines_read{};

    public:
        input_queue(const std::vector<T> &i, std::mutex& mutex) : m_input(i), m_mutex(mutex), m_it(m_input.begin()), m_end(m_input.end()), m_num(m_input.size()) {}

        T get() {
            std::lock_guard<std::mutex> lg(m_mutex);
            if(m_it == m_end) {
                m_ready = false;
                return T{};
            }
            return *m_it++;
        }

        bool ready() {
            return m_ready;
        }
};

std::uint64_t find_reflected_rows(std::vector<std::string>& pattern) {
    std::string current_line;
    size_t pattern_width = pattern[0].size(); 
    for(size_t i = 0; i < pattern_width; ++i) {
        current_line.push_back(pattern[0][i]);
        size_t upper_bound = (current_line.size() > pattern_width) ? pattern_width : current_line.size();
        for(size_t j = 0; j < upper_bound; ++j) {
            if(current_line[current_line.size() - j - 1] == pattern[0][i + j]) {

            }
        }
    }
    return 0;
}

void problem_1_thread(input_queue<std::vector<std::string>>& iq)
{
    std::uint64_t local_sum{};

    while(iq.ready())
    {
        std::vector<std::string> pattern = iq.get();
        if(pattern.empty()) {
            continue;
        }
        std::uint64_t answer = find_reflected_rows(pattern);
        local_sum += answer;
    }
    {
        std::lock_guard<std::mutex> lg(answer_1_mutex);
        answer_1 += local_sum;
    }
}

// void problem_2_thread(input_queue& iq)
// {
// }

void problem_1(const std::vector<std::vector<std::string>>& input, std::size_t n_threads) {
    input_queue<std::vector<std::string>> iq(input, input_mutex);
    std::vector<std::thread> thread_vec;
    for(std::size_t i = 0; i < n_threads; ++i) {
        thread_vec.emplace_back(problem_1_thread, std::ref(iq));
    }
    for(auto &t : thread_vec) {
        t.join();
    }
}

// void problem_2(const std::vector<std::string> &input, std::size_t n_threads)
// {
//     input_queue iq(input, input_mutex);
//     std::vector<std::thread> thread_vec;
//     for(std::size_t i = 0; i < n_threads; ++i) {
//         thread_vec.emplace_back(problem_2_thread, std::ref(iq));
//     }
//     for(auto &t : thread_vec) {
//         t.join();
//     }
// }

int main()
{
    help_func::Timer t;
    std::vector<std::string> lines = help_func::read_input("../data/input-day-13.txt");

    std::vector<std::vector<std::string>> patterns;

    std::vector<std::string> pattern;
    auto it = lines.begin();
    while(it != lines.end()) {
        if((*it).empty()) {
            patterns.push_back(pattern);
            pattern.clear();
        }
        pattern.push_back(*it);
        ++it;
    }
    patterns.push_back(pattern);

    std::size_t n_t{1};
    std::thread t1(problem_1, std::ref(patterns), n_t);
    // std::thread t2(problem_2, std::ref(input), n_t);
    t1.join();
    // t2.join();

    std::cout << "Day 13:" << answer_1 << ", " << answer_2 << " ";
}