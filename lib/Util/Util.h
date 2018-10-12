#include <utility>

std::pair<int, int> map_single(int x, int y) {
    return std::pair<int, int>((3 - x / 8) * 8 + x % 8, y);
}