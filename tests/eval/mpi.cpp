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




#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE parallel
#include <iostream>
#include <boost/test/unit_test.hpp>
#if 0
//#ifdef MPI_ENABLED

#include <boost/test/unit_test.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/ea/rank_simple.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/best_fit.hpp>
#include <sferes/stat/mean_fit.hpp>
#include <sferes/modif/dummy.hpp>

#include <sferes/eval/mpi.hpp>

using namespace sferes;
using namespace sferes::gen::evo_float;


struct Params
{
  struct evo_float
  {

    static const float mutation_rate = 0.1f;
    static const float eta_m = 15.0f;
    static const float eta_c = 10.0f;
    static const mutation_t mutation_type = polynomial;
    static const cross_over_t cross_over_type = sbx;
  };
  struct pop
  {
    static const unsigned size = 100;
    static const unsigned nb_gen = 1000;
    static const int dump_period = -1;
    static const int initial_aleat = 1;
    static const float coeff = 1.1f;
    static const float keep_rate = 0.6f;    
  };
  struct parameters
  {
    static const float min = -10.0f;
    static const float max = 10.0f;
  };
};
 
SFERES_FITNESS(FitTest, sferes::fit::Fitness)
{
 public:
  template<typename Indiv>
    void eval(Indiv& ind) 
  {
    float v = 0;
    for (unsigned i = 0; i < ind.size(); ++i)
      {
	float p = ind.data(i);
	v += p * p * p * p;
      }
    this->_value = -v;
  }
};

BOOST_AUTO_TEST_CASE(test_mpi)
{
  dbg::out(dbg::info)<<"running test_mpi ..."<<std::endl;

  typedef gen::EvoFloat<10, Params> gen_t;
  typedef phen::Parameters<gen_t, FitTest<Params>, Params> phen_t;
  typedef eval::Mpi<Params> eval_t;
  typedef boost::fusion::vector<stat::BestFit<phen_t, Params>, stat::MeanFit<Params> >  stat_t;
  typedef modif::Dummy<> modifier_t;
  typedef ea::RankSimple<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
  ea_t ea;
  ea.run();

  std::cout<<"==> best fitness ="<<ea.stat<0>().best()->fit().value()<<std::endl;
  std::cout<<"==> mean fitness ="<<ea.stat<1>().mean()<<std::endl; 
}
 
#else
BOOST_AUTO_TEST_CASE(test_mpi)
{

  std::cout<<"MPI is disabled"<<std::endl;
}
#endif
 
