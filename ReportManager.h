// ============================================================
// File: ReportManager.h
// Description: Metrics tracking and weekly summary reports
// Project: EMHR Healthcare System - Semester 2 AY2025/2026
// ============================================================

#ifndef REPORT_MANAGER_H
#define REPORT_MANAGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "FileManager.h"
#include "Triage.h"

using namespace std;

// ============================================================
// MetricsCounter — CLASS
// ENCAPSULATION: all counters are private
// Tracks all numerical activity in the system
// ============================================================
class MetricsCounter {
private:
    int    appointmentsBooked;
    int    appointmentsCancelled;
    int    appointmentsCompleted;
    int    appointmentsNoShow;
    int    schedulingConflicts;
    int    highRiskTriageCases;
    int    mediumRiskTriageCases;
    int    lowRiskTriageCases;
    int    notificationsSent;
    int    billingTransactions;
    double totalRevenue;

    const string METRICS_FILE = "metrics.txt";

public:
    MetricsCounter()
        : appointmentsBooked(0), appointmentsCancelled(0),
          appointmentsCompleted(0), appointmentsNoShow(0),
          schedulingConflicts(0), highRiskTriageCases(0),
          mediumRiskTriageCases(0), lowRiskTriageCases(0),
          notificationsSent(0), billingTransactions(0),
          totalRevenue(0.0) {
        loadFromFile();
    }

    // Mutators - increment counters with validation
    void recordAppointmentBooked()    { appointmentsBooked++;    saveToFile(); }
    void recordAppointmentCancelled() { appointmentsCancelled++; saveToFile(); }
    void recordAppointmentCompleted() { appointmentsCompleted++; saveToFile(); }
    void recordAppointmentNoShow()    { appointmentsNoShow++;    saveToFile(); }
    void recordSchedulingConflict()   { schedulingConflicts++;   saveToFile(); }
    void recordNotificationSent()     { notificationsSent++;     saveToFile(); }

    void recordTriageCase(RiskLevel level) {
        if      (level == RiskLevel::HIGH)   highRiskTriageCases++;
        else if (level == RiskLevel::MEDIUM) mediumRiskTriageCases++;
        else                                 lowRiskTriageCases++;
        saveToFile();
    }

    void recordBillingTransaction(double amount) {
        if (amount < 0) return;
        billingTransactions++;
        totalRevenue += amount;
        saveToFile();
    }

    // Accessors
    int    getAppointmentsBooked()    const { return appointmentsBooked;    }
    int    getAppointmentsCancelled() const { return appointmentsCancelled; }
    int    getAppointmentsCompleted() const { return appointmentsCompleted; }
    int    getAppointmentsNoShow()    const { return appointmentsNoShow;    }
    int    getSchedulingConflicts()   const { return schedulingConflicts;   }
    int    getHighRiskTriageCases()   const { return highRiskTriageCases;   }
    int    getMediumRiskTriageCases() const { return mediumRiskTriageCases; }
    int    getLowRiskTriageCases()    const { return lowRiskTriageCases;    }
    int    getNotificationsSent()     const { return notificationsSent;     }
    int    getBillingTransactions()   const { return billingTransactions;   }
    double getTotalRevenue()          const { return totalRevenue;          }

    void displayMetrics() const {
        cout << "\n===== SYSTEM METRICS =====\n";
        cout << left << setw(30) << "  Appointments Booked"
             << ": " << appointmentsBooked    << "\n";
        cout << setw(30) << "  Appointments Cancelled"
             << ": " << appointmentsCancelled << "\n";
        cout << setw(30) << "  Appointments Completed"
             << ": " << appointmentsCompleted << "\n";
        cout << setw(30) << "  No-Shows"
             << ": " << appointmentsNoShow    << "\n";
        cout << setw(30) << "  Scheduling Conflicts"
             << ": " << schedulingConflicts   << "\n";
        cout << setw(30) << "  High Risk Triage Cases"
             << ": " << highRiskTriageCases   << "\n";
        cout << setw(30) << "  Medium Risk Triage Cases"
             << ": " << mediumRiskTriageCases << "\n";
        cout << setw(30) << "  Low Risk Triage Cases"
             << ": " << lowRiskTriageCases    << "\n";
        cout << setw(30) << "  Notifications Sent"
             << ": " << notificationsSent     << "\n";
        cout << setw(30) << "  Billing Transactions"
             << ": " << billingTransactions   << "\n";
        cout << fixed << setprecision(2);
        cout << setw(30) << "  Total Revenue"
             << ": JMD " << totalRevenue      << "\n";
        cout << "==========================\n";
    }

    void saveToFile() const {
        ofstream file(METRICS_FILE);
        if (!file.is_open()) { cerr << "[WARN] Cannot save metrics.\n"; return; }
        file << appointmentsBooked    << "\n"
             << appointmentsCancelled << "\n"
             << appointmentsCompleted << "\n"
             << appointmentsNoShow    << "\n"
             << schedulingConflicts   << "\n"
             << highRiskTriageCases   << "\n"
             << mediumRiskTriageCases << "\n"
             << lowRiskTriageCases    << "\n"
             << notificationsSent     << "\n"
             << billingTransactions   << "\n"
             << totalRevenue          << "\n";
        file.close();
    }

