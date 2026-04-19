// ============================================================
// File: EmailNotification.h
// Description: Email notification with Observer integration
// Project: EMHR Healthcare System - Semester 2 AY2025/2026
// ============================================================

#ifndef EMAIL_NOTIFICATION_H
#define EMAIL_NOTIFICATION_H

#include "NotificationService.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

// Helper: get ISO-8601 timestamp
inline std::string currentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(t, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

// ============================================================
// EmailNotification
// INHERITANCE: extends NotificationService
// POLYMORPHISM: overrides sendNotification()
// ============================================================
class EmailNotification : public NotificationService,
                          public NotificationObserver {
private:
    std::string smtpServer;     // email server config (encapsulated)
    std::string senderAddress;

public:
    // Constructor
    EmailNotification(const std::string& smtp = "smtp.emhr.gov.jm",
                      const std::string& sender = "noreply@emhr.gov.jm")
        : NotificationService("EmailService"),
          smtpServer(smtp), senderAddress(sender) {}

    // POLYMORPHISM - override of pure virtual
    void sendNotification(const std::string& recipient,
                          const std::string& subject,
                          const std::string& message) override {
        // Validate input (defensive programming)
        if (recipient.empty() || subject.empty()) {
            throw std::invalid_argument("Email recipient and subject cannot be empty.");
        }

        std::cout << "\n[EMAIL] -----------------------------------------------\n";
        std::cout << "  From    : " << senderAddress << "\n";
        std::cout << "  To      : " << recipient << "\n";
        std::cout << "  Subject : " << subject << "\n";
        std::cout << "  Message : " << message << "\n";
        std::cout << "  Sent At : " << currentTimestamp() << "\n";
        std::cout << "[EMAIL] -----------------------------------------------\n";

        logNotification(recipient, subject);
        persistNotification(recipient, subject, message, "EMAIL");
    }

    // Observer pattern: react to system events
    void onEvent(const std::string& eventType,
                 const std::string& recipient,
                 const std::string& details) override {
        std::string subject = buildSubject(eventType);
        sendNotification(recipient, subject, details);
    }

    // Accessors (Encapsulation)
    std::string getSmtpServer()    const { return smtpServer; }
    std::string getSenderAddress() const { return senderAddress; }

    // Mutators with validation
    void setSmtpServer(const std::string& s) {
        if (s.empty()) throw std::invalid_argument("SMTP server cannot be empty.");
        smtpServer = s;
    }

    // Predefined subject lines for known event types
    std::string buildSubject(const std::string& eventType) const {
        if (eventType == "APPT_BOOKED")     return "Appointment Confirmation - EMHR";
        if (eventType == "LAB_RESULT")      return "Your Lab Results Are Ready - EMHR";
        if (eventType == "PAYMENT_REMINDER")return "Payment Reminder - EMHR";
        if (eventType == "FOLLOW_UP")       return "Follow-Up Visit Reminder - EMHR";
        if (eventType == "HIGH_RISK_TRIAGE")return "URGENT: High-Risk Triage Alert - EMHR";
        return "Notification from EMHR";
    }

private:
    // FILE PERSISTENCE: log all email notifications
    void persistNotification(const std::string& recipient,
                              const std::string& subject,
                              const std::string& message,
                              const std::string& channel) const {
        std::ofstream file("notifications.txt", std::ios::app);
        if (file.is_open()) {
            file << currentTimestamp() << "|" << channel << "|"
                 << recipient << "|" << subject << "|" << message << "\n";
            file.close();
        }
    }
};

// ============================================================
// Vendor Adapter interfaces (Adapter pattern)
// Hides Digicel / Flow vendor-specific APIs
// ============================================================
class SMSVendorAdapter {
public:
    virtual ~SMSVendorAdapter() {}
    virtual void dispatchSMS(const std::string& phone,
                             const std::string& text) = 0;
    virtual std::string vendorName() const = 0;
};

// Concrete Adapter for Digicel
class DigicelAdapter : public SMSVendorAdapter {
public:
    void dispatchSMS(const std::string& phone, const std::string& text) override {
        // In production this would call Digicel's REST API
        std::cout << "  [Digicel API] -> Sending SMS to " << phone << "\n";
        std::cout << "  [Digicel API] -> Text: " << text << "\n";
    }
    std::string vendorName() const override { return "Digicel"; }
};

// Concrete Adapter for Flow
class FlowAdapter : public SMSVendorAdapter {
public:
    void dispatchSMS(const std::string& phone, const std::string& text) override {
        // In production this would call Flow's REST API
        std::cout << "  [Flow API] -> Sending SMS to " << phone << "\n";
        std::cout << "  [Flow API] -> Text: " << text << "\n";
    }
    std::string vendorName() const override { return "Flow"; }
};

// ============================================================
// SMSNotification
// INHERITANCE: extends NotificationService
// ADAPTER pattern: uses SMSVendorAdapter to talk to carriers
// OBSERVER: reacts to system events
// ============================================================
class SMSNotification : public NotificationService,
                        public NotificationObserver {
private:
    SMSVendorAdapter* vendorAdapter;   // injected adapter (encapsulated)

public:
    // COMPOSITION: SMSNotification owns a vendor adapter
    explicit SMSNotification(SMSVendorAdapter* adapter)
        : NotificationService("SMSService"), vendorAdapter(adapter) {
        if (!adapter) throw std::invalid_argument("SMS vendor adapter cannot be null.");
    }

    // POLYMORPHISM - override of pure virtual
    void sendNotification(const std::string& recipient,
                          const std::string& subject,
                          const std::string& message) override {
        if (recipient.empty()) {
            throw std::invalid_argument("SMS recipient phone number cannot be empty.");
        }

        std::string sms = "[" + subject + "] " + message;
        // Cap SMS at 160 chars
        if (sms.size() > 160) sms = sms.substr(0, 157) + "...";

        std::cout << "\n[SMS][" << vendorAdapter->vendorName()
                  << "] ------------------------------------------\n";
        std::cout << "  To      : " << recipient << "\n";
        std::cout << "  Subject : " << subject   << "\n";
        std::cout << "  Message : " << sms       << "\n";
        std::cout << "  Sent At : " << currentTimestamp() << "\n";

        vendorAdapter->dispatchSMS(recipient, sms);
        std::cout << "[SMS] ------------------------------------------\n";

        logNotification(recipient, subject);
        persistNotification(recipient, subject, sms);
    }

    // Observer callback
    void onEvent(const std::string& eventType,
                 const std::string& recipient,
                 const std::string& details) override {
        std::string subject = buildSubject(eventType);
        sendNotification(recipient, subject, details);
    }

    // Switch vendor at runtime (Strategy-like flexibility)
    void setVendorAdapter(SMSVendorAdapter* adapter) {
        if (!adapter) throw std::invalid_argument("Adapter cannot be null.");
        vendorAdapter = adapter;
    }

    std::string getVendorName() const {
        return vendorAdapter ? vendorAdapter->vendorName() : "None";
    }

    std::string buildSubject(const std::string& eventType) const {
        if (eventType == "APPT_BOOKED")      return "Appt Confirmed";
        if (eventType == "LAB_RESULT")       return "Lab Results Ready";
        if (eventType == "PAYMENT_REMINDER") return "Payment Due";
        if (eventType == "FOLLOW_UP")        return "Follow-Up Reminder";
        if (eventType == "HIGH_RISK_TRIAGE") return "URGENT Triage Alert";
        return "EMHR Notice";
    }

private:
    void persistNotification(const std::string& recipient,
                              const std::string& subject,
                              const std::string& message) const {
        std::ofstream file("notifications.txt", std::ios::app);
        if (file.is_open()) {
            file << currentTimestamp() << "|SMS|"
                 << recipient << "|" << subject << "|" << message << "\n";
            file.close();
        }
    }
};

#endif // EMAIL_NOTIFICATION_H
