// ============================================================
// File: FileManager.h
// Description: File persistence for Patients, Appointments,
//              EHR records, and Billing (Repository pattern)
// Project: EMHR Healthcare System - Semester 2 AY2025/2026
// ============================================================

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

// ============================================================
// Simple data structs for persistence
// (Your existing classes likely have these — adapt as needed)
// ============================================================

struct PatientRecord {
    std::string patientId;
    std::string firstName;
    std::string lastName;
    std::string dateOfBirth;      // ISO-8601
    std::string gender;
    std::string address;
    std::string phone;
    std::string email;
    std::string nextOfKinName;
    std::string nextOfKinPhone;
    std::string createdAt;        // ISO-8601
};

struct AppointmentRecord {
    std::string appointmentId;
    std::string patientId;
    std::string staffId;
    std::string scheduledAt;      // ISO-8601
    std::string status;           // SCHEDULED|CANCELLED|COMPLETED|NO_SHOW
    std::string purpose;
    bool        conflictFlagged;
};

struct BillingRecord {
    std::string billingId;
    std::string patientId;
    std::string appointmentId;
    double      totalAmount;
    double      insuranceCoverage; // e.g. 80%
    double      deductible;
    double      patientPays;
    std::string paymentMethod;     // CASH|CARD|INSURANCE|COMBO
    std::string status;            // PENDING|PAID|PARTIAL
    std::string billedAt;          // ISO-8601
};

struct EHRRecord {
    std::string ehrId;
    std::string patientId;
    std::string visitDate;         // ISO-8601
    std::string diagnosis;
    std::string prescription;
    std::string allergies;
    std::string immunizations;
    std::string labResultsSummary;
    std::string purposeOfProcessing;
    std::string recordedBy;        // staffId
};

// ============================================================
// FileManager — Repository pattern
// Handles all file I/O for the EMHR system
// ABSTRACTION: hides file format details from business logic
// ENCAPSULATION: all file names are private constants
// ============================================================
class FileManager {
private:
    // Private file name constants (encapsulated)
    const std::string PATIENTS_FILE     = "patients.txt";
    const std::string APPOINTMENTS_FILE = "appointments.txt";
    const std::string BILLING_FILE      = "billing.txt";
    const std::string EHR_FILE          = "ehr_records.txt";
    const std::string AUDIT_FILE        = "audit_log.txt";

    // Delimiter used in flat files
    const char DELIM = '|';

    // Helper: escape pipes in strings to avoid parsing issues
    std::string escape(const std::string& s) const {
        std::string out;
        for (char c : s) {
            if (c == '|') out += "\\|";
            else out += c;
        }
        return out;
    }

public:
    FileManager() = default;

    // =============================================
    // PATIENTS
    // =============================================
    void savePatients(const std::vector<PatientRecord>& patients) const {
        std::ofstream file(PATIENTS_FILE);
        if (!file.is_open())
            throw std::runtime_error("Cannot open " + PATIENTS_FILE + " for writing.");

        for (const auto& p : patients) {
            file << escape(p.patientId)      << DELIM
                 << escape(p.firstName)      << DELIM
                 << escape(p.lastName)       << DELIM
                 << escape(p.dateOfBirth)    << DELIM
                 << escape(p.gender)         << DELIM
                 << escape(p.address)        << DELIM
                 << escape(p.phone)          << DELIM
                 << escape(p.email)          << DELIM
                 << escape(p.nextOfKinName)  << DELIM
                 << escape(p.nextOfKinPhone) << DELIM
                 << escape(p.createdAt)      << "\n";
        }
        file.close();
        std::cout << "[FILE] Saved " << patients.size() << " patient(s) to " << PATIENTS_FILE << "\n";
    }

