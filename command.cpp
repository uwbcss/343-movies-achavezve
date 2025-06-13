#include "command.h"
#include "Store.h"
#include <iostream>
#include <sstream>

// Initialize static members - auto-registers commands at startup
bool BorrowCommand::registered = BorrowCommand::registerSelf();
bool ReturnCommand::registered = ReturnCommand::registerSelf();
bool InventoryCommand::registered = InventoryCommand::registerSelf();
bool HistoryCommand::registered = HistoryCommand::registerSelf();

// BorrowCommand Implementation

/**
 * Constructor for BorrowCommand - initializes all member variables needed to
 * execute a borrow operation
 */
BorrowCommand::BorrowCommand(int customerId, char mediaType, char movieType,
                             const std::string &movieInfo)
    : customerId(customerId), mediaType(mediaType), movieType(movieType),
      movieInfo(movieInfo) {}

/**
 * Executes the borrow command by calling the store's borrowMovie method with
 * stored parameters
 */
bool BorrowCommand::execute(Store &store) {
  return store.borrowMovie(customerId, mediaType, movieType, movieInfo);
}

/**
 * Returns a string representation of this borrow command for logging/debugging
 * purposes
 */
std::string BorrowCommand::toString() const {
  return "Borrow: Customer " + std::to_string(customerId) + " borrows " +
         movieInfo;
}

/**
 * Factory method that parses a command line string and creates a new
 * BorrowCommand object Expected format: "B customerId mediaType movieType
 * movieInfo"
 */
Command *BorrowCommand::create(const std::string &line) {
  std::istringstream iss(line);
  char cmd;
  int customerId;
  char mediaType;
  char movieType;

  if (!(iss >> cmd >> customerId >> mediaType >> movieType)) {
    return nullptr;
  }

  // Get the rest of the line as movie info
  std::string movieInfo;
  std::getline(iss, movieInfo);
  if (!movieInfo.empty() && movieInfo[0] == ' ') {
    movieInfo = movieInfo.substr(1); // Remove leading space
  }

  return new BorrowCommand(customerId, mediaType, movieType, movieInfo);
}

/**
 * Registers the BorrowCommand with the CommandFactory using 'B' as the command
 * identifier
 */
bool BorrowCommand::registerSelf() {
  return CommandFactory::getInstance().registerCommand('B',
                                                       BorrowCommand::create);
}

// ReturnCommand Implementation

/**
 * Constructor for ReturnCommand - initializes all member variables needed to
 * execute a return operation
 */
ReturnCommand::ReturnCommand(int customerId, char mediaType, char movieType,
                             const std::string &movieInfo)
    : customerId(customerId), mediaType(mediaType), movieType(movieType),
      movieInfo(movieInfo) {}

/**
 * Executes the return command by calling the store's returnMovie method with
 * stored parameters
 */
bool ReturnCommand::execute(Store &store) {
  return store.returnMovie(customerId, mediaType, movieType, movieInfo);
}

/**
 * Returns a string representation of this return command for logging/debugging
 * purposes
 */
std::string ReturnCommand::toString() const {
  return "Return: Customer " + std::to_string(customerId) + " returns " +
         movieInfo;
}

/**
 * Factory method that parses a command line string and creates a new
 * ReturnCommand object Expected format: "R customerId mediaType movieType
 * movieInfo"
 */
Command *ReturnCommand::create(const std::string &line) {
  std::istringstream iss(line);
  char cmd;
  int customerId;
  char mediaType;
  char movieType;

  if (!(iss >> cmd >> customerId >> mediaType >> movieType)) {
    return nullptr;
  }

  // Get the rest of the line as movie info
  std::string movieInfo;
  std::getline(iss, movieInfo);
  if (!movieInfo.empty() && movieInfo[0] == ' ') {
    movieInfo = movieInfo.substr(1); // Remove leading space
  }

  return new ReturnCommand(customerId, mediaType, movieType, movieInfo);
}

/**
 * Registers the ReturnCommand with the CommandFactory using 'R' as the command
 * identifier
 */
bool ReturnCommand::registerSelf() {
  return CommandFactory::getInstance().registerCommand('R',
                                                       ReturnCommand::create);
}

// InventoryCommand Implementation

/**
 * Executes the inventory command by calling the store's displayInventory method
 * to show all movies
 */
bool InventoryCommand::execute(Store &store) {
  store.displayInventory();
  return true;
}

/**
 * Returns a string representation of this inventory command for
 * logging/debugging purposes
 */
std::string InventoryCommand::toString() const { return "Display Inventory"; }

/**
 * Factory method that creates a new InventoryCommand object (no parsing needed
 * as it takes no parameters)
 */
Command *InventoryCommand::create(const std::string & /*unused*/) {
  return new InventoryCommand();
}

/**
 * Registers the InventoryCommand with the CommandFactory using 'I' as the
 * command identifier
 */
bool InventoryCommand::registerSelf() {
  return CommandFactory::getInstance().registerCommand(
      'I', InventoryCommand::create);
}

// HistoryCommand Implementation

/**
 * Constructor for HistoryCommand - initializes the customer ID whose history
 * will be displayed
 */
HistoryCommand::HistoryCommand(int customerId) : customerId(customerId) {}

/**
 * Executes the history command by calling the store's displayCustomerHistory
 * method for the specified customer
 */
bool HistoryCommand::execute(Store &store) {
  store.displayCustomerHistory(customerId);
  return true;
}

/**
 * Returns a string representation of this history command for logging/debugging
 * purposes
 */
std::string HistoryCommand::toString() const {
  return "Display History for Customer " + std::to_string(customerId);
}

/**
 * Factory method that parses a command line string and creates a new
 * HistoryCommand object Expected format: "H customerId"
 */
Command *HistoryCommand::create(const std::string &line) {
  std::istringstream iss(line);
  char cmd;
  int customerId;

  if (!(iss >> cmd >> customerId)) {
    return nullptr;
  }

  return new HistoryCommand(customerId);
}

/**
 * Registers the HistoryCommand with the CommandFactory using 'H' as the command
 * identifier
 */
bool HistoryCommand::registerSelf() {
  return CommandFactory::getInstance().registerCommand('H',
                                                       HistoryCommand::create);
}

// CommandFactory Implementation

/**
 * Returns the singleton instance of CommandFactory using the Meyer's singleton
 * pattern
 */
CommandFactory &CommandFactory::getInstance() {
  static CommandFactory instance;
  return instance;
}

/**
 * Registers a command creation function with the factory, mapping a command
 * type character to its creator function
 */
bool CommandFactory::registerCommand(char cmdType, CreateFunction func) {
  creators[cmdType] = func;
  return true;
}

/**
 * Creates a command object by parsing the input line, determining the command
 * type, and calling the appropriate creator function
 */
Command *CommandFactory::createCommand(const std::string &line) {
  if (line.empty()) {
    return nullptr;
  }

  char cmdType = line[0];
  auto it = creators.find(cmdType);

  if (it != creators.end()) {
    Command *cmd = it->second(line);
    if (cmd == nullptr) {
      std::cerr << "Error: Invalid command format: " << line << std::endl;
    }
    return cmd;
  }
  std::cerr << "Error: Unknown command type '" << cmdType << "'" << std::endl;
  return nullptr;
}