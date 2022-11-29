#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <climits>
#include <cmath>
#include <deque>
#include "parser.hpp"
using namespace std;




void HardBlock::updateInfo(int& new_width, int& new_height, int& new_x, int& new_y)
{
    
    downleft_x = new_x;
    downleft_y = new_y;
    if(width == new_width && height == new_height)
    {
      rotated = false;
    }  
    else
    {
      rotated = true;
    }
    center_pin->x_cor = new_x + new_width/2;
    center_pin->y_cor = new_y + new_height/2;
}




int net::calHPWL()
{
  int min_x = INT_MAX, max_x = INT_MIN;
  int min_y = INT_MAX, max_y = INT_MIN;
  for(const auto& pin: pins)
  {
    if(pin->x_cor < min_x)  min_x = pin->x_cor;
    if(pin->x_cor > max_x)  max_x = pin->x_cor;
    if(pin->y_cor < min_y)  min_y = pin->y_cor;
    if(pin->y_cor > max_y)  max_y = pin->y_cor;
  }
  for(const auto& hb: hardblocks)
  {
    if(hb->center_pin->x_cor < min_x) min_x = hb->center_pin->x_cor;
    if(hb->center_pin->x_cor > max_x) max_x = hb->center_pin->x_cor;
    if(hb->center_pin->y_cor < min_y) min_y = hb->center_pin->y_cor;
    if(hb->center_pin->y_cor > max_y) max_y = hb->center_pin->y_cor;
  }
  return (max_x - min_x) + (max_y - min_y);

}


// unordered_map <string, pin*>PinTable;
vector<HardBlock*> HBList;
unordered_map<string, HardBlock*> HBTable;
vector<net*> NetList;


struct TreeNode
{
    int type; //0:hb, -1:Vertical, -2:Horizontal
    int width, weight;
    HardBlock* hardblock;
    TreeNode *lchild, *rchild;
    vector<tuple<int,int,pair<int,int>>> shape; // For VHnode (cut node)
    
    TreeNode(int type = 0, HardBlock* hardblock = nullptr):
      type(type), hardblock(hardblock), lchild(nullptr), rchild(nullptr)
    {
      if(type == 0) // leaf block shape
      {
        shape.emplace_back(make_tuple(hardblock->width, hardblock->height, make_pair(0,0)));
        shape.emplace_back(make_tuple(hardblock->height, hardblock->width, make_pair(1,1)));
      }
    }
    
  void updateShape()
  {
    decltype(shape)().swap(shape);
    if(type == -1)
    {
      sort(lchild->shape.begin(), lchild->shape.end(), [&](auto& a, auto& b) {return get<1>(a) > get<1>(b);});
      sort(rchild->shape.begin(), rchild->shape.end(), [&](auto& a, auto& b) {return get<1>(a) > get<1>(b);});
      int i = 0, j = 0;
      while(i < lchild->shape.size() && j < rchild->shape.size())
      {
        auto cur_shape = make_tuple(get<0>(lchild->shape[i])+get<0>(rchild->shape[j]), max(get<1>(lchild->shape[i]), get<1>(rchild->shape[j])),make_pair(i,j));
        shape.emplace_back(cur_shape);
        if(get<1>(lchild->shape[i]) > get<1>(rchild->shape[j]))
        {
          ++i;
        }
        else if(get<1>(lchild->shape[i]) < get<1>(rchild->shape[j]))
        {
          ++j;
        }
        else
        {
          ++i;++j;
        }
      }
    }
    else
    {
      sort(lchild->shape.begin(), lchild->shape.end(), [&](auto& a, auto& b) {return get<0>(a) > get<0>(b);});
      sort(rchild->shape.begin(), rchild->shape.end(), [&](auto& a, auto& b) {return get<0>(a) > get<0>(b);});
      int i = 0, j = 0;
      while(i < lchild->shape.size() && j < rchild->shape.size())
      {
        auto cur_shape = make_tuple(max(get<0>(lchild->shape[i]),get<0>(rchild->shape[j])), get<1>(lchild->shape[i])+get<1>(rchild->shape[j]), make_pair(i,j));
        shape.emplace_back(cur_shape);
        if(get<0>(lchild->shape[i]) > get<0>(rchild->shape[j]))
        {
          ++i;
        }
        else if(get<0>(lchild->shape[i]) < get<0>(rchild->shape[j]))
        {
          ++j;
        }
        else
        {
          ++i;++j;
        }
      }
    }
  }
};


