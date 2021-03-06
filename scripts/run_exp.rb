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


#!/usr/bin/ruby
require "rexml/document"
require "yaml"


files = []
conf = YAML::load(File.open(ARGV[0]))

print "run exp, sure ?"
sure = STDIN.getc

n = 0
ml = conf["machines"]
pwd = `pwd`
pwd = pwd[0..pwd.size-2]

ld_lib_path=ENV['LD_LIBRARY_PATH']
conf["nb_trials"].to_i.times { |i|
  d = "exp-" + i.to_s
  begin
    Dir.mkdir(d)
  rescue Errno::EEXIST
    puts "  /!\\ dir not created"
  end
  m = ml[n % ml.size]
  str = "ssh -o CheckHostIP=false -f #{m} 'export LD_LIBRARY_PATH=#{ld_lib_path} && cd #{pwd}/#{d} && #{pwd}/#{conf["command"]} 1> #{d}.log 2> #{d}.err &'"
  if (not conf["simulate"] or conf["simulate"] == 1) then
    puts "[SIMULATION]  "  + str
  else
    system(str)
  end
  n += 1
}
