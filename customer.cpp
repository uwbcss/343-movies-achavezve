#include "customer.h"
#include "movie.h"
#include <iomanip>
#include <iostream>

// Transaction Implementation
Transaction::Transaction(Type type, Movie *movie) : type(type), movie(movie) {}

std::string Transaction::toString() const {
  std::string action = (type == BORROW) ? "Borrowed" : "Returned";
  if (movie != nullptr) {
    return action + " " + movie->toString();
  }
  return action + " [Unknown Movie]";
}

// Customer Implementation
Customer::Customer(int id, const std::string &lastName,
                   const std::string &firstName)
    : id(id), lastName(lastName), firstName(firstName) {}

void Customer::addTransaction(Transaction::Type type, Movie *movie) {
  if (movie != nullptr) {
    history.emplace_back(type, movie);
  }
}

void Customer::displayHistory() const {
  std::cout << "Transaction history for " << getFullName() << " (ID: " << id
            << "):" << std::endl;

  if (history.empty()) {
    std::cout << "No transactions found." << std::endl;
    return;
  }

  // Display transactions in chronological order (first transaction first)
  for (size_t i = 0; i < history.size(); ++i) {
    std::cout << std::setw(3) << (i + 1) << ". " << history[i].toString()
              << std::endl;
  }
  std::cout << std::endl; // Add blank line after history
}