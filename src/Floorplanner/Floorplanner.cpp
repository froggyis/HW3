#include <algorithm>
#include <climits>
#include <cmath>
#include <deque>
#include <iostream>
#include <stdlib.h> 
#include <unordered_map>
#include "Floorplanner.hpp"
using namespace std;

extern vector<HardBlock*> HBList;
extern unordered_map<string, HardBlock*> HBTable;
extern vector<net*> NetList;
extern vector<TreeNode*> CutNodes;



void SA::InitNPE(vector<int>& NPE)//change
{
  vector<int> tmp;
  int cur_width = 0;
  int row_cnt = 0;
  bool new_hb = true;
  bool new_row = true;
  for(int i = 0 ; i<HBList.size(); i++)
  {
    auto HB = HBList[i];
    if(cur_width + HB->width <= region_side_len)
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




bool SA::isSkewed(vector<int>& curNPE, int idx)//not change
{

  if(idx-1 >= 0 && curNPE[idx-1] == curNPE[idx+1])
  {
    return false;
  }
  else if(idx+2 < curNPE.size() && curNPE[idx] == curNPE[idx+2])
  {
    return false;
  }
  return true;
}

bool SA::Ballot(vector<int>& curNPE, int i)//changed
{
  int operators = 0;
  if(curNPE[i] >= 0)
    for(int k = 0; k <= i+1; ++k)
      if(curNPE[k] == -1 || curNPE[k] == -2)  
        operators++;
  
  if(2 * operators < i+1) return true;

  else return false;

}

vector<int> SA::Perturb(vector<int> curNPE, int M)//M3 not change
{
 
  vector<int>tmp;
  switch(M)
  {
    case 0://M1 for swap two adjacent blocks
    { //Different from slids taught in calss
      //Slides tell us only swap the adjancent block which is hard to find
      //feasible answer, so I globally choose two position to change the block
      // vector<int> tmp;
      for(int i = 0 ; i<curNPE.size(); i++)
        if(curNPE[i]>=0)tmp.emplace_back(i);

      int tmp_pos1 = rand()%(tmp.size());
      int tmp_pos2;
      do{
        tmp_pos2 = rand()%(tmp.size());
      }while(tmp_pos1==tmp_pos2);
      swap(curNPE[tmp[tmp_pos1]], curNPE[tmp[tmp_pos2]]);


      break;
    }
    case 1:
    {
      //same as the way teacher taught in class
      //Randomly find a chain then complement the H/V blocks
      // vector<int> tmp;
      for(int i = 0 ; i < curNPE.size(); i++)
        if(curNPE[i]<0)tmp.emplace_back(i);

      int idx = rand() % tmp.size();
      for(int i = idx; i < curNPE.size(); i++)
      {
        if(curNPE[tmp[idx]]==-1)curNPE[tmp[idx]]=-2;
        else if(curNPE[tmp[idx]]==-2)curNPE[tmp[idx]]=-1;
      }
      break;

    }
    case 2:
       { 
      vector<int> SwapPos;
      for(int i = 0; i < curNPE.size()-1; ++i)
      {
        if(curNPE[i] >= 0 && curNPE[i+1] < 0)
        {
          if(isSkewed(curNPE, i) && Ballot(curNPE, i))
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
        swap(curNPE[SwapIdx], curNPE[SwapIdx+1]);
      }
      break;
    }

  }

  return curNPE;
  
}

TreeNode* SA::ConstructTree(vector<int>& NPE)//not change
{
  int cutIdx = 0;
  vector<TreeNode*> st;
  for(auto &element:NPE)
  {
    if(element >= 0)
    {
      // string hbNode_name = "sb"+ to_string(element);
      // HardBlock* hb = HBTable[hbNode_name];
      // TreeNode* hbNode = new TreeNode(0, hb);
      // st.emplace_back(hbNode);
      st.emplace_back(hbNodeList[element]);
    }
    else
    {
      auto node = cutNodeList[cutIdx++];
      node->type = element;
      node->rchild = st.back();
      st.pop_back();
      node->lchild = st.back();
      st.pop_back();
      st.emplace_back(node);
      node->updateShape();
    }
  }
  return st.back(); // root
}

void SA::PlaceBlock(TreeNode* node, int shapeIdx, int x, int y)//not change
{
  // pair<int, int>tmp = make_pair{x, y};
  if(node->type >= 0)
  {
    node->hardblock->Update(node->shape[shapeIdx][0], node->shape[shapeIdx][1], x, y);

  }
  else
  {
    PlaceBlock(node->lchild, node->shape[shapeIdx][2], x, y);

    int displacementX = 0, displacementY = 0;
    if(node->type == -1)
    {
      displacementX = node->lchild->shape[   node->shape[shapeIdx][2]    ][0];

    }
    else
    {
      displacementY = node->lchild->shape[   node->shape[shapeIdx][2]    ][1];
    }
    PlaceBlock(node->rchild, node->shape[shapeIdx][3], x+displacementX, y+displacementY);


  }
}



int SA::GetCost(vector<int>& NPE, bool const& forWL)
{
  TreeNode* root = ConstructTree(NPE);
  int min_out_area = INT_MAX, out_of_range_area = 0, shapeIdx = 0;
  for(int i = 0; i < root->shape.size(); ++i)
  {
    auto info = root->shape[i];
    // int cur_width = get<0>(info), cur_height = get<1>(info);
    int cur_width = info[0];
    int cur_height = info[1];
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
  int totalWL = 0;
  for(auto& net: NetList)totalWL += net->calHPWL();
 

  return min_out_area *10  +  totalWL;
}

void SA::SAfloorplanning(double epsilon, double r, int k, bool forWL, vector<int>& initNPE, vector<int>& bestNPE)
{
  bestNPE = initNPE;
  int MT, uphill, reject; MT = uphill = reject = 0;
  int N = k * HBList.size();
  vector<int> curNPE = initNPE;
  int cur_cost = GetCost(curNPE, forWL);
  int best_cost = cur_cost;
  if(best_cost == 0)
  { 
    GetCost(bestNPE, true); return; 
  }
  do
  {
    double T0 = 1000;
    do
    {
      MT = uphill = reject = 0;
      do
      {
        int M = 0;
        if(forWL == false)  M = rand() % 3;
        vector<int> tryNPE = Perturb(curNPE, M);
        MT += 1;
        int try_cost = GetCost(tryNPE, forWL);
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
              GetCost(bestNPE, true); return; 
            }
          }
        }
        else
        {
          reject += 1;
        }
      }while(uphill <= N && MT <= 2*N);
      T0 = r * T0;
      // if(!forWL)cout<<T0<<endl;
    }while(reject/MT <= 0.95 && T0 >= epsilon);
  }while (forWL == false);
  GetCost(bestNPE, true); return; 
}

int SA::Run()
{
  unsigned seed = 2;
  srand(seed);
  int totalWL = 0;
  vector<int> initNPE, bestNPE, finalNPE;
  InitNPE(initNPE);
  SAfloorplanning(0.1, 0.9, 10, false, initNPE, bestNPE);

  for(auto& net: NetList)totalWL += net->calHPWL();

  cout << "Find a feasible floorplan.\n" << "Total wirelength: " << totalWL << "\n";

  finalNPE = bestNPE;
  SAfloorplanning(10, 0.75, 5, true, bestNPE, finalNPE);//set the parameter so can debug fast, need to adjust for better solution.
  
  
  int finalWL = 0;
  for(auto& net: NetList)finalWL += net->calHPWL();
  cout << "Find a better floorplan.\n" << "Total wirelength: " << finalWL << "\n";

  return finalWL;
}