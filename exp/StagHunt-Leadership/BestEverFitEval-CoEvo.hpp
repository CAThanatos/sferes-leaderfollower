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




#ifndef BEST_EVER_FIT_EVAL_CO_EVO_
#define BEST_EVER_FIT_EVAL_CO_EVO_

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <sferes/stat/stat.hpp>

namespace sferes
{
  namespace stat
  {
    // assume that the population is sorted !
    SFERES_STAT(BestEverFitEvalCoEvo, Stat)
    {
    public:
      template<typename E>
				void refresh(const E& ea)
      {
				assert(!ea.pop().empty());


#ifdef DIVERSITY
        boost::shared_ptr<Phen> best = *ea.pop().begin();
        boost::shared_ptr<Phen> best_co = *ea.pop_co().begin();

        float max = ea.pop()[0]->fit().obj(0);
        for(size_t i = 0; i < ea.pop().size(); ++i)
        {
          if(ea.pop()[i]->fit().obj(0) > max)
          {
            best = ea.pop()[i];
            max = best->fit().obj(0);
          }
        }

        float max_co = ea.pop_co()[0]->fit().obj(0);
        for(size_t i = 0; i < ea.pop_co().size(); ++i)
        {
          if(ea.pop_co()[i]->fit().obj(0) > max_co)
          {
            best_co = ea.pop_co()[i];
            max_co = best_co->fit().obj(0);
          }
        }

        if(_bestEver == NULL || best->fit().obj(0) > _bestEver->fit().obj(0))
        {
            _bestEver = boost::shared_ptr<Phen>(new Phen(*best));
        }

        if(_bestEver_co == NULL || best_co->fit().obj(0) > _bestEver_co->fit().obj(0))
        {
            _bestEver_co = boost::shared_ptr<Phen>(new Phen(*best_co));
        }
#else
        if(_bestEver == NULL || (*ea.pop().begin())->fit().value() > _bestEver->fit().value())
        {
            _bestEver = boost::shared_ptr<Phen>(new Phen(**(ea.pop().begin())));
        }

        if(_bestEver_co == NULL || (*ea.pop_co().begin())->fit().value() > _bestEver_co->fit().value())
        {
            _bestEver_co = boost::shared_ptr<Phen>(new Phen(**(ea.pop_co().begin())));
        }
#endif
				this->_create_log_file(ea, "besteverfit.dat");

				if (ea.dump_enabled())
				{
#ifdef DIVERSITY
          (*this->_log_file) << ea.nb_eval() << "," << _bestEver->fit().obj(0);
#else
					(*this->_log_file) << ea.nb_eval() << "," << _bestEver->fit().value();
#endif
					(*this->_log_file) << "," << _bestEver->nb_hares() << "," << _bestEver->nb_hares_solo() << "," << _bestEver->nb_sstag() << "," << _bestEver->nb_sstag_solo() << "," << _bestEver->nb_bstag() << "," << _bestEver->nb_bstag_solo() << std::endl;
				}
        
        this->_create_log_file_co(ea, "besteverfit-co.dat");

        if (ea.dump_enabled())
        {
#ifdef DIVERSITY
          (*this->_log_file_co) << ea.nb_eval() << "," << _bestEver_co->fit().obj(0);
#else
          (*this->_log_file_co) << ea.nb_eval() << "," << _bestEver_co->fit().value();
#endif
          (*this->_log_file_co) << "," << _bestEver_co->nb_hares() << "," << _bestEver_co->nb_hares_solo() << "," << _bestEver_co->nb_sstag() << "," << _bestEver_co->nb_sstag_solo() << "," << _bestEver_co->nb_bstag() << "," << _bestEver_co->nb_bstag_solo() << std::endl;
        }
      }

      void show(std::ostream& os, size_t k)
      {
				_bestEver->develop();
				_bestEver->show(os);
        _bestEver_co->develop();
        _bestEver_co->show(os);
				_bestEver->fit().set_mode(fit::mode::view);
				_bestEver->fit().eval_compet(*_bestEver, *_bestEver_co);
      }
      const boost::shared_ptr<Phen> best() const { return _bestEver; }
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
        ar & BOOST_SERIALIZATION_NVP(_bestEver);
        ar & BOOST_SERIALIZATION_NVP(_bestEver_co);
      }
    protected:
      boost::shared_ptr<Phen> _bestEver;
      boost::shared_ptr<Phen> _bestEver_co;

      boost::shared_ptr<std::ofstream> _log_file_co;
      
      template<typename E>
      void _create_log_file_co(const E& ea, const std::string& name)
      {
        if (!_log_file_co && ea.dump_enabled())
        {
          std::string log = ea.res_dir() + "/" + name;
          _log_file_co = boost::shared_ptr<std::ofstream>(new std::ofstream(log.c_str()));
        }
      }
    };
  }
}
#endif
