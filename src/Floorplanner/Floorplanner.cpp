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
    if(cur_width + HB->width <= RegionOutline)
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




bool SA::Skewed(vector<int>& curNPE, int idx)//C
{
  if(idx+2 < curNPE.size())
    if(curNPE[idx] == curNPE[idx+2])return false;

  else if(idx-1 >= 0 && curNPE[idx-1] == curNPE[idx+1])
    if(idx-1>=0)return false;
  
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

vector<int> SA::Perturb(vector<int> curNPE, int M)//M3 NC
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
          if(Skewed(curNPE, i) && Ballot(curNPE, i))
          {
            SwapPos.emplace_back(i);
          }
        }
        else if(curNPE[i] < 0 && curNPE[i+1] >= 0)
        {
          if(Skewed(curNPE, i))
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
  int index = 0;
  vector<TreeNode*> st;
  for(auto &element:NPE)
  {
    //if this is a hardblock
    if(element >= 0)st.emplace_back(hbNodeList[element]);
 
    //if this is a cut node then we conduct the stockmeyer algorithm.
    else
    {
      //if current stack is |1|2|... then we push V into it
      //so we first pop out 1 and 2 it will look like 12V
      //following we conduct the algorithm to confirm which shape give us the least area
      //after that we push the 12V into stack |12v|
      //notice that the node 12V stands for the new abstract hardblock
      auto node = cutNodeList[index];
      index++;
      node->type = element;
      node->rchild = st.back();st.pop_back();
      node->lchild = st.back();st.pop_back();
      st.emplace_back(node);
      node->updateShape();
    }
  }
  return st.back(); // return back the pointer point to root
}

void SA::PlaceBlock(TreeNode* node, int shapeIdx, int x, int y)//not change
{
  if(node->type >= 0)
  {
    // node->hardblock->Update(node->shape[shapeIdx][0], node->shape[shapeIdx][1], x, y);
    // Whenever we place a new block, we need to check wether it's original placement is same as before
    // If so, then we know this block keep it's original shape
    // If dont, then we know this block has bennd rotated, so we maintain the coresspond information. 
     if(node->hardblock->width == node->shape[shapeIdx][0])
      node->hardblock->rotated=false;
    else node->hardblock->rotated = true;

    int new_width = node->shape[shapeIdx][0];
    int new_height = node->shape[shapeIdx][1];

    node->hardblock->coor.first = x;
    node->hardblock->coor.second = y;
   
    node->hardblock->center_pin->x_cor = x + new_width /2;
    node->hardblock->center_pin->y_cor = y + new_height /2;

  }
  else
  {
    PlaceBlock(node->lchild, node->shape[shapeIdx][2], x, y);

    int offset_x = 0, offset_y = 0;
    if(node->type == -1) offset_x = node->lchild->shape[node->shape[shapeIdx][2]][0];
    else offset_y = node->lchild->shape[   node->shape[shapeIdx][2]    ][1];
    PlaceBlock(node->rchild, node->shape[shapeIdx][3], x+offset_x, y+offset_y);

  }
}



int SA::GetCost(vector<int>& NPE, bool const& forWL)
{
  int min_dif_area = INT_MAX;
  int dif_area = 0;
  int shape = 0;
  TreeNode* root = ConstructTree(NPE);

  // Due to the structure we build , record.
  // We iterate all the possible shape to look for the area which can fit in the area
  // When the process is conduct by SA, it means whenever we got the new NPE
  // we get the cost calculated by area and WL to imporve the quality of answer.
  for(int i = 0; i < root->shape.size(); ++i)
  {
    // auto info = root->shape[i];
    // int width = info[0];
    // int height = info[1];
    int width = root->shape[i][0];
    int height = root->shape[i][1];

  
    if(width > RegionOutline && height > RegionOutline)
    { 
      dif_area = width * height - pow(RegionOutline,2);
    }
    else if(height > RegionOutline) dif_area = width * (height - RegionOutline);
    else if(width > RegionOutline) dif_area= height * (width - RegionOutline);
    else dif_area = 0;
    
    //We first focus on area, so we keep return the minimum area as cost
    //It might not be a good answer but is acceptable
    if(dif_area < min_dif_area)
    {
      min_dif_area = dif_area;
      shape = i;
    }
  }

  if(!forWL)  return min_dif_area * 10;
  //if we find the better solution
  //then we place the block based on the record we maintain before.
  PlaceBlock(root, shape, 0, 0);//we set x,y to zero because we always place the block from (0,0)
  int totalWL = 0;
  for(auto& net: NetList)totalWL += net->calHPWL();
 
  return min_dif_area *10  +  totalWL;
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