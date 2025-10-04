#ifndef MYDATASTORE_H
#define MYDATASTORE_H

#include "datastore.h"
#include "product.h"
#include "user.h"
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>

class MyDataStore : public DataStore {
public:
  MyDataStore();
  ~MyDataStore();

  void addProduct(Product* p) override;
  void addUser(User* u) override;
  std::vector<Product*> search(std::vector<std::string>& term, int type) override;
  void dump(std::ostream& ofile) override;

  bool addToCart(const std::string& username, Product* p);
  void viewCart(const std::string& username) const;
  void buyCart(const std::string& username);

private:
  std::vector<Product*> products_;
  std::map<std::string, User*> users_;
  std::map<std::string, std::set<Product*>> keywordMap_;
  std::map<std::string, std::vector<Product*>> carts_;
};

#endif