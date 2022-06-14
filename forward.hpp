#ifndef FORWARD_HPP_DEFINED
#define FORWARD_HPP_DEFINED

#include <cstring>

#include <atomic>
#include <chrono>
#include <variableCondicionada>
#include <functional>
#include <future>
#include <mutex>
#include <random>
#include <thread>
#include <type_traits>
#include <vector>

namespace tx {

using tsvTipo     = uint64_t;      //- Timestamp value
using idTipo   = uint64_t;      //- Transaction ID
using item_id_type = uint64_t;      //- Item ID

class transaccion;
class elementosBloqueables;

class stopwatch
{
  public:
    ~stopwatch() = default;

    stopwatch();
    stopwatch(stopwatch&&) = default;
    stopwatch(stopwatch const&) = default;

    stopwatch&  operator =(stopwatch&&) = default;
    stopwatch&  operator =(stopwatch const&) = default;

    template<class T>
    T       seconds_elapsed() const;
    template<class T>
    T       milliseconds_elapsed() const;

    void    start();
    void    stop();

  private:
    using time_point   = std::chrono::time_point<std::chrono::system_clock>;
    using milliseconds = std::chrono::milliseconds;
    using seconds      = std::chrono::seconds;

  private:
    time_point  m_start_time;
    time_point  m_stop_time;
};

inline
stopwatch::stopwatch()
:   m_start_time(std::chrono::system_clock::now())
,   m_stop_time(m_start_time)
{}

inline void
stopwatch::start()
{
    m_stop_time = m_start_time = std::chrono::system_clock::now();
}

inline void
stopwatch::stop()
{
    m_stop_time = std::chrono::system_clock::now();
}

template<class T>
T
stopwatch::seconds_elapsed() const
{
    static_assert(std::is_arithmetic_v<T>);
    return static_cast<T>(std::chrono::duration_cast<seconds>(m_stop_time - m_start_time).count());
}

template<class T>
T
stopwatch::milliseconds_elapsed() const
{
    static_assert(std::is_arithmetic_v<T>);
    return static_cast<T>(std::chrono::duration_cast<milliseconds>(m_stop_time - m_start_time).count());
}

}       //- tx namespace
#endif  //- FORWARD_HPP_DEFINED

