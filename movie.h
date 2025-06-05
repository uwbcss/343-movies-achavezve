#ifndef MOVIE_H
#define MOVIE_H

#include <string>
#include <iostream>

// Base Movie class
class Movie {
public:
    Movie(int stock, const std::string& director, const std::string& title);
    virtual ~Movie() = default;
    
    // Pure virtual functions for polymorphism
    virtual bool operator<(const Movie& other) const = 0;
    virtual bool operator==(const Movie& other) const = 0;
    virtual std::string toString() const = 0;
    virtual char getGenre() const = 0;
    virtual Movie* clone() const = 0;
    
    // Stock management
    bool borrowMovie();
    bool returnMovie();
    int getStock() const { return stock; }
    int getBorrowed() const { return borrowed; }
    
    // Getters
    const std::string& getDirector() const { return director; }
    const std::string& getTitle() const { return title; }

protected:
    int stock;
    int borrowed;
    std::string director;
    std::string title;
};

// Comedy Movie class
class Comedy : public Movie {
public:
    Comedy(int stock, const std::string& director, const std::string& title, int year);
    
    bool operator<(const Movie& other) const override;
    bool operator==(const Movie& other) const override;
    std::string toString() const override;
    char getGenre() const override { return 'F'; }
    Movie* clone() const override;
    
    int getYear() const { return year; }
    
    // Static factory method with self-registration
    static Movie* create(int stock, const std::string& director, 
                        const std::string& title, const std::string& extra);
    static bool registerSelf();

private:
    int year;
    static bool registered;
};

// Drama Movie class  
class Drama : public Movie {
public:
    Drama(int stock, const std::string& director, const std::string& title, int year);
    
    bool operator<(const Movie& other) const override;
    bool operator==(const Movie& other) const override;
    std::string toString() const override;
    char getGenre() const override { return 'D'; }
    Movie* clone() const override;
    
    int getYear() const { return year; }
    
    // Static factory method with self-registration
    static Movie* create(int stock, const std::string& director, 
                        const std::string& title, const std::string& extra);
    static bool registerSelf();

private:
    int year;
    static bool registered;
};

// Classic Movie class
class Classic : public Movie {
public:
    Classic(int stock, const std::string& director, const std::string& title, 
            const std::string& actor, int month, int year);
    
    bool operator<(const Movie& other) const override;
    bool operator==(const Movie& other) const override;
    std::string toString() const override;
    char getGenre() const override { return 'C'; }
    Movie* clone() const override;
    
    const std::string& getActor() const { return actor; }
    int getMonth() const { return month; }
    int getYear() const { return year; }
    
    // Static factory method with self-registration
    static Movie* create(int stock, const std::string& director, 
                        const std::string& title, const std::string& extra);
    static bool registerSelf();

private:
    std::string actor;
    int month;
    int year;
    static bool registered;
};

#endif // MOVIE_H