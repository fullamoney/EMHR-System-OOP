// ============================================================
// File: FileManager.h
// Description: File persistence for Patients, Appointments,
//              EHR, and Billing (Repository pattern)
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

using namespace std;

// ============================================================
// PatientRecord — CLASS (not struct)
// ENCAPSULATION: private attributes with accessors/mutators
// Used by FileManager as a persistence data object
// ============================================================
class PatientRecord {
private:
    string patientId;
    string firstName;
    string lastName;
    string dateOfBirth;
    string gender;
    string address;
    string phone;
    string email;
    string nextOfKinName;
    string nextOfKinPhone;
    string createdAt;

public:
    PatientRecord() {}

    PatientRecord(const string& pid, const string& fn, const string& ln,
                  const string& dob, const string& gen, const string& addr,
                  const string& ph, const string& em, const string& kinName,
                  const string& kinPhone, const string& created)
        : patientId(pid), firstName(fn), lastName(ln),
          dateOfBirth(dob), gender(gen), address(addr),
          phone(ph), email(em), nextOfKinName(kinName),
          nextOfKinPhone(kinPhone), createdAt(created) {}

    // Accessors
    string getPatientId()     const { return patientId;     }
    string getFirstName()     const { return firstName;     }
    string getLastName()      const { return lastName;      }
    string getDateOfBirth()   const { return dateOfBirth;   }
    string getGender()        const { return gender;        }
    string getAddress()       const { return address;       }
    string getPhone()         const { return phone;         }
    string getEmail()         const { return email;         }
    string getNextOfKinName() const { return nextOfKinName; }
    string getNextOfKinPhone()const { return nextOfKinPhone;}
    string getCreatedAt()     const { return createdAt;     }

    // Mutators with validation
    void setPatientId(const string& v)    { if (!v.empty()) patientId = v;    }
    void setFirstName(const string& v)    { if (!v.empty()) firstName = v;    }
    void setLastName(const string& v)     { if (!v.empty()) lastName = v;     }
    void setDateOfBirth(const string& v)  { dateOfBirth = v;                  }
    void setGender(const string& v)       { gender = v;                       }
    void setAddress(const string& v)      { address = v;                      }
    void setPhone(const string& v)        { phone = v;                        }
    void setEmail(const string& v)        { email = v;                        }
    void setNextOfKinName(const string& v){ nextOfKinName = v;                }
    void setNextOfKinPhone(const string& v){ nextOfKinPhone = v;              }
    void setCreatedAt(const string& v)    { createdAt = v;                    }
};

// ============================================================
// AppointmentRecord — CLASS
// ============================================================
class AppointmentRecord {
private:
    string appointmentId;
    string patientId;
    string staffId;
    string scheduledAt;
    string status;
    string purpose;
    bool   conflictFlagged;

public:
    AppointmentRecord()
        : conflictFlagged(false) {}

    AppointmentRecord(const string& aid, const string& pid,
                      const string& sid, const string& sched,
                      const string& stat, const string& purp, bool conflict)
        : appointmentId(aid), patientId(pid), staffId(sid),
          scheduledAt(sched), status(stat), purpose(purp),
          conflictFlagged(conflict) {}

    // Accessors
    string getAppointmentId()  const { return appointmentId;  }
    string getPatientId()      const { return patientId;      }
    string getStaffId()        const { return staffId;        }
    string getScheduledAt()    const { return scheduledAt;    }
    string getStatus()         const { return status;         }
    string getPurpose()        const { return purpose;        }
    bool   getConflictFlagged()const { return conflictFlagged;}

    // Mutators
    void setAppointmentId(const string& v) { appointmentId  = v; }
    void setPatientId(const string& v)     { patientId      = v; }
    void setStaffId(const string& v)       { staffId        = v; }
    void setScheduledAt(const string& v)   { scheduledAt    = v; }
    void setStatus(const string& v)        { status         = v; }
    void setPurpose(const string& v)       { purpose        = v; }
    void setConflictFlagged(bool v)        { conflictFlagged = v;}
};

