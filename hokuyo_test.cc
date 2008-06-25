#include "hokuyo.hh"
#include <iostream>
#include <sys/time.h>
#include <time.h>

using namespace std;

int main (int argc, const char** argv){
  if (argc<2){
    printf( "Usage: urg_test <device> ");
    return 0;
  }

  URG urg;
  if (!urg.open(argv[1]))
  {
      cerr << "cannot open device: " << urg.errorString() << endl;
      perror("errno is");
      return 1;
  }
  cout << urg.getInfo() << endl;

  if (!urg.setBaudrate(115200))
  {
      cerr << "cannot set baudrate: " << urg.errorString() << endl;
      perror("errno is");
      return 1;
  }

  if (!urg.startAcquisition(0, -1, -1, 0, 2))
  {
      cerr << "cannot start acquisition: " << urg.errorString() << endl;
      perror("errno is");
      return 1;
  }

  URG::RangeReading ranges;
  timeval reftime;
  gettimeofday(&reftime, 0);
  for (int i = 0; i < 20; ++i)
  {
      if (!urg.readRanges(ranges, 1000))
      {
          cerr << "failed to read ranges: " << urg.errorString() << endl;
          perror("errno is");
          return 1;
      }

      int too_far = 0;
      int bad_ranges = 0;
      for (int range_idx = 0; range_idx < ranges.count; ++range_idx)
      {
          unsigned int val = ranges.ranges[range_idx];
          if (val < 20)
          {
              if (val == URG::TOO_FAR)
                  too_far++;
              else
                  bad_ranges++;
          }
      }

      int dt = (ranges.cpu_timestamp.tv_sec - reftime.tv_sec) * 1000 + ranges.cpu_timestamp.tv_usec / 1000;
      cerr << i << " " << ranges.device_timestamp << " " << dt << "\n"
          << "  too far: " << too_far << "\n"
          << "  invalid: " << bad_ranges << endl;

  }

  urg.stopAcquisition();
  for (int i = 0; i < 20; ++i)
  {
      if (!urg.readRanges(ranges, 1000))
      {
          cerr << "the device did stop scanning ..." << endl;
          break;
      }
  }

  urg.close();
}

