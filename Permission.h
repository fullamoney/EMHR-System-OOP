/**
 * Permission.h
 * Eastern Medical Health Region (EMHR) System
 * Author  : Ishmael Moulton
 * Date    : March 2026
 * Desc    : Represents a single privilege/action in the RBAC model.
 */

#pragma once
#include <string>
#include <iostream>

class Permission {
private:
    std::string permissionName;   // e.g. "createUser", "deleteUser"
    std::string description;      // human-readable description

public:
    // ----- Constructors -----
    Permission(const std::string& name, const std::string& desc = "");

    // ----- Accessors -----
    std::string GetPermissionName() const;
    std::string GetDescription()    const;

    // ----- Operators -----
    bool operator==(const Permission& other) const;
    bool operator< (const Permission& other) const;   // for sorting / sets

    // ----- Display -----
    void Display() const;
};