void WriteResult(string filename, int WL)
{
  ofstream output(filename);

  output << "Wirelength " << WL << "\n";
  output << "Blocks" << "\n";
  for(auto& hb:HBList)
  {
    output << hb->name << " " << hb->downleft_x << " " << hb->downleft_y << " " << hb->rotated << "\n"; 
  }
}



class SA
{ 
  private:
    double region_side_len;
    void CalSideLen(double& dead_space_ratio)
    {
      double total_area = 0;
      for(const auto& hb: HBList)
      {
        int w = hb->width, h = hb->height;
        total_area += (w * h);
      }
      region_side_len = sqrt(total_area * (1+dead_space_ratio));
    }

    template<class T>
    void SWAP(T& a, T& b) // "perfect swap" (almost)
{
  T tmp {move(a)}; // move from a
  a = move(b); // move from b
  b = move(tmp); // move from tmp
}
    
    void InitNPE(vector<int>& NPE)
    {
      vector<int> row;
      deque<vector<int>> rows;
      int cur_width = HBList[0]->width;
      row.emplace_back(0);
      int placedBlock = 1;
      for(int i = 1; i < HBList.size(); ++i)
      {
        auto curHB = HBList[i];
        if(cur_width + curHB->width <= region_side_len)
        {
          row.emplace_back(i);
          cur_width += curHB->width;
          row.emplace_back(-1);
        }
        else
        {
          rows.emplace_back(row);
          decltype(row)().swap(row);

          row.emplace_back(i);
          cur_width = curHB->width;
        }
      }
      rows.emplace_back(row);

      for(auto& row_i: rows[0])
      {
        NPE.emplace_back(row_i);
      }
      rows.pop_front();
      //NPE.emplace_back(-2);

      for(auto& row:rows)
      {
        for(auto& row_i:row)
        {
          NPE.emplace_back(row_i);
        }
        NPE.emplace_back(-2);
      }
    }
    void Complement(vector<int>& curNPE, int startIdx)
    {
      for(int i = startIdx; i < curNPE.size(); ++i)
      {
        if(curNPE[i] == -1) curNPE[i] = -2;
        else if(curNPE[i] == -2)  curNPE[i] = -1;
        else break;
      }
    }
    


    bool isSkewed(vector<int>& curNPE, int i)
    {

      if(i-1 >= 0 && curNPE[i-1] == curNPE[i+1])
      {
        return false;
      }
      else if(i+2 < curNPE.size() && curNPE[i] == curNPE[i+2])
      {
        return false;
      }
      return true;
    }
    
