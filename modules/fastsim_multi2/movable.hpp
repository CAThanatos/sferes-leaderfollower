/*
** robot.hh
** Login : <mouret@asuncion.lip6.fr>
** Started on  Mon Jan 14 16:40:06 2008 Jean-Baptiste MOURET
** $Id$
** 
** Copyright (C) 2008 Jean-Baptiste MOURET
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef   	MOVABLE_HH_
#define   	MOVABLE_HH_

#include <vector>
#include <map>

#include "map.hpp"
#include "posture.hpp"


namespace fastsim
{
  class Movable
  {
  public:
    struct BoundingBox { float x, y, w, h; };
    Movable();
    Movable(float radius);
    Movable(float radius, const Posture& pos);    
    Movable(float radius, const Posture& pos, int color, int display_color = -1);

		Movable(const Movable& source)
		{
		}
		
		virtual ~Movable() { }

    //
    void reinit()
    {
      _collision = false;
      _left_bumper = false;
      _right_bumper = false;
    }
    void move(float v1, float v2, const boost::shared_ptr<Map>& m);
    void teleport(Posture& pos, const boost::shared_ptr<Map>& m);
    const Posture& get_pos() const { return _pos; }
    void set_pos(const Posture& pos) { _pos = pos; _ill_sw->set_pos(_pos.x(), _pos.y()); /*_update_bb();*/ }
    const BoundingBox& get_bb() const { return _bb; }
    float get_radius() const { return _radius; }
    bool get_collision() const { return _collision;}
    //
    bool get_left_bumper() const { return _left_bumper; }
    bool get_right_bumper() const { return _right_bumper; }
    
    boost::shared_ptr<IlluminatedSwitch>& get_ill_sw() { return _ill_sw; }

    // lasers
    void add_laser(const Laser& l) { _lasers.push_back(l); }
    std::vector<Laser>& get_lasers() { return _lasers; }

    // radars
    void add_radar(const Radar& r){ _radars.push_back(r); }
    const std::vector<Radar>& get_radars() const { return _radars; }

    // light sensors
    void add_light_sensor(const LightSensor& l) { _light_sensors.push_back(l); }
    const std::vector<LightSensor>& get_light_sensors() const { return _light_sensors; }
    void set_color(int color) {_color=color; update_illuminated_sw_color(_color); }
    int color() const {return _color;}
    void set_display_color(unsigned int display_color) { _display_color = display_color; }
    unsigned int display_color() const { return _display_color; }

    // camera
    void use_camera(const LinearCamera& c) { _camera = c; _use_camera = true; }
    void use_camera() { _use_camera = true; }
    const LinearCamera& get_camera() const { return _camera; }
    bool is_using_camera() const { return _use_camera; }

    void update_illuminated_sw_color(int color) { _ill_sw->set_color(color); }

    void set_display_status(std::string status, unsigned int color) { _display_status_map[status] = color; }
    void remove_display_status(std::string status) { _display_status_map[status] = -1; }
    int get_display_status(std::string status) { return _display_status_map[status]; }
  protected:
    bool _check_collision(const boost::shared_ptr<Map>& m);
    void _update_bb();

    void _create_display_status_map()
    {
      _display_status_map["circle_ext"] = -1;
      _display_status_map["circle_int"] = -1;
      _display_status_map["cross"] = -1;
    }
	  
    float _radius;
    Posture _pos;
    BoundingBox _bb;
    bool _left_bumper, _right_bumper;
    std::vector<Laser> _lasers;
    std::vector<Radar> _radars;
    std::vector<LightSensor> _light_sensors;
    LinearCamera _camera;
    bool _use_camera;
    bool _collision;
    int _color;

    unsigned int _display_color;

    std::map<std::string, int> _display_status_map;
    
    boost::shared_ptr<IlluminatedSwitch> _ill_sw;
  };

  struct ClosestRobot_f
  {
    ClosestRobot_f(float x, float y) : _x(x), _y(y) {}
    bool operator()(const boost::shared_ptr<Robot> r1,
		    const boost::shared_ptr<Robot> r2)
    {
      float x1 = r1->get_pos().x() - _x;
      float y1 = r1->get_pos().y() - _y;
      float x2 = r2->get_pos().x() - _x;
      float y2 = r2->get_pos().y() - _y;
      float d1 = sqrtf(x1 * x1 + y1 * y1);
      float d2 = sqrtf(x2 * x2 + y2 * y2);
      return d1 < d2;
    }
    float _x, _y;
  };
}


#endif	    /* !ROBOT_HH_ */
