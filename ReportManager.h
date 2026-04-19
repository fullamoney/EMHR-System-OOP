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

// ============================================================
// MetricsCounter — ENCAPSULATION of all system counters
// Tracks important numerical activity in the system
// ============================================================
class MetricsCounter {
private:
    int appointmentsBooked;
    int appointmentsCancelled;
    int appointmentsCompleted;
    int appointmentsNoShow;
    int schedulingConflicts;
    int highRiskTriageCases;
    int mediumRiskTriageCases;
    int lowRiskTriageCases;
    int notificationsSent;
    int billingTransactions;
    double totalRevenue;

    const std::string METRICS_FILE = "metrics.txt";

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

    // ---- Mutators (increment counters) ----
    void recordAppointmentBooked()    { appointmentsBooked++;    saveToFile(); }
    void recordAppointmentCancelled() { appointmentsCancelled++; saveToFile(); }
    void recordAppointmentCompleted() { appointmentsCompleted++; saveToFile(); }
    void recordAppointmentNoShow()    { appointmentsNoShow++;    saveToFile(); }
    void recordSchedulingConflict()   { schedulingConflicts++;   saveToFile(); }
    void recordNotificationSent()     { notificationsSent++;     saveToFile(); }

    void recordTriageCase(RiskLevel level) {
        if      (level == RiskLevel::HIGH)   highRiskTriageCases++;
        else if (level == RiskLevel::MEDIUM) mediumRiskTriageCases++;
        else                                  lowRiskTriageCases++;
        saveToFile();
    }

    void recordBillingTransaction(double amount) {
        billingTransactions++;
        totalRevenue += amount;
        saveToFile();
    }

    // ---- Accessors ----
    int    getAppointmentsBooked()    const { return appointmentsBooked;    }
    int    getSchedulingConflicts()   const { return schedulingConflicts;   }
    int    getHighRiskTriageCases()   const { return highRiskTriageCases;   }
    int    getMediumRiskTriageCases() const { return mediumRiskTriageCases; }
    int    getLowRiskTriageCases()    const { return lowRiskTriageCases;    }
    int    getNotificationsSent()     const { return notificationsSent;     }
    int    getBillingTransactions()   const { return billingTransactions;   }
    double getTotalRevenue()          const { return totalRevenue;          }

    // ---- Display all counters ----
    void displayMetrics() const {
        std::cout << "\n===== SYSTEM METRICS =====\n";
        std::cout << std::left << std::setw(30) << "  Appointments Booked"
                  << ": " << appointmentsBooked << "\n";
        std::cout << std::setw(30) << "  Appointments Cancelled"
                  << ": " << appointmentsCancelled << "\n";
        std::cout << std::setw(30) << "  Appointments Completed"
                  << ": " << appointmentsCompleted << "\n";
        std::cout << std::setw(30) << "  No-Shows"
                  << ": " << appointmentsNoShow << "\n";
        std::cout << std::setw(30) << "  Scheduling Conflicts"
                  << ": " << schedulingConflicts << "\n";
        std::cout << std::setw(30) << "  High Risk Triage Cases"
                  << ": " << highRiskTriageCases << "\n";
        std::cout << std::setw(30) << "  Medium Risk Triage Cases"
                  << ": " << mediumRiskTriageCases << "\n";
        std::cout << std::setw(30) << "  Low Risk Triage Cases"
                  << ": " << lowRiskTriageCases << "\n";
        std::cout << std::setw(30) << "  Notifications Sent"
                  << ": " << notificationsSent << "\n";
        std::cout << std::setw(30) << "  Billing Transactions"
                  << ": " << billingTransactions << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << std::setw(30) << "  Total Revenue"
                  << ": JMD " << totalRevenue << "\n";
        std::cout << "==========================\n";
    }

