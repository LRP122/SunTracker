#include "wifi.h"

namespace WIFI
{
    // Define static members
    wifi_init_config_t Wifi::_wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t Wifi::_wifi_cfg = {};                // Empty structure for now
    Wifi::state_e Wifi::_state = Wifi::state_e::NOT_INITIALIZED; // Default state
    char Wifi::_mac_addr_cstr[13] = {};                // Empty MAC address string
    std::mutex Wifi::_mutx;                            // Mutex for synchronization
    // Statics
    Wifi::Wifi(void)
    {
        if (!_mac_addr_cstr[0])
        {
            if (ESP_OK != _get_mac())
            {
                esp_restart();
            }
        }
    }

    void Wifi::wifi_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
    {
        if (WIFI_EVENT == event_base)
        {
            const wifi_event_t event_type{static_cast<wifi_event_t>(event_id)};

            switch (event_type)
            {
            case WIFI_EVENT_STA_START:
            {
                std::lock_guard<std::mutex> state_guard(_mutx);
                _state = state_e::READY_TO_CONNECT;
                break;
            }
            case WIFI_EVENT_STA_CONNECTED:
            {
                std::lock_guard<std::mutex> state_guard(_mutx);
                _state = state_e::WAITING_FOR_IP;
                break;
            }
            case WIFI_EVENT_STA_DISCONNECTED:
            {
                std::lock_guard<std::mutex> state_guard(_mutx);
                _state = state_e::DISCONNECTED;
                break;
            }

            default:
                break;
            }
        }
    }

    void Wifi::ip_event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
    {
        if (IP_EVENT == event_base)
        {
            const ip_event_t event_type{static_cast<ip_event_t>(event_id)};

            switch (event_type)
            {
            case IP_EVENT_STA_GOT_IP:
            {
                std::lock_guard<std::mutex> state_guard(_mutx);
                _state = state_e::CONNECTED;
                break;
            }
            case IP_EVENT_STA_LOST_IP:
            {
                std::lock_guard<std::mutex> state_guard(_mutx);
                if (state_e::DISCONNECTED != _state)
                {
                    _state = state_e::WAITING_FOR_IP;
                }
                break;
            }

            default:
                break;
            }
        }
    }

    esp_err_t Wifi::Begin()
    {
        std::lock_guard<std::mutex> connect_guard(_mutx);

        esp_err_t status{ESP_OK};

        switch (_state)
        {
        case state_e::READY_TO_CONNECT:
        case state_e::DISCONNECTED:
            status = esp_wifi_connect();
            if (ESP_OK == status)
            {
                _state = state_e ::CONNECTING;
            }
            break;
        case state_e::CONNECTING:
        case state_e::WAITING_FOR_IP:
        case state_e::CONNECTED:
            break;
        case state_e::NOT_INITIALIZED:
        case state_e::INITIALIZED:
        case state_e::ERROR:
            status = ESP_FAIL;
            break;
        }
        return status;
    }
    esp_err_t Wifi::_init()
    {
        std::lock_guard<std::mutex> mutx_guard(_mutx);

        esp_err_t status{ESP_OK};

        if (state_e::NOT_INITIALIZED == _state)
        {
            status |= esp_netif_init();
            if (ESP_OK == status)
            {
                const esp_netif_t *const p_netif = esp_netif_create_default_wifi_sta();

                if (!p_netif)
                {
                    status = ESP_FAIL;
                }
            }

            if (ESP_OK == status)
            {
                status = esp_wifi_init(&_wifi_init_cfg);
            }

            if (ESP_OK == status)
            {
                status = esp_event_handler_instance_register(WIFI_EVENT,
                                                             ESP_EVENT_ANY_ID,
                                                             &wifi_event_handler,
                                                             nullptr,
                                                             nullptr);
            }

            if (ESP_OK == status)
            {
                status = esp_event_handler_instance_register(IP_EVENT,
                                                             ESP_EVENT_ANY_ID,
                                                             &ip_event_handler,
                                                             nullptr,
                                                             nullptr);
            }

            if (ESP_OK == status)
            {
                status = esp_wifi_set_mode(WIFI_MODE_STA);
            }

            if (ESP_OK == status)
            {
                _wifi_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
                _wifi_cfg.sta.pmf_cfg.capable = true;
                _wifi_cfg.sta.pmf_cfg.required = false;

                status = esp_wifi_set_config(WIFI_IF_STA, &_wifi_cfg);
            }

            if (ESP_OK == status)
            {
                status = esp_wifi_start(); // start Wifi
            }

            if (ESP_OK == status)
            {
                _state = state_e::INITIALIZED;
            }
        }

        else if (state_e::ERROR == _state)
        {
            _state = state_e::NOT_INITIALIZED;
        }

        return status;
    }

