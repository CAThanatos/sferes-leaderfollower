/*
** robot.cc
** Login : <mouret@asuncion.lip6.fr>
** Started on  Mon Jan 14 16:40:38 2008 Jean-Baptiste MOURET
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

#include <iostream>
#include <list>
#include <cmath>

#include "robot.hpp"

namespace fastsim
{
  Robot :: Robot() :
    _radius(20.0f),
    _pos(0, 0, 0),
    _left_bumper(false),
    _right_bumper(false),
    _use_camera(false),
    _collision(false),
    _color(1),
    _display_color(1)
  {
    _bb.w = _radius * 2 + 8;
    _bb.h = _radius * 2 + 8;      
    _ill_sw = boost::shared_ptr<IlluminatedSwitch>(new IlluminatedSwitch(_color, _radius + 1, _pos.x(), _pos.y(), true, _bb.x, _bb.y, _bb.w, _bb.h));
    _update_bb();

    _create_display_status_map();
  }

  Robot :: Robot(float radius) : 
    _radius(radius),
    _pos(0, 0, 0),
    _left_bumper(false),
    _right_bumper(false),
    _use_camera(false),
    _collision(false),
    _color(1),
    _display_color(1)
  {
    _bb.w = _radius * 2 + 8;
    _bb.h = _radius * 2 + 8;      
    _ill_sw = boost::shared_ptr<IlluminatedSwitch>(new IlluminatedSwitch(_color, _radius + 1, _pos.x(), _pos.y(), true, _bb.x, _bb.y, _bb.w, _bb.h));
    _update_bb();

    _create_display_status_map();
  }

  Robot :: Robot(float radius, const Posture& pos) :
    _radius(radius), _pos(pos), 
    _left_bumper(false),
    _right_bumper(false),
    _use_camera(false),
    _collision(false),
    _color(1),
    _display_color(1)
  {
    _bb.w = _radius * 2 + 8;
    _bb.h = _radius * 2 + 8;      
    _ill_sw = boost::shared_ptr<IlluminatedSwitch>(new IlluminatedSwitch(_color, _radius + 1, _pos.x(), _pos.y(), true, _bb.x, _bb.y, _bb.w, _bb.h));
    _update_bb();

    _create_display_status_map();
  }

  Robot :: Robot(float radius, const Posture& pos, int color, int display_color) :
    _radius(radius), _pos(pos), 
    _left_bumper(false),
    _right_bumper(false),
    _use_camera(false),
    _collision(false),
    _color(color)
  {
  	if(display_color == -1)
			_display_color = (unsigned int)color;
		else
			_display_color = (unsigned int)display_color;  
  	
    _bb.w = _radius * 2 + 8;
    _bb.h = _radius * 2 + 8;      
    _ill_sw = boost::shared_ptr<IlluminatedSwitch>(new IlluminatedSwitch(_color, _radius + 1, _pos.x(), _pos.y(), true, _bb.x, _bb.y, _bb.w, _bb.h));
    _update_bb();

    _create_display_status_map();
  }
    
  void Robot :: move(float v1, float v2, const boost::shared_ptr<Map>& m)
  {
    // We set if off so that we're sure it doesn't interfere with the sensors
    _ill_sw->set_off();
  
    Posture prev = _pos;
    _pos.move(v1, v2, _radius * 2);
    _update_bb();
    
    // update bumpers & go back if there is a collision
    if(_check_collision(m))
    {
			float theta = _pos.theta();
			_pos = prev;
			_pos.set_theta(theta);
			_collision = true;
			_update_bb();
    }
    else
    	_collision = false;
     
    // Update the illuminated switch position
    _ill_sw->set_pos(_pos.x(), _pos.y());

    std::vector<float> vec_dist = std::vector<float>(_lasers.size());
    for(size_t i = 0; i < _lasers.size(); ++i)
      vec_dist[i] = -1.0f;

    float x1 = _pos.x();
    float y1 = _pos.y();
    float laser_range = _lasers[0].get_range();
    for (size_t i = 0; i < m->get_illuminated_switches().size(); ++i)
    {
      Map::ill_sw_t isv = m->get_illuminated_switches()[i];

      if(isv->get_on())
      {
        float x_isv = isv->get_x();
        float y_isv = isv->get_y();
        float r = isv->get_radius();

        float dist = sqrtf((x_isv - x1) * (x_isv - x1) + (y_isv - y1) * (y_isv - y1));

        if((dist - r) <= laser_range)
        {
          float ang = normalize_angle(asin((y_isv - y1)/dist));

          if(x1 >= x_isv)
          {
            if(y1 >= y_isv)
              ang = -M_PI/2 - (M_PI/2 + ang);
            else
              ang = M_PI/2 + (M_PI/2 - ang);
          }

          float x_isv2 = x_isv + cosf(normalize_angle(ang + M_PI/2)) * r;
          float y_isv2 = y_isv + sinf(normalize_angle(ang + M_PI/2)) * r;
          float dist2 = sqrtf((x_isv2 - x1) * (x_isv2 - x1) + (y_isv2 - y1) * (y_isv2 - y1));
          float limAng1 = normalize_angle(asin((y_isv2 - y1)/dist2));

          if(x1 >= x_isv2)
          {
            if(y1 >= y_isv2)
              limAng1 = -M_PI/2 - (M_PI/2 + limAng1);
            else
              limAng1 = M_PI/2 + (M_PI/2 - limAng1);
          }

          float limAng2 = normalize_angle(ang - (limAng1 - ang));
    
          for (size_t j = 0; j < _lasers.size(); ++j)
          {
            if((vec_dist[j] < 0.0f) || (dist < vec_dist[j]))
            {
              float angle = normalize_angle(_lasers[j].get_angle() + _pos.theta());

              if(limAng2 < limAng1)
              {
                if(angle >= limAng2 && angle <= limAng1)
                  vec_dist[j] = dist - r;
              }
              else
              {
                if((angle >= limAng2 && angle <= M_PI) || (angle <= limAng1 && angle >= -M_PI))
                  vec_dist[j] = dist - r;
              }
            }
          }
        }
      }
    }

    // Walls
    for (size_t i = 0; i < m->get_walls().size(); ++i)
    {
      const Wall& w = m->get_walls()[i];

      float x1_w = w.get_x1();
      float y1_w = w.get_y1();
      float x2_w = w.get_x2();
      float y2_w = w.get_y2();

      float x2 = x1;
      float y2 = y1;
      float angle_wall = 0.0f;

      if(x1 < x1_w)
      {
        x2 = x1_w;
        angle_wall = 0.0f;
      }
      else if(x1 > x2_w)
      {
        x2 = x2_w;
        angle_wall = M_PI;
      }

      if(y1 < y1_w)
      {
        y2 = y1_w;
        angle_wall = M_PI/2.0f;
      }
      else if(y1 > y2_w)
      {
        y2 = y2_w;
        angle_wall = -M_PI/2.0f;
      }

      float dist = sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

      if(dist <= laser_range)
      {
        for(size_t j = 0; j < _lasers.size(); ++j)
        {
          float angle = normalize_angle(_pos.theta() + _lasers[j].get_angle());
          angle = normalize_angle(angle - angle_wall);
          if(fabs(angle) <= M_PI/2)
          {
            float distr;

            if(x2 == x1)
              distr = fabs((y2 - y1) / cosf(angle));
            else if(y2 == y1)
              distr = fabs((x2 - x1) / cosf(angle));
            else
              distr = 0.0f; // TODO

            if(distr <= laser_range)
            {
              if((vec_dist[j] < 0.0f) || (dist < vec_dist[j]))
                vec_dist[j] = distr;
            }
          }
        }
      }
    }

    // update lasers
    for (size_t i = 0; i < _lasers.size(); ++i)
      _lasers[i].update(_pos, m, vec_dist[i]);
      
    // // update radars
    // for (size_t i = 0; i < _radars.size(); ++i)
    //   _radars[i].update(_pos, m);
      
    // // update light sensors
    // for (size_t i = 0; i < _light_sensors.size(); ++i)
    //   _light_sensors[i].update(_pos, m);
      
    // update camera
    if (_use_camera)
      _camera.update(_pos, m);
    
    // And we set the illuminated switch on again
    _ill_sw->set_on(true);
  }
    
  void Robot :: teleport(Posture& pos, const boost::shared_ptr<Map>& m)
  {
    std::cout << "WARNING : function not implemented ! You are not supposed to use it !" << std::endl;
//     // We set if off so that we're sure it doesn't interfere with the sensors
//     _ill_sw->set_off();

//     Posture prev = _pos;
// //    _pos.move(v1, v2, _radius * 2);
// 		_pos = pos;
//     _update_bb();
    
//     // update bumpers & go back if there is a collision
//     if(_check_collision(m))
//     {
// 			float theta = _pos.theta();
// 			_pos = prev;
// 			_pos.set_theta(theta);
// 			_collision = true;
// 			_update_bb();
//     }
//     else
//     	_collision = false;
     
//     // Update the illuminated switch position
//     _ill_sw->set_pos(_pos.x(), _pos.y());
        
//     // update lasers
//     // for (size_t i = 0; i < _lasers.size(); ++i)
//     //   _lasers[i].update(_pos, m);
      
//     // update radars
//     for (size_t i = 0; i < _radars.size(); ++i)
//       _radars[i].update(_pos, m);
      
//     // update light sensors
//     for (size_t i = 0; i < _light_sensors.size(); ++i)
//       _light_sensors[i].update(_pos, m);
      
//     // update camera
//     if (_use_camera)
//       _camera.update(_pos, m);
    
//     // And we set the illuminated switch on again
//     _ill_sw->set_on(true);
  }
  void Robot :: _update_bb()
  {
    // robot bb
    _bb.x = _pos.x() - _radius - 4;
    _bb.y = _pos.y() - _radius - 4;
    
    // switch bb
    _ill_sw->set_bb_pos(_bb.x, _bb.y);
  }

  bool Robot :: _check_collision(const boost::shared_ptr<Map>& m)
  {
    return m->check_collision(_pos.x(), _pos.y(), _radius);
  }
}
