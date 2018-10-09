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




#ifndef ALL_FIT_EVAL_CO_EVO_
#define ALL_FIT_EVAL_CO_EVO_

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <sferes/stat/stat.hpp>

namespace sferes
{
  namespace stat
  {
    // assume that the population is sorted !
    SFERES_STAT(AllFitEvalStatCoEvo, Stat)
    {
    public:
      template<typename E>
				void refresh(const E& ea)
      {
				assert(!ea.pop_co().empty());
        this->_create_log_file(ea, "allfitevalstat.dat");
        this->_create_log_file_genome(ea, "allgenomes.dat");
				this->_create_log_file_co(ea, "allfitevalstat-co.dat");
				this->_create_log_file_genome_co(ea, "allgenomes-co.dat");

				if (ea.dump_enabled())
				{
          for(int i = 0; i < ea.pop().size(); ++i)
          {
            (*this->_log_file) << ea.nb_eval() << "," << ea.pop()[i]->fit().value();
            (*this->_log_file) << "," << ea.pop()[i]->nb_hares() << "," << ea.pop()[i]->nb_hares_solo() << "," << ea.pop()[i]->nb_sstag() << "," << ea.pop()[i]->nb_sstag_solo() << "," << ea.pop()[i]->nb_bstag() << "," << ea.pop()[i]->nb_bstag_solo() << std::endl;
          
            (*this->_log_file_genome) << ea.nb_eval();
            for(size_t j = 0; j < ea.pop()[i]->gen().size(); ++j)
            {
              (*this->_log_file_genome) << "," << ea.pop()[i]->gen().data(j);
            }
            (*this->_log_file_genome) << std::endl;
          }

					for(int i = 0; i < ea.pop_co().size(); ++i)
					{
						(*this->_log_file_co) << ea.nb_eval() << "," << ea.pop_co()[i]->fit().value();
						(*this->_log_file_co) << "," << ea.pop_co()[i]->nb_hares() << "," << ea.pop_co()[i]->nb_hares_solo() << "," << ea.pop_co()[i]->nb_sstag() << "," << ea.pop_co()[i]->nb_sstag_solo() << "," << ea.pop_co()[i]->nb_bstag() << "," << ea.pop_co()[i]->nb_bstag_solo() << std::endl;
					
						(*this->_log_file_genome_co) << ea.nb_eval();
						for(size_t j = 0; j < ea.pop_co()[i]->gen().size(); ++j)
						{
							(*this->_log_file_genome_co) << "," << ea.pop_co()[i]->gen().data(j);
						}
						(*this->_log_file_genome_co) << std::endl;
					}
				}
      }

      void show(std::ostream& os, size_t k)
      {
      	std::cout << "No sense in showing this stat !" << std::endl;
      }

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
      }
    protected:
      boost::shared_ptr<std::ofstream> _log_file_genome;
      boost::shared_ptr<std::ofstream> _log_file_co;
      boost::shared_ptr<std::ofstream> _log_file_genome_co;
      
      template<typename E>
      void _create_log_file_genome(const E& ea, const std::string& name)
      {
				if (!_log_file_genome && ea.dump_enabled())
				{
					std::string log = ea.res_dir() + "/" + name;
					_log_file_genome = boost::shared_ptr<std::ofstream>(new std::ofstream(log.c_str()));
				}
      }
      
      template<typename E>
      void _create_log_file_co(const E& ea, const std::string& name)
      {
        if (!_log_file_co && ea.dump_enabled())
        {
          std::string log = ea.res_dir() + "/" + name;
          _log_file_co = boost::shared_ptr<std::ofstream>(new std::ofstream(log.c_str()));
        }
      }
      
      template<typename E>
      void _create_log_file_genome_co(const E& ea, const std::string& name)
      {
        if (!_log_file_genome_co && ea.dump_enabled())
        {
          std::string log = ea.res_dir() + "/" + name;
          _log_file_genome_co = boost::shared_ptr<std::ofstream>(new std::ofstream(log.c_str()));
        }
      }
    };
  }
}
#endif
