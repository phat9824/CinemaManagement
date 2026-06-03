#include "Movie.hpp"
#include <iostream>

Movie::Movie(std::string id, std::string name, std::string genre, int duration, int ageRestriction, MovieStatus status)
    : id(std::move(id)), name(std::move(name)), genre(std::move(genre)), 
      duration(duration), ageRestriction(ageRestriction), status(status) {}

std::string Movie::getId() const { return id; }
std::string Movie::getName() const { return name; }
std::string Movie::getGenre() const { return genre; }
int Movie::getDuration() const { return duration; }
int Movie::getAgeRestriction() const { return ageRestriction; }
MovieStatus Movie::getStatus() const { return status; }

std::string Movie::getStatusString() const {
    return (status == MovieStatus::SHOWING) ? "Showing" : "Stopped";
}

void Movie::setStatus(MovieStatus newStatus) {
    status = newStatus;
}

void Movie::updateDetails(const std::string& newName, const std::string& newGenre, int newDuration, int newAgeRestriction) {
    name = newName;
    genre = newGenre;
    duration = newDuration;
    ageRestriction = newAgeRestriction;
}

void Movie::display() const {
    std::cout << "ID: " << id << " | Name: " << name 
              << " | Genre: " << genre << " | Duration: " << duration 
              << " mins | Age: " << ageRestriction << "+ | Status: " 
              << getStatusString() << "\n";
}