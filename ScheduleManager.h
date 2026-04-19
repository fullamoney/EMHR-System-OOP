
#ifndef  ScheduleManager_H
#define  ScheduleManager_H

#include "PatientRegistry.h"

// ─────────────────────────────────────────────
//  ScheduleManager Class
// ─────────────────────────────────────────────
class ScheduleManager {
private:
    static const int BUFFER_MINUTES = 15;
    std::vector<Appointment> appointments;

    // Returns true if two appointments overlap (including 15-min buffer)
    bool overlaps(const Appointment& existing, const Appointment& incoming) const {
        if (existing.getStatus() == AppointmentStatus::CANCELLED)
            return false;
        if (existing.getDate() != incoming.getDate())
            return false;

        int existStart = existing.startMinutes();
        int existEnd   = existStart + existing.getDuration() + BUFFER_MINUTES;
        int newStart   = incoming.startMinutes();
        int newEnd     = newStart + incoming.getDuration() + BUFFER_MINUTES;

        return !(newEnd <= existStart || newStart >= existEnd);
    }

public:
    // Book a new appointment
    bool bookAppointment(int patientId,
                         const std::string& dateTime,
                         int durationMinutes = 30,
                         const std::string& notes = "") {
        Appointment newAppt(patientId, dateTime, durationMinutes, notes);

        for (const auto& existing : appointments) {
            if (overlaps(existing, newAppt)) {
                std::cout << "[CONFLICT] Overlaps with appointment ID "
                          << existing.getId() << " at " << existing.getDateTime()
                          << " (including 15-min buffer).\n";
                return false;
            }
        }
        appointments.push_back(newAppt);
        std::cout << "[SUCCESS] Appointment booked (ID: "
                  << appointments.back().getId() << ").\n";
        return true;
    }

    // Update appointment time
    bool updateAppointment(int apptId, const std::string& newDateTime) {
        for (auto& appt : appointments) {
            if (appt.getId() == apptId) {
                std::string oldDateTime = appt.getDateTime();
                appt.setDateTime(newDateTime);

                // Check for conflicts with the updated time
                for (const auto& other : appointments) {
                    if (other.getId() == apptId) continue;
                    if (overlaps(other, appt)) {
                        appt.setDateTime(oldDateTime); // rollback
                        std::cout << "[CONFLICT] Update failed – overlaps with appointment ID "
                                  << other.getId() << ".\n";
                        return false;
                    }
                }
                std::cout << "[SUCCESS] Appointment " << apptId << " updated.\n";
                return true;
            }
        }
        std::cout << "[ERROR] Appointment ID " << apptId << " not found.\n";
        return false;
    }

    // Cancel appointment
    bool cancelAppointment(int apptId) {
        for (auto& appt : appointments) {
            if (appt.getId() == apptId) {
                appt.setStatus(AppointmentStatus::CANCELLED);
                std::cout << "[SUCCESS] Appointment " << apptId << " cancelled.\n";
                return true;
            }
        }
        std::cout << "[ERROR] Appointment ID " << apptId << " not found.\n";
        return false;
    }

    // View all appointments for a patient
    void viewByPatient(int patientId) const {
        bool found = false;
        for (const auto& appt : appointments) {
            if (appt.getPatientId() == patientId) {
                appt.display();
                std::cout << "  ---\n";
                found = true;
            }
        }
        if (!found)
            std::cout << "  No appointments found for patient ID " << patientId << ".\n";
    }

    // View all appointments
    void viewAll() const {
        if (appointments.empty()) {
            std::cout << "  No appointments scheduled.\n";
            return;
        }
        for (const auto& appt : appointments) {
            appt.display();
            std::cout << "  ---\n";
        }
    }
};

#endif

