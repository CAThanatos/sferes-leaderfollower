#include <iostream>
#include "light_sensor.hpp"


namespace fastsim
{
  int LightSensor :: update(const Posture& pos,
			    const boost::shared_ptr<Map>& map)
  {
    const std::vector<Map::ill_sw_t>& isv = map->get_illuminated_switches();
    _activated = false;
    for (size_t i = 0; i < isv.size(); ++i)
      if (isv[i]->get_color() == get_color() && isv[i]->get_on())
			{
				float angle = 
								normalize_angle(atan2(isv[i]->get_y() - pos.y(), 
																isv[i]->get_x() - pos.x())
								- pos.theta());
								
				float x_res = 0, y_res = 0;// ignored
				
				bool test1 = angle > normalize_angle(_angle - _range / 2.0f);
				bool test2 = angle < normalize_angle(_angle + _range / 2.0f);
				bool test3 = !map->check_inter_real(isv[i]->get_x(), isv[i]->get_y(),
																			pos.x(), pos.y(),
																			x_res, y_res);
/*				if (angle > normalize_angle(_angle - _range / 2.0f)
					  && angle < normalize_angle(_angle + _range / 2.0f)
					  && !map->check_inter_real(isv[i]->get_x(), isv[i]->get_y(),
																			pos.x(), pos.y(),
																			x_res, y_res))*/
//				std::cout << test1 << "/" << test2 << "/" << test3 << " - ";
				if(test1 && test2 && !test3)
				{
					_activated = true;
					_num=i;
				}	  
			}
			
//		std::cout << std::endl;
    return _activated;
  }
}
