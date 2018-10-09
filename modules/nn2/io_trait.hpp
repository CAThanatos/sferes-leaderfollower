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




#ifndef _IO_TRAIT_HPP_
#define _IO_TRAIT_HPP_

// for std::pair
#include <map>
#include <valarray>

#ifdef EIGEN2_ENABLED
#include <Eigen/Core>
#endif

namespace nn
{
  // io trait
  template<typename T>
  struct io_trait
  {
    static T zero() { return T(0.0f); }
    static T zero(size_t k) { return zero(); }
    typedef std::valarray<T> vector_t;
  };

#ifdef EIGEN2_ENABLED
  // go with eigen with float (TODO : double)
  template<>
  struct io_trait<float>
  {
    typedef Eigen::VectorXf vector_t;
    static float zero() { return 0.0f; }
    static vector_t zero(size_t k) { return Eigen::VectorXf::Zero(k); }

  };
#endif

  template<>
  struct io_trait<std::pair<float, float> >
  {
    static std::pair<float, float> zero() { return std::make_pair(0.0f, 0.0f); }
    typedef std::valarray<std::pair<float, float> > vector_t;
  };

  // useful but wrong place (?)
  template<typename _CharT, typename _Traits>
  std::basic_ostream<_CharT, _Traits>&
  operator<<(std::basic_ostream<_CharT, _Traits>& ofs, const std::pair<float, float>& p)
  {
    return ofs<<p.first<<" "<<p.second;
  }

 template<typename _CharT, typename _Traits>
  std::basic_ostream<_CharT, _Traits>&
 operator<<(std::basic_ostream<_CharT, _Traits>& ofs, const std::vector<float>& p)
  {
    for (size_t i = 0; i < p.size(); ++i)
      ofs<<p[i]<<" ";
    return ofs;
  }


  template<typename T1, typename T2>
  std::istream& operator>>(std::istream& ifs, std::pair<T1, T2>& p)
  {
    T1 t1;
    T2 t2;
    ifs >> t1;
    ifs >> t2;
    return std::make_pair(t1, t2);
  }

  
}


#endif
