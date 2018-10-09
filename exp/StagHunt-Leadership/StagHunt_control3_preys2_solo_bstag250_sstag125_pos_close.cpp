// THIS IS A GENERATED FILE - DO NOT EDIT
#define CONTROL3
#define PREYS2
#define SOLO
#define BSTAG250
#define SSTAG125
#define POS_CLOSE
#line 1 "/home/arthur/sferes2-0.99/exp/StagHunt-Leadership/StagHunt.cpp"
#include "includes.hpp"
#include "defparams.hpp"

namespace sferes
{
  using namespace nn;
  
  // ********** Main Class ***********
  SFERES_FITNESS(FitStagHunt, sferes::fit::Fitness)
  {
  public:
		typedef Neuron<PfWSum<>, AfSigmoidNoBias<> > neuron_t;
		typedef Connection<float> connection_t;
#ifdef ELMAN
		typedef Elman< neuron_t, connection_t > nn_t;
#else
		typedef Mlp< neuron_t, connection_t > nn_t;
#endif

    template<typename Indiv>
      void eval(Indiv& ind) 
		{
			std::cout << "Nop !" << std::endl;
		}

#ifdef COEVO
    template<typename Indiv>
      void eval_compet(Indiv& ind1, Indiv& ind2, int num_leader = 1) 
#elif defined(LISTMATCH)
#ifdef GENOME_TRACES
    template<typename Indiv>
      void eval_compet(Indiv& ind1, Indiv& ind2, bool genome_traces = false, bool evaluateInd2 = false) 
#else
    template<typename Indiv>
      void eval_compet(Indiv& ind1, Indiv& ind2, bool evaluateInd2 = false) 
#endif
#elif defined(GENOME_TRACES)
    template<typename Indiv>
      void eval_compet(Indiv& ind1, Indiv& ind2, bool genome_traces = false) 
#else
    template<typename Indiv>
      void eval_compet(Indiv& ind1, Indiv& ind2) 
#endif
    {
      nn_t nn1(Params::nn::nb_inputs, Params::nn::nb_hidden, Params::nn::nb_outputs);
      nn_t nn2(Params::nn::nb_inputs, Params::nn::nb_hidden, Params::nn::nb_outputs);

#ifndef MLP_PERSO
      nn1.set_all_weights(ind1.data());
      nn2.set_all_weights(ind2.data());

      nn1.init();
      nn2.init();
#endif

      typedef simu::FastsimMulti<Params> simu_t;
      typedef fastsim::Map map_t;

      using namespace fastsim;	
      
      //this->set_mode(fit::mode::eval);

      // *** Main Loop *** 
      float moy_hares1 = 0, moy_sstags1 = 0, moy_bstags1 = 0;
      float moy_hares1_solo = 0, moy_sstags1_solo = 0, moy_bstags1_solo = 0;
     	int food1 = 0;

      float moy_hares2 = 0, moy_sstags2 = 0, moy_bstags2 = 0;
      float moy_hares2_solo = 0, moy_sstags2_solo = 0, moy_bstags2_solo = 0;
     	int food2 = 0;

#ifdef COEVO
     	_num_leader = num_leader;
#endif

#ifdef DIFF_FIT
     	float fit2 = 0;
#endif

#ifdef MOVING_LEADER
     	float dist_traveled = 0.0f;
#endif

#ifdef FITFOLLOW
			float fit_follow = 0;
#endif

			_nb_leader_first = 0;
			_nb_preys_killed = 0;
			_nb_preys_killed_coop = 0;

			float proportion_leader = 0;

			float nb_ind1_leader_first = 0;

#ifdef DOUBLE_NN
			float nb_nn1_chosen = 0;
			float nb_bigger_nn1_chosen = 0;
			float nb_role_divisions = 0;
			float fit_nn1 = 0;
			float fit_nn2 = 0;
#endif

#ifdef DIVERSITY
			std::vector<float> vec_sm;
#endif

      for (size_t j = 0; j < Params::simu::nb_trials; ++j)
      {
				// clock_t deb = clock();

      	// init
				map_t map(Params::simu::map_name(), Params::simu::real_w);
				simu_t simu(map, (this->mode() == fit::mode::view));
				init_simu(simu, nn1, nn2);


#ifdef BEHAVIOUR_LOG
				_file_behaviour = "behaviourTrace";
				cpt_files = 0;
#endif
	
			// std::cout << "yop" << std::endl;
#ifdef MLP_PERSO
				StagHuntRobot* robot1 = (StagHuntRobot*)(simu.robots()[0]);
				StagHuntRobot* robot2 = (StagHuntRobot*)(simu.robots()[1]);
				Hunter* hunter1 = (Hunter*)robot1;
				Hunter* hunter2 = (Hunter*)robot2;

#ifdef DOUBLE_NN
				float rand1 = misc::rand(1.0f);
				float rand2 = misc::rand(1.0f);
				float diff_hunters = rand1 - rand2;
				diff_hunters = (diff_hunters/2.0f) + 0.5f;

				if(rand1 > rand2)
				{
					_num_leader = 1;
					hunter1->set_bool_leader(true);
					hunter2->set_bool_leader(false);
				}
				else
				{
					_num_leader = 2;
					hunter1->set_bool_leader(false);
					hunter2->set_bool_leader(true);
				}

#ifdef NN_LEADER
				if(hunter1->is_leader())
				{
					hunter1->choose_nn(0, ind1.data());
					hunter2->choose_nn(1, ind2.data());
				}
				else
				{
					hunter1->choose_nn(1, ind1.data());
					hunter2->choose_nn(0, ind2.data());
				}
#elif defined(NO_CHOICE_DUP)
				int nb_weights = (Params::nn::nb_inputs + 1) * Params::nn::nb_hidden + Params::nn::nb_outputs * Params::nn::nb_hidden + Params::nn::nb_outputs;

				if(ind1.data().size() == nb_weights)
				{
					hunter1->choose_nn(0, ind1.data());

					if((ind2.data().size() == nb_weights) || (misc::rand<float>() < 0.5f))
						hunter2->choose_nn(0, ind2.data());
					else
						hunter2->choose_nn(1, ind2.data());
				}	
				else
				{
					if(ind2.data().size() == nb_weights)
					{
						if(misc::rand<float>() < 0.5f)
							hunter1->choose_nn(0, ind1.data());
						else
							hunter1->choose_nn(1, ind1.data());

						hunter2->choose_nn(0, ind2.data());
					}
					else
					{
						if(rand1 > rand2)
						{
							hunter1->choose_nn(0, ind1.data());
							hunter2->choose_nn(1, ind2.data());
						}
						else
						{
							hunter1->choose_nn(1, ind1.data());
							hunter2->choose_nn(0, ind2.data());
						}
					}
				}
#elif defined(BINARY_DIFF)
				if(rand1 > rand2)
				{
					hunter1->choose_nn(ind1.data(), 1);
					hunter2->choose_nn(ind2.data(), 0);
				}
				else
				{
					hunter1->choose_nn(ind1.data(), 0);
					hunter2->choose_nn(ind2.data(), 1);
				}
#elif defined(CHOICE_ORDERED)
				int choice_leader;
				if(hunter1->is_leader())
				{
#ifdef FORCE_LEADER
					if(misc::rand<float>() < 0.5f)
					{
						choice_leader = 0;
						hunter1->choose_nn(0, ind1.data());
					}
					else
					{
						choice_leader = 1;
						hunter1->choose_nn(1, ind1.data());
					}
#else
					choice_leader = hunter1->choose_nn(ind1.data(), -1);
#endif
					hunter2->choose_nn(ind2.data(), choice_leader);
				}
				else
				{
#ifdef FORCE_LEADER
					if(misc::rand<float>() < 0.5f)
					{
						choice_leader = 0;
						hunter2->choose_nn(0, ind2.data());
					}
					else
					{
						choice_leader = 1;
						hunter2->choose_nn(1, ind2.data());
					}
#else
					choice_leader = hunter2->choose_nn(ind2.data(), -1);
#endif
					hunter1->choose_nn(ind1.data(), choice_leader);
				}
#elif defined(COM_NN)
#ifdef DECISION_MAPPING
				hunter1->choose_nn(ind1.data(), 0);
				hunter2->choose_nn(ind2.data(), 0);
#else
				hunter1->choose_nn(0, ind1.data());
				hunter2->choose_nn(0, ind2.data());
#endif
#else
				hunter1->choose_nn(ind1.data(), diff_hunters);
				hunter2->choose_nn(ind2.data(), 1 - diff_hunters);
#endif

				// Is the nn chosen because of the two random numbers ?
				if(hunter1->nn1_chosen())
				{
					if(rand1 > rand2)
						nb_bigger_nn1_chosen++;

					nb_nn1_chosen++;
				}

				// Did the two individuals select a different nn ?
				if(hunter1->nn1_chosen() != hunter2->nn1_chosen())
					nb_role_divisions++;
#else
				hunter1->set_weights(ind1.data());
				hunter2->set_weights(ind2.data());
#endif
#endif
				// std::cout << "lait" << std::endl;

				float food_trial = 0;

#ifdef MOVING_LEADER
				Posture prev_pos;
				StagHuntRobot* rob = (StagHuntRobot*)(simu.robots()[_num_leader - 1]);
				prev_pos.set_x(rob->get_pos().x());
				prev_pos.set_y(rob->get_pos().y());
#endif

				_nb_leader_first_trial = 0;
				_nb_preys_killed_coop_trial = 0;

#ifdef DIVERSITY
				std::vector<float> vec_sm_trial;
#endif

				for (size_t i = 0; i < Params::simu::nb_steps && !stop_eval; ++i)
				{
#if defined(FITFOLLOW) || defined(STAGFOLLOW)
					// We compute the distance between the two individuals
					StagHuntRobot *rob1, *rob2;

					if(_num_leader == 1)
					{
						rob1 = (StagHuntRobot*)(simu.robots()[0]);
						rob2 = (StagHuntRobot*)(simu.robots()[1]);
					}
					else
					{
						rob1 = (StagHuntRobot*)(simu.robots()[1]);
						rob2 = (StagHuntRobot*)(simu.robots()[0]);
					}

					Hunter* follower = (Hunter *)rob2;
					float distance_leader = rob1->get_pos().dist_to(rob2->get_pos().x(), rob2->get_pos().y());
					distance_leader = distance_leader - rob1->get_radius() - rob2->get_radius();
					follower->set_distance_hunter(distance_leader);

#ifdef COMPAS_FOLLOWER
					float angle_toLeader = normalize_angle(atan2(rob1->get_pos().y() - rob2->get_pos().y(), rob1->get_pos().x() - rob2->get_pos().x()));
					angle_toLeader = normalize_angle(angle_toLeader - rob2->get_pos().theta());
					follower->set_angle_hunter(angle_toLeader);

					// We set the values to 0 for the leader by simplicity
					Hunter* leader = (Hunter *)rob1;
					leader->set_distance_hunter(0.0f);
					leader->set_angle_hunter(0.0f);
#endif
#endif

#ifdef COM_COMPAS
					StagHuntRobot *rob1 = (StagHuntRobot*)(simu.robots()[0]);
					StagHuntRobot *rob2 = (StagHuntRobot*)(simu.robots()[1]);

					Hunter* hunter1 = (Hunter*)rob1;
					Hunter* hunter2 = (Hunter*)rob2;

					float distance = rob1->get_pos().dist_to(rob2->get_pos().x(), rob2->get_pos().y());
					distance = distance - rob1->get_radius() - rob2->get_radius();
					hunter1->set_distance_hunter(distance);

					float angle12 = normalize_angle(atan2(rob2->get_pos().y() - rob1->get_pos().y(), rob2->get_pos().x() - rob1->get_pos().x()));
					angle12 = normalize_angle(angle12 - rob1->get_pos().theta());
					hunter1->set_angle_hunter(angle12);

					float angle21 = normalize_angle(atan2(rob1->get_pos().y() - rob2->get_pos().y(), rob1->get_pos().x() - rob2->get_pos().x()));
					angle21 = normalize_angle(angle21 - rob2->get_pos().theta());
					hunter2->set_angle_hunter(angle21);
#endif

					// Number of steps the robots are evaluated
					_nb_eval = i + 1;

#ifdef STAGFOLLOW
					close_hunters = false;
#endif

					// We compute the robots' actions in an ever-changing order
					std::vector<size_t> ord_vect;
					misc::rand_ind(ord_vect, simu.robots().size());

					for(int k = 0; k < ord_vect.size(); ++k)
					{
						int num = (int)(ord_vect[k]);
						assert(num < simu.robots().size());
	
						StagHuntRobot* robot = (StagHuntRobot*)(simu.robots()[num]);
	
						std::vector<float> action = robot->step_action();

						// We compute the movement of the robot
						if(action[0] >= 0 || action[1] >= 0)
						{
							float v1 = 4.0*(action[0] * 2.0-1.0);
							float v2 = 4.0*(action[1] * 2.0-1.0);

#ifdef SLOW_LEADER
							Hunter* hunter = (Hunter*)robot;

							if(hunter->is_leader())
							{
								v1 /= 2.0;
								v2 /= 2.0;
							}
#endif
							
							// v1 and v2 are between -4 and 4
							simu.move_robot(v1, v2, num);

#ifdef DIVERSITY
							if (0 == num)
								for(size_t l = 0; l < action.size(); ++l)
									vec_sm_trial.push_back(action[l]);
#endif

#if defined(SCREAM) && !defined(SCREAM_PREY)
							if(action[2] >= 0.5)
							{
								if(0 == num)
									hunter2->hear_scream();
								else
									hunter1->hear_scream();
							}
#endif
						}
					}

#ifdef DIVERSITY
					std::vector<float>& vec_inputs_rob = ((Hunter*)(simu.robots()[0]))->get_last_inputs();

					for (size_t l = 0; l < vec_inputs_rob.size(); ++l)
						vec_sm_trial.push_back(vec_inputs_rob[l]);
#endif

					update_simu(simu);

					// And then we update their status : food gathered, prey dead
					// For each prey, we check if there are hunters close
					std::vector<int> dead_preys;

#ifdef STAGFOLLOW
					if(distance_leader <= CATCHING_DISTANCE)
						close_hunters = true;
#endif

					for(int k = 2; k < simu.robots().size(); ++k)
						check_status(simu, (Prey*)(simu.robots()[k]), dead_preys, k);

#ifdef FITFOLLOW
#ifdef SMOOTHPROX
					fit_follow += (3400.0f - distance_leader)/3400.0f;
#else
					if(distance_leader <= CATCHING_DISTANCE)
						fit_follow += 1;
#endif
#endif

#ifdef MOVING_LEADER
					dist_traveled += rob->get_pos().dist_to(prev_pos.x(), prev_pos.y())/3400.0f;
					prev_pos.set_x(rob->get_pos().x());
					prev_pos.set_y(rob->get_pos().y());
#endif
					
					// We remove the dead preys
					while(!dead_preys.empty())
					{
						int index = dead_preys.back();
	
						Prey::type_prey type = ((Prey*)simu.robots()[index])->get_type();
						Posture pos;

						int nb_blocked = ((Prey*)simu.robots()[index])->get_nb_blocked();
						bool alone = (nb_blocked > 1) ? false : true;

#ifdef BEHAVIOUR_LOG
   					// Is the first robot the first one on the target ?
   					bool first_robot = (((Prey*)simu.robots()[index])->get_leader_first() == 1) == (((Hunter*)simu.robots()[0])->is_leader());

						if(this->mode() == fit::mode::view)
						{
							std::string fileDump = _file_behaviour + boost::lexical_cast<std::string>(cpt_files + 1) + ".bmp";
       				simu.add_dead_prey(index, alone, first_robot);
						  // simu.dump_behaviour_log(fileDump.c_str());
       				cpt_files++;
						}
#endif

						bool pred_on_it = ((Prey*)simu.robots()[index])->is_pred_on_it();

						dead_preys.pop_back();
						simu.remove_robot(index);

						if(type == Prey::HARE)
						{
							Posture pos;
		
							if(simu.get_random_initial_position(Params::simu::hare_radius, pos))
							{
								Hare* r = new Hare(Params::simu::hare_radius, pos, HARE_COLOR);
								
								if(pred_on_it)
									r->set_pred_on_it(true);
								
								simu.add_robot(r);
							}
						}
						else if(type == Prey::SMALL_STAG)
						{
							if(simu.get_random_initial_position(Params::simu::big_stag_radius, pos))
							{
								int fur_color = 0;

#ifdef FIXED_POS
								Stag* r = new Stag(Params::simu::big_stag_radius, simu.robots()[index]->get_pos(), BIG_STAG_COLOR, Stag::big, fur_color);
#else
								Stag* r = new Stag(Params::simu::big_stag_radius, pos, BIG_STAG_COLOR, Stag::big, fur_color);
#endif

								if(pred_on_it)
									r->set_pred_on_it(true);
									
								simu.add_robot(r);
							}
				 		}
						else if(type == Prey::BIG_STAG)
						{
							Posture pos;

							if(simu.get_random_initial_position(Params::simu::big_stag_radius, pos))
							{
								int fur_color = 50;

#ifdef FIXED_POS
								Stag* r = new Stag(Params::simu::big_stag_radius, simu.robots()[index]->get_pos(), BIG_STAG_COLOR, Stag::big, fur_color);
#else
								Stag* r = new Stag(Params::simu::big_stag_radius, pos, BIG_STAG_COLOR, Stag::big, fur_color);
#endif
								
								if(pred_on_it)
									r->set_pred_on_it(true);
									
								simu.add_robot(r);
							}
						}

#ifdef FIXED_POS
						std::vector<Posture> pos_init;

#ifdef POS_CLOSE
						pos_init.push_back(Posture(width/2 - 40, 80, M_PI/2));
						pos_init.push_back(Posture(width/2 + 40, 80, M_PI/2));
#else
						pos_init.push_back(Posture(width/2, 80, M_PI/2));
						pos_init.push_back(Posture(width/2, height - 80, -M_PI/2));
#endif

						for(size_t i = 0; i < 2;++i)
							simu.teleport_robot(pos_init[i], i);
#endif
					}
				}

       	Hunter* h1 = (Hunter*)(simu.robots()[0]);
       	food1 += h1->get_food_gathered();
		   	food_trial += h1->get_food_gathered();
       	
       	moy_hares1 += h1->nb_hares_hunted();
       	moy_sstags1 += h1->nb_small_stags_hunted();
       	moy_bstags1 += h1->nb_big_stags_hunted();
				moy_hares1_solo += h1->nb_hares_hunted_solo();
       	moy_sstags1_solo += h1->nb_small_stags_hunted_solo();
       	moy_bstags1_solo += h1->nb_big_stags_hunted_solo();
       	
       	Hunter* h2 = (Hunter*)(simu.robots()[1]);
       	food2 += h2->get_food_gathered();
       	
       	// std::cout << "Trial " << j << " : " << std::endl;
       	// std::cout << "Food 1 : " << h1->get_food_gathered()  << std::endl;
       	// std::cout << "Food 2 : " << h2->get_food_gathered()  << std::endl;
       	// float tmpLeadership = fabs(0.5 - (_nb_leader_first_trial/_nb_preys_killed_coop_trial))/0.5;
       	// std::cout << "Leadership : " << tmpLeadership << std::endl;

       	moy_hares2 += h2->nb_hares_hunted();
       	moy_sstags2 += h2->nb_small_stags_hunted();
       	moy_bstags2 += h2->nb_big_stags_hunted();
				moy_hares2_solo += h2->nb_hares_hunted_solo();
       	moy_sstags2_solo += h2->nb_small_stags_hunted_solo();
       	moy_bstags2_solo += h2->nb_big_stags_hunted_solo();

       	_nb_leader_first += _nb_leader_first_trial;
       	_nb_preys_killed_coop += _nb_preys_killed_coop_trial;

				float max_hunts = Params::simu::nb_steps/STAMINA;
       	if(_nb_preys_killed_coop_trial > 0)
	       	proportion_leader += fabs(0.5 - (_nb_leader_first_trial/_nb_preys_killed_coop_trial));//*(_nb_preys_killed_coop_trial/max_hunts);

	      if(_num_leader == 1)
	      	nb_ind1_leader_first += _nb_leader_first_trial;
	      else
	      	nb_ind1_leader_first += _nb_preys_killed_coop_trial - _nb_leader_first_trial;

#ifdef DOUBLE_NN
				if(hunter1->nn1_chosen())
					fit_nn1 += food_trial;
				else
					fit_nn2 += food_trial;
#endif

#ifdef DIVERSITY
	      for (size_t l = 0; l < vec_sm_trial.size(); ++l)
	      	vec_sm.push_back(vec_sm_trial[l]);
#endif


#if defined(BEHAVIOUR_LOG)
	 			if(this->mode() == fit::mode::view)
	 			{
					std::string fileDump = _file_behaviour + ".bmp";
 	 				simu.dump_behaviour_log(fileDump.c_str());
	 			}
#endif

	 			// std::cout << h1->nb_big

#if defined(BEHAVIOUR_VIDEO)
				if(this->mode() == fit::mode::view)
				{
					return;
				}
#endif

#ifdef GENOME_TRACES
				if(genome_traces)
				{
					_nb_ind1_leader_first = nb_ind1_leader_first;
					_nb_preys_killed_ind1 = h1->nb_hares_hunted();
					_nb_preys_killed_ind1 += h1->nb_small_stags_hunted();
					_nb_preys_killed_ind1 += h1->nb_big_stags_hunted();
					return;
				}
#endif
			}
			
			// Mean on all the trials
     	moy_hares1 /= Params::simu::nb_trials;
     	moy_sstags1 /= Params::simu::nb_trials;
     	moy_bstags1 /= Params::simu::nb_trials;
      moy_hares1_solo /= Params::simu::nb_trials;
     	moy_sstags1_solo /= Params::simu::nb_trials;
     	moy_bstags1_solo /= Params::simu::nb_trials;

			food1 /= Params::simu::nb_trials;


     	moy_hares2 /= Params::simu::nb_trials;
     	moy_sstags2 /= Params::simu::nb_trials;
     	moy_bstags2 /= Params::simu::nb_trials;
      moy_hares2_solo /= Params::simu::nb_trials;
     	moy_sstags2_solo /= Params::simu::nb_trials;
     	moy_bstags2_solo /= Params::simu::nb_trials;

			food2 /= Params::simu::nb_trials;

			_nb_leader_first /= Params::simu::nb_trials;
			_nb_preys_killed_coop /= Params::simu::nb_trials;
			proportion_leader /= Params::simu::nb_trials;
			nb_ind1_leader_first /= Params::simu::nb_trials;

#ifdef DOUBLE_NN
			nb_nn1_chosen /= Params::simu::nb_trials;
			nb_bigger_nn1_chosen /= Params::simu::nb_trials;
			nb_role_divisions /= Params::simu::nb_trials;
			fit_nn1 /= Params::simu::nb_trials;
			fit_nn2 /= Params::simu::nb_trials;
#endif
		
#ifdef NOT_AGAINST_ALL	
			int nb_encounters = Params::pop::nb_opponents*Params::pop::nb_eval;
#elif defined(ALTRUISM)
			int nb_encounters = 1;
#else
			int nb_encounters = Params::pop::size - 1;
#endif

     	moy_hares1 /= nb_encounters;
     	moy_sstags1 /= nb_encounters;
     	moy_bstags1 /= nb_encounters;
      moy_hares1_solo /= nb_encounters;
     	moy_sstags1_solo /= nb_encounters;
     	moy_bstags1_solo /= nb_encounters;

     	moy_hares2 /= nb_encounters;
     	moy_sstags2 /= nb_encounters;
     	moy_bstags2 /= nb_encounters;
      moy_hares2_solo /= nb_encounters;
     	moy_sstags2_solo /= nb_encounters;
     	moy_bstags2_solo /= nb_encounters;

			_nb_leader_first /= nb_encounters;
			_nb_preys_killed_coop /= nb_encounters;
			proportion_leader /= nb_encounters;
			proportion_leader /= 0.5f; 
			nb_ind1_leader_first /= nb_encounters;
			_nb_ind1_leader_first = nb_ind1_leader_first;

#ifdef DOUBLE_NN
			nb_nn1_chosen /= nb_encounters;
			nb_bigger_nn1_chosen /= nb_encounters;
			nb_role_divisions /= nb_encounters;
			fit_nn1 /= nb_encounters;
			fit_nn2 /= nb_encounters;
#endif

     	food2 /= nb_encounters;
     	food1 /= nb_encounters;
     	
			ind1.add_nb_hares(moy_hares1, moy_hares1_solo);
			ind1.add_nb_sstag(moy_sstags1, moy_sstags1_solo);
			ind1.add_nb_bstag(moy_bstags1, moy_bstags1_solo);

			ind1.add_nb_leader_first(_nb_leader_first);
			ind1.add_nb_preys_killed_coop(_nb_preys_killed_coop);
			ind1.add_proportion_leader(proportion_leader);
			ind1.add_nb_ind1_leader_first(nb_ind1_leader_first);

#ifdef DOUBLE_NN
			ind1.add_nb_nn1_chosen(nb_nn1_chosen);
			ind1.add_nb_bigger_nn1_chosen(nb_bigger_nn1_chosen);
			ind1.add_nb_role_divisions(nb_role_divisions);
			ind1.add_fit_nn1(fit_nn1);
			ind1.add_fit_nn2(fit_nn2);
#endif

     	ind1.fit().add_fitness(food1);
     	
#if (!defined(NOT_AGAINST_ALL) && !defined(ALTRUISM)) || defined(LISTMATCH)
#ifdef LISTMATCH
			if(evaluateInd2)
#else
			if(true)
#endif
			{
				ind2.add_nb_hares(moy_hares2, moy_hares2_solo);
				ind2.add_nb_sstag(moy_sstags2, moy_sstags2_solo);
				ind2.add_nb_bstag(moy_bstags2, moy_bstags2_solo);

				ind2.add_nb_leader_first(_nb_leader_first);
				ind2.add_nb_preys_killed_coop(_nb_preys_killed_coop);
				ind2.add_proportion_leader(proportion_leader);
				ind2.add_nb_ind1_leader_first(nb_ind1_leader_first);

#ifdef DOUBLE_NN
				ind2.add_nb_nn1_chosen(nb_nn1_chosen);
				ind2.add_nb_bigger_nn1_chosen(nb_bigger_nn1_chosen);
				ind2.add_nb_role_divisions(nb_role_divisions);
				ind2.add_fit_nn1(fit_nn1);
				ind2.add_fit_nn2(fit_nn2);
#endif

				ind2.fit().add_fitness(food2);
			}
#endif


#ifdef DIVERSITY
			for(size_t l = 0; l < vec_sm.size(); ++l)
			{
				vec_sm[l] /= nb_encounters;
				ind1.add_vec_sm(vec_sm[l], l);
			}
#endif
    } // *** end of eval ***

    
    template<typename Simu>
      void init_simu(Simu& simu, nn_t &nn1, nn_t &nn2)
    {
  		// Preparing the environment
      prepare_env(simu, nn1, nn2);
      
      // Visualisation mode
			if(this->mode() == fit::mode::view)
			{
			  simu.init_view(true);
				
#if defined(BEHAVIOUR_VIDEO)
				simu.set_file_video(_file_video);
#endif
			}
      
      stop_eval = false;
      reinit = false;
    }

    // Prepare environment
    template<typename Simu>
      void prepare_env(Simu& simu, nn_t &nn1, nn_t &nn2)
    {
      using namespace fastsim;

      simu.init();
      simu.map()->clear_illuminated_switches();
			
		  // Sizes
	    width=simu.map()->get_real_w();
	    height=simu.map()->get_real_h();

			// Robots :
			// First the robots for the two hunters
			std::vector<Posture> pos_init;
			
#ifdef POS_CLOSE
			pos_init.push_back(Posture(width/2 - 40, 80, M_PI/2));
			pos_init.push_back(Posture(width/2 + 40, 80, M_PI/2));
#else
			pos_init.push_back(Posture(width/2, 80, M_PI/2));
			pos_init.push_back(Posture(width/2, height - 80, -M_PI/2));
#endif

#ifdef INVERS_POS
			invers_pos = misc::flip_coin();
#else
			invers_pos = false;
#endif

#ifdef FIXED_POS
			std::vector<Posture> vec_pos_preys(Params::simu::nb_big_stags + Params::simu::nb_hares);

			if(Params::simu::nb_big_stags + Params::simu::nb_hares == 2)
			{
				float pos_arr[Params::simu::nb_big_stags + Params::simu::nb_hares][2] =
				{
					{width/2 - 600, height/2},
					{width/2 + 600, height/2}
				};

				for(size_t i = 0; i < vec_pos_preys.size(); ++i)
					vec_pos_preys[i] = Posture(pos_arr[i][0], pos_arr[i][1], M_PI/2);
			}

			std::vector<size_t> ord_vect;
			misc::rand_ind(ord_vect, vec_pos_preys.size());
#endif
			
#ifdef COEVO
			bool first_leader = (_num_leader == 1);
#else
#ifdef DEACT_LEADER
			bool first_leader = false;
#else
			bool first_leader = misc::flip_coin();
			// bool first_leader = true;
#endif
			_num_leader = first_leader?1:2;
#endif

#ifdef OBSTACLE_AVOIDANCE
			for(int i = 0; i < 2; ++i)
			{
				Posture pos;
				if(simu.map()->get_random_initial_position(Params::simu::big_stag_radius + 5, pos))
					pos_init[i] = pos;
			}
#endif

			for(int i = 0; i < 2; ++i)
			{
				Hunter* r;				
				Posture pos = (invers_pos) ? pos_init[(i + 1)%2] : pos_init[i];

				// Simple NXOR : if first hunter is supposed to be leader and this is
				// the first hunter we're creating -> true
				//							 if first hunter isn't supposed to be leader and this is
				// not the first hunter we're creating -> true
				// 							 else -> false
				bool leader = first_leader == (i == 0);
		
				if(0 == i)
					r = new Hunter(Params::simu::hunter_radius, pos, HUNTER_COLOR, nn1);
				else
					r = new Hunter(Params::simu::hunter_radius, pos, HUNTER_COLOR, nn2);

#ifdef SOLO
				if(i > 0)
					r->set_deactivated(true);
#endif

				if(!r->is_deactivated())
				{	
					// 12 lasers range sensors 
#ifdef AVOID_LEADER
					if(leader)
					{
						r->add_laser(Laser(0, 50));
						r->add_laser(Laser(M_PI / 3, 50));
						r->add_laser(Laser(-M_PI / 3, 50));
						r->add_laser(Laser(2 * M_PI / 3, 50));
						r->add_laser(Laser(-2 * M_PI / 3, 50));
						r->add_laser(Laser(M_PI, 50));
					}
					else
					{
						r->add_laser(Laser(0, 50));
						r->add_laser(Laser(M_PI / 6, 50));
						r->add_laser(Laser(M_PI / 3, 50));
						r->add_laser(Laser(M_PI / 2, 50));
						r->add_laser(Laser(2*M_PI / 3, 50));
						r->add_laser(Laser(5*M_PI / 6, 50));
						r->add_laser(Laser(M_PI, 50));
						r->add_laser(Laser(-5*M_PI / 6, 50));
						r->add_laser(Laser(-2*M_PI / 3, 50));
						r->add_laser(Laser(-M_PI / 2, 50));
						r->add_laser(Laser(-M_PI / 3, 50));
						r->add_laser(Laser(-M_PI / 6, 50));
					}
#else
					r->add_laser(Laser(0, 50));
					r->add_laser(Laser(M_PI / 6, 50));
					r->add_laser(Laser(M_PI / 3, 50));
					r->add_laser(Laser(M_PI / 2, 50));
					r->add_laser(Laser(2*M_PI / 3, 50));
					r->add_laser(Laser(5*M_PI / 6, 50));
					r->add_laser(Laser(M_PI, 50));
					r->add_laser(Laser(-5*M_PI / 6, 50));
					r->add_laser(Laser(-2*M_PI / 3, 50));
					r->add_laser(Laser(-M_PI / 2, 50));
					r->add_laser(Laser(-M_PI / 3, 50));
					r->add_laser(Laser(-M_PI / 6, 50));
#endif
		
					// 1 linear camera
					r->use_camera(LinearCamera(M_PI/2, Params::simu::nb_camera_pixels));

					r->set_bool_leader(leader);

#ifdef DEACT_LEADER
					if(leader)
						r->set_deactivated(true);
#endif
				}
		
				simu.add_robot(r);
			}

			// Then the hares std::vector<Posture> vec_pos;
			int nb_big_stags = Params::simu::ratio_big_stags*Params::simu::nb_big_stags;
			int nb_small_stags = Params::simu::nb_big_stags - nb_big_stags;

			for(int i = 0; i < Params::simu::nb_hares; ++i)
			{
				Posture pos;

				if(simu.map()->get_random_initial_position(Params::simu::big_stag_radius + 5, pos, 1000, false))
				{
#ifdef FIXED_POS
					Hare* r = new Hare(Params::simu::hare_radius, vec_pos_preys[ord_vect[i]], HARE_COLOR);
#else
					Hare* r = new Hare(Params::simu::hare_radius, pos, HARE_COLOR);
#endif
			
#ifdef COOP_HARE
					if(0 == i)
						r->set_pred_on_it(true);
#endif
		
					simu.add_robot(r);
				}
			}
	
			// And finally the big stags
			for(int i = 0; i < Params::simu::nb_big_stags; ++i)
			{
				int fur_color = 0;
				if(i >= nb_small_stags)
					fur_color = 50;
			
				Posture pos;

				if(simu.map()->get_random_initial_position(Params::simu::big_stag_radius + 5, pos, 1000, false))
				{
#ifdef FIXED_POS
					Stag* r = new Stag(Params::simu::big_stag_radius, vec_pos_preys[ord_vect[Params::simu::nb_hares + i]], BIG_STAG_COLOR, Stag::big, fur_color);
#else
					Stag* r = new Stag(Params::simu::big_stag_radius, pos, BIG_STAG_COLOR, Stag::big, fur_color);
#endif
			
#ifdef COOP_SSTAG
					if(0 == i)
						r->set_pred_on_it(true);
#endif
#ifdef COOP_BSTAG
					if(nb_small_stags == i)
						r->set_pred_on_it(true);
#endif
					simu.add_robot(r);
				}
			}

#ifdef LIGHTS
			for(int i = 0; i < Params::simu::nb_lights; ++i)
			{
				Posture pos;

				if(simu.map()->get_random_initial_position(Params::simu::light_radius + 5, pos))
				{
					fastsim::Map::ill_sw_t sw = boost::shared_ptr<IlluminatedSwitch>(new IlluminatedSwitch(HARE_COLOR, Params::simu::light_radius, pos.x(), pos.y(), true, HARE_COLOR/100));
					simu.add_illuminated_switch(sw);
				}
			}
#endif
		}
    
    // *** Refresh infos about the robots
    template<typename Simu>
      void update_simu(Simu &simu) 
    {
      // refresh
      using namespace fastsim;
//      simu.refresh();
			
			if (this->mode() == fit::mode::view)
			  simu.refresh_view();
    }
    
    template<typename Simu>
    	void check_status(Simu &simu, Prey* prey, std::vector<int>& dead_preys, int position)
   	{
   		using namespace fastsim;
   		
   		// We compute the distance between this prey and each of its hunters
   		std::vector<Hunter*> hunters;
   		float px = prey->get_pos().x();
   		float py = prey->get_pos().y();
   		
   		for(int i = 0; i < 2; ++i)
   		{
   			Hunter* hunter = (Hunter*)(simu.robots()[i]);
   			float hx = hunter->get_pos().x();
   			float hy = hunter->get_pos().y();
   			float dist = sqrt(pow(px - hx, 2) + pow(py - hy, 2));
   			
   			// The radius must not be taken into account
   			if((dist - prey->get_radius() - hunter->get_radius()) <= CATCHING_DISTANCE)
   				hunters.push_back(hunter);
   		}

   		if(hunters.size() == 1)
   		{
#ifdef COM_NN
   			// If the individual got first on the prey, we change the nn of the other one
   			for(int i = 0; i < 2; ++i)
   			{
	   			Hunter* hunter = (Hunter*)(simu.robots()[i]);

#ifdef DECISION_MAPPING
	   			if(hunter != hunters[0])
						hunter->change_nn_mapping(1);
					else
						hunter->change_nn_mapping(0);
#else
	   			if(hunter != hunters[0])
	   			{
	   				if(hunter->nn1_chosen())
	   					hunter->change_nn(1);
	   			}
#endif
   			}
#elif defined(SCREAM_PREY)
   			// If the individual got first on the prey, we alert the other one
   			for(int i = 0; i < 2; ++i)
   			{
	   			Hunter* hunter = (Hunter*)(simu.robots()[i]);
	   			if(hunter != hunters[0])
	   				hunter->hear_scream();
   			}
#endif

   			if(prey->get_leader_first() == -1)
   			{
	   			if(hunters[0]->is_leader())
	   				prey->set_leader_first(1);
	   			else
	   				prey->set_leader_first(0);
	   		}

	   		if(!(hunters[0]->is_leader()))
	   		{
#if defined(FORCE_LEADERSHIP) || defined(FITSECOND)
					prey->set_no_reward(true);
#elif defined(FORCE_LEADERSHIP_STRONG)
	   			dead_preys.push_back(position);
	   			return;
#endif
	   		}
   		}

   		// We check if the prey is blocked
#ifdef STAGFOLLOW
   		if(close_hunters)
   			if(hunters.size() > 0)
	   			prey->blocked_by_hunters(2);
		   	else
		   		prey->blocked_by_hunters(hunters.size());
	   	else
	   		prey->blocked_by_hunters(hunters.size());
#else
   		prey->blocked_by_hunters(hunters.size());
#endif
   	
   		// And then we check if the prey is dead and let the hunters
   		// have a glorious feast on its corpse. Yay !
   		if(prey->is_dead())
   		{
   			float food = 0;
   			
	   		food = prey->feast();
	   		
   			Prey::type_prey type = prey->get_type();
   			dead_preys.push_back(position);

				bool alone;
				if(prey->get_nb_blocked() <= 1)
					alone = true;
				else
					alone = false;

				_nb_preys_killed++;

				if(!alone)
				{
					_nb_preys_killed_coop_trial++;

					if(prey->get_leader_first() == 1)
						_nb_leader_first_trial++;
				}

   			for(int i = 0; i < hunters.size(); ++i)
   			{
#ifdef FITSECOND
   				if((prey->get_no_reward()) && !(hunters[i]->is_leader()))
   					continue;
#endif

   				hunters[i]->add_food(food);

   				if(type == Prey::HARE)
   					hunters[i]->eat_hare(alone);
   				else if(type == Prey::SMALL_STAG)
   					hunters[i]->eat_small_stag(alone);
   				else
   				{
   					hunters[i]->eat_big_stag(alone);

#ifdef STAG_STUN
   					if(alone)
   						hunters[i]->stun_hunter();
#elif defined(STAG_KILL)
   					if(alone)
   						hunters[i]->kill_hunter();
#endif
   				}

#ifdef EASY_SIMU
   			stop_eval = true;
#endif
   			}

#ifdef COM_NN
   			for(int i = 0; i < 2; ++i)
#ifdef DECISION_MAPPING
					((Hunter*)(simu.robots()[i]))->change_nn_mapping(0);
#else
	   			((Hunter*)(simu.robots()[i]))->change_nn(0);
#endif
#elif defined(SCREAM_PREY)
   			for(int i = 0; i < 2; ++i)
	   			((Hunter*)(simu.robots()[i]))->set_scream(0.0f);
#endif
   		}
   	}
   	    
		
		float width, height;
		bool stop_eval;                                  // Stops the evaluation
		int _nb_eval;                                    // Number of evaluation (until the robot stands still)
		bool invers_pos;
		bool reinit;
		int nb_invers_pos, nb_spawn;

#ifdef STAGFOLLOW
		bool close_hunters;
#endif

		int _num_leader;

		float _nb_leader_first;
		float _nb_preys_killed;
		float _nb_preys_killed_ind1;
		float _nb_preys_killed_coop;

		float _nb_leader_first_trial;
		float _nb_preys_killed_coop_trial;
		float _nb_ind1_leader_first;

		std::string res_dir;
		size_t gen;
		
		void set_res_dir(std::string res_dir)
		{
			this->res_dir = res_dir;
		}
		
		void set_gen(size_t gen)
		{
			this->gen = gen;
		}

#ifdef BEHAVIOUR_VIDEO
		std::string _file_video;
		
		void set_file_video(const std::string& file_video)
		{
			_file_video = file_video;
		}
#endif

#ifdef BEHAVIOUR_LOG
		std::string _file_behaviour;
		int cpt_files;
#endif

		std::vector<float> vec_fitness;
		tbb::atomic<float> fitness_at;
		
		void add_fitness(float fitness)
		{
			fitness_at.fetch_and_store(fitness_at + fitness);
		}
  };
}