    void Wifi::SetCredentials(const char *ssid, const char *password)
    {
        memcpy(_wifi_cfg.sta.ssid, ssid, std::ranges::min(strlen(ssid), sizeof(_wifi_cfg.sta.ssid)));

        memcpy(_wifi_cfg.sta.password, password, std::ranges::min(strlen(password), sizeof(_wifi_cfg.sta.password)));
    }

    esp_err_t Wifi::Init()
    {
        return _init();
    }

    // Get default MAC from API and convert to ASCII HEX
    esp_err_t Wifi::_get_mac(void)
    {
        uint8_t mac_byte_buffer[6]{};

        const esp_err_t status{esp_efuse_mac_get_default(mac_byte_buffer)};

        if (ESP_OK == status)
        {
            snprintf(_mac_addr_cstr, sizeof(_mac_addr_cstr), "%02X%02X%02X%02X%02X%02X",
                     mac_byte_buffer[0],
                     mac_byte_buffer[1],
                     mac_byte_buffer[2],
                     mac_byte_buffer[3],
                     mac_byte_buffer[4],
                     mac_byte_buffer[5]);
        }

        return status;
    }

void Wifi::wifi_run()
{

    char *ourTaskName = pcTaskGetName(NULL);
    ESP_LOGI(ourTaskName, "Starting Up!\n");
    
    wifiState = Wifi::GetState();

    switch (wifiState)
    {
    case WIFI::Wifi::state_e::READY_TO_CONNECT:
        ESP_LOGI(ourTaskName,"Wifi Status: READY_TO_CONNECT\n");
        Wifi::Begin();
        break;
    case WIFI::Wifi::state_e::DISCONNECTED:
        ESP_LOGI(ourTaskName,"Wifi Status: DISCONNECTED\n");
        Wifi::Begin();
        break;
    case WIFI::Wifi::state_e::CONNECTING:
        ESP_LOGI(ourTaskName,"Wifi Status: CONNECTING\n");
        break;
    case WIFI::Wifi::state_e::WAITING_FOR_IP:
        ESP_LOGI(ourTaskName,"Wifi Status: WAITING_FOR_IP\n");
        break;;
    case WIFI::Wifi::state_e::ERROR:
        ESP_LOGI(ourTaskName,"Wifi Status: ERROR\n");
        break;
    case WIFI::Wifi::state_e::CONNECTED:
        ESP_LOGI(ourTaskName,"Wifi Status: CONNECTED\n");
        break;
    case WIFI::Wifi::state_e::NOT_INITIALIZED:
        ESP_LOGI(ourTaskName,"Wifi Status: NOT_INITIALIZED\n");
        break;
    case WIFI::Wifi::state_e::INITIALIZED:
        ESP_LOGI(ourTaskName,"Wifi Status: INITIALIZED\n");
        break;
    }
}

void Wifi::wifi_setup(){

    esp_event_loop_create_default();
    nvs_flash_init();

    Wifi::SetCredentials("WIFI", "PW");
    Wifi::Init();

}

void Wifi::connect_to_wifi(){

    Wifi::wifi_setup();

    char *ourTaskName = pcTaskGetName(NULL);
    ESP_LOGI(ourTaskName, "Starting Up!\n");

    while(Wifi::GetState() != WIFI::Wifi::state_e::CONNECTED){
        vTaskDelay(pdMS_TO_TICKS(1000));
        Wifi::wifi_run();
    }
}


} // namespace WIFI

