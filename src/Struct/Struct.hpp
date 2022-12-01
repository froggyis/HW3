#pragma once
#include <vector>
#include <string>
#include <iostream> 
#include <utility>
using namespace std;

struct pin;
struct HardBlock
{
  string hb_name;
  int width, height;
  pair<int, int>coor;//down left coordinate (X, Y)
  bool rotated;
  pin* center;
  void Update(int& new_width, int& new_height, int& new_x, int& new_y);

  HardBlock(string hb_name, int width, int height, pin* center, pair<int, int> coor):
    hb_name(hb_name), width(width), height(height), rotated(false), center(center) , coor(coor) {}
};

struct pin
{
  int x, y;
  string pin_name; 

  pin(string pin_name, int x, int y):pin_name(pin_name), x(x), y(y) {}
};


struct net
{

  vector<pin*> pins;
  vector<HardBlock*> hardblocks;
 
  int calHPWL();
};

struct TreeNode
{
    int type; //0 stans for hardblock while (-1, -2) => (V, H)
    int width, weight;
    HardBlock* hardblock;
    TreeNode *lchild, *rchild;
    vector<vector<int>> shape;
    TreeNode(int type = 0, HardBlock* hardblock = nullptr):
      type(type), hardblock(hardblock), lchild(nullptr), rchild(nullptr){}
    void stockmeyer();
};
