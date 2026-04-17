/**
 * Role.h
 * EMHR System - User Management & RBAC
 * Author  : Ishamel Moulton
 * Date    : March 2026
 * Desc    : A Role groups a set of Permissions. Users are assigned Roles.
 *           Demonstrates composition (Role HAS-A collection of Permission).
 */

#pragma once
#include <string>
#include <vector>
#include "Permission.h"

class Role {
private:
    std::string          roleName; 
    std::vector<Permission> permissions;

public:
    // ----- Constructors -----
    explicit Role(const std::string& name);

    // ----- Accessors -----
    std::string              GetRoleName()    const;
    std::vector<Permission>  GetPermissions() const;

    // ----- Permission Management -----
    void AddPermission(const Permission& perm);
    void RemovePermission(const std::string& permName);
    bool HasPermission(const std::string& permName) const;

    // ----- Operators -----
    bool operator==(const Role& other) const;

    // ----- Display -----
    void Display() const;
};
