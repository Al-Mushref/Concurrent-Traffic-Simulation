#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

enum TrafficLightPhase { red, green };
// forward declarations to avoid include cycle
class Vehicle;

// FP.3 Define a class „MessageQueue“ which has the public methods send and
// receive. Send should take an rvalue reference of type TrafficLightPhase
// whereas receive should return this type. Also, the class should define an
// std::dequeue called _queue, which stores objects of type TrafficLightPhase.
// Also, there should be an std::condition_variable as well as an std::mutex as
// private members.

template <class T> class MessageQueue {
public:
  void send(T &&traffic_phase);
  T receive();

private:
  std::condition_variable _condition;
  std::mutex _mutex;
  std::deque<T> _queue;
};

// FP.1
class TrafficLight : public TrafficObject {
public:
  TrafficLight();
  void waitForGreen();
  void simulate();
  TrafficLightPhase getCurrentPhase();

private:
  // typical behaviour methods
  void cycleThroughPhases();

  // FP.4b
  std::condition_variable _condition;
  std::mutex _mutex;
  TrafficLightPhase _currentPhase;
  std::shared_ptr<MessageQueue<TrafficLightPhase>> _messages;
};

#endif