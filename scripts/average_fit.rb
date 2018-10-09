#| This file is a part of the sferes2 framework.
#| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
#| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
#|
#| This software is a computer program whose purpose is to facilitate
#| experiments in evolutionary computation and evolutionary robotics.
#| 
#| This software is governed by the CeCILL license under French law
#| and abiding by the rules of distribution of free software.  You
#| can use, modify and/ or redistribute the software under the terms
#| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
#| following URL "http://www.cecill.info".
#| 
#| As a counterpart to the access to the source code and rights to
#| copy, modify and redistribute granted by the license, users are
#| provided only with a limited warranty and the software's author,
#| the holder of the economic rights, and the successive licensors
#| have only limited liability.
#|
#| In this respect, the user's attention is drawn to the risks
#| associated with loading, using, modifying and/or developing or
#| reproducing the software by the user in light of its specific
#| status of free software, that may mean that it is complicated to
#| manipulate, and that also therefore means that it is reserved for
#| developers and experienced professionals having in-depth computer
#| knowledge. Users are therefore encouraged to load and test the
#| software's suitability as regards their requirements in conditions
#| enabling the security of their systems and/or data to be ensured
#| and, more generally, to use and operate it in the same conditions
#| as regards security.
#|
#| The fact that you are presently reading this means that you have
#| had knowledge of the CeCILL license and that you accept its terms.


#usage average_fit.rb gen_max *.dat > res.dat
data = Hash.new([])
gen = ARGV[0].to_i
ARGV.shift
ARGV.each{ |a|
 $stderr.puts a
 k = 0
  File.open(a).each { |i| 
    l = i.split(' ')
    data[l[0].to_i] += [l[1].to_f]
    k += 1
  }
  last = data[k - 1][data[k-1].length - 1]
   for i in (k..gen) do
     data[k] += [last]
     k += 1
   end

}


average = Hash[data.map { |k, v| [k, v.inject(0.0) { |sum, e| sum + e} / v.length] }]
median =  Hash[data.map { |k, v| [k, v.sort[v.length / 2] ] }]
std_dev = Hash[data.map { |k, v| [k, Math.sqrt(v.inject(0.0) { |sum, e| sum + (e - average[k]) * (e - average[k])} / v.length )] } ]

data.keys.sort.each { |k| print k.to_s + " " + average[k].to_s + " " + std_dev[k].to_s + " " + median[k].to_s + "\n" }

