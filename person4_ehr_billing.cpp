#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iomanip>

// ─────────────────────────────────────────────
//  EHR – Abstract Base: MedicalRecord
// ─────────────────────────────────────────────
class MedicalRecord {
protected:
    static int nextId;
    int    id;
    int    patientId;
    std::string date;

public:
    MedicalRecord(int patientId, const std::string& date)
        : id(nextId++), patientId(patientId), date(date) {}

    virtual ~MedicalRecord() = default;

    int         getId()        const { return id; }
    int         getPatientId() const { return patientId; }
    std::string getDate()      const { return date; }

    virtual std::string getType() const = 0;
    virtual void display()        const = 0;
};
int MedicalRecord::nextId = 1;

// ─────────────────────────────────────────────
//  Vitals
// ─────────────────────────────────────────────
class Vitals : public MedicalRecord {
    double temperature;   // °C
    int    heartRate;     // bpm
    std::string bloodPressure;  // e.g. "120/80"
    double weight;        // kg
    double height;        // cm

public:
    Vitals(int patientId, const std::string& date,
           double temperature, int heartRate,
           const std::string& bloodPressure,
           double weight, double height)
        : MedicalRecord(patientId, date),
          temperature(temperature), heartRate(heartRate),
          bloodPressure(bloodPressure), weight(weight), height(height) {}

    std::string getType() const override { return "Vitals"; }

    void display() const override {
        std::cout << "  [Vitals] ID:" << id << " Patient:" << patientId
                  << " Date:" << date << "\n"
                  << "    Temp:" << temperature << "C  HR:" << heartRate
                  << "bpm  BP:" << bloodPressure
                  << "  Weight:" << weight << "kg  Height:" << height << "cm\n";
    }
};

// ─────────────────────────────────────────────
//  VisitRecord
// ─────────────────────────────────────────────
class VisitRecord : public MedicalRecord {
    std::string diagnosis;
    std::string doctorNotes;
    std::string followUpDate;

public:
    VisitRecord(int patientId, const std::string& date,
                const std::string& diagnosis,
                const std::string& doctorNotes,
                const std::string& followUpDate = "")
        : MedicalRecord(patientId, date),
          diagnosis(diagnosis), doctorNotes(doctorNotes), followUpDate(followUpDate) {}

    std::string getType() const override { return "VisitRecord"; }

    void display() const override {
        std::cout << "  [Visit] ID:" << id << " Patient:" << patientId
                  << " Date:" << date << "\n"
                  << "    Diagnosis  : " << diagnosis    << "\n"
                  << "    Notes      : " << doctorNotes  << "\n"
                  << "    Follow-up  : " << (followUpDate.empty() ? "N/A" : followUpDate) << "\n";
    }
};

// ─────────────────────────────────────────────
//  Prescription
// ─────────────────────────────────────────────
class Prescription : public MedicalRecord {
    std::string medication;
    std::string dosage;
    std::string frequency;
    std::string duration;

public:
    Prescription(int patientId, const std::string& date,
                 const std::string& medication,
                 const std::string& dosage,
                 const std::string& frequency,
                 const std::string& duration)
        : MedicalRecord(patientId, date),
          medication(medication), dosage(dosage),
          frequency(frequency), duration(duration) {}

    std::string getType() const override { return "Prescription"; }

    void display() const override {
        std::cout << "  [Prescription] ID:" << id << " Patient:" << patientId
                  << " Date:" << date << "\n"
                  << "    Drug:" << medication << "  Dose:" << dosage
                  << "  Freq:" << frequency << "  Duration:" << duration << "\n";
    }
};

// ─────────────────────────────────────────────
//  LabResult
// ─────────────────────────────────────────────
class LabResult : public MedicalRecord {
    std::string testName;
    std::string result;
    std::string referenceRange;
    bool abnormal;

public:
    LabResult(int patientId, const std::string& date,
              const std::string& testName,
              const std::string& result,
              const std::string& referenceRange,
              bool abnormal = false)
        : MedicalRecord(patientId, date),
          testName(testName), result(result),
          referenceRange(referenceRange), abnormal(abnormal) {}

    std::string getType()   const override { return "LabResult"; }
    bool        isAbnormal() const         { return abnormal; }

    void display() const override {
        std::cout << "  [Lab] ID:" << id << " Patient:" << patientId
                  << " Date:" << date << "\n"
                  << "    Test:" << testName << "  Result:" << result
                  << "  Ref:" << referenceRange
                  << (abnormal ? "  *** ABNORMAL ***" : "") << "\n";
    }
};

// ─────────────────────────────────────────────
//  ImmunizationRecord (stored inside EHR)
// ─────────────────────────────────────────────
struct Immunization {
    int patientId;
    std::string vaccine;
    std::string dateGiven;
    std::string nextDueDate;
};

// ─────────────────────────────────────────────
//  Allergy (stored inside EHR)
// ─────────────────────────────────────────────
struct Allergy {
    int patientId;
    std::string allergen;
    std::string reaction;
};

