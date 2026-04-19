// ============================================================
// File: main_notifications_triage.cpp
// Description: Demo / integration test for the Notifications,
//              Triage, File Persistence, and Reports modules
// Project: EMHR Healthcare System - Semester 2 AY2025/2026
// ============================================================
// HOW TO COMPILE (g++):
//   g++ -std=c++17 main_notifications_triage.cpp -o emhr_demo
// HOW TO RUN:
//   ./emhr_demo
// ============================================================

#include <iostream>
#include <stdexcept>
#include "NotificationService.h"
#include "EmailNotification.h"
#include "Triage.h"
#include "FileManager.h"
#include "ReportManager.h"

int main() {
    std::cout << "=======================================================\n";
    std::cout << "   EMHR SYSTEM - Notifications, Triage & Reports Demo\n";
    std::cout << "=======================================================\n\n";

    // ======================================================
    // 1. NOTIFICATIONS SETUP
    //    Observer pattern: EmailNotification and SMSNotification
    //    subscribe to the NotificationSubject
    // ======================================================
    std::cout << "--- [1] Setting up Notification Services ---\n";

    NotificationSubject publisher;           // Observable (subject)

    EmailNotification emailSvc;              // Email observer
    DigicelAdapter    digicelAdapter;        // SMS vendor adapter (Digicel)
    FlowAdapter       flowAdapter;           // SMS vendor adapter (Flow)
    SMSNotification   smsSvc(&digicelAdapter); // SMS observer using Digicel

    // Register observers with the publisher
    publisher.addObserver(&emailSvc);
    publisher.addObserver(&smsSvc);
    std::cout << "  Email & SMS observers registered.\n\n";

    // ---- Send individual notifications ----
    try {
        emailSvc.sendNotification(
            "john.brown@gmail.com",
            "Appointment Confirmation - EMHR",
            "Your appointment is scheduled for 2026-04-22T10:00:00 with Dr. Smith."
        );

        smsSvc.sendNotification(
            "+1876-555-1234",
            "Appt Confirmed",
            "Appt on 2026-04-22 at 10:00AM with Dr. Smith."
        );

        // Switch SMS vendor to Flow mid-session (Strategy-like adapter swap)
        smsSvc.setVendorAdapter(&flowAdapter);
        smsSvc.sendNotification(
            "+1876-555-9999",
            "Payment Due",
            "You have an outstanding balance of JMD 3,500. Please pay at your next visit."
        );

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Notification: " << e.what() << "\n";
    }

    // ======================================================
    // 2. TRIAGE SETUP
    //    RiskScore calculation, moving average, flagging
    // ======================================================
    std::cout << "\n--- [2] Triage & Analytics ---\n";

    TriageManager triageManager(&publisher, 3);  // window of 3 visits

    // Patient 1 - LOW risk
    {
        Vitals v1(118, 78, 72, 36.8, 98.5, 16, "2026-04-19T08:00:00");
        TriageRecord r1("P001", "Mary Brown", v1, "2026-04-19T08:00:00");
        r1.triageNote = "Routine check-up.";
        r1.responseTimeMinutes = 5;
        triageManager.addRecord(r1);
    }

    // Patient 2 - MEDIUM risk
    {
        Vitals v2(145, 95, 105, 38.2, 95.0, 22, "2026-04-19T08:30:00");
        TriageRecord r2("P002", "Devon Clarke", v2, "2026-04-19T08:30:00");
        r2.triageNote = "Elevated BP and heart rate on arrival.";
        r2.responseTimeMinutes = 12;
        triageManager.addRecord(r2);
    }

    // Patient 3 - HIGH risk (will trigger observer notification)
    {
        Vitals v3(185, 110, 135, 39.8, 88.0, 28, "2026-04-19T09:00:00");
        TriageRecord r3("P003", "Sandra White", v3, "2026-04-19T09:00:00");
        r3.triageNote = "Patient in acute distress.";
        r3.responseTimeMinutes = 3;
        triageManager.addRecord(r3);
    }

    // Second visit for P001 - check moving average flag
    {
        Vitals v4(120, 80, 130, 37.0, 97.0, 17, "2026-04-19T10:00:00");
        TriageRecord r4("P001", "Mary Brown", v4, "2026-04-19T10:00:00");
        r4.triageNote = "Follow-up. HR spike vs last visit.";
        r4.responseTimeMinutes = 4;
        triageManager.addRecord(r4);
    }

    // Moving average for P001
    double avgHR   = triageManager.movingAverageHeartRate("P001");
    double avgTemp = triageManager.movingAverageTemperature("P001");
    std::cout << "\n[ANALYTICS] P001 Moving Avg Heart Rate : " << avgHR   << " bpm\n";
    std::cout << "[ANALYTICS] P001 Moving Avg Temperature: " << avgTemp << " C\n";

    // All triage records
    triageManager.displayAll();
    triageManager.analyzeResponseTimes();

    // ======================================================
    // 3. FILE PERSISTENCE DEMO
    //    Save and reload Patients, Appointments, Billing, EHR
    // ======================================================
    std::cout << "\n--- [3] File Persistence ---\n";

    FileManager fileManager;

    // Patients
    std::vector<PatientRecord> patients = {
        {"P001", "Mary",   "Brown",  "1990-05-14", "F",
         "12 Main St, Kingston", "876-555-0001", "mary.brown@email.com",
         "John Brown", "876-555-0002", "2026-01-10T09:00:00"},
        {"P002", "Devon",  "Clarke", "1985-08-22", "M",
         "7 Hope Rd, Kingston",  "876-555-0003", "devon.c@email.com",
         "Lisa Clarke", "876-555-0004", "2026-02-01T10:30:00"},
        {"P003", "Sandra", "White",  "1972-11-30", "F",
         "45 Park Ave, Saint Thomas", "876-555-0005", "s.white@email.com",
         "Paul White", "876-555-0006", "2026-03-15T08:15:00"},
    };
    fileManager.savePatients(patients);

    std::vector<PatientRecord> loadedPatients = fileManager.loadPatients();
    std::cout << "  Loaded " << loadedPatients.size() << " patient(s) from file.\n";

    // Appointments
    std::vector<AppointmentRecord> appts = {
        {"A001", "P001", "S001", "2026-04-22T10:00:00", "SCHEDULED", "General Check-up", false},
        {"A002", "P002", "S002", "2026-04-22T11:00:00", "SCHEDULED", "Cardiac Review", false},
        {"A003", "P003", "S001", "2026-04-22T10:30:00", "CANCELLED", "Follow-up", true},
    };
    fileManager.saveAppointments(appts);

    std::vector<AppointmentRecord> loadedAppts = fileManager.loadAppointments();
    std::cout << "  Loaded " << loadedAppts.size() << " appointment(s) from file.\n";

    // Billing
    std::vector<BillingRecord> bills = {
        {"B001", "P001", "A001", 5000.00, 0.80, 500.00, 1500.00, "COMBO",  "PAID",    "2026-04-22T12:00:00"},
        {"B002", "P002", "A002", 8000.00, 0.80, 500.00, 2100.00, "CARD",   "PAID",    "2026-04-22T13:00:00"},
        {"B003", "P003", "A003", 3500.00, 0.00, 0.00,   3500.00, "CASH",   "PENDING", "2026-04-22T14:00:00"},
    };
    fileManager.saveBilling(bills);

    // EHR
    std::vector<EHRRecord> ehrRecords = {
        {"EHR001", "P001", "2026-04-22T10:00:00",
         "Hypertension Stage 1", "Lisinopril 10mg daily",
         "Penicillin", "Flu Vaccine 2025-10-01",
         "CBC Normal", "Treatment of chronic condition", "S001"},
        {"EHR002", "P003", "2026-04-19T09:00:00",
         "Acute Respiratory Distress", "Oxygen therapy; Salbutamol",
         "None", "COVID Vaccine 2024-06-01",
         "SpO2 critically low; ECG abnormal",
         "Emergency treatment", "S002"},
    };
    fileManager.saveEHR(ehrRecords);
    std::cout << "  Saved EHR records.\n";

    // Audit log
    fileManager.writeAuditLog("ADMIN01", "CREATE_PATIENT",
                              "Added patient P001", "2026-04-19T09:00:00");
    fileManager.writeAuditLog("ADMIN01", "CANCEL_APPOINTMENT",
                              "Cancelled A003 due to conflict", "2026-04-19T09:05:00");
    fileManager.displayAuditLog();

    // ======================================================
    // 4. METRICS & WEEKLY REPORT
    // ======================================================
    std::cout << "\n--- [4] Metrics & Weekly Report ---\n";

    MetricsCounter metrics;

    // Record activity
    metrics.recordAppointmentBooked();
    metrics.recordAppointmentBooked();
    metrics.recordAppointmentBooked();
    metrics.recordAppointmentCancelled();
    metrics.recordSchedulingConflict();
    metrics.recordTriageCase(RiskLevel::LOW);
    metrics.recordTriageCase(RiskLevel::MEDIUM);
    metrics.recordTriageCase(RiskLevel::HIGH);
    metrics.recordTriageCase(RiskLevel::HIGH);
    metrics.recordNotificationSent();
    metrics.recordNotificationSent();
    metrics.recordNotificationSent();
    metrics.recordBillingTransaction(5000.00);
    metrics.recordBillingTransaction(8000.00);
    metrics.recordBillingTransaction(3500.00);

    ReportManager reportManager(metrics, triageManager, fileManager);

    // Display live dashboard
    reportManager.displayDashboard();

    // Generate and save weekly report
    reportManager.generateWeeklyReport();

    std::cout << "\n=======================================================\n";
    std::cout << "   EMHR Demo Complete. Check generated .txt files.\n";
    std::cout << "=======================================================\n";

    return 0;
}
