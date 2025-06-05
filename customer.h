#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <vector>
#include <memory>

class Movie;

// Transaction class to store customer history
class Transaction {
public:
    enum Type { BORROW, RETURN };
    
    Transaction(Type type, Movie* movie);
    ~Transaction() = default;
    
    Type getType() const { return type; }
    const Movie* getMovie() const { return movie; }
    std::string toString() const;
    
private:
    Type type;
    Movie* movie; // Pointer to the movie (not owned)
};

// Customer class
class Customer {
public:
    Customer(int id, const std::string& lastName, const std::string& firstName);
    ~Customer() = default;
    
    // Transaction management
    void addTransaction(Transaction::Type type, Movie* movie);
    void displayHistory() const;
    
    // Getters
    int getId() const { return id; }
    const std::string& getLastName() const { return lastName; }
    const std::string& getFirstName() const { return firstName; }
    std::string getFullName() const { return firstName + " " + lastName; }
    
private:
    int id;
    std::string lastName;
    std::string firstName;
    std::vector<Transaction> history;
};

#endif // CUSTOMER_H