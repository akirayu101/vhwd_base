
#ifndef __H_VHWD_COLLECTION_KV_TRAIT__
#define __H_VHWD_COLLECTION_KV_TRAIT__

#include "vhwd/config.h"

VHWD_ENTER

template<typename K,typename V>
class kv_pair
{
public:
	K first;
	V second;
	kv_pair(){}
	kv_pair(const K& k,const V& v=V()):first(k),second(v){}
};

template<typename K,typename V=void>
class kv_trait
{
public:

	typedef K key_type;
	typedef V mapped_type;
	typedef kv_pair<key_type,mapped_type> value_type;
	typedef kv_pair<const key_type,mapped_type>& value_reference;

	static const key_type& key(const key_type& v){return v;}
	static const key_type& key(const value_type& v){return v.first;}
	static const mapped_type& value(const value_type& v){return v.second;}
	static value_type pair(const key_type& k,const mapped_type& v=mapped_type()){return value_type(k,v);}


};


template<typename K>
class kv_trait<K,void>
{
public:

	typedef K key_type;
	typedef K mapped_type;
	typedef K value_type;
	typedef const value_type& value_reference;

	static const key_type& key(const value_type& v){return v;}
	static const mapped_type& value(const value_type& v){return v;}
	static const value_type& pair(const key_type& k){return k;}

};


VHWD_LEAVE

#endif