    // ---- FILE PERSISTENCE ----
    void saveToFile() const {
        std::ofstream file(METRICS_FILE);
        if (!file.is_open()) { std::cerr << "[WARN] Cannot save metrics.\n"; return; }
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
        std::ifstream file(METRICS_FILE);
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
// ReportManager
// Generates weekly summary reports to console and file
// Uses MetricsCounter and TriageManager data
// ============================================================
class ReportManager {
private:
    MetricsCounter& metrics;
    TriageManager&  triageManager;
    FileManager&    fileManager;

    const std::string REPORTS_DIR = "weekly_report_";

    std::string getWeekLabel() const {
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(t, "%Y-W%U");  // e.g. 2026-W15
        return oss.str();
    }

    std::string getCurrentDateTime() const {
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(t, "%Y-%m-%dT%H:%M:%S");
        return oss.str();
    }

public:
    // COMPOSITION: ReportManager uses Metrics, Triage, and File references
    ReportManager(MetricsCounter& m, TriageManager& tm, FileManager& fm)
        : metrics(m), triageManager(tm), fileManager(fm) {}

    // ---- Generate weekly summary report ----
    void generateWeeklyReport() const {
        std::string week     = getWeekLabel();
        std::string filename = REPORTS_DIR + week + ".txt";
        std::string now      = getCurrentDateTime();

        // Build report lines
        std::ostringstream report;
        report << "=======================================================\n";
        report << "   EASTERN MEDICAL HEALTH REGION (EMHR)\n";
        report << "   WEEKLY SUMMARY REPORT\n";
        report << "   Week     : " << week << "\n";
        report << "   Generated: " << now  << "\n";
        report << "=======================================================\n\n";

        report << "--- APPOINTMENTS ---\n";
        report << "  Booked               : " << metrics.getAppointmentsBooked()    << "\n";
        report << "  Scheduling Conflicts : " << metrics.getSchedulingConflicts()   << "\n\n";

        report << "--- TRIAGE ---\n";
        report << "  High Risk Cases      : " << metrics.getHighRiskTriageCases()   << "\n";
        report << "  Medium Risk Cases    : " << metrics.getMediumRiskTriageCases() << "\n";
        report << "  Low Risk Cases       : " << metrics.getLowRiskTriageCases()    << "\n";
        report << "  Total Triage Records : " << triageManager.totalRecords()       << "\n\n";

        report << "--- NOTIFICATIONS ---\n";
        report << "  Notifications Sent   : " << metrics.getNotificationsSent()     << "\n\n";

        report << "--- BILLING ---\n";
        report << "  Transactions         : " << metrics.getBillingTransactions()   << "\n";
        report << std::fixed << std::setprecision(2);
        report << "  Total Revenue (JMD)  : " << metrics.getTotalRevenue()          << "\n\n";

        // Triage efficiency analysis
        report << "--- TRIAGE EFFICIENCY SUMMARY ---\n";
        int highRisk = metrics.getHighRiskTriageCases();
        int total    = triageManager.totalRecords();
        if (total > 0) {
            double highRiskPct = (double)highRisk / total * 100.0;
            report << std::fixed << std::setprecision(1);
            report << "  High Risk %          : " << highRiskPct << "%\n";
            if (highRiskPct > 30)
                report << "  Alert: High-risk rate above 30% - escalate staffing review.\n";
            else
                report << "  Status: Within normal high-risk range.\n";
        } else {
            report << "  No triage records for this period.\n";
        }

        // Conflict rate
        int booked    = metrics.getAppointmentsBooked();
        int conflicts = metrics.getSchedulingConflicts();
        if (booked > 0) {
            double conflictRate = (double)conflicts / booked * 100.0;
            report << "\n--- SCHEDULING CONFLICT RATE ---\n";
            report << std::fixed << std::setprecision(1);
            report << "  Conflict Rate        : " << conflictRate << "%\n";
            if (conflictRate > 5.0)
                report << "  Alert: Conflict rate > 5% - review scheduling rules.\n";
        }

        report << "\n=======================================================\n";
        report << "             END OF REPORT\n";
        report << "=======================================================\n";

        std::string reportStr = report.str();

        // Print to console
        std::cout << reportStr;

        // Save to file (FILE PERSISTENCE)
        std::ofstream file(filename);
        if (file.is_open()) {
            file << reportStr;
            file.close();
            std::cout << "\n[REPORT] Saved to: " << filename << "\n";
        } else {
            std::cerr << "[ERROR] Could not save report to file.\n";
        }

        // Append to cumulative reports log
        std::ofstream logFile("all_reports.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << reportStr << "\n\n";
            logFile.close();
        }
    }

    // ---- Display just the metrics dashboard ----
    void displayDashboard() const {
        metrics.displayMetrics();
        triageManager.analyzeResponseTimes();
    }
};

#endif // REPORT_MANAGER_H
