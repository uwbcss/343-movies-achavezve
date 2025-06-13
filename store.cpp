#include "Store.h"
#include "command.h"
#include "customer.h"
#include "movie.h"
#include "movie_factory.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

/**
 * Constructor for Store - initializes empty containers for movies and customers
 * All data structures are initialized automatically by their default
 * constructors
 */
Store::Store() {
  // Constructor - containers are initialized automatically
}

/**
 * Loads movie data from a CSV file, parsing each line and creating Movie
 * objects via MovieFactory File format: genre,stock,director,title,extra_info
 * (where extra_info varies by genre) Returns true if file loads successfully,
 * false if file cannot be opened
 */
bool Store::loadMovies(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Cannot open " << filename << std::endl;
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    trimString(line);
    std::vector<std::string> parts = split(line, ',');

    if (parts.size() < 5) {
      std::cerr << "Error: Invalid movie format: " << line << std::endl;
      continue;
    }

    // Clean up each part
    for (auto &part : parts) {
      trimString(part);
    }

    char genre = parts[0][0];
    int stock;

    try {
      stock = std::stoi(parts[1]);
    } catch (...) {
      std::cerr << "Error: Invalid stock number in: " << line << std::endl;
      continue;
    }

    std::string director = parts[2];
    std::string title = parts[3];
    std::string extra = parts[4];

    // Add remaining parts to extra for classics (actor month year)
    for (size_t i = 5; i < parts.size(); i++) {
      extra += "," + parts[i];
    }

    Movie *movie = MovieFactory::getInstance().createMovie(
        genre, stock, director, title, extra);
    if (movie != nullptr) {
      movies.insert(std::unique_ptr<Movie>(movie));
    } else {
      std::cerr << "Error: Unknown movie genre '" << genre
                << "' in line: " << line << std::endl;
    }
  }

  return true;
}

/**
 * Loads customer data from a text file, creating Customer objects and storing
 * them in both hash table and vector File format: customerID lastName firstName
 * (space-separated) Returns true if file loads successfully, false if file
 * cannot be opened
 */
bool Store::loadCustomers(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Cannot open " << filename << std::endl;
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    std::istringstream iss(line);
    int id;
    std::string lastName;
    std::string firstName;

    if (iss >> id >> lastName >> firstName) {
      auto customer = std::make_unique<Customer>(id, lastName, firstName);
      customers.insert(id, customer.get());
      customerStorage.push_back(std::move(customer));
    } else {
      std::cerr << "Error parsing customer line: " << line << std::endl;
    }
  }
  return true;
}

/**
 * Processes command file line by line, creating Command objects via
 * CommandFactory and executing them Each command modifies store state
 * (borrow/return movies, display inventory/history) Returns true if file
 * processes successfully, false if file cannot be opened
 */
bool Store::processCommands(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Cannot open " << filename << std::endl;
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    Command *cmd = CommandFactory::getInstance().createCommand(line);
    if (cmd != nullptr) {
      cmd->execute(*this);
      delete cmd;
    }
  }
  return true;
}

/**
 * Searches for a specific movie by genre and search criteria using
 * genre-specific comparison logic Comedy: searches by title and year; Drama: by
 * director and title; Classic: by month, year, and actor Returns pointer to
 * found movie or nullptr if not found
 */
Movie *Store::findMovie(char genre, const std::string &searchCriteria) {
  std::string parsedCriteria = parseMovieSearchCriteria(genre, searchCriteria);

  for (const auto &moviePtr : movies) {
    if (moviePtr->getGenre() == genre) {
      // Create a temporary movie for comparison based on search criteria
      if (genre == 'F') { // Comedy: Title, Year
        std::vector<std::string> parts = split(parsedCriteria, ',');
        if (parts.size() >= 2) {
          std::string searchTitle = parts[0];
          trimString(searchTitle);
          int searchYear;
          try {
            searchYear = std::stoi(parts[1]);
          } catch (...) {
            continue;
          }

          const Comedy *comedy = dynamic_cast<const Comedy *>(moviePtr.get());
          if ((comedy != nullptr) && comedy->getTitle() == searchTitle &&
              comedy->getYear() == searchYear) {
            return moviePtr.get();
          }
        }
      } else if (genre == 'D') { // Drama: Director, Title
        std::vector<std::string> parts = split(parsedCriteria, ',');
        if (parts.size() >= 2) {
          std::string searchDirector = parts[0];
          std::string searchTitle = parts[1];
          trimString(searchDirector);
          trimString(searchTitle);

          const Drama *drama = dynamic_cast<const Drama *>(moviePtr.get());
          if ((drama != nullptr) && drama->getDirector() == searchDirector &&
              drama->getTitle() == searchTitle) {
            return moviePtr.get();
          }
        }
      } else if (genre == 'C') { // Classic: Month Year Actor
        std::vector<std::string> parts = split(parsedCriteria, ' ');
        if (parts.size() >= 4) {
          int searchMonth;
          int searchYear;
          try {
            searchMonth = std::stoi(parts[0]);
            searchYear = std::stoi(parts[1]);
          } catch (...) {
            continue;
          }

          std::string searchActor = parts[2] + " " + parts[3];

          const Classic *classic =
              dynamic_cast<const Classic *>(moviePtr.get());
          if ((classic != nullptr) && classic->getMonth() == searchMonth &&
              classic->getYear() == searchYear &&
              classic->getActor() == searchActor) {
            return moviePtr.get();
          }
        }
      }
    }
  }
  return nullptr;
}

