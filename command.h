#ifndef COMMAND_H
#define COMMAND_H

#include <functional>
#include <map>
#include <sstream>
#include <string>

class Store; // Forward declaration

// Base Command class
class Command {
public:
  virtual ~Command() = default;
  virtual bool execute(Store &store) = 0;
  virtual std::string toString() const = 0;
};

// Borrow Command
class BorrowCommand : public Command {
public:
  BorrowCommand(int customerId, char mediaType, char movieType,
                const std::string &movieInfo);

  bool execute(Store &store) override;
  std::string toString() const override;

  // Static factory method with self-registration
  static Command *create(const std::string &line);
  static bool registerSelf();

private:
  int customerId;
  char mediaType;
  char movieType;
  std::string movieInfo;
  static bool registered;
};

// Return Command
class ReturnCommand : public Command {
public:
  ReturnCommand(int customerId, char mediaType, char movieType,
                const std::string &movieInfo);

  bool execute(Store &store) override;
  std::string toString() const override;

  // Static factory method with self-registration
  static Command *create(const std::string &line);
  static bool registerSelf();

private:
  int customerId;
  char mediaType;
  char movieType;
  std::string movieInfo;
  static bool registered;
};

// Inventory Command
class InventoryCommand : public Command {
public:
  InventoryCommand() = default;

  bool execute(Store &store) override;
  std::string toString() const override;

  // Static factory method with self-registration
  static Command *create(const std::string &line);
  static bool registerSelf();

private:
  static bool registered;
};

// History Command
class HistoryCommand : public Command {
public:
  explicit HistoryCommand(int customerId);

  bool execute(Store &store) override;
  std::string toString() const override;

  // Static factory method with self-registration
  static Command *create(const std::string &line);
  static bool registerSelf();

private:
  int customerId;
  static bool registered;
};

// Command Factory using self-registration pattern
class CommandFactory {
public:
  using CreateFunction = std::function<Command *(const std::string &)>;

  static CommandFactory &getInstance();

  bool registerCommand(char cmdType, CreateFunction func);
  Command *createCommand(const std::string &line);

private:
  std::map<char, CreateFunction> creators;
  CommandFactory() = default;
};

#endif // COMMAND_H