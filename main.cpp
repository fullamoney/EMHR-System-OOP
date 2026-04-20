// ============================================================
// File        : main.cpp
// Project     : Eastern Medical Health Region (EMHR) System
// Description : Single entry point. Connects all modules:
//               Users, Patients, Appointments, EHR, Billing,
//               Notifications, Triage, and Reports.
//
// HOW TO COMPILE (run this one command in your terminal):
//   g++ -std=c++17 main.cpp UserManager.cpp User.cpp Admin.cpp Role.cpp Permission.cpp -o emhr
//
// HOW TO RUN:
//   Windows : emhr.exe     (or just double-click it)
//   Mac/Linux: ./emhr
//
// NOTE: Make sure ALL files are in the SAME folder before compiling.
//       Delete the main() from person4_ehr_billing.cpp before compiling.
// ============================================================

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <limits>

// ---- User / RBAC headers (your team's files) ----
#include "UserManager.h"       // UserManager, uses User.h, Admin.h, Role.h, Permission.h

// ---- Patient & Appointments (your team's files) ----
#include "PatientRegistry.h"   // PatientRegistry, pulls in Patient.h, Appointment.h

// ---- Schedule (your team's file) ----
#include "ScheduleManager.h"   // ScheduleManager

// ---- EHR & Billing (your team's file - person4) ----
// IMPORTANT: Before compiling, open person4_ehr_billing.cpp and
// DELETE the int main() { ... } block at the very bottom of that file.
// Leave all the class definitions above it untouched.
#include "person4_ehr_billing.cpp"

// ---- Notification, Triage, File, Reports (my files) ----
#include "NotificationService.h"   // NotificationService, NotificationSubject, NotificationObserver
#include "EmailNotification.h"     // EmailNotification, SMSNotification, DigicelAdapter, FlowAdapter
#include "Triage.h"                // Vitals, RiskScore, RiskLevel, TriageRecord, TriageManager
#include "FileManager.h"           // FileManager
#include "ReportManager.h"         // MetricsCounter, ReportManager

using namespace std;

// ============================================================
// Utility helpers
// ============================================================
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printHeader(const string& title) {
    cout << "\n============================================\n";
    cout << "  EMHR  |  " << title << "\n";
    cout << "============================================\n";
}

