#include <vector>

std::vector<std::pair<float, float>> locateCentroids(const char* image, float d, int w);
void run_test(int side, int numDots, int radius = 1);
void run_file(const char* imFile, float d, int w);
