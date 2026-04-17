/**
 * UserManager.h
 * EMHR System - User Management & RBAC
 * Author  : Ishmael Moulton
 * Date    : March 2026
 * Desc    : Central controller for all User operations.
 *           Implements Repository pattern: in-memory store + file persistence.
 *           Handles: Add, Update, View, ViewAll, Delete, Audit Logs, RBAC.
 */

#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "User.h"
#include "Admin.h"

class UserManager {
private:
    // ---- In-memory store (user-id → User) ----
    std::map<std::string, std::shared_ptr<User>> users;

    // ---- Pre-defined system roles ----
    std::vector<Role> systemRoles;

    // ---- File paths ----
    std::string usersFilePath;
    std::string auditFilePath;

    // ---- Internal helpers ----
    std::string  GenerateUserId();
    bool         UsernameExists(const std::string& username) const;
    void         WriteAuditLog(const std::string& actor,
                               const std::string& action,
                               const std::string& target,
                               const std::string& detail = "");
    std::string  CurrentTimestamp() const;
    void         InitSystemRoles();
    Role         GetSystemRole(const std::string& roleName) const;

public:
    // ----- Constructor -----
    explicit UserManager(const std::string& dataDir = "./data");

    // ---- Persistence ----
    void LoadFromFile();
    void SaveToFile()  const;

    // ---- CRUD Operations ----
    bool AddUser(const std::string& actorId,
                 const std::string& username,
                 const std::string& plainPassword,
                 const std::string& fullName,
                 const std::string& email,
                 const std::string& phone,
                 const std::string& roleChoice,   // "Admin","Manager","User"
                 const std::string& dept = "");

    bool UpdateUser(const std::string& actorId,
                    const std::string& targetUserId);

    bool DeleteUser(const std::string& actorId,
                    const std::string& targetUserId);

    // ---- View Operations ----
    void ViewUser   (const std::string& actorId,
                     const std::string& targetUserId) const;
    void ViewAllUsers(const std::string& actorId)      const;

    // ---- Authentication ----
    std::shared_ptr<User> Login(const std::string& username,
                                const std::string& plainPassword);

    // ---- RBAC Check ----
    bool CheckPermission(const std::string& userId,
                         const std::string& permName) const;

    // ---- Audit ----
    void ViewAuditLog(const std::string& actorId) const;

    // ---- Role Assignment ----
    bool AssignRole(const std::string& actorId,
                    const std::string& targetId,
                    const std::string& roleName);

    // ---- Helpers ----
    std::shared_ptr<User> FindById(const std::string& userId)   const;
    std::shared_ptr<User> FindByUsername(const std::string& un) const;
    int                   GetUserCount() const;

    // ---- First-run bootstrap (bypasses RBAC – only when DB is empty) ----
    bool BootstrapFirstAdmin(const std::string& username,
                             const std::string& plainPassword,
                             const std::string& fullName,
                             const std::string& email,
                             const std::string& phone,
                             const std::string& dept = "IT");
};
