#include <bits/stdc++.h>
using namespace std;


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
    int type, width, weight;
    HardBlock *hardblock;
    TreeNode *lchild, *rchild;
    vector<tuple<int, int, int,int >> shape;
    
    TreeNode(int type, HardBlock* hardblock = nullptr):
      type(type), hardblock(hardblock), lchild(nullptr), rchild(nullptr){};

    void update();
};

vector<int> NPE ;//0~n stands for HB , -1 stands for V cut while -2 stands for H cut
void InitializeNPE(vector<int> &NPE);
void Placement(vector<int> &NPE);

double floor_plan_region;
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
  Placement(NPE);;

  return 0;
}

// 12V3V4H.....

void InitializeNPE(vector<int> &NPE)
{
  int row_cnt = 1 ;
  int cur_width = 0;
  auto HB = HBList[0];
  cur_width += HB->width;
  NPE.emplace_back(0);
  for(int i = 1 ; i<HBList.size(); i++)
  {
    auto HB = HBList[i];
    cur_width += HB->width;
    if(cur_width <= floor_plan_region)//if current row can still fit with HB
    {
      
      NPE.emplace_back(i);
      NPE.emplace_back(-1);

    }

    else//if not , We go to new row and continue the process
    {
      cur_width = HB->width;
      row_cnt=2;
    }

    if(row_cnt==2)//whenever we make two row, we need to inset an H cut into NPE
    {
      NPE.emplace_back(-2);
      row_cnt=1;
    }
  }
  NPE.emplace_back(-2);
}

void Placement(vector<int> &NPE)
{
  unordered_map<string, pair<int, int>> tmp;
  int cor_x = 0, cor_y = 0;
  int height_max = INT_MIN;
  pair<int, int>cur_coor = make_pair(0, 0);
  cout<<HBList.size()<<endl;
  cout<<NPE.size()<<endl;
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
  for(auto name : tmp)
    cout<<"name : "<<name.first<<" x : "<<name.second.first<<"y : "<<name.second.second<<endl;

}


