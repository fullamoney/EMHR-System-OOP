// ============================================================
// File: EmailNotification.h
// Description: Email and SMS notification classes with
//              Observer and Adapter patterns
// Project: EMHR Healthcare System - Semester 2 AY2025/2026
// ============================================================

#ifndef EMAIL_NOTIFICATION_H
#define EMAIL_NOTIFICATION_H

#include "NotificationService.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <stdexcept>

using namespace std;

// Helper: get ISO-8601 timestamp
inline string currentTimestamp() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    ostringstream oss;
    oss << put_time(t, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

// ============================================================
// EmailNotification
// INHERITANCE: extends NotificationService
// POLYMORPHISM: overrides sendNotification()
// OBSERVER: implements NotificationObserver
// ============================================================
class EmailNotification : public NotificationService,
                          public NotificationObserver {
private:
    string smtpServer;
    string senderAddress;

    void persistNotification(const string& recipient,
                              const string& subject,
                              const string& message,
                              const string& channel) const {
        ofstream file("notifications.txt", ios::app);
        if (file.is_open()) {
            file << currentTimestamp() << "|" << channel << "|"
                 << recipient << "|" << subject << "|" << message << "\n";
            file.close();
        }
    }

public:
    EmailNotification(const string& smtp   = "smtp.emhr.gov.jm",
                      const string& sender = "noreply@emhr.gov.jm")
        : NotificationService("EmailService"),
          smtpServer(smtp), senderAddress(sender) {}

    // POLYMORPHISM - overrides pure virtual
    void sendNotification(const string& recipient,
                          const string& subject,
                          const string& message) override {
        if (recipient.empty() || subject.empty())
            throw invalid_argument("Email recipient and subject cannot be empty.");

        cout << "\n[EMAIL] -----------------------------------------------\n";
        cout << "  From    : " << senderAddress << "\n";
        cout << "  To      : " << recipient     << "\n";
        cout << "  Subject : " << subject       << "\n";
        cout << "  Message : " << message       << "\n";
        cout << "  Sent At : " << currentTimestamp() << "\n";
        cout << "[EMAIL] -----------------------------------------------\n";

        logNotification(recipient, subject);
        persistNotification(recipient, subject, message, "EMAIL");
    }

    // Observer pattern callback
    void onEvent(const string& eventType,
                 const string& recipient,
                 const string& details) override {
        sendNotification(recipient, buildSubject(eventType), details);
    }

    // Accessors
    string getSmtpServer()    const { return smtpServer; }
    string getSenderAddress() const { return senderAddress; }

    // Mutators with validation
    void setSmtpServer(const string& s) {
        if (s.empty()) throw invalid_argument("SMTP server cannot be empty.");
        smtpServer = s;
    }

    string buildSubject(const string& eventType) const {
        if (eventType == "APPT_BOOKED")      return "Appointment Confirmation - EMHR";
        if (eventType == "LAB_RESULT")       return "Your Lab Results Are Ready - EMHR";
        if (eventType == "PAYMENT_REMINDER") return "Payment Reminder - EMHR";
        if (eventType == "FOLLOW_UP")        return "Follow-Up Visit Reminder - EMHR";
        if (eventType == "HIGH_RISK_TRIAGE") return "URGENT: High-Risk Triage Alert - EMHR";
        return "Notification from EMHR";
    }
};

// ============================================================
// SMSVendorAdapter - Abstract Adapter interface
// Hides Digicel / Flow vendor APIs (Adapter pattern)
// ============================================================
class SMSVendorAdapter {
public:
    virtual ~SMSVendorAdapter() {}
    virtual void dispatchSMS(const string& phone,
                             const string& text) = 0;
    virtual string vendorName() const = 0;
};

// Concrete Adapter for Digicel
class DigicelAdapter : public SMSVendorAdapter {
public:
    void dispatchSMS(const string& phone, const string& text) override {
        cout << "  [Digicel API] -> Sending SMS to " << phone << "\n";
        cout << "  [Digicel API] -> Text: " << text << "\n";
    }
    string vendorName() const override { return "Digicel"; }
};

// Concrete Adapter for Flow
class FlowAdapter : public SMSVendorAdapter {
public:
    void dispatchSMS(const string& phone, const string& text) override {
        cout << "  [Flow API] -> Sending SMS to " << phone << "\n";
        cout << "  [Flow API] -> Text: " << text << "\n";
    }
    string vendorName() const override { return "Flow"; }
};

// ============================================================
// SMSNotification
// INHERITANCE: extends NotificationService
// ADAPTER: owns a SMSVendorAdapter (COMPOSITION)
// OBSERVER: implements NotificationObserver
// ============================================================
class SMSNotification : public NotificationService,
                        public NotificationObserver {
private:
    SMSVendorAdapter* vendorAdapter;

    void persistNotification(const string& recipient,
                              const string& subject,
                              const string& message) const {
        ofstream file("notifications.txt", ios::app);
        if (file.is_open()) {
            file << currentTimestamp() << "|SMS|"
                 << recipient << "|" << subject << "|" << message << "\n";
            file.close();
        }
    }

public:
    explicit SMSNotification(SMSVendorAdapter* adapter)
        : NotificationService("SMSService"), vendorAdapter(adapter) {
        if (!adapter) throw invalid_argument("SMS vendor adapter cannot be null.");
    }

    void sendNotification(const string& recipient,
                          const string& subject,
                          const string& message) override {
        if (recipient.empty())
            throw invalid_argument("SMS recipient cannot be empty.");

        string sms = "[" + subject + "] " + message;
        if (sms.size() > 160) sms = sms.substr(0, 157) + "...";

        cout << "\n[SMS][" << vendorAdapter->vendorName()
             << "] ------------------------------------------\n";
        cout << "  To      : " << recipient << "\n";
        cout << "  Message : " << sms       << "\n";
        cout << "  Sent At : " << currentTimestamp() << "\n";

        vendorAdapter->dispatchSMS(recipient, sms);
        cout << "[SMS] ------------------------------------------\n";

        logNotification(recipient, subject);
        persistNotification(recipient, subject, sms);
    }

    void onEvent(const string& eventType,
                 const string& recipient,
                 const string& details) override {
        sendNotification(recipient, buildSubject(eventType), details);
    }

    void setVendorAdapter(SMSVendorAdapter* adapter) {
        if (!adapter) throw invalid_argument("Adapter cannot be null.");
        vendorAdapter = adapter;
    }

    string getVendorName() const {
        return vendorAdapter ? vendorAdapter->vendorName() : "None";
    }

    string buildSubject(const string& eventType) const {
        if (eventType == "APPT_BOOKED")      return "Appt Confirmed";
        if (eventType == "LAB_RESULT")       return "Lab Results Ready";
        if (eventType == "PAYMENT_REMINDER") return "Payment Due";
        if (eventType == "FOLLOW_UP")        return "Follow-Up Reminder";
        if (eventType == "HIGH_RISK_TRIAGE") return "URGENT Triage Alert";
        return "EMHR Notice";
    }
};

#endif // EMAIL_NOTIFICATION_H
