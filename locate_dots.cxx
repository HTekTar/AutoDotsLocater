#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>

#include "LocatingDotsConfig.h"
#include "dots_locate.h"

bool findOption(char **begin, char **end, std::string opt)
{
  return std::find(begin, end, opt) != end;
}

char *getOption(char **begin, char **end, std::string opt)
{
  char **itr = std::find(begin, end, opt);
  if (itr != end && ++itr != end)
  {
    return *itr;
  }
  return 0;
}

int main(int argc, char **argv)
{
  const char *file = getOption(argv, argv + argc, "-f");     // image file
  const char *outDir = getOption(argv, argv + argc, "-d");   // output directory
  const char *numHoles = getOption(argv, argv + argc, "-n"); // number of holes to be generated in test
  const char *a = getOption(argv, argv + argc, "-a");        // subpixel accuracy required
  const char *w = getOption(argv, argv + argc, "-w");        // holes' width in pixel

  if (findOption(argv, argv + argc, "--test"))
  {
    std::cout << "running test..\n";
    int n = 50;
    if (numHoles)
      n = std::stoi(numHoles);
    if (n <= 0){
      std::cout << "n must be positive integer\n";
      return -1;
    }
      
    run_test(1000, n, 5);
  }

  else if (file)
  {
    if (!std::ifstream(std::string(file).c_str()).good())
    {
      std::cout << "file does not exist\n";
      return -1;
    }
    float d = 0.2, width = 4;
    if(a) d = std::stof(a);
    if(w) width = std::stoi(w);

    if (width < 2)
    {
      std::cout << "hole width too small\n";
      return -1;
    }
    else if (d > 1.0f || d < 0.0f)
    {
      std::cout << "pixel accuracy should be between 0 and 1\n";
      return -1;
    }
    std::cout << "d:" << d << " w:" << width << std::endl;
    std::cout << "running file..\n";
    run_file(file, d, width);
    return 0;
  }

  else
  {
    std::cout << ("usage: -f <image-file> or --test\n");
    return -1;
  }

  return 0;
}