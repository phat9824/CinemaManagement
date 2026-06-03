#pragma once

#include <string>

enum class MovieStatus{
    SHOWING,
    STOPPED
};

class Movie {
private: 
    std::string id;
    std::string name;
    std::string genre;
    int duration; // in minutes
    int ageRestriction; // minimum age required to watch the movie
    MovieStatus status;

public:
    Movie(std::string id, std::string name, std::string genre, int duration, int ageRestriction, MovieStatus status = MovieStatus::SHOWING);

    //Getters
    std::string getId() const;
    std::string getName() const;
    std::string getGenre() const;
    int getDuration() const;
    int getAgeRestriction() const;
    MovieStatus getStatus() const;
    std::string getStatusString() const;

    //Setters
    void setStatus(MovieStatus newStatus);
    void updateDetails(const std::string& name, const std::string& genre, int duration, int ageRestriction);

    void display()  const;
};