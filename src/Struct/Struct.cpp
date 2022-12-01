#include <algorithm>
#include <climits>
#include <iostream>
#include "Struct.hpp"

int net::calHPWL()
{
  int max_x, max_y, min_x, min_y;

  if(pins[0]->x > hardblocks[0]->center->x)
  {
    max_x = hardblocks[0]->center->x;
    min_x = pins[0]->x;
  }
  else 
  {
    max_x = pins[0]->x;
    min_x = hardblocks[0]->center->x;
  }

 if(pins[0]->y > hardblocks[0]->center->y)
  {
    max_y = hardblocks[0]->center->y;
    min_y = pins[0]->y;
  }

  else 
  {
    max_y = pins[0]->y;
    min_y = hardblocks[0]->center->y;
  }

  min_x = INT_MAX;max_x = INT_MIN;
  min_y = INT_MAX;max_y = INT_MIN;

  for(const auto& pin: pins)
  {
    if(pin->x < min_x)  min_x = pin->x;
    if(pin->x > max_x)  max_x = pin->x;
    if(pin->y < min_y)  min_y = pin->y;
    if(pin->y > max_y)  max_y = pin->y;
  }
  for(const auto& hb: hardblocks)
  {
    if(hb->center->x < min_x) min_x = hb->center->x;
    if(hb->center->x > max_x) max_x = hb->center->x;
    if(hb->center->y < min_y) min_y = hb->center->y;
    if(hb->center->y > max_y) max_y = hb->center->y;
  }
  return (max_x + max_y) - (min_x + min_y);
  
}

void TreeNode::stockmeyer()
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

