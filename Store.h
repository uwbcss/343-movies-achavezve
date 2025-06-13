#ifndef STORE_H
#define STORE_H

#include "command.h"
#include "customer.h"
#include "movie.h"
#include "movie_factory.h"
#include <fstream>
#include <memory>
#include <set>
#include <string>
#include <vector>

class Movie;
class Command;

// Custom comparator for movies to maintain sorted order
struct MovieComparator {
  bool operator()(const std::unique_ptr<Movie> &a,
                  const std::unique_ptr<Movie> &b) const {
    // First sort by genre (Comedy, Drama, Classics)
    if (a->getGenre() != b->getGenre()) {
      if (a->getGenre() == 'F') {
        return true;
      }
      if (b->getGenre() == 'F') {
        return false;
      }
      if (a->getGenre() == 'D') {
        return true;
      }
      if (b->getGenre() == 'D') {
        return false;
      }
      return false;
    }
    // Then by movie-specific sorting criteria
    return *a < *b;
  }
};

class Store {
public:
  Store();
  ~Store() = default;

  // File processing
  bool loadMovies(const std::string &filename);
  bool loadCustomers(const std::string &filename);
  bool processCommands(const std::string &filename);

  // Movie operations
  Movie *findMovie(char genre, const std::string &searchCriteria);
  bool borrowMovie(int customerId, char mediaType, char movieType,
                   const std::string &movieInfo);
  bool returnMovie(int customerId, char mediaType, char movieType,
                   const std::string &movieInfo);

  // Customer operations
  Customer *findCustomer(int customerId);

  // Display operations
  void displayInventory();
  void displayCustomerHistory(int customerId);

private:
  std::set<std::unique_ptr<Movie>, MovieComparator> movies;
  HashTable<int, Customer *> customers;
  std::vector<std::unique_ptr<Customer>> customerStorage;

  // Helper methods
  static std::string parseMovieSearchCriteria(char genre,
                                              const std::string &info);
  static void trimString(std::string &str);
  static std::vector<std::string> split(const std::string &str, char delimiter);
};

#endif // STORE_H