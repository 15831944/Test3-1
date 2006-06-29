#pragma once
#ifndef OBJECT__PTR__H__
#define OBJECT__PTR__H__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include "TS.h"

namespace RioTintoTS
{

template<class T> class TS_API ObjectPtr : public boost::shared_ptr<T>
{
public:

	// Construct empty
	ObjectPtr( ) { }

	// Construct with (new) T*
	explicit ObjectPtr( T* p ) : boost::shared_ptr<T>( p ) { }

	// Constuct with (new) other type
	template<class Y>
	explicit ObjectPtr( Y* p ) : boost::shared_ptr<T>( dynamic_cast<T*>(p) ) { }

	// Copy construct (own type)
	ObjectPtr( boost::shared_ptr<T> const& r ) : boost::shared_ptr<T>( r ) { }

	// Copy construct (other types)
	template<class Y>
	ObjectPtr( boost::shared_ptr<Y> const& r )
	: boost::shared_ptr<T>( r, boost::detail::dynamic_cast_tag() ) { }

	// Assignment (own type)
	ObjectPtr<T>& operator=( boost::shared_ptr<T> const& r )
	{
		boost::shared_ptr<T>::operator=( r );
		return *this;
	}

	// Assignment (other type)
	template<class Y>
	ObjectPtr<T>& operator=( boost::shared_ptr<Y> const& r )
	{
		boost::shared_ptr<T>::operator=( boost::dynamic_pointer_cast<T,Y>(r) );
		return *this;
	}
};

}  // end of namespace ECUUtility

#endif