#pragma once
#include <tchar.h>
#include <string>

#include <boost\signals2.hpp>
class Prop;

typedef boost::signals2::signal<void(int)> IdPublisher;
typedef boost::signals2::signal<void(Prop*)> PropPublisher;
typedef boost::signals2::signal<void()> tickPublisher;

typedef std::function<void(int)> IdSlot;
typedef std::function<void(Prop*)> PropSlot;
typedef std::function<void()> ActionSlot;

#include "Prop.h"
#include "Throw.h"