    bool isBallot(vector<int>& curNPE, int i)
    {

      if(curNPE[i] >= 0)
      {
        int N = 0;
        for(int k = 0; k <= i+1; ++k)
        {
          if(curNPE[i] == -1 || curNPE[i] == -2)  ++N;
          if(2 * N < i) return false;
        }
      }
      return true;
    }

    
    vector<int> Perturb(vector<int> curNPE, int M)
  {
    switch(M)
    {
      case 0:
      {
        vector<int> SwapPos;
        for(int i = 0; i < curNPE.size(); ++i)
        {
          if (curNPE[i] >= 0)
            SwapPos.emplace_back(i);
        }
        int n = SwapPos.size();

        
        // Change to pick two random hardblock for swapping
        int swap_pos1,swap_pos2;
        swap_pos1 = swap_pos2 = rand() % n;
        while(swap_pos1 == swap_pos2) swap_pos2 = rand() % n;
        swap_pos1 = SwapPos[swap_pos1];
        swap_pos2 = SwapPos[swap_pos2];

        SWAP(curNPE[swap_pos1], curNPE[swap_pos2]);
        break;
      }
      case 1:
      {
        vector<int> InverseStartPos;
        for(int i = 0; i < curNPE.size()-1; ++i)
          if(curNPE[i] >= 0 && curNPE[i+1] < 0)
              InverseStartPos.emplace_back(i+1);
        int n = InverseStartPos.size();
        int StartIdx = rand() % n;
        StartIdx = InverseStartPos[StartIdx];
        Complement(curNPE, StartIdx);
        break;
      }
      case 2:
      { 
        vector<int> SwapPos;
        for(int i = 0; i < curNPE.size()-1; ++i)
        {
          if(curNPE[i] >= 0 && curNPE[i+1] < 0)
          {
            if(isSkewed(curNPE, i) && isBallot(curNPE, i))
            {
              SwapPos.emplace_back(i);
            }
          }
          else if(curNPE[i] < 0 && curNPE[i+1] >= 0)
          {
            if(isSkewed(curNPE, i))
            {
              SwapPos.emplace_back(i);
            }
          }
        }
        int n = SwapPos.size();
        if(n != 0)
        {
          int r = rand() % n;
          int SwapIdx = SwapPos[r];
          // cout << "SwapIdx = " << SwapIdx << endl;
          SWAP(curNPE[SwapIdx], curNPE[SwapIdx+1]);
        }
        break;
      }
    }
    return curNPE;
  }
    
    TreeNode* ConstructTree(vector<int>& NPE)
    {
      vector<TreeNode*> st;
      for(auto element:NPE)
      {
        if(element >= 0)
        {
          string hbNode_name = "sb"+ to_string(element);
          HardBlock* hb = HBTable[hbNode_name];
          TreeNode* hbNode = new TreeNode(0, hb);
          st.emplace_back(hbNode);
        }
        else
        {
          TreeNode* VHnode = new TreeNode(element);
          TreeNode* Rnode = st.back(); st.pop_back();
          VHnode->rchild = Rnode;
          TreeNode* Lnode = st.back(); st.pop_back();
          VHnode->lchild = Lnode;
          st.emplace_back(VHnode);
          VHnode->updateShape();
        }
      }
      return st.back(); // root
    }
    void PlaceBlock(TreeNode* node, int shapeIdx, int x, int y)
    {
      if(node->type == 0)
      {
        node->hardblock->updateInfo(get<0>(node->shape[shapeIdx]), get<1>(node->shape[shapeIdx]), x, y);
      }
      else
      {
        PlaceBlock(node->lchild, get<2>(node->shape[shapeIdx]).first, x, y);
        int displacementX = 0, displacementY = 0;
        if(node->type == -1)
        {
          displacementX = get<0>(node->lchild->shape[get<2>(node->shape[shapeIdx]).first]);
        }
        else
        {
          displacementY = get<1>(node->lchild->shape[get<2>(node->shape[shapeIdx]).first]);
        }
        PlaceBlock(node->rchild, get<2>(node->shape[shapeIdx]).second, x+displacementX, y+displacementY);
      }
    }
    
