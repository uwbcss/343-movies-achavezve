#include "movie.h"
#include "movie_factory.h"
#include <iostream>
#include <sstream>

// Initialize static members
bool Comedy::registered = Comedy::registerSelf();
bool Drama::registered = Drama::registerSelf();
bool Classic::registered = Classic::registerSelf();

// Base Movie Implementation
Movie::Movie(int stock, const std::string &director, const std::string &title)
    : stock(stock), borrowed(0), director(director), title(title) {}

bool Movie::borrowMovie() {
  if (stock > borrowed) {
    borrowed++;
    return true;
  }
  return false;
}

bool Movie::returnMovie() {
  if (borrowed > 0) {
    borrowed--;
    return true;
  }
  return false;
}

// Comedy Implementation
Comedy::Comedy(int stock, const std::string &director, const std::string &title,
               int year)
    : Movie(stock, director, title), year(year) {}

bool Comedy::operator<(const Movie &other) const {
  const Comedy *otherComedy = dynamic_cast<const Comedy *>(&other);
  if (otherComedy == nullptr) {
    return getGenre() < other.getGenre();
  }

  if (title != otherComedy->title) {
    return title < otherComedy->title;
  }
  return year < otherComedy->year;
}

bool Comedy::operator==(const Movie &other) const {
  const Comedy *otherComedy = dynamic_cast<const Comedy *>(&other);
  if (otherComedy == nullptr) {
    return false;
  }

  return title == otherComedy->title && year == otherComedy->year;
}

std::string Comedy::toString() const {
  return "Comedy: " + title + " (" + std::to_string(year) +
         ") Dir: " + director + " Stock: " + std::to_string(stock - borrowed) +
         " Out: " + std::to_string(borrowed);
}

Movie *Comedy::clone() const {
  return new Comedy(stock, director, title, year);
}

Movie *Comedy::create(int stock, const std::string &director,
                      const std::string &title, const std::string &extra) {
  try {
    int year = std::stoi(extra);
    return new Comedy(stock, director, title, year);
  } catch (...) {
    std::cerr << "Error: Invalid year for comedy: " << extra << std::endl;
    return nullptr;
  }
}

bool Comedy::registerSelf() {
  return MovieFactory::getInstance().registerMovie('F', Comedy::create);
}

// Drama Implementation
Drama::Drama(int stock, const std::string &director, const std::string &title,
             int year)
    : Movie(stock, director, title), year(year) {}

bool Drama::operator<(const Movie &other) const {
  const Drama *otherDrama = dynamic_cast<const Drama *>(&other);
  if (otherDrama == nullptr) {
    return getGenre() < other.getGenre();
  }

  if (director != otherDrama->director) {
    return director < otherDrama->director;
  }
  return title < otherDrama->title;
}

bool Drama::operator==(const Movie &other) const {
  const Drama *otherDrama = dynamic_cast<const Drama *>(&other);
  if (otherDrama == nullptr) {
    return false;
  }

  return director == otherDrama->director && title == otherDrama->title;
}

std::string Drama::toString() const {
  return "Drama: " + director + ", " + title + " (" + std::to_string(year) +
         ") Stock: " + std::to_string(stock - borrowed) +
         " Out: " + std::to_string(borrowed);
}

Movie *Drama::clone() const { return new Drama(stock, director, title, year); }

Movie *Drama::create(int stock, const std::string &director,
                     const std::string &title, const std::string &extra) {
  try {
    int year = std::stoi(extra);
    return new Drama(stock, director, title, year);
  } catch (...) {
    std::cerr << "Error: Invalid year for drama: " << extra << std::endl;
    return nullptr;
  }
}

bool Drama::registerSelf() {
  return MovieFactory::getInstance().registerMovie('D', Drama::create);
}

// Classic Implementation
Classic::Classic(int stock, const std::string &director,
                 const std::string &title, const std::string &actor, int month,
                 int year)
    : Movie(stock, director, title), actor(actor), month(month), year(year) {}

bool Classic::operator<(const Movie &other) const {
  const Classic *otherClassic = dynamic_cast<const Classic *>(&other);
  if (otherClassic == nullptr) {
    return getGenre() < other.getGenre();
  }

  if (month != otherClassic->month) {
    return month < otherClassic->month;
  }
  if (year != otherClassic->year) {
    return year < otherClassic->year;
  }
  return actor < otherClassic->actor;
}

bool Classic::operator==(const Movie &other) const {
  const Classic *otherClassic = dynamic_cast<const Classic *>(&other);
  if (otherClassic == nullptr) {
    return false;
  }

  return month == otherClassic->month && year == otherClassic->year &&
         actor == otherClassic->actor;
}

std::string Classic::toString() const {
  return "Classic: " + std::to_string(month) + " " + std::to_string(year) +
         " " + actor + " - " + title + " Dir: " + director +
         " Stock: " + std::to_string(stock - borrowed) +
         " Out: " + std::to_string(borrowed);
}

Movie *Classic::clone() const {
  return new Classic(stock, director, title, actor, month, year);
}

Movie *Classic::create(int stock, const std::string &director,
                       const std::string &title, const std::string &extra) {
  // Parse: "actor,month,year" or "actor month year"
  std::string parseStr = extra;

  // Replace commas with spaces for easier parsing
  for (char &c : parseStr) {
    if (c == ',') {
      c = ' ';
    }
  }

  std::istringstream iss(parseStr);
  std::string firstName;
  std::string lastName;
  int month;
  int year;

  if (!(iss >> firstName >> lastName >> month >> year)) {
    std::cerr << "Error: Invalid classic movie format: " << extra << std::endl;
    return nullptr;
  }

  std::string actor = firstName + " " + lastName;
  return new Classic(stock, director, title, actor, month, year);
}

bool Classic::registerSelf() {
  return MovieFactory::getInstance().registerMovie('C', Classic::create);
}

// MovieFactory Implementation
MovieFactory &MovieFactory::getInstance() {
  static MovieFactory instance;
  return instance;
}

bool MovieFactory::registerMovie(char genre, CreateFunction func) {
  creators[genre] = func;
  return true;
}

Movie *MovieFactory::createMovie(char genre, int stock,
                                 const std::string &director,
                                 const std::string &title,
                                 const std::string &extra) {
  auto it = creators.find(genre);
  if (it != creators.end()) {
    return it->second(stock, director, title, extra);
  }
  return nullptr;
}