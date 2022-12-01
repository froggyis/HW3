#pragma once
#include <vector>
#include <string>
#include <iostream> 
#include <utility>
using namespace std;

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
  pair<int, int>coor;
  bool rotated;
  pin* center_pin;
  void Update(int& new_width, int& new_height, int& new_x, int& new_y);

  HardBlock(string name, int width, int height, pin* center_pin, pair<int, int> coor):
    name(name), width(width), height(height), rotated(false), center_pin(center_pin) , coor(coor) {}
};

struct net
{

  vector<pin*> pins;
  vector<HardBlock*> hardblocks;
 
  int calHPWL();
};

struct TreeNode
{
    int type; //0:hb, -1:Vertical, -2:Horizontal
    int width, weight;
    HardBlock* hardblock;
    TreeNode *lchild, *rchild;
    vector<vector<int>> shape;
    TreeNode(int type = 0, HardBlock* hardblock = nullptr):
      type(type), hardblock(hardblock), lchild(nullptr), rchild(nullptr){}
    void updateShape();
};