    void loadFromFile() {
        ifstream file(METRICS_FILE);
        if (!file.is_open()) return;
        file >> appointmentsBooked    >> appointmentsCancelled
             >> appointmentsCompleted >> appointmentsNoShow
             >> schedulingConflicts   >> highRiskTriageCases
             >> mediumRiskTriageCases >> lowRiskTriageCases
             >> notificationsSent     >> billingTransactions
             >> totalRevenue;
        file.close();
    }
};

// ============================================================
// ReportManager — CLASS
// COMPOSITION: uses MetricsCounter, TriageManager, FileManager
// Generates weekly summary reports to console and file
// ============================================================
class ReportManager {
private:
    MetricsCounter& metrics;
    TriageManager&  triageManager;
    FileManager&    fileManager;

    string getWeekLabel() const {
        time_t now = time(nullptr);
        tm* t = localtime(&now);
        ostringstream oss;
        oss << put_time(t, "%Y-W%U");
        return oss.str();
    }

    string getCurrentDateTime() const {
        time_t now = time(nullptr);
        tm* t = localtime(&now);
        ostringstream oss;
        oss << put_time(t, "%Y-%m-%dT%H:%M:%S");
        return oss.str();
    }

public:
    ReportManager(MetricsCounter& m, TriageManager& tm, FileManager& fm)
        : metrics(m), triageManager(tm), fileManager(fm) {}

    void generateWeeklyReport() const {
        string week     = getWeekLabel();
        string filename = "weekly_report_" + week + ".txt";
        string now      = getCurrentDateTime();

        ostringstream report;
        report << "=======================================================\n";
        report << "   EASTERN MEDICAL HEALTH REGION (EMHR)\n";
        report << "   WEEKLY SUMMARY REPORT\n";
        report << "   Week     : " << week << "\n";
        report << "   Generated: " << now  << "\n";
        report << "=======================================================\n\n";

        report << "--- APPOINTMENTS ---\n";
        report << "  Booked               : " << metrics.getAppointmentsBooked()  << "\n";
        report << "  Scheduling Conflicts : " << metrics.getSchedulingConflicts() << "\n\n";

        report << "--- TRIAGE ---\n";
        report << "  High Risk Cases      : " << metrics.getHighRiskTriageCases()   << "\n";
        report << "  Medium Risk Cases    : " << metrics.getMediumRiskTriageCases() << "\n";
        report << "  Low Risk Cases       : " << metrics.getLowRiskTriageCases()    << "\n";
        report << "  Total Triage Records : " << triageManager.totalRecords()       << "\n\n";

        report << "--- NOTIFICATIONS ---\n";
        report << "  Notifications Sent   : " << metrics.getNotificationsSent()    << "\n\n";

        report << "--- BILLING ---\n";
        report << "  Transactions         : " << metrics.getBillingTransactions()  << "\n";
        report << fixed << setprecision(2);
        report << "  Total Revenue (JMD)  : " << metrics.getTotalRevenue()         << "\n\n";

        int highRisk = metrics.getHighRiskTriageCases();
        int total    = triageManager.totalRecords();
        report << "--- TRIAGE EFFICIENCY SUMMARY ---\n";
        if (total > 0) {
            double highRiskPct = (double)highRisk / total * 100.0;
            report << fixed << setprecision(1);
            report << "  High Risk %          : " << highRiskPct << "%\n";
            if (highRiskPct > 30)
                report << "  Alert: High-risk rate above 30% - escalate staffing review.\n";
            else
                report << "  Status: Within normal high-risk range.\n";
        } else {
            report << "  No triage records for this period.\n";
        }

        int booked    = metrics.getAppointmentsBooked();
        int conflicts = metrics.getSchedulingConflicts();
        if (booked > 0) {
            double conflictRate = (double)conflicts / booked * 100.0;
            report << "\n--- SCHEDULING CONFLICT RATE ---\n";
            report << fixed << setprecision(1);
            report << "  Conflict Rate        : " << conflictRate << "%\n";
            if (conflictRate > 5.0)
                report << "  Alert: Conflict rate > 5% - review scheduling rules.\n";
        }

        report << "\n=======================================================\n";
        report << "             END OF REPORT\n";
        report << "=======================================================\n";

        string reportStr = report.str();
        cout << reportStr;

        ofstream file(filename);
        if (file.is_open()) {
            file << reportStr;
            file.close();
            cout << "\n[REPORT] Saved to: " << filename << "\n";
        } else {
            cerr << "[ERROR] Could not save report to file.\n";
        }

        ofstream logFile("all_reports.txt", ios::app);
        if (logFile.is_open()) {
            logFile << reportStr << "\n\n";
            logFile.close();
        }
    }

    void displayDashboard() const {
        metrics.displayMetrics();
        triageManager.analyzeResponseTimes();
    }
};

#endif // REPORT_MANAGER_H

};

#endif // REPORT_MANAGER_H
