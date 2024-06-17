#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <boost/asio/ip/tcp.hpp>

#include <async_mqtt5.hpp>

#ifdef BOOST_ASIO_HAS_CO_AWAIT

// Modified completion token that will prevent co_await from throwing exceptions.
constexpr auto use_nothrow_awaitable = boost::asio::as_tuple(boost::asio::use_awaitable);

using client_type = async_mqtt5::mqtt_client<boost::asio::ip::tcp::socket>;

boost::asio::awaitable<bool> subscribe(client_type& client) {
    // Configure the request to subscribe to a Topic.
    async_mqtt5::subscribe_topic sub_topic = async_mqtt5::subscribe_topic{
        "topic1",
        async_mqtt5::subscribe_options {
            async_mqtt5::qos_e::exactly_once, // All messages will arrive at QoS 2.
            async_mqtt5::no_local_e::no, // Forward message from Clients with same ID.
            async_mqtt5::retain_as_published_e::retain, // Keep the original RETAIN flag.
            async_mqtt5::retain_handling_e::send // Send retained messages when the subscription is established.
        }
    };

    // Subscribe to a single Topic.
    auto&& [ec, sub_codes, sub_props] = co_await client.async_subscribe(
        sub_topic, async_mqtt5::subscribe_props {}, use_nothrow_awaitable
    );
    // Note: you can subscribe to multiple Topics in one mqtt_client::async_subscribe call.

    // An error can occur as a result of:
    //  a) wrong subscribe parameters
    //  b) mqtt_client::cancel is called while the Client is in the process of subscribing

    co_return !ec && !sub_codes[0]; // True if the subscription was successfully established.
}

boost::asio::awaitable<bool> unsubscribe(client_type& client) {
    std::vector<std::string> unsub_topic {
        "topic1"
    };

    // Subscribe to a single Topic.
    auto&& [ec, unsub_codes, unsub_props] = co_await client.async_unsubscribe(
        unsub_topic, async_mqtt5::unsubscribe_props {}, use_nothrow_awaitable
    );
    // Note: you can subscribe to multiple Topics in one mqtt_client::async_subscribe call.

    // An error can occur as a result of:
    //  a) wrong subscribe parameters
    //  b) mqtt_client::cancel is called while the Client is in the process of subscribing

    co_return !ec && !unsub_codes[0]; // True if the subscription was successfully established.
}

boost::asio::awaitable<void> subscribe_and_receive(client_type& client) {
    // Configure the Client.
    // It is mandatory to call brokers() and async_run() to configure the Brokers to connect to and start the Client.
    client.brokers("127.0.0.1", 1883) // Broker that we want to connect to. 1883 is the default TCP port.
        .async_run(boost::asio::detached); // Start the client.

    // Before attempting to receive an Application Message from the Topic we just subscribed to,
    // it is advisable to verify that the subscription succeeded.
    // It is not recommended to call mqtt_client::async_receive if you do not have any
    // subscription established as the corresponding handler will never be invoked.
    if (!(co_await subscribe(client)))
        co_return;

    // Publish the sensor reading with QoS 1.
    co_await client.async_publish<async_mqtt5::qos_e::at_least_once>(
        "topic1", "hello",
        async_mqtt5::retain_e::no,
        async_mqtt5::publish_props {},
        use_nothrow_awaitable
    );

    {
        // Receive an Appplication Message from the subscribed Topic(s).
        auto&& [ec, topic, payload, publish_props] = co_await client.async_receive(use_nothrow_awaitable);
    }

    if (!(co_await unsubscribe(client)))
        co_return;

    co_await client.async_disconnect(
        async_mqtt5::disconnect_rc_e::normal_disconnection,
        async_mqtt5::disconnect_props {},
        use_nothrow_awaitable
    );

    co_return;
}

int main() {
    // Initialise execution context.
    boost::asio::io_context ioc;

    // Initialise the Client to connect to the Broker over TCP.
    client_type client(ioc);

    // Spawn the coroutine.
    co_spawn(ioc, subscribe_and_receive(client), boost::asio::detached);

    // Start the execution.
    ioc.run();
}

#endif

//]
