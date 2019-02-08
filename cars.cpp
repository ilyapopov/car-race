#include "racetrack.hpp"
#include "bresenham.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "absl/hash/hash.h"
#include "absl/container/flat_hash_map.h"
#include "tsl/robin_map.h"
#include "tsl/hopscotch_map.h"
#include "flat_hash_map/bytell_hash_map.hpp"
#include "flat_hash_map/flat_hash_map.hpp"
#include "robin_hood.h"

const int VMAX = 10;
const int inf = std::numeric_limits<int>::max();

struct CarState
{
    CarState()
        : x(-1), y(-1), vx(0), vy(0)
    {}
    CarState(int x_, int y_)
        : x(x_), y(y_), vx(0), vy(0)
    {}
    CarState(int x_, int y_, int vx_, int vy_)
        : x(x_), y(y_), vx(vx_), vy(vy_)
    {}

    bool operator == (const CarState & other) const
    {
        return x == other.x && y == other.y
                && vx == other.vx && vy == other.vy;
    }

    friend bool operator<(const CarState & lhs, const CarState & rhs)
    {
        return std::tie(lhs.x, lhs.y, lhs.vx, lhs.vy)
                < std::tie(rhs.x, rhs.y, rhs.vx, rhs.vy);
    }

    template <typename H>
    friend H AbslHashValue(H h, const CarState & st) {
        return H::combine(std::move(h), st.x, st.y, st.vx, st.vy);
    }

    int16_t x, y;
    int8_t vx, vy;
};

template<typename Mapping, typename State, typename Callable>
std::vector<State> trace_back(const Mapping & prev, State state, Callable && getter)
{
    std::vector<State> trace;
    auto i = prev.begin();
    while((i = prev.find(state)) != prev.end())
    {
        trace.push_back(state);
        state = getter(i->second);
    }
    std::reverse(trace.begin(), trace.end());
    return trace;
}

template<typename Racetrack, typename State, typename Callback>
void for_possible_moves(const Racetrack & map, const State & state, Callback && callback)
{
    int nvx1 = std::max(state.vx - 1, -VMAX);
    int nvx2 = std::min(state.vx + 1,  VMAX);
    int nvy1 = std::max(state.vy - 1, -VMAX);
    int nvy2 = std::min(state.vy + 1,  VMAX);

    for (int nvx = nvx1; nvx <= nvx2; ++nvx)
    {
        int x = state.x + nvx;

        for (int nvy = nvy1; nvy <= nvy2; ++nvy)
        {
            int y = state.y + nvy;

            if (map.valid(x, y)
                    && map(x, y) != Racetrack::cell_t::outside
                    && bresenham(state.x, state.y, x, y,
                                  [&map](int x, int y){return map(x, y) != Racetrack::cell_t::outside;})
                    )
            {
                callback(CarState(x, y, nvx, nvy));
            }
        }
    }
}

std::vector<CarState> find_path_bfs(const Racetrack & map, const CarState & start)
{
    //std::map<CarState, CarState> prev;
    //std::unordered_map<CarState, CarState, absl::Hash<CarState>> prev;
    absl::flat_hash_map<CarState, CarState> prev;
    //tsl::robin_map<CarState, CarState, absl::Hash<CarState>> prev;
    //tsl::hopscotch_map<CarState, CarState, absl::Hash<CarState>> prev;
    //ska::bytell_hash_map<CarState, CarState, absl::Hash<CarState>> prev;
    //ska::flat_hash_map<CarState, CarState, absl::Hash<CarState>> prev;
    //robin_hood::unordered_map<CarState, CarState, absl::Hash<CarState>> prev;

    std::queue<CarState> q;

    prev.insert({start, CarState(-1, -1)});
    q.push(start);

    size_t count = 0;
    size_t lookups = 0;

    while (!q.empty())
    {
        auto state = q.front();
        q.pop();
        ++count;

        if (map(state.x, state.y) == Racetrack::cell_t::finish)
        {
            std::cout << "BFS: states processed: " << count << '\n'
                      << "     total added:      " << prev.size() << '\n'
                      << "     lookups:          " << lookups
                      << std::endl;

            return trace_back(prev, state, [](const CarState & st){return st;});
        }

        for_possible_moves(map, state, [&](const CarState & newstate)
        {
            ++lookups;
            if (prev.insert({newstate, state}).second)
            {
                q.push(newstate);
            }
        });
    }
    return {};
}

int main(int argc, char * argv[])
{
    Racetrack map;

    if (argc < 2)
    {
        std::cerr << "Filename expected" << std::endl;
        return 1;
    }

    std::string map_name = argv[1];

    std::cout << "Reading the map from " << map_name << " ..." << std::endl;

    std::ifstream is(map_name);
    if (!map.read_ppm(is))
    {
        std::cerr << "Error" << std::endl;
        return -1;
    }

    std::cout << "Finding the path ..." << std::endl;

    auto start = std::chrono::steady_clock::now();
    auto path = find_path_bfs(map, CarState(0, 0));

    std::chrono::duration<double> elapsed =
            std::chrono::steady_clock::now() - start;
    std::cout << "Search took " << elapsed.count() << " s" << std::endl;
    std::cout << "Path length: " << path.size() << std::endl;

    std::cout << "Writing the path ..." << std::endl;

    for (const auto & cs: path)
    {
        if (map.valid(cs.x, cs.y))
            map(cs.x, cs.y) = Racetrack::cell_t::trace;
    }

    std::ofstream os("path.ppm");
    map.write_ppm(os);

    std::cout << "Done" << std::endl;
    return 0;
}
