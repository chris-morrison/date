// The MIT License (MIT)
//
// Copyright (c) 2018 Tomasz Kamiński
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

#include <chrono>
#include "date/tz.h"

int
main()
{
   using namespace date;
   using namespace std::chrono;

   // self
   {
     auto ls = local_days{1970_y/January/1_d};
     assert(clock_cast<local_t>(ls) == ls);
   }

   /// sys epoch
   {
     auto ls = local_days{1970_y/January/1_d};
     auto st = clock_cast<system_clock>(ls);
     assert(clock_cast<local_t>(st) == ls);
     assert(st.time_since_epoch() == seconds(0));
   }

   /// sys 2000 case
   {
     auto ls = local_days{2000_y/January/1_d};
     auto st = clock_cast<system_clock>(ls);
     assert(clock_cast<local_t>(st) == ls);
     assert(st.time_since_epoch() == seconds(946684800));
   }

   /// utc epoch
   {
     auto lu = local_days{1970_y/January/1_d};
     auto ut = clock_cast<utc_clock>(lu);

     assert(clock_cast<local_t>(ut) == lu);
     assert(ut.time_since_epoch() == seconds(0));

     auto lt = local_days{ 1970_y / January / 1_d } - 8s;
     auto tt = clock_cast<tai_clock>(lt);
     assert(clock_cast<local_t>(tt) == lt);
   }

   // utc leap second
   {
     auto lu = local_days{2015_y/July/1_d} - milliseconds(1);
     auto ut = clock_cast<utc_clock>(lu) + milliseconds(500); //into leap second
     assert(clock_cast<local_t>(ut) == lu);


     /*
      * Expected behaviour during leap second (notice SYS time remains frozen 
      * during the leap second)
      * 2015-06-30 23:59:59.800 SYS  ==  2015-06-30 23:59:59.800 UTC 
      * 2015-06-30 23:59:59.900 SYS  ==  2015-06-30 23:59:59.900 UTC 
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.000 UTC <-- leap second start
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.100 UTC 
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.200 UTC 
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.300 UTC 
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.400 UTC 
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.500 UTC 
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.600 UTC 
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.700 UTC 
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.800 UTC 
      * 2015-06-30 23:59:59.999 SYS  ==  2015-06-30 23:59:60.900 UTC 
      * 2015-07-01 00:00:00.000 SYS  ==  2015-07-01 00:00:00.000 UTC <-- leap second end
      * 2015-07-01 00:00:00.100 SYS  ==  2015-07-01 00:00:00.100 UTC 
      * 2015-07-01 00:00:00.200 SYS  ==  2015-07-01 00:00:00.200 UTC 
     */
     auto start = clock_cast<utc_clock>(sys_days{ 2015_y / July / 1 } - 200ms);
     auto end = start + 1.4s;
     
     auto prev_utc = start;
     auto increment = 100ms;

     for (auto utc = start; utc < end; utc += increment)
     {
       auto sys = clock_cast<system_clock>(utc);

       if (utc.time_since_epoch() < seconds(1435708827) /* pre 2015-06-30 23:59:60.000 UTC */) 
       {
         /* 
          * 27 leap seconds from 1970 to this point, composed of:
          *    2 leap seconds between 1970-01-01 to 1972-01-01 (formula-based)
          *   25 leap seconds between 1972-01-01 to 2015-06-30 (discrete insertions)
          */
         assert(seconds(27) == (utc.time_since_epoch() - sys.time_since_epoch()));

         prev_utc = utc;
       }
       else if (utc.time_since_epoch() >= seconds(1435708828) /* post 2015-07-01 00:00:00.000 UTC */)
       {
         /* a new leap second has been inserted */
         assert(seconds(28) == (utc.time_since_epoch() - sys.time_since_epoch()));
       }
       else
       {
         /* in region 2015-06-30 23:59:60.000 UTC to 2015-07-01 00:00:00.000 UTC */
         
         /* sys time frozen */
         assert(milliseconds(1435708799999) == sys.time_since_epoch());

         /* UTC time progresses */
         assert(utc == prev_utc + increment);
         prev_utc = utc;
       }
     }

   }

   /// utc paper example
   {
     auto lu = local_days{2000_y/January/1_d};
     auto ut = clock_cast<utc_clock>(lu);
     assert(clock_cast<local_t>(ut) == lu);
     assert(ut.time_since_epoch() == seconds(946684824));
   }

   /// tai epoch
   {
     auto lt = local_days{1958_y/January/1_d};
     auto tt = clock_cast<tai_clock>(lt);
     assert(clock_cast<local_t>(tt) == lt);
     assert(tt.time_since_epoch() == seconds(0));

     auto lu = local_days{1958_y/January/1_d};
     auto ut = clock_cast<utc_clock>(lu);
     assert(clock_cast<tai_clock>(ut) == tt);
   }

   // tai paper example
   {
      auto lt = local_days{2000_y/January/1_d} + seconds(32);
      auto tt = clock_cast<tai_clock>(lt);
      assert(clock_cast<local_t>(tt) == lt);

      auto lu = local_days{2000_y/January/1_d};
      auto ut = clock_cast<utc_clock>(lu);
      assert(clock_cast<tai_clock>(ut) == tt);
   }

   /// gps epoch
   {
     auto lg = local_days{1980_y/January/Sunday[1]};
     auto gt = clock_cast<gps_clock>(lg);
     assert(clock_cast<local_t>(gt) == lg);
     assert(gt.time_since_epoch() == seconds(0));

     auto lu = local_days{1980_y/January/Sunday[1]};
     auto ut = clock_cast<utc_clock>(lu);
     assert(clock_cast<gps_clock>(ut) == gt);
     assert(ut.time_since_epoch() == seconds(315964811));

     auto lt = local_days{1980_y/January/Sunday[1]} + seconds(19);
     auto tt = clock_cast<tai_clock>(lt);
     assert(clock_cast<gps_clock>(tt) == gt);
     assert(tt.time_since_epoch() == seconds(694656019));
   }

   // gps 2000 example
   {
     auto lg = local_days{2000_y/January/1_d};
     auto gt = clock_cast<gps_clock>(lg);
     assert(clock_cast<local_t>(gt) == lg);

     auto lu = local_days{2000_y/January/1_d} - seconds(13);
     auto ut = clock_cast<utc_clock>(lu);
     assert(clock_cast<gps_clock>(ut) == gt);

     auto lt = local_days{2000_y/January/1_d} + seconds(19);
     auto tt = clock_cast<tai_clock>(lt);
     assert(clock_cast<gps_clock>(tt) == gt);
   }

}
