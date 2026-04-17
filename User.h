/**
 * User.h
 * EMHR System - User Management & RBAC
 * Author  : IShmael Moulton
 * Date    : March 2026
 * Desc    : Abstract-like base class for all system users.
 *           Demonstrates encapsulation (private attrs, public accessors/mutators),
 *           and serves as the base for inheritance (Admin extends User).
 */

#pragma once
#include <string>
#include <vector>
#include "Role.h"

class User {
protected:
    std::string          userId;
    std::string          username;
    std::string          passwordHash;   // SHA-256-like simple hash (demo)
    std::string          fullName;
    std::string          email;
    std::string          phone;
    bool                 isActive;
    std::vector<Role>    roles;
    std::string          createdAt;      // ISO-8601 timestamp
    std::string          lastModified;   // ISO-8601 timestamp

    // ----- Internal helpers -----
    static std::string   HashPassword(const std::string& password);
    static std::string   CurrentTimestamp();

public:
    // ----- Constructors -----
    User();
    User(const std::string& id,
         const std::string& username,
         const std::string& plainPassword,
         const std::string& fullName,
         const std::string& email,
         const std::string& phone);

    virtual ~User() = default;

    // ----- Accessors -----
    std::string          GetUserId()       const;
    std::string          GetUsername()     const;
    std::string          GetFullName()     const;
    std::string          GetEmail()        const;
    std::string          GetPhone()        const;
    bool                 GetIsActive()     const;
    std::vector<Role>    GetRoles()        const;
    std::string          GetCreatedAt()    const;
    std::string          GetLastModified() const;
    std::string          GetPasswordHash() const;

    // ----- Mutators (with validation) -----
    bool SetFullName(const std::string& name);
    bool SetEmail(const std::string& email);
    bool SetPhone(const std::string& phone);
    void SetIsActive(bool active);
    void SetLastModified(const std::string& ts);
    void SetPasswordHash(const std::string& hash);   // for file loading

    // ----- Role / Permission Management -----
    void AddRole(const Role& role);
    void RemoveRole(const std::string& roleName);
    bool HasRole(const std::string& roleName)        const;
    bool HasPermission(const std::string& permName)  const;

    // ----- Authentication -----
    bool   ValidatePassword(const std::string& plainPassword) const;
    bool   ChangePassword(const std::string& oldPlain,
                          const std::string& newPlain);

    // ----- Password Policy (static) -----
    static bool IsValidPassword(const std::string& password);
    static bool IsValidEmail(const std::string& email);

    // ----- Polymorphic interface -----
    virtual std::string  GetUserType()  const;
    virtual void         Display()      const;

    // ----- Serialization -----
    virtual std::string  Serialize()    const;
    static  User         Deserialize(const std::string& line);
};
