/**
 * @file test_parser.cpp
 * @author TheSomeMan
 * @date 2023-10-09
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "parser.h"
#include "gtest/gtest.h"
#include "ruuvi_endpoint_ca_uart.h"
#include <string>

using namespace std;

/*** Google-test class implementation
 * *********************************************************************************/

class TestParser;
static TestParser* g_pTestClass;


class TestParser : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
        g_pTestClass               = this;
    }

    void
    TearDown() override
    {
        g_pTestClass = nullptr;
    }

public:
    TestParser();

    ~TestParser() override;
};

TestParser::TestParser()
    : Test()
{
}

TestParser::~TestParser() = default;

extern "C" {

} // extern "C"

/*** Unit-Tests
 * *******************************************************************************************************/

TEST_F(TestParser, test_1) // NOLINT
{
    const std::array<uint8_t, 47> buf = {
        0xCAU, 0x29U, 0x10U, 0xE3U, 0x75U, 0xCFU, 0x37U, 0x4EU, 0x23U, 0x2CU, 0x02U, 0x01U, 0x06U, 0x1BU, 0xFFU, 0x99U,
        0x04U, 0x05U, 0x17U, 0x0BU, 0x71U, 0xDFU, 0xC6U, 0xF2U, 0x03U, 0xFCU, 0xFFU, 0x34U, 0x00U, 0x00U, 0x8CU, 0x36U,
        0xCDU, 0xD4U, 0x7AU, 0xE3U, 0x75U, 0xCFU, 0x37U, 0x4EU, 0x23U, 0x2CU, 0xD2U, 0x2CU, 0x69U, 0xB6U, 0x0AU,
    };
    re_ca_uart_parser_obj_t state;
    parser_init(&state);
    re_ca_uart_message_t message = {};
    std::array<uint8_t, buf.size()> message_array {};

    // Message 1
    {
        for (size_t i = 0; i < buf.size() - 1 - 2 - 1; ++i)
        {
            ASSERT_FALSE(parser_handle_byte(&state, buf[i])) << "i=" << i;
        }
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 4])); // Last byte of data
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 3])); // CRC1
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 2])); // CRC2
        ASSERT_TRUE(parser_handle_byte(&state, buf[buf.size() - 1])); // ETX

        memset(&message, 0, sizeof(message));
        ASSERT_EQ(buf.size(), parser_get_message(&state, &message));

        std::copy(message.buffer, message.buffer + buf.size(), message_array.begin());
        ASSERT_EQ(buf, message_array);
    }

    // Message 2
    {
        for (size_t i = 0; i < buf.size() - 1 - 2 - 1; ++i)
        {
            ASSERT_FALSE(parser_handle_byte(&state, buf[i])) << "i=" << i;
        }
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 4])); // Last byte of data
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 3])); // CRC1
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 2])); // CRC2
        ASSERT_TRUE(parser_handle_byte(&state, buf[buf.size() - 1])); // ETX

        memset(&message, 0, sizeof(message));
        ASSERT_EQ(buf.size(), parser_get_message(&state, &message));

        std::copy(message.buffer, message.buffer + buf.size(), message_array.begin());
        ASSERT_EQ(buf, message_array);
    }

    // Message 3
    {
        for (size_t i = 0; i < buf.size() - 1 - 2 - 1; ++i)
        {
            ASSERT_FALSE(parser_handle_byte(&state, buf[i])) << "i=" << i;
        }
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 4])); // Last byte of data
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 3])); // CRC1
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 2])); // CRC2
        ASSERT_TRUE(parser_handle_byte(&state, buf[buf.size() - 1])); // ETX

        memset(&message, 0, sizeof(message));
        ASSERT_EQ(buf.size(), parser_get_message(&state, &message));

        std::copy(message.buffer, message.buffer + buf.size(), message_array.begin());
        ASSERT_EQ(buf, message_array);
    }

    // Message 4
    {
        for (size_t i = 0; i < buf.size() - 1 - 2 - 1; ++i)
        {
            ASSERT_FALSE(parser_handle_byte(&state, buf[i])) << "i=" << i;
        }
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 4])); // Last byte of data
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 3])); // CRC1
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 2])); // CRC2
        ASSERT_TRUE(parser_handle_byte(&state, buf[buf.size() - 1])); // ETX

        memset(&message, 0, sizeof(message));
        ASSERT_EQ(buf.size(), parser_get_message(&state, &message));

        std::copy(message.buffer, message.buffer + buf.size(), message_array.begin());
        ASSERT_EQ(buf, message_array);
    }

    // Message 5
    {
        for (size_t i = 0; i < buf.size() - 1 - 2 - 1; ++i)
        {
            ASSERT_FALSE(parser_handle_byte(&state, buf[i])) << "i=" << i;
        }
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 4])); // Last byte of data
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 3])); // CRC1
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 2])); // CRC2
        ASSERT_TRUE(parser_handle_byte(&state, buf[buf.size() - 1])); // ETX

        memset(&message, 0, sizeof(message));
        ASSERT_EQ(buf.size(), parser_get_message(&state, &message));

        std::copy(message.buffer, message.buffer + buf.size(), message_array.begin());
        ASSERT_EQ(buf, message_array);
    }

    // Message 6
    {
        for (size_t i = 0; i < buf.size() - 1 - 2 - 1; ++i)
        {
            ASSERT_FALSE(parser_handle_byte(&state, buf[i])) << "i=" << i;
        }
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 4])); // Last byte of data
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 3])); // CRC1
        ASSERT_FALSE(parser_handle_byte(&state, buf[buf.size() - 2])); // CRC2
        ASSERT_TRUE(parser_handle_byte(&state, buf[buf.size() - 1])); // ETX

        ASSERT_EQ(buf.size(), parser_get_message(&state, &message));

        std::copy(message.buffer, message.buffer + buf.size(), message_array.begin());
        ASSERT_EQ(buf, message_array);
    }
}