    std::vector<PatientRecord> loadPatients() const {
        std::vector<PatientRecord> patients;
        std::ifstream file(PATIENTS_FILE);
        if (!file.is_open()) return patients;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::vector<std::string> parts;
            std::string token;
            while (std::getline(ss, token, DELIM)) parts.push_back(token);
            if (parts.size() < 11) continue;

            PatientRecord p;
            p.patientId      = parts[0];  p.firstName      = parts[1];
            p.lastName       = parts[2];  p.dateOfBirth    = parts[3];
            p.gender         = parts[4];  p.address        = parts[5];
            p.phone          = parts[6];  p.email          = parts[7];
            p.nextOfKinName  = parts[8];  p.nextOfKinPhone = parts[9];
            p.createdAt      = parts[10];
            patients.push_back(p);
        }
        file.close();
        return patients;
    }

    // Append a single patient
    void appendPatient(const PatientRecord& p) const {
        std::ofstream file(PATIENTS_FILE, std::ios::app);
        if (!file.is_open())
            throw std::runtime_error("Cannot append to " + PATIENTS_FILE);
        file << escape(p.patientId)      << DELIM
             << escape(p.firstName)      << DELIM
             << escape(p.lastName)       << DELIM
             << escape(p.dateOfBirth)    << DELIM
             << escape(p.gender)         << DELIM
             << escape(p.address)        << DELIM
             << escape(p.phone)          << DELIM
             << escape(p.email)          << DELIM
             << escape(p.nextOfKinName)  << DELIM
             << escape(p.nextOfKinPhone) << DELIM
             << escape(p.createdAt)      << "\n";
        file.close();
    }

    // =============================================
    // APPOINTMENTS
    // =============================================
    void saveAppointments(const std::vector<AppointmentRecord>& appts) const {
        std::ofstream file(APPOINTMENTS_FILE);
        if (!file.is_open())
            throw std::runtime_error("Cannot open " + APPOINTMENTS_FILE);

        for (const auto& a : appts) {
            file << escape(a.appointmentId) << DELIM
                 << escape(a.patientId)     << DELIM
                 << escape(a.staffId)       << DELIM
                 << escape(a.scheduledAt)   << DELIM
                 << escape(a.status)        << DELIM
                 << escape(a.purpose)       << DELIM
                 << (a.conflictFlagged ? "1" : "0") << "\n";
        }
        file.close();
        std::cout << "[FILE] Saved " << appts.size() << " appointment(s).\n";
    }

    std::vector<AppointmentRecord> loadAppointments() const {
        std::vector<AppointmentRecord> appts;
        std::ifstream file(APPOINTMENTS_FILE);
        if (!file.is_open()) return appts;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::vector<std::string> parts;
            std::string token;
            while (std::getline(ss, token, DELIM)) parts.push_back(token);
            if (parts.size() < 7) continue;

            AppointmentRecord a;
            a.appointmentId  = parts[0]; a.patientId    = parts[1];
            a.staffId        = parts[2]; a.scheduledAt  = parts[3];
            a.status         = parts[4]; a.purpose      = parts[5];
            a.conflictFlagged = (parts[6] == "1");
            appts.push_back(a);
        }
        file.close();
        return appts;
    }

    void appendAppointment(const AppointmentRecord& a) const {
        std::ofstream file(APPOINTMENTS_FILE, std::ios::app);
        if (!file.is_open())
            throw std::runtime_error("Cannot append to " + APPOINTMENTS_FILE);
        file << escape(a.appointmentId) << DELIM
             << escape(a.patientId)     << DELIM
             << escape(a.staffId)       << DELIM
             << escape(a.scheduledAt)   << DELIM
             << escape(a.status)        << DELIM
             << escape(a.purpose)       << DELIM
             << (a.conflictFlagged ? "1" : "0") << "\n";
        file.close();
    }

    // =============================================
    // BILLING
    // =============================================
    void saveBilling(const std::vector<BillingRecord>& bills) const {
        std::ofstream file(BILLING_FILE);
        if (!file.is_open())
            throw std::runtime_error("Cannot open " + BILLING_FILE);

        for (const auto& b : bills) {
            file << escape(b.billingId)       << DELIM
                 << escape(b.patientId)       << DELIM
                 << escape(b.appointmentId)   << DELIM
                 << b.totalAmount             << DELIM
                 << b.insuranceCoverage       << DELIM
                 << b.deductible              << DELIM
                 << b.patientPays             << DELIM
                 << escape(b.paymentMethod)   << DELIM
                 << escape(b.status)          << DELIM
                 << escape(b.billedAt)        << "\n";
        }
        file.close();
        std::cout << "[FILE] Saved " << bills.size() << " billing record(s).\n";
    }

    std::vector<BillingRecord> loadBilling() const {
        std::vector<BillingRecord> bills;
        std::ifstream file(BILLING_FILE);
        if (!file.is_open()) return bills;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::vector<std::string> parts;
            std::string token;
            while (std::getline(ss, token, DELIM)) parts.push_back(token);
            if (parts.size() < 10) continue;

            BillingRecord b;
            b.billingId       = parts[0]; b.patientId     = parts[1];
            b.appointmentId   = parts[2];
            b.totalAmount     = std::stod(parts[3]);
            b.insuranceCoverage = std::stod(parts[4]);
            b.deductible      = std::stod(parts[5]);
            b.patientPays     = std::stod(parts[6]);
            b.paymentMethod   = parts[7]; b.status        = parts[8];
            b.billedAt        = parts[9];
            bills.push_back(b);
        }
        file.close();
        return bills;
    }

    void appendBilling(const BillingRecord& b) const {
        std::ofstream file(BILLING_FILE, std::ios::app);
        if (!file.is_open())
            throw std::runtime_error("Cannot append to " + BILLING_FILE);
        file << escape(b.billingId)     << DELIM
             << escape(b.patientId)     << DELIM
             << escape(b.appointmentId) << DELIM
             << b.totalAmount           << DELIM
             << b.insuranceCoverage     << DELIM
             << b.deductible            << DELIM
             << b.patientPays           << DELIM
             << escape(b.paymentMethod) << DELIM
             << escape(b.status)        << DELIM
             << escape(b.billedAt)      << "\n";
        file.close();
    }

    // =============================================
    // EHR RECORDS
    // =============================================
    void saveEHR(const std::vector<EHRRecord>& records) const {
        std::ofstream file(EHR_FILE);
        if (!file.is_open())
            throw std::runtime_error("Cannot open " + EHR_FILE);

        for (const auto& e : records) {
            file << escape(e.ehrId)                << DELIM
                 << escape(e.patientId)            << DELIM
                 << escape(e.visitDate)            << DELIM
                 << escape(e.diagnosis)            << DELIM
                 << escape(e.prescription)         << DELIM
                 << escape(e.allergies)            << DELIM
                 << escape(e.immunizations)        << DELIM
                 << escape(e.labResultsSummary)    << DELIM
                 << escape(e.purposeOfProcessing)  << DELIM
                 << escape(e.recordedBy)           << "\n";
        }
        file.close();
        std::cout << "[FILE] Saved " << records.size() << " EHR record(s).\n";
    }

    std::vector<EHRRecord> loadEHR() const {
        std::vector<EHRRecord> records;
        std::ifstream file(EHR_FILE);
        if (!file.is_open()) return records;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::vector<std::string> parts;
            std::string token;
            while (std::getline(ss, token, DELIM)) parts.push_back(token);
            if (parts.size() < 10) continue;

            EHRRecord e;
            e.ehrId               = parts[0]; e.patientId         = parts[1];
            e.visitDate           = parts[2]; e.diagnosis         = parts[3];
            e.prescription        = parts[4]; e.allergies         = parts[5];
            e.immunizations       = parts[6]; e.labResultsSummary = parts[7];
            e.purposeOfProcessing = parts[8]; e.recordedBy        = parts[9];
            records.push_back(e);
        }
        file.close();
        return records;
    }

    void appendEHR(const EHRRecord& e) const {
        std::ofstream file(EHR_FILE, std::ios::app);
        if (!file.is_open())
            throw std::runtime_error("Cannot append to " + EHR_FILE);
        file << escape(e.ehrId)               << DELIM
             << escape(e.patientId)           << DELIM
             << escape(e.visitDate)           << DELIM
             << escape(e.diagnosis)           << DELIM
             << escape(e.prescription)        << DELIM
             << escape(e.allergies)           << DELIM
             << escape(e.immunizations)       << DELIM
             << escape(e.labResultsSummary)   << DELIM
             << escape(e.purposeOfProcessing) << DELIM
             << escape(e.recordedBy)          << "\n";
        file.close();
    }

    // =============================================
    // AUDIT LOG
    // =============================================
    void writeAuditLog(const std::string& userId,
                       const std::string& action,
                       const std::string& detail,
                       const std::string& timestamp) const {
        std::ofstream file(AUDIT_FILE, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "[WARN] Cannot write audit log.\n";
            return;
        }
        file << timestamp << DELIM << userId << DELIM
             << action << DELIM << detail << "\n";
        file.close();
    }

    void displayAuditLog() const {
        std::ifstream file(AUDIT_FILE);
        if (!file.is_open()) {
            std::cout << "[AUDIT] No audit log found.\n";
            return;
        }
        std::cout << "\n===== AUDIT LOG =====\n";
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) std::cout << "  " << line << "\n";
        }
        std::cout << "=====================\n";
        file.close();
    }
};

#endif // FILE_MANAGER_H
