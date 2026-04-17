/**
 * UserManager.cpp
 * EMHR System - User Management & RBAC
 */

#include "UserManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <limits>
#include <filesystem>

// ============================================================
// Constructor & Role Initialization
// ============================================================

UserManager::UserManager(const std::string& dataDir)
    : usersFilePath(dataDir + "/users.dat"),
      auditFilePath(dataDir + "/audit.log")
{
    InitSystemRoles();
    LoadFromFile();
}

void UserManager::InitSystemRoles() {
    // ---- Admin Role ----
    Role adminRole("Admin");
    adminRole.AddPermission(Permission("createUser",         "Create users"));
    adminRole.AddPermission(Permission("deleteUser",         "Delete users"));
    adminRole.AddPermission(Permission("updateSystemConfig", "Modify system config"));
    adminRole.AddPermission(Permission("viewReports",        "View all reports"));
    adminRole.AddPermission(Permission("auditLogs",          "Read audit trail"));
    adminRole.AddPermission(Permission("approveTransaction", "Approve transactions"));
    systemRoles.push_back(adminRole);

    // ---- Manager Role ----
    Role managerRole("Manager");
    managerRole.AddPermission(Permission("approveTransaction","Approve transactions"));
    managerRole.AddPermission(Permission("viewReports",       "View reports"));
    managerRole.AddPermission(Permission("viewAllPatients",   "View all patient data"));
    systemRoles.push_back(managerRole);

    // ---- User Role ----
    Role userRole("User");
    userRole.AddPermission(Permission("viewOwnData", "View own records"));
    userRole.AddPermission(Permission("bookAppointment", "Book appointments"));
    systemRoles.push_back(userRole);
}

Role UserManager::GetSystemRole(const std::string& roleName) const {
    for (const auto& r : systemRoles)
        if (r.GetRoleName() == roleName) return r;
    return Role("Unknown");   // fallback
}

// ============================================================
// Timestamp Helper
// ============================================================

std::string UserManager::CurrentTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(t, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

// ============================================================
// ID Generator
// ============================================================

std::string UserManager::GenerateUserId() {
    // USR-XXXX  (padded sequential)
    int count = static_cast<int>(users.size()) + 1;
    std::ostringstream oss;
    oss << "USR-" << std::setw(4) << std::setfill('0') << count;
    return oss.str();
}

// ============================================================
// Duplicate Check
// ============================================================

bool UserManager::UsernameExists(const std::string& username) const {
    for (const auto& pair : users)
        if (pair.second->GetUsername() == username) return true;
    return false;
}

// ============================================================
// Audit Log
// ============================================================

void UserManager::WriteAuditLog(const std::string& actor,
                                const std::string& action,
                                const std::string& target,
                                const std::string& detail) {
    std::ofstream file(auditFilePath, std::ios::app);
    if (!file.is_open()) return;
    file << CurrentTimestamp()
         << " | ACTOR=" << actor
         << " | ACTION=" << action
         << " | TARGET=" << target;
    if (!detail.empty()) file << " | DETAIL=" << detail;
    file << "\n";
    file.close();
}

// ============================================================
// File Persistence
// ============================================================

void UserManager::SaveToFile() const {
    // Create data directory if it doesn't exist
    std::filesystem::path dirPath = std::filesystem::path(usersFilePath).parent_path();
    if (!dirPath.empty()) {
        std::filesystem::create_directories(dirPath);
    }
    
    std::ofstream file(usersFilePath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot open users file for writing: "
                  << usersFilePath << "\n";
        return;
    }
    for (const auto& pair : users) {
        file << pair.second->Serialize() << "\n";
    }
    file.close();
}

void UserManager::LoadFromFile() {
    std::ifstream file(usersFilePath);
    if (!file.is_open()) return;   // file doesn't exist yet – first run

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> fields;
        while (std::getline(ss, field, '|')) fields.push_back(field);

        if (fields.size() < 11) continue;

        std::string type  = fields[0];
        std::string rolesField = fields[10];

        std::shared_ptr<User> u;
        if (type == "Admin" && fields.size() >= 13) {
            auto a = std::make_shared<Admin>();
            // Directly restore admin-specific fields
            a->SetDepartment(fields[11]);
            a->SetAdminLevel(fields[12]);
            u = a;
        } else {
            u = std::make_shared<User>();
        }

        // Restore base fields via Deserialize
        User base = User::Deserialize(line);
        // copy base fields – use a trick: re-assign via setters / direct copy
        // (in a real project we'd use a factory; this keeps it simple)
        *u = base;          // works because Admin inherits all base data

        // Restore roles
        std::istringstream rss(rolesField);
        std::string rname;
        while (std::getline(rss, rname, ',')) {
            if (!rname.empty())
                u->AddRole(GetSystemRole(rname));
        }

        users[u->GetUserId()] = u;
    }
    file.close();
}

