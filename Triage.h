// ============================================================
// File: Triage.h
// Description: Triage & Analytics - RiskScore, moving average,
//              deviation flagging, and response-time analysis
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

// ============================================================
// Vitals struct — COMPOSITION: used inside TriageRecord
// ============================================================
struct Vitals {
    double bloodPressureSystolic;   // mmHg
    double bloodPressureDiastolic;  // mmHg
    double heartRate;               // bpm
    double temperature;             // Celsius
    double oxygenSaturation;        // %
    double respiratoryRate;         // breaths/min
    std::string recordedAt;         // ISO-8601 timestamp

    Vitals(double bpS = 120, double bpD = 80, double hr = 72,
           double temp = 37.0, double spo2 = 98.0, double rr = 16,
           const std::string& ts = "")
        : bloodPressureSystolic(bpS), bloodPressureDiastolic(bpD),
          heartRate(hr), temperature(temp), oxygenSaturation(spo2),
          respiratoryRate(rr), recordedAt(ts) {}

    void display() const {
        std::cout << "  BP: " << bloodPressureSystolic << "/" << bloodPressureDiastolic
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

inline std::string riskLevelToString(RiskLevel r) {
    switch (r) {
        case RiskLevel::LOW:    return "LOW";
        case RiskLevel::MEDIUM: return "MEDIUM";
        case RiskLevel::HIGH:   return "HIGH";
    }
    return "UNKNOWN";
}

// ============================================================
// RiskScore — COMPOSITION: owned by TriageRecord
// Calculates risk based on vitals thresholds
// ============================================================
class RiskScore {
private:
    int score;          // 0–100
    RiskLevel level;

    // Thresholds (encapsulated)
    static constexpr int LOW_THRESHOLD    = 30;
    static constexpr int MEDIUM_THRESHOLD = 60;

public:
    RiskScore() : score(0), level(RiskLevel::LOW) {}

    // Core scoring logic based on clinical thresholds
    void calculate(const Vitals& v) {
        score = 0;

        // Blood pressure
        if (v.bloodPressureSystolic >= 180 || v.bloodPressureSystolic < 80) score += 25;
        else if (v.bloodPressureSystolic >= 140 || v.bloodPressureSystolic < 90) score += 15;

        // Heart rate
        if (v.heartRate > 130 || v.heartRate < 40) score += 25;
        else if (v.heartRate > 100 || v.heartRate < 60) score += 10;

        // Temperature
        if (v.temperature >= 39.5 || v.temperature < 35.0) score += 20;
        else if (v.temperature >= 38.5) score += 10;

        // Oxygen saturation
        if (v.oxygenSaturation < 90) score += 25;
        else if (v.oxygenSaturation < 95) score += 10;

        // Respiratory rate
        if (v.respiratoryRate > 30 || v.respiratoryRate < 8) score += 5;

        // Cap at 100
        if (score > 100) score = 100;

        // Assign level
        if (score >= MEDIUM_THRESHOLD) level = RiskLevel::HIGH;
        else if (score >= LOW_THRESHOLD) level = RiskLevel::MEDIUM;
        else level = RiskLevel::LOW;
    }

    // Accessors
    int       getScore() const { return score; }
    RiskLevel getLevel() const { return level; }
    std::string getLevelString() const { return riskLevelToString(level); }

    void display() const {
        std::cout << "  Risk Score : " << score << "/100"
                  << " | Level: " << getLevelString() << "\n";
    }
};

// ============================================================
// TriageRecord — COMPOSITION: has Vitals and RiskScore
// ============================================================
struct TriageRecord {
    std::string patientId;
    std::string patientName;
    Vitals vitals;
    RiskScore riskScore;
    std::string arrivalTime;       // ISO-8601
    std::string treatmentStartTime;
    std::string triageNote;
    bool        flaggedAbnormal;

    TriageRecord() : flaggedAbnormal(false) {}

    TriageRecord(const std::string& pid, const std::string& name,
                 const Vitals& v, const std::string& arrival)
        : patientId(pid), patientName(name), vitals(v),
          arrivalTime(arrival), flaggedAbnormal(false) {
        riskScore.calculate(v);
    }

    // Response time in minutes (naive string diff not used — store as minutes directly)
    int responseTimeMinutes = 0;

    void display() const {
        std::cout << "=== Triage Record ===\n";
        std::cout << "  Patient  : " << patientName << " (" << patientId << ")\n";
        std::cout << "  Arrival  : " << arrivalTime << "\n";
        std::cout << "  Note     : " << triageNote << "\n";
        std::cout << "  Flagged  : " << (flaggedAbnormal ? "YES" : "No") << "\n";
        vitals.display();
        riskScore.display();
        std::cout << "====================\n";
    }
};

// ============================================================
// TriageManager
// Manages triage records, moving averages, flagging, analytics
// ============================================================
class TriageManager {
private:
    std::vector<TriageRecord> records;
    NotificationSubject*      notifier;  // for HIGH_RISK events
    int                       windowN;   // moving average window

    const std::string TRIAGE_FILE = "triage_records.txt";

public:
    explicit TriageManager(NotificationSubject* n = nullptr, int window = 3)
        : notifier(n), windowN(window) {
        loadFromFile();
    }

    // Add a new triage record, auto-calculate risk, flag if needed
    void addRecord(TriageRecord& rec) {
        rec.riskScore.calculate(rec.vitals);
        rec.flaggedAbnormal = isFlaggedAbnormal(rec);

        records.push_back(rec);
        saveToFile();

        std::cout << "\n[TRIAGE] Record added for " << rec.patientName << "\n";
        rec.display();

        // Fire observer event if HIGH risk
        if (rec.riskScore.getLevel() == RiskLevel::HIGH && notifier) {
            std::string detail = "Patient " + rec.patientName +
                                 " flagged HIGH RISK. Score: " +
                                 std::to_string(rec.riskScore.getScore());
            notifier->notifyObservers("HIGH_RISK_TRIAGE",
                                      "staff@emhr.gov.jm", detail);
        }
    }

    // ---- Moving Average ----
    // Returns moving average of heart rate over last N visits for a patient
    double movingAverageHeartRate(const std::string& patientId) const {
        std::vector<double> vals;
        for (auto it = records.rbegin(); it != records.rend(); ++it) {
            if (it->patientId == patientId) {
                vals.push_back(it->vitals.heartRate);
                if ((int)vals.size() == windowN) break;
            }
        }
        if (vals.empty()) return 0.0;
        return std::accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
    }

    double movingAverageTemperature(const std::string& patientId) const {
        std::vector<double> vals;
        for (auto it = records.rbegin(); it != records.rend(); ++it) {
            if (it->patientId == patientId) {
                vals.push_back(it->vitals.temperature);
                if ((int)vals.size() == windowN) break;
            }
        }
        if (vals.empty()) return 0.0;
        return std::accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
    }

    // ---- Flag abnormal: deviation > 20% from moving average ----
    bool isFlaggedAbnormal(const TriageRecord& rec) const {
        double avgHR = movingAverageHeartRate(rec.patientId);
        if (avgHR > 0) {
            double deviation = std::abs(rec.vitals.heartRate - avgHR) / avgHR;
            if (deviation > 0.20) return true;
        }
        double avgTemp = movingAverageTemperature(rec.patientId);
        if (avgTemp > 0) {
            double deviation = std::abs(rec.vitals.temperature - avgTemp) / avgTemp;
            if (deviation > 0.03) return true;  // 3% for temp (smaller scale)
        }
        return false;
    }

    // ---- Analytics: response time vs outcomes ----
    void analyzeResponseTimes() const {
        if (records.empty()) {
            std::cout << "[ANALYTICS] No triage records to analyze.\n";
            return;
        }

        int highRiskCount = 0, totalResponse = 0, countWithResponse = 0;
        for (const auto& r : records) {
            if (r.riskScore.getLevel() == RiskLevel::HIGH) highRiskCount++;
            if (r.responseTimeMinutes > 0) {
                totalResponse += r.responseTimeMinutes;
                countWithResponse++;
            }
        }

        double avgResponse = countWithResponse > 0
                             ? (double)totalResponse / countWithResponse : 0;

        std::cout << "\n===== TRIAGE ANALYTICS =====\n";
        std::cout << "  Total Records     : " << records.size()       << "\n";
        std::cout << "  High Risk Cases   : " << highRiskCount         << "\n";
        std::cout << "  Avg Response Time : " << avgResponse << " min\n";
        if (avgResponse <= 10)
            std::cout << "  Efficiency        : GOOD (<=10 min avg)\n";
        else if (avgResponse <= 20)
            std::cout << "  Efficiency        : MODERATE (10-20 min avg)\n";
        else
            std::cout << "  Efficiency        : POOR (>20 min avg - review staffing)\n";
        std::cout << "============================\n";
    }

    // Count of high-risk records (used by metrics/reports)
    int countHighRisk() const {
        int c = 0;
        for (const auto& r : records) {
            if (r.riskScore.getLevel() == RiskLevel::HIGH) c++;
        }
        return c;
    }

    int totalRecords() const { return (int)records.size(); }

    void displayAll() const {
        std::cout << "\n===== ALL TRIAGE RECORDS =====\n";
        if (records.empty()) { std::cout << "  No records.\n"; return; }
        for (const auto& r : records) r.display();
    }

    // ---- FILE PERSISTENCE ----
    void saveToFile() const {
        std::ofstream file(TRIAGE_FILE);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Cannot write triage_records.txt\n";
            return;
        }
        for (const auto& r : records) {
            file << r.patientId         << "|"
                 << r.patientName       << "|"
                 << r.vitals.bloodPressureSystolic  << "|"
                 << r.vitals.bloodPressureDiastolic << "|"
                 << r.vitals.heartRate   << "|"
                 << r.vitals.temperature << "|"
                 << r.vitals.oxygenSaturation << "|"
                 << r.vitals.respiratoryRate  << "|"
                 << r.arrivalTime        << "|"
                 << r.responseTimeMinutes << "|"
                 << r.riskScore.getScore() << "|"
                 << r.riskScore.getLevelString() << "|"
                 << (r.flaggedAbnormal ? "1" : "0") << "|"
                 << r.triageNote << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        std::ifstream file(TRIAGE_FILE);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::string token;
            std::vector<std::string> parts;
            while (std::getline(ss, token, '|')) parts.push_back(token);
            if (parts.size() < 14) continue;

            TriageRecord r;
            r.patientId   = parts[0];
            r.patientName = parts[1];
            r.vitals = Vitals(
                std::stod(parts[2]), std::stod(parts[3]),
                std::stod(parts[4]), std::stod(parts[5]),
                std::stod(parts[6]), std::stod(parts[7])
            );
            r.arrivalTime           = parts[8];
            r.responseTimeMinutes   = std::stoi(parts[9]);
            r.flaggedAbnormal       = (parts[12] == "1");
            r.triageNote            = parts[13];
            r.riskScore.calculate(r.vitals);
            records.push_back(r);
        }
        file.close();
    }
};

#endif // TRIAGE_H
