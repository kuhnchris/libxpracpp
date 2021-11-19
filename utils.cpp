/*https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/81886*/
#include <iostream>
#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>
#include <map>
#include <list>
#include <boost/any.hpp>
#include "utils.hpp"

void outputDataStructure(boost::any any, int level, bool prefix) {
  if (prefix){
    for (int i = 0; i < level; i++)
      std::cout << " ";
    std::cout << "* ";
  }
  if (is_any_a<std::list<boost::any>>(any)){
    std::cout << "(list)\n";
    std::list<boost::any> l = boost::any_cast<std::list<boost::any>>(any);
    for (auto ll: l){
      outputDataStructure(ll, level + 2);
      std::cout << "\n";
    }
  }
  else if (is_any_a<std::string>(any)){
    if (prefix)
      std::cout << "(string) ";
    std::cout << boost::any_cast<std::string>(any);
  }
  else if (is_any_a<std::map<std::string,boost::any>>(any)){
    std::cout << "(map: string, any)" << "\n";
    std::map<std::string,boost::any> l = boost::any_cast<std::map<std::string,boost::any>>(any);
    for (auto ll: l){
      for (int i = 0; i < level + 2; i++)
        std::cout << " "; 
      std::cout << "- [";
      outputDataStructure(ll.first, 0, false);
      std::cout << "]: ";
      outputDataStructure(ll.second, level + 2, false);
      std::cout << "\n";
    }
  } else if (is_any_a<unsigned int>(any)){
    std::cout << "(unsigned int) ";
    std::cout << boost::any_cast<unsigned int>(any);
  } else if (is_any_a<int>(any)){
    std::cout << "(int) ";
    std::cout << boost::any_cast<unsigned int>(any);
  } else if (is_any_a<long long>(any)){
    std::cout << "(long long) ";
    std::cout << boost::any_cast<long long>(any);
  } else if (is_any_a<bool>(any)){
    std::cout << "(bool) ";
    if (boost::any_cast<bool>(any))
      std::cout << "TRUE";
    else 
      std::cout << "FALSE";
  
  } else {
    
    std::cout << "??? (" << boost::typeindex::type_index(any.type()).pretty_name() << ")";
  }
} 
