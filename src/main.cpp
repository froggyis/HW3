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
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
using namespace std;
void mem_usage(double& vm_usage, double& resident_set) {
   vm_usage = 0.0;
   resident_set = 0.0;
   ifstream stat_stream("/proc/self/stat",ios_base::in); //get info from proc directory
   //create some variables to get info
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;
   unsigned long vsize;
   long rss;
   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
   >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
   >> utime >> stime >> cutime >> cstime >> priority >> nice
   >> O >> itrealvalue >> starttime >> vsize >> rss; // don't careabout the rest
   stat_stream.close();
   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // for x86-64 is configuredto use 2MB pages
   vm_usage = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}

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

   double vm, rss;
   mem_usage(vm, rss);
   cout << "Virtual Memory: " << vm << "Resident set size: " << rss << endl;


  return 0;
}