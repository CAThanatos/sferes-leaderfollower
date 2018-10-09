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




#ifndef ELITIST_COEVO_HPP_
#define ELITIST_COEVO_HPP_

#include <algorithm>
#include <boost/foreach.hpp>
#include <sferes/stc.hpp>
#include <sferes/ea/ea.hpp>
#include <sferes/fit/fitness.hpp>

#include <cmath>

namespace sferes
{
  namespace ea
  {
    SFERES_EA(ElitistCoEvo, Ea)
    {
    public:
      typedef boost::shared_ptr<Phen > indiv_t;
      typedef typename std::vector<indiv_t> pop_t;
      typedef typename pop_t::iterator it_t;
      
      static const unsigned mu = Params::pop::mu;
      static const unsigned lambda = Params::pop::lambda;

			ElitistCoEvo() : _step_size(1), _step_size_co(1)
			{ }

      void random_pop()
      {
				this->_pop.resize(mu);
				BOOST_FOREACH(boost::shared_ptr<Phen>& indiv, this->_pop)
				{
					indiv = boost::shared_ptr<Phen>(new Phen());
					indiv->random();
				}

				this->_pop_co.resize(mu);
				BOOST_FOREACH(boost::shared_ptr<Phen>& indiv, this->_pop_co)
				{
					indiv = boost::shared_ptr<Phen>(new Phen());
					indiv->random();
				}

				_mixed_pop.clear();
				_merge(this->_pop, this->_pop_co, this->_mixed_pop);

				this->_eval.eval(this->_mixed_pop, 0, this->_mixed_pop.size());
				this->apply_modifier();
				std::partial_sort(this->_pop.begin(), this->_pop.begin() + mu,
							this->_pop.end(), fit::compare());
				this->_pop.resize(mu);
      }
      
      void epoch()
      {
      	// Population 1
				assert(this->_pop.size()); 

				// We create the children
				pop_t child_pop;
				std::vector<int> parents_list(lambda);
				int parent_rank = 0;
				for(size_t i = 0; i < lambda; ++i)
				{
					// Cloning of a parent
					child_pop.push_back(this->_pop[parent_rank%mu]->clone());
					parents_list[i] = parent_rank%mu;
					parent_rank++;
					
					// Mutation
					child_pop[i]->gen().mutate(_step_size);
				}
				assert(child_pop.size() == lambda);

				pop_t selection_pop;
				for(size_t i = 0; i < mu; ++i)
				{
					selection_pop.push_back(this->_pop[i]);
				}
				for(size_t i = 0; i < lambda; ++i)
				{
					selection_pop.push_back(child_pop[i]);
				}
				assert(selection_pop.size() == (mu + lambda));


      	// Population 2
				assert(this->_pop_co.size()); 

				// We create the children
				pop_t child_pop_co;;
				std::vector<int> parents_list_co(lambda);
				parent_rank = 0;
				for(size_t i = 0; i < lambda; ++i)
				{
					// Cloning of a parent
					child_pop_co.push_back(this->_pop_co[parent_rank%mu]->clone());
					parents_list_co[i] = parent_rank%mu;
					parent_rank++;
					
					// Mutation
					child_pop_co[i]->gen().mutate(_step_size_co);
				}
				assert(child_pop_co.size() == lambda);

				pop_t selection_pop_co;
				for(size_t i = 0; i < mu; ++i)
				{
					selection_pop_co.push_back(this->_pop_co[i]);
				}
				for(size_t i = 0; i < lambda; ++i)
				{
					selection_pop_co.push_back(child_pop_co[i]);
				}
				assert(selection_pop_co.size() == (mu + lambda));


				
				// Evaluation of the children and the parents if need be
				pop_t selection_pop_mixed;
				_merge(selection_pop, selection_pop_co, selection_pop_mixed);
				this->_eval.eval(selection_pop_mixed, 0, selection_pop_mixed.size());

				int successful_offsprings = 0;
				int successful_offsprings_co = 0;

#ifdef ONE_PLUS_ONE_REPLACEMENT
				// We count the number of offsprings who are better than their parents
				// and replace the parent population if need be
				std::vector<bool> replaced(mu, false);
				std::vector<bool> replaced_co(mu, false);
				for(size_t i = 0; i < lambda; ++i)
				{
					if(child_pop[i]->fit().value() > this->_pop[parents_list[i]]->fit().value())
					{
						successful_offsprings++;
						
						if(!replaced[parents_list[i]])
						{
							this->_pop[parents_list[i]] = child_pop[i];
							replaced[parents_list[i]] = true;
						}
					}

					if(child_pop_co[i]->fit().value() > this->_pop_co[parents_list_co[i]]->fit().value())
					{
						successful_offsprings++;
						
						if(!replaced_co[parents_list_co[i]])
						{
							this->_pop_co[parents_list_co[i]] = child_pop_co[i];
							replaced_co[parents_list_co[i]] = true;
						}
					}
				}
#elif defined(N_PLUS_N_REPLACEMENT)
				std::partial_sort(this->_pop.begin(), this->_pop.begin() + mu,
							this->_pop.end(), fit::compare());

				std::partial_sort(this->_pop_co.begin(), this->_pop_co.begin() + mu,
							this->_pop_co.end(), fit::compare());

				// We count the number of offsprings better than the worst parent
				for(size_t i = 0; i < lambda; ++i)
				{
					if(child_pop[i]->fit().value() > this->_pop[this->_pop.size() - 1]->fit().value())
					{
						successful_offsprings++;
					}
					if(child_pop_co[i]->fit().value() > this->_pop_co[this->_pop_co.size() - 1]->fit().value())
					{
						successful_offsprings_co++;
					}
				}

				std::partial_sort(selection_pop.begin(), selection_pop.begin() + mu + lambda,
							selection_pop.end(), fit::compare());

				std::partial_sort(selection_pop_co.begin(), selection_pop_co.begin() + mu + lambda,
							selection_pop_co.end(), fit::compare());
															
				// We select the mu best individuals
				for(size_t i = 0; i < mu; ++i)
				{
					this->_pop[i] = selection_pop[i];
					this->_pop_co[i] = selection_pop_co[i];
				}
#endif
							
				// We modify the step_size
				float ps = successful_offsprings/(float)lambda;
				float factor = (1.0f/3.0f) * (ps - 0.2f) / (1.0f - 0.2f);
				_step_size = _step_size * exp(factor);

				assert(this->_pop.size() == mu);

				std::partial_sort(this->_pop.begin(), this->_pop.begin() + mu,
							this->_pop.end(), fit::compare());

				ps = successful_offsprings_co/(float)lambda;
				factor = (1.0f/3.0f) * (ps - 0.2f) / (1.0f - 0.2f);
				_step_size_co = _step_size_co * exp(factor);

				assert(this->_pop_co.size() == mu);

				std::partial_sort(this->_pop_co.begin(), this->_pop_co.begin() + mu,
							this->_pop_co.end(), fit::compare());
							
				dbg::out(dbg::info, "ea")<<"best fitness: " << this->_pop[0]->fit().value() << std::endl;
      }

      void play_run(const std::string& fname)
      {
        std::ifstream ifs(fname.c_str());

        boost::shared_ptr<Phen> ind1 = boost::shared_ptr<Phen>(new Phen());;
        boost::shared_ptr<Phen> ind2 = boost::shared_ptr<Phen>(new Phen());;

        if (!ifs.fail())
        {
          int numIndiv = 0;
          while(ifs.good())
          {
            std::string line;
            std::getline(ifs, line);

            std::stringstream ss(line);
            std::string gene;
            int cpt = 0;
            while(std::getline(ss, gene, ','))
            {
              if(numIndiv == 0)
                ind1->gen().data(cpt, std::atof(gene.c_str()));
              else
                ind2->gen().data(cpt, std::atof(gene.c_str()));

              cpt++;
            }

            numIndiv++;
          }

          ind1->develop();
          ind2->develop();
          ind1->fit().set_mode(fit::mode::view);
          // std::string file_behaviour = ea.res_dir() + "/behaviourGen_" + boost::lexical_cast<std::string>(ea.gen()) + ".bmp";
          // ind1->fit().set_file_behaviour("./videoDir");
          ind1->fit().eval_compet(*ind1, *ind2);
        }
        else
        {
          std::cerr << "Cannot open :" << fname
                    << "(does this file exist ?)" << std::endl;
          exit(1);
        }
      }      
    protected:
    	float _step_size;
    	float _step_size_co;
    	pop_t _mixed_pop;

      void _merge(const pop_t& pop1, const pop_t& pop2, pop_t& pop3)
      {
				assert(pop1.size());
				assert(pop2.size());
				pop3.clear();
				pop3.insert(pop3.end(), pop1.begin(), pop1.end());
				pop3.insert(pop3.end(), pop2.begin(), pop2.end());
				assert(pop3.size() == pop1.size() + pop2.size());
      }
    };
  }
}
#endif
