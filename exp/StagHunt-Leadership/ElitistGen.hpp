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




#ifndef ELITIST_GEN_HPP_
#define ELITIST_GEN_HPP_

#include <iostream>
#include <cmath>
#include <limits>

#include <boost/foreach.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/static_assert.hpp>

#include <sferes/dbg/dbg.hpp>
#include <sferes/stc.hpp>
#include <sferes/misc.hpp>


namespace sferes
{
  namespace gen
  {
    template<int Size, typename Params, typename Exact = stc::Itself> 
    class ElitistGen : public stc::Any<Exact>
    {
    public:
      typedef Params params_t;
      typedef ElitistGen<Size, Params, Exact> this_t;

      ElitistGen() : _data(Size), _size(Size), _genome_from(-1)
      { }

      void random() 
      {
				BOOST_FOREACH(float &v, _data) v = misc::rand<float>(); 

#ifdef DIFFSTART
        int nb_genes = (Params::nn::nb_inputs + 1) * Params::nn::nb_hidden + Params::nn::nb_outputs * Params::nn::nb_hidden + Params::nn::nb_outputs;
				_data[nb_genes*2] = misc::rand<float>(0.0f, 0.5f);
				_data[nb_genes*2 + 1] = misc::rand<float>(0.5f, 1.0f);
#elif defined(NOISE_CHOICE)
        int nb_genes = (Params::nn::nb_inputs + 1) * Params::nn::nb_hidden + Params::nn::nb_outputs * Params::nn::nb_hidden + Params::nn::nb_outputs;
				_data[nb_genes*2] = 0.5f;
				_data[nb_genes*2 + 1] = 0.5f;
#endif

				_check_invariant(); 
      }
      
      void mutate(float step_size = 1.0f) 
      {
#ifdef GAUSSIAN_MUTATION
      	float sigma = Params::evo_float::sigma;
				for (size_t i = 0; i < _size; i++)
				{
					float randMutation = misc::rand<float>();
#ifdef STRONG_MUTATION
					if (randMutation < Params::evo_float::strong_mutation_rate)
					{
						float f = misc::rand<float>(0.0f, 1.0f);
						_data[i] = misc::put_in_range(f, 0.0f, 1.0f);
						continue;
					}
#endif

#if defined(DECISION_MAPPING) && defined(NU_MUT_MAPP)
					int nb_weights = (Params::nn::nb_inputs + 1) * Params::nn::nb_hidden + Params::nn::nb_outputs * Params::nn::nb_hidden + Params::nn::nb_outputs;
					if((_size > nb_weights) && ((i == (nb_weights * 2)) || (i == (nb_weights * 2 + 1))))
						continue
#endif

					if (randMutation < Params::evo_float::mutation_rate)
					{
						float value_mut = step_size * misc::gaussian_rand<float>(0, sigma * sigma);
						float f = _data[i] + value_mut;
						_data[i] = misc::put_in_range(f, 0.0f, 1.0f);
	  			}
	  		}
#elif defined(UNIFORM_MUTATION)
				for (size_t i = 0; i < _size; i++)
				{
					float randMutation = misc::rand<float>();

					if (randMutation < Params::evo_float::mutation_rate)
					{
						float f = misc::rand<float>(0.0f, 1.0f);
						_data[i] = misc::put_in_range(f, 0.0f, 1.0f);
	  			}
	  		}
#else
				static const float eta_m = Params::evo_float::eta_m;
				assert(eta_m != -1.0f);
				for (size_t i = 0; i < _size; i++)
					if (misc::rand<float>() < Params::evo_float::mutation_rate)
					{
						float ri = misc::rand<float>();
						float delta_i = ri < 0.5 ?
							pow(2.0 * ri, 1.0 / (eta_m + 1.0)) - 1.0 :
							1 - pow(2.0 * (1.0 - ri), 1.0 / (eta_m + 1.0));
						assert(!std::isnan(delta_i));
						assert(!std::isinf(delta_i));
						// std::cout << delta_i << std::endl;
						float value_mut = step_size*delta_i;
						float f = _data[i] + value_mut;
						_data[i] = misc::put_in_range(f, 0.0f, 1.0f);
					}
#endif
				_check_invariant();
	    }

	    void cross(const ElitistGen& o, ElitistGen& c1, ElitistGen& c2)
	    {
	    	if(misc::rand<float>() < Params::evo_float::cross_rate)
	    	{
					int nb_genes = (Params::nn::nb_inputs + 1) * Params::nn::nb_hidden + Params::nn::nb_outputs * Params::nn::nb_hidden + Params::nn::nb_outputs;

					assert(this->size() == nb_genes * 2);
					assert(o.size() == nb_genes * 2);
					assert(c1.size() == nb_genes * 2);
					assert(c2.size() == nb_genes * 2);

					for(size_t i = 0; i < nb_genes; ++i)
					{
						c1.data(i, this->data(i));
						c2.data(i, o.data(i));
					}
					for(size_t i = nb_genes; i < nb_genes*2; ++i)
					{
						c1.data(i, o.data(i));
						c2.data(i, this->data(i));
					}
					c1.data(nb_genes * 2, this->data(nb_genes * 2));
					c2.data(nb_genes * 2, o.data(nb_genes * 2));
	    	}
	    	else
	    	{
	    		for(size_t i = 0; i < _size; ++i)
	    		{
	    			c1.data(i, this->data(i));
	    			c2.data(i, o.data(i));
	    		}
	    	}
	    }

