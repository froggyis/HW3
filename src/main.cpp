#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <climits>
using namespace std;

//  ../bin/hw3 ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl 0.1

struct HardBlock
{
  string name;
  int width, height;
  pair<int, int>coor;
  bool isrotate ;
  // pin* center_pin;
  
  HardBlock(string name, int width, int height, bool isrotate = false, pair<int, int>coor = {0,0} ):
    name(name), width(width), height(height), isrotate(isrotate), coor(coor) {}
};

struct Pin;
struct Net
{
  vector<Pin*> pins;
  int HPWL();
};

struct Pin
{
  string name; 
  int x, y;

  Pin(string name, int x, int y):name(name), x(x), y(y) {}
};

vector<HardBlock *> HBList;
unordered_map<string, Pin*> PinMap;
vector<Net*> NetList;

struct TreeNode
{
    int type, width, height;//1 for block -1 for V, -2 for H
    HardBlock *hardblock;
    TreeNode *lchild, *rchild;
    vector<vector<int>> shape; //record for stockmeyer

    // TreeNode(int type, HardBlock* hardblock):
    //   type(type), hardblock(hardblock), lchild(nullptr), rchild(nullptr)
    //   {
    //     shape = {{hardblock->width, hardblock->height, 0, 0},
    //    {hardblock->height, hardblock->width, 1, 1}};
    //   };

    TreeNode(int type):
    type(type), hardblock(nullptr), lchild(nullptr), rchild(nullptr){};
    

    void stockmeyer();
};

vector<int> NPE ;//0~n stands for HB , -1 stands for V cut while -2 stands for H cut
void InitializeNPE(vector<int> &NPE);
void Placement(vector<int> &NPE);
void ChangeCut(vector<int> &NPE);
bool ifSkew(vector<int> &NPE);
bool ifBallot(vector<int> &NPE);

double floor_plan_region;
TreeNode *constructTree(vector<int> &NPE);
int main(int argc, char *argv[])
{
  //TODO 1. Parser
  // TODO 1-1: HardBlcoks
  ifstream in_hardblock , in_net, in_pin;
  in_hardblock.open(string(argv[1]));
  string line;
  string hb_name, tmp, tmp2, x1, y1, x2, y2, x3, y3, x4, y4;
  pair<int, int> coor;
  int width, height;
  int x[4], y[4];
  while(getline(in_hardblock, line))
  {
    if(line[0]!='s')continue;
    stringstream ss;
    ss.str(line);
    while(ss >> hb_name >> tmp >> tmp2 >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4)
    {

      x[0] = stoi(x1.substr(1, x1.size() - 2));
      y[0] = stoi(y1.substr(0, y1.size() - 1));

      x[1] = stoi(x2.substr(1, x2.size() - 2));
      y[1] = stoi(y2.substr(0, y2.size() - 1));

      x[2] = stoi(x3.substr(1, x3.size() - 2));
      y[2] = stoi(y3.substr(0, y3.size() - 1));

      x[3] = stoi(x4.substr(1, x4.size() - 2));
      y[3] = stoi(y4.substr(0, y4.size() - 1));
      width = *max_element(x, x+4) - *min_element(x, x+4);
      height = *max_element(y, y+4) - *min_element(y, y+4);

      coor = make_pair( *min_element(x, x+4), *min_element(y, y+4) );

      // cout<<x1_<<" "<<y1_<<" "<<x2_<<" "<<y2_<<" "<<x3_<<" "<<y3_<<" "<<endl;
      
    }
    HardBlock *HB = new HardBlock(hb_name, width, height, false, coor);
    HBList.emplace_back(HB);

  }

  // TODO 1-2: Pins
	in_pin.open(string(argv[3]));
  string pin_name;
  int pin_x, pin_y;
  while(in_pin >> pin_name >> pin_x >> pin_y)
  {
    Pin *input_pin = new Pin(pin_name, pin_x, pin_y);
    PinMap[pin_name] = input_pin;
  }
    

  // TODO 1-3: Nets
  in_net.open(string(argv[2]));
  string tmp_, _, pin_name_ ;
  int pin_degree;
  while(in_net>>tmp_)
  {
    if(tmp_ == "NetDegree")
    {

      Net *input_net = new Net();
      in_net >> _ >> pin_degree;
      NetList.emplace_back(input_net);
      for(int i = 0 ; i<pin_degree ; i++)
      {
        in_net >> pin_name_;
        if(pin_name_[0] =='p')
        { //push p1 p2.... into this net
          auto fix_pin = PinMap[pin_name_];
          NetList.back()->pins.emplace_back(fix_pin);
        }
        else
        { //push hardblock's center, ex: sb0 into this net
         Pin *hb_pin = new Pin(pin_name_, 0, 0);
         NetList.back()->pins.emplace_back(hb_pin);
        }
    
      }
      
    }
  }


  //TODO 2 , initialization
  //TODO 2-1, calculate constraint
  int total_block_area = 0;
  for (auto hb : HBList)
    total_block_area += hb->height * hb->width ; 

  double dead_space_ratiod = stof(string(argv[4]));
  floor_plan_region = sqrt(total_block_area * (1+dead_space_ratiod));
  //TODO 2-2, initialize NPE
  InitializeNPE(NPE);
  // for(auto c : NPE)
  // {
  //   cout<<c<<" ";
  // }

  auto root = constructTree(NPE);
  while(root->lchild)
  {
    for(auto c : root->shape)
    {
       cout << c[0] << endl;
    }

    root = root->lchild;
  }

  return 0;

}



