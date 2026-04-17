/**
 * Role.cpp
 * EMHR System - User Management & RBAC
 */

#include "Role.h"
#include <algorithm>
#include <iostream>

Role::Role(const std::string& name) : roleName(name) {}

// ----- Accessors -----
std::string             Role::GetRoleName()    const { return roleName; }
std::vector<Permission> Role::GetPermissions() const { return permissions; }

// ----- Permission Management -----
void Role::AddPermission(const Permission& perm) {
    // avoid duplicates
    for (const auto& p : permissions)
        if (p == perm) return;
    permissions.push_back(perm);
}

void Role::RemovePermission(const std::string& permName) {
    permissions.erase(
        std::remove_if(permissions.begin(), permissions.end(),
            [&](const Permission& p){ return p.GetPermissionName() == permName; }),
        permissions.end());
}

bool Role::HasPermission(const std::string& permName) const {
    for (const auto& p : permissions)
        if (p.GetPermissionName() == permName) return true;
    return false;
}

bool Role::operator==(const Role& other) const {
    return roleName == other.roleName;
}

void Role::Display() const {
    std::cout << "Role: " << roleName << "\n";
    if (permissions.empty()) {
        std::cout << "  (no permissions assigned)\n";
    } else {
        for (const auto& p : permissions) p.Display();
    }
}
