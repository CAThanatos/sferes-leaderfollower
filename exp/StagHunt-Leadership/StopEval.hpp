#ifndef STOP_EVAL_HPP
#define STOP_EVAL_HPP

#include "Stop.hpp"
#include "defparams.hpp"

namespace sferes
{
  namespace stat
  {
    SFERES_CLASS_D(StopEval, Stop)
    {
    public:
      template<typename EA>
			bool stop_criterion(const EA& ea) const
      {  
#ifndef NOSTOP
#ifdef STOPTRANSITION
#else
				for (size_t i = 0; i < ea.pop().size(); ++i)
					if (ea.eval().nb_eval() >= Params::pop::budget)
						return true;
#endif
#endif
	      return false;
      }  
    };
  }
}

#endif
