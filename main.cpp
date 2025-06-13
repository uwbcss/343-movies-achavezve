#include "Store.h"
#include <iostream>

// Forward declaration for the test function from store_test.cpp
void testAll();

// This function runs the main movie store simulation.
void runSimulation() {
  Store store;
  std::cout << "--- Starting Movie Store Simulation ---" << std::endl;

  std::cout << "\nLoading customers from file..." << std::endl;
  store.loadCustomers("data4customers.txt");

  std::cout << "Loading movies from file..." << std::endl;
  store.loadMovies("data4movies.txt");

  std::cout << "Processing commands from file..." << std::endl;
  store.processCommands("data4commands.txt");

  std::cout << "\n--- Simulation Complete ---" << std::endl;
}

int main() {
  // Run the main simulation logic first.
  runSimulation();

  // Add a separator for clarity before running the original tests.
  std::cout << "\n\n--- Starting Original Tests ---" << std::endl;

  // Now, call the original test function.
  testAll();

  std::cout << "\nDone." << std::endl;
  return 0;
}