/**
 * Searches for a customer by ID using the custom hash table implementation
 * Returns pointer to customer if found, nullptr if customer ID doesn't exist
 */
Customer *Store::findCustomer(int customerId) {
  Customer *customer = nullptr;
  if (customers.find(customerId, customer)) {
    return customer;
  }
  return nullptr;
}

/**
 * Processes a movie borrow request by validating customer, finding movie,
 * checking availability, and updating records Validates media type (must be 'D'
 * for DVD), finds customer and movie, decrements stock, adds transaction to
 * history Returns true if borrow succeeds, false if any validation fails or
 * movie is out of stock
 */
bool Store::borrowMovie(int customerId, char mediaType, char movieType,
                        const std::string &movieInfo) {
  if (mediaType != 'D') {
    std::cerr << "Error: Invalid media type " << mediaType << std::endl;
    return false;
  }

  Customer *customer = findCustomer(customerId);
  if (customer == nullptr) {
    std::cerr << "Error: Customer " << customerId << " not found" << std::endl;
    return false;
  }

  Movie *movie = findMovie(movieType, movieInfo);
  if (movie == nullptr) {
    std::cerr << "Error: Movie not found: " << movieInfo << std::endl;
    return false;
  }

  if (!movie->borrowMovie()) {
    std::cerr << "Error: Movie out of stock: " << movie->getTitle()
              << std::endl;
    return false;
  }

  customer->addTransaction(Transaction::BORROW, movie);
  return true;
}

/**
 * Processes a movie return request by validating customer, finding movie,
 * updating stock, and recording transaction Validates media type (must be 'D'
 * for DVD), finds customer and movie, increments available stock, adds
 * transaction to history Returns true if return succeeds, false if any
 * validation fails
 */
bool Store::returnMovie(int customerId, char mediaType, char movieType,
                        const std::string &movieInfo) {
  if (mediaType != 'D') {
    std::cerr << "Error: Invalid media type " << mediaType << std::endl;
    return false;
  }

  Customer *customer = findCustomer(customerId);
  if (customer == nullptr) {
    std::cerr << "Error: Customer " << customerId << " not found" << std::endl;
    return false;
  }

  Movie *movie = findMovie(movieType, movieInfo);
  if (movie == nullptr) {
    std::cerr << "Error: Movie not found: " << movieInfo << std::endl;
    return false;
  }

  movie->returnMovie();
  customer->addTransaction(Transaction::RETURN, movie);
  return true;
}

/**
 * Displays complete movie inventory sorted automatically by the MovieComparator
 * in the set container Shows all movies with their genre-specific sorting
 * (Comedy: title/year, Drama: director/title, Classic: date/actor)
 */
void Store::displayInventory() {
  std::cout << "INVENTORY:" << std::endl;

  // Movies are automatically sorted by the MovieComparator in the set
  for (const auto &movie : movies) {
    std::cout << movie->toString() << std::endl;
  }
  std::cout << std::endl;
}

/**
 * Displays transaction history for a specific customer by finding the customer
 * and calling their displayHistory method Shows chronological list of all
 * borrow/return transactions for the specified customer ID
 */
void Store::displayCustomerHistory(int customerId) {
  Customer *customer = findCustomer(customerId);
  if (customer == nullptr) {
    std::cerr << "Error: Customer " << customerId << " not found" << std::endl;
    return;
  }

  customer->displayHistory();
}

/**
 * Preprocesses movie search criteria by trimming whitespace (currently minimal
 * processing) Can be extended to handle more complex parsing logic for
 * different search formats
 */
std::string Store::parseMovieSearchCriteria(char /* genre */,
                                            const std::string &info) {
  std::string result = info;
  trimString(result);
  return result;
}

/**
 * Utility function that removes leading and trailing whitespace from a string
 * using STL algorithms Uses lambda functions with std::find_if to locate first
 * and last non-whitespace characters
 */
void Store::trimString(std::string &str) {
  // Remove leading whitespace
  str.erase(str.begin(),
            std::find_if(str.begin(), str.end(), [](unsigned char ch) {
              return std::isspace(ch) == 0;
            }));

  // Remove trailing whitespace
  str.erase(std::find_if(str.rbegin(), str.rend(),
                         [](unsigned char ch) { return std::isspace(ch) == 0; })
                .base(),
            str.end());
}

/**
 * Utility function that splits a string into tokens using a specified delimiter
 * character Returns vector of string tokens, useful for parsing CSV lines and
 * command parameters
 */
std::vector<std::string> Store::split(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;

  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}