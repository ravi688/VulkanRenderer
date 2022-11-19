#pragma once

#include <common/assert.h>

#define _assert(assertion) assert(DESCRIPTION(assertion), #assertion)
