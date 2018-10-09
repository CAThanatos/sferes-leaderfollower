#ifndef FASTSIM_WALL_HPP_
#define FASTSIM_WALL_HPP_

#include <vector>

namespace fastsim
{
  class Wall
  {
    public:
      Wall(float x1, float y1, float x2, float y2) : _x1(x1), _y1(y1), _x2(x2), _y2(y2) {}

      float get_x1() const { return _x1; }
      float get_y1() const { return _y1; }
      float get_x2() const { return _x2; }
      float get_y2() const { return _y2; }

      void set_x1(float x1) { _x1 = x1; }
      void set_y1(float y1) { _y1 = y1; }
      void set_x2(float x2) { _x2 = x2; }
      void set_y2(float y2) { _y2 = y2; }

    protected:
      float _x1, _y1, _x2, _y2;
  };
}

#endif