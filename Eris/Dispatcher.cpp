#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Utils.h>
#include <Eris/ClassDispatcher.h>
#include <Eris/TypeDispatcher.h>
#include <Eris/OpDispatcher.h>
#include <Eris/EncapDispatcher.h>
#include <Eris/IdDispatcher.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>

#include <Atlas/Message/Element.h>

#include <stdio.h>
#include <cassert>

using namespace Atlas::Message;

namespace Eris
{

Dispatcher::Dispatcher(const std::string &nm) :
	_name((nm[0] == '_') ? nm + getAnonymousSuffix(this) : nm),
	_refcount(0)
{
}

Dispatcher::~Dispatcher()
{
	assert(_refcount == 0);
}

std::string Dispatcher::getAnonymousSuffix(Dispatcher *d)
{
	static char buf[32];
	snprintf(buf, 32, "_%p", d);
	return buf;
}

void Dispatcher::enter()
{
    assert(global_inDispatch == false);
    global_inDispatch = true;
}

void Dispatcher::exit()
{
    assert(global_inDispatch == true);
    global_inDispatch = false;
    
    while (!global_needsPurging.empty()) {
	global_needsPurging.front()->purge();
	global_needsPurging.pop_front();
    }
}

bool Dispatcher::global_inDispatch = false;
std::list<Dispatcher*> Dispatcher::global_needsPurging;

//////////////////////////////////////////////////////////////////////////////////

StdBranchDispatcher::StdBranchDispatcher(const std::string nm) :
	Dispatcher(nm)
{
	
}

StdBranchDispatcher::~StdBranchDispatcher()
{
	for (DispatcherDict::iterator d = _subs.begin(); d!=_subs.end(); ++d)
		d->second->decRef();
}

Dispatcher* StdBranchDispatcher::addSubdispatch(Dispatcher *d, const std::string /*data*/)
{
	if (!d)
		throw InvalidOperation("NULL dispatcher passed to addSubdispatch");
	std::string nm = d->getName();
	DispatcherDict::iterator di = _subs.find(nm);
	if (di != _subs.end() && (di->second != NULL))
		throw InvalidOperation("Duplicate dispatcher <" + nm + "> added");
	
	d->addRef();
	
	if (di != _subs.end()) {
	    /* entry has been removed inside a dispatch, so the map entry still exists. Instead of
	    inserting, we just update the current one and we're done. */
	    di->second = d;
	} else
	    _subs.insert(di, DispatcherDict::value_type(nm, d));
	return d;
}

void StdBranchDispatcher::rmvSubdispatch(Dispatcher *d)
{
	if (!d)
		throw InvalidOperation("NULL dispatcher passed to rmvSubdispatch");
	std::string nm = d->getName();
	DispatcherDict::iterator di = _subs.find(nm);
	
	if (di != _subs.end()) {
	    if (di->second == NULL) // might happen with delayed deleteion
		throw InvalidOperation("duplicate remove of dispatcher " + di->first);
	    
	    safeSubErase(di);
	    d->decRef();
	    return;
	}
	
	// check for anonymous
	for (di=_subs.begin(); di!=_subs.end(); ++di)
	    if (di->second && (di->second->_name[0] == '_')) {
		Dispatcher *ds = di->second->getSubdispatch(nm);
		if (ds) {
		    di->second->rmvSubdispatch(d);
		    // clean up empty anonymous nodes automatically
		    if (di->second->empty()) {
			di->second->decRef();
			safeSubErase(di);
		    }
		    return;
	    }
	}
}

Dispatcher* StdBranchDispatcher::getSubdispatch(const std::string &nm)
{
    DispatcherDict::iterator D = _subs.find(nm);
    if (D != _subs.end())
		return D->second;
	
    // deal with annonymous ones
    for (D=_subs.begin(); D!=_subs.end(); ++D)
	if (D->second && (D->second->_name[0] == '_')) {
	    Dispatcher *ds = D->second->getSubdispatch(nm);
	    if (ds)
		return ds;
	}
	
    return NULL;
}

bool StdBranchDispatcher::subdispatch(DispatchContextDeque &dq)
{
    addRef();
    for (DispatcherDict::const_iterator d = _subs.begin(); d!=_subs.end(); ++d)
	if (d->second) {
           if (d->second->dispatch(dq)) {
               decRef();
               return true;
           }
        }
    decRef();
	
    return false;		
}

void StdBranchDispatcher::safeSubErase(const DispatcherDict::iterator &D)
{
    if (Dispatcher::global_inDispatch) {
	global_needsPurging.push_back(this); // make sure it gets cleaned up ASAP
	D->second = NULL;	// remove the entry
    } else
	_subs.erase(D);
}

void StdBranchDispatcher::purge()
{
    for (DispatcherDict::iterator d = _subs.begin(); d!=_subs.end();) {
	DispatcherDict::iterator cur(d++);
	if (!cur->second)
	    _subs.erase(cur);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

LeafDispatcher::LeafDispatcher(const std::string &nm) :
	Dispatcher(nm)
{
	;
}

bool LeafDispatcher::dispatch(DispatchContextDeque &dq)
{
	Element::MapType &o = dq.back().asMap();
	o["__DISPATCHED__"] = "1";
	return false;
}

Dispatcher* LeafDispatcher::addSubdispatch(Dispatcher*, const std::string)
{
    throw InvalidOperation("called addSubdispatch on LeafDispatcher " + _name);
}
	
void LeafDispatcher::rmvSubdispatch(Dispatcher*)
{
    throw InvalidOperation("called rmvSubdispatch on LeafDispatcher " + _name);
}


Dispatcher* LeafDispatcher::getSubdispatch(const std::string &nm)
{
	Eris::log(LOG_ERROR, "looking for child %s in LeafDispatcher %s", 
		nm.c_str(), _name.c_str());
	return NULL;
}

bool LeafDispatcher::empty()
{
	throw InvalidOperation("called empty() on LeafDispatcher " + _name);
}


void LeafDispatcher::purge()
{
    throw InvalidOperation("called purge() on LeafDispatcher " + _name);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IdDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (getMember(dq.front(), "id").asString() != _id)
		return false;
	return StdBranchDispatcher::subdispatch(dq);
}

bool TypeDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (!hasMember(dq.front(), "objtype"))
		return false;
	
	if (getMember(dq.front(), "objtype").asString() != _type)
		return false;
	return StdBranchDispatcher::subdispatch(dq);
}

bool ObjectDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (!hasMember(dq.front(), "objtype"))
		return false;
	
	const std::string & otype = getMember(dq.front(), "objtype").asString();
	if ((otype != "object") && (otype != "obj"))
		return false;
	return StdBranchDispatcher::subdispatch(dq);
}

bool OpFromDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (getMember(dq.front(), "from").asString() != _id)
		return false;
	return StdBranchDispatcher::subdispatch(dq);	
}

