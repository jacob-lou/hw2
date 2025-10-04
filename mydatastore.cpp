#include "mydatastore.h"
#include "util.h"
#include <algorithm>
#include <iomanip>

MyDataStore::MyDataStore() {}

MyDataStore::~MyDataStore() {
  for(auto p:products_) delete p;
  for (auto& kv : users_) delete kv.second;
}

void MyDataStore::addProduct(Product* p){
  products_.push_back(p);
  std::set<std::string>keys = p->keywords();
  for(auto const& k : keys){
    keywordMap_[convToLower(k)].insert(p);
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
  for (auto &t : terms) {
    lowerTerms.push_back(convToLower(t));
  } 
  std::set<Product*> accum;
  bool first = true;
  for(const auto &term: lowerTerms){
    auto it = keywordMap_.find(term);
    std::set<Product*> termSet;
    if(it != keywordMap_.end()) termSet = it->second;
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
  for (Product* p : accum) results.push_back(p);
  return results;

}


void MyDataStore::dump(std::ostream& ofile) {
ofile<< "<products>\n";
for (Product* p : products_) {
      p->dump(ofile);
  }
  ofile << "</products>\n";
  ofile << "<users>\n";
  for (auto& kv : users_) {
      kv.second->dump(ofile);
  }
  ofile << "</users>\n";
}

bool MyDataStore::addToCart(const std::string& username, Product* p) {
  std::string lname = convToLower(username);
  auto it = users_.find(lname);
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
    auto uit = users_.find(lname);
    if (uit == users_.end()) {
        std::cout << "Invalid username" << std::endl;
        return;
    }
    
    auto cit = carts_.find(lname);
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
  auto uit = users_.find(lname);
  if(uit==users_.end()){
    std::cout << "Invalid username"<<std::endl;
    return;
  }
  User* user = uit->second;
  auto &cart = carts_[lname];
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
