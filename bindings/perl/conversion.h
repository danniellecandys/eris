#ifndef ERIS_PERL_CONVERSION_H
#define ERIS_PERL_CONVERSION_H

#include "Lobby.h"
namespace Eris {class Person; class Entity;}

#include <sigcperl/convert.h>

#include <Atlas/Message/Object.h>

extern "C" {
#include "EXTERN.h"
#include "perl.h"
}
#undef convert
#undef list

#define PACK_HV(typename, inst, hv) \
{ \
  const char typestring[] = #typename " *"; \
  SV **ret_val = hv_store(hv, typestring, sizeof(typestring) - 1, \
    newSViv((I32) inst), 0); \
  assert(ret_val); \
}

inline HV* PackInheritanceHV(Eris::Room *room, HV *hv)
{
  PACK_HV(Room, room, hv);
  return hv;
}

inline HV* PackInheritanceHV(Eris::Lobby *lobby, HV *hv)
{
  PACK_HV(Lobby, lobby, hv);
  PackInheritanceHV(static_cast<Eris::Room*>(lobby), hv);
  return hv;
}

SV* AtlasToSV(const Atlas::Message::Object &obj)
{
  switch(obj.GetType()) {
    case Atlas::Message::Object::TYPE_NONE:
      return NEWSV(0, 0);
    case Atlas::Message::Object::TYPE_INT:
      return newSViv(obj.AsInt());
    case Atlas::Message::Object::TYPE_FLOAT:
      return newSVnv(obj.AsFloat());
    case Atlas::Message::Object::TYPE_STRING:
      {
        const std::string &str = obj.AsString();
        return newSVpv(str.c_str(), str.size());
      }
    case Atlas::Message::Object::TYPE_MAP:
      {
        HV *hv = newHV();
        const Atlas::Message::Object::MapType &map = obj.AsMap();
        Atlas::Message::Object::MapType::const_iterator I;
        for(I = map.begin(); I != map.end(); ++I)
             hv_store(hv, I->first.c_str(), I->first.size(), AtlasToSV(I->second), 0);
        return newRV_noinc((SV*) hv);
      }
    case Atlas::Message::Object::TYPE_LIST:
      {
        AV *av = newAV();
        const Atlas::Message::Object::ListType &list = obj.AsList();
        av_extend(av, list.size() - 1);
        for(I32 i; i < list.size(); ++i)
          av_store(av, i, AtlasToSV(list[i]));
        return newRV_noinc((SV*) av);
      }
    default:
      assert(false);
  }
}

namespace SigCPerl {
  inline SV* GetSV(const Atlas::Message::Object &obj) {return AtlasToSV(obj);}
}

Atlas::Message::Object SVToAtlas(SV *sv)
{
  svtype type = (svtype) SvTYPE(sv);
  if(type == SVt_RV)
    type = (svtype) SvTYPE((SV*) SvRV(sv));

  switch(type) {
    case SVt_IV:
      return SvIV(sv);
    case SVt_NV:
      return SvNV(sv);
    case SVt_PV:
      {
        STRLEN len;
        const char *str = SvPV(sv, len);
        return std::string(str, len);
      }
    case SVt_PVAV:
      {
        AV* av = (AV*) SvRV(sv);
        I32 len = av_len(av);
        if(len == -1)
          return Atlas::Message::Object::ListType();
        assert(len >= 0);
        Atlas::Message::Object::ListType list(len + 1);
        for(I32 i = 0; i <= len; ++i) {
          SV **val = av_fetch(av, i, 0);
          assert(val);
          list[i] = SVToAtlas(*val);
        }
        return list;
      }
    case SVt_PVHV:
      {
        HV* hv = (HV*) SvRV(sv);
        Atlas::Message::Object::MapType map;
        I32 havekeys = hv_iterinit(hv);
        while(havekeys) {
          char *key;
          SV *val = hv_iternextsv(hv, &key, &havekeys);
          map[key] = SVToAtlas(val);
        }
        return map;
      }
    case SVt_NULL: // undef -> TYPE_NONE (?)
      return Atlas::Message::Object();
    default:
      throw Atlas::Message::WrongTypeException();
  }
}

using Eris::Room;
using Eris::Person;
using Eris::Entity;
// The following autogenerated header file is not complete, and should
// only be included here
#include "conversion_wrapper.h"

#endif // ERIS_PERL_CONVERSION_H