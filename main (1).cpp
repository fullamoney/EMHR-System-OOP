// ============================================================
// File: main.cpp
// Project: Eastern Medical Health Region (EMHR) System
// Description: Unified entry point. Combines Users, Patients,
//              Appointments, EHR, Billing, Notifications,
//              Triage, and Reports into one running program.
// Compile:
//   g++ -std=c++17 main.cpp UserManager.cpp User.cpp Admin.cpp \
//       Role.cpp Permission.cpp -o emhr
// Run:
//   ./emhr
// ============================================================

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <limits>

// ---- Your existing headers ----
#include "UserManager.h"
#include "PatientRegistry.h"
#include "ScheduleManager.h"

// ---- My headers (notifications, triage, file, reports) ----
#include "NotificationService.h"
#include "EmailNotification.h"
#include "Triage.h"
#include "FileManager.h"
#include "ReportManager.h"

// ---- EHR & Billing (pulled from person4_ehr_billing.cpp) ----
// NOTE: Remove the main() from person4_ehr_billing.cpp before compiling.
//       All the classes inside it are included here directly.
#include "person4_ehr_billing.cpp"
// If your lecturer wants separate files, move all classes from
// person4_ehr_billing.cpp into EHR.h and Billing.h headers instead.

using namespace std;

// ============================================================
// Helper utilities
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
    cout << "  EMHR SYSTEM  |  " << title << "\n";
    cout << "============================================\n";
}

int getIntInput(const string& prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cout << "  [ERROR] Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string getStringInput(const string& prompt) {
    string val;
    cout << prompt;
    getline(cin, val);
    return val;
}