// ============================================================
// CRUD – Add User
// ============================================================

bool UserManager::AddUser(const std::string& actorId,
                          const std::string& username,
                          const std::string& plainPassword,
                          const std::string& fullName,
                          const std::string& email,
                          const std::string& phone,
                          const std::string& roleChoice,
                          const std::string& dept) {
    // Permission check
    if (!CheckPermission(actorId, "createUser")) {
        std::cerr << "[DENIED] You do not have permission to create users.\n";
        return false;
    }

    // Duplicate username check
    if (UsernameExists(username)) {
        std::cerr << "[ERROR] Username '" << username << "' already exists.\n";
        return false;
    }

    // Password validation
    if (!User::IsValidPassword(plainPassword)) return false;

    // Email validation
    if (!User::IsValidEmail(email)) {
        std::cerr << "[ERROR] Invalid email address.\n";
        return false;
    }

    std::string newId = GenerateUserId();
    std::shared_ptr<User> newUser;

    if (roleChoice == "Admin") {
        auto a = std::make_shared<Admin>(newId, username, plainPassword,
                                         fullName, email, phone, dept);
        newUser = a;
    } else {
        newUser = std::make_shared<User>(newId, username, plainPassword,
                                          fullName, email, phone);
        newUser->AddRole(GetSystemRole(roleChoice));
    }

    users[newId] = newUser;
    SaveToFile();
    WriteAuditLog(actorId, "ADD_USER", newId,
                  "username=" + username + " role=" + roleChoice);

    std::cout << "[OK] User '" << username << "' created with ID: " << newId << "\n";
    return true;
}

// ============================================================
// CRUD – Update User (interactive)
// ============================================================

bool UserManager::UpdateUser(const std::string& actorId,
                             const std::string& targetUserId) {
    // Permission: Admins can update anyone; users can update themselves
    bool isSelf = (actorId == targetUserId);
    if (!isSelf && !CheckPermission(actorId, "createUser")) {
        std::cerr << "[DENIED] No permission to update other users.\n";
        return false;
    }

    auto it = users.find(targetUserId);
    if (it == users.end()) {
        std::cerr << "[ERROR] User ID not found: " << targetUserId << "\n";
        return false;
    }

    auto& u = it->second;
    std::cout << "\n--- Update User: " << u->GetUsername() << " ---\n";
    std::cout << " 1. Full Name  [" << u->GetFullName() << "]\n";
    std::cout << " 2. Email      [" << u->GetEmail()    << "]\n";
    std::cout << " 3. Phone      [" << u->GetPhone()    << "]\n";
    std::cout << " 4. Password\n";
    std::cout << " 5. Active Status [" << (u->GetIsActive() ? "Active" : "Inactive") << "]\n";
    std::cout << " 0. Cancel\n";
    std::cout << "Choice: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string input;
    bool changed = false;

    switch (choice) {
        case 1:
            std::cout << "New full name: ";
            std::getline(std::cin, input);
            changed = u->SetFullName(input);
            break;
        case 2:
            std::cout << "New email: ";
            std::getline(std::cin, input);
            changed = u->SetEmail(input);
            break;
        case 3:
            std::cout << "New phone: ";
            std::getline(std::cin, input);
            changed = u->SetPhone(input);
            break;
        case 4: {
            std::string oldPw, newPw;
            std::cout << "Current password: ";
            std::getline(std::cin, oldPw);
            std::cout << "New password: ";
            std::getline(std::cin, newPw);
            changed = u->ChangePassword(oldPw, newPw);
            break;
        }
        case 5: {
            bool newStatus = !u->GetIsActive();
            u->SetIsActive(newStatus);
            changed = true;
            std::cout << "[OK] Status set to "
                      << (newStatus ? "Active" : "Inactive") << "\n";
            break;
        }
        case 0:
            std::cout << "Update cancelled.\n";
            return false;
        default:
            std::cerr << "[ERROR] Invalid choice.\n";
            return false;
    }

    if (changed) {
        SaveToFile();
        WriteAuditLog(actorId, "UPDATE_USER", targetUserId,
                      "field=" + std::to_string(choice));
        std::cout << "[OK] User updated successfully.\n";
    }
    return changed;
}

