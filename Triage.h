// ============================================================
// File: Triage.h
// Description: Triage & Analytics - RiskScore, TriageRecord,
//              TriageManager with moving average and flagging
// Project: EMHR Healthcare System - Semester 2 AY2025/2026
// ============================================================

#ifndef TRIAGE_H
#define TRIAGE_H

#include <string>
#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "NotificationService.h"

using namespace std;

// ============================================================
// TriageVitals — CLASS (not struct)
// Named TriageVitals to avoid clash with person4 Vitals class
// ENCAPSULATION: private attributes, public accessors/mutators
// ============================================================
class TriageVitals {
private:
    double bloodPressureSystolic;
    double bloodPressureDiastolic;
    double heartRate;
    double temperature;
    double oxygenSaturation;
    double respiratoryRate;
    string recordedAt;

public:
    // Default constructor
    TriageVitals()
        : bloodPressureSystolic(120), bloodPressureDiastolic(80),
          heartRate(72), temperature(37.0),
          oxygenSaturation(98.0), respiratoryRate(16),
          recordedAt("") {}

    // Full constructor
    TriageVitals(double bpS, double bpD, double hr,
                 double temp, double spo2, double rr,
                 const string& ts = "")
        : bloodPressureSystolic(bpS), bloodPressureDiastolic(bpD),
          heartRate(hr), temperature(temp),
          oxygenSaturation(spo2), respiratoryRate(rr),
          recordedAt(ts) {}

    // Accessors
    double getBloodPressureSystolic()  const { return bloodPressureSystolic;  }
    double getBloodPressureDiastolic() const { return bloodPressureDiastolic; }
    double getHeartRate()              const { return heartRate;              }
    double getTemperature()            const { return temperature;            }
    double getOxygenSaturation()       const { return oxygenSaturation;       }
    double getRespiratoryRate()        const { return respiratoryRate;        }
    string getRecordedAt()             const { return recordedAt;             }

    // Mutators with validation
    void setBloodPressureSystolic(double v) {
        if (v < 0) throw invalid_argument("BP cannot be negative.");
        bloodPressureSystolic = v;
    }
    void setBloodPressureDiastolic(double v) {
        if (v < 0) throw invalid_argument("BP cannot be negative.");
        bloodPressureDiastolic = v;
    }
    void setHeartRate(double v) {
        if (v < 0) throw invalid_argument("Heart rate cannot be negative.");
        heartRate = v;
    }
    void setTemperature(double v) {
        if (v < 0) throw invalid_argument("Temperature cannot be negative.");
        temperature = v;
    }
    void setOxygenSaturation(double v) {
        if (v < 0 || v > 100) throw invalid_argument("SpO2 must be 0-100.");
        oxygenSaturation = v;
    }
    void setRespiratoryRate(double v) {
        if (v < 0) throw invalid_argument("Respiratory rate cannot be negative.");
        respiratoryRate = v;
    }
    void setRecordedAt(const string& ts) { recordedAt = ts; }

    void display() const {
        cout << "  BP: " << bloodPressureSystolic << "/"
             << bloodPressureDiastolic
             << " mmHg | HR: " << heartRate << " bpm"
             << " | Temp: " << temperature << " C"
             << " | SpO2: " << oxygenSaturation << "%"
             << " | RR: " << respiratoryRate << " br/min\n";
    }
};

// ============================================================
// RiskLevel enum
// ============================================================
enum class RiskLevel { LOW, MEDIUM, HIGH };

inline string riskLevelToString(RiskLevel r) {
    switch (r) {
        case RiskLevel::LOW:    return "LOW";
        case RiskLevel::MEDIUM: return "MEDIUM";
        case RiskLevel::HIGH:   return "HIGH";
    }
    return "UNKNOWN";
}

// ============================================================
// RiskScore — CLASS
// ENCAPSULATION: private score and level
// Calculates risk from TriageVitals
// ============================================================
class RiskScore {
private:
    int       score;
    RiskLevel level;

    static const int LOW_THRESHOLD    = 30;
    static const int MEDIUM_THRESHOLD = 60;

public:
    RiskScore() : score(0), level(RiskLevel::LOW) {}

