
#ifndef APPLE
#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE fastsim



#include <boost/test/unit_test.hpp>
#endif

#include <boost/foreach.hpp>
#include <sferes/fit/fitness_simu.hpp>
#include "simu_fastsim_multi.hpp"

using namespace sferes;
//using namespace sferes::ctrl;
struct Params
{
  struct simu
  {
//    SFERES_STRING(map_name, "modules/fastsim/cuisine.pbm");
		SFERES_STRING(map_name, "modules/fastsim_multi/map200x200.pbm");
  };
};


BOOST_AUTO_TEST_CASE(fastsim_intersection)
{
  using namespace fastsim;
  typedef simu::FastsimMulti<Params> simu_t;
  simu_t s;
  
  typedef fastsim::Robot robot_t;
  robot_t* r1, *r2;
  r1 = new robot_t(20.0f);
  r2 = new robot_t(20.0f);
  
  r1->set_pos(Posture(200, 100, 0));
  r1->use_camera();
  s.add_robot(r1);
  
/*  r2.set_pos(Posture(100, 200, 0));
  r2.use_camera();
  s.add_robot(r2);*/

  Map::ill_sw_t s1 = Map::ill_sw_t(new IlluminatedSwitch(1, 10, 300, 250, true)); 
  Map::ill_sw_t s2 = Map::ill_sw_t(new IlluminatedSwitch(2, 30, 500, 250, true));
  Map::ill_sw_t s3 = Map::ill_sw_t(new IlluminatedSwitch(3, 10, 200, 150, true));
  Map::ill_sw_t s4 = Map::ill_sw_t(new IlluminatedSwitch(4, 10, 400, 350, true));
  s.map()->add_illuminated_switch(s1);
  s.map()->add_illuminated_switch(s2);
  s.map()->add_illuminated_switch(s3);
  s.map()->add_illuminated_switch(s4);
  
  int c1 = s.map()->check_inter_is(250, 250, 1000, 250);
  BOOST_CHECK_EQUAL(c1, 1);
  
  int c2 = s.map()->check_inter_is(250, 250, 250, 0);
  BOOST_CHECK_EQUAL(c2, -1);
  
  int c3 = s.map()->check_inter_is(200, 350, 200, 100);
  BOOST_CHECK_EQUAL(c3, 3);
  
  // s.init_view();
  // while(true)
  //   {
  //     s.refresh();
  //     s.refresh_view();
  //   }
}

BOOST_AUTO_TEST_CASE(fastsim_traverse)
{
  using namespace fastsim;
  typedef simu::FastsimMulti<Params> simu_t;
  simu_t s;
  
  typedef fastsim::Robot robot_t;
  robot_t* r1;
  r1 = new robot_t(20.0f);
  
  r1->set_pos(Posture(50, 50, M_PI/4.0f));
  r1->use_camera();
  s.add_robot(r1);

  s.init_view(true);

	for (size_t i = 0; i < 1000; ++i)
	{
		s.move_robot(2.0, 2.0, 0);
		
		Posture pos_robot = r1->get_pos();
		if(pos_robot.x() > 550 && pos_robot.y() > 550)
		{
			std::cout << "Yep : " << i << std::endl;
			break;
		}
	  s.refresh_view();
	}
}

#ifdef APPLE 
int main(int argc, char *argv[])
{
	std::cout<<"WARNING: BOOST TEST framework desactivated on macos to avoid problems with SDL."<<std::endl;
#else
BOOST_AUTO_TEST_CASE(fastsim_simu_refresh)
{
#endif

	using namespace fastsim;
	typedef simu::FastsimMulti<Params> simu_t;
	simu_t s;
	
	typedef fastsim::Robot robot_t;
//	robot_t* r1, *r2;

/*	Map::ill_sw_t s1 = Map::ill_sw_t(new IlluminatedSwitch(1, 10, 300, 251, true));
	Map::ill_sw_t s2 = Map::ill_sw_t(new IlluminatedSwitch(2, 10, 530, 150, false));
	Map::ill_sw_t s3 = Map::ill_sw_t(new IlluminatedSwitch(3, 10, 200, 150, false));
	Map::ill_sw_t s4 = Map::ill_sw_t(new IlluminatedSwitch(4, 10, 400, 350, false));
	s1->link(s2);
	s1->link(s3);
	s2->link(s4);
	s.map()->add_illuminated_switch(s1);
	s.map()->add_illuminated_switch(s2);
	s.map()->add_illuminated_switch(s3);
	s.map()->add_illuminated_switch(s4);*/


	for (int k = 0; k < 3; ++k)
	{
		s.init();

		robot_t* r1 = new robot_t(20.0f);
		robot_t* r2 = new robot_t(20.0f);	
	
		r1->set_pos(Posture(250, 250, 0));
		r1->add_laser(Laser(M_PI / 3, 50));
		r1->add_laser(Laser(0, 50));
		r1->add_laser(Laser(-M_PI / 3, 50));
	
		r1->add_light_sensor(LightSensor(1, M_PI / 3, M_PI / 3));
		r1->add_light_sensor(LightSensor(1, -M_PI / 3, M_PI / 3));
		r1->add_light_sensor(LightSensor(2, M_PI / 3, M_PI / 3));
		r1->add_light_sensor(LightSensor(2, -M_PI / 3, M_PI / 3));
		s.add_robot(r1);
	
		r2->set_pos(Posture(100, 100, 0));
		r2->add_laser(Laser(M_PI / 3, 50));
		r2->add_laser(Laser(0, 50));
		r2->add_laser(Laser(-M_PI / 3, 50));
	
		r2->add_light_sensor(LightSensor(1, M_PI / 3, M_PI / 3));
		r2->add_light_sensor(LightSensor(1, -M_PI / 3, M_PI / 3));
		r2->add_light_sensor(LightSensor(2, M_PI / 3, M_PI / 3));
		r2->add_light_sensor(LightSensor(2, -M_PI / 3, M_PI / 3));
		s.add_robot(r2);

//		s.init_view();

		for (size_t j = 0; j < 500; ++j)
		{
			for(int i = 0; i < s.robots().size() ; ++i)
			{
				if (s.robots()[i]->get_left_bumper())
					s.move_robot(2.0, -2.0, i);
				else if (s.robots()[i]->get_right_bumper())
					s.move_robot(2.0, -2.0, i);
				else if (rand()/(RAND_MAX + 1.0) < 0.05)
					s.move_robot(3.0, -3.0, i);
				else if (rand()/(RAND_MAX + 1.0) < 0.05)
					s.move_robot(-3.0, 3.0, i);
				else
					s.move_robot(2.0, 2.0, i);
			}

			std::cout<<"j="<<j<<std::endl;
//			s.refresh();
//			s.refresh_view();
			std::cout << "prout " << std::endl;
		}
  }
	
#ifdef APPLE
	return 0;
#endif
}