    int CalTotalHPWL()
{
  int totalHPWL = 0;
  for(auto& net: NetList)
  {
    totalHPWL += net->calHPWL();
  }
  return totalHPWL;
}
    int CalCost(vector<int>& NPE, bool const& forWL)
    {
      TreeNode* root = ConstructTree(NPE);
      int min_out_area = INT_MAX, out_of_range_area = 0, shapeIdx = 0;
      for(int i = 0; i < root->shape.size(); ++i)
      {
        auto info = root->shape[i];
        int cur_width = get<0>(info), cur_height = get<1>(info);
        if(cur_width > region_side_len && cur_height > region_side_len)
        {
          out_of_range_area = cur_width * cur_height - pow(region_side_len,2);
        }
        else if(cur_height > region_side_len)
        {
          out_of_range_area = cur_width * (cur_height - region_side_len);
        }
        else if(cur_width > region_side_len)
        {
          out_of_range_area= cur_height * (cur_width - region_side_len);
        }
        else
        {
          out_of_range_area = 0;
        }
        // Pick 1st shape which is within the region due to time-saving.
        // But it might not be the min-area-shape of all the qualified shape. 
        if(out_of_range_area < min_out_area)
        {
          min_out_area = out_of_range_area;
          shapeIdx = i;
        }
      }

      if(forWL == false)  return min_out_area * 10;

      PlaceBlock(root, shapeIdx, 0, 0);

      return min_out_area * 10 + CalTotalHPWL();
    }
    void SAfloorplanning(double epsilon, double r, int k, bool forWL, vector<int>& initNPE, vector<int>& bestNPE)
    {
      bestNPE = initNPE;
      int MT, uphill, reject; MT = uphill = reject = 0;
      int N = k * HBList.size();
      vector<int> curNPE = initNPE;
      int cur_cost = CalCost(curNPE, forWL);
      int best_cost = cur_cost;
      if(best_cost == 0)
      { 
        CalCost(bestNPE, true); return; 
      }
      // mt19937 random_number_generator(random_device{}());
      // uniform_int_distribution<> rand_move(1, 3);
      // uniform_real_distribution<> rand_prob(0, 1);
      do
      {
        double T0 = 1000;
        do
        {
          MT = uphill = reject = 0;
          do
          {
            // int M = rand_move(random_number_generator);
            // int M = rand() % 3;
            int M = 0;
            if(forWL == false)  M = rand() % 3;
            vector<int> tryNPE = Perturb(curNPE, M);
            MT += 1;
            int try_cost = CalCost(tryNPE, forWL);
            int delta_cost = try_cost - cur_cost;
            if(delta_cost < 0 || (double)rand()/RAND_MAX < exp(-1*delta_cost/T0))
            {
              if(delta_cost > 0) { uphill += 1; }
              curNPE = tryNPE;
              cur_cost = try_cost; 
              if(cur_cost < best_cost)
              {
                bestNPE = curNPE;
                best_cost = cur_cost;
                if(best_cost == 0)
                { 
                  CalCost(bestNPE, true); return; 
                }
              }
            }
            else
            {
              reject += 1;
            }
          }while(uphill <= N && MT <= 2*N);
          T0 = r * T0;
        }while(reject/MT <= 0.95 && T0 >= epsilon);
      }while (forWL == false);
      CalCost(bestNPE, true); return; 
    }

  public:
    SA(double dead_space_ratio) { CalSideLen(dead_space_ratio); }
    int Run()
{
  unsigned seed = 2;
  srand(seed);

  vector<int> initNPE, bestNPE, finalNPE;
  InitNPE(initNPE);
  SAfloorplanning(0.1, 0.9, 10, false, initNPE, bestNPE);
  cout << "Find a feasible floorplan.\n" << "Total wirelength: " << CalTotalHPWL() << "\n";

  finalNPE = bestNPE;
  cout<<"start for HPWL : "<<endl;
  SAfloorplanning(1, 0.95, 5, true, bestNPE, finalNPE);
  int finalWL = CalTotalHPWL();
  cout << "Find a better floorplan.\n" << "Total wirelength: " << finalWL << "\n";

  return finalWL;
}

};





int main(int argc, char *argv[])
{
    parser *par = new parser();
    par->read_hardblock(argv[1]);
    par->read_net(argv[3]);
    par->read_pin(argv[2]);

    

    SA sa(stod(argv[5]));
    int finalWL = sa.Run();

    WriteResult(argv[4], finalWL);

    return 0;
}