    void calculate(const TriageVitals& v) {
        score = 0;

        if (v.getBloodPressureSystolic() >= 180 ||
            v.getBloodPressureSystolic() < 80)  score += 25;
        else if (v.getBloodPressureSystolic() >= 140 ||
                 v.getBloodPressureSystolic() < 90) score += 15;

        if (v.getHeartRate() > 130 || v.getHeartRate() < 40)      score += 25;
        else if (v.getHeartRate() > 100 || v.getHeartRate() < 60) score += 10;

        if (v.getTemperature() >= 39.5 || v.getTemperature() < 35.0) score += 20;
        else if (v.getTemperature() >= 38.5)                          score += 10;

        if (v.getOxygenSaturation() < 90)      score += 25;
        else if (v.getOxygenSaturation() < 95) score += 10;

        if (v.getRespiratoryRate() > 30 || v.getRespiratoryRate() < 8) score += 5;

        if (score > 100) score = 100;

        if      (score >= MEDIUM_THRESHOLD) level = RiskLevel::HIGH;
        else if (score >= LOW_THRESHOLD)    level = RiskLevel::MEDIUM;
        else                                level = RiskLevel::LOW;
    }

    // Accessors
    int       getScore()       const { return score; }
    RiskLevel getLevel()       const { return level; }
    string    getLevelString() const { return riskLevelToString(level); }

    void display() const {
        cout << "  Risk Score : " << score << "/100"
             << " | Level: " << getLevelString() << "\n";
    }
};

// ============================================================
// TriageRecord — CLASS
// COMPOSITION: owns TriageVitals and RiskScore
// ENCAPSULATION: private attributes, public accessors/mutators
// ============================================================
class TriageRecord {
private:
    string       patientId;
    string       patientName;
    TriageVitals vitals;
    RiskScore    riskScore;
    string       arrivalTime;
    string       triageNote;
    bool         flaggedAbnormal;
    int          responseTimeMinutes;

public:
    TriageRecord()
        : flaggedAbnormal(false), responseTimeMinutes(0) {}

    TriageRecord(const string& pid, const string& name,
                 const TriageVitals& v, const string& arrival)
        : patientId(pid), patientName(name), vitals(v),
          arrivalTime(arrival), flaggedAbnormal(false),
          responseTimeMinutes(0) {
        riskScore.calculate(v);
    }

    // Accessors
    string       getPatientId()           const { return patientId;           }
    string       getPatientName()         const { return patientName;         }
    TriageVitals getVitals()              const { return vitals;              }
    RiskScore    getRiskScore()           const { return riskScore;           }
    string       getArrivalTime()         const { return arrivalTime;         }
    string       getTriageNote()          const { return triageNote;          }
    bool         isFlaggedAbnormal()      const { return flaggedAbnormal;     }
    int          getResponseTimeMinutes() const { return responseTimeMinutes; }

    // Mutators
    void setPatientId(const string& v)        { patientId = v;            }
    void setPatientName(const string& v)      { patientName = v;          }
    void setVitals(const TriageVitals& v)     { vitals = v; riskScore.calculate(v); }
    void setArrivalTime(const string& v)      { arrivalTime = v;          }
    void setTriageNote(const string& v)       { triageNote = v;           }
    void setFlaggedAbnormal(bool v)           { flaggedAbnormal = v;      }
    void setResponseTimeMinutes(int v)        { responseTimeMinutes = v;  }

    // Allow TriageManager to recalculate risk
    void recalculateRisk() { riskScore.calculate(vitals); }

    void display() const {
        cout << "=== Triage Record ===\n";
        cout << "  Patient  : " << patientName << " (" << patientId << ")\n";
        cout << "  Arrival  : " << arrivalTime << "\n";
        cout << "  Note     : " << triageNote  << "\n";
        cout << "  Flagged  : " << (flaggedAbnormal ? "YES" : "No") << "\n";
        vitals.display();
        riskScore.display();
        cout << "====================\n";
    }
};

// ============================================================
// TriageManager — CLASS
// Manages all triage records, moving averages, analytics
// FILE PERSISTENCE: saves/loads from triage_records.txt
// ============================================================
class TriageManager {
private:
    vector<TriageRecord> records;
    NotificationSubject* notifier;
    int                  windowN;
    const string         TRIAGE_FILE = "triage_records.txt";

public:
    explicit TriageManager(NotificationSubject* n = nullptr, int window = 3)
        : notifier(n), windowN(window) { loadFromFile(); }

    void addRecord(TriageRecord& rec) {
        rec.recalculateRisk();
        rec.setFlaggedAbnormal(isFlaggedAbnormal(rec));
        records.push_back(rec);
        saveToFile();

        cout << "\n[TRIAGE] Record added for " << rec.getPatientName() << "\n";
        rec.display();

        if (rec.getRiskScore().getLevel() == RiskLevel::HIGH && notifier) {
            string detail = "Patient " + rec.getPatientName() +
                            " flagged HIGH RISK. Score: " +
                            to_string(rec.getRiskScore().getScore());
            notifier->notifyObservers("HIGH_RISK_TRIAGE",
                                      "staff@emhr.gov.jm", detail);
        }
    }

