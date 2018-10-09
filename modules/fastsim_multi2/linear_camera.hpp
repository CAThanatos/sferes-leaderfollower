#ifndef FASTSIM_LINEAR_CAMERA_HPP_
#define FASTSIM_LINEAR_CAMERA_HPP_

#include <boost/shared_ptr.hpp>
#include "posture.hpp"
#include "map.hpp"

#define SYMETRIE_CAMERA

// #define MAP_WALL

namespace fastsim
{
  class LinearCamera
  {
    public:
    LinearCamera() : _angular_range(M_PI / 2), _range(10000), _pixels(12), _dist(12)
    { std::fill(_pixels.begin(), _pixels.end(), -1); std::fill(_dist.begin(), _dist.end(), 0); }
    LinearCamera(float angular_range, int nb_pixels, float range = 10000) :
      _angular_range(angular_range), _range(range), _pixels(nb_pixels), _dist(nb_pixels)
    { std::fill(_pixels.begin(), _pixels.end(), -1); std::fill(_dist.begin(), _dist.end(), 0); }
   
    void update(const Posture& pos,
	       const boost::shared_ptr<Map>& map);
    const std::vector<int>& pixels() const { return _pixels; }
    const std::vector<float>& dist() const { return _dist; }
    void set_pixel(size_t i, int pixel) { _pixels[i] = pixel; }
    void set_dist(size_t i, float dist) { _dist[i] = dist; }
    float get_angular_range() const { return _angular_range; }
    float get_range() const { return _range; }
  protected:
    float _angular_range;
    float _range;
    std::vector<int> _pixels;
    std::vector<float> _dist;
  };
}
#endif
