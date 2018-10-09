#ifndef BEST_LEADERSHIP_EVAL_
#define BEST_LEADERSHIP_EVAL_

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <sferes/stat/stat.hpp>

namespace sferes
{
  namespace stat
  {
    // assume that the population is sorted !
    SFERES_STAT(BestLeadershipEval, Stat)
    {
    public:
      template<typename E>
				void refresh(const E& ea)
      {
        _best = *ea.pop().begin();

#ifdef DIVERSITY
        float max = ea.pop()[0]->fit().obj(0);
        for(size_t i = 0; i < ea.pop().size(); ++i)
        {
          if(ea.pop()[i]->fit().obj(0) > max)
          {
            _best = ea.pop()[i];
            max = _best->fit().obj(0);
          }
        }
#endif

				this->_create_log_file_leadership(ea, "bestleadership.dat");

				if (ea.dump_enabled())
				{
#ifdef DIVERSITY
          (*this->_log_file_leadership) << ea.nb_eval() << "," << _best->fit().obj(0);
#else
					(*this->_log_file_leadership) << ea.nb_eval() << "," << _best->fit().value();
#endif

          (*this->_log_file_leadership) << "," << _best->proportion_leader();
          float proportion_leader_first = _best->nb_leader_first()/_best->nb_preys_killed_coop();
          (*this->_log_file_leadership) << "," << proportion_leader_first;
          (*this->_log_file_leadership) << "," << _best->nb_leader_first() << "," << _best->nb_preys_killed_coop() << "," << std::endl;
				}
      }
      void show(std::ostream& os, size_t k)
      {
				_best->develop();
				_best->show(os);
				_best->fit().set_mode(fit::mode::view);
				_best->fit().eval_compet(*_best, *_best);
      }
      const boost::shared_ptr<Phen> best() const { return _best; }
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
        ar & BOOST_SERIALIZATION_NVP(_best);
      }
    protected:
      boost::shared_ptr<Phen> _best;
      
      boost::shared_ptr<std::ofstream> _log_file_leadership;
      
      template<typename E>
      void _create_log_file_leadership(const E& ea, const std::string& name)
      {
				if (!_log_file_leadership && ea.dump_enabled())
				{
					std::string log = ea.res_dir() + "/" + name;
					_log_file_leadership = boost::shared_ptr<std::ofstream>(new std::ofstream(log.c_str()));
				}
      }
    };
  }
}
#endif
