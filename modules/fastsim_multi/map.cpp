/*
** map.cc
** Login : <mouret@asuncion.lip6.fr>
** Started on  Mon Jan 14 16:39:08 2008 Jean-Baptiste MOURET
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
#include "map.hpp"

namespace fastsim
{
  void Map::_read_file(const std::string& fname)
  {
    std::string str;
    std::ifstream ifs(fname.c_str());
    if (!ifs.good())
      throw Exception(std::string("cannot open map :") + fname);
    //ifs >> str >> _w >> _h;
    ifs >> str;
    if (str != "P4")
      throw Exception("wrong file type for map");

		// These next few lines are useful when your map file is not correctly
		// formatted (as can Gimp spit out). This ignores comments lines (#) and empty
		// lines 
    std::string comments;
    ifs >> comments;
    while(comments=="#"||comments==""||comments==" ")
    {
    	std::getline(ifs, comments);
    	ifs >> comments;
    }
    
    _w = atof(comments.c_str());    
    ifs >> _h;

    _data.resize(_w * _h);
    if (_w % 8 != 0)
      throw Exception("wrong map size");
    int k = _w * _h / 8;
    char buffer[k];

    ifs.read((char*)buffer, k);
    for (int i = 0; i < k - 1; ++i)
      for (int j = 0; j < 8; ++j)
				_data[i * 8 + j] = _get_bit(buffer[i + 1], j) ? obstacle : free;
  }
  
  // we use the "triangle method"
  // concept : 
  // * area = base * height / 2
  // * area = cross product
  // -> if height < radius, intersection
  bool Map::_check_inter_ray_circle(float x1, float y1,
				    float x2, float y2,
				    float xr, float yr, float radius) const
  {
    // check if the object is on the right side of the camera
    float dot = (x2 - x1) * (xr - x1) + (y2 - y1) * (yr - y1);
    if (dot < 0)
      return false;
    float area2 = fabs((x2 - x1) * (yr - y1) -  (y2 - y1) * (xr - x1));
    float dx = x2 - x1;
    float dy = y2 - y1;
    float lab = sqrt(dx * dx + dy * dy);
    float h = area2 / lab;
    if (h < radius)
      return true;
    return false;
  }
  
  int Map::check_inter_is(float x1, float y1,
			  float x2, float y2) 
  {
    // list Illuminated Switches intersections with rays
    ill_sw_t res_sw;
    bool first_found = false;
    ClosestSwitch_f closest(x1, y1);
    for (size_t i = 0; i < _illuminated_switches.size(); ++i)
    {
			ill_sw_t isv = _illuminated_switches[i];
			
			if(isv->get_on())
			{
				if(!first_found || closest(isv, res_sw))
				{
					if(_check_inter_ray_circle(x1, y1, x2, y2,
																		  isv->get_x(), isv->get_y(),
																		  isv->get_radius()))
					{
						// To avoid collision problems, the ray is traced
						// from (x1, y1) to the circle of the illuminated switch
						// It should also make the camera more "precise"
			
						// The angle
						float angle = normalize_angle(atan2(isv->get_y() - y1, isv->get_x() - x1));
			
						// Intersection point between the radius circle and the ray
						float xinter, yinter;
						xinter = isv->get_x() - (cos(angle)*(isv->get_radius() + 3));
						yinter = isv->get_y() - (sin(angle)*(isv->get_radius() + 3));
			
						float xtmp, ytmp;
			
						if (!check_inter_real(x1, y1, xinter, yinter, xtmp, ytmp))
						{
							res_sw = isv;
							first_found = true;
						}
					}
				}
	  	}
    }
    
		if(!first_found)
      return -1;

    return res_sw->get_color();
  }
  
  Map::Return_check_t Map::check_inter_is_dist(float x1, float y1,
			  float x2, float y2) 
  {
    // list Illuminated Switches intersections with rays
    Return_check_t ret;
    ret.pixel = -1;
    ret.dist = -1;
    
    ill_sw_t res_sw;
    bool first_found = false;
    ClosestSwitch_f closest(x1, y1);
    for (size_t i = 0; i < _illuminated_switches.size(); ++i)
    {
			ill_sw_t isv = _illuminated_switches[i];
			
			if(isv->get_on())
			{
				if(!first_found || closest(isv, res_sw))
				{
					if(_check_inter_ray_circle(x1, y1, x2, y2,
																		  isv->get_x(), isv->get_y(),
																		  isv->get_radius()))
					{
						// To avoid collision problems, the ray is traced
						// from (x1, y1) to the circle of the illuminated switch
						// It should also make the camera more "precise"
			
						// The angle
						float angle = normalize_angle(atan2(isv->get_y() - y1, isv->get_x() - x1));
			
						// Intersection point between the radius circle and the ray
						float xinter, yinter;
						xinter = isv->get_x() - (cos(angle)*(isv->get_radius() + 3));
						yinter = isv->get_y() - (sin(angle)*(isv->get_radius() + 3));
			
						float xtmp, ytmp;
			
						if (!check_inter_real(x1, y1, xinter, yinter, xtmp, ytmp))
						{
							res_sw = isv;
							first_found = true;
						}
					}
				}
	  	}
    }
    
		if(!first_found)
			ret.pixel = -1;
		else
		{
			ret.pixel = res_sw->get_color();
			
			float x_sw = res_sw->get_x();
			float y_sw = res_sw->get_y();
			float x_dist = x_sw - x1;
			float y_dist = y_sw - y1;
			float dist = sqrtf(x_dist*x_dist + y_dist*y_dist);
			ret.dist = dist;
		}
    
    return ret;
  }  
  
  bool Map :: _try_pixel(int x, int y) const
  {
    if (x >= 0 && y >= 0 
	&& x < get_pixel_w() 
	&& y < get_pixel_h() 
	&& get_pixel(x, y) == free)
      return false;
    else
      return true;
  }


  
  // see
  // http://lifc.univ-fcomte.fr/~dedu/projects/bresenham/index.html
  // In PIXEL coordinates
  bool Map :: check_inter_pixel(int y1, int x1,
				int y2, int x2,
				int& y_res, int& x_res) const
  {
    int i;               // loop counter
    int ystep, xstep;    // the step on y and x axis
    int error;           // the error accumulated during the increment
    int errorprev;       // *vision the previous value of the error variable
    int y = y1, x = x1;  // the line points
    int ddy, ddx;        // compulsory variables: the double values of dy and dx
    int dx = x2 - x1;
    int dy = y2 - y1;
    bool inter = _try_pixel(y1, x1);
    if (dy < 0) { ystep = -1; dy = -dy; } else ystep = 1;
    if (dx < 0) { xstep = -1; dx = -dx; } else xstep = 1;
    ddy = dy * 2;
    ddx = dx * 2;
    if (ddx >= ddy) // first octant (0 <= slope <= 1)
    {  
			errorprev = error = dx;
			for (i = 0 ; i < dx ; i++)
	  	{  // do not use the first point (already done)
	  	  x += xstep;
	  	  error += ddy;
	   		if (error > ddx)
	      {
					y += ystep;
					error -= ddx;
					if (error + errorprev < ddx)  // bottom square also
					  inter = inter || _try_pixel(y - ystep, x);
					else if (error + errorprev > ddx)  // left square also
						inter = inter || _try_pixel(y, x - xstep);
					else // corner: bottom and left squares also
					{
						inter = inter || _try_pixel(y - ystep, x);
						inter = inter || _try_pixel(y, x - xstep);
					}
	      }
			  inter = inter || _try_pixel(y, x);
			  errorprev = error;
			  if (inter)
	      {
					x_res = x;
					y_res = y;
					return true;
	      }
		  }
    }
    else
    {  // the same as above
			errorprev = error = dy;
			for (i = 0 ; i < dy ; i++)
		  {
			  y += ystep;
			  error += ddx;
			  if (error > ddy)
			  {
	      	x += xstep;
	      	error -= ddy;
	      	if (error + errorprev < ddy)
						inter = inter || _try_pixel(y, x - xstep);
	      	else if (error + errorprev > ddy)
						inter = inter || _try_pixel(y - ystep, x);
		      else
					{
					  inter = inter || _try_pixel(y, x - xstep);
					  inter = inter || _try_pixel(y -  ystep, x);
					}
		    }
		    inter = inter || _try_pixel(y, x);
		    errorprev = error;
		    if (inter)
	      {
					x_res = x;
					y_res = y;
					return true;
	      }
		  }
    }
    return false;
  }

    // Draws a rectangle with (x,y) the upper left point and (lx,ly) the size
  void Map:: draw_rect(int x, int y, int lx, int ly) {
    int i,j;

    for (i=0;i<lx;i++)
      for (j=0;j<ly;j++) {
	if ((x+i) >= 0 && (y+j) >= 0 
	    && (x+i) < get_pixel_w() 
	    && (y+j) < get_pixel_h())
	  set_pixel(x+i,y+j,obstacle);
      }
  }
  
  void Map :: clean_robot_obstacle(float x, float y, float radius, float bbx, float bby, float bbw, float bbh)
  {
    int _rp = real_to_pixel(radius);
    int _r = _rp * _rp;
    int _x = real_to_pixel(x);
    int _y = real_to_pixel(y);
    int _bbx = real_to_pixel(bbx);
    int _bby = real_to_pixel(bby);
    int _bbw = real_to_pixel(bbx + bbw);
    int _bbh = real_to_pixel(bby + bbh);
    
    typedef std::pair<int, int> p_t;
    std::list<p_t > coll_points;
    for (int i = _bbx; i < _bbw; ++i)
      for (int j = _bby; j < _bbh; ++j)
				if (get_pixel(i, j) == Map::obstacle)
			  {
					float d1 = (i - _x);
					float d2 = (j - _y);
					if (d1 * d1 + d2 * d2 <= _r)
						set_pixel(i, j, Map::free);
			  }
	 }
	 
	void Map :: add_robot_obstacle(float x, float y, float radius, float bbx, float bby, float bbw, float bbh)
	{
    int _rp = real_to_pixel(radius);
    int _r = _rp * _rp;
    int _x = real_to_pixel(x);
    int _y = real_to_pixel(y);
    int _bbx = real_to_pixel(bbx);
    int _bby = real_to_pixel(bby);
    int _bbw = real_to_pixel(bbx + bbw);
    int _bbh = real_to_pixel(bby + bbh);
    
    typedef std::pair<int, int> p_t;
    std::list<p_t > coll_points;
    for (int i = _bbx; i < _bbw; ++i)
      for (int j = _bby; j < _bbh; ++j)
				if (get_pixel(i, j) == Map::free)
			  {
					float d1 = (i - _x);
					float d2 = (j - _y);
					if (d1 * d1 + d2 * d2 <= _r)
						set_pixel(i, j, Map::obstacle);
			  }
	}
			  
  bool Map :: get_random_initial_position(float radius, fastsim::Posture &pos, int max_tries)
  {
		// We don't want robots to appear outside of the map
		float x_min = radius + 1;
		float x_max = _real_w - radius - 1;
		float y_min = x_min;
		float y_max = _real_h - radius - 1;

		bool found = false;
		float rand_x = 0;
		float rand_y = 0;
		for(int k = 0; k < max_tries && !found; ++k)
		{
			rand_x = sferes::misc::rand(x_min, x_max);
			rand_y = sferes::misc::rand(y_min, y_max);
	
			// We don't want this robot to collide with anything
			// pixel wise
			int rp = real_to_pixel(radius);
			int r = rp * rp;
			int x = real_to_pixel(rand_x);
			int y = real_to_pixel(rand_y);
			
			// Bounding Box
			float _bbx = rand_x - radius - 4;
			float _bby = rand_y - radius - 4;
			float _bbw = radius * 2 + 8;
			float _bbh = radius * 2 + 8;
			int bbx = real_to_pixel(_bbx);
			int bby = real_to_pixel(_bby);
			int bbw = real_to_pixel(_bbx + _bbw);
			int bbh = real_to_pixel(_bby + _bbh);
	
			typedef std::pair<int, int> p_t;
			found = true;

			int compteur = 0;
			for (int i = bbx; i < bbw; ++i)
			{
				for (int j = bby; j < bbh; ++j)
				{
					if (get_pixel(i, j) == obstacle)
					{
						float d1 = (i - x);
						float d2 = (j - y);
						if (d1 * d1 + d2 * d2 <= r)
						{
							found = false;
							break;
						}
					}
					compteur++;
				}
				
				if(!found)
					break;
			}
		}

		if(found)
		{
			float rand_theta = sferes::misc::rand(-M_PI, M_PI);
	
			pos.set_x(rand_x);
			pos.set_y(rand_y);
			pos.set_theta(rand_theta);
		}

		return found;
  }
        
  bool Map :: get_random_initial_position_circle(float radius, fastsim::Posture &pos, float circle_radius, int max_tries)
  {
		// We don't want robots to appear outside of the map
		float x_min = (_real_w/2) - circle_radius;
		float x_max = (_real_w/2) + circle_radius;

		bool found = false;
		float rand_x = 0;
		float rand_y = 0;
		for(int k = 0; k < max_tries && !found; ++k)
		{
			rand_x = sferes::misc::rand(x_min, x_max);

			// y_min and y_max depend on x
			float yabs = sqrtf(pow(circle_radius, 2) - pow((_real_w/2) - rand_x, 2));
			float y_min = (_real_h/2) - yabs;
			float y_max = (_real_h/2) + yabs;
			
			rand_y = sferes::misc::rand(y_min, y_max);
	
			// We don't want this robot to collide with anything
			// pixel wise
			int rp = real_to_pixel(radius);
			int r = rp * rp;
			int x = real_to_pixel(rand_x);
			int y = real_to_pixel(rand_y);
			
			// Bounding Box
			float _bbx = rand_x - radius - 4;
			float _bby = rand_y - radius - 4;
			float _bbw = radius * 2 + 8;
			float _bbh = radius * 2 + 8;
			int bbx = real_to_pixel(_bbx);
			int bby = real_to_pixel(_bby);
			int bbw = real_to_pixel(_bbx + _bbw);
			int bbh = real_to_pixel(_bby + _bbh);
	
			typedef std::pair<int, int> p_t;
			found = true;

			int compteur = 0;
			for (int i = bbx; i < bbw; ++i)
			{
				for (int j = bby; j < bbh; ++j)
				{
					if (get_pixel(i, j) == obstacle)
					{
						float d1 = (i - x);
						float d2 = (j - y);
						if (d1 * d1 + d2 * d2 <= r)
						{
							found = false;
							break;
						}
					}
					compteur++;
				}
				
				if(!found)
					break;
			}
		}

		if(found)
		{
			float rand_theta = sferes::misc::rand(-M_PI, M_PI);
	
			pos.set_x(rand_x);
			pos.set_y(rand_y);
			pos.set_theta(rand_theta);
		}

		return found;
  }
}