	    void duplicate_nn()
	    {
				int nb_genes = (Params::nn::nb_inputs + 1) * Params::nn::nb_hidden + Params::nn::nb_outputs * Params::nn::nb_hidden + Params::nn::nb_outputs;

	    	int start_duplication = 0;
	    	int end_duplication = nb_genes;
	    	int start_destination = nb_genes;
	    	int end_destination = nb_genes*2;

	    	// One neural network
	    	if(Params::nn::genome_size == _size)
	    	{
	    		_data.resize(Params::nn::genome_size + nb_genes);
	    		_data[2*nb_genes] = _data[nb_genes];
#ifndef DECISION_THRESHOLD
	    		_data[2*nb_genes + 1] = _data[nb_genes + 1];
#endif
	    		_size = Params::nn::genome_size + nb_genes;
	    	}
	    	// Two neural networks
	    	else if((Params::nn::genome_size + nb_genes) == _size)
	    	{
	    		if(misc::flip_coin())
	    		{
	    			start_duplication = start_destination;
	    			end_duplication = end_destination;
	    			start_destination = 0;
	    			end_destination = nb_genes;
	    		}
	    	}
	    	else
	    	{
#ifdef DUPLOAD
	    		_data.resize(Params::nn::genome_size);
	    		_data[2*nb_genes] = _data[nb_genes];
#ifndef DECISION_THRESHOLD
	    		_data[2*nb_genes + 1] = _data[nb_genes + 1];
#endif
	    		_size = Params::nn::genome_size;
#else
	    		std::cout << "There is a problem in your genotype !" << std::endl;
#endif
	    	}

	    	size_t j = start_destination;
	    	for(size_t i = start_duplication; i < end_duplication; ++i)
	    	{
	    		_data[j] = _data[i];
	    		++j;
	    	}

	    	assert(j == end_destination);
	    }

	    void delete_nn()
	    {
				int nb_genes = (Params::nn::nb_inputs + 1) * Params::nn::nb_hidden + Params::nn::nb_outputs * Params::nn::nb_hidden + Params::nn::nb_outputs;

	    	// We cannot lose our only neural network
	    	if((Params::nn::genome_size + nb_genes) == _size)
	    	{
	    		// We lose one network or the other (50/50)
	    		if(misc::flip_coin())
	    		{
	    			int start_duplication = nb_genes;
	    			int end_duplication = nb_genes*2;
	    			int start_destination = 0;
	    			int end_destination = nb_genes;

	    			int j = start_destination;
	    			for(size_t i = start_duplication; i < end_duplication; ++i)
	    			{
	    				assert(i < _size);
	    				assert(j < _size);
	    				_data[j] = _data[i];
	    				++j;
	    			}
	    			assert(j == end_destination);
	    		}

	    		_data[nb_genes] = _data[2*nb_genes];
#ifndef DECISION_THRESHOLD
	    		_data[nb_genes + 1] = _data[2*nb_genes + 1];
#endif
	    		_data.resize(Params::nn::genome_size);
	    		_size = Params::nn::genome_size;
	    	}
	    }

	    std::vector<float>& data()
	    {
	    	return _data;
	    }

      float data(size_t i) const 
      { 
				assert(_data.size());
				assert(i < _data.size()); 
				_check_invariant(); 
				return _data[i]; 
      }

      void data(size_t i, float v)
      { 
				assert(_data.size());
				assert(i < _data.size()); 
				_data[i] = v; 
				_check_invariant(); 
      }
      
      void set_genome_from(int genome_from) { _genome_from = genome_from; }
      int get_genome_from() { return _genome_from; }

      size_t size() const { return _size; }

      void resize(size_t size)
      {
      	_data.resize(size);
      	_size = size;
      }
      
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
        ar & BOOST_SERIALIZATION_NVP(_data);
        ar & BOOST_SERIALIZATION_NVP(_size);
      }

    protected:
      std::vector<float> _data;
      int _size;
      int _genome_from;

      void _check_invariant() const
      {
#ifdef DBG_ENABLED
				BOOST_FOREACH(float p, _data)
				{
					assert(!std::isnan(p)); 
					assert(!std::isinf(p)); 
					assert(p >= 0 && p <= 1); 
				}
#endif
      }      
    };
  } // gen
} // sferes

#endif
