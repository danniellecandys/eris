#include <stdexcept>

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>

namespace Atlas {

template <class T>
T atlas_cast(const Message::Object &data)
{
	T obj = T::Instantiate();
	if (!data.IsMap()) {
		assert(false);
		throw std::invalid_argument("Input message object is not a map");
	}
	
	Message::Object::MapType mp = data.AsMap();
	
	for (Message::Object::MapType::iterator A = mp.begin();
		A != mp.end(); ++A)
	{
		obj.SetAttr(A->first, A->second);
	}
	
	return obj;
}

template <class T>
T atlas_cast(const Objects::Root &data)
{
	T obj = T::Instantiate();
	Message::Object::MapType mp = data.AsMap();
	
	for (Message::Object::MapType::iterator A = mp.begin();
		A != mp.end(); ++A)
	{
		obj.SetAttr(A->first, A->second);
	}
	
	return obj;
}

}