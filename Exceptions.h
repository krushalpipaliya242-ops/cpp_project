// Exceptions.h - custom exception hierarchy for the Photography Studio Booking system
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

// Base class for every application-specific error (caught in main loop).
class StudioException : public std::runtime_error {
public:
    explicit StudioException(const std::string& msg) : std::runtime_error(msg) {}
};

// Bad user input / bad field value (name, phone, date ...).
class ValidationException : public StudioException {
public:
    explicit ValidationException(const std::string& msg) : StudioException(msg) {}
};

// File could not be read / written.
class FileException : public StudioException {
public:
    explicit FileException(const std::string& msg) : StudioException(msg) {}
};

// A record with the requested ID does not exist.
class RecordNotFoundException : public StudioException {
public:
    explicit RecordNotFoundException(const std::string& msg) : StudioException(msg) {}
};

// Business-rule violation (double booking, studio closed, invalid state ...).
class BookingException : public StudioException {
public:
    explicit BookingException(const std::string& msg) : StudioException(msg) {}
};

// Input stream closed (Ctrl+Z / Ctrl+D / end of piped input) - not a StudioException on purpose.
class InputClosedException : public std::runtime_error {
public:
    InputClosedException() : std::runtime_error("Input stream closed") {}
};

#endif
