#include "TrafficLight.h"
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  // FP.5a
  std::unique_lock<std::mutex> lock(_mutex);
  _condition.wait(lock, [this] { return !_queue.empty(); });
  T msg = std::move(_queue.back());
  _queue.pop_back();
  return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  // FP.4a
  std::lock_guard<std::mutex> lock(_mutex);
  _queue.push_back(std::move(msg));
  _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() {
  _currentPhase = TrafficLightPhase::red;
  _messages = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

void TrafficLight::waitForGreen() {
  // FP.5b :
  while (true) {
    if (_messages->receive() == TrafficLightPhase::green) {
      return;
    }
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  // FP.2b : Finally, the private method „cycleThroughPhases“ should be started
  // in a thread when the public method „simulate“ is called. To do this, use
  // the thread queue in the base class.
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
  // FP.2a : Implement the function with an infinite loop that measures the time
  // between two loop cycles and toggles the current phase of the traffic light
  // between red and green and sends an update method to the message queue using
  // move semantics. The cycle duration should be a random value between 4 and 6
  // seconds. Also, the while-loop should use std::this_thread::sleep_for to
  // wait 1ms between two cycles.

  std::unique_lock<std::mutex> lock(_mutex);
  // generate a radom number
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dist(4000, 6000);
  double cycle_duration = dist(gen);
  // monitoring time
  std::chrono::time_point<std::chrono::system_clock> last_update;

  // init stop watch
  last_update = std::chrono::system_clock::now();
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    long time_since_last_update =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - last_update)
            .count();

    if (time_since_last_update >= cycle_duration) {
      _currentPhase == TrafficLightPhase::red
          ? _currentPhase = TrafficLightPhase::green
          : _currentPhase = TrafficLightPhase::red;

      _messages->send(std::move(_currentPhase));
      std::cout << "Traffic Light #" << getID() << ", Traffic Phase: "
                << (_currentPhase == TrafficLightPhase::red ? "red" : "green")
                << std::endl;
      last_update = std::chrono::system_clock::now();
    }
  }
}