    double movingAverageHeartRate(const string& pid) const {
        vector<double> vals;
        for (auto it = records.rbegin(); it != records.rend(); ++it) {
            if (it->getPatientId() == pid) {
                vals.push_back(it->getVitals().getHeartRate());
                if ((int)vals.size() == windowN) break;
            }
        }
        if (vals.empty()) return 0.0;
        return accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
    }

    double movingAverageTemperature(const string& pid) const {
        vector<double> vals;
        for (auto it = records.rbegin(); it != records.rend(); ++it) {
            if (it->getPatientId() == pid) {
                vals.push_back(it->getVitals().getTemperature());
                if ((int)vals.size() == windowN) break;
            }
        }
        if (vals.empty()) return 0.0;
        return accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
    }

    bool isFlaggedAbnormal(const TriageRecord& rec) const {
        double avgHR = movingAverageHeartRate(rec.getPatientId());
        if (avgHR > 0 &&
            abs(rec.getVitals().getHeartRate() - avgHR) / avgHR > 0.20)
            return true;
        double avgT = movingAverageTemperature(rec.getPatientId());
        if (avgT > 0 &&
            abs(rec.getVitals().getTemperature() - avgT) / avgT > 0.03)
            return true;
        return false;
    }

    void analyzeResponseTimes() const {
        if (records.empty()) {
            cout << "[ANALYTICS] No triage records.\n"; return;
        }
        int highRisk = 0, totalResp = 0, countResp = 0;
        for (const auto& r : records) {
            if (r.getRiskScore().getLevel() == RiskLevel::HIGH) highRisk++;
            if (r.getResponseTimeMinutes() > 0) {
                totalResp += r.getResponseTimeMinutes();
                countResp++;
            }
        }
        double avg = countResp > 0 ? (double)totalResp / countResp : 0;
        cout << "\n===== TRIAGE ANALYTICS =====\n";
        cout << "  Total Records   : " << records.size() << "\n";
        cout << "  High Risk Cases : " << highRisk       << "\n";
        cout << "  Avg Response    : " << avg << " min\n";
        if      (avg <= 10) cout << "  Efficiency: GOOD\n";
        else if (avg <= 20) cout << "  Efficiency: MODERATE\n";
        else                cout << "  Efficiency: POOR\n";
        cout << "============================\n";
    }

    int countHighRisk() const {
        int c = 0;
        for (const auto& r : records)
            if (r.getRiskScore().getLevel() == RiskLevel::HIGH) c++;
        return c;
    }

    int totalRecords() const { return (int)records.size(); }

    void displayAll() const {
        cout << "\n===== ALL TRIAGE RECORDS =====\n";
        if (records.empty()) { cout << "  No records.\n"; return; }
        for (const auto& r : records) r.display();
    }

    void saveToFile() const {
        ofstream file(TRIAGE_FILE);
        if (!file.is_open()) {
            cerr << "[ERROR] Cannot write triage file.\n"; return;
        }
        for (const auto& r : records) {
            file << r.getPatientId()                          << "|"
                 << r.getPatientName()                        << "|"
                 << r.getVitals().getBloodPressureSystolic()  << "|"
                 << r.getVitals().getBloodPressureDiastolic() << "|"
                 << r.getVitals().getHeartRate()              << "|"
                 << r.getVitals().getTemperature()            << "|"
                 << r.getVitals().getOxygenSaturation()       << "|"
                 << r.getVitals().getRespiratoryRate()        << "|"
                 << r.getArrivalTime()                        << "|"
                 << r.getResponseTimeMinutes()                << "|"
                 << r.getRiskScore().getScore()               << "|"
                 << r.getRiskScore().getLevelString()         << "|"
                 << (r.isFlaggedAbnormal() ? "1" : "0")      << "|"
                 << r.getTriageNote()                         << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        ifstream file(TRIAGE_FILE);
        if (!file.is_open()) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            vector<string> parts;
            string token;
            while (getline(ss, token, '|')) parts.push_back(token);
            if (parts.size() < 14) continue;

            TriageRecord r;
            r.setPatientId(parts[0]);
            r.setPatientName(parts[1]);
            TriageVitals v(stod(parts[2]), stod(parts[3]),
                           stod(parts[4]), stod(parts[5]),
                           stod(parts[6]), stod(parts[7]));
            r.setVitals(v);
            r.setArrivalTime(parts[8]);
            r.setResponseTimeMinutes(stoi(parts[9]));
            r.setFlaggedAbnormal(parts[12] == "1");
            r.setTriageNote(parts[13]);
            records.push_back(r);
        }
        file.close();
    }
};

#endif // TRIAGE_H
