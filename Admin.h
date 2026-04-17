/**
 * Admin.h
 * EMHR System - User Management & RBAC
 * Author  : Ishmael
 * Date    : March 2026
 * Desc    : Admin IS-A User (inheritance). Overrides GetUserType() and
 *           Display() to demonstrate polymorphism / method overriding.
 *           Admin has exclusive privileges like createUser, deleteUser,
 *           updateSystemConfig.
 */

#pragma once
#include "User.h"

class Admin : public User {
private:
    std::string department;
    std::string adminLevel;   // "SuperAdmin", "DepartmentAdmin"

public:
    // ----- Constructors -----
    Admin();
    Admin(const std::string& id,
          const std::string& username,
          const std::string& plainPassword,
          const std::string& fullName,
          const std::string& email,
          const std::string& phone,
          const std::string& department = "IT",
          const std::string& level      = "DepartmentAdmin");

    // ----- Accessors -----
    std::string GetDepartment() const;
    std::string GetAdminLevel() const;

    // ----- Mutators -----
    bool SetDepartment(const std::string& dept);
    bool SetAdminLevel(const std::string& level);

    // ----- Overrides (Polymorphism) -----
    std::string GetUserType() const override;
    void        Display()     const override;
    std::string Serialize()   const override;
};