bool OpToDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (!hasMember(dq.front(), "to")) return false;
	if (getMember(dq.front(), "to").asString() != _id)
		return false;
	return StdBranchDispatcher::subdispatch(dq);	
}

bool OpRefnoDispatcher::dispatch(DispatchContextDeque &dq)
{
	DispatchContextDeque::const_iterator item = dq.begin();
	for(unsigned i = 0; i < _depth; ++i)
		if(++item == dq.end())
			return false;

	assert((*item).isMap());
	const Atlas::Message::Element::MapType &map = (*item).asMap();
	Atlas::Message::Element::MapType::const_iterator I = map.find("refno");
	if(I == map.end()) {
		std::string warning = "Op without a refno, keys are:";
		for(I = map.begin(); I != map.end(); ++I)
			warning += " " + I->first;
		log(LOG_DEBUG, warning.c_str());
		return false;
	}
	assert(I->second.isInt());
	if (I->second.asInt() != _refno)
		return false;
	return StdBranchDispatcher::subdispatch(dq);	
}

bool EncapDispatcher::dispatch(DispatchContextDeque &dq)
{
	const Atlas::Message::Element::ListType &args = 
		getMember(dq.front(), "args").asList();
	if (args.size() < _position)
		return false;
	
	dq.push_front(args[_position]);
	bool ret = StdBranchDispatcher::subdispatch(dq);	
	
	// we need to restore the context stack, otherwise the next dispatcher along gets very confused
	dq.pop_front();
	return ret;
}

Dispatcher* EncapDispatcher::newAnonymous()
{
	return new EncapDispatcher("_encap");
}

}
