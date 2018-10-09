#ifndef STAT_STOP_HPP
#define STAT_STOP_HPP

#include <sferes/stat/stat.hpp>
#include <sferes/stc.hpp>

namespace sferes
{
  namespace stat
  {
    SFERES_CLASS(Stop)
    {
    public:
      template<typename E>
			void refresh(const E& ea) const
      {
				if (stc::exact(this)->stop_criterion(ea))
				{
					std::cout << "Stop criterion reached" << std::endl;
//					ea.update_stats();
					ea.write();
					exit(0);
				}
      }

      void show(std::ostream& os, size_t k) {}

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version) {}

      template<typename EA>
      bool stop_criterion(const EA& ea) const 
      {
				assert(0);
				return false; 
      }
    };
  }
}
#endif
