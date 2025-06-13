#include "Store.h"
#include "command.h"
#include "customer.h"
#include "movie.h"
#include "movie_factory.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

Store::Store() {
  // Constructor - containers are initialized automatically
}

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

Customer *Store::findCustomer(int customerId) {
  Customer *customer = nullptr;
  if (customers.find(customerId, customer)) {
    return customer;
  }
  return nullptr;
}

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

void Store::displayInventory() {
  std::cout << "INVENTORY:" << std::endl;

  // Movies are automatically sorted by the MovieComparator in the set
  for (const auto &movie : movies) {
    std::cout << movie->toString() << std::endl;
  }
  std::cout << std::endl;
}

void Store::displayCustomerHistory(int customerId) {
  Customer *customer = findCustomer(customerId);
  if (customer == nullptr) {
    std::cerr << "Error: Customer " << customerId << " not found" << std::endl;
    return;
  }

  customer->displayHistory();
}

std::string Store::parseMovieSearchCriteria(char /* genre */,
                                            const std::string &info) {
  std::string result = info;
  trimString(result);
  return result;
}

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

std::vector<std::string> Store::split(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;

  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}