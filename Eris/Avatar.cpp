#include "Avatar.h"

#include <sigc++/object_slot.h>
#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Info.h>

#include "World.h"
#include "Entity.h"
#include "OpDispatcher.h"
#include "Connection.h"
#include "ClassDispatcher.h"
#include "Log.h"

using namespace Eris;

Avatar::AvatarMap Avatar::_avatars;

// Not very efficient, but it works.
static std::string refno_to_string(long refno)
{
  // deal with any character set weirdness
  const char digits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

  unsigned long val = (refno >= 0) ? val : -val;

  std::string result;

  do {
    result = digits[val % 10] + result;
    val /= 10;
  } while(val);

  if(refno < 0)
    result = '-' + result;

  return result;
}

// We may not be able to set _entity yet, if we haven't gotten a
// Sight op from the server
Avatar::Avatar(World* world, long refno, const std::string& character_id)
	: _world(world), _id(character_id), _entity(0)
{
    assert(world);

    _dispatch_id = "character_" + refno_to_string(refno);

// info operation
    Dispatcher *d = _world->getConnection()->getDispatcherByPath("op:info");
    assert(d);
    d = d->addSubdispatch(ClassDispatcher::newAnonymous(_world->getConnection()));
    d = d->addSubdispatch(new OpRefnoDispatcher(_dispatch_id, refno), "game_entity");
    d->addSubdispatch( new SignalDispatcher2<Atlas::Objects::Operation::Info, 
    	Atlas::Objects::Entity::GameEntity>(
    	"character", SigC::slot(*this, &Avatar::recvInfoCharacter))
    );

    if(!_id.empty()) {
	bool success = _avatars.insert(std::make_pair(
	    AvatarIndex(_world->getConnection(), _id), this)).second;
	if(!success) // We took a character that already had an Avatar
	    throw InvalidOperation("Character " + _id + " already has an Avatar");
    }

    log(LOG_DEBUG, "Created new Avatar with id %s and refno %i", _id.c_str(), refno);
}

Avatar::~Avatar()
{
    if(!_dispatch_id.empty())
	_world->getConnection()->removeDispatcherByPath("op:info", _dispatch_id);
    if(!_id.empty()) {
	AvatarMap::iterator I = _avatars.find(AvatarIndex(_world->getConnection(), _id));
	assert(I != _avatars.end());
	_avatars.erase(I);
    }
    // If at some point we have multiple Avatars per World, this will need to
    // be changed to some unregister function
    delete _world;
}

Avatar* Avatar::find(Connection* con, const std::string& id)
{
    AvatarMap::const_iterator I = _avatars.find(AvatarIndex(con, id));
    return (I != _avatars.end()) ? I->second : 0;
}

std::vector<Avatar*> Avatar::getAvatars(Connection* con)
{
    std::vector<Avatar*> result;

    for(AvatarMap::const_iterator I = _avatars.begin(); I != _avatars.end(); ++I)
	if(I->first.first == con)
	    result.push_back(I->second);

    return result;
}

void Avatar::recvInfoCharacter(const Atlas::Objects::Operation::Info &ifo,
		const Atlas::Objects::Entity::GameEntity &character)
{
    log(LOG_DEBUG, "Have id %s, got id %s", _id.c_str(), character.GetId().c_str());

    assert(_id.empty() || _id == character.GetId());
    if(_id.empty()) {
	_id = character.GetId();
	bool success = _avatars.insert(std::make_pair(
	    AvatarIndex(_world->getConnection(), _id), this)).second;
	assert(success); // Newly created character should have unique id
    }

    log(LOG_DEBUG, "Got character info with id %s", _id.c_str());

    _world->recvInfoCharacter(ifo, character);

    _world->getConnection()->removeDispatcherByPath("op:info", _dispatch_id);
    _dispatch_id = "";
}