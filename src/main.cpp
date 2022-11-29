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
#include "./Module/module.h"
#include "./SAfloorplan/sa.h"
#include "./Parser/Parser.hpp"

using namespace chrono;
using namespace std;

// unordered_map<string, pin*> PinTable;
vector<HardBlock*> HBList;
unordered_map<string, HardBlock*> HBTable;
vector<net*> NetList;

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

double CalDeadSpaceRatio(double &ratio)
{
  double area = 0;
  for(auto hb:HBList)area += hb->height * hb->width;
  return sqrt(area*(1+ratio));

}


int main(int argc, char *argv[])
{
  auto begin = high_resolution_clock::now();

 

  Parser *par = new Parser();
  par->read_hardblock(argv[1]);
  par->read_pin(argv[3]);
  par->read_net(argv[2]);

 
  double input = stod(argv[5]);
  double ratio = CalDeadSpaceRatio(input);
  // SA sa(stod(argv[5]));
  SA *sa = new SA();
  sa->region_side_len = ratio;
  int finalWL = sa->Run();
 
  WriteResult(argv[4], finalWL);
  
  auto end = high_resolution_clock::now();
  auto time = chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
  cout<<"========================================================"<<endl;
	cout<< "Time measured: "<<  time.count() * 1e-9 << "seconds" <<endl;
  cout<<"========================================================"<<endl;


  return 0;
}