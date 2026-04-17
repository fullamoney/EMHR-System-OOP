/**
 * Permission.cpp
 * EMHR System - User Management & RBAC
 */

#include "Permission.h"

Permission::Permission(const std::string& name, const std::string& desc)
    : permissionName(name), description(desc) {}

std::string Permission::GetPermissionName() const { return permissionName; }
std::string Permission::GetDescription()    const { return description; }

bool Permission::operator==(const Permission& other) const {
    return permissionName == other.permissionName;
}

bool Permission::operator<(const Permission& other) const {
    return permissionName < other.permissionName;
}

void Permission::Display() const {
    std::cout << "  [+] " << permissionName;
    if (!description.empty()) std::cout << " (" << description << ")";
    std::cout << "\n";
}
