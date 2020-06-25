/*
  ==============================================================================

    CombFilters.h
    Created: 22 Jun 2020 8:20:15pm
    Author:  Allen Lee

  ==============================================================================
*/

#pragma once

#include <vector>
#include "DelayLine.h"
#include "OnePoleFilter.h"

template <typename T>
class FFCF
{
public:
    FFCF();
    FFCF(size_t N, T b0, T bm);
    T tick(T x);
    
    void setDelayLength(size_t N);
    
    T b0;
    T bm;
    
protected:
    DelayLine<T> _M;
};


template <typename T>
class FBCF
{
public:
    FBCF();
    FBCF(size_t N, T b0, T am);
    T tick(T x);
    
    void setDelayLength(size_t N);
    
    T b0;
    T am;
    
protected:
    DelayLine<T> _M;
};


template <typename T>
class APCF
{
public:
    APCF();
    APCF(size_t N, T g);
    T tick(T x);
    
    void setGain(T gain);
    void setDelayLength(size_t N);
    
protected:
    FFCF<T> _ff;
    FBCF<T> _fb;
};



//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//



template <typename T>
FFCF<T>::FFCF()
{
    b0 = 1.0;
    bm = 1.0;
}


template <typename T>
FFCF<T>::FFCF(size_t N, T b0, T bm) : _M(N)
{
    this->b0 = b0;
    this->bm = bm;
}


template <typename T>
T FFCF<T>::tick(T x)
{
    return (x * b0) + (bm * _M.tick(x));
}


template <typename T>
void FFCF<T>::setDelayLength(size_t N)
{
    _M.setDelayLength(N);
}

//--------------------------------------------------------------------------------//


template <typename T>
FBCF<T>::FBCF()
{
    b0 = 1.0;
    am = -1.0;
}


template <typename T>
FBCF<T>::FBCF(size_t N, T b0, T am) : _M(N)
{
    this->b0 = b0;
    this->am = am;
}


template <typename T>
T FBCF<T>::tick(T x)
{
    T v = x + (am * _M.peek());
    _M.tick(v);
    
    return b0 * v;
}


template <typename T>
void FBCF<T>::setDelayLength(size_t N)
{
    _M.setDelayLength(N);
}

//--------------------------------------------------------------------------------//


template <typename T>
APCF<T>::APCF()
{
    _ff.b0 = -0.5;
    _ff.bm = 1.0;
    _fb.am = 0.5;
    _fb.b0 = 1.0;
}


template <typename T>
APCF<T>::APCF(size_t N, T g)
{
    _ff.setDelayLength(N);
    _ff.b0 = -g;
    _ff.bm = 1.0;
    _fb.am = g;
    _fb.b0 = 1.0;
}


template <typename T>
T APCF<T>::tick(T x)
{
    return _fb.tick(_ff.tick(x));
}


template <typename T>
void APCF<T>::setDelayLength(size_t N)
{
    _ff.setDelayLength(N);
    _fb.setDelayLength(N);
}


template <typename T>
void APCF<T>::setGain(T gain)
{
    _ff.b0 = -gain;
    _fb.am = gain;
}