double getDoubleInput(const string& prompt) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cout << "  [ERROR] Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ============================================================
// Module: USER MANAGEMENT MENU
// ============================================================
void userMenu(UserManager& um, shared_ptr<User>& loggedIn) {
    bool back = false;
    while (!back) {
        printHeader("USER MANAGEMENT");
        cout << "  1. Add User\n";
        cout << "  2. Update User\n";
        cout << "  3. View User\n";
        cout << "  4. View All Users\n";
        cout << "  5. Delete User\n";
        cout << "  6. View Audit Log\n";
        cout << "  0. Back\n";
        int choice = getIntInput("\n  Choice: ");
        switch (choice) {
            case 1: {
                string uname = getStringInput("  Username     : ");
                string pass  = getStringInput("  Password     : ");
                string fname = getStringInput("  Full Name    : ");
                string email = getStringInput("  Email        : ");
                string phone = getStringInput("  Phone        : ");
                cout << "  Role (Admin/Manager/User): ";
                string role; getline(cin, role);
                um.AddUser(loggedIn->getId(), uname, pass, fname, email, phone, role);
                break;
            }
            case 2: {
                string tid = getStringInput("  Target User ID: ");
                um.UpdateUser(loggedIn->getId(), tid);
                break;
            }
            case 3: {
                string tid = getStringInput("  User ID: ");
                um.ViewUser(loggedIn->getId(), tid);
                break;
            }
            case 4:
                um.ViewAllUsers(loggedIn->getId());
                break;
            case 5: {
                string tid = getStringInput("  User ID to delete: ");
                um.DeleteUser(loggedIn->getId(), tid);
                break;
            }
            case 6:
                um.ViewAuditLog(loggedIn->getId());
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
// Module: PATIENT REGISTRY MENU
// ============================================================
void patientMenu(PatientRegistry& registry) {
    bool back = false;
    while (!back) {
        printHeader("PATIENT REGISTRY");
        cout << "  1. Add Patient\n";
        cout << "  2. View Patient\n";
        cout << "  3. Update Patient\n";
        cout << "  4. Delete Patient\n";
        cout << "  5. View All Patients\n";
        cout << "  0. Back\n";
        int choice = getIntInput("\n  Choice: ");
        switch (choice) {
            case 1: {
                string name = getStringInput("  Full Name    : ");
                string addr = getStringInput("  Address      : ");
                string cont = getStringInput("  Contact      : ");
                string kin  = getStringInput("  Next of Kin  : ");
                registry.createPatient(name, addr, cont, kin);
                break;
            }
            case 2: {
                int id = getIntInput("  Patient ID: ");
                Patient* p = registry.readPatient(id);
                if (p) p->display();
                break;
            }
            case 3: {
                int id   = getIntInput("  Patient ID   : ");
                string name = getStringInput("  New Name     : ");
                string addr = getStringInput("  New Address  : ");
                string cont = getStringInput("  New Contact  : ");
                string kin  = getStringInput("  New Next-Kin : ");
                registry.updatePatient(id, name, addr, cont, kin);
                break;
            }
            case 4: {
                int id = getIntInput("  Patient ID to delete: ");
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
// Module: APPOINTMENTS MENU
// ============================================================
void appointmentMenu(ScheduleManager& scheduler, MetricsCounter& metrics) {
    bool back = false;
    while (!back) {
        printHeader("APPOINTMENTS");
        cout << "  1. Book Appointment\n";
        cout << "  2. Update Appointment\n";
        cout << "  3. Cancel Appointment\n";
        cout << "  4. View Appointments by Patient\n";
        cout << "  5. View All Appointments\n";
        cout << "  0. Back\n";
        int choice = getIntInput("\n  Choice: ");
        switch (choice) {
            case 1: {
                int pid      = getIntInput("  Patient ID          : ");
                string dt    = getStringInput("  Date/Time (YYYY-MM-DD HH:MM): ");
                int dur      = getIntInput("  Duration (minutes)  : ");
                string notes = getStringInput("  Notes               : ");
                bool ok = scheduler.bookAppointment(pid, dt, dur, notes);
                if (ok) {
                    metrics.recordAppointmentBooked();
                } else {
                    metrics.recordSchedulingConflict();
                }
                break;
            }
            case 2: {
                int id    = getIntInput("  Appointment ID: ");
                string dt = getStringInput("  New Date/Time (YYYY-MM-DD HH:MM): ");
                scheduler.updateAppointment(id, dt);
                break;
            }
            case 3: {
                int id = getIntInput("  Appointment ID: ");
                scheduler.cancelAppointment(id);
                metrics.recordAppointmentCancelled();
                break;
            }
            case 4: {
                int pid = getIntInput("  Patient ID: ");
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
// Module: EHR MENU
// ============================================================
void ehrMenu(EHRSystem& ehr) {
    bool back = false;
    while (!back) {
        printHeader("ELECTRONIC HEALTH RECORDS (EHR)");
        cout << "  1. Add Vitals\n";
        cout << "  2. Add Visit Record\n";
        cout << "  3. Add Prescription\n";
        cout << "  4. Add Lab Result\n";
        cout << "  5. Add Immunization\n";
        cout << "  6. Add Allergy\n";
        cout << "  7. View Patient EHR\n";
        cout << "  0. Back\n";
        int choice = getIntInput("\n  Choice: ");
        switch (choice) {
            case 1: {
                int pid     = getIntInput("  Patient ID  : ");
                string date = getStringInput("  Date (YYYY-MM-DD): ");
                double temp = getDoubleInput("  Temperature (C): ");
                int hr      = getIntInput("  Heart Rate (bpm): ");
                string bp   = getStringInput("  Blood Pressure (e.g. 120/80): ");
                double wt   = getDoubleInput("  Weight (kg): ");
                double ht   = getDoubleInput("  Height (cm): ");
                ehr.addRecord(make_shared<Vitals>(pid, date, temp, hr, bp, wt, ht));
                break;
            }
            case 2: {
                int pid     = getIntInput("  Patient ID  : ");
                string date = getStringInput("  Date (YYYY-MM-DD): ");
                string diag = getStringInput("  Diagnosis   : ");
                string note = getStringInput("  Doctor Notes: ");
                string fu   = getStringInput("  Follow-up Date (or leave blank): ");
                ehr.addRecord(make_shared<VisitRecord>(pid, date, diag, note, fu));
                break;
            }
            case 3: {
                int pid     = getIntInput("  Patient ID  : ");
                string date = getStringInput("  Date (YYYY-MM-DD): ");
                string med  = getStringInput("  Medication  : ");
                string dose = getStringInput("  Dosage      : ");
                string freq = getStringInput("  Frequency   : ");
                string dur  = getStringInput("  Duration    : ");
                ehr.addRecord(make_shared<Prescription>(pid, date, med, dose, freq, dur));
                break;
            }
            case 4: {
                int pid     = getIntInput("  Patient ID  : ");
                string date = getStringInput("  Date (YYYY-MM-DD): ");
                string test = getStringInput("  Test Name   : ");
                string res  = getStringInput("  Result      : ");
                string ref  = getStringInput("  Reference Range: ");
                string abn  = getStringInput("  Abnormal? (y/n): ");
                ehr.addRecord(make_shared<LabResult>(pid, date, test, res, ref, abn=="y"));
                break;
            }
            case 5: {
                int pid     = getIntInput("  Patient ID  : ");
                string vac  = getStringInput("  Vaccine     : ");
                string dg   = getStringInput("  Date Given  : ");
                string nd   = getStringInput("  Next Due Date (or blank): ");
                ehr.addImmunization(pid, vac, dg, nd);
                break;
            }
            case 6: {
                int pid      = getIntInput("  Patient ID  : ");
                string all   = getStringInput("  Allergen    : ");
                string react = getStringInput("  Reaction    : ");
                ehr.addAllergy(pid, all, react);
                break;
            }
            case 7: {
                int pid = getIntInput("  Patient ID: ");
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
// Module: BILLING MENU
// ============================================================
void billingMenu(BillingSystem& billing, MetricsCounter& metrics) {
    bool back = false;
    while (!back) {
        printHeader("BILLING");
        cout << "  1. Create Invoice\n";
        cout << "  2. Pay Invoice (Cash)\n";
        cout << "  3. Pay Invoice (Card)\n";
        cout << "  4. Pay Invoice (Insurance + Card)\n";
        cout << "  5. View Patient Invoices\n";
        cout << "  0. Back\n";
        int choice = getIntInput("\n  Choice: ");
        switch (choice) {
            case 1: {
                int pid     = getIntInput("  Patient ID  : ");
                double amt  = getDoubleInput("  Amount (JMD): ");
                string desc = getStringInput("  Description : ");
                billing.createInvoice(pid, amt, desc);
                break;
            }
            case 2: {
                int invId   = getIntInput("  Invoice ID  : ");
                double cash = getDoubleInput("  Cash Tendered: ");
                vector<shared_ptr<PaymentStrategy>> s = { make_shared<CashPayment>(cash) };
                billing.payInvoice(invId, s);
                metrics.recordBillingTransaction(cash);
                break;
            }
            case 3: {
                int invId   = getIntInput("  Invoice ID  : ");
                string last4 = getStringInput("  Card Last 4 Digits: ");
                double amt  = getDoubleInput("  Charge Amount: ");
                vector<shared_ptr<PaymentStrategy>> s = { make_shared<CardPayment>(last4) };
                billing.payInvoice(invId, s);
                metrics.recordBillingTransaction(amt);
                break;
            }
            case 4: {
                int invId  = getIntInput("  Invoice ID     : ");
                double cov = getDoubleInput("  Insurance Coverage % (e.g. 0.80): ");
                double ded = getDoubleInput("  Deductible (JMD): ");
                double cap = getDoubleInput("  Per-Visit Cap (JMD): ");
                string last4 = getStringInput("  Card Last 4 (for remainder): ");
                double total = getDoubleInput("  Total Amount (JMD): ");
                vector<shared_ptr<PaymentStrategy>> s = {
                    make_shared<InsurancePayment>(cov, ded, cap),
                    make_shared<CardPayment>(last4)
                };
                billing.payInvoice(invId, s);
                metrics.recordBillingTransaction(total);
                break;
            }
            case 5: {
                int pid = getIntInput("  Patient ID: ");
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
// Module: NOTIFICATIONS MENU
// ============================================================
void notificationMenu(EmailNotification& emailSvc,
                      SMSNotification&   smsSvc,
                      MetricsCounter&    metrics) {
    bool back = false;
    while (!back) {
        printHeader("NOTIFICATIONS");
        cout << "  1. Send Email Notification\n";
        cout << "  2. Send SMS Notification\n";
        cout << "  0. Back\n";
        int choice = getIntInput("\n  Choice: ");
        switch (choice) {
            case 1: {
                string to   = getStringInput("  Recipient Email: ");
                cout << "  Type (APPT_BOOKED / LAB_RESULT / PAYMENT_REMINDER / FOLLOW_UP): ";
                string type; getline(cin, type);
                string msg  = getStringInput("  Message        : ");
                try {
                    emailSvc.sendNotification(to, emailSvc.buildSubject(type), msg);
                    metrics.recordNotificationSent();
                } catch (const exception& e) {
                    cout << "  [ERROR] " << e.what() << "\n";
                }
                break;
            }
            case 2: {
                string to   = getStringInput("  Recipient Phone: ");
                cout << "  Type (APPT_BOOKED / LAB_RESULT / PAYMENT_REMINDER / FOLLOW_UP): ";
                string type; getline(cin, type);
                string msg  = getStringInput("  Message        : ");
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
// Module: TRIAGE MENU
// ============================================================
void triageMenu(TriageManager& triageManager, MetricsCounter& metrics) {
    bool back = false;
    while (!back) {
        printHeader("TRIAGE & ANALYTICS");
        cout << "  1. Add Triage Record\n";
        cout << "  2. View All Triage Records\n";
        cout << "  3. View Analytics / Response Times\n";
        cout << "  4. View Moving Average for Patient\n";
        cout << "  0. Back\n";
        int choice = getIntInput("\n  Choice: ");
        switch (choice) {
            case 1: {
                string pid  = getStringInput("  Patient ID   : ");
                string name = getStringInput("  Patient Name : ");
                string arr  = getStringInput("  Arrival Time (YYYY-MM-DDTHH:MM:SS): ");
                double bpS  = getDoubleInput("  BP Systolic  : ");
                double bpD  = getDoubleInput("  BP Diastolic : ");
                double hr   = getDoubleInput("  Heart Rate   : ");
                double temp = getDoubleInput("  Temperature  : ");
                double spo2 = getDoubleInput("  SpO2 (%)     : ");
                double rr   = getDoubleInput("  Resp. Rate   : ");
                int resp    = getIntInput("  Response Time (min): ");
                string note = getStringInput("  Triage Note  : ");

                Vitals v(bpS, bpD, hr, temp, spo2, rr, arr);
                TriageRecord rec(pid, name, v, arr);
                rec.responseTimeMinutes = resp;
                rec.triageNote = note;
                triageManager.addRecord(rec);
                metrics.recordTriageCase(rec.riskScore.getLevel());
                break;
            }
            case 2:
                triageManager.displayAll();
                break;
            case 3:
                triageManager.analyzeResponseTimes();
                break;
            case 4: {
                string pid = getStringInput("  Patient ID: ");
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
// Module: REPORTS MENU
// ============================================================
void reportsMenu(ReportManager& reportManager) {
    bool back = false;
    while (!back) {
        printHeader("METRICS & REPORTS");
        cout << "  1. View Live Metrics Dashboard\n";
        cout << "  2. Generate Weekly Summary Report\n";
        cout << "  0. Back\n";
        int choice = getIntInput("\n  Choice: ");
        switch (choice) {
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
// LOGIN
// ============================================================
shared_ptr<User> loginScreen(UserManager& um) {
    printHeader("LOGIN");
    string uname = getStringInput("  Username: ");
    string pass  = getStringInput("  Password: ");
    auto user = um.Login(uname, pass);
    if (!user) {
        cout << "\n  [ERROR] Invalid username or password.\n";
    }
    return user;
}

// ============================================================
// MAIN MENU
// ============================================================
void mainMenu(shared_ptr<User>& loggedIn,
              UserManager&      um,
              PatientRegistry&  registry,
              ScheduleManager&  scheduler,
              EHRSystem&        ehr,
              BillingSystem&    billing,
              EmailNotification& emailSvc,
              SMSNotification&  smsSvc,
              TriageManager&    triageManager,
              ReportManager&    reportManager,
              MetricsCounter&   metrics) {
    bool running = true;
    while (running) {
        clearScreen();
        printHeader("MAIN MENU  |  Logged in: " + loggedIn->getFullName());
        cout << "  1. User Management\n";
        cout << "  2. Patient Registry\n";
        cout << "  3. Appointments\n";
        cout << "  4. Electronic Health Records (EHR)\n";
        cout << "  5. Billing\n";
        cout << "  6. Notifications\n";
        cout << "  7. Triage & Analytics\n";
        cout << "  8. Metrics & Reports\n";
        cout << "  0. Logout\n";
        int choice = getIntInput("\n  Choice: ");
        switch (choice) {
            case 1: userMenu(um, loggedIn);                                    break;
            case 2: patientMenu(registry);                                     break;
            case 3: appointmentMenu(scheduler, metrics);                       break;
            case 4: ehrMenu(ehr);                                              break;
            case 5: billingMenu(billing, metrics);                             break;
            case 6: notificationMenu(emailSvc, smsSvc, metrics);              break;
            case 7: triageMenu(triageManager, metrics);                        break;
            case 8: reportsMenu(reportManager);                                break;
            case 0: running = false;                                           break;
            default: cout << "  [ERROR] Invalid option.\n"; pauseScreen();    break;
        }
    }
}

// ============================================================
// ENTRY POINT
// ============================================================
int main() {
    clearScreen();
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════╗\n";
    cout << "  ║  Eastern Medical Health Region (EMHR)   ║\n";
    cout << "  ║        Healthcare Management System      ║\n";
    cout << "  ║          Semester 2 AY2025/2026          ║\n";
    cout << "  ╚══════════════════════════════════════════╝\n\n";

    // ---- Initialise all systems ----
    UserManager      um("./data");
    PatientRegistry  registry;
    ScheduleManager  scheduler;
    EHRSystem        ehr;
    BillingSystem    billing;
    MetricsCounter   metrics;

    // Notification setup
    NotificationSubject publisher;
    EmailNotification   emailSvc;
    DigicelAdapter      digicelAdapter;
    SMSNotification     smsSvc(&digicelAdapter);
    publisher.addObserver(&emailSvc);
    publisher.addObserver(&smsSvc);

    // Triage & Reports
    TriageManager  triageManager(&publisher, 3);
    FileManager    fileManager;
    ReportManager  reportManager(metrics, triageManager, fileManager);

    // Bootstrap: create first admin if no users exist
    if (um.GetUserCount() == 0) {
        cout << "  [SETUP] No users found. Creating default admin account...\n";
        um.BootstrapFirstAdmin("admin", "Admin@123", "System Administrator",
                               "admin@emhr.gov.jm", "876-000-0000", "IT");
        cout << "  Default login -> Username: admin | Password: Admin@123\n\n";
    }

    // ---- Login loop ----
    shared_ptr<User> loggedIn = nullptr;
    int attempts = 0;
    while (!loggedIn && attempts < 3) {
        loggedIn = loginScreen(um);
        if (!loggedIn) {
            attempts++;
            cout << "  Attempts remaining: " << (3 - attempts) << "\n";
            pauseScreen();
        }
    }

    if (!loggedIn) {
        cout << "\n  [LOCKED] Too many failed attempts. Exiting.\n";
        return 1;
    }

    cout << "\n  Welcome, " << loggedIn->getFullName() << "!\n";
    pauseScreen();

    // ---- Run main menu ----
    mainMenu(loggedIn, um, registry, scheduler, ehr, billing,
             emailSvc, smsSvc, triageManager, reportManager, metrics);

    cout << "\n  Goodbye! Data has been saved.\n\n";
    return 0;
}