// ============================================================
// CRUD – Delete User
// ============================================================

bool UserManager::DeleteUser(const std::string& actorId,
                             const std::string& targetUserId) {
    if (!CheckPermission(actorId, "deleteUser")) {
        std::cerr << "[DENIED] You do not have permission to delete users.\n";
        return false;
    }

    if (actorId == targetUserId) {
        std::cerr << "[ERROR] You cannot delete your own account.\n";
        return false;
    }

    auto it = users.find(targetUserId);
    if (it == users.end()) {
        std::cerr << "[ERROR] User ID not found: " << targetUserId << "\n";
        return false;
    }

    std::string uname = it->second->GetUsername();
    std::cout << "Are you sure you want to delete '" << uname << "'? (y/n): ";
    char confirm;
    std::cin >> confirm;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (confirm != 'y' && confirm != 'Y') {
        std::cout << "Delete cancelled.\n";
        return false;
    }

    users.erase(it);
    SaveToFile();
    WriteAuditLog(actorId, "DELETE_USER", targetUserId, "username=" + uname);
    std::cout << "[OK] User '" << uname << "' deleted.\n";
    return true;
}

// ============================================================
// View Operations
// ============================================================

void UserManager::ViewUser(const std::string& actorId,
                           const std::string& targetUserId) const {
    bool isSelf = (actorId == targetUserId);
    if (!isSelf && !CheckPermission(actorId, "viewReports")) {
        std::cerr << "[DENIED] No permission to view other user profiles.\n";
        return;
    }

    auto it = users.find(targetUserId);
    if (it == users.end()) {
        std::cerr << "[ERROR] User ID not found: " << targetUserId << "\n";
        return;
    }
    it->second->Display();   // polymorphic call
}

void UserManager::ViewAllUsers(const std::string& actorId) const {
    if (!CheckPermission(actorId, "viewReports")) {
        std::cerr << "[DENIED] No permission to view all users.\n";
        return;
    }

    if (users.empty()) {
        std::cout << "No users in the system.\n";
        return;
    }

    std::cout << "\n========== ALL USERS (" << users.size() << ") ==========\n";
    for (const auto& pair : users) {
        std::cout << "ID: " << pair.second->GetUserId()
                  << "  | Username: " << std::setw(15) << std::left
                  << pair.second->GetUsername()
                  << "  | Type: " << std::setw(8) << pair.second->GetUserType()
                  << "  | Status: "
                  << (pair.second->GetIsActive() ? "Active" : "Inactive") << "\n";
    }
    std::cout << "==========================================\n";
}

// ============================================================
// Authentication / Login
// ============================================================

