# Schroeder Reverb Multithreaded
This is a project that is part of the Real-Time Systems 2 course taken on the University of Twente. It is a group project of Thijs Bink and Geert Roks.

The goal of this project is to create a reverb audio effect that runs multithreaded on a Raspberry Pi 3B. The reverb algorithm used is the Schroeder Reverb Algorithm, which used 4 parallel comb filters with 3 allpass filters in series. These processes need to be parallelized effectively. This is done with Data Flow graphs to reason about the performance and bottlenecks. The program is written with modern C++ using std::thread.

## Task graph


## Dataflow Graph


## Audio Input and output schematics


## Sources
Electrosmash PedalPi schematic/code: https://www.electrosmash.com/pedal-pi
Electrosmash PWM audio output: https://www.electrosmash.com/forum/pedal-pi/210-pwm-audio-on-raspberry-pi-zero
Schroeder Reverb Algorithm CCRMA: https://ccrma.stanford.edu/~jos/pasp/Schroeder_Reverberators.html
