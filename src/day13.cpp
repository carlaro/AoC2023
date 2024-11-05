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
#include <bitset>
#include "help_func.hpp"

const unsigned long long BITSET_LENGTH = 19;

std::uint64_t answer_1{};
std::mutex answer_1_mutex;

std::uint64_t answer_2{};
std::mutex answer_2_mutex;

std::mutex input_mutex;

template <typename T>
class input_queue
{
    const std::vector<T> &m_input;
    std::mutex &m_mutex;
    std::vector<T>::const_iterator m_it;
    std::vector<T>::const_iterator m_end;
    bool m_ready{true};
    const size_t m_num;
    size_t m_lines_read{};

public:
    input_queue(const std::vector<T> &i, std::mutex &mutex) : m_input(i), m_mutex(mutex), m_it(m_input.begin()), m_end(m_input.end()), m_num(m_input.size()) {}

    T get()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        if (m_it == m_end)
        {
            m_ready = false;
            return T{};
        }
        return *m_it++;
    }

    bool ready()
    {
        return m_ready;
    }
};

std::uint64_t find_reflected_rows(std::vector<std::string> &pattern)
{
    size_t pattern_width = pattern[0].size();
    std::deque<char> left;
    std::deque<char> right;
    std::bitset<BITSET_LENGTH> viable_horizontal_positions;
    viable_horizontal_positions.set();

    for (const auto &line : pattern)
    {
        for (const auto &c : line)
        {
            right.emplace_back(c);
        }

        std::bitset<BITSET_LENGTH> local_viable_horizontal_positions;
        size_t attempted_mirror_position{};
        while (attempted_mirror_position < BITSET_LENGTH)
        {
            left.emplace_back(right.front());
            right.pop_front();
            if (right.empty())
            {
                break;
            }
            if (!viable_horizontal_positions[attempted_mirror_position])
            {
                ++attempted_mirror_position;
                continue;
            }

            std::ranges::reverse_view left_reversed(left);
            bool position_failed = false;

            for (auto tuple : std::views::zip(left_reversed, right))
            {
                if (std::get<0>(tuple) != std::get<1>(tuple))
                {
                    position_failed = true;
                    break;
                }
            }
            if (!position_failed)
            {
                local_viable_horizontal_positions.flip(attempted_mirror_position);
            }
            ++attempted_mirror_position;
        }
        viable_horizontal_positions &= local_viable_horizontal_positions;
        left.clear();
    }
    if (viable_horizontal_positions.any())
    {
        size_t horizontal_result = std::countr_zero(viable_horizontal_positions.to_ulong()) + 1;
        return horizontal_result;
    }

    left.clear();
    right.clear();
    std::bitset<BITSET_LENGTH> viable_vertical_positions;
    viable_vertical_positions.set();

    for (size_t row_index = 0; row_index < pattern_width; ++row_index)
    {
        for (size_t col_index = 0; col_index < pattern.size(); ++col_index)
        {
            right.emplace_back(pattern[col_index][row_index]);
        }

        std::bitset<BITSET_LENGTH> local_viable_vertical_positions;
        size_t attempted_mirror_position{};
        while (attempted_mirror_position < pattern.size())
        {
            left.emplace_back(right.front());
            right.pop_front();
            if (right.empty())
            {
                break;
            }
            if (!viable_vertical_positions[attempted_mirror_position])
            {
                ++attempted_mirror_position;
                continue;
            }

            std::ranges::reverse_view left_reversed(left);
            bool position_failed = false;

            for (auto tuple : std::views::zip(left_reversed, right))
            {
                if (std::get<0>(tuple) != std::get<1>(tuple))
                {
                    position_failed = true;
                    break;
                }
            }
            if (!position_failed)
            {
                local_viable_vertical_positions.flip(attempted_mirror_position);
            }
            ++attempted_mirror_position;
        }
        viable_vertical_positions &= local_viable_vertical_positions;
        left.clear();
    }

    if (viable_vertical_positions.any())
    {
        size_t vertical_result = std::countr_zero(viable_vertical_positions.to_ulong()) + 1;
        return vertical_result * 100;
    }

    return 0;
}

void problem_1_thread(input_queue<std::vector<std::string>> &iq)
{
    std::uint64_t local_sum{};

    while (iq.ready())
    {
        std::vector<std::string> pattern = iq.get();
        if (pattern.empty())
        {
            continue;
        }
        std::uint64_t answer = find_reflected_rows(pattern);
        // std::cout << answer << "\n";
        local_sum += answer;
    }
    {
        std::lock_guard<std::mutex> lg(answer_1_mutex);
        answer_1 += local_sum;
    }
}

bool is_power_of_two(std::uint64_t num)
{
    return (num & (num - 1)) == 0;
}

