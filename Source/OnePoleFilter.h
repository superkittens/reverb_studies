/*
  ==============================================================================

    OnePoleFilter.h
    Created: 22 Jun 2020 8:20:52pm
    Author:  Allen Lee

  ==============================================================================
*/

#pragma once

#include "DelayLine.h"
#include "CombFilters.h"

template <typename T>
class OnePoleFilter
{
public:
    OnePoleFilter();
    OnePoleFilter(T b0, T a1);
    T tick(T x);
    
    T b0;
    T a1;
    
protected:
    DelayLine<T> _M;
};



template <typename T>
OnePoleFilter<T>::OnePoleFilter()
{
    b0 = 1.0;
    a1 = -0.5;
}


template <typename T>
OnePoleFilter<T>::OnePoleFilter(T b0, T a1)
{
    b0 = b0;
    a1 = a1;
}


template <typename T>
T OnePoleFilter<T>::tick(T x)
{
    T y = (x * b0) + (a1 * _M.peek());
    _M.tick(y);
    
    return y;
}
