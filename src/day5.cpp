#include <string>
#include <string_view>
#include <iostream>
#include <unordered_map>
#include <limits>
#include <thread>
#include <charconv>
#include <cmath>
#include <algorithm>
#include <concepts>
#include <ranges>
#include "help_func.hpp"

enum class Overlap {no, lower, upper, contains, covers};

class Range{
    
    std::uint64_t destination_start;
    std::uint64_t source_start;
    std::uint64_t width;
    public:
        
        Range(std::uint64_t ds, std::uint64_t ss, std::uint64_t rl) : destination_start(ds), source_start(ss), width(rl) {}
        bool contains(std::uint64_t);
        std::uint64_t map(std::uint64_t);
        void invert();

        uint64_t lbo() const {return destination_start;};
        uint64_t lbi() const {return source_start;};
        uint64_t w() const {return width;};

        void update_ds(std::uint64_t lb) {
            destination_start = lb;    
        }

        void update_ss(std::uint64_t lb) {
            source_start = lb;    
        }

        void update_width(std::uint64_t w) {
            width = w;
        }
};

std::ostream& operator<<(std::ostream& os, const Range& r){
    os << r.lbi() << " " << r.lbi() + r.w() << " -> " << r.lbo() << " " << r.lbo() + r.w();
    return os;
}

void Range::invert() {
    std::swap(destination_start, source_start);
}

bool Range::contains(std::uint64_t num)
{
    return num >= source_start && num < source_start + width;
}

std::uint64_t Range::map(std::uint64_t num){
    std::uint64_t offset = num - source_start;
    return destination_start + offset;
}

Overlap calc_overlap(const Range &r1, const Range &r2) {
    auto r1_lb = r1.lbo();
    auto r1_ub = r1.lbo() + r1.w();
    auto r2_lb = r2.lbi();
    auto r2_ub = r2.lbi() + r2.w();

    bool c1 = r1_lb >= r2_ub;
    bool c2 = r1_ub <= r2_lb;

    if(c1 || c2) {
        return Overlap::no;
    }

    bool c3 = r1_lb >= r2_lb;
    bool c4 = r1_ub <= r2_ub;

    if(c3 && c4) {
        return Overlap::contains;
    }

    if( !c3 && !c4) {
        return Overlap::covers;
    }

    if(c3)  {
        return Overlap::upper;
    }

    return Overlap::lower;
}

void fill_out_range(std::vector<Range> &r_vec, std::uint64_t max_seed) 
{
    auto l_sort = [](Range &r1, Range &r2){return r1.lbi() < r2.lbi();};
    std::ranges::sort(r_vec, l_sort);

    std::uint64_t start{};

    std::vector<Range> new_ranges;

    std::size_t ind{};
    while(start < max_seed) 
    {
        if (ind < r_vec.size()) {
            std::uint64_t delta = r_vec[ind].lbi() > start ? r_vec[ind].lbi() - start : 0;
            if(0 == delta) {
                start = r_vec[ind].lbi() + r_vec[ind].w();
                ++ind;
                continue;
            }

            new_ranges.emplace_back(start, start, delta);
            start = r_vec[ind].lbi() + r_vec[ind].w();
            ++ind;
            continue;
        }
        new_ranges.emplace_back(start, start, max_seed - start);
        start = max_seed;
    }
    std::move(new_ranges.begin(), new_ranges.end(), std::back_inserter(r_vec));

    std::ranges::sort(r_vec, l_sort);
}

[[nodiscard]] std::vector<Range> range_collapse(std::vector<std::vector<Range>>& stacked_filter, std::uint64_t max_seed){
    std::vector<Range> flat_filter;
    std::vector<Range> queue{stacked_filter[0]};

    fill_out_range(queue, max_seed);

    for(std::size_t i = 1; i < stacked_filter.size(); ++i){

        flat_filter.clear();
        
        while(!queue.empty()) {
            Range r1 = queue.back();
            queue.pop_back();

            bool bf = false;
            for(Range &r2 : stacked_filter[i]) { 

                if(bf) {
                    break;
                }

                std::uint64_t delta = 0;
                std::uint64_t d2 = 0;
                switch(calc_overlap(r1, r2)) 
                {
                    case Overlap::no:
                        continue;
                    case Overlap::covers:
                        d2 = r2.lbi() - r1.lbo();
                        delta = r2.w() + d2;

                        queue.emplace_back(r1.lbo(), r1.lbi(), d2);
                        queue.emplace_back(r1.lbo() + delta, r1.lbi() + delta, r1.w() - delta);
                        r1.update_ds(r2.lbo());
                        r1.update_width(r2.w());
                        r1.update_ss( r1.lbi() + d2);
                        bf = true;
                        break;
                    case Overlap::upper:
                        delta = (r2.w() + r2.lbi()) - r1.lbo();
                        queue.emplace_back(r1.lbo() + delta, r1.lbi() + delta, r1.w() - delta);
                        r1.update_ds(r2.map(r1.lbo()));
                        r1.update_width(delta);
                        bf = true;
                        break;
                    case Overlap::lower:
                        delta = r2.lbi() - r1.lbo();
                        queue.emplace_back(r1.lbo(), r1.lbi(), delta);
                        r1.update_ds(r2.lbo());
                        r1.update_ss(r1.lbi() + delta);
                        r1.update_width(r1.w() - delta);
                        bf = true;
                        break;

                    case Overlap::contains:
                        r1.update_ds(r2.map(r1.lbo()));
                        bf = true;
                        break;
                    default:
                        continue;
                }
            }
            flat_filter.push_back(r1);
        }

        queue = flat_filter;
    }

    return flat_filter;
}

