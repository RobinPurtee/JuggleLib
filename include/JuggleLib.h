#pragma once
#include <tchar.h>
#include <string>

#include <boost\signals2.hpp>
class Prop;
class Hand;
class Throw;


typedef boost::signals2::signal<void()> tickPublisher;
typedef boost::signals2::signal<void(int)> IdPublisher;
typedef boost::signals2::signal<void(Prop*)> PropPublisher;
typedef boost::signals2::signal<void(Hand*)> handPublisher;
typedef boost::signals2::signal<void(Throw*)> throwPublisher;

typedef std::function<void()> ActionSlot;
typedef std::function<void(int)> IdSlot;
typedef std::function<void(Prop*)> PropSlot;
typedef std::function<void(Hand*)> HandSlot;
typedef std::function<void(Throw*)> ThrowSlot;

#include "Prop.h"
#include "Throw.h"
