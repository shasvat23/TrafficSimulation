#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <future>

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
  
  	std::unique_lock<std::mutex> lck(_mutex);
  	_condition.wait(lck,[this]{return !q.empty();});
    T msg = std::move(q.back());
  	q.pop_back();
  	return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
  std::lock_guard<std::mutex> lck(_mutex);
  q.push_back(std::move(msg)); 
  _condition.notify_one();  
  
}


/* Implementation of class "TrafficLight" */

/* 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
}

*/

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}
void TrafficLight::simulate()
{
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}
TrafficLightPhase TrafficLight::getCurrentPhase()
{
  return _currentPhase;
}

void TrafficLight::waitForGreen()
{
  while(true)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto currPhase = msgQ->receive();
    if(currPhase == green)
      return;
  }
  
}
void TrafficLight::cycleThroughPhases()
{
	srand(time(NULL));
	int randNum = (rand() % 6) + 4; 
  
  std::unique_lock<std::mutex> lck(_mtx) ;
  std::cout<<"traffic light : "<< _id << " cylcing through phase:: thread id = " <<std::this_thread::get_id() <<std::endl;
  lck.unlock(); 
  auto uiNow = std::chrono::system_clock::now() ; 
  while (true)
  {
    auto timeElapsed = std::chrono::duration_cast<std::chrono::seconds>		(std::chrono::system_clock::now() - uiNow).count();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (timeElapsed >= randNum)
    {

          if (_currentPhase == red)
          {
            _currentPhase = green;
          }
          else
          {
            _currentPhase = red ;
          }
          randNum = (rand() % 6) + 4;
       auto phase = _currentPhase ; 
    std::future<void> ftr = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send,msgQ,std::move(phase));
    ftr.wait();
      uiNow = std::chrono::system_clock::now(); 
    }
   
    
    
     
  }
         
}
  

  	