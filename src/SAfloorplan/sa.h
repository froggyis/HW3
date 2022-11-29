#pragma once
#include <vector>
#include "../Module/module.h"
using namespace std;

class SA
{ 
  private:
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
    SA(){};
    int Run();

};

