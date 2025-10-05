#include "mydatastore.h"
#include "util.h"
#include <iomanip>

MyDataStore::MyDataStore() {}

MyDataStore::~MyDataStore() {
  for(std::vector<Product*>::iterator it = products_.begin(); it != products_.end(); ++it) {
      delete *it;
  }
  for (std::map<std::string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
      delete it->second;
  }
}

void MyDataStore::addProduct(Product* p){
  products_.push_back(p);
  std::set<std::string>keys = p->keywords();
  for(std::set<std::string>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    keywordMap_[convToLower(*it)].insert(p);
  }
}

void MyDataStore::addUser(User* u){
  std::string uname = convToLower(u->getName());
  users_[uname] = u;
  if (carts_.find(uname) == carts_.end()) {
    carts_[uname] = std::vector<Product*>();
  }
}

std::vector<Product*> MyDataStore::search(std::vector<std::string>& terms, int type){
  std::vector<Product*> results;
  if(terms.empty()) return results;

  std::vector<std::string> lowerTerms;
  for (std::vector<std::string>::iterator it = terms.begin(); it != terms.end(); ++it) {
    lowerTerms.push_back(convToLower(*it));
  } 
  
  std::set<Product*> accum;
  bool first = true;
  for(std::vector<std::string>::iterator termIt = lowerTerms.begin(); termIt != lowerTerms.end(); ++termIt) {
    std::map<std::string, std::set<Product*>>::iterator mapIt = keywordMap_.find(*termIt);
    std::set<Product*> termSet;
    if(mapIt != keywordMap_.end()) termSet = mapIt->second;
    if(first){
      accum = termSet;
      first = false;
    }else{
      if(type ==0){
        accum = setIntersection(accum, termSet);
      }else{
        accum = setUnion(accum, termSet);
      }
    }
  }
  for (std::set<Product*>::iterator it = accum.begin(); it != accum.end(); ++it) {
      results.push_back(*it);
  }  
  return results;

}


void MyDataStore::dump(std::ostream& ofile) {
ofile<< "<products>\n";
for (Product* p : products_) {
      p->dump(ofile);
  }
  ofile << "</products>\n";
  ofile << "<users>\n";
  for (std::map<std::string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
    it->second->dump(ofile);
  }
  ofile << "</users>\n";
}

bool MyDataStore::addToCart(const std::string& username, Product* p) {
  std::string lname = convToLower(username);
  std::map<std::string, User*>::iterator it = users_.find(lname);
  if (it == users_.end()) {
      return false; // invalid user
  }
  if (carts_.find(lname) == carts_.end()) {
    carts_[lname] = std::vector<Product*>();
  }
  carts_[lname].push_back(p);
  return true;
}

void MyDataStore::viewCart(const std::string& username) const {
    std::string lname = convToLower(username);
    std::map<std::string, User*>::const_iterator uit = users_.find(lname);
    if (uit == users_.end()) {
        std::cout << "Invalid username" << std::endl;
        return;
    }
    
    std::map<std::string, std::vector<Product*>>::const_iterator cit = carts_.find(lname);
    if (cit == carts_.end() || cit->second.empty()) {
        std::cout << "Cart is empty" << std::endl;
        return;
    }
    
    const std::vector<Product*>& cart = cit->second;
    for (size_t i = 0; i < cart.size(); ++i) {
        std::cout << "Item " << (i+1) << std::endl;
        std::cout << cart[i]->displayString() << std::endl;
        std::cout << std::endl; 
    }
}


void MyDataStore::buyCart(const std::string& username){
  std::string lname = convToLower(username);
  std::map<std::string, User*>::iterator uit = users_.find(lname);
  if(uit==users_.end()){
    std::cout << "Invalid username"<<std::endl;
    return;
  }
  User* user = uit->second;
  std::vector<Product*>& cart = carts_[lname];
  size_t i = 0;
  while(i<cart.size()){
    Product* p = cart[i];
    if(p->getQty() > 0 && user->getBalance() >= p->getPrice()){
      p->subtractQty(1);
      user->deductAmount(p->getPrice());
      cart.erase(cart.begin()+i);
    }else{
      ++i;
    }
  }
}
