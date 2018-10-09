#ifndef PREY_HPP_
#define PREY_HPP_

#include <boost/assign/list_of.hpp>
#include <modules/fastsim_multi/posture.hpp>
#include <modules/fastsim_multi/simu_fastsim_multi.hpp>

#include "defparams.hpp"
#include "StagHuntRobot.hpp"

namespace sferes
{
	class Prey : public StagHuntRobot
	{
		public :
			Prey() : StagHuntRobot(), _blocked(false), _no_reward(false), _leader_first(-1) {}
			Prey(float radius, const fastsim::Posture& pos, int color, unsigned int color_alt = 0) : StagHuntRobot(radius, pos, color, color_alt), _blocked(false), _nb_blocked(0), _stamina_lost_coop(1), _pred_on_it(false), _no_reward(false), _leader_first(-1)
			{}

			virtual ~Prey()	{	}

			std::vector<float> step_action()
			{
				std::vector<float> ret(2);
				ret[0] = -1.0f;
				ret[1] = -1.0f;

				if(_blocked)
					lose_stamina();
					
				return ret;
			}
			
			bool is_blocked() { return _blocked; }
			bool is_dead() { return (_stamina <= 0); }
			
			virtual void blocked_by_hunters(int nb_hunters) = 0;
			virtual float feast() = 0;
			virtual void lose_stamina() = 0;
		
      void set_pred_on_it(bool pred_on_it)
      {
				_pred_on_it = false;
      }

			bool is_pred_on_it() { return _pred_on_it; }

			enum type_prey
			{
				HARE,
				SMALL_STAG,
				BIG_STAG
			};
			
			type_prey get_type() { return _type_prey; }

			int get_nb_blocked() { return _nb_blocked; }
			void set_no_reward(bool no_reward) { _no_reward = no_reward; }
			bool get_no_reward() { return _no_reward; }

			void set_leader_first(int leader_first) { _leader_first = leader_first; }
			int get_leader_first() { return _leader_first; }
		
		protected :
			bool _blocked;
			int _nb_blocked;
			float _stamina;
			float _stamina_lost_coop;
			type_prey _type_prey;
			
			bool _pred_on_it;
			bool _no_reward;
			int _leader_first;
	};
}

#endif
