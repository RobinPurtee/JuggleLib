#pragma once
#include <tchar.h>
#include <string>

#include <boost\signals2.hpp>
class Prop;

typedef boost::signals2::signal<void(int)> IdPublisher;
typedef boost::signals2::signal<void(Prop*)> PropPublisher;

typedef std::function<void(int)> IdSlot;
typedef std::function<void(Prop*)> PropSlot;

#include "Prop.h"
#include "Throw.h"