// ─────────────────────────────────────────────
//  EHRSystem
// ─────────────────────────────────────────────
class EHRSystem {
    std::vector<std::shared_ptr<MedicalRecord>> records;
    std::vector<Immunization> immunizations;
    std::vector<Allergy>      allergies;

public:
    // Generic add
    void addRecord(std::shared_ptr<MedicalRecord> rec) {
        records.push_back(rec);
        std::cout << "[EHR] Added " << rec->getType() << " (ID:" << rec->getId() << ")\n";
    }

    void addImmunization(int patientId, const std::string& vaccine,
                         const std::string& dateGiven, const std::string& nextDue = "") {
        immunizations.push_back({patientId, vaccine, dateGiven, nextDue});
        std::cout << "[EHR] Immunization recorded: " << vaccine << " for patient " << patientId << "\n";
    }

    void addAllergy(int patientId, const std::string& allergen, const std::string& reaction) {
        allergies.push_back({patientId, allergen, reaction});
        std::cout << "[EHR] Allergy recorded: " << allergen << " for patient " << patientId << "\n";
    }

    void viewPatientEHR(int patientId) const {
        std::cout << "\n  === EHR for Patient " << patientId << " ===\n";
        bool anyRecord = false;
        for (const auto& rec : records) {
            if (rec->getPatientId() == patientId) {
                rec->display();
                anyRecord = true;
            }
        }
        if (!anyRecord) std::cout << "  No medical records.\n";

        std::cout << "\n  -- Immunizations --\n";
        bool anyImm = false;
        for (const auto& imm : immunizations) {
            if (imm.patientId == patientId) {
                std::cout << "    " << imm.vaccine << "  Given:" << imm.dateGiven
                          << "  NextDue:" << (imm.nextDueDate.empty() ? "N/A" : imm.nextDueDate) << "\n";
                anyImm = true;
            }
        }
        if (!anyImm) std::cout << "    None.\n";

        std::cout << "\n  -- Allergies --\n";
        bool anyAllergy = false;
        for (const auto& a : allergies) {
            if (a.patientId == patientId) {
                std::cout << "    " << a.allergen << " -> " << a.reaction << "\n";
                anyAllergy = true;
            }
        }
        if (!anyAllergy) std::cout << "    None.\n";
    }
};

// ─────────────────────────────────────────────
//  BILLING – Strategy Pattern
// ─────────────────────────────────────────────

// Abstract payment strategy
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    // Returns the amount actually charged (remaining after this method)
    virtual double processPayment(double amountDue) = 0;
    virtual std::string getName() const = 0;
};

class CashPayment : public PaymentStrategy {
    double cashTendered;
public:
    explicit CashPayment(double cashTendered) : cashTendered(cashTendered) {}

    double processPayment(double amountDue) override {
        if (cashTendered >= amountDue) {
            double change = cashTendered - amountDue;
            std::cout << "    [Cash] Paid $" << std::fixed << std::setprecision(2)
                      << amountDue << "  Change: $" << change << "\n";
            return 0.0;
        } else {
            std::cout << "    [Cash] Partial payment $" << cashTendered
                      << "  Remaining: $" << (amountDue - cashTendered) << "\n";
            return amountDue - cashTendered;
        }
    }
    std::string getName() const override { return "Cash"; }
};

class CardPayment : public PaymentStrategy {
    std::string cardLastFour;
public:
    explicit CardPayment(const std::string& lastFour) : cardLastFour(lastFour) {}

    double processPayment(double amountDue) override {
        std::cout << "    [Card] Charged $" << std::fixed << std::setprecision(2)
                  << amountDue << " to card ending " << cardLastFour << "\n";
        return 0.0;
    }
    std::string getName() const override { return "Card"; }
};

class InsurancePayment : public PaymentStrategy {
    double coveragePercent;  // e.g. 0.80 for 80%
    double deductible;
    double perVisitCap;
    double deductiblePaid;   // tracks how much deductible has been met

public:
    InsurancePayment(double coveragePercent, double deductible, double perVisitCap)
        : coveragePercent(coveragePercent), deductible(deductible),
          perVisitCap(perVisitCap), deductiblePaid(0.0) {}

    double processPayment(double amountDue) override {
        // Apply deductible first
        double afterDeductible = amountDue;
        if (deductiblePaid < deductible) {
            double remaining = deductible - deductiblePaid;
            double applied   = std::min(remaining, amountDue);
            afterDeductible -= applied;
            deductiblePaid  += applied;
            std::cout << "    [Insurance] Deductible applied: $" << applied
                      << "  Deductible met: $" << deductiblePaid << "/$" << deductible << "\n";
        }

        // Apply per-visit cap
        double coverable = std::min(afterDeductible, perVisitCap);
        double covered   = coverable * coveragePercent;
        double patientOwes = amountDue - covered;

        std::cout << "    [Insurance] Covered: $" << std::fixed << std::setprecision(2)
                  << covered << " (" << (int)(coveragePercent * 100) << "%)"
                  << "  Patient owes: $" << patientOwes << "\n";
        return patientOwes;
    }
    std::string getName() const override { return "Insurance"; }
};

