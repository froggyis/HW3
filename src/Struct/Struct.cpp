#include <algorithm>
#include <climits>
#include <iostream>
#include "Struct.hpp"

// void HardBlock::Update(int& new_width, int& new_height, int& new_x, int& new_y)
// {
//   coor.first = new_x;
//   coor.second = new_y;


//   if(width == new_width && height == new_height)
//   {
//     rotated = false;
//   }
//   else
//   {
//     rotated = true;
//   }
//   center_pin->x_cor = new_x + new_width/2;
//   center_pin->y_cor = new_y + new_height/2;
// }

int net::calHPWL()
{
  int max_x, max_y, min_x, min_y;

  if(pins[0]->x_cor > hardblocks[0]->center_pin->x_cor)
  {
    max_x = hardblocks[0]->center_pin->x_cor;
    min_x = pins[0]->x_cor;
  }
  else 
  {
    max_x = pins[0]->x_cor;
    min_x = hardblocks[0]->center_pin->x_cor;
  }

 if(pins[0]->y_cor > hardblocks[0]->center_pin->y_cor)
  {
    max_y = hardblocks[0]->center_pin->y_cor;
    min_y = pins[0]->y_cor;
  }

  else 
  {
    max_y = pins[0]->y_cor;
    min_y = hardblocks[0]->center_pin->y_cor;
  }

  min_x = INT_MAX;max_x = INT_MIN;
  min_y = INT_MAX;max_y = INT_MIN;

  for(const auto& pin: pins)
  {
    if(pin->x_cor < min_x)  min_x = pin->x_cor;
    if(pin->x_cor > max_x)  max_x = pin->x_cor;
    if(pin->y_cor < min_y)  min_y = pin->y_cor;
    if(pin->y_cor > max_y)  max_y = pin->y_cor;
  }
  for(const auto& hb: hardblocks)
  {
    if(hb->center_pin->x_cor < min_x) min_x = hb->center_pin->x_cor;
    if(hb->center_pin->x_cor > max_x) max_x = hb->center_pin->x_cor;
    if(hb->center_pin->y_cor < min_y) min_y = hb->center_pin->y_cor;
    if(hb->center_pin->y_cor > max_y) max_y = hb->center_pin->y_cor;
  }
  // return (max_x - min_x) + (max_y - min_y);
  return (max_x + max_y) - (min_x + min_y);
  
}

void TreeNode::updateShape()
{
  int i,j;
  shape.clear();
  if(type == -1)
  {
    sort(lchild->shape.begin(), lchild->shape.end() );
    sort(rchild->shape.begin(), rchild->shape.end() );
    i = 0;j = 0;
    while(i < lchild->shape.size() && j < rchild->shape.size())
    {
      auto cur_shape = {lchild->shape[i][0]+rchild->shape[j][0], max( lchild->shape[i][1] , rchild->shape[j][1]), i, j};
      shape.emplace_back(cur_shape);
      if(lchild->shape[i][1] > rchild->shape[j][1] )++i;
      else if( lchild->shape[i][1] < rchild->shape[j][1])++j;
      else
      {
        ++i;++j;
      }
    }
  }

  else
  {
    sort(lchild->shape.begin(), lchild->shape.end(), greater<>() );
    sort(rchild->shape.begin(), rchild->shape.end(), greater<>() );
    i = 0;j = 0;
    while(i < lchild->shape.size() && j < rchild->shape.size())
    {
      auto cur_shape = { max( lchild->shape[i][0],rchild->shape[j][0]), lchild->shape[i][1]+ rchild->shape[j][1], i, j};
      shape.emplace_back(cur_shape);
      if( lchild->shape[i][0]  > rchild->shape[j][0])++i;
      else if( lchild->shape[i][0]  < rchild->shape[j][0] )++j;
      else
      {
        ++i;++j;
      }
    }
  }
}

