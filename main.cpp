
#include <iostream>


#include "Patient.h"
#include "Appointment.h"
#include "PatientRegistry.h"
#include "ScheduleManager.h"
using namespace std;


int main() {
    std::cout << "=== Patient Registry & Appointments ===\n\n";

    PatientRegistry registry;
    ScheduleManager scheduler;

    // --- Patient CRUD ---
    std::cout << "-- Creating patients --\n";
    int id1 = registry.createPatient("Alice Brown",  "10 King St",    "555-1001", "Bob Brown");
    int id2 = registry.createPatient("Carlos Diaz",  "22 Queen Ave",  "555-2002", "Maria Diaz");
    int id3 = registry.createPatient("Diana Prince", "33 Hero Blvd",  "555-3003", "Steve Trevor");

    std::cout << "\n-- All patients --\n";
    registry.viewAll();

    std::cout << "\n-- Update patient " << id1 << " --\n";
    registry.updatePatient(id1, "Alice Green", "10 King St, Apt 2", "555-1001", "Bob Brown");

    std::cout << "\n-- Read patient " << id1 << " --\n";
    Patient* p = registry.readPatient(id1);
    if (p) p->display();

    std::cout << "\n-- Delete patient " << id3 << " --\n";
    registry.deletePatient(id3);

    std::cout << "\n-- All patients after delete --\n";
    registry.viewAll();

    // --- Appointments ---
    std::cout << "\n-- Booking appointments --\n";
    scheduler.bookAppointment(id1, "2026-04-20 09:00", 30, "General check-up");
    scheduler.bookAppointment(id2, "2026-04-20 09:20", 30, "Follow-up");  // should conflict (buffer)
    scheduler.bookAppointment(id2, "2026-04-20 09:45", 30, "Follow-up");  // ok (after buffer)
    scheduler.bookAppointment(id1, "2026-04-21 10:00", 30, "Blood test");

    std::cout << "\n-- All appointments --\n";
    scheduler.viewAll();

    std::cout << "\n-- Appointments for patient " << id1 << " --\n";
    scheduler.viewByPatient(id1);

    std::cout << "\n-- Update appointment 1 to 14:00 --\n";
    scheduler.updateAppointment(1, "2026-04-20 14:00");

    std::cout << "\n-- Cancel appointment 3 --\n";
    scheduler.cancelAppointment(3);

    std::cout << "\n-- All appointments after changes --\n";
    scheduler.viewAll();
    
    
    
    
    
    
    

    return 0;
}