#pragma once
#include <vector>
#include "../Module/module.h"
using namespace std;


extern vector<HardBlock*> HBList;

class SA
{ 
  private:
    vector<TreeNode*> hbNodeList, cutNodeList;
    void InitNPE(vector<int>& NPE);
    // void Complement(vector<int>& curNPE, int startIdx);
    bool isSkewed(vector<int>& curNPE, int i);
    bool violateBallot(vector<int>& curNPE, int i);
    vector<int> Perturb(vector<int> curNPE, int M);
    TreeNode* ConstructTree(vector<int>& NPE);
    void PlaceBlock(TreeNode* node, int shapeIdx, int new_x, int new_y);
    int CalTotalHPWL();
    int CalCost(vector<int>& NPE, bool const & forWL);
    void SAfloorplanning(double epsilon, double r, int k, bool forWL, vector<int>& curNPE, vector<int>& bestNPE);
    
  public:
    double region_side_len;
    // SA(double dead_space_ratio) { CalSideLen(dead_space_ratio); }
    SA()
    {
      for(int i = 0; i < HBList.size(); ++i)
      {
        HardBlock* cur_hb = HBList[i];
        TreeNode* hbNode = new TreeNode(0, cur_hb);
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

