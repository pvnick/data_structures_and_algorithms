#include <iostream>
#include <iomanip>

void trial(const char* c, int n) {
    size_t DP[27][n + 1];
    for (int i = 0; i <= 26; ++i) {
        for (int j = n; j >= 0; --j) {
            if (i == 0 || j == n)
                DP[i][j] = n;
            else {
                int charval = c[j] - 'A';
                if (charval < i) {
                    int takeit = j;
                    int leaveit = DP[i][j+1];
                    int best = std::min(takeit, leaveit);
                    DP[i][j] = best;
                } else {
                    DP[i][j] = DP[i][j+1];
                }
            }
        }
    }
    for (int i = 0; i != n; ++i) {
        for (int j = 0; j != 27; ++j) {
            std::cout << std::setw(3) << DP[i][j] << " | ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    std::string str = "ALKSDGLAKSDAFASDFWQEJAFIJSDF";
    trial(str.c_str(), str.size());
    return 0;
}
