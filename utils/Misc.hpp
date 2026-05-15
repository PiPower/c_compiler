#pragma once
#include "../LangTypes.hpp"

bool IsPointer(const AccessType* acc);
bool IsArray(const AccessType* acc);
AccessType* MargeAccessTypes(const AccessType* first, const AccessType* second);
void FreeMergedAccType(AccessType* acc);