bool check_horizontal(size_t row, size_t col, size_t steps, bool &smudge_fixed, std::vector<std::string> &pattern)
{
    std::uint64_t left{};
    std::uint64_t right{};
    for (size_t n = 1; n <= steps; ++n)
    {
        if (pattern[row][col + 1 - n] == '#')
        {
            left |= (1 << (n - 1));
        }
        if (pattern[row][col + n] == '#')
        {
            right |= (1 << (n - 1));
        }
    }
    std::uint64_t res = (left > right) ? left - right : right - left;
    // std::bitset<64> bm(res);
    // std::bitset<64> bm_right(right);
    // std::bitset<64> bm_left(left);

    // std::cout << pattern[row] << "\n";
    // std::cout << bm_right << "\n";
    // std::cout << bm_left << "\n";
    // std::cout << bm << "\n";

    if (res == 0)
    {
        // std::cout << "res == 0\n";
        return true;
    }
    if (!is_power_of_two(res))
    {
        // std::cout << res <<" is not a power of two or 0\n";
        return false;
    }
    if (smudge_fixed)
    {
        // std::cout << "smudge already fixed\n";
        return false;
    }
    if (!smudge_fixed)
    {
        smudge_fixed = true;
    }
    return true;
}

bool check_vertical(size_t row, size_t col, size_t steps, bool &smudge_fixed, std::vector<std::string> &pattern)
{
    std::uint64_t left{};
    std::uint64_t right{};
    for (size_t n = 1; n <= steps; ++n)
    {
        if (pattern[row + 1 - n][col] == '#')
        {
            left |= (1 << (n - 1));
        }
        if (pattern[row + n][col] == '#')
        {
            right |= (1 << (n - 1));
        }
    }
    std::uint64_t res = (left > right) ? left - right : right - left;
    // std::bitset<64> bm(res);
    // std::cout << bm << "\n";

    if (res == 0)
    {
        return true;
    }
    if (!is_power_of_two(res))
    {
        return false;
    }
    if (smudge_fixed)
    {
        return false;
    }
    if (!smudge_fixed)
    {
        smudge_fixed = true;
    }
    return true;
}

std::uint64_t find_reflected_smudged_rows(std::vector<std::string> &pattern)
{
    const size_t height = pattern.size();
    const size_t width = pattern[0].size();

    for (size_t col = 0; col < width - 1; ++col)
    {
        size_t steps = std::min(1 + col, width - 1 - col);
        bool smudge_located = false;
        size_t row = 0;
        while(1) {
            if(!check_horizontal(row, col, steps, smudge_located, pattern)) {
                break;
            }
            if(row == height - 1) {
                if(smudge_located){
                    return col + 1;
                }
                break;
            }
            ++row;
        }
    }
    for (size_t row = 0; row < height - 1; ++row)
    {
        size_t steps = std::min(1 + row, height - 1 - row);
        bool smudge_located = false;
        size_t col = 0;
        while(1) {
            if(!check_vertical(row, col, steps, smudge_located, pattern)) {
                break;   
            }
            if(col == width - 1) {
                if(smudge_located){
                    return 100*(row + 1);
                }
                break;
            }
            ++col;
        }
    }
    std::cout << "unable to solve the following pattern: \n";
    for(auto& line: pattern) {
        std::cout << line << "\n";
    }
    return 0;
}


void problem_2_thread(input_queue<std::vector<std::string>> &iq)
{
    std::uint64_t local_sum{};
    while (iq.ready())
    {
        std::vector<std::string> pattern = iq.get();
        // for (auto &line : pattern)
        // {
        //     std::cout << line << "\n";
        // }
        if (pattern.empty())
        {
            continue;
        }
        std::uint64_t answer = find_reflected_smudged_rows(pattern);
        local_sum += answer;
    }
    {
        std::lock_guard lg(answer_2_mutex);
        answer_2 += local_sum;
    }
}

void problem_1(const std::vector<std::vector<std::string>> &input, std::size_t n_threads)
{
    input_queue<std::vector<std::string>> iq(input, input_mutex);
    std::vector<std::thread> thread_vec;
    for (std::size_t i = 0; i < n_threads; ++i)
    {
        thread_vec.emplace_back(problem_1_thread, std::ref(iq));
    }
    for (auto &t : thread_vec)
    {
        t.join();
    }
}

void problem_2(const std::vector<std::vector<std::string>> &input, std::size_t n_threads)
{
    input_queue<std::vector<std::string>> iq(input, input_mutex);
    std::vector<std::thread> thread_vec;
    for (std::size_t i = 0; i < n_threads; ++i)
    {
        thread_vec.emplace_back(problem_2_thread, std::ref(iq));
    }
    for (auto &t : thread_vec)
    {
        t.join();
    }
}

int main()
{
    help_func::Timer t;
    std::vector<std::string> lines = help_func::read_input("../data/input-day-13.txt");
    // std::vector<std::string> lines = help_func::read_input("../data/small_data.txt");

    std::vector<std::vector<std::string>> patterns;

    std::vector<std::string> pattern;
    auto it = lines.begin();
    while (it != lines.end())
    {
        if ((*it).empty())
        {
            patterns.push_back(pattern);
            pattern.clear();
        }
        else
        {
            pattern.push_back(*it);
        }
        ++it;
    }
    patterns.push_back(pattern);

    std::size_t n_t{16};
    std::thread t1(problem_1, std::ref(patterns), n_t);
    std::thread t2(problem_2, std::ref(patterns), n_t);
    t1.join();
    t2.join();

    std::cout << "Day 13:" << answer_1 << ", " << answer_2 << " ";
}