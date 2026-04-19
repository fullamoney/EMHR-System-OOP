#ifndef Patient_H
#define Patient_H

#include <iostream>
using namespace std;
#include <vector>

enum class AppointmentStatus {
    SCHEDULED,
    CANCELLED,
    COMPLETED,
    NO_SHOW
};

std::string statusToString(AppointmentStatus s) {
    switch (s) {
        case AppointmentStatus::SCHEDULED:  return "SCHEDULED";
        case AppointmentStatus::CANCELLED:  return "CANCELLED";
        case AppointmentStatus::COMPLETED:  return "COMPLETED";
        case AppointmentStatus::NO_SHOW:    return "NO_SHOW";
        default:                            return "UNKNOWN";
    }
}

// ─────────────────────────────────────────────
//  Patient Class
// ─────────────────────────────────────────────
class Patient {
private:
    static int nextId;
    int id;
    std::string name;
    std::string address;
    std::string contact;
    std::string nextOfKin;

public:
    Patient(const std::string& name,
            const std::string& address,
            const std::string& contact,
            const std::string& nextOfKin)
        : id(nextId++), name(name), address(address),
          contact(contact), nextOfKin(nextOfKin) {}

    // Getters
    int         getId()        const { return id; }
    std::string getName()      const { return name; }
    std::string getAddress()   const { return address; }
    std::string getContact()   const { return contact; }
    std::string getNextOfKin() const { return nextOfKin; }

    // Setters
    void setName(const std::string& n)      { name = n; }
    void setAddress(const std::string& a)   { address = a; }
    void setContact(const std::string& c)   { contact = c; }
    void setNextOfKin(const std::string& k) { nextOfKin = k; }

    void display() const {
        std::cout << "  Patient ID : " << id         << "\n"
                  << "  Name       : " << name       << "\n"
                  << "  Address    : " << address    << "\n"
                  << "  Contact    : " << contact    << "\n"
                  << "  Next of Kin: " << nextOfKin  << "\n";
    }
};
int Patient::nextId = 1;

#endif


