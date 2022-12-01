#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "../Struct/Struct.hpp"
using namespace std;

class Parser
{
  public:
    void read_hardblock(string const &filename);
    void read_net(string const &filename);
    void read_pin(string const &filename);
    
};
