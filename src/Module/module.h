#pragma once
#include <vector>
#include <string>
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

struct TreeNode
{
    int type; //0:hb, -1:Vertical, -2:Horizontal
    int width, weight;
    HardBlock* hardblock;
    TreeNode *lchild, *rchild;
    // vector<tuple<int,int,pair<int,int>>> shape; // For VHnode (cut node)
    vector<vector<int>> shape;
    TreeNode(int type = 0, HardBlock* hardblock = nullptr):
      type(type), hardblock(hardblock), lchild(nullptr), rchild(nullptr)
    {
      if(type == 0) // leaf block shape
      {
        // shape.emplace_back(make_tuple(hardblock->width, hardblock->height, make_pair(0,0)));
        // shape.emplace_back(make_tuple(hardblock->height, hardblock->width, make_pair(1,1)));
        shape = {{hardblock->width, hardblock->height, 0, 0}, 
        {hardblock->height, hardblock->width, 1,1}};
      }
    }
    void updateShape();
};
