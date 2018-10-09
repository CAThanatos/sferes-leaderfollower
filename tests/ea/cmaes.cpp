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




#define NO_PARALLEL
#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE cmaes

#ifdef EIGEN2_ENABLED

#include <boost/test/unit_test.hpp>
#include <cmath>
#include <iostream>
#include <limits>

#include <sferes/phen/parameters.hpp>
#include <sferes/gen/cmaes.hpp>
#include <sferes/ea/cmaes.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/best_fit.hpp>
#include <sferes/eval/parallel.hpp>
#include <sferes/modif/dummy.hpp>

using namespace sferes;


struct Params
{
  struct pop
  {
    static const size_t size = 1;//not used by CMAES
    static const unsigned nb_gen = 650;
    static const int dump_period = 10;
  };
  struct cmaes
  {
    static const float sigma = 0.5f;
    static const float max_value = -1e-10;
		static const float lower_bound = -1e10;
		static const float upper_bound = 1e10;
  };
  
 struct parameters
  {
    static const float min = 0.0f;
    static const float max = 1.0f;
  };
};

float felli(const std::vector<float>& xx)
{
  Eigen::VectorXf x = Eigen::VectorXf::Zero(xx.size());
  for (size_t i = 0; i < xx.size(); ++i)
    x[i] = xx[i];
  Eigen::VectorXf v = Eigen::VectorXf::Zero(x.size());
  for (size_t i = 0; i < v.size(); ++i)
    v[i] = powf(1e6, i / (x.size() - 1.0f));
  return v.dot(x.cwise() * x);
}

SFERES_FITNESS(FitElli, sferes::fit::Fitness)
{
 public:
  FitElli(const FitElli& f) { assert(0); BOOST_ERROR("copy constructors should be useless"); }
  FitElli& operator=(const FitElli& f) { BOOST_ERROR("= operator should be useless"); return *this; }
  FitElli() : _this(this) {}
  template<typename Indiv>
    void eval(Indiv& ind) 
  {
    this->_value = -felli(ind.data());
  }
  FitElli* _this;
};

SFERES_FITNESS(FitLol, sferes::fit::Fitness)
{
 public:
  FitLol(const FitLol& f) { assert(0); BOOST_ERROR("copy constructors should be useless"); }
  FitLol& operator=(const FitLol& f) { BOOST_ERROR("= operator should be useless"); return *this; }
  FitLol() : _this(this) {}
  template<typename Indiv>
    void eval(Indiv& ind) 
  {
  	this->_value = 0;
  	for(int i = 0; i < ind.data().size(); ++i)
  	{
  		this->_value -= powf(ind.data(i) - 0, 2);
  		//std::cout << ind.data(i) << "/" << this->_value << std::endl;
  	}
  }
  FitLol* _this;
};


BOOST_AUTO_TEST_CASE(test_cmaes)
{
  srand(time(0));
  typedef gen::Cmaes<10, Params> gen_t;
  typedef phen::Parameters<gen_t, FitElli<Params>, Params> phen_t;
  typedef eval::Parallel<Params> eval_t;
  typedef boost::fusion::vector<stat::BestFit<phen_t, Params> >  stat_t;
  typedef modif::Dummy<> modifier_t;
  typedef ea::Cmaes<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
  ea_t ea;

  ea.run();
  float best = ea.stat<0>().best()->fit().value();
  std::cout<<"best fit (cmaes):"<<best<<std::endl;
  BOOST_CHECK(best > -1e-3);

}

#else
#warning Eigen2 is disabled -> no CMAES
int main() { return 0; }
#endif