// Safe integer input — keeps asking until user types a number
int getInt(const string& prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cout << "  [ERROR] Enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Safe string input
string getStr(const string& prompt) {
    string val;
    cout << prompt;
    getline(cin, val);
    return val;
}

// Safe double input
double getDbl(const string& prompt) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cout << "  [ERROR] Enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ============================================================
// 1. USER MANAGEMENT MENU
//    Uses: UserManager (UserManager.h)
//    Method names match UserManager.h exactly (PascalCase)
// ============================================================
void userMenu(UserManager& um, shared_ptr<User>& loggedIn) {
    bool back = false;
    while (!back) {
        printHeader("USER MANAGEMENT");
        cout << "  1. Add User\n"
             << "  2. Update User\n"
             << "  3. View User\n"
             << "  4. View All Users\n"
             << "  5. Delete User\n"
             << "  6. View Audit Log\n"
             << "  0. Back\n";
        int ch = getInt("\n  Choice: ");
        switch (ch) {
            case 1: {
                string uname = getStr("  Username            : ");
                string pass  = getStr("  Password            : ");
                string fname = getStr("  Full Name           : ");
                string email = getStr("  Email               : ");
                string phone = getStr("  Phone               : ");
                string role  = getStr("  Role (Admin/Manager/User): ");
                string dept  = getStr("  Department (optional): ");
                // AddUser signature from UserManager.h:
                // bool AddUser(actorId, username, plainPassword, fullName, email, phone, roleChoice, dept)
                um.AddUser(loggedIn->GetUserId(), uname, pass, fname, email, phone, role, dept);
                break;
            }
            case 2: {
                string tid = getStr("  Target User ID: ");
                // UpdateUser prompts for fields internally per UserManager.cpp
                um.UpdateUser(loggedIn->GetUserId(), tid);
                break;
            }
            case 3: {
                string tid = getStr("  User ID: ");
                um.ViewUser(loggedIn->GetUserId(), tid);
                break;
            }
            case 4:
                um.ViewAllUsers(loggedIn->GetUserId());
                break;
            case 5: {
                string tid = getStr("  User ID to delete: ");
                um.DeleteUser(loggedIn->GetUserId(), tid);
                break;
            }
            case 6:
                um.ViewAuditLog(loggedIn->GetUserId());
                break;
            case 0:
                back = true;
                break;
            default:
                cout << "  [ERROR] Invalid option.\n";
        }
        if (!back) pauseScreen();
    }
}

// ============================================================
// 2. PATIENT REGISTRY MENU
//    Uses: PatientRegistry (PatientRegistry.h)
//    Method names: createPatient, readPatient, updatePatient,
//                  deletePatient, viewAll  — all camelCase
//                  (matches PatientRegistry.h exactly)
// ============================================================
void patientMenu(PatientRegistry& registry) {
    bool back = false;
    while (!back) {
        printHeader("PATIENT REGISTRY");
        cout << "  1. Add Patient\n"
             << "  2. View Patient\n"
             << "  3. Update Patient\n"
             << "  4. Delete Patient\n"
             << "  5. View All Patients\n"
             << "  0. Back\n";
        int ch = getInt("\n  Choice: ");
        switch (ch) {
            case 1: {
                string name = getStr("  Full Name   : ");
                string addr = getStr("  Address     : ");
                string cont = getStr("  Contact     : ");
                string kin  = getStr("  Next of Kin : ");
                registry.createPatient(name, addr, cont, kin);
                break;
            }
            case 2: {
                int id = getInt("  Patient ID: ");
                Patient* p = registry.readPatient(id);
                if (p) p->display();
                break;
            }
            case 3: {
                int    id   = getInt("  Patient ID      : ");
                string name = getStr("  New Name        : ");
                string addr = getStr("  New Address     : ");
                string cont = getStr("  New Contact     : ");
                string kin  = getStr("  New Next of Kin : ");
                registry.updatePatient(id, name, addr, cont, kin);
                break;
            }
            case 4: {
                int id = getInt("  Patient ID to delete: ");
                registry.deletePatient(id);
                break;
            }
            case 5:
                registry.viewAll();
                break;
            case 0:
                back = true;
                break;
            default:
                cout << "  [ERROR] Invalid option.\n";
        }
        if (!back) pauseScreen();
    }
}

// ============================================================
// 3. APPOINTMENTS MENU
//    Uses: ScheduleManager (ScheduleManager.h)
//    Methods: bookAppointment, updateAppointment,
//             cancelAppointment, viewByPatient, viewAll
// ============================================================
void appointmentMenu(ScheduleManager& scheduler, MetricsCounter& metrics) {
    bool back = false;
    while (!back) {
        printHeader("APPOINTMENTS");
        cout << "  1. Book Appointment\n"
             << "  2. Update Appointment\n"
             << "  3. Cancel Appointment\n"
             << "  4. View By Patient\n"
             << "  5. View All\n"
             << "  0. Back\n";
        int ch = getInt("\n  Choice: ");
        switch (ch) {
            case 1: {
                int    pid   = getInt("  Patient ID                   : ");
                string dt    = getStr("  Date/Time (YYYY-MM-DD HH:MM) : ");
                int    dur   = getInt("  Duration (minutes)           : ");
                string notes = getStr("  Notes                        : ");
                bool ok = scheduler.bookAppointment(pid, dt, dur, notes);
                if (ok)  metrics.recordAppointmentBooked();
                else     metrics.recordSchedulingConflict();
                break;
            }
            case 2: {
                int    id = getInt("  Appointment ID                   : ");
                string dt = getStr("  New Date/Time (YYYY-MM-DD HH:MM) : ");
                scheduler.updateAppointment(id, dt);
                break;
            }
            case 3: {
                int id = getInt("  Appointment ID: ");
                scheduler.cancelAppointment(id);
                metrics.recordAppointmentCancelled();
                break;
            }
            case 4: {
                int pid = getInt("  Patient ID: ");
                scheduler.viewByPatient(pid);
                break;
            }
            case 5:
                scheduler.viewAll();
                break;
            case 0:
                back = true;
                break;
            default:
                cout << "  [ERROR] Invalid option.\n";
        }
        if (!back) pauseScreen();
    }
}

// ============================================================
// 4. EHR MENU
//    Uses: EHRSystem, Vitals, VisitRecord, Prescription,
//          LabResult (all from person4_ehr_billing.cpp)
//    Methods: addRecord, addImmunization, addAllergy,
//             viewPatientEHR
// ============================================================
void ehrMenu(EHRSystem& ehr) {
    bool back = false;
    while (!back) {
        printHeader("ELECTRONIC HEALTH RECORDS");
        cout << "  1. Add Vitals\n"
             << "  2. Add Visit Record\n"
             << "  3. Add Prescription\n"
             << "  4. Add Lab Result\n"
             << "  5. Add Immunization\n"
             << "  6. Add Allergy\n"
             << "  7. View Patient EHR\n"
             << "  0. Back\n";
        int ch = getInt("\n  Choice: ");
        switch (ch) {
            case 1: {
                int    pid  = getInt("  Patient ID          : ");
                string date = getStr("  Date (YYYY-MM-DD)   : ");
                double temp = getDbl("  Temperature (C)     : ");
                int    hr   = getInt("  Heart Rate (bpm)    : ");
                string bp   = getStr("  Blood Pressure      : ");
                double wt   = getDbl("  Weight (kg)         : ");
                double ht   = getDbl("  Height (cm)         : ");
                ehr.addRecord(make_shared<Vitals>(pid, date, temp, hr, bp, wt, ht));
                break;
            }
            case 2: {
                int    pid  = getInt("  Patient ID          : ");
                string date = getStr("  Date (YYYY-MM-DD)   : ");
                string diag = getStr("  Diagnosis           : ");
                string note = getStr("  Doctor Notes        : ");
                string fu   = getStr("  Follow-up Date      : ");
                ehr.addRecord(make_shared<VisitRecord>(pid, date, diag, note, fu));
                break;
            }
            case 3: {
                int    pid  = getInt("  Patient ID          : ");
                string date = getStr("  Date (YYYY-MM-DD)   : ");
                string med  = getStr("  Medication          : ");
                string dose = getStr("  Dosage              : ");
                string freq = getStr("  Frequency           : ");
                string dur  = getStr("  Duration            : ");
                ehr.addRecord(make_shared<Prescription>(pid, date, med, dose, freq, dur));
                break;
            }
            case 4: {
                int    pid  = getInt("  Patient ID          : ");
                string date = getStr("  Date (YYYY-MM-DD)   : ");
                string test = getStr("  Test Name           : ");
                string res  = getStr("  Result              : ");
                string ref  = getStr("  Reference Range     : ");
                string abn  = getStr("  Abnormal? (y/n)     : ");
                ehr.addRecord(make_shared<LabResult>(pid, date, test, res, ref, abn == "y"));
                break;
            }
            case 5: {
                int    pid = getInt("  Patient ID          : ");
                string vac = getStr("  Vaccine             : ");
                string dg  = getStr("  Date Given          : ");
                string nd  = getStr("  Next Due (or blank) : ");
                ehr.addImmunization(pid, vac, dg, nd);
                break;
            }
            case 6: {
                int    pid    = getInt("  Patient ID  : ");
                string alg    = getStr("  Allergen    : ");
                string react  = getStr("  Reaction    : ");
                ehr.addAllergy(pid, alg, react);
                break;
            }
            case 7: {
                int pid = getInt("  Patient ID: ");
                ehr.viewPatientEHR(pid);
                break;
            }
            case 0:
                back = true;
                break;
            default:
                cout << "  [ERROR] Invalid option.\n";
        }
        if (!back) pauseScreen();
    }
}

// ============================================================
// 5. BILLING MENU
//    Uses: BillingSystem, CashPayment, CardPayment,
//          InsurancePayment (person4_ehr_billing.cpp)
//    Methods: createInvoice, payInvoice, viewInvoices
// ============================================================
void billingMenu(BillingSystem& billing, MetricsCounter& metrics) {
    bool back = false;
    while (!back) {
        printHeader("BILLING");
        cout << "  1. Create Invoice\n"
             << "  2. Pay with Cash\n"
             << "  3. Pay with Card\n"
             << "  4. Pay with Insurance + Card\n"
             << "  5. View Patient Invoices\n"
             << "  0. Back\n";
        int ch = getInt("\n  Choice: ");
        switch (ch) {
            case 1: {
                int    pid  = getInt("  Patient ID   : ");
                double amt  = getDbl("  Amount (JMD) : ");
                string desc = getStr("  Description  : ");
                billing.createInvoice(pid, amt, desc);
                break;
            }
            case 2: {
                int    id   = getInt("  Invoice ID    : ");
                double cash = getDbl("  Cash Tendered : ");
                vector<shared_ptr<PaymentStrategy>> s = { make_shared<CashPayment>(cash) };
                billing.payInvoice(id, s);
                metrics.recordBillingTransaction(cash);
                break;
            }
            case 3: {
                int    id    = getInt("  Invoice ID        : ");
                string last4 = getStr("  Card Last 4 Digits: ");
                double amt   = getDbl("  Charge Amount     : ");
                vector<shared_ptr<PaymentStrategy>> s = { make_shared<CardPayment>(last4) };
                billing.payInvoice(id, s);
                metrics.recordBillingTransaction(amt);
                break;
            }
            case 4: {
                int    id    = getInt("  Invoice ID              : ");
                double cov   = getDbl("  Insurance Coverage (0.80): ");
                double ded   = getDbl("  Deductible (JMD)         : ");
                double cap   = getDbl("  Per-Visit Cap (JMD)      : ");
                string last4 = getStr("  Card Last 4 (remainder)  : ");
                double total = getDbl("  Total Invoice Amount     : ");
                vector<shared_ptr<PaymentStrategy>> s = {
                    make_shared<InsurancePayment>(cov, ded, cap),
                    make_shared<CardPayment>(last4)
                };
                billing.payInvoice(id, s);
                metrics.recordBillingTransaction(total);
                break;
            }
            case 5: {
                int pid = getInt("  Patient ID: ");
                billing.viewInvoices(pid);
                break;
            }
            case 0:
                back = true;
                break;
            default:
                cout << "  [ERROR] Invalid option.\n";
        }
        if (!back) pauseScreen();
    }
}

// ============================================================
// 6. NOTIFICATIONS MENU
//    Uses: EmailNotification, SMSNotification
//          (EmailNotification.h)
//    Methods: sendNotification, buildSubject
// ============================================================
void notificationMenu(EmailNotification& emailSvc,
                      SMSNotification&   smsSvc,
                      MetricsCounter&    metrics) {
    bool back = false;
    while (!back) {
        printHeader("NOTIFICATIONS");
        cout << "  1. Send Email\n"
             << "  2. Send SMS\n"
             << "  0. Back\n";
        int ch = getInt("\n  Choice: ");
        switch (ch) {
            case 1: {
                string to   = getStr("  Recipient Email : ");
                cout << "  Type (APPT_BOOKED / LAB_RESULT / PAYMENT_REMINDER / FOLLOW_UP): ";
                string type; getline(cin, type);
                string msg  = getStr("  Message         : ");
                try {
                    emailSvc.sendNotification(to, emailSvc.buildSubject(type), msg);
                    metrics.recordNotificationSent();
                } catch (const exception& e) {
                    cout << "  [ERROR] " << e.what() << "\n";
                }
                break;
            }
            case 2: {
                string to   = getStr("  Recipient Phone : ");
                cout << "  Type (APPT_BOOKED / LAB_RESULT / PAYMENT_REMINDER / FOLLOW_UP): ";
                string type; getline(cin, type);
                string msg  = getStr("  Message         : ");
                try {
                    smsSvc.sendNotification(to, smsSvc.buildSubject(type), msg);
                    metrics.recordNotificationSent();
                } catch (const exception& e) {
                    cout << "  [ERROR] " << e.what() << "\n";
                }
                break;
            }
            case 0:
                back = true;
                break;
            default:
                cout << "  [ERROR] Invalid option.\n";
        }
        if (!back) pauseScreen();
    }
}

// ============================================================
// 7. TRIAGE MENU
//    Uses: TriageManager, TriageRecord, Vitals (Triage.h)
//    Methods: addRecord, displayAll, analyzeResponseTimes,
//             movingAverageHeartRate, movingAverageTemperature
// ============================================================
void triageMenu(TriageManager& triageManager, MetricsCounter& metrics) {
    bool back = false;
    while (!back) {
        printHeader("TRIAGE & ANALYTICS");
        cout << "  1. Add Triage Record\n"
             << "  2. View All Records\n"
             << "  3. Response Time Analytics\n"
             << "  4. Moving Average for Patient\n"
             << "  0. Back\n";
        int ch = getInt("\n  Choice: ");
        switch (ch) {
            case 1: {
                string pid  = getStr("  Patient ID                           : ");
                string name = getStr("  Patient Name                         : ");
                string arr  = getStr("  Arrival Time (YYYY-MM-DDTHH:MM:SS)  : ");
                double bpS  = getDbl("  BP Systolic                          : ");
                double bpD  = getDbl("  BP Diastolic                         : ");
                double hr   = getDbl("  Heart Rate (bpm)                     : ");
                double temp = getDbl("  Temperature (C)                      : ");
                double spo2 = getDbl("  SpO2 (%)                             : ");
                double rr   = getDbl("  Respiratory Rate                     : ");
                int    resp = getInt("  Response Time (minutes)              : ");
                string note = getStr("  Triage Note                          : ");

                // Vitals constructor: (bpS, bpD, hr, temp, spo2, rr, timestamp)
                TriageVitals v(bpS, bpD, hr, temp, spo2, rr, arr);
                // TriageRecord constructor: (patientId, patientName, vitals, arrivalTime)
                TriageRecord rec(pid, name, v, arr);
                triageManager.addRecord(rec);
                // recordTriageCase takes RiskLevel from riskScore
                metrics.recordTriageCase(rec.getRiskScore().getLevel());
                break;
            }
            case 2:
                triageManager.displayAll();
                break;
            case 3:
                triageManager.analyzeResponseTimes();
                break;
            case 4: {
                string pid = getStr("  Patient ID: ");
                cout << "  Moving Avg Heart Rate : "
                     << triageManager.movingAverageHeartRate(pid) << " bpm\n";
                cout << "  Moving Avg Temperature: "
                     << triageManager.movingAverageTemperature(pid) << " C\n";
                break;
            }
            case 0:
                back = true;
                break;
            default:
                cout << "  [ERROR] Invalid option.\n";
        }
        if (!back) pauseScreen();
    }
}

// ============================================================
// 8. REPORTS MENU
//    Uses: ReportManager (ReportManager.h)
//    Methods: displayDashboard, generateWeeklyReport
// ============================================================
void reportsMenu(ReportManager& reportManager) {
    bool back = false;
    while (!back) {
        printHeader("METRICS & REPORTS");
        cout << "  1. Live Metrics Dashboard\n"
             << "  2. Generate Weekly Report\n"
             << "  0. Back\n";
        int ch = getInt("\n  Choice: ");
        switch (ch) {
            case 1:
                reportManager.displayDashboard();
                break;
            case 2:
                reportManager.generateWeeklyReport();
                break;
            case 0:
                back = true;
                break;
            default:
                cout << "  [ERROR] Invalid option.\n";
        }
        if (!back) pauseScreen();
    }
}

// ============================================================
// LOGIN SCREEN
//    Uses: UserManager::Login(username, plainPassword)
//    Returns shared_ptr<User> or nullptr on failure
// ============================================================
shared_ptr<User> loginScreen(UserManager& um) {
    printHeader("LOGIN");
    string uname = getStr("  Username : ");
    string pass  = getStr("  Password : ");
    // Login is defined in UserManager.h as:
    // shared_ptr<User> Login(const string& username, const string& plainPassword)
    return um.Login(uname, pass);
}

// ============================================================
// MAIN MENU
// ============================================================
void mainMenu(shared_ptr<User>&   loggedIn,
              UserManager&        um,
              PatientRegistry&    registry,
              ScheduleManager&    scheduler,
              EHRSystem&          ehr,
              BillingSystem&      billing,
              EmailNotification&  emailSvc,
              SMSNotification&    smsSvc,
              TriageManager&      triageManager,
              ReportManager&      reportManager,
              MetricsCounter&     metrics) {

    bool running = true;
    while (running) {
        clearScreen();
        // GetFullName() matches User.h accessor exactly
        printHeader("MAIN MENU  |  " + loggedIn->GetFullName());
        cout << "  1. User Management\n"
             << "  2. Patient Registry\n"
             << "  3. Appointments\n"
             << "  4. Electronic Health Records\n"
             << "  5. Billing\n"
             << "  6. Notifications\n"
             << "  7. Triage & Analytics\n"
             << "  8. Metrics & Reports\n"
             << "  0. Logout\n";
        int ch = getInt("\n  Choice: ");
        switch (ch) {
            case 1: userMenu(um, loggedIn);                        break;
            case 2: patientMenu(registry);                         break;
            case 3: appointmentMenu(scheduler, metrics);           break;
            case 4: ehrMenu(ehr);                                  break;
            case 5: billingMenu(billing, metrics);                 break;
            case 6: notificationMenu(emailSvc, smsSvc, metrics);   break;
            case 7: triageMenu(triageManager, metrics);            break;
            case 8: reportsMenu(reportManager);                    break;
            case 0: running = false;                               break;
            default:
                cout << "  [ERROR] Invalid option.\n";
                pauseScreen();
                break;
        }
    }
}

// ============================================================
// PROGRAM ENTRY POINT
// ============================================================
int main() {
    clearScreen();
    cout << "\n";
    cout << "  +===========================================+\n";
    cout << "  |  Eastern Medical Health Region (EMHR)    |\n";
    cout << "  |      Healthcare Management System        |\n";
    cout << "  |       Semester 2  AY2025/2026            |\n";
    cout << "  +===========================================+\n\n";

    // ---- Initialise all subsystems ----

    // UserManager loads from ./data folder (creates it if missing)
    UserManager     um("./data");

    PatientRegistry registry;
    ScheduleManager scheduler;
    EHRSystem       ehr;
    BillingSystem   billing;
    MetricsCounter  metrics;

    // Notification infrastructure
    NotificationSubject publisher;
    EmailNotification   emailSvc;
    DigicelAdapter      digicelAdapter;
    SMSNotification     smsSvc(&digicelAdapter);
    publisher.addObserver(&emailSvc);
    publisher.addObserver(&smsSvc);

    // Triage (window of 3 visits for moving average), File, Reports
    TriageManager  triageManager(&publisher, 3);
    FileManager    fileManager;
    ReportManager  reportManager(metrics, triageManager, fileManager);

    // ---- First-run: bootstrap admin if no users exist ----
    if (um.GetUserCount() == 0) {
        cout << "  [FIRST RUN] No users found. Creating default admin...\n";
        // BootstrapFirstAdmin from UserManager.h:
        // bool BootstrapFirstAdmin(username, plainPassword, fullName, email, phone, dept)
        um.BootstrapFirstAdmin("admin", "Admin@123",
                               "System Administrator",
                               "admin@emhr.gov.jm",
                               "876-000-0000", "IT");
        cout << "  Default credentials -> Username: admin  |  Password: Admin@123\n\n";
    }

    // ---- Login loop (max 3 attempts) ----
    shared_ptr<User> loggedIn = nullptr;
    int attempts = 0;
    while (!loggedIn && attempts < 3) {
        loggedIn = loginScreen(um);
        if (!loggedIn) {
            attempts++;
            cout << "  [ERROR] Invalid credentials. Attempts left: " << (3 - attempts) << "\n";
            pauseScreen();
        }
    }

    if (!loggedIn) {
        cout << "\n  [LOCKED] Too many failed attempts. System locked.\n\n";
        return 1;
    }

    cout << "\n  Login successful. Welcome, "
         << loggedIn->GetFullName() << "!\n";
    pauseScreen();

    // ---- Hand off to main menu ----
    mainMenu(loggedIn, um, registry, scheduler, ehr, billing,
             emailSvc, smsSvc, triageManager, reportManager, metrics);

    cout << "\n  Session ended. Goodbye!\n\n";
    return 0;
}
