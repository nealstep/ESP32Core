#pragma once

#include "log/e32c_log.hpp"

#if USE_QUEUE
#include "circular_queue.hpp"
#endif  // USE_QUEUE

#include "version.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif  // ARDUINO

class ESP32Net {
   protected:
    // make encryption default if on
    static constexpr bool use_aes = USE_AES;

   public:
    class Config {
       public:
        // sizes
        static constexpr uint8_t tz_size = 32 + 1;
        static constexpr uint8_t ssid_size = 32 + 1;
        static constexpr uint8_t passwd_size = 64 + 1;
        static constexpr uint8_t udp_msg_size = 200;
        static constexpr uint8_t net_queue_size = 10;
        static constexpr uint8_t udp_queue_size = 10;
#if USE_AES
        static constexpr uint8_t hex_key_size = 64 + 1;
        static constexpr uint8_t aes_key_size = 32;
        static constexpr uint8_t iv_size = 12;
        static constexpr uint8_t tag_size = 16;
        static constexpr uint8_t prefix_size = iv_size + tag_size;
#endif  // USE_AES

        // code
        static constexpr uint8_t wait = 0;
        static constexpr uint8_t nocode = 0;
        static constexpr uint8_t httpcode = 204;

        // counts
        static constexpr uint8_t time_sync_attempts = 5;

        // chars
        static constexpr char eos = '\0';

        // sites
        static constexpr const char* internet_check_url =
            "http://clients3.google.com/generate_204";
        static constexpr const char* ntp_server_1 = "0.pool.ntp.org";
        static constexpr const char* ntp_server_2 = "1.pool.ntp.org";
        static constexpr const char* ntp_server_3 = "2.pool.ntp.org";
    };

#ifdef ARDUINO

    class Message {
       public:
        bool encrypt;
        IPAddress destination;
        uint16_t port;
        char str[Config::udp_msg_size];

        // we will coerce IPaddress in a uint32_t for storage
        static constexpr size_t base_size =
            sizeof(bool) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(size_t);
        static constexpr size_t max_size = base_size + Config::udp_msg_size;

        size_t size(void) { return base_size + strlen(str) + 1; }
        bool serialize(uint8_t* data, size_t len);
        bool deserialize(uint8_t* data, size_t len);
    };

    typedef struct {
        enum class Type : uint8_t {
            Connected = 0,
            GotIP,
            Disconnected,
            TimeSynced,
            TimeSyncFailed,
            InternetConnected,
            NoInternet
        } type;
        uint8_t code;
    } NetMessage;

    typedef struct {
        IPAddress remoteIP;
        bool broadcast;
        char data[Config::udp_msg_size];
    } UDPMessage;

    // statuses
    bool udp_init = false;
    bool ota_init = false;

    // broadcast ip
    IPAddress broadcastIP;

    // queues
    QueueHandle_t netQueue;
    QueueHandle_t udpQueue;

#endif  // ARDUINO

    // lazy singleton
    static ESP32Net& getInstance(void) {
        static ESP32Net instance;
        return instance;
    }
    ESP32Net(const ESP32Net&) = delete;
    ESP32Net& operator=(const ESP32Net&) = delete;

#ifdef ARDUINO
    void early_init(void);
    Log::Err init(void);
    bool have_ip(void) { return ip_ready; }
    void set_ip(IPAddress ip) {
        local_ip = ip;
        if (ip == INADDR_NONE) ip_ready = false;
    }
    void set_ip_ready();
    void set_subnet_mask(IPAddress smask) {
        subnet_mask = smask;
        subnet_addr = (uint32_t)local_ip & subnet_mask;
        broadcast_addr = (uint32_t)local_ip | ~subnet_mask;
        broadcastIP = broadcast_addr;
    }
    IPAddress get_ip(void) { return local_ip; }
    bool have_internet(void) { return internet_connected; }
    bool check_internet(void);
    Log::Err check_clock(void);
    Log::Err check_queue(void);
    void queue_net_msg(NetMessage::Type type, uint8_t code);
    Log::Err broadcast_str(const char* str, bool encrypt = use_aes,
                           uint16_t port = 0) {
        if (broadcastIP != INADDR_NONE)
            return send_str(broadcastIP, str, encrypt, port);
        else
            return Log::Err::NoError;
    }
    Log::Err log_str(const char* str, bool encrypt = use_aes,
                     uint16_t port = 0) {
        if (remote_host != INADDR_NONE)
            return send_str(remote_host, str, encrypt, port);
        else
            return Log::Err::NoError;
    }
    Log::Err send_str(IPAddress ip, const char* str, bool encrypt = use_aes,
                      uint16_t port = 0);
    Log::Err update_ssid(const char* new_ssid);
    Log::Err update_password(const char* new_password);
    Log::Err update_ota_password(const char* new_ota_password);
#if USE_AES
    Log::Err update_aes_key(const char* new_hex_key);
#endif  // USE_AES
    void reconnect(void);
#endif  // ARDUINO
    void update_remote_host(const char* ip_s) {
        remote_host = IPAddress(ip_s);
        LOG_N(Log::Uni::Net, Log::Sev::Inf, Log::Note::SimpleValueStr,
              "Remote IP", remote_host.toString().c_str());
    }

   protected:
#ifdef ARDUINO
    // flags
    bool internet_connected = false;

    // ip address
    IPAddress local_ip = INADDR_NONE;
    uint32_t subnet_mask = 0;
    uint32_t subnet_addr = 0;
    uint32_t broadcast_addr = 0;
    IPAddress remote_host = INADDR_NONE;
    bool ip_ready = false;

#if USE_AES
    uint8_t send_buffer[Config::udp_msg_size + Config::prefix_size];
#else
    uint8_t send_buffer[Config::udp_msg_size];
#endif  // USE_AES ELSE

#if USE_AES
    uint8_t aes_key[Config::aes_key_size];
#endif  // USE_AES

#if USE_QUEUE
    CircularQueue* local_q = nullptr;
    CircularQueue* internet_q = nullptr;
#endif  // USE_QUEUE
#endif  // ARDUINO
    // hidden creator
    ESP32Net(void) {};

#ifdef ARDUINO

    Log::Err connection_check(IPAddress ip, bool& local);
#if USE_QUEUE
    Log::Err queue_message(CircularQueue& q, Message& m);
    Log::Err empty_queue(CircularQueue& q, bool local_only);
#endif  // USE_QUEUE
    Log::Err send_message(Message& message);
#if USE_AES
    uint8_t nibbleToHex(char nibble);
    void genAesKey(void);
#endif  // USE_AES
#endif  // ARDUINO
};

static ESP32Net& esp32Net = ESP32Net::getInstance();

#ifdef ARDUINO
void get_net_prefs(void);
void ota_check(void);
bool check_internet_t(void);
#endif  // ARDUINO
