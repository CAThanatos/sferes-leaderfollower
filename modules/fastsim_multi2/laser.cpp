/*
** laser.cc
** Login : <mandor@ithaqua>
** Started on  Wed Jun  3 00:41:29 2009 mandor
** $Id$
** 
** Copyright (C) 2009 mandor
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
#include "laser.hpp"

namespace fastsim
{
  float Laser :: update(const Posture& pos,
            const boost::shared_ptr<Map>& m, float dist)
  {
    _dist = dist;
    return _dist;
  }  

  // float Laser :: update(const Posture& pos,
  //        const boost::shared_ptr<Map>& m)
  // {
  //   float x1 = pos.x();
  //   float y1 = pos.y();

  //   float x2 = cosf(_angle + pos.theta()) * _range + pos.x() + _gap_dist * cosf(_gap_angle + pos.theta());
  //   float y2 = sinf(_angle + pos.theta()) * _range + pos.y() + _gap_dist * sinf(_gap_angle + pos.theta());

  //   _dist = -1;
  //   for (size_t i = 0; i < m->get_illuminated_switches().size(); ++i)
  //   {
  //       Map::ill_sw_t isv = m->get_illuminated_switches()[i];

  //       if(isv->get_on())
  //       {
  //         float x_isv = isv->get_x();
  //         float y_isv = isv->get_y();
  //         float r_isv = isv->get_radius();

  //         float dist = sqrtf((x_isv - x1) * (x_isv - x1) + (y_isv - y1) * (y_isv - y1));

  //         if(dist <= _range)
  //         {
  //           if((_dist < 0.0f) || (dist < _dist))
  //           {
  //             if(m->check_inter_ray_circle(x1, y1, x2, y2, x_isv, y_isv, r_isv))
  //               _dist = dist;
  //           }
  //         }
  //       }
  //   }

  //   return _dist;
  // }  

  // float Laser :: update(const Posture& pos,
		// 	const boost::shared_ptr<Map>& m)
  // {
  //   float x2 = cosf(_angle + pos.theta()) * _range + pos.x() + _gap_dist * cosf(_gap_angle + pos.theta());
  //   float y2 = sinf(_angle + pos.theta()) * _range + pos.y() + _gap_dist * sinf(_gap_angle + pos.theta());

  //   // convert to pixel
  //   int xx1 = m->real_to_pixel(pos.x() + _gap_dist * cosf(_gap_angle + pos.theta()));
  //   int yy1 = m->real_to_pixel(pos.y() + _gap_dist * sinf(_gap_angle + pos.theta()));
  //   int xx2 = m->real_to_pixel(x2);
  //   int yy2 = m->real_to_pixel(y2);

  //   // check intersection

  //   _x_pixel = m->real_to_pixel(x2), _y_pixel = m->real_to_pixel(y2);

  //   bool inter = m->check_inter_pixel(xx1, yy1, xx2, yy2, _x_pixel, _y_pixel);
  //   _x_pixel = std::min(std::max(0, _x_pixel), (int)m->get_pixel_w());
  //   _y_pixel = std::min(std::max(0, _y_pixel), (int)m->get_pixel_h());
    
  //   assert(_x_pixel >= 0);
  //   assert(_y_pixel >= 0);

  //   // convert back to real
  //   _x = m->pixel_to_real(_x_pixel);
  //   _y = m->pixel_to_real(_y_pixel);


  //   if (!inter)
  //     _dist = -1;
  //   else
  //   {
		// 	float px = pos.x() + _gap_dist * cosf(_gap_angle + pos.theta()) - _x;
		// 	float py = pos.y() + _gap_dist * sinf(_gap_angle + pos.theta()) - _y;    
		// 	_dist = sqrtf(px * px  + py * py);
			
		// 	// Seems like conversions pixel/real can get strange cases where _dist > _range
		// 	_dist = std::min(_range, _dist);
  //   }

  //   return _dist;
  // }  
}
