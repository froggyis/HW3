#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
using namespace std;

class parser
{
  public:
    void read_hardblock(string const &filename);
    void read_net(string const &filename);
    void read_pin(string const &filename);
};

struct pin
{
  string id; 
  int x_cor, y_cor;

  pin(string id, int x_cor, int y_cor):id(id), x_cor(x_cor), y_cor(y_cor) {}
};

struct HardBlock
{
  string name;
  int width, height;
  int downleft_x, downleft_y; 
  bool rotated;
  pin* center_pin;
  
  void updateInfo(int& new_width, int& new_height, int& new_x, int& new_y);

    HardBlock(string name, int width, int height, pin* center_pin, int downleft_x = 0, int downleft_y = 0):
      name(name), width(width), height(height), rotated(false), downleft_x(downleft_x), downleft_y(downleft_y), center_pin(center_pin) {}
};


struct net
{
  int degree;
  vector<pin*> pins;
  vector<HardBlock*> hardblocks;

  net(int degree = 0):degree(degree) {}

  int calHPWL();

};