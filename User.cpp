/**
 * User.cpp
 * EMHR System - User Management & RBAC
 */

#include "User.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <functional>

// ============================================================
// Private / Static Helpers
// ============================================================

/** Simple deterministic hash (not cryptographic – for demo only).
 *  In production use bcrypt / Argon2. */
std::string User::HashPassword(const std::string& password) {
    std::hash<std::string> hasher;
    std::size_t hashVal = hasher(password + "EMHR_SALT_2026");
    std::ostringstream oss;
    oss << std::hex << hashVal;
    return oss.str();
}

/** Returns current time as ISO-8601 string: 2026-03-01T14:30:00 */
std::string User::CurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(t, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

// ============================================================
// Constructors
// ============================================================

User::User() : isActive(false) {}

User::User(const std::string& id,
           const std::string& uname,
           const std::string& plainPassword,
           const std::string& name,
           const std::string& mail,
           const std::string& ph)
    : userId(id), username(uname),
      fullName(name), email(mail), phone(ph), isActive(true)
{
    passwordHash = HashPassword(plainPassword);
    createdAt    = CurrentTimestamp();
    lastModified = createdAt;
}

// ============================================================
// Accessors
// ============================================================

std::string       User::GetUserId()       const { return userId; }
std::string       User::GetUsername()     const { return username; }
std::string       User::GetFullName()     const { return fullName; }
std::string       User::GetEmail()        const { return email; }
std::string       User::GetPhone()        const { return phone; }
bool              User::GetIsActive()     const { return isActive; }
std::vector<Role> User::GetRoles()        const { return roles; }
std::string       User::GetCreatedAt()    const { return createdAt; }
std::string       User::GetLastModified() const { return lastModified; }
std::string       User::GetPasswordHash() const { return passwordHash; }

// ============================================================
// Mutators  (validation applied)
// ============================================================

bool User::SetFullName(const std::string& name) {
    if (name.empty() || name.length() < 2) {
        std::cerr << "[ERROR] Full name must be at least 2 characters.\n";
        return false;
    }
    fullName     = name;
    lastModified = CurrentTimestamp();
    return true;
}

bool User::SetEmail(const std::string& mail) {
    if (!IsValidEmail(mail)) {
        std::cerr << "[ERROR] Invalid email format.\n";
        return false;
    }
    email        = mail;
    lastModified = CurrentTimestamp();
    return true;
}

bool User::SetPhone(const std::string& ph) {
    // basic: digits/spaces/+/- only, 7–15 chars
    std::string stripped;
    for (char c : ph)
        if (std::isdigit(c) || c == '+' || c == '-' || c == ' ')
            stripped += c;
    if (stripped.length() < 7) {
        std::cerr << "[ERROR] Phone number too short.\n";
        return false;
    }
    phone        = ph;
    lastModified = CurrentTimestamp();
    return true;
}

void User::SetIsActive(bool active) {
    isActive     = active;
    lastModified = CurrentTimestamp();
}

void User::SetLastModified(const std::string& ts) { lastModified = ts; }
void User::SetPasswordHash(const std::string& hash) { passwordHash = hash; }

// ============================================================
// Role / Permission Management
// ============================================================

void User::AddRole(const Role& role) {
    for (const auto& r : roles)
        if (r == role) return;   // no duplicates
    roles.push_back(role);
    lastModified = CurrentTimestamp();
}

void User::RemoveRole(const std::string& roleName) {
    roles.erase(
        std::remove_if(roles.begin(), roles.end(),
            [&](const Role& r){ return r.GetRoleName() == roleName; }),
        roles.end());
    lastModified = CurrentTimestamp();
}

bool User::HasRole(const std::string& roleName) const {
    for (const auto& r : roles)
        if (r.GetRoleName() == roleName) return true;
    return false;
}

bool User::HasPermission(const std::string& permName) const {
    for (const auto& r : roles)
        if (r.HasPermission(permName)) return true;
    return false;
}

// ============================================================
// Authentication
// ============================================================

bool User::ValidatePassword(const std::string& plain) const {
    return HashPassword(plain) == passwordHash;
}

bool User::ChangePassword(const std::string& oldPlain,
                          const std::string& newPlain) {
    if (!ValidatePassword(oldPlain)) {
        std::cerr << "[ERROR] Current password is incorrect.\n";
        return false;
    }
    if (!IsValidPassword(newPlain)) return false;
    passwordHash = HashPassword(newPlain);
    lastModified = CurrentTimestamp();
    return true;
}

// ============================================================
// Static Validators
// ============================================================

/** Password policy: >= 6 chars, at least 1 digit */
bool User::IsValidPassword(const std::string& pw) {
    if (pw.length() < 6) {
        std::cerr << "[ERROR] Password must be at least 6 characters.\n";
        return false;
    }
    bool hasDigit = false;
    for (char c : pw) {
        if (std::isdigit(c)) hasDigit = true;
    }
    if (!hasDigit) {
        std::cerr << "[ERROR] Password needs at least one digit.\n";
        return false;
    }
    return true;
}

bool User::IsValidEmail(const std::string& mail) {
    // minimal check: must have '@' and '.'
    auto atPos = mail.find('@');
    if (atPos == std::string::npos) return false;
    auto dotPos = mail.find('.', atPos);
    if (dotPos == std::string::npos) return false;
    return mail.length() >= 5;
}

// ============================================================
// Polymorphic
// ============================================================

std::string User::GetUserType() const { return "User"; }

void User::Display() const {
    std::cout << "========================================\n";
    std::cout << "  User ID   : " << userId      << "\n";
    std::cout << "  Type      : " << GetUserType()<< "\n";
    std::cout << "  Username  : " << username    << "\n";
    std::cout << "  Full Name : " << fullName    << "\n";
    std::cout << "  Email     : " << email       << "\n";
    std::cout << "  Phone     : " << phone       << "\n";
    std::cout << "  Status    : " << (isActive ? "Active" : "Inactive") << "\n";
    std::cout << "  Created   : " << createdAt   << "\n";
    std::cout << "  Modified  : " << lastModified<< "\n";
    std::cout << "  Roles     :\n";
    if (roles.empty()) {
        std::cout << "    (none)\n";
    } else {
        for (const auto& r : roles)
            std::cout << "    - " << r.GetRoleName() << "\n";
    }
    std::cout << "========================================\n";
}

// ============================================================
// Serialization  (pipe-delimited: field1|field2|...)
// ============================================================

std::string User::Serialize() const {
    std::ostringstream oss;
    // type|id|username|passwordHash|fullName|email|phone|isActive|createdAt|lastModified|roles(comma-sep)
    oss << GetUserType() << "|"
        << userId        << "|"
        << username      << "|"
        << passwordHash  << "|"
        << fullName      << "|"
        << email         << "|"
        << phone         << "|"
        << (isActive ? "1" : "0") << "|"
        << createdAt     << "|"
        << lastModified  << "|";

    for (std::size_t i = 0; i < roles.size(); ++i) {
        if (i) oss << ",";
        oss << roles[i].GetRoleName();
    }
    return oss.str();
}

User User::Deserialize(const std::string& line) {
    // Minimal deserialization – role names only (full rebuild by UserManager)
    std::istringstream ss(line);
    std::string token;
    std::vector<std::string> fields;
    while (std::getline(ss, token, '|')) fields.push_back(token);

    User u;
    if (fields.size() < 11) return u;
    // fields: [0]type [1]id [2]username [3]hash [4]name [5]email [6]phone [7]active [8]created [9]modified [10]roles
    u.userId       = fields[1];
    u.username     = fields[2];
    u.passwordHash = fields[3];
    u.fullName     = fields[4];
    u.email        = fields[5];
    u.phone        = fields[6];
    u.isActive     = (fields[7] == "1");
    u.createdAt    = fields[8];
    u.lastModified = fields[9];
    // roles re-attached by UserManager after loading
    return u;
}
