/*
  ==============================================================================

    DelayLine.h
    Created: 22 Jun 2020 8:20:02pm
    Author:  Allen Lee

  ==============================================================================
*/

#pragma once

#include <vector>


template <typename T>
class DelayLine
{
public:
    DelayLine();
    DelayLine(size_t N);
    T tick(T x) noexcept;
    T peek() noexcept;
    
    void setDelayLength(size_t N);

protected:
    std::vector<T> _buffer;
    size_t _currentPtr;
    size_t _N;
    
};



template<typename T>
DelayLine<T>::DelayLine()
{
    _currentPtr = 0;
    _N = 1;
}


template <typename T>
DelayLine<T>::DelayLine(size_t N)
{
    if (N > 0)
        _buffer = std::vector<T>(N);
    
    _N = N;
    _currentPtr = 0;
}


template <typename T>
T DelayLine<T>::tick(T x) noexcept
{
    if (_N == 0)
        return x;
    
    T y = _buffer[_currentPtr];
    _buffer[_currentPtr] = x;

    _currentPtr++;
    if (_currentPtr >= _N)
        _currentPtr = 0;

    return y;
}


template <typename T>
T DelayLine<T>::peek() noexcept
{
    if (_N > 0)
        return _buffer[_currentPtr];
    
    return 0;
}


template <typename T>
void DelayLine<T>::setDelayLength(size_t N)
{
    _N = N;
    _buffer.reserve(N);
    
    for (auto i = 0; i < N; ++i)
        _buffer.push_back(0);
}

