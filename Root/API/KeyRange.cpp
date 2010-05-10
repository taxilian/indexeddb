/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "KeyRange.h"
#include "DatabaseException.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

KeyRange::KeyRange(void)
	: flags(-1)
	{ initializeMethods(); }

KeyRange::KeyRange(FB::variant left, FB::variant right, int flags)
	: flags(flags), left(left), right(right)
	{
	if(left.get_type() != right.get_type() && !left.empty() && !right.empty())
		throw new DatabaseException("NOT_ALLOWED_ERR", DatabaseException::NOT_ALLOWED_ERR);

	initializeMethods();
	}

void KeyRange::initializeMethods()
	{	
	registerProperty("left", make_property(this, &KeyRange::getLeft));
	registerProperty("right", make_property(this, &KeyRange::getRight));
	registerProperty("flags", make_property(this, &KeyRange::getFlags));

	registerProperty("SINGLE", make_property(this, &KeyRange::getSingle));
	registerProperty("LEFT_OPEN", make_property(this, &KeyRange::GetLeftOpen));
	registerProperty("RIGHT_OPEN", make_property(this, &KeyRange::GetRightOpen));
	registerProperty("LEFT_BOUND", make_property(this, &KeyRange::GetLeftBound));
	registerProperty("RIGHT_BOUND", make_property(this, &KeyRange::GetRightBound));

	registerMethod("only", make_method(this, &KeyRange::only));
	registerMethod("leftBound", make_method(this, static_cast<FB::JSOutObject (KeyRange::*)(FB::variant, const FB::CatchAll &)>(&KeyRange::leftBound))); 
	registerMethod("rightBound", make_method(this, static_cast<FB::JSOutObject (KeyRange::*)(FB::variant, const FB::CatchAll &)>(&KeyRange::rightBound)));
	registerMethod("bound", make_method(this, static_cast<FB::JSOutObject (KeyRange::*)(FB::variant, FB::variant, const FB::CatchAll &)>(&KeyRange::bound)));
	}

FB::JSOutObject KeyRange::leftBound(FB::variant bound, const FB::CatchAll& args)
	{ 
	const FB::VariantList& values = args.value;

	if(values.size() > 1)
		throw FB::invalid_arguments();
	else if(values.size() == 1 && !values[0].is_of_type<bool>())
		throw FB::invalid_arguments();
	
	bool open = values.size() == 1 ? values[0].cast<bool>() : false;

	return static_cast<FB::JSOutObject>(leftBound(bound, open));
	}

FB::JSOutObject KeyRange::rightBound(FB::variant bound, const FB::CatchAll& args)
	{ 
	const FB::VariantList& values = args.value;

	if(values.size() > 1)
		throw FB::invalid_arguments();
	else if(values.size() == 1 && !values[0].is_of_type<bool>())
		throw FB::invalid_arguments();
	
	bool open = values.size() == 1 ? values[0].cast<bool>() : false;

	return static_cast<FB::JSOutObject>(rightBound(bound, open));
	}

FB::JSOutObject KeyRange::bound(FB::variant left, FB::variant right, const FB::CatchAll& args)
	{ 
	const FB::VariantList& values = args.value;

	if(values.size() > 2)
		throw FB::invalid_arguments();
	else if(values.size() >= 1 && !values[0].is_of_type<bool>())
		throw FB::invalid_arguments();
	else if(values.size() == 2 && !values[1].is_of_type<bool>())
		throw FB::invalid_arguments();
	
	bool openLeft = values.size() >= 1 ? values[0].cast<bool>() : false;
	bool openRight = values.size() == 2 ? values[1].cast<bool>() : false;

	return static_cast<FB::JSOutObject>(bound(left, right, openLeft, openRight));
	}

FB::AutoPtr<KeyRange> KeyRange::only(FB::variant value)
	{ return new KeyRange(value, value, KeyRange::SINGLE); }

FB::AutoPtr<KeyRange> KeyRange::leftBound(FB::variant bound, const bool open)
	{ return this->bound(bound, FB::variant(), open, false); }

FB::AutoPtr<KeyRange> KeyRange::rightBound(FB::variant bound, const bool open)
	{ return this->bound(FB::variant(), bound, false, open); }
	
FB::AutoPtr<KeyRange> KeyRange::bound(FB::variant left, FB::variant right, const bool openLeft, const bool openRight)
	{ return new KeyRange(left, right, 
		(!left.empty() ? KeyRange::LEFT_BOUND : 0) | 
		(!right.empty() ? KeyRange::RIGHT_BOUND : 0) |
		(openLeft ? KeyRange::LEFT_OPEN : 0) |
		(openRight ? KeyRange::RIGHT_OPEN : 0)); }

}
}
}