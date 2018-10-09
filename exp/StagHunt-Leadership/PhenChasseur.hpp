//| This file is a part of the sferes2 framework.
//| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
//| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//| 
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//| 
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.




#ifndef PHEN_CHASSEUR_HPP_
#define PHEN_CHASSEUR_HPP_

#include <vector>
#include <sferes/phen/indiv.hpp>
#include <boost/foreach.hpp>
#include <tbb/atomic.h>

namespace sferes
{
  namespace phen
  {
    SFERES_INDIV(PhenChasseur, Indiv)
    {
		  template<typename G, typename F, typename P, typename E> 
		    friend std::ostream& operator<<(std::ostream& output, const PhenChasseur< G, F, P, E >& e);
		  public:
#ifdef EIGEN_CORE_H
		    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif
		    PhenChasseur() : _params((*this)._gen.size()), _nb_hares(0), _nb_hares_solo(0), _nb_sstag(0), _nb_sstag_solo(0), _nb_bstag(0), _nb_bstag_solo(0), _nb_leader_first(0), _nb_preys_killed_coop(0), _fit_mov(0), _pop_pos(-1), _developed(false) 					{
		    }
		    
		    typedef float type_t;
		    static const float max_p = Params::parameters::max;
		    static const float min_p = Params::parameters::min;

		    void develop()
		    {
#if defined(DOUBLE_NN) && defined(DUPLICATION)
		    	_params.resize((*this)._gen.size());
#endif
					for (unsigned i = 0; i < _params.size(); ++i)
						_params[i] = this->_gen.data(i) * (max_p - min_p) + min_p;
						
					_nb_hares = 0;
					_nb_hares_solo = 0;
					_nb_sstag = 0;
					_nb_sstag_solo = 0;
					_nb_bstag = 0;
					_nb_bstag_solo = 0;
					_nb_leader_first = 0;
					_nb_preys_killed_coop = 0;
					_proportion_leader = 0;
					_nb_ind1_leader_first = 0;

#ifdef DOUBLE_NN
					_nb_nn1_chosen = 0;
					_nb_bigger_nn1_chosen = 0;
					_nb_role_divisions = 0;
					_fit_nn1 = 0;
					_fit_nn2 = 0;
#endif
					_fit_mov = 0;
					
					this->fit().vec_fitness.clear();
					_developed = true;

					_nb_hares_at = 0;
					_nb_hares_solo_at = 0;
					_nb_sstag_at = 0;
					_nb_sstag_solo_at = 0;
					_nb_bstag_at = 0;
					_nb_bstag_solo_at = 0;
					_nb_leader_first_at = 0;
					_nb_preys_killed_coop_at = 0;
					_proportion_leader_at = 0;
					_nb_ind1_leader_first_at = 0;

#ifdef DOUBLE_NN
					_nb_nn1_chosen_at = 0;
					_nb_bigger_nn1_chosen_at = 0;
					_nb_role_divisions_at = 0;
					_fit_nn1_at = 0;
					_fit_nn2_at = 0;
#endif
					this->fit().fitness_at = 0;

#ifdef DIVERSITY
					_vector_diversity_init = false;
					_vector_diversity.clear();

					_vec_sm_at.clear();
					_vec_sm_at.resize((Params::nn::nb_inputs + Params::nn::nb_outputs)*Params::simu::nb_steps*Params::simu::nb_trials);
					_vec_sm.clear();
					_vec_sm.resize((Params::nn::nb_inputs + Params::nn::nb_outputs)*Params::simu::nb_steps*Params::simu::nb_trials);
#endif
				}

				float data(size_t i) const { assert(i < size()); return _params[i]; }

				size_t genome_size() const { return this->_gen.size(); }

				size_t size() const { return _params.size(); }

				const std::vector<float>& data() const { return _params; }

				// squared Euclidean distance
				float dist(const PhenChasseur& chasseur) const
				{
					assert(chasseur.size() == size());
					float d = 0.0f;
					for (size_t i = 0; i < _params.size(); ++i)
						{
							float x = _params[i] - chasseur._params[i];
							d += x * x;
						}
					return d;
		    }

		    void show(std::ostream& os) const
		    {
		    	for(size_t i = 0; i << this->_gen.size(); ++i)
		    	{
		    		os << this->_gen.data(i) << ",";
		    	}
		    	os << std::endl;
					BOOST_FOREACH(float p, _params)
						os<<p<<",";
					os<<std::endl;
		    }
		    
		    float nb_hares() const { return _nb_hares; }
        float nb_hares_solo() const { return _nb_hares_solo; }
		    void set_nb_hares(float nb_hares, float solo) { _nb_hares = nb_hares; _nb_hares_solo = solo; }

		    float nb_hares_at() const { return _nb_hares_at; }
        float nb_hares_solo_at() const {return _nb_hares_solo_at; }
		    void add_nb_hares(float nb_hares, float solo) { _nb_hares_at.fetch_and_store(_nb_hares_at + nb_hares); _nb_hares_solo_at.fetch_and_store(_nb_hares_solo_at + solo); }

		    
		    float nb_sstag() const { return _nb_sstag; }
		    float nb_sstag_solo() const { return _nb_sstag_solo; }
		    void set_nb_sstag(float nb_sstag, float solo) { _nb_sstag = nb_sstag; _nb_sstag_solo = solo; }
		    
		    float nb_sstag_at() const { return _nb_sstag_at; }
		    float nb_sstag_solo_at() const { return _nb_sstag_solo_at; }
		    void add_nb_sstag(float nb_sstag, float solo) { _nb_sstag_at.fetch_and_store(_nb_sstag_at + nb_sstag); _nb_sstag_solo_at.fetch_and_store(_nb_sstag_solo_at + solo); }


		    float nb_bstag() const { return _nb_bstag; }
		    float nb_bstag_solo() const { return _nb_bstag_solo; }
		    void set_nb_bstag(float nb_bstag, float solo) { _nb_bstag = nb_bstag; _nb_bstag_solo = solo; }
		    
		    float nb_bstag_at() const { return _nb_bstag_at; }
		    float nb_bstag_solo_at() const { return _nb_bstag_solo_at; }
		    void add_nb_bstag(float nb_bstag, float solo) { _nb_bstag_at.fetch_and_store(_nb_bstag_at + nb_bstag); _nb_bstag_solo_at.fetch_and_store(_nb_bstag_solo_at + solo); }

		    
		    float proportion_leader() const { return _proportion_leader; }
		    void set_proportion_leader(float proportion_leader) { _proportion_leader = proportion_leader; }

		    float proportion_leader_at() const { return _proportion_leader_at; }
		    void add_proportion_leader(float proportion_leader) { _proportion_leader_at.fetch_and_store(_proportion_leader_at + proportion_leader); }


		    float nb_leader_first() const { return _nb_leader_first; }
		    void set_nb_leader_first(float nb_leader_first) { _nb_leader_first = nb_leader_first; }
		    
		    float nb_leader_first_at() const { return _nb_leader_first_at; }
		    void add_nb_leader_first(float nb_leader_first) { _nb_leader_first_at.fetch_and_store(_nb_leader_first_at + nb_leader_first); }


		    float nb_preys_killed_coop() const { return _nb_preys_killed_coop; }
		    void set_nb_preys_killed_coop(float nb_preys_killed_coop) { _nb_preys_killed_coop = nb_preys_killed_coop; }
		    
		    float nb_preys_killed_coop_at() const { return _nb_preys_killed_coop_at; }
		    void add_nb_preys_killed_coop(float nb_preys_killed_coop) { _nb_preys_killed_coop_at.fetch_and_store(_nb_preys_killed_coop_at + nb_preys_killed_coop); }


		    float nb_ind1_leader_first() const { return _nb_ind1_leader_first; }
		    void set_nb_ind1_leader_first(float nb_ind1_leader_first) { _nb_ind1_leader_first = nb_ind1_leader_first; }
		    
		    float nb_ind1_leader_first_at() const { return _nb_ind1_leader_first_at; }
		    void add_nb_ind1_leader_first(float nb_ind1_leader_first) { _nb_ind1_leader_first_at.fetch_and_store(_nb_ind1_leader_first_at + nb_ind1_leader_first); }


#ifdef DOUBLE_NN
		    float fit_nn1() const { return _fit_nn1; }
		    void set_fit_nn1(float fit_nn1) { _fit_nn1 = fit_nn1; }
		    
		    float fit_nn1_at() const { return _fit_nn1_at; }
		    void add_fit_nn1(float fit_nn1) { _fit_nn1_at.fetch_and_store(_fit_nn1_at + fit_nn1); }


		    float fit_nn2() const { return _fit_nn2; }
		    void set_fit_nn2(float fit_nn2) { _fit_nn2 = fit_nn2; }
		    
		    float fit_nn2_at() const { return _fit_nn2_at; }
		    void add_fit_nn2(float fit_nn2) { _fit_nn2_at.fetch_and_store(_fit_nn2_at + fit_nn2); }


		    float nb_nn1_chosen() const { return _nb_nn1_chosen; }
		    void set_nb_nn1_chosen(float nb_nn1_chosen) { _nb_nn1_chosen = nb_nn1_chosen; }
		    
		    float nb_nn1_chosen_at() const { return _nb_nn1_chosen_at; }
		    void add_nb_nn1_chosen(float nb_nn1_chosen) { _nb_nn1_chosen_at.fetch_and_store(_nb_nn1_chosen_at + nb_nn1_chosen); }


		    float nb_bigger_nn1_chosen() const { return _nb_bigger_nn1_chosen; }
		    void set_nb_bigger_nn1_chosen(float nb_bigger_nn1_chosen) { _nb_bigger_nn1_chosen = nb_bigger_nn1_chosen; }
		    
		    float nb_bigger_nn1_chosen_at() const { return _nb_bigger_nn1_chosen_at; }
		    void add_nb_bigger_nn1_chosen(float nb_bigger_nn1_chosen) { _nb_bigger_nn1_chosen_at.fetch_and_store(_nb_bigger_nn1_chosen_at + nb_bigger_nn1_chosen); }


		    float nb_role_divisions() const { return _nb_role_divisions; }
		    void set_nb_role_divisions(float nb_role_divisions) { _nb_role_divisions = nb_role_divisions; }
		    
		    float nb_role_divisions_at() const { return _nb_role_divisions_at; }
		    void add_nb_role_divisions(float nb_role_divisions) { _nb_role_divisions_at.fetch_and_store(_nb_role_divisions_at + nb_role_divisions); }
#endif

#ifdef DIVERSITY
		    const std::vector<float>& vector_diversity() const { return _vector_diversity; }
				void set_vector_diversity(size_t index, float value)
				{
					assert(index < _vector_diversity.size());
					_vector_diversity[index] = value;
				}

				bool is_vector_diversity_init() { return _vector_diversity_init; }

				void create_vector_diversity()
				{
					_vector_diversity.resize(_vec_sm.size());
					for (size_t i = 0; i < _vec_sm.size(); ++i)
						_vector_diversity[i] = _vec_sm[i];
				}

				float dist_diversity(const PhenChasseur& chasseur)
				{
					float dist = 0.0f;
					for (size_t i = 0; i < _vector_diversity.size(); ++i)
					{
						assert(0.0f <= _vector_diversity[i]);
						assert(_vector_diversity[i] <= 1.0f);

						assert(0.0f <= chasseur.vector_diversity()[i]);
						assert(chasseur.vector_diversity()[i] <= 1.0f);

						float x = _vector_diversity[i] - chasseur.vector_diversity()[i];
						dist += x * x;
					}

					return sqrtf(dist)/sqrt(_vector_diversity.size());
				}

				float dist_hamming_diversity(const PhenChasseur& chasseur)
				{
					float dist = 0.0f;
					for (size_t i = 0; i < _vector_diversity.size(); ++i)
					{
						assert(0.0f <= _vector_diversity[i]);
						assert(_vector_diversity[i] <= 1.0f);

						assert(0.0f <= chasseur.vector_diversity()[i]);
						assert(chasseur.vector_diversity()[i] <= 1.0f);

						dist += abs((int)_vector_diversity[i] - (int)chasseur.vector_diversity()[i]);
					}

					return dist/(float)_vector_diversity.size();
				}

				void add_vec_sm(float value, int index)
				{
					assert(index < _vec_sm_at.size());
					_vec_sm_at[index].fetch_and_store(_vec_sm_at[index] + value);
				}

				void set_vec_sm()
				{
					_vec_sm.resize(_vec_sm_at.size());
					for (size_t i = 0; i < _vec_sm_at.size(); ++i)
					{
						_vec_sm[i] = _vec_sm_at[i];
						_vec_sm[i] = (_vec_sm[i] > Params::simu::threshold_hamming) ? 1 : 0;
					}
				}

				float vec_sm(int index)
				{
					assert(index < _vec_sm_at.size());
					return _vec_sm_at[index];
				}
#endif


		    float fit_mov() const { return _fit_mov; }
		    void set_fit_mov(float fit_mov) { _fit_mov = fit_mov; }
		    int pop_pos() const { return _pop_pos; }
		    void set_pop_pos(int pop_pos) { _pop_pos = pop_pos; }

				bool developed() const { return _developed; }
				void set_developed(bool developed) { _developed = developed; }
				bool developed_at() const { return _developed_at; }
				void set_developed_at(bool developed) { _developed_at = developed; }
				
				boost::shared_ptr<PhenChasseur> clone()
				{
					boost::shared_ptr<PhenChasseur> new_indiv = boost::shared_ptr<PhenChasseur>(new PhenChasseur());

#if defined(DOUBLE_NN) && defined(DUPLICATION)
					if(this->_gen.size() > new_indiv->_gen.size())
						new_indiv->_gen.resize(this->_gen.size());
#endif

					for(unsigned i = 0; i < this->_gen.size(); ++i)
						new_indiv->_gen.data(i, this->_gen.data(i));
						
					return new_indiv;
				}
      
		  protected:
		    std::vector<float> _params;
		    
		    // Number of hares caughted
		    float _nb_hares;
        float _nb_hares_solo;
        
		    tbb::atomic<float>  _nb_hares_at;
        tbb::atomic<float>  _nb_hares_solo_at;
		    
		    // Number of small stags caughted
		    float _nb_sstag;
		    float _nb_sstag_solo;

		    tbb::atomic<float> _nb_sstag_at;
		    tbb::atomic<float> _nb_sstag_solo_at;
		    
		    // Number of big stags caughted
		    float _nb_bstag;
		    float _nb_bstag_solo;

		    tbb::atomic<float> _nb_bstag_at;
		    tbb::atomic<float> _nb_bstag_solo_at;
		    
		    // Number of times the leader arrived first on a prey
		    float _nb_leader_first;
		    tbb::atomic<float> _nb_leader_first_at;
		    
		    // Total number of cooperative hunts
		    float _nb_preys_killed_coop;
		    tbb::atomic<float> _nb_preys_killed_coop_at;

		    // Proportion of times the designated leader hunted first
		    float _proportion_leader;
		    tbb::atomic<float> _proportion_leader_at;

		    // Proportion of times this particular individual arrived first on a prey
		    float _nb_ind1_leader_first;
		    tbb::atomic<float> _nb_ind1_leader_first_at;

#ifdef DOUBLE_NN
		    // Total number of times the first nn was chosen
		    float _nb_nn1_chosen;
		    tbb::atomic<float> _nb_nn1_chosen_at;

		    // Fitness of the first NN
		    float _fit_nn1;
		    tbb::atomic<float> _fit_nn1_at;

		    // Fitness of the second NN
		    float _fit_nn2;
		    tbb::atomic<float> _fit_nn2_at;

		    // Number of times the first nn was chosen when the random number was bigger
		    float _nb_bigger_nn1_chosen;
		    tbb::atomic<float> _nb_bigger_nn1_chosen_at;

		    // Number of times the individuals chose a different nn
		    float _nb_role_divisions;
		    tbb::atomic<float> _nb_role_divisions_at;
#endif

#ifdef DIVERSITY
		    std::vector<float> _vector_diversity;
		    bool _vector_diversity_init;

		    std::vector<tbb::atomic<float> > _vec_sm_at;
		    std::vector<float> _vec_sm;
#endif
		    
		    float _fit_mov;
		    
		    int _pop_pos;
		    
		    bool _developed;
		    tbb::atomic<bool> _developed_at;
    };
    
    template<typename G, typename F, typename P, typename E> 
    std::ostream& operator<<(std::ostream& output, const PhenChasseur< G, F, P, E >& e) {
      for (size_t i = 0; i < e.size();++i)
        output <<" "<<e.data(i) ;

			output << std::endl;
      output << "Fitness = " << e.fit().value() << std::endl;
      output << "#Hares = " << e.nb_hares() << "/#Hares Solo = " << e.nb_hares_solo() << "/#Small Stags = " << e.nb_sstag() << "/#Small Stags Solo = " << e.nb_sstag_solo() << "/#Big Stags = " << e.nb_bstag() << "/#Big Stags Solo = " << e.nb_bstag_solo() << std::endl;
      output << "Movement fitness = " << e.fit_mov() << std::endl;
      return output;
    }
  }
}

#endif
