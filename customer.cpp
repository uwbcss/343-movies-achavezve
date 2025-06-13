#include "customer.h"
#include "movie.h"
#include <iomanip>
#include <iostream>

// Transaction Implementation

/**
 * Constructor for Transaction - creates a transaction record linking a type
 * (borrow/return) with a specific movie
 */
Transaction::Transaction(Type type, Movie *movie) : type(type), movie(movie) {}

/**
 * Converts the transaction to a readable string format showing the action and
 * movie details Returns either "Borrowed [movie details]" or "Returned [movie
 * details]" or "[action] [Unknown Movie]" if movie is null
 */
std::string Transaction::toString() const {
  std::string action = (type == BORROW) ? "Borrowed" : "Returned";
  if (movie != nullptr) {
    return action + " " + movie->toString();
  }
  return action + " [Unknown Movie]";
}

// Customer Implementation

/**
 * Constructor for Customer - initializes customer with ID, last name, and first
 * name for identification
 */
Customer::Customer(int id, const std::string &lastName,
                   const std::string &firstName)
    : id(id), lastName(lastName), firstName(firstName) {}

/**
 * Adds a new transaction to the customer's history by creating and storing a
 * Transaction object Only adds the transaction if the movie pointer is valid
 * (not null)
 */
void Customer::addTransaction(Transaction::Type type, Movie *movie) {
  if (movie != nullptr) {
    history.emplace_back(type, movie);
  }
}

/**
 * Displays the complete transaction history for this customer in chronological
 * order Shows customer name, ID, and numbered list of all borrow/return
 * transactions, or "No transactions" if history is empty
 */
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