std::shared_ptr<User> UserManager::Login(const std::string& username,
                                          const std::string& plainPassword) {
    for (const auto& pair : users) {
        if (pair.second->GetUsername() == username) {
            if (!pair.second->GetIsActive()) {
                std::cerr << "[ERROR] Account is deactivated.\n";
                return nullptr;
            }
            if (pair.second->ValidatePassword(plainPassword)) {
                WriteAuditLog(pair.second->GetUserId(), "LOGIN", username, "success");
                return pair.second;
            } else {
                WriteAuditLog("UNKNOWN", "LOGIN_FAIL", username, "bad_password");
                std::cerr << "[ERROR] Incorrect password.\n";
                return nullptr;
            }
        }
    }
    WriteAuditLog("UNKNOWN", "LOGIN_FAIL", username, "user_not_found");
    std::cerr << "[ERROR] Username not found.\n";
    return nullptr;
}

// ============================================================
// RBAC Permission Check
// ============================================================

bool UserManager::CheckPermission(const std::string& userId,
                                  const std::string& permName) const {
    auto it = users.find(userId);
    if (it == users.end()) return false;
    return it->second->HasPermission(permName);
}

// ============================================================
// Audit Log Viewer
// ============================================================

void UserManager::ViewAuditLog(const std::string& actorId) const {
    if (!CheckPermission(actorId, "auditLogs")) {
        std::cerr << "[DENIED] No permission to view audit logs.\n";
        return;
    }

    std::ifstream file(auditFilePath);
    if (!file.is_open()) {
        std::cout << "No audit log entries yet.\n";
        return;
    }

    std::cout << "\n========== AUDIT LOG ==========\n";
    std::string line;
    while (std::getline(file, line)) std::cout << line << "\n";
    std::cout << "================================\n";
    file.close();
}

// ============================================================
// Role Assignment
// ============================================================

bool UserManager::AssignRole(const std::string& actorId,
                             const std::string& targetId,
                             const std::string& roleName) {
    if (!CheckPermission(actorId, "updateSystemConfig")) {
        std::cerr << "[DENIED] No permission to assign roles.\n";
        return false;
    }

    auto it = users.find(targetId);
    if (it == users.end()) {
        std::cerr << "[ERROR] User not found.\n";
        return false;
    }

    Role role = GetSystemRole(roleName);
    if (role.GetRoleName() == "Unknown") {
        std::cerr << "[ERROR] Role '" << roleName << "' does not exist.\n";
        return false;
    }

    it->second->AddRole(role);
    SaveToFile();
    WriteAuditLog(actorId, "ASSIGN_ROLE", targetId, "role=" + roleName);
    std::cout << "[OK] Role '" << roleName << "' assigned to user "
              << it->second->GetUsername() << "\n";
    return true;
}

// ============================================================
// Lookup Helpers
// ============================================================

std::shared_ptr<User> UserManager::FindById(const std::string& id) const {
    auto it = users.find(id);
    return (it != users.end()) ? it->second : nullptr;
}

std::shared_ptr<User> UserManager::FindByUsername(const std::string& un) const {
    for (const auto& pair : users)
        if (pair.second->GetUsername() == un) return pair.second;
    return nullptr;
}

int UserManager::GetUserCount() const {
    return static_cast<int>(users.size());
}

bool UserManager::BootstrapFirstAdmin(const std::string& username,
                                       const std::string& plainPassword,
                                       const std::string& fullName,
                                       const std::string& email,
                                       const std::string& phone,
                                       const std::string& dept) {
    if (!users.empty()) {
        std::cerr << "[ERROR] Bootstrap only allowed on empty system.\n";
        return false;
    }
    if (UsernameExists(username)) return false;
    if (!User::IsValidPassword(plainPassword)) return false;
    if (!User::IsValidEmail(email)) return false;

    std::string newId = "USR-0001";
    auto a = std::make_shared<Admin>(newId, username, plainPassword,
                                      fullName, email, phone, dept, "SuperAdmin");
    users[newId] = a;
    SaveToFile();
    WriteAuditLog("SYSTEM", "BOOTSTRAP_ADMIN", newId, "username=" + username);
    std::cout << "[OK] Bootstrap admin '" << username << "' created (ID: " << newId << ").\n";
    return true;
}
