#include "Parser.hpp"
#include  <iostream>
#include <utility> 
using namespace std;
extern vector<HardBlock*> HBList;
extern unordered_map<string, HardBlock*> HBTable;
extern vector<net*> NetList;

unordered_map <string, pin*>PinTable;


void Parser::read_hardblock(string const &filename)
{
    ifstream in_hardblock(filename);
    string s;
    int width, height;
    int x[4], y[4];
    while(getline(in_hardblock, s))
    {
        if(s == "" || s[0] != 's') continue;
        stringstream ss(s);
        string hb_name, tmp1, tmp2, x0, y0, x1, y1, x2, y2, x3, y3;

        while(ss >> hb_name >> tmp1 >> tmp2 >> x0 >> y0 >> x1 >> y1 >> x2 >> y2 >> x3 >> y3)
        {
            x[0] = stoi(x0.substr(1, x0.size() - 2));
            y[0] = stoi(y0.substr(0, y0.size() - 1));

            x[1] = stoi(x1.substr(1, x1.size() - 2));
            y[1] = stoi(y1.substr(0, y1.size() - 1));

            x[2] = stoi(x2.substr(1, x2.size() - 2));
            y[2] = stoi(y2.substr(0, y2.size() - 1));

            x[3] = stoi(x3.substr(1, x3.size() - 2));
            y[3] = stoi(y3.substr(0, y3.size() - 1));
            width = *max_element(x, x+4) - *min_element(x, x+4);
            height = *max_element(y, y+4) - *min_element(y, y+4);

            int center_x = *min_element(x, x+4) + width/2;
            int center_y = *min_element(y, y+4) + height/2;
            pin* center = new pin(hb_name, center_x, center_y);
            HardBlock *HB = new HardBlock(hb_name, width, height, center, make_pair(x[0], y[0]) );

            HBList.emplace_back(HB);
            HBTable[hb_name] = HB;
            PinTable[hb_name] = HB->center;
        }
    }
}


void Parser::read_net(string const &filename)
{
    
    ifstream in_net(filename);
    string tmp,_,pin_name_;
    string tmp_;
    // string tmp_, _, pin_name_ ;
    int pin_degree;
    while(in_net>>tmp_)
    {
        if(tmp_ == "NetDegree")
        {
            net *input_net = new net();
            in_net >> _ >> pin_degree;
            NetList.emplace_back(input_net);
            for(int i = 0 ; i<pin_degree ; i++)
            {
                in_net >> pin_name_;
                if(pin_name_[0] =='p')
                { //push p1 p2.... into this net
                    auto fix_pin = PinTable[pin_name_];
                    NetList.back()->pins.emplace_back(fix_pin);
                }
                else
                { //push hardblock's center, ex: sb0 into this net
                    auto hb_pin = HBTable[pin_name_];
                    NetList.back()->hardblocks.emplace_back(hb_pin);
                }
            
            }
    
        }
    }
  
}

    
void Parser::read_pin(string const &filename)
{

    ifstream in_pin(filename);
    string pin_name;
    int x_cor, y_cor;
    while(in_pin >> pin_name >> x_cor >> y_cor)
    {
        pin *cur_pin = new pin(pin_name, x_cor, y_cor);
        PinTable[pin_name] = cur_pin;
    }

}

