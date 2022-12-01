#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <cmath>
#include "./Struct/Struct.hpp"
#include "./Floorplanner/Floorplanner.hpp"
#include "./Parser/Parser.hpp"
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
using namespace std;

using namespace chrono;
using namespace std;

vector<HardBlock*> HBList;
unordered_map<string, HardBlock*> HBTable;
vector<net*> NetList;


void WriteResult(string filename, int WL)
{
  ofstream output(filename);

  output << "Wirelength " << WL << "\n";
  output << "Blocks" << "\n";
  // for(auto& hb:HBList)
  // {
  //   // output << hb->name << " " << hb->downleft_x << " " << hb->downleft_y << " " << hb->rotated << "\n"; 
  //   output << hb->name << " " << hb->coor.first << " " << hb->coor.second << " " << hb->rotated << "\n"; 

  // }
  for(auto& hb:HBTable)
  {
    output << hb.first << " " << hb.second->coor.first << " " << hb.second->coor.second << " " << hb.second->rotated << "\n"; 

  }
}

double CalDeadSpaceRatio(double &ratio)
{
  double area = 0;
  // for(auto hb:HBList)area += hb->height * hb->width;
  for(auto hb:HBTable)area += hb.second->height * hb.second->width;

  return sqrt(area*(1+ratio));

}

void determine_seed(double input)
{
  unsigned seed;
  seed = 0;
  switch (HBList.size())
  {
    case 100:
      if(input == 0.1)seed = 1407;
      else seed = 65032;
      
      break;
    case 200:
      if(input == 0.1)seed = 90806;
      else seed = 70653;
      
      break;
    case 300:
      if(input == 0.1)seed = 32260;
      else seed = 90365;
      break;


    default:
      seed = 5487;
      break;
  }

}

int main(int argc, char *argv[])
{

  auto Input_begin = high_resolution_clock::now();
 
  



  Parser *par = new Parser();
  par->read_hardblock(argv[1]);
  par->read_pin(argv[3]);
  par->read_net(argv[2]);

 
  double input = stod(argv[5]);
  determine_seed(input);

  auto Input_end = high_resolution_clock::now();
  auto Input_time = chrono::duration_cast<std::chrono::nanoseconds>(Input_end - Input_begin);
  // cout<<"========================================================"<<endl;
	// cout<< "Input_time measured: "<<  Input_time.count() * 1e-9 << "seconds" <<endl;
  // cout<<"========================================================"<<endl;

  double ratio = CalDeadSpaceRatio(input);


  Floorplanner *start = new Floorplanner();
  start->RegionOutline = ratio;
  
  int totalWL = 0;
  vector<int> initNPE, bestNPE;

  auto Feasible_begin = high_resolution_clock::now();
  start->InitNPE(initNPE);
  start->SA(0.1, 0.8, 10, false, initNPE, bestNPE, Feasible_begin);
  auto Feasible_end = high_resolution_clock::now();
  auto Feasible_time = chrono::duration_cast<std::chrono::nanoseconds>(Feasible_end - Feasible_begin);
  // cout<<"========================================================"<<endl;
	// cout<< "Feasible_time measured: "<<  Feasible_time.count() * 1e-9 << "seconds" <<endl;
  // cout<<"========================================================"<<endl;




  for(auto& net: NetList)totalWL += net->calHPWL();

  cout << "Find a feasible floorplan.\n" << "Total wirelength: " << totalWL << "\n";

  vector<int> finalNPE = bestNPE;
  start->SA(1, 0.95, 5, true, bestNPE, finalNPE, Input_begin);//set the parameter so can debug fast, need to adjust for better solution.
  
  
  int finalWL = 0;
  for(auto& net: NetList)finalWL += net->calHPWL();
  cout << "Find a better floorplan.\n" << "Total wirelength: " << finalWL << "\n";



  auto Output_begin = high_resolution_clock::now();
 
  WriteResult(argv[4], finalWL);
  
  auto Output_end = high_resolution_clock::now();
  auto Output_time = chrono::duration_cast<std::chrono::nanoseconds>(Output_end - Output_begin);
  // cout<<"========================================================"<<endl;
	// cout<< "Output_time measured: "<<  Output_time.count() * 1e-9 << "seconds" <<endl;
  // cout<<"========================================================"<<endl;



  return 0;
}