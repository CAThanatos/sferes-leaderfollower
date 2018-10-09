#ifndef ALL_NN_EVAL_
#define ALL_NN_EVAL_

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <sferes/stat/stat.hpp>

namespace sferes
{
  namespace stat
  {
    // assume that the population is sorted !
    SFERES_STAT(AllNNEvalStat, Stat)
    {
    public:
      template<typename E>
				void refresh(const E& ea)
      {
        this->_create_log_file(ea, "allnnevalstat.dat");

				if (ea.dump_enabled())
				{
          for(int i = 0; i < ea.pop().size(); ++i)
          {
#ifdef DIVERSITY
            (*this->_log_file) << ea.nb_eval() << "," << ea.pop()[i]->fit().obj(0);
#else
            (*this->_log_file) << ea.nb_eval() << "," << ea.pop()[i]->fit().value();
#endif

            (*this->_log_file) << "," << ea.pop()[i]->nb_nn1_chosen();
            (*this->_log_file) << "," << ea.pop()[i]->nb_bigger_nn1_chosen();
            (*this->_log_file) << "," << ea.pop()[i]->nb_role_divisions();
            (*this->_log_file) << "," << ea.pop()[i]->genome_size();
            (*this->_log_file) << "," << ea.pop()[i]->fit_nn1();
            (*this->_log_file) << "," << ea.pop()[i]->fit_nn2() << std::endl;;
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
      boost::shared_ptr<std::ofstream> _log_file;
      
      template<typename E>
      void _create_log_file(const E& ea, const std::string& name)
      {
				if (!_log_file && ea.dump_enabled())
				{
					std::string log = ea.res_dir() + "/" + name;
					_log_file = boost::shared_ptr<std::ofstream>(new std::ofstream(log.c_str()));
				}
      }
    };
  }
}
#endif
