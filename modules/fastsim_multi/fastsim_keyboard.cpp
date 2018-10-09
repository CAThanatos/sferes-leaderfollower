#include <boost/foreach.hpp>
#include "simu_fastsim_multi.hpp"

using namespace sferes;
struct Params
{
  struct simu
  {
    SFERES_STRING(map_name, "modules/fastsim/cuisine.pbm");
  };
};



int main(int argc, char *argv[])
{
  using namespace fastsim;
  typedef simu::FastsimMulti<Params> simu_t;
  simu_t s(true);

	Robot* r1 = new Robot(20.0f, Posture(250, 250, M_PI/3), 2);  
  r1->use_camera();
	r1->add_laser(Laser(0, 50));
 //  r1->add_laser(Laser(M_PI / 3, 50));
	// r1->add_laser(Laser(-M_PI / 3, 50));
 //  r1->add_laser(Laser(2 * M_PI / 3, 50));
	// r1->add_laser(Laser(M_PI, 50));
	r1->add_laser(Laser(-2 * M_PI / 3, 50));
	s.add_robot(r1);

/*	Robot* r2 = new Robot(20.0f, Posture(100, 100, -M_PI/2), 4, 2);  
  r2->use_camera();
  r2->add_laser(Laser(M_PI / 3, 50));
	r2->add_laser(Laser(0, 50));
	r2->add_laser(Laser(-M_PI / 3, 50));
	s.add_robot(r2);*/

  Map::ill_sw_t s1 = Map::ill_sw_t(new IlluminatedSwitch(2, 10, 300, 251, true));
/*  Map::ill_sw_t s2 = Map::ill_sw_t(new IlluminatedSwitch(2, 10, 530, 150, true));
  Map::ill_sw_t s3 = Map::ill_sw_t(new IlluminatedSwitch(3, 10, 200, 150, true));
  Map::ill_sw_t s4 = Map::ill_sw_t(new IlluminatedSwitch(4, 10, 400, 350, true));*/

//  s.map()->add_illuminated_switch(s1);
/*  s.map()->add_illuminated_switch(s2);
  s.map()->add_illuminated_switch(s3);
  s.map()->add_illuminated_switch(s4);*/

  s.init_view();

  int numkey;
  int num_robot = 0;

  Posture pos_center(s.map()->get_real_w()/2, s.map()->get_real_h()/2, 0);
  while(true)
  {
    float distance_toCenter1 = r1->get_pos().dist_to(pos_center.x(), pos_center.y());
    float angle_toCenter1 = normalize_angle(atan2(pos_center.y() - r1->get_pos().y(), pos_center.x() - r1->get_pos().x()));
    angle_toCenter1 = normalize_angle(angle_toCenter1 - r1->get_pos().theta());

    distance_toCenter1 = (3400.0f - distance_toCenter1)/(3400.0f);
    angle_toCenter1 = (angle_toCenter1 + M_PI)/(2.0*M_PI);
    std::cout << distance_toCenter1 << "/" << angle_toCenter1 << std::endl;


    SDL_PumpEvents();
    Uint8* keys = SDL_GetKeyState(&numkey);
    if (keys[SDLK_TAB]) 
    {
    	num_robot = (num_robot+1)%(s.robots().size());
    	s.change_selected_robot(num_robot);
    }
    if (keys[SDLK_UP])
			s.move_robot(1.0, 1.0, num_robot);
    if (keys[SDLK_DOWN])
			s.move_robot(-1.0, -1.0, num_robot);
    if (keys[SDLK_LEFT])
     	s.move_robot(1.0, -1.0, num_robot);
    if (keys[SDLK_RIGHT])
     	s.move_robot(-1.0, 1.0, num_robot);
    s.refresh();
    s.refresh_view();
    
		for (size_t j = 0; j < 6; ++j)
		{
			float d = r1->get_lasers()[j].get_dist();
			float range = r1->get_lasers()[j].get_range();
		
			// Getting the input between 0 and 1
			d = d - 20.0f;
			float val = 1 - d/(range - 20.0f);
			std::cout << std::min(1.f, std::max(0.f, val)) << "/";
		}
		std::cout << std::endl;
  }

  return 0;
}
