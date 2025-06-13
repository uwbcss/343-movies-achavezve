#include "command.h"
#include "Store.h"
#include <iostream>
#include <sstream>

// Initialize static members
bool BorrowCommand::registered = BorrowCommand::registerSelf();
bool ReturnCommand::registered = ReturnCommand::registerSelf();
bool InventoryCommand::registered = InventoryCommand::registerSelf();
bool HistoryCommand::registered = HistoryCommand::registerSelf();

// BorrowCommand Implementation
BorrowCommand::BorrowCommand(int customerId, char mediaType, char movieType,
                             const std::string &movieInfo)
    : customerId(customerId), mediaType(mediaType), movieType(movieType),
      movieInfo(movieInfo) {}

bool BorrowCommand::execute(Store &store) {
  return store.borrowMovie(customerId, mediaType, movieType, movieInfo);
}

std::string BorrowCommand::toString() const {
  return "Borrow: Customer " + std::to_string(customerId) + " borrows " +
         movieInfo;
}

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

bool BorrowCommand::registerSelf() {
  return CommandFactory::getInstance().registerCommand('B',
                                                       BorrowCommand::create);
}

// ReturnCommand Implementation
ReturnCommand::ReturnCommand(int customerId, char mediaType, char movieType,
                             const std::string &movieInfo)
    : customerId(customerId), mediaType(mediaType), movieType(movieType),
      movieInfo(movieInfo) {}

bool ReturnCommand::execute(Store &store) {
  return store.returnMovie(customerId, mediaType, movieType, movieInfo);
}

std::string ReturnCommand::toString() const {
  return "Return: Customer " + std::to_string(customerId) + " returns " +
         movieInfo;
}

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

bool ReturnCommand::registerSelf() {
  return CommandFactory::getInstance().registerCommand('R',
                                                       ReturnCommand::create);
}

// InventoryCommand Implementation
bool InventoryCommand::execute(Store &store) {
  store.displayInventory();
  return true;
}

std::string InventoryCommand::toString() const { return "Display Inventory"; }

Command *InventoryCommand::create(const std::string & /*unused*/) {
  return new InventoryCommand();
}

bool InventoryCommand::registerSelf() {
  return CommandFactory::getInstance().registerCommand(
      'I', InventoryCommand::create);
}

// HistoryCommand Implementation
HistoryCommand::HistoryCommand(int customerId) : customerId(customerId) {}

bool HistoryCommand::execute(Store &store) {
  store.displayCustomerHistory(customerId);
  return true;
}

std::string HistoryCommand::toString() const {
  return "Display History for Customer " + std::to_string(customerId);
}

Command *HistoryCommand::create(const std::string &line) {
  std::istringstream iss(line);
  char cmd;
  int customerId;

  if (!(iss >> cmd >> customerId)) {
    return nullptr;
  }

  return new HistoryCommand(customerId);
}

bool HistoryCommand::registerSelf() {
  return CommandFactory::getInstance().registerCommand('H',
                                                       HistoryCommand::create);
}

// CommandFactory Implementation
CommandFactory &CommandFactory::getInstance() {
  static CommandFactory instance;
  return instance;
}

bool CommandFactory::registerCommand(char cmdType, CreateFunction func) {
  creators[cmdType] = func;
  return true;
}

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