#include "movie.h"
#include "movie_factory.h"
#include <iostream>
#include <sstream>

// Initialize static members - auto-registers movie types at startup
bool Comedy::registered = Comedy::registerSelf();
bool Drama::registered = Drama::registerSelf();
bool Classic::registered = Classic::registerSelf();

// Base Movie Implementation

/**
 * Constructor for base Movie class - initializes stock count, borrowed count (starts at 0), director, and title
 */
Movie::Movie(int stock, const std::string &director, const std::string &title)
    : stock(stock), borrowed(0), director(director), title(title) {}

/**
 * Attempts to borrow a movie by incrementing borrowed count if copies are available
 * Returns true if successful, false if all copies are already borrowed
 */
bool Movie::borrowMovie() {
  if (stock > borrowed) {
    borrowed++;
    return true;
  }
  return false;
}

/**
 * Returns a borrowed movie by decrementing the borrowed count if any copies are currently out
 * Returns true if successful, false if no copies were borrowed
 */
bool Movie::returnMovie() {
  if (borrowed > 0) {
    borrowed--;
    return true;
  }
  return false;
}

// Comedy Implementation

/**
 * Constructor for Comedy - extends Movie with a release year for comedy-specific sorting and identification
 */
Comedy::Comedy(int stock, const std::string &director, const std::string &title,
               int year)
    : Movie(stock, director, title), year(year) {}

/**
 * Comparison operator for Comedy sorting - comedies are sorted first by title, then by year
 * Different genres are sorted by their genre character for consistent ordering
 */
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

/**
 * Equality operator for Comedy - two comedies are equal if they have the same title and year
 * Used for finding specific movies during borrow/return operations
 */
bool Comedy::operator==(const Movie &other) const {
  const Comedy *otherComedy = dynamic_cast<const Comedy *>(&other);
  if (otherComedy == nullptr) {
    return false;
  }

  return title == otherComedy->title && year == otherComedy->year;
}

/**
 * Converts Comedy to string format showing genre, title, year, director, and stock information
 * Format: "Comedy: Title (Year) Dir: Director Stock: Available Out: Borrowed"
 */
std::string Comedy::toString() const {
  return "Comedy: " + title + " (" + std::to_string(year) +
         ") Dir: " + director + " Stock: " + std::to_string(stock - borrowed) +
         " Out: " + std::to_string(borrowed);
}

/**
 * Creates a deep copy of this Comedy object with the same properties
 * Used for prototype pattern implementation
 */
Movie *Comedy::clone() const {
  return new Comedy(stock, director, title, year);
}

/**
 * Factory method that creates a Comedy object from parsed data
 * Expects the extra parameter to contain the year as a string
 */
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

/**
 * Registers Comedy with the MovieFactory using 'F' as the genre identifier
 * Called automatically at program startup via static initialization
 */
bool Comedy::registerSelf() {
  return MovieFactory::getInstance().registerMovie('F', Comedy::create);
}

// Drama Implementation

/**
 * Constructor for Drama - extends Movie with a release year for drama-specific sorting and identification
 */
Drama::Drama(int stock, const std::string &director, const std::string &title,
             int year)
    : Movie(stock, director, title), year(year) {}

/**
 * Comparison operator for Drama sorting - dramas are sorted first by director, then by title
 * Different genres are sorted by their genre character for consistent ordering
 */
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

/**
 * Equality operator for Drama - two dramas are equal if they have the same director and title
 * Used for finding specific movies during borrow/return operations
 */
bool Drama::operator==(const Movie &other) const {
  const Drama *otherDrama = dynamic_cast<const Drama *>(&other);
  if (otherDrama == nullptr) {
    return false;
  }

  return director == otherDrama->director && title == otherDrama->title;
}

/**
 * Converts Drama to string format showing genre, director, title, year, and stock information
 * Format: "Drama: Director, Title (Year) Stock: Available Out: Borrowed"
 */
std::string Drama::toString() const {
  return "Drama: " + director + ", " + title + " (" + std::to_string(year) +
         ") Stock: " + std::to_string(stock - borrowed) +
         " Out: " + std::to_string(borrowed);
}

/**
 * Creates a deep copy of this Drama object with the same properties
 * Used for prototype pattern implementation
 */
Movie *Drama::clone() const { return new Drama(stock, director, title, year); }

/**
 * Factory method that creates a Drama object from parsed data
 * Expects the extra parameter to contain the year as a string
 */
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

/**
 * Registers Drama with the MovieFactory using 'D' as the genre identifier
 * Called automatically at program startup via static initialization
 */
bool Drama::registerSelf() {
  return MovieFactory::getInstance().registerMovie('D', Drama::create);
}

// Classic Implementation

/**
 * Constructor for Classic - extends Movie with actor, month, and year for classic-specific sorting and identification
 */
Classic::Classic(int stock, const std::string &director,
                 const std::string &title, const std::string &actor, int month,
                 int year)
    : Movie(stock, director, title), actor(actor), month(month), year(year) {}

/**
 * Comparison operator for Classic sorting - classics are sorted by month, then year, then actor
 * Different genres are sorted by their genre character for consistent ordering
 */
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

/**
 * Equality operator for Classic - two classics are equal if they have the same month, year, and actor
 * Used for finding specific movies during borrow/return operations
 */
bool Classic::operator==(const Movie &other) const {
  const Classic *otherClassic = dynamic_cast<const Classic *>(&other);
  if (otherClassic == nullptr) {
    return false;
  }

  return month == otherClassic->month && year == otherClassic->year &&
         actor == otherClassic->actor;
}

/**
 * Converts Classic to string format showing genre, release date, actor, title, director, and stock information
 * Format: "Classic: Month Year Actor - Title Dir: Director Stock: Available Out: Borrowed"
 */
std::string Classic::toString() const {
  return "Classic: " + std::to_string(month) + " " + std::to_string(year) +
         " " + actor + " - " + title + " Dir: " + director +
         " Stock: " + std::to_string(stock - borrowed) +
         " Out: " + std::to_string(borrowed);
}

/**
 * Creates a deep copy of this Classic object with the same properties
 * Used for prototype pattern implementation
 */
Movie *Classic::clone() const {
  return new Classic(stock, director, title, actor, month, year);
}

/**
 * Factory method that creates a Classic object from parsed data
 * Parses "actor,month,year" or "actor month year" format from the extra parameter
 */
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

/**
 * Registers Classic with the MovieFactory using 'C' as the genre identifier
 * Called automatically at program startup via static initialization
 */
bool Classic::registerSelf() {
  return MovieFactory::getInstance().registerMovie('C', Classic::create);
}

// MovieFactory Implementation

/**
 * Returns the singleton instance of MovieFactory using the Meyer's singleton pattern
 * Ensures only one factory exists throughout the program lifetime
 */
MovieFactory &MovieFactory::getInstance() {
  static MovieFactory instance;
  return instance;
}

/**
 * Registers a movie creation function with the factory, mapping a genre character to its creator function
 * Enables the factory to create movies of different types based on genre identifier
 */
bool MovieFactory::registerMovie(char genre, CreateFunction func) {
  creators[genre] = func;
  return true;
}

/**
 * Creates a movie object by determining the genre type and calling the appropriate creator function
 * Returns nullptr if the genre is not registered or creation fails
 */
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