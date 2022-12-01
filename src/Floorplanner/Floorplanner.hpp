#pragma once
#include <vector>
#include "../Struct/Struct.hpp"
using namespace std;


extern vector<HardBlock*> HBList;

class SA
{ 
  private:
    vector<TreeNode*> hbNodeList, cutNodeList;
    void InitNPE(vector<int>& NPE);//C
    bool Skewed(vector<int>& curNPE, int idx);//C
    bool Ballot(vector<int>& curNPE, int i);//C
    vector<int> Perturb(vector<int> curNPE, int M);//M3 NC
    TreeNode* ConstructTree(vector<int>& NPE);//NC
    void PlaceBlock(TreeNode* node, int shapeIdx, int new_x, int new_y);//NC
    // int CalTotalHPWL();
    int GetCost(vector<int>& NPE, bool const & forWL);//WC
    void SAfloorplanning(double epsilon, double r, int k, bool forWL, vector<int>& curNPE, vector<int>& bestNPE);//WC
    
  public:
    double RegionOutline;
    SA()
    {
      for(int i = 0; i < HBList.size(); ++i)
      {
        HardBlock* cur_hb = HBList[i];
        TreeNode* hbNode = new TreeNode(0, cur_hb);
        hbNode->shape = { {cur_hb->width, cur_hb->height, 0, 0}, {cur_hb->height, cur_hb->width, 1, 1}};
        hbNodeList.emplace_back(hbNode);
      }

      for(int i = 0; i < HBList.size()-1; ++i)
      {
        TreeNode* cutNode = new TreeNode(-1);
        cutNodeList.emplace_back(cutNode);
      }

    };
    int Run();

};

