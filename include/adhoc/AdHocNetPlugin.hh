#ifndef ADHOCNETPLUGIN_HH_
#define ADHOCNETPLUGIN_HH_

#include <mutex>
#include <memory>
#include <queue>

#include <ros/ros.h>
#include <std_msgs/Bool.h>

#include <gazebo/common/Event.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <sdf/sdf.hh>

namespace adhoc
{
  namespace msgs
  {
    // Forward declarations.
    class Datagram;
  }
}

namespace gazebo
{
  /// \brief A plugin that controls ad hoc communications.
  class AdHocNetPlugin : public WorldPlugin
  {
    // Documentation inherited
    public: virtual void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf);

    // to receive a message to start operation.
    public: void OnStartStopMessage(const ros::MessageEvent<std_msgs::Bool const>& event);

    /// \brief Callback for World Update events.
    private: void OnUpdate();

    /// \brief Process all incoming messages.
    private: void ProcessIncomingMsgs();

    /// \brief Callback executed when a new request is received.
    /// \param _req The datagram contained in the request.
    private: void OnMessage(const boost::shared_ptr<adhoc::msgs::Datagram const> &_req);

    private: void OnClientMessage(const boost::shared_ptr<gazebo::msgs::GzString const> &_data);

    /// \brief World pointer.
    private: physics::WorldPtr world;

    /// \brief Connection to World Update events.
    private: event::ConnectionPtr updateConnection;

    /// \brief An Ignition Transport node for communications.
    private: transport::NodePtr node;

    /// \brief publisher map to send data.
    private: std::map< std::string, transport::PublisherPtr > pubMap;

    /// \brief subscriber map to receive data.
    private: std::map< std::string, transport::SubscriberPtr > subMap;

    /// \brief Collection of incoming messages received during the last
    /// simulation step.
    private: std::queue<adhoc::msgs::Datagram> incomingMsgs;

    /// \brief the time when the robot positions were last displayed.
    private: common::Time lastDisplayed;

    /// \brief Protect data from races.
    private: std::mutex mutex;

    private: ros::NodeHandle n;

    private: bool started;
    private: bool finished;

    private: ros::Subscriber enableSub;

    private: std::mutex mutexStartStop;

    private: transport::SubscriberPtr clientOutputSub;
  };
}
#endif
