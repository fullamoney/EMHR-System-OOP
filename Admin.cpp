/**
 * Admin.cpp
 * EMHR System - User Management & RBAC
 */

#include "Admin.h"
#include <iostream>

// ============================================================
// Constructors
// ============================================================

Admin::Admin() {}

Admin::Admin(const std::string& id,
             const std::string& uname,
             const std::string& plainPassword,
             const std::string& name,
             const std::string& mail,
             const std::string& ph,
             const std::string& dept,
             const std::string& level)
    : User(id, uname, plainPassword, name, mail, ph),
      department(dept), adminLevel(level)
{
    // Pre-load Admin role with its permissions
    Role adminRole("Admin");
    adminRole.AddPermission(Permission("createUser",         "Create new system users"));
    adminRole.AddPermission(Permission("deleteUser",         "Delete system users"));
    adminRole.AddPermission(Permission("updateSystemConfig", "Modify system settings"));
    adminRole.AddPermission(Permission("viewReports",        "View system reports"));
    adminRole.AddPermission(Permission("auditLogs",          "Access audit trail"));
    AddRole(adminRole);
}

// ============================================================
// Accessors
// ============================================================

std::string Admin::GetDepartment() const { return department; }
std::string Admin::GetAdminLevel() const { return adminLevel; }

// ============================================================
// Mutators
// ============================================================

bool Admin::SetDepartment(const std::string& dept) {
    if (dept.empty()) {
        std::cerr << "[ERROR] Department cannot be empty.\n";
        return false;
    }
    department = dept;
    return true;
}

bool Admin::SetAdminLevel(const std::string& level) {
    if (level != "SuperAdmin" && level != "DepartmentAdmin") {
        std::cerr << "[ERROR] Admin level must be 'SuperAdmin' or 'DepartmentAdmin'.\n";
        return false;
    }
    adminLevel = level;
    return true;
}

// ============================================================
// Overrides
// ============================================================

std::string Admin::GetUserType() const { return "Admin"; }

void Admin::Display() const {
    User::Display();   // call base display first (reuse)
    std::cout << "  Department: " << department << "\n";
    std::cout << "  Level     : " << adminLevel << "\n";
    std::cout << "========================================\n";
}

std::string Admin::Serialize() const {
    // Extend base serialization with admin-specific fields
    return User::Serialize() + "|" + department + "|" + adminLevel;
}
