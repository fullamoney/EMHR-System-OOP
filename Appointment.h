
#ifndef Appointment_H
#define Appointment_H


#include "Patient.h"



// ─────────────────────────────────────────────
//  Appointment Class
// ─────────────────────────────────────────────
class Appointment {
private:
    static int nextId;
    int id;
    int patientId;
    std::string dateTime;   // Format: "YYYY-MM-DD HH:MM"
    int durationMinutes;
    AppointmentStatus status;
    std::string notes;

public:
    Appointment(int patientId,
                const std::string& dateTime,
                int durationMinutes = 30,
                const std::string& notes = "")
        : id(nextId++), patientId(patientId), dateTime(dateTime),
          durationMinutes(durationMinutes), status(AppointmentStatus::SCHEDULED),
          notes(notes) {}

    int         getId()              const { return id; }
    int         getPatientId()       const { return patientId; }
    std::string getDateTime()        const { return dateTime; }
    int         getDuration()        const { return durationMinutes; }
    AppointmentStatus getStatus()    const { return status; }
    std::string getNotes()           const { return notes; }

    void setDateTime(const std::string& dt) { dateTime = dt; }
    void setStatus(AppointmentStatus s)     { status = s; }
    void setNotes(const std::string& n)     { notes = n; }

    // Returns start time in minutes-from-midnight for overlap checking
    int startMinutes() const {
        // dateTime format: "YYYY-MM-DD HH:MM"
        int hh = std::stoi(dateTime.substr(11, 2));
        int mm = std::stoi(dateTime.substr(14, 2));
        return hh * 60 + mm;
    }

    std::string getDate() const { return dateTime.substr(0, 10); }

    void display() const {
        std::cout << "  Appointment ID: " << id                          << "\n"
                  << "  Patient ID    : " << patientId                   << "\n"
                  << "  Date/Time     : " << dateTime                    << "\n"
                  << "  Duration      : " << durationMinutes << " mins"  << "\n"
                  << "  Status        : " << statusToString(status)      << "\n"
                  << "  Notes         : " << notes                       << "\n";
    }
};
int Appointment::nextId = 1;

#endif
