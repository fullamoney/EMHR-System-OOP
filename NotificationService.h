// ============================================================
// File: NotificationService.h
// Description: Abstract base class for notification services
// Project: EMHR Healthcare System - Semester 2 AY2025/2026
// ============================================================

#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <string>
#include <vector>
#include <iostream>

// ---- Abstract base: NotificationService ----
// Demonstrates ABSTRACTION - defines interface without implementation
class NotificationService {
protected:
    std::string serviceName;

public:
    NotificationService(const std::string& name) : serviceName(name) {}
    virtual ~NotificationService() {}

    // Pure virtual - forces subclasses to implement
    virtual void sendNotification(const std::string& recipient,
                                  const std::string& subject,
                                  const std::string& message) = 0;

    virtual std::string getServiceName() const { return serviceName; }

    // Concrete helper shared by all subclasses
    void logNotification(const std::string& recipient, const std::string& subject) const {
        std::cout << "[LOG][" << serviceName << "] Notification sent to: "
                  << recipient << " | Subject: " << subject << "\n";
    }
};

// ---- Observer interface ----
// Part of Observer pattern: observers react to system events
class NotificationObserver {
public:
    virtual ~NotificationObserver() {}
    virtual void onEvent(const std::string& eventType,
                         const std::string& recipient,
                         const std::string& details) = 0;
};

// ---- Subject (Publisher) ----
// Holds a list of observers and notifies them on events
class NotificationSubject {
private:
    std::vector<NotificationObserver*> observers;

public:
    void addObserver(NotificationObserver* obs) {
        observers.push_back(obs);
    }

    void removeObserver(NotificationObserver* obs) {
        for (auto it = observers.begin(); it != observers.end(); ++it) {
            if (*it == obs) { observers.erase(it); break; }
        }
    }

    // Notify all registered observers
    void notifyObservers(const std::string& eventType,
                         const std::string& recipient,
                         const std::string& details) {
        for (auto* obs : observers) {
            obs->onEvent(eventType, recipient, details);
        }
    }
};

#endif // NOTIFICATION_SERVICE_H