// ============================================================
// BillingRecord — CLASS
// ============================================================
class BillingRecord {
private:
    string billingId;
    string patientId;
    string appointmentId;
    double totalAmount;
    double insuranceCoverage;
    double deductible;
    double patientPays;
    string paymentMethod;
    string status;
    string billedAt;

public:
    BillingRecord()
        : totalAmount(0), insuranceCoverage(0),
          deductible(0), patientPays(0) {}

    // Accessors
    string getBillingId()        const { return billingId;        }
    string getPatientId()        const { return patientId;        }
    string getAppointmentId()    const { return appointmentId;    }
    double getTotalAmount()      const { return totalAmount;      }
    double getInsuranceCoverage()const { return insuranceCoverage;}
    double getDeductible()       const { return deductible;       }
    double getPatientPays()      const { return patientPays;      }
    string getPaymentMethod()    const { return paymentMethod;    }
    string getStatus()           const { return status;           }
    string getBilledAt()         const { return billedAt;         }

    // Mutators with validation
    void setBillingId(const string& v)     { billingId     = v; }
    void setPatientId(const string& v)     { patientId     = v; }
    void setAppointmentId(const string& v) { appointmentId = v; }
    void setTotalAmount(double v)          { if (v >= 0) totalAmount = v; }
    void setInsuranceCoverage(double v)    { if (v >= 0 && v <= 1) insuranceCoverage = v; }
    void setDeductible(double v)           { if (v >= 0) deductible = v; }
    void setPatientPays(double v)          { if (v >= 0) patientPays = v; }
    void setPaymentMethod(const string& v) { paymentMethod = v; }
    void setStatus(const string& v)        { status        = v; }
    void setBilledAt(const string& v)      { billedAt      = v; }
};

// ============================================================
// EHRRecord — CLASS
// ============================================================
class EHRRecord {
private:
    string ehrId;
    string patientId;
    string visitDate;
    string diagnosis;
    string prescription;
    string allergies;
    string immunizations;
    string labResultsSummary;
    string purposeOfProcessing;
    string recordedBy;

public:
    EHRRecord() {}

    // Accessors
    string getEhrId()               const { return ehrId;               }
    string getPatientId()           const { return patientId;           }
    string getVisitDate()           const { return visitDate;           }
    string getDiagnosis()           const { return diagnosis;           }
    string getPrescription()        const { return prescription;        }
    string getAllergies()            const { return allergies;           }
    string getImmunizations()       const { return immunizations;       }
    string getLabResultsSummary()   const { return labResultsSummary;   }
    string getPurposeOfProcessing() const { return purposeOfProcessing; }
    string getRecordedBy()          const { return recordedBy;          }

    // Mutators
    void setEhrId(const string& v)               { ehrId               = v; }
    void setPatientId(const string& v)           { patientId           = v; }
    void setVisitDate(const string& v)           { visitDate           = v; }
    void setDiagnosis(const string& v)           { diagnosis           = v; }
    void setPrescription(const string& v)        { prescription        = v; }
    void setAllergies(const string& v)           { allergies           = v; }
    void setImmunizations(const string& v)       { immunizations       = v; }
    void setLabResultsSummary(const string& v)   { labResultsSummary   = v; }
    void setPurposeOfProcessing(const string& v) { purposeOfProcessing = v; }
    void setRecordedBy(const string& v)          { recordedBy          = v; }
};

// ============================================================
// FileManager — CLASS (Repository pattern)
// ENCAPSULATION: private file names, public save/load methods
// Handles all file I/O for the EMHR system
// ============================================================
class FileManager {
private:
    const string PATIENTS_FILE     = "patients.txt";
    const string APPOINTMENTS_FILE = "appointments.txt";
    const string BILLING_FILE      = "billing.txt";
    const string EHR_FILE          = "ehr_records.txt";
    const string AUDIT_FILE        = "audit_log.txt";
    const char   DELIM             = '|';