// ****************** Main *************************
int main(int argc, char **argv)
{
  srand(time(0));
  
  // FITNESS
  typedef FitStagHunt<Params> fit_t;

	// GENOTYPE
#ifdef DIVERSITY
  typedef gen::EvoFloat<Params::nn::genome_size, Params> gen_t;
#else
#ifdef CMAES
  typedef gen::Cmaes<Params::nn::genome_size, Params> gen_t;
//  typedef gen::Cmaes<90, Params> gen_t;
#else
#ifdef ELITIST
  typedef gen::ElitistGen<Params::nn::genome_size, Params> gen_t;
#else
#ifdef EVOFLOAT
  typedef gen::EvoFloat<Params::nn::genome_size, Params> gen_t;
#else
  typedef gen::ElitistGen<Params::nn::genome_size, Params> gen_t;
#endif
#endif
#endif
#endif
  
  // PHENOTYPE
  typedef phen::PhenChasseur<gen_t, fit_t, Params> phen_t;

	// EVALUATION
	#ifdef COEVO
		typedef eval::StagHuntEvalParallelCoEvo<Params> eval_t;
	#else
		typedef eval::StagHuntEvalParallel<Params> eval_t;
	#endif

  // STATS 
  typedef boost::fusion::vector<
#ifdef COEVO
		  sferes::stat::BestFitEvalCoEvo<phen_t, Params>,
		  // sferes::stat::MeanFitEvalCoEvo<phen_t, Params>,
		  sferes::stat::BestEverFitEvalCoEvo<phen_t, Params>,
		  sferes::stat::AllFitEvalStatCoEvo<phen_t, Params>,
		  sferes::stat::BestLeadershipEvalCoEvo<phen_t, Params>,
		  sferes::stat::AllLeadershipEvalStatCoEvo<phen_t, Params>,
#ifdef BEHAVIOUR_VIDEO
		  sferes::stat::BestFitBehaviourVideoCoEvo<phen_t, Params>,
#endif
#else
		  sferes::stat::BestFitEval<phen_t, Params>,
		  // sferes::stat::MeanFitEval<phen_t, Params>,
		  sferes::stat::BestEverFitEval<phen_t, Params>,
		  sferes::stat::AllFitEvalStat<phen_t, Params>,
		  sferes::stat::BestLeadershipEval<phen_t, Params>,

#ifndef POP100
		  sferes::stat::AllLeadershipEvalStat<phen_t, Params>,

#ifdef GENOME_TRACES
		  sferes::stat::AllGenomesTraceStat<phen_t, Params>,
#endif

#endif
#ifdef DOUBLE_NN
		  sferes::stat::BestNNEval<phen_t, Params>,
		  sferes::stat::AllNNEvalStat<phen_t, Params>,
#endif
#ifdef BEHAVIOUR_VIDEO
		  sferes::stat::BestFitBehaviourVideo<phen_t, Params>,
#endif
#endif
#ifdef DIVERSITY
		  sferes::stat::BestDiversityEval<phen_t, Params>,
		  sferes::stat::AllDiversityEvalStat<phen_t, Params>,
		  // sferes::stat::MeanDiversityEval<phen_t, Params>,
		  // sferes::stat::BestEverDiversityEval<phen_t, Params>,
#endif
		  sferes::stat::StopEval<Params>
    >  stat_t;
  
  //MODIFIER
  typedef modif::Dummy<Params> modifier_t;

	// EVOLUTION ALGORITHM
#ifdef DIVERSITY
#ifdef COEVO
	typedef ea::Nsga2CoEvo<phen_t, eval_t, stat_t, modifier_t, Params> ea_t; 
#else
	typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t; 
#endif
#else
#ifdef FITPROP
  typedef ea::FitnessProp<phen_t, eval_t, stat_t, modifier_t, Params> ea_t; 
#elif defined(CMAES)
  typedef ea::Cmaes<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#elif defined(ELITIST)
#ifdef COEVO
  typedef ea::ElitistCoEvo<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#else
  typedef ea::Elitist<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#endif
#else
  typedef ea::RankSimple<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#endif
#endif

  ea_t ea;

  // RUN EA
  time_t exec = time(NULL);
  run_ea(argc, argv, ea);
	std::cout << "Temps d'exécution : " << time(NULL) - exec;

  return 0;
}