// ─────────────────────────────────────────────
//  PaymentProcessor (uses strategy)
// ─────────────────────────────────────────────
class PaymentProcessor {
public:
    // Processes a chain of payment strategies until bill is settled (mixed payments)
    void processPayments(double totalAmount,
                         std::vector<std::shared_ptr<PaymentStrategy>>& strategies) {
        std::cout << "  Bill total: $" << std::fixed << std::setprecision(2) << totalAmount << "\n";
        double remaining = totalAmount;

        for (auto& strategy : strategies) {
            if (remaining <= 0.0) break;
            std::cout << "  Using " << strategy->getName() << ":\n";
            remaining = strategy->processPayment(remaining);
        }

        if (remaining <= 0.0) {
            std::cout << "  [PAID IN FULL]\n";
        } else {
            std::cout << "  [OUTSTANDING BALANCE] $"
                      << std::fixed << std::setprecision(2) << remaining << "\n";
        }
    }
};

// ─────────────────────────────────────────────
//  BillingSystem
// ─────────────────────────────────────────────
struct Invoice {
    static int nextId;
    int    id;
    int    patientId;
    double amount;
    bool   paid;
    std::string description;
};
int Invoice::nextId = 1;

class BillingSystem {
    std::vector<Invoice> invoices;
    PaymentProcessor processor;

public:
    int createInvoice(int patientId, double amount, const std::string& description) {
        Invoice inv;
        inv.id          = Invoice::nextId++;
        inv.patientId   = patientId;
        inv.amount      = amount;
        inv.paid        = false;
        inv.description = description;
        invoices.push_back(inv);
        std::cout << "[Billing] Invoice " << inv.id << " created for patient "
                  << patientId << " - $" << std::fixed << std::setprecision(2)
                  << amount << "\n";
        return inv.id;
    }

    void payInvoice(int invoiceId,
                    std::vector<std::shared_ptr<PaymentStrategy>> strategies) {
        for (auto& inv : invoices) {
            if (inv.id == invoiceId) {
                std::cout << "\n[Billing] Processing invoice " << invoiceId
                          << " (" << inv.description << "):\n";
                processor.processPayments(inv.amount, strategies);
                inv.paid = true;
                return;
            }
        }
        std::cout << "[ERROR] Invoice " << invoiceId << " not found.\n";
    }

    void viewInvoices(int patientId) const {
        std::cout << "\n  -- Invoices for patient " << patientId << " --\n";
        for (const auto& inv : invoices) {
            if (inv.patientId == patientId) {
                std::cout << "  Invoice " << inv.id
                          << " | $" << std::fixed << std::setprecision(2) << inv.amount
                          << " | " << inv.description
                          << " | " << (inv.paid ? "PAID" : "UNPAID") << "\n";
            }
        }
    }
};

// ─────────────────────────────────────────────
//  Demo / main
// ─────────────────────────────────────────────
int main() {
    std::cout << "=== Person 4: EHR & Billing System ===\n\n";

    EHRSystem ehr;
    BillingSystem billing;

    int patientId = 101; // Assume patient already exists from Person 3

    // --- EHR ---
    std::cout << "-- Adding EHR records for patient " << patientId << " --\n";

    ehr.addRecord(std::make_shared<Vitals>(
        patientId, "2026-04-20", 37.1, 72, "118/76", 70.5, 175.0));

    ehr.addRecord(std::make_shared<VisitRecord>(
        patientId, "2026-04-20",
        "Hypertension Stage 1",
        "Patient reports headaches. Prescribed medication.",
        "2026-05-20"));

    ehr.addRecord(std::make_shared<Prescription>(
        patientId, "2026-04-20",
        "Amlodipine", "5mg", "Once daily", "30 days"));

    ehr.addRecord(std::make_shared<LabResult>(
        patientId, "2026-04-20",
        "Blood Glucose", "6.8 mmol/L", "3.9–5.6 mmol/L", true));

    ehr.addImmunization(patientId, "Influenza",    "2025-10-01", "2026-10-01");
    ehr.addImmunization(patientId, "COVID-19",     "2024-03-15");
    ehr.addAllergy(patientId,      "Penicillin",   "Anaphylaxis");
    ehr.addAllergy(patientId,      "Peanuts",      "Hives");

    ehr.viewPatientEHR(patientId);

    // --- Billing ---
    std::cout << "\n-- Billing --\n";
    int inv1 = billing.createInvoice(patientId, 250.00, "General consultation + labs");
    int inv2 = billing.createInvoice(patientId, 180.00, "Follow-up + blood pressure monitoring");

    // Pay invoice 1 with Insurance (80%, $50 deductible, $300 cap) then Card for remainder
    std::vector<std::shared_ptr<PaymentStrategy>> strategies1 = {
        std::make_shared<InsurancePayment>(0.80, 50.00, 300.00),
        std::make_shared<CardPayment>("4242")
    };
    billing.payInvoice(inv1, strategies1);

    // Pay invoice 2 with Cash
    std::vector<std::shared_ptr<PaymentStrategy>> strategies2 = {
        std::make_shared<CashPayment>(200.00)
    };
    billing.payInvoice(inv2, strategies2);

    billing.viewInvoices(patientId);

    return 0;
}
