#include "Parser.hpp"
#include  <iostream>
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
            pin* center_pin = new pin(hb_name, center_x, center_y);
            HardBlock *HB = new HardBlock(hb_name, width, height, center_pin, x[0], y[0]);
            HBList.emplace_back(HB);
            HBTable[hb_name] = HB;
            PinTable[hb_name] = HB->center_pin;
        }
    }
}


void Parser::read_net(string const &filename)
{
    ifstream fin_nets(filename);
    string tmp;
    while(getline(fin_nets, tmp))
    {
        if(tmp[3] != 'D') continue;
        stringstream ss(tmp);
        string temp, colon;
        int degree;
        while(ss >> temp >> colon >> degree)
        {
            net *cur_net = new net(degree);
            NetList.emplace_back(cur_net);
            for(int i = 0; i < degree; ++i)
            {
            string terminal;
            fin_nets >> terminal;
            if(terminal[0] == 'p')
            { 
                auto fixed_pin = PinTable[terminal];
                NetList.back()->pins.emplace_back(fixed_pin);
            }
            else
            {
                auto hb_pin = HBTable[terminal];
                NetList.back()->hardblocks.emplace_back(hb_pin);
            }
            }
        }
    }
  
}

    
void Parser::read_pin(string const &filename)
{

    ifstream fin_pl(filename);
    string pin_name;
    int x_cor, y_cor;
    while(fin_pl >> pin_name >> x_cor >> y_cor)
    {
        pin *cur_pin = new pin(pin_name, x_cor, y_cor);
        PinTable[pin_name] = cur_pin;
    }

}