// 12V3V4H.....

void InitializeNPE(vector<int> &NPE)
{
  vector<int> tmp;
  int cur_width = 0;
  int row_cnt = 0;
  bool new_hb = true;
  bool new_row = true;
  for(int i = 0 ; i<HBList.size(); i++)
  {
    auto HB = HBList[i];
    if(cur_width + HB->width <= floor_plan_region)
    {
      tmp.emplace_back(i);
      if(!new_hb)
      {
        tmp.emplace_back(-1);
      }
      cur_width += HB->width;
      new_hb = false;
    }

    else
    {
      NPE.insert(NPE.end(), tmp.begin(), tmp.end());
      tmp.clear();
      tmp.emplace_back(i);
      cur_width = HB->width;
      if(!new_row)
      {
        NPE.emplace_back(-2);
      }
      new_row = false;
      
    }
  }
  NPE.insert(NPE.end(), tmp.begin(), tmp.end());
  NPE.emplace_back(-2);
}


void Placement(vector<int> &NPE)
{
  unordered_map<string, pair<int, int>> tmp;
  int cor_x = 0, cor_y = 0;
  int height_max = INT_MIN;
  pair<int, int>cur_coor = make_pair(0, 0);
  for(int i = 0 ; i<NPE.size()-1 ; i++)
  {
    if(NPE[i]!=-1 && NPE[i]!= -2)
    {
      auto HB = HBList[NPE[i]];      
      cur_coor.first += HB->width;
      if(HB->height > height_max) height_max = HB->height;
      tmp[HB->name] = cur_coor;
    }
    if(NPE[i]==-2)
    {
      cur_coor.first = 0;
      cur_coor.second = height_max;
      height_max = INT_MIN;
    }

  
  }
  // for(auto name : tmp)
  //   cout<<"name : "<<name.first<<" x : "<<name.second.first<<"y : "<<name.second.second<<endl;

}


void ChangeCut(vector<int> &NPE)
{
  for(int i = 0 ; i<NPE.size(); i++)
  {
    if(NPE[i]==-1)NPE[i]=-2;
    if(NPE[i]==-2)NPE[i]=-1;
  }
}


bool ifSkew(vector<int> &NPE)
{
  for(int i = 1 ; i<NPE.size() ; i++)
  {
    if( NPE[i] == NPE[i-1] ||  NPE[i] == NPE[i+1])
      return false;
  }
  return true;

}

bool ifBallot(vector<int> &NPE)
{
  int operand = 0 ;
  int operators = 0 ;

  for(auto n : NPE)
  {
    if(n==-1 || n==-2)operators++;
    else operand++;
  }
  if(operand< operators)return false;
  return true;
  
}


TreeNode *constructTree(vector<int> &NPE)
{
  vector<TreeNode *>postorder;
  for(int block : NPE)
  {
    if(block >=0)// if block is hardblock then push
    {
      auto *hb = HBList[block];
      TreeNode *node = new TreeNode(0); // 0 means this node is an hardblock
      node->shape  = {{hb->width, hb->height, 0, 0},{hb->height, hb->width, 1, 1}};//build the node for stockmeyer
      postorder.push_back(node);
    }

    else//if block is V/H then pop and run stockmeyer
    {
      TreeNode *CutNode = new TreeNode(block);
      CutNode->type = block;
      
      auto Rnode = postorder.back();
      postorder.pop_back();
      CutNode->rchild = Rnode;

      auto Lnode = postorder.back();
      postorder.pop_back();
      CutNode->lchild = Lnode;

      postorder.push_back(CutNode);
      CutNode->stockmeyer();
    }
  }
  return postorder.back();
}


void TreeNode::stockmeyer()
{
  int i, j;
  //when we implement the tree, check whether the node is CUT or HARDBLOCK.
  //if the block is CUT then we implemnt the STOCKMEYER algorithm to update our shape
  // -1 means VERTICAL cut while -2 means HORIZONTAL
  vector<int> tmp;
  if(type==-1)
  {
    i = 0;
    j = 0;
    sort(lchild->shape.begin(), lchild->shape.end());  // without assign lamda function it will sort by the first element 
    sort(rchild->shape.begin(), rchild->shape.end());
    // cout<<"lchild : "<<lchild->shape.size()<<"rchild : "<<rchild->shape.size()<<endl;
    while (i< lchild->shape.size() && j < rchild->shape.size())
    {
      tmp = {lchild->shape[i][0] + rchild->shape[i][0], max(lchild->shape[i][1], rchild->shape[i][1]), i, j};
      shape.emplace_back(tmp);
      if(lchild->shape[i][0] > rchild->shape[i][0]) i ++;
      else if (lchild->shape[i][0] < rchild->shape[i][0]) j++;
      else i++;j++;
    }
    
  }

  else
  {
    i = 0;
    j = 0;
    sort(lchild->shape.begin(), lchild->shape.end(), greater<>());  // without assign lamda function it will sort by the first element 
    sort(rchild->shape.begin(), rchild->shape.end(), greater<>());  // sort by decending order.
    while (i<lchild->shape.size() && j < rchild->shape.size())
    {
      tmp = {lchild->shape[i][0] + rchild->shape[i][0], max(lchild->shape[i][1], rchild->shape[i][1]), i, j};
      shape.emplace_back(tmp);
      if(lchild->shape[i][0] > rchild->shape[i][0]) i ++;
      else if (lchild->shape[i][0] < rchild->shape[i][0]) j++;
      else i++;j++;
    }

  }

}