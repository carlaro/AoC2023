#include <string>
#include <string_view>
#include <iostream>
#include <unordered_map>
#include <limits>
#include <thread>
#include "help_func.hpp"

class state_machine 
{
    private:
        std::size_t state;
        const std::string target;
        const int value;
        std::size_t end_state;
        std::vector<std::string> const &lines;
        std::vector<std::vector<std::size_t>> indices;

    public:
        state_machine() = delete;
        state_machine(std::string t, int v,std::vector<std::string>& input): 
            state(0), 
            target(t),
            value(v), 
            end_state(target.size()),
            lines(input), 
            indices(std::vector<std::vector<std::size_t>>(lines.size(), std::vector<std::size_t>())) 
        {}

        void operator()() {
            std::size_t line_index{};
            std::size_t ind{};
            std::size_t length{};

            for(std::string const &line : lines) {
                ind = 0;
                length = line.size();
                state = 0;

                while(ind < length){
                    if(line[ind] == target[state]){
                        ++state;
                        ++ind;
                    } else {
                        state = (line[ind] == target[0])? 1 : 0;
                        ++ind;
                    }
                    
                    if(state == end_state) {
                        indices[line_index].push_back(ind - state);
                    }

                }
                ++line_index;
            }
        }
        const std::vector<std::vector<std::size_t>>& get_indices() const
        {
            return indices;
        }

        const std::vector<std::size_t>& get_indices(std::size_t i) const
        {
            return indices[i];
        }

        const std::string& get_name() const
        {
            return target;
        }

        const int& get_value() const
        {
            return value;
        }
};

[[nodiscard]] int find_solution(std::vector<std::string>& input, std::unordered_map<std::string, int> const &search_map){
    std::vector<state_machine> sm_vec;
    for(auto &s: search_map){
        sm_vec.emplace_back(s.first, s.second, input);
    }

    std::vector<std::thread> thread_vec;
    for(state_machine &sm: sm_vec) {
        thread_vec.emplace_back(std::ref(sm));
    }

    for(std::thread &t: thread_vec) {
        t.join();
    }

    int sum = 0;
    for(std::size_t i{}; i < input.size(); ++i)
    {
        int first_val = 0;
        int last_val = 0;
        std::size_t f_ind = std::numeric_limits<std::size_t>::max();
        std::size_t l_ind = 0;

        for(state_machine &sm: sm_vec) {
            for(auto l : sm.get_indices(i))
            {
                if(l <= f_ind)
                {
                    f_ind = l;
                    first_val = sm.get_value();
                }

                if(l >= l_ind)
                {
                    l_ind = l;
                    last_val = sm.get_value();
                }
            }
        }
        sum += 10*first_val + last_val;

    }

    return sum;
}

int main()
{
    help_func::Timer t;
    auto input = help_func::read_input("../data/input-day-1.txt");

    const std::unordered_map<std::string, int> search_map_problem_1{
        {"1", 1}, {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5},
        {"6", 6}, {"7", 7}, {"8", 8}, {"9", 9}
    };

    const std::unordered_map<std::string, int> search_map_problem_2{
        {"one", 1}, {"two", 2}, {"three", 3}, 
        {"four", 4}, {"five", 5}, {"six", 6}, 
        {"seven", 7}, {"eight", 8}, {"nine", 9},
        {"1", 1}, {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5},
        {"6", 6}, {"7", 7}, {"8", 8}, {"9", 9}
    };

    int answer_1 = find_solution(input, search_map_problem_1);
    int answer_2 = find_solution(input, search_map_problem_2);

    std::cout << "Day 1:" << answer_1  << ", " << answer_2 << " ";
    
    return 0;
}