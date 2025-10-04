#include "movie.h"
#include "util.h"
#include <sstream>
#include <iomanip>

Movie::Movie(const std::string& category, const std::string& name, double price, int qty, const std::string& genre, const std::string& rating)
    : Product(category, name, price, qty), genre_(genre), rating_(rating)
{}

Movie::~Movie(){}

std::set<std::string>Movie::keywords() const{
  std::set<std::string> keys=parseStringToWords(name_);
  keys.insert(convToLower(genre_));
  return keys;

}

std::string Movie::displayString()const{
  std::ostringstream oss;
  oss<<name_<<"\n";
  oss << "Genre: " << genre_ << " Rating: " << rating_ << "\n";
  oss<<std::fixed<<std::setprecision(2)<<price_<<" "<<qty_<<" left.";
  return oss.str();
}

void Movie::dump(std::ostream& os) const {
  Product::dump(os);
  os << genre_ << "\n" << rating_ << "\n";
}