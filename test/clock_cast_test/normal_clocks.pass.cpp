// The MIT License (MIT)
//
// Copyright (c) 2017 Tomasz Kamiński
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "tz.h"
#include <cassert>

template <class Duration>
void time_conversion(std::vector<date::sys_days> const &inputs)
{
  using namespace date;
  for (const date::sys_days& st : inputs)
  {
    sys_time<Duration> s = st;
    utc_time<Duration> ufroms = to_utc_time(s);
    sys_time<Duration> sfromu = to_sys_time(ufroms);
    tai_time<Duration> tfroms = to_tai_time(s);
    sys_time<Duration> sfromt = to_sys_time(tfroms);
    gps_time<Duration> gfroms = to_gps_time(s);
    sys_time<Duration> sfromg = to_sys_time(gfroms);
    tai_time<Duration> tfromu = to_tai_time(ufroms);
    utc_time<Duration> ufromt = to_utc_time(tfromu);
    gps_time<Duration> gfromu = to_gps_time(ufroms);
    utc_time<Duration> ufromg = to_utc_time(gfromu);
    gps_time<Duration> gfromt = to_gps_time(tfroms);
    tai_time<Duration> tfromg = to_tai_time(gfromt);

    assert(s == sfromu);
    assert(s == sfromt);
    assert(s == sfromg);
    assert(ufroms == ufromt);
    assert(ufroms == ufromg);
    assert(tfroms == tfromu);
    assert(tfroms == tfromg);
    assert(gfroms == gfromu);
    assert(gfroms == gfromt);
  }
}

int
main()
{
    using namespace date;
    using sys_clock = std::chrono::system_clock;

    /*
     * a lot of vectors between 1958 and 1972 to
     * check behaviour during the period when
     * leap time was based on formulae, rather
     * than discrete leap seconds
     */
    std::vector<sys_days> inputs = {
      sys_days{ 1957_y / January / 1 },
      sys_days{ 1958_y / January / 1 },  
      sys_days{ 1961_y / January / 1 },  
      sys_days{ 1961_y / August / 1 },  
      sys_days{ 1962_y / January / 1 },  
      sys_days{ 1963_y / November / 1 },
      sys_days{ 1964_y / January / 1 },
      sys_days{ 1964_y / April / 1 },
      sys_days{ 1964_y / September / 1 },
      sys_days{ 1965_y / January / 1 },
      sys_days{ 1965_y / March / 1 },
      sys_days{ 1965_y / July / 1 },
      sys_days{ 1965_y / September / 1 },
      sys_days{ 1966_y / January / 2 },
      sys_days{ 1968_y / February / 1 },
      sys_days{ 1970_y / January / 1 },
      sys_days{ 1971_y / December / 31 },
      sys_days{ 1972_y / January / 1 },
      sys_days{ 1980_y / January / 6 },
      sys_days{ 1980_y / November / 3 },
      sys_days{ 1997_y / December / 12},
      sys_days{ 2017_y / January / 17 },
    };

    for (const sys_days& st : inputs)
    {
      // self
      {
        auto ut = utc_clock::from_sys(st);
        auto tt = tai_clock::from_utc(ut);

        assert(clock_cast<sys_clock>(st) == st);
        assert(clock_cast<utc_clock>(ut) == ut);
        assert(clock_cast<tai_clock>(tt) == tt);
      }

      // sys <-> utc
      {
        auto ut = utc_clock::from_sys(st);

        assert(clock_cast<utc_clock>(st) == ut);
        assert(clock_cast<sys_clock>(ut) == st);
      }

      // tai <-> utc
      {
        auto ut = utc_clock::from_sys(st);
        auto tt = tai_clock::from_utc(ut);

        assert(clock_cast<tai_clock>(ut) == tt);
        assert(clock_cast<utc_clock>(tt) == ut);
      }

      // tai <-> sys
      {
        auto ut = utc_clock::from_sys(st);
        auto tt = tai_clock::from_utc(ut);

        assert(clock_cast<tai_clock>(st) == tt);
        assert(clock_cast<sys_clock>(tt) == st);
      }

      // gps <-> utc
      {
        auto ut = utc_clock::from_sys(st);
        auto gt = gps_clock::from_utc(ut);

        assert(clock_cast<gps_clock>(ut) == gt);
        assert(clock_cast<utc_clock>(gt) == ut);
      }

      // gps <-> sys
      {
        auto ut = utc_clock::from_sys(st);
        auto gt = gps_clock::from_utc(ut);

        assert(clock_cast<gps_clock>(st) == gt);
        assert(clock_cast<sys_clock>(gt) == st);
      }

      // tai <-> gps
      {
        auto ut = utc_clock::from_sys(st);
        auto tt = tai_clock::from_utc(ut);
        auto gt = gps_clock::from_utc(ut);

        assert(clock_cast<gps_clock>(tt) == gt);
        assert(clock_cast<tai_clock>(gt) == tt);
      }
    }

    time_conversion<std::chrono::nanoseconds>(inputs);
    time_conversion<std::chrono::microseconds>(inputs);
    time_conversion<std::chrono::milliseconds>(inputs);
    time_conversion<std::chrono::seconds>(inputs);
}
