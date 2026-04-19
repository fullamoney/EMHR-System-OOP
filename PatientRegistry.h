
#ifndef  PatientRegistry_H
#define  PatientRegistry_H



#include "Appointment.h"


// ─────────────────────────────────────────────
//  PatientRegistry Class
// ─────────────────────────────────────────────
class PatientRegistry {
private:
    std::vector<Patient> patients;

    int findIndex(int id) const {
        for (int i = 0; i < (int)patients.size(); i++)
            if (patients[i].getId() == id) return i;
        return -1;
    }

public:
    // CREATE
    int createPatient(const std::string& name,
                      const std::string& address,
                      const std::string& contact,
                      const std::string& nextOfKin) {
        patients.emplace_back(name, address, contact, nextOfKin);
        int newId = patients.back().getId();
        std::cout << "[SUCCESS] Patient created (ID: " << newId << ").\n";
        return newId;
    }

    // READ
    Patient* readPatient(int id) {
        int idx = findIndex(id);
        if (idx == -1) {
            std::cout << "[ERROR] Patient ID " << id << " not found.\n";
            return nullptr;
        }
        return &patients[idx];
    }

    // UPDATE
    bool updatePatient(int id,
                       const std::string& name,
                       const std::string& address,
                       const std::string& contact,
                       const std::string& nextOfKin) {
        int idx = findIndex(id);
        if (idx == -1) {
            std::cout << "[ERROR] Patient ID " << id << " not found.\n";
            return false;
        }
        patients[idx].setName(name);
        patients[idx].setAddress(address);
        patients[idx].setContact(contact);
        patients[idx].setNextOfKin(nextOfKin);
        std::cout << "[SUCCESS] Patient " << id << " updated.\n";
        return true;
    }

    // DELETE
    bool deletePatient(int id) {
        int idx = findIndex(id);
        if (idx == -1) {
            std::cout << "[ERROR] Patient ID " << id << " not found.\n";
            return false;
        }
        patients.erase(patients.begin() + idx);
        std::cout << "[SUCCESS] Patient " << id << " deleted.\n";
        return true;
    }

    // View all patients
    void viewAll() const {
        if (patients.empty()) {
            std::cout << "  No patients registered.\n";
            return;
        }
        for (const auto& p : patients) {
            p.display();
            std::cout << "  ---\n";
        }
    }
};

#endif