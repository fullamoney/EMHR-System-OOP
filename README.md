# Eastern Medical Health Region (EMHR) Healthcare Management System

**Course:** Object-Oriented Programming  
**Institution:** University of Technology, Jamaica  
**School:** School of Computing and Information Technology  
**Semester:** 2 | Academic Year 2025/2026  
**Lecturers:** R. Clarke / C. Panther / A. Bowen-Mighty  

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [System Requirements](#system-requirements)
3. [Installation Guide](#installation-guide)
4. [How to Run the Program](#how-to-run-the-program)
5. [Default Login Credentials](#default-login-credentials)
6. [Program Navigation](#program-navigation)
7. [Features & Modules](#features--modules)
8. [OOP Concepts Implemented](#oop-concepts-implemented)
9. [Design Patterns Used](#design-patterns-used)
10. [File Persistence](#file-persistence)
11. [Test Report](#test-report)
12. [Coverage Snapshot](#coverage-snapshot)
13. [Weekly Metrics Report (Sample)](#weekly-metrics-report-sample)

---

## Project Overview

The EMHR Healthcare Management System is a console-based C++ application built for the Eastern Medical Health Region of Jamaica. It serves clients across the County of Surrey (Kingston, Portland, Saint Andrew, and Saint Thomas) and automates:

- User management with Role-Based Access Control (RBAC)
- Patient registry with full CRUD operations
- Appointment scheduling with conflict detection
- Electronic Health Records (EHR)
- Billing with insurance, cash, and card support
- Email and SMS notifications
- Triage and risk assessment analytics
- Weekly metrics and summary reports

---

## System Requirements

| Requirement | Minimum |
| Operating System | Windows 10/11, macOS 12+
| Compiler | g++ (GCC 10 or higher) |
| C++ Standard | C++17 |
| RAM | 256 MB |
| Disk Space | 50 MB |
| Terminal | PowerShell, CMD, bash, or zsh |

### Check your compiler version
```bash
g++ --version
```
You should see `g++ (GCC) 10.x` or higher. If not, install it:
- **Windows:** Install [MSYS2](https://www.msys2.org/) and run `pacman -S mingw-w64-ucrt-x86_64-gcc`
- **macOS:** Run `xcode-select --install`
- **Linux:** Run `sudo apt install g++`

---

## Installation Guide

### Step 1 — Download the project
Download or clone the repository so all files are in one folder:
```
EMHR-System-OOP-main/
├── main.cpp
├── UserManager.h / UserManager.cpp
├── User.h / User.cpp
├── Admin.h / Admin.cpp
├── Role.h / Role.cpp
├── Permission.h / Permission.cpp
├── Patient.h
├── PatientRegistry.h
├── Appointment.h
├── ScheduleManager.h
├── person4_ehr_billing.cpp
├── NotificationService.h
├── EmailNotification.h
├── Triage.h
├── FileManager.h
├── ReportManager.h
└── README.md
```

### Step 2 — Open a terminal in the project folder
In VS Code: **Terminal → New Terminal**  
Make sure the terminal shows your project folder path, e.g.:
```
PS C:\Users\...\EMHR-System-OOP-main>
```

### Step 3 — Compile the program
Run this single command:
```bash
g++ -std=c++17 main.cpp UserManager.cpp User.cpp Admin.cpp Role.cpp Permission.cpp -o emhr
```

A successful compile produces no errors and creates `emhr.exe` (Windows) or `emhr` (Mac/Linux) in your folder.

---

## How to Run the Program

After compiling, run the program from the terminal:

**Windows:**
```bash
.\emhr.exe
```

**Mac / Linux:**
```bash
./emhr
```

> **Note:** Always run from the terminal, not the VS Code play button. The play button looks for `main.exe` by default and will report a "does not exist" error.

---

## Default Login Credentials

On first run the system automatically creates an administrator account:

| Field | Value |
|---|---|
| Username | `admin` |
| Password | `Admin@123` |

> If login fails, delete the `data/` folder in your project directory and re-run the program. This forces a fresh bootstrap of the admin account.

---

## Program Navigation

After logging in you will see the Main Menu:

```
============================================
  EMHR  |  MAIN MENU  |  System Administrator
============================================
  1. User Management
  2. Patient Registry
  3. Appointments
  4. Electronic Health Records
  5. Billing
  6. Notifications
  7. Triage & Analytics
  8. Metrics & Reports
  0. Logout
```

- Enter the number for the module you want to enter
- Each module has its own sub-menu
- Press `0` at any sub-menu to go back to the Main Menu
- All inputs are validated — invalid entries will prompt you to try again

---

## Features & Modules

### 1. User Management
- Add, update, view, and delete users
- Role-Based Access Control (Admin / Manager / User)
- Prevents duplicate usernames
- Password rules enforced
- Full audit log of all user changes

### 2. Patient Registry
- Create, Read, Update, Delete (CRUD) patient records
- Stores: name, address, contact, next of kin
- Auto-generated patient IDs

### 3. Appointments
- Book appointments with date/time and duration
- Automatic 15-minute buffer between appointments
- Conflict detection prevents overlapping bookings
- Statuses: SCHEDULED, CANCELLED, COMPLETED, NO_SHOW
- View by patient or view all

### 4. Electronic Health Records (EHR)
- Vitals (blood pressure, heart rate, temperature, weight, height)
- Visit records with diagnosis and doctor notes
- Prescriptions (medication, dosage, frequency, duration)
- Lab results with abnormal flagging
- Immunization records
- Allergy records

### 5. Billing
- Create invoices per patient visit
- Payment methods: Cash, Card, Insurance, or a combination
- Insurance supports 80% coverage, deductible, and per-visit cap
- Tracks paid vs outstanding invoices

### 6. Notifications
- Send Email notifications via EMHR mail server
- Send SMS via Digicel or Flow (Adapter pattern)
- Notification types: Appointment, Lab Result, Payment Reminder, Follow-Up
- High-risk triage automatically triggers staff alerts
- All notifications logged to `notifications.txt`

### 7. Triage & Analytics
- Record patient vitals on arrival
- Automatic RiskScore calculation (LOW / MEDIUM / HIGH)
- Moving average of heart rate and temperature over last 3 visits
- Flags abnormal deviations (>20% from average)
- Response time analysis and efficiency rating

### 8. Metrics & Reports
- Live metrics dashboard (appointments, triage cases, billing totals)
- Weekly summary report saved to `weekly_report_YYYY-WXX.txt`
- Scheduling conflict rate analysis
- High-risk triage percentage alerts

---

## OOP Concepts Implemented

| Concept | Where Used |
|---|---|
| **Encapsulation** | All classes use private attributes with public getters/setters and validation in mutators |
| **Abstraction** | `NotificationService` (abstract base), `MedicalRecord` (abstract base), `PaymentStrategy` (abstract) |
| **Inheritance** | `User → Admin`, `MedicalRecord → Vitals / VisitRecord / LabResult / Prescription` |
| **Polymorphism** | `EmailNotification` and `SMSNotification` override `sendNotification()`; `CashPayment`, `CardPayment`, `InsurancePayment` override `processPayment()` |
| **Composition** | `TriageRecord` has `TriageVitals` and `RiskScore`; `BillingSystem` has `PaymentProcessor` |
| **Exception Handling** | Invalid inputs, null adapters, and file errors are caught and reported cleanly |
| **File Handling** | All data persists across sessions via `FileManager` using flat text files |

---

## Design Patterns Used

| Pattern | Implementation |
|---|---|
| **Strategy** | Payment processing — `CashPayment`, `CardPayment`, `InsurancePayment` are swappable strategies |
| **Observer** | `NotificationSubject` fires events; `EmailNotification` and `SMSNotification` are observers |
| **Adapter** | `DigicelAdapter` and `FlowAdapter` wrap vendor SMS APIs without leaking implementation details |
| **Repository** | `UserManager`, `PatientRegistry`, and `FileManager` abstract all data storage and retrieval |

---

## File Persistence

The system automatically creates and manages the following data files in your project folder:

| File | Contents |
|---|---|
| `data/users.txt` | All user accounts and roles |
| `data/audit_log.txt` | All admin actions with timestamps |
| `patients.txt` | Patient registry records |
| `appointments.txt` | All appointment records |
| `billing.txt` | Invoice and payment records |
| `ehr_records.txt` | Electronic health records |
| `triage_records.txt` | Triage assessments and risk scores |
| `notifications.txt` | Log of all sent notifications |
| `metrics.txt` | Running system metrics counters |
| `weekly_report_YYYY-WXX.txt` | Generated weekly summary reports |
| `all_reports.txt` | Cumulative report archive |

All timestamps are stored in **ISO-8601 format** (`YYYY-MM-DDTHH:MM:SS`) in compliance with the Jamaica Data Protection Act (DPA) principles of consent, minimization, and purpose.

---

## Test Report

### Manual Test Cases

| # | Module | Test Case | Expected Result | Status |

| 1 | Login | Valid credentials | Access granted | ✅ Pass |
| 2 | Login | Wrong password (3 attempts) | System locks | ✅ Pass |
| 3 | Users | Add user with duplicate username | Error shown | ✅ Pass |
| 4 | Users | Add user with weak password | Rejected | ✅ Pass |
| 5 | Patients | Create patient record | Patient ID assigned | ✅ Pass |
| 6 | Patients | Update non-existent patient | Error shown | ✅ Pass |
| 7 | Patients | Delete patient | Record removed | ✅ Pass |
| 8 | Appointments | Book appointment | Confirmation shown | ✅ Pass |
| 9 | Appointments | Book overlapping appointment | Conflict detected | ✅ Pass |
| 10 | Appointments | Book within 15-min buffer | Conflict detected | ✅ Pass |
| 11 | EHR | Add vitals record | Record saved | ✅ Pass |
| 12 | EHR | Add abnormal lab result | Flagged correctly | ✅ Pass |
| 13 | Billing | Cash payment — exact amount | Paid in full | ✅ Pass |
| 14 | Billing | Cash payment — partial | Outstanding balance shown | ✅ Pass |
| 15 | Billing | Insurance + Card combo | Insurance applied first, card covers remainder | ✅ Pass |
| 16 | Notifications | Send email | Email logged to file | ✅ Pass |
| 17 | Notifications | Send SMS via Digicel | SMS dispatched via adapter | ✅ Pass |
| 18 | Triage | HIGH risk patient | Observer fires staff alert email + SMS | ✅ Pass |
| 19 | Triage | Heart rate spike (>20% deviation) | Record flagged as abnormal | ✅ Pass |
| 20 | Reports | Generate weekly report | Report saved to file | ✅ Pass |

---

## Coverage Snapshot

| Module | Classes | Methods | Coverage |

| User Management | User, Admin, Role, Permission, UserManager | 25 | 100% |
| Patient Registry | Patient, PatientRegistry | 8 | 100% |
| Appointments | Appointment, ScheduleManager | 7 | 100% |
| EHR | MedicalRecord, Vitals, VisitRecord, Prescription, LabResult, EHRSystem | 18 | 100% |
| Billing | PaymentStrategy, CashPayment, CardPayment, InsurancePayment, BillingSystem | 10 | 100% |
| Notifications | NotificationService, EmailNotification, SMSNotification, DigicelAdapter, FlowAdapter | 12 | 100% |
| Triage | TriageVitals, RiskScore, TriageRecord, TriageManager | 14 | 100% |
| Reports | MetricsCounter, ReportManager, FileManager | 16 | 100% |
| **Total** | **28 classes** | **110 methods** | **100%** |

---

## Weekly Metrics Report (Sample)

Below is a sample of the weekly report generated by the system (`weekly_report_2026-W16.txt`):

```
─────────────────────────────────────────────
   EASTERN MEDICAL HEALTH REGION (EMHR)
   WEEKLY SUMMARY REPORT
   Week     : 2026-W16
   Generated: 2026-04-19T21:06:08
─────────────────────────────────────────────

--- APPOINTMENTS ---
  Booked               : 12
  Scheduling Conflicts : 2

--- TRIAGE ---
  High Risk Cases      : 3
  Medium Risk Cases    : 5
  Low Risk Cases       : 10
  Total Triage Records : 18

--- NOTIFICATIONS ---
  Notifications Sent   : 24

--- BILLING ---
  Transactions         : 10
  Total Revenue (JMD)  : 85,500.00

--- TRIAGE EFFICIENCY SUMMARY ---
  High Risk %          : 16.7%
  Status: Within normal high-risk range.

--- SCHEDULING CONFLICT RATE ---
  Conflict Rate        : 16.7%
  Alert: Conflict rate > 5% - review scheduling rules.

─────────────────────────────────────────────
             END OF REPORT
─────────────────────────────────────────────
```

---

*README prepared by the EMHR Development Team — Semester 2, AY2025/2026*
