#ifndef ALL_LEADERSHIP_EVAL_CO_EVO_
#define ALL_LEADERSHIP_EVAL_CO_EVO_

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <sferes/stat/stat.hpp>

namespace sferes
{
  namespace stat
  {
    // assume that the population is sorted !
    SFERES_STAT(AllLeadershipEvalStatCoEvo, Stat)
    {
    public:
      template<typename E>
				void refresh(const E& ea)
      {
				assert(!ea.pop_co().empty());
        this->_create_log_file_leadership(ea, "allleadershipevalstat.dat");
				this->_create_log_file_leadership_co(ea, "allleadershipevalstat-co.dat");

				if (ea.dump_enabled())
				{
          for(int i = 0; i < ea.pop().size(); ++i)
          {
#ifdef DIVERSITY
            (*this->_log_file_leadership) << ea.nb_eval() << "," << ea.pop()[i]->fit().obj(0);
#else
            (*this->_log_file_leadership) << ea.nb_eval() << "," << ea.pop()[i]->fit().value();
#endif

            float proportion_leader_first = ea.pop()[i]->nb_leader_first()/ea.pop()[i]->nb_preys_killed_coop();
            (*this->_log_file_leadership) << "," << proportion_leader_first;
            (*this->_log_file_leadership) << "," << ea.pop()[i]->nb_leader_first() << "," << ea.pop()[i]->nb_preys_killed_coop() << std::endl;
          }

					for(int i = 0; i < ea.pop_co().size(); ++i)
					{
#ifdef DIVERSITY
            (*this->_log_file_leadership_co) << ea.nb_eval() << "," << ea.pop_co()[i]->fit().obj(0);
#else
            (*this->_log_file_leadership_co) << ea.nb_eval() << "," << ea.pop_co()[i]->fit().value();
#endif

            float proportion_leader_first = ea.pop_co()[i]->nb_leader_first()/ea.pop_co()[i]->nb_preys_killed_coop();
            (*this->_log_file_leadership_co) << "," << proportion_leader_first;
            (*this->_log_file_leadership_co) << "," << ea.pop_co()[i]->nb_leader_first() << "," << ea.pop_co()[i]->nb_preys_killed_coop() << std::endl;
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
      boost::shared_ptr<std::ofstream> _log_file_leadership;
      boost::shared_ptr<std::ofstream> _log_file_leadership_co;
      
      template<typename E>
      void _create_log_file_leadership(const E& ea, const std::string& name)
      {
				if (!_log_file_leadership && ea.dump_enabled())
				{
					std::string log = ea.res_dir() + "/" + name;
					_log_file_leadership = boost::shared_ptr<std::ofstream>(new std::ofstream(log.c_str()));
				}
      }
      
      template<typename E>
      void _create_log_file_leadership_co(const E& ea, const std::string& name)
      {
        if (!_log_file_leadership_co && ea.dump_enabled())
        {
          std::string log = ea.res_dir() + "/" + name;
          _log_file_leadership_co = boost::shared_ptr<std::ofstream>(new std::ofstream(log.c_str()));
        }
      }
    };
  }
}
#endif