    string escape(const string& s) const {
        string out;
        for (char c : s) {
            if (c == '|') out += "\\|";
            else out += c;
        }
        return out;
    }

public:
    FileManager() = default;

    // ---- PATIENTS ----
    void savePatients(const vector<PatientRecord>& patients) const {
        ofstream file(PATIENTS_FILE);
        if (!file.is_open())
            throw runtime_error("Cannot open " + PATIENTS_FILE);
        for (const auto& p : patients) {
            file << escape(p.getPatientId())      << DELIM
                 << escape(p.getFirstName())      << DELIM
                 << escape(p.getLastName())       << DELIM
                 << escape(p.getDateOfBirth())    << DELIM
                 << escape(p.getGender())         << DELIM
                 << escape(p.getAddress())        << DELIM
                 << escape(p.getPhone())          << DELIM
                 << escape(p.getEmail())          << DELIM
                 << escape(p.getNextOfKinName())  << DELIM
                 << escape(p.getNextOfKinPhone()) << DELIM
                 << escape(p.getCreatedAt())      << "\n";
        }
        file.close();
        cout << "[FILE] Saved " << patients.size() << " patient(s).\n";
    }

    vector<PatientRecord> loadPatients() const {
        vector<PatientRecord> patients;
        ifstream file(PATIENTS_FILE);
        if (!file.is_open()) return patients;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            vector<string> parts;
            string token;
            while (getline(ss, token, DELIM)) parts.push_back(token);
            if (parts.size() < 11) continue;
            PatientRecord p;
            p.setPatientId(parts[0]);   p.setFirstName(parts[1]);
            p.setLastName(parts[2]);    p.setDateOfBirth(parts[3]);
            p.setGender(parts[4]);      p.setAddress(parts[5]);
            p.setPhone(parts[6]);       p.setEmail(parts[7]);
            p.setNextOfKinName(parts[8]); p.setNextOfKinPhone(parts[9]);
            p.setCreatedAt(parts[10]);
            patients.push_back(p);
        }
        file.close();
        return patients;
    }

    // ---- APPOINTMENTS ----
    void saveAppointments(const vector<AppointmentRecord>& appts) const {
        ofstream file(APPOINTMENTS_FILE);
        if (!file.is_open())
            throw runtime_error("Cannot open " + APPOINTMENTS_FILE);
        for (const auto& a : appts) {
            file << escape(a.getAppointmentId()) << DELIM
                 << escape(a.getPatientId())     << DELIM
                 << escape(a.getStaffId())       << DELIM
                 << escape(a.getScheduledAt())   << DELIM
                 << escape(a.getStatus())        << DELIM
                 << escape(a.getPurpose())       << DELIM
                 << (a.getConflictFlagged() ? "1" : "0") << "\n";
        }
        file.close();
        cout << "[FILE] Saved " << appts.size() << " appointment(s).\n";
    }

    vector<AppointmentRecord> loadAppointments() const {
        vector<AppointmentRecord> appts;
        ifstream file(APPOINTMENTS_FILE);
        if (!file.is_open()) return appts;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            vector<string> parts;
            string token;
            while (getline(ss, token, DELIM)) parts.push_back(token);
            if (parts.size() < 7) continue;
            AppointmentRecord a;
            a.setAppointmentId(parts[0]); a.setPatientId(parts[1]);
            a.setStaffId(parts[2]);       a.setScheduledAt(parts[3]);
            a.setStatus(parts[4]);        a.setPurpose(parts[5]);
            a.setConflictFlagged(parts[6] == "1");
            appts.push_back(a);
        }
        file.close();
        return appts;
    }

    // ---- BILLING ----
    void saveBilling(const vector<BillingRecord>& bills) const {
        ofstream file(BILLING_FILE);
        if (!file.is_open())
            throw runtime_error("Cannot open " + BILLING_FILE);
        for (const auto& b : bills) {
            file << escape(b.getBillingId())     << DELIM
                 << escape(b.getPatientId())     << DELIM
                 << escape(b.getAppointmentId()) << DELIM
                 << b.getTotalAmount()           << DELIM
                 << b.getInsuranceCoverage()     << DELIM
                 << b.getDeductible()            << DELIM
                 << b.getPatientPays()           << DELIM
                 << escape(b.getPaymentMethod()) << DELIM
                 << escape(b.getStatus())        << DELIM
                 << escape(b.getBilledAt())      << "\n";
        }
        file.close();
        cout << "[FILE] Saved " << bills.size() << " billing record(s).\n";
    }

    vector<BillingRecord> loadBilling() const {
        vector<BillingRecord> bills;
        ifstream file(BILLING_FILE);
        if (!file.is_open()) return bills;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            vector<string> parts;
            string token;
            while (getline(ss, token, DELIM)) parts.push_back(token);
            if (parts.size() < 10) continue;
            BillingRecord b;
            b.setBillingId(parts[0]);     b.setPatientId(parts[1]);
            b.setAppointmentId(parts[2]);
            b.setTotalAmount(stod(parts[3]));
            b.setInsuranceCoverage(stod(parts[4]));
            b.setDeductible(stod(parts[5]));
            b.setPatientPays(stod(parts[6]));
            b.setPaymentMethod(parts[7]); b.setStatus(parts[8]);
            b.setBilledAt(parts[9]);
            bills.push_back(b);
        }
        file.close();
        return bills;
    }

    // ---- EHR ----
    void saveEHR(const vector<EHRRecord>& records) const {
        ofstream file(EHR_FILE);
        if (!file.is_open())
            throw runtime_error("Cannot open " + EHR_FILE);
        for (const auto& e : records) {
            file << escape(e.getEhrId())               << DELIM
                 << escape(e.getPatientId())           << DELIM
                 << escape(e.getVisitDate())           << DELIM
                 << escape(e.getDiagnosis())           << DELIM
                 << escape(e.getPrescription())        << DELIM
                 << escape(e.getAllergies())            << DELIM
                 << escape(e.getImmunizations())       << DELIM
                 << escape(e.getLabResultsSummary())   << DELIM
                 << escape(e.getPurposeOfProcessing()) << DELIM
                 << escape(e.getRecordedBy())          << "\n";
        }
        file.close();
        cout << "[FILE] Saved " << records.size() << " EHR record(s).\n";
    }

    vector<EHRRecord> loadEHR() const {
        vector<EHRRecord> records;
        ifstream file(EHR_FILE);
        if (!file.is_open()) return records;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            vector<string> parts;
            string token;
            while (getline(ss, token, DELIM)) parts.push_back(token);
            if (parts.size() < 10) continue;
            EHRRecord e;
            e.setEhrId(parts[0]);               e.setPatientId(parts[1]);
            e.setVisitDate(parts[2]);           e.setDiagnosis(parts[3]);
            e.setPrescription(parts[4]);        e.setAllergies(parts[5]);
            e.setImmunizations(parts[6]);       e.setLabResultsSummary(parts[7]);
            e.setPurposeOfProcessing(parts[8]); e.setRecordedBy(parts[9]);
            records.push_back(e);
        }
        file.close();
        return records;
    }

    // ---- AUDIT LOG ----
    void writeAuditLog(const string& userId, const string& action,
                       const string& detail, const string& timestamp) const {
        ofstream file(AUDIT_FILE, ios::app);
        if (!file.is_open()) { cerr << "[WARN] Cannot write audit log.\n"; return; }
        file << timestamp << DELIM << userId << DELIM
             << action << DELIM << detail << "\n";
        file.close();
    }

    void displayAuditLog() const {
        ifstream file(AUDIT_FILE);
        if (!file.is_open()) { cout << "[AUDIT] No audit log found.\n"; return; }
        cout << "\n===== AUDIT LOG =====\n";
        string line;
        while (getline(file, line))
            if (!line.empty()) cout << "  " << line << "\n";
        cout << "=====================\n";
        file.close();
    }
};

#endif // FILE_MANAGER_H

