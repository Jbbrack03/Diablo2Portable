#include <gtest/gtest.h>
#include "network/network_manager.h"
#include "network/game_session.h"
#include <thread>
#include <chrono>

using namespace d2::network;

class NetworkReceiveTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = std::make_unique<NetworkManager>();
        manager->initialize();
    }
    
    void TearDown() override {
        manager.reset();
    }
    
    std::unique_ptr<NetworkManager> manager;
};

// Test that GameSession can receive data over the network
TEST_F(NetworkReceiveTest, ReceiveNetworkData) {
    // Host a game session
    auto session = manager->hostGame("Receive Test", 2);
    ASSERT_TRUE(session.isActive());
    ASSERT_GT(session.getSocketDescriptor(), 0);
    
    // Test receiving data
    std::vector<uint8_t> receivedData;
    bool dataReceived = session.receiveRawData(receivedData, 100); // 100ms timeout
    
    // Since no client is connected, we expect no data but the method should exist
    EXPECT_FALSE(dataReceived); // No data expected without a client
    EXPECT_TRUE(receivedData.empty());
    
    // The method should be implemented and return false when no data is available
    EXPECT_EQ(session.getBytesReceived(), 0u);
}

// Test non-blocking receive operation
TEST_F(NetworkReceiveTest, NonBlockingReceive) {
    auto session = manager->hostGame("NonBlocking Test", 2);
    ASSERT_TRUE(session.isActive());
    
    // Set socket to non-blocking mode
    bool nonBlockingSet = session.setNonBlocking(true);
    EXPECT_TRUE(nonBlockingSet);
    
    // Try to receive data (should return immediately)
    std::vector<uint8_t> buffer;
    bool hasData = session.receiveRawData(buffer, 0); // 0 timeout = non-blocking
    
    EXPECT_FALSE(hasData); // No data without client
    EXPECT_TRUE(session.isNonBlocking());
}