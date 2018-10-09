#ifndef SIMU_FASTSIM_MULTI_HPP_
#define SIMU_FASTSIM_MULTI_HPP_

#include <sferes/simu/simu.hpp>
#include <sferes/misc/rand.hpp>
#include "fastsim_multi.hpp"

namespace sferes
{
  namespace simu
  {
    SFERES_SIMU(FastsimMulti, Simu)
    {
      public:
        FastsimMulti(bool view_activated = false) : _view_activated(view_activated)
        {
          //_map = boost::shared_ptr<fastsim::Map>(new fastsim::Map(Params::simu::map_name(), 600.0f));
          _map = boost::shared_ptr<fastsim::Map>(new fastsim::Map(Params::simu::map_name(), 600.0f));
          _view_initiated = false;
        }
        FastsimMulti(const fastsim::Map & m, bool view_activated = false) : _view_activated(view_activated)
        {
          _map = boost::shared_ptr<fastsim::Map>(new fastsim::Map(m));
          _view_initiated = false;
        }
        ~FastsimMulti()
        {
        	for(int i = 0; i < _robots.size(); ++i)
        	{
        		delete _robots[i];
        	}
        	_robots.clear();
        }
        void init()
        {
	       	for(int i = 0; i < _robots.size(); ++i)
        	{
        		delete _robots[i];
        	}
        	_robots.clear();
          _view_initiated = false;
        }
        void reinit()
        {
        	for(int i = 0; i < _robots.size(); ++i)
        	{
        		delete _robots[i];
        	}
        	_robots.clear();
          _view_initiated = false;
        }
        void reinit_with_map()
        {
        	for(int i = 0; i < _robots.size(); ++i)
        	{
        		delete _robots[i];
        	}
        	_robots.clear();
            _view_initiated = false;

            _map = boost::shared_ptr<fastsim::Map>(new fastsim::Map(Params::simu::map_name(), Params::simu::real_w));
        }
        void reinit_with_map(const fastsim::Map & m)
        {
        	for(int i = 0; i < _robots.size(); ++i)
        	{
        		delete _robots[i];
        	}
        	_robots.clear();
            _view_initiated = false;

			_map = boost::shared_ptr<fastsim::Map>(new fastsim::Map(m));
        }
        void refresh() 
        {
        	// We don't want to always refresh the robots in the same order each time
        	std::vector<size_t> ord_vect;
        	sferes::misc::rand_ind(ord_vect, _robots.size());
        	
        	for(int i = 0; i < ord_vect.size(); ++i)
        	{
        		int num = (int)ord_vect[i];
        		assert(num < _robots.size());
         		_map->update(_robots[num]->get_pos()); 
          }
        }
        void init_view(bool dump = false)
        {
            _display =
            boost::shared_ptr<fastsim::Display>(new fastsim::Display(_map, _robots));
			_view_initiated = true;
        }
        void set_map(const boost::shared_ptr<fastsim::Map>& map) { _map = map; }
        void refresh_view()
        {
            _display->update();
        }
        void refresh_map_view()
        {
            _display->update_map();
        }
        void dump_display(const char * file)
        {
        	_display->dump_display(file);
        }
        void dump_behaviour_log(const char * file)
        {
        	_display->dump_behaviour_log(file);
        }
        void add_dead_prey(int index, std::string& file, bool alone = true, bool first_robot = true)
        {
        	_display->add_dead_prey(_robots[index]->get_pos().x(), _robots[index]->get_pos().y(), _robots[index]->get_radius(), file, alone, first_robot);
        }
        void add_dead_prey(int index, bool alone = true, bool first_robot = true)
        {
            _display->add_dead_prey(_robots[index]->get_pos().x(), _robots[index]->get_pos().y(), _robots[index]->get_radius(), alone, first_robot);
        }
        void add_dead_prey(float x, float y, float radius, bool alone = true, bool first_robot = true)
        {
            _display->add_dead_prey(x, y, radius, alone, first_robot);
        }
        void switch_map()
        {
          _map->terrain_switch(Params::simu::alt_map_name());
        }
        void reset_map()
        {
          _map->terrain_switch(Params::simu::map_name());
        }
        void move_robot(float v1, float v2, int index) 
        { 
        	if(index < _robots.size())
	        	_robots[index]->move(v1, v2, _map); 
        }
        void teleport_robot(fastsim::Posture& pos, int index)
        {
        	if(index < _robots.size())
        		_robots[index]->teleport(pos, _map);
        }
        
        // Used to get a random initial position in the map for a robot. Not a very smart function so it's encouraged
        // to use your own list of initial positions in a map with a lot of obstacles and/or robots as is
        // could take a pretty long time for this function to get a valid position
        bool get_random_initial_position(float radius, fastsim::Posture &pos, int max_tries = 1000, bool avoid_start_pos = false)
        {
            return _map->get_random_initial_position(radius, pos, max_tries, avoid_start_pos);
        }

        				
		void add_robot(fastsim::Robot *robot)
		{
			_robots.push_back(robot);
			
			// We add an Illuminated Switch that is the robot to the map
			assert(_robots.size() > 0);
			fastsim::Map::ill_sw_t ill_sw = _robots[_robots.size() - 1]->get_ill_sw();
            _map->add_illuminated_switch(ill_sw);
		}
				
		void remove_robot(int index)
		{
			assert(index < _robots.size());
			_map->remove_illuminated_switch(_robots[index]->get_ill_sw());
			
			fastsim::Robot* robot = _robots[index];
			_robots.erase(_robots.begin() + index);
			
			delete robot;
		}

        void add_illuminated_switch(fastsim::Map::ill_sw_t ill_sw)
        {
            _map->add_illuminated_switch(ill_sw);
        }
		
		void change_selected_robot(int selected)
		{
			if(selected < _robots.size())
				_display->set_selected(selected);
		}

        boost::shared_ptr<fastsim::Map> map() { return _map; }
        const boost::shared_ptr<fastsim::Map> map() const { return _map; }

        std::vector<fastsim::Robot*>& robots() { return _robots; }
        const std::vector<fastsim::Robot*>& robots() const { return _robots; }

        fastsim::Display& display() {return *_display; }
        
        void set_file_video(std::string file_video)
        {
        	if(_display)
        		_display->set_file_video(file_video);
        }
      protected:
        std::vector<fastsim::Robot*> _robots;
        boost::shared_ptr<fastsim::Map> _map;
        boost::shared_ptr<fastsim::Display> _display;
        bool _view_initiated;
        bool _view_activated;
    };
  }
}

#endif
