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




#ifndef FITNESS_PROP_HPP_
#define FITNESS_PROP_HPP_

#include <algorithm>
#include <boost/foreach.hpp>
#include <sferes/stc.hpp>
#include <sferes/ea/ea.hpp>
#include <sferes/fit/fitness.hpp>

namespace sferes
{
  namespace ea
  {
    SFERES_EA(FitnessProp, Ea)
    {
    public:
      typedef boost::shared_ptr<Phen > indiv_t;
      typedef typename std::vector<indiv_t> pop_t;
      typedef typename pop_t::iterator it_t;

      static const unsigned nb_keep = (unsigned)(Params::pop::keep_rate * Params::pop::size);

      void random_pop()
      {
				this->_pop.resize(Params::pop::size * Params::pop::initial_aleat);
				BOOST_FOREACH(boost::shared_ptr<Phen>& indiv, this->_pop)
				{
					indiv = boost::shared_ptr<Phen>(new Phen());
					indiv->random();
				}

				this->_eval.set_res_dir(this->_res_dir);
				this->_eval.set_gen(this->_gen);
				this->_eval.eval(this->_pop, 0, this->_pop.size());
				this->apply_modifier();
				std::partial_sort(this->_pop.begin(), this->_pop.begin() + Params::pop::size,
							this->_pop.end(), fit::compare());
				this->_pop.resize(Params::pop::size);
      }
      
      void epoch()
      {
				assert(this->_pop.size());
				this->_eval.set_gen(this->_gen);
				
				// We calculate a bias, used when fitness can be negative
				float worst_fit = this->_pop[this->_pop.size() - 1]->fit().value();

				float bias = 0.0;				
				if(worst_fit < 0)
					bias = 1.0 - worst_fit;
				
				// We first create the children of the new generation
				pop_t new_pop;
				int nb_children = Params::pop::size - nb_keep;
				new_pop.resize(nb_children);
				float tot_fitness = 0;
				for (unsigned i = 0; i < this->_pop.size(); ++i)
					tot_fitness += (this->_pop[i]->fit().value() + bias);

				if(tot_fitness > 0)
				{		
#ifndef RANK_PROP
					for (unsigned i = 0; i < nb_children; i += 2)
					{
						float rand1, rand2;
						rand1 = misc::rand(tot_fitness);
						rand2 = misc::rand(tot_fitness);
						int p1 = -1, p2 = -1;
						float sum = 0;
						for (unsigned j = 0; j < this->_pop.size() && (rand1 != -1 || rand2 != -1); ++j)
						{
							sum += (this->_pop[j]->fit().value() + bias);
						
							if((-1 != rand1) && (rand1 <= sum))
							{
								// Parent selected
								if(-1 == p1)
									p1 = j;
								else
									p2 = j;
								
								rand1 = -1;
							}						
						
							if((-1 != rand2) && (rand2 <= sum))
							{
								// Parent selected
								if(-1 == p1)
									p1 = j;
								else
									p2 = j;
								
								rand2 = -1;
							}						
						}
					
						assert(rand1 == -1);
						assert(rand2 == -1);
						assert(p1 != -1);
						assert(p2 != -1);
						this->_pop[p1]->cross(this->_pop[p2], new_pop[i], new_pop[i + 1]);					
					}
				
					// We add the individuals from the previous generation
					for (unsigned i = nb_children; i < Params::pop::size; ++i)
					{
						int parent = -1;
						
						if(tot_fitness > 0)
						{
							float rand = misc::rand(tot_fitness);
							float fit_parent = 0;
							float sum = 0;
							for (unsigned j = 0; j < this->_pop.size() && rand != -1; ++j)
							{
								fit_parent = this->_pop[j]->fit().value() + bias;
								sum += fit_parent;
						
								// Parent selected
								if(rand <= sum)
								{
									parent = j;
									break;
								}
							}
						}
					
						assert(parent != -1);
						new_pop.push_back(this->_pop[parent]->clone());
						//new_pop.push_back(this->_pop[parent]);
					
						// We delete this individual so that it is not chosen again
//						this->_pop.erase(this->_pop.begin() + parent);
					
						// And we've got to change the total fitness
/*						tot_fitness = 0;
						for (unsigned j = 0; j < this->_pop.size(); ++j)
							tot_fitness += (this->_pop[j]->fit().value() + bias);*/
					}
#else
					for (unsigned i = 0; i < nb_children; i += 2)
					{
						float rand1, rand2;
						rand1 = misc::rand<float>();
						rand2 = misc::rand<float>();
						int p1 = -1, p2 = -1;
						float sum = 0;
						for (unsigned j = 0; j < this->_pop.size() && (rand1 != -1 || rand2 != -1); ++j)
						{
							sum += (1.0f / (double)pow(2.0f, j + 1));
						
							if((-1 != rand1) && (rand1 <= sum))
							{
								// Parent selected
								if(-1 == p1)
									p1 = j;
								else
									p2 = j;
								
								rand1 = -1;
							}						
						
							if((-1 != rand2) && (rand2 <= sum))
							{
								// Parent selected
								if(-1 == p1)
									p1 = j;
								else
									p2 = j;
								
								rand2 = -1;
							}						
						}
					
						assert(rand1 == -1);
						assert(rand2 == -1);
						assert(p1 != -1);
						assert(p2 != -1);
						this->_pop[p1]->cross(this->_pop[p2], new_pop[i], new_pop[i + 1]);					
					}
				
					// We add the individuals from the previous generation
					for (unsigned i = nb_children; i < Params::pop::size; ++i)
					{
						int parent = -1;
						
						if(tot_fitness > 0)
						{
							float rand = misc::rand<float>();
							float fit_parent = 0;
							float sum = 0;
							for (unsigned j = 0; j < this->_pop.size() && rand != -1; ++j)
							{
								sum += (1.0f / (double)pow(2.0f, j + 1));
						
								// Parent selected
								if(rand <= sum)
								{
									parent = j;
									break;
								}
							}
						}
					
						assert(parent != -1);
						new_pop.push_back(this->_pop[parent]->clone());
					}
#endif
				}
				else
				{
					new_pop.resize(Params::pop::size);
					for(unsigned i = 0; i < this->_pop.size(); ++i)
						//new_pop[i] = this->_pop[i];
						new_pop[i] = this->_pop[i]->clone();
				}
				
				// We finally replace the new population (and mutate the individuals in the process)
				for(unsigned i = 0; i < this->_pop.size(); ++i)
					this->_pop[i].reset();

				this->_pop.clear();
				this->_pop.resize(Params::pop::size);
				assert(this->_pop.size() == new_pop.size());

				for (unsigned i = 0; i < new_pop.size(); ++i)
				{
#ifndef NO_MUTATION
					new_pop[i]->mutate();
#endif
					this->_pop[i] = new_pop[i];
				}

				this->_eval.eval(this->_pop, 0, Params::pop::size);

/*#ifndef EA_EVAL_ALL
				this->_eval.eval(this->_pop, nb_keep, Params::pop::size);
#else
				this->_eval.eval(this->_pop, 0, Params::pop::size);
#endif*/
				
				this->apply_modifier();
				std::partial_sort(this->_pop.begin(), this->_pop.begin() + Params::pop::size,
							this->_pop.end(), fit::compare());
				dbg::out(dbg::info, "ea")<<"best fitness: " << this->_pop[0]->fit().value() << std::endl;
      }

      void load_genome(const std::string fname) 
      {
        // Add a particular individual to the population
        std::string path = fname.substr(0, fname.find_last_of("/") + 1);
        std::ifstream ifs2((path + "genome.dat").c_str());

        if (!ifs2.fail())
        {
          std::cout << "Adding individual(s) with genotype in genome.dat..." << std::endl;
          
          int numIndiv = 0;
          while(ifs2.good())
          {
            std::string line;
            std::getline(ifs2, line);

            std::stringstream ss(line);
            std::string gene;
            int cpt = 0;
            while(std::getline(ss, gene, ','))
            {
              this->_pop[this->_pop.size() - numIndiv - 1]->gen().data(cpt, std::atof(gene.c_str()));
              cpt++;
            }

            numIndiv++;
          }
        }

				this->_eval.eval(this->_pop, 0, this->_pop.size());
      }

      void fill_pop()
      {
        std::cout << "Duplicating individuals from pop_size = " << this->_pop.size() << " to pop_size = " << Params::pop::size << std::endl;

        int cur_pop_size = this->_pop.size();
        this->_pop.resize(Params::pop::size);

        int cpt_indiv = 0;
        for(size_t i = cur_pop_size; i < Params::pop::size; ++i)
        {
          this->_pop[i] = this->_pop[cpt_indiv]->clone();
          cpt_indiv = (cpt_indiv + 1)%cur_pop_size;
        }
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
    };
  }
}
#endif
