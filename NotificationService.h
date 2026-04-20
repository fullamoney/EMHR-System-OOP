// ============================================================
// File: NotificationService.h
// Description: Abstract base class for notification services,
//              Observer pattern interfaces
// Project: EMHR Healthcare System - Semester 2 AY2025/2026
// ============================================================

#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

// ============================================================
// Abstract base class: NotificationService
// Demonstrates ABSTRACTION - pure virtual method forces
// every subclass to implement sendNotification()
// ============================================================
class NotificationService {
protected:
    string serviceName;

public:
    NotificationService(const string& name) : serviceName(name) {}
    virtual ~NotificationService() {}

    // Pure virtual method - ABSTRACTION
    virtual void sendNotification(const string& recipient,
                                  const string& subject,
                                  const string& message) = 0;

    virtual string getServiceName() const { return serviceName; }

    // Shared helper available to all subclasses
    void logNotification(const string& recipient,
                         const string& subject) const {
        cout << "[LOG][" << serviceName << "] Notification sent to: "
             << recipient << " | Subject: " << subject << "\n";
    }
};

// ============================================================
// Observer interface (abstract)
// Part of Observer pattern - all observers must implement onEvent
// ============================================================
class NotificationObserver {
public:
    virtual ~NotificationObserver() {}
    virtual void onEvent(const string& eventType,
                         const string& recipient,
                         const string& details) = 0;
};

// ============================================================
// NotificationSubject (Publisher)
// Holds list of observers and notifies them on system events
// ============================================================
class NotificationSubject {
private:
    vector<NotificationObserver*> observers;

public:
    void addObserver(NotificationObserver* obs) {
        observers.push_back(obs);
    }

    void removeObserver(NotificationObserver* obs) {
        for (auto it = observers.begin(); it != observers.end(); ++it) {
            if (*it == obs) { observers.erase(it); break; }
        }
    }

    void notifyObservers(const string& eventType,
                         const string& recipient,
                         const string& details) {
        for (auto* obs : observers) {
            obs->onEvent(eventType, recipient, details);
        }
    }
};

#endif // NOTIFICATION_SERVICE_H