[[nodiscard]] std::uint64_t problem_1(std::vector<std::string>& input){
    auto raw_line = help_func::split(input[0], " ");
    std::vector<std::uint64_t> seeds;

    for(std::size_t i = 1; i < raw_line.size(); ++i)
    {
        seeds.push_back(std::stoull(raw_line[i]));
    }

    std::uint64_t max_seed = 0;

    std::vector<std::vector<Range>> range_filter;
    std::vector<Range> range_vec;
    for(std::string const &row : input | std::views::drop(3))
    {

        if(row.empty()){
            continue;
        }
        if(row.find(':') < std::string::npos)
        {
            range_filter.push_back(range_vec);
            range_vec.clear();
            continue;
        }
        std::vector<std::string> _s_vec = help_func::split(row, " ");
        std::uint64_t dest_start = std::stoull(_s_vec[0]);
        std::uint64_t source_start = std::stoull(_s_vec[1]);
        std::uint64_t width = std::stoull(_s_vec[2]);

        std::uint64_t widest = dest_start > source_start ? dest_start + width : source_start + width;
        max_seed = widest > max_seed ? widest : max_seed;

        range_vec.emplace_back(dest_start, source_start, width);
    }
    range_filter.push_back(range_vec);

    std::vector<Range> flat_filter = range_collapse(range_filter, max_seed+ 1);

    std::uint64_t smollest_loc = std::numeric_limits<std::uint64_t>::max();
    for(auto seed : seeds) {
        for(auto filter : flat_filter) {
            if(filter.contains(seed)){
                std::uint64_t loc = filter.map(seed);
                if(loc < smollest_loc) {
                    smollest_loc = loc;
                }
            }
        }
    }

    return smollest_loc;
}

[[nodiscard]] std::uint64_t problem_2(std::vector<std::string>& input){
    auto raw_line = help_func::split(input[0], " ");
    std::vector<std::uint64_t> seeds;

    for(std::size_t i = 1; i < raw_line.size(); ++i)
    {
        seeds.push_back(std::stoull(raw_line[i]));
    }

    std::vector<Range> seed_range;
    std::size_t i{};
    while(i < seeds.size()){
        seed_range.emplace_back(seeds[i], seeds[i], seeds[i + 1]);
        ++++i;
    }

    std::uint64_t max_seed = 0;

    std::vector<std::vector<Range>> range_filter;
    std::vector<Range> range_vec;
    for(std::string const &row : input | std::views::drop(3))
    {

        if(row.empty()){
            continue;
        }
        if(row.find(':') < std::string::npos)
        {
            range_filter.push_back(range_vec);
            range_vec.clear();
            continue;
        }
        std::vector<std::string> _s_vec = help_func::split(row, " ");
        std::uint64_t dest_start = std::stoull(_s_vec[0]);
        std::uint64_t source_start = std::stoull(_s_vec[1]);
        std::uint64_t width = std::stoull(_s_vec[2]);

        std::uint64_t widest = dest_start > source_start ? dest_start + width : source_start + width;
        max_seed = widest > max_seed ? widest : max_seed;

        range_vec.emplace_back(dest_start, source_start, width);
    }
    range_filter.push_back(range_vec);

    std::vector<Range> flat_filter = range_collapse(range_filter, max_seed);
    

    for(auto &r : flat_filter) {
        r.invert();
    }
    std::ranges::sort(flat_filter, [](Range &r1, Range &r2){return r1.lbi() < r2.lbi();});


    std::uint64_t smallest_seed = 0;
    std::uint64_t offset = 0;

    bool bf = false;
    for(auto r : flat_filter) {
        
        if(bf) {
            break;
        }
        for(auto sr : seed_range) {
            switch(calc_overlap(r, sr)) {
                case Overlap::no:
                    continue;
                case Overlap::upper:
                    smallest_seed = r.lbi();
                    bf = true;
                    break;
                case Overlap::contains:
                    smallest_seed = r.lbi();
                    bf = true;
                    break;
                case Overlap::covers:
                    offset = sr.lbi() - r.lbo();
                    smallest_seed = offset + r.lbi();
                    bf = true;
                    break;
                case Overlap::lower:
                    smallest_seed = offset + r.lbi();
                    bf = true;
                    break;
                default:
                    continue;
            }
        }
    }

    return smallest_seed;
}

int main(){
    help_func::Timer t;
    std::vector<std::string> input =help_func::read_input("input-day-5.txt");
    // std::vector<std::string> input =help_func::read_input("small_data.txt");

    std::uint64_t answer_1 = problem_1(input);
    std::uint64_t answer_2 = problem_2(input);

    std::cout << answer_1 << "\n";
    std::cout << answer_2 << "\n";
}