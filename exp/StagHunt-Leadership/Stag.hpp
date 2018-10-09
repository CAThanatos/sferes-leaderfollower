#ifndef STAG_HPP_
#define STAG_HPP_

#include <modules/fastsim_multi/posture.hpp>
#include <modules/fastsim_multi/simu_fastsim_multi.hpp>
#include <sferes/misc/rand.hpp>
#include <cmath>

#include "defparams.hpp"
#include "Prey.hpp"

namespace sferes
{
	class Stag : public Prey
	{
		public :
			enum type_stag 
			{
				small,
				big
			}; 
		
			Stag() : Prey() { _stamina = STAMINA; }
			Stag(float radius, const fastsim::Posture& pos, int color, type_stag type, unsigned int fur_color) : Prey(radius, pos, (fur_color + color), color/100), _type(type), _fur_color(fur_color)
			{
				if(type == Stag::small)
				{
					_stamina = STAMINA;
 					_type_prey = Prey::SMALL_STAG;
 					_fur_color = fur_color;
 					_huntable_alone = true; 					
					this->set_display_color(SMALL_STAG_COLOR/100);
 				}
				else
				{
					if(_fur_color < 50)
					{
				  	_stamina = STAMINA;
				  	_type_prey = Prey::SMALL_STAG;
				  	_fur_color = fur_color;
				  	_huntable_alone = true;		
						this->set_display_color(SMALL_STAG_COLOR/100);
						// this->set_display_color(SMALL_STAG_COLOR/100);
					}
					else if(_fur_color >= 50)
					{
						_stamina = STAMINA;
						_type_prey = Prey::BIG_STAG;
				  	_fur_color = fur_color;
						_huntable_alone = true;
						this->set_display_color(BIG_STAG_COLOR/100);
					}
				}
			}
			
			~Stag()	{}
			
			void blocked_by_hunters(int nb_hunters)
			{
#ifdef HARDSTAG
				if(nb_hunters >= 2)
#else
				if(nb_hunters > 0)
#endif
				{
					_blocked = true;
					
#if !defined(NO_COOP) && !defined(NO_BOOLEAN)
					set_pred_on_it(true);
#endif
				}
				else
				{
					_blocked = false;
					set_pred_on_it(false);
					
#ifdef RECUP_STAMINA
					if(_stamina < STAMINA)
						_stamina = STAMINA;
#endif
				}

				if(_blocked)
					_nb_blocked = nb_hunters;
				else
					_nb_blocked = 0;
			}
			
			float feast()
			{
#ifdef FORCE_LEADERSHIP
				if(_no_reward)
					return 0;
#endif

				if(_type_prey == Prey::SMALL_STAG)
				{
					if(_nb_blocked == 1)
						return FOOD_SMALL_STAG_SOLO;
					else if(_nb_blocked > 1)
						return FOOD_SMALL_STAG_COOP;
					else
						return 0;
				}
				else if(_type_prey == Prey::BIG_STAG)
				{
#if defined(FITFOLLOW) || defined(FITSECOND)
					if(_nb_blocked >= 1)
						return FOOD_BIG_STAG_COOP;
#else
					if(_nb_blocked == 1)
						return FOOD_BIG_STAG_SOLO;
					else if(_nb_blocked > 1)
						return FOOD_BIG_STAG_COOP;
#endif
					else
						return 0;
				}
				else
				{
					return 0;
				}
			}
			
			void lose_stamina()
			{
				if(_nb_blocked > 0)
				{
					float stamina_lost = 1;
					_stamina = _stamina - stamina_lost;
				}
			}
			
			type_stag get_type() const { return _type; }
			float fur_color() const { return _fur_color; }
			bool huntable_alone() const { return _huntable_alone; }

		private :
			type_stag _type;
			float _fur_color;
			bool _huntable_alone;
	};
}

#endif
