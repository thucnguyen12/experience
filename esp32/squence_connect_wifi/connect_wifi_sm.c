static esp_ping_handle_t ping;
static esp_ping_handle_t ping_wifi;
static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    printf("%d bytes from %s icmp_seq=%d ttl=%d time=%d ms\n",
    recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
    vTaskDelay (2);
}

static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    printf("From %s icmp_seq=%d timeout\n", inet_ntoa(target_addr.u_addr.ip4), seqno);
    if (hdl == ping_wifi)
    {
        xEventGroupSetBits(s_wifi_event_group, WIFI_PING_TIMEOUT);
    }
    else
    {
        xEventGroupSetBits(s_ping_event_group, PING_TIMEOUT);
    }
}


static void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args)
{
    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
    uint32_t loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
    if (IP_IS_V4(&target_addr)) {
        printf("\n--- %s ping statistics ---\n", inet_ntoa(*ip_2_ip4(&target_addr)));
    } else {
        printf("\n--- %s ping statistics ---\n", inet6_ntoa(*ip_2_ip6(&target_addr)));
    }
    printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
           transmitted, received, loss, total_time_ms);
    // delete the ping sessions, so that we clean up all resources and can create a new ping session
    // we don't have to call delete function in the callback, instead we can call delete function from other tasks
    /*
        need send a signal    
    */
    if(loss < 100)
    {
        if (hdl == ping_wifi)
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_PING_SUCESS);
            ESP_LOGW (TAG, "        PING WIFI OK");
        }
        else
        {
            ESP_LOGW (TAG, "        PING NORMAL OK");
            xEventGroupSetBits(s_ping_event_group, PING_SUCESS);
        }
    }
    esp_ping_delete_session(hdl);
}
static int do_ping_cmd(esp_netif_t * netif_ping)
{
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    

    config.interval_ms = (uint32_t)(EXAMPLE_PING_INTERVAL * 1000);
    config.count = (uint32_t)(EXAMPLE_PING_COUNT);
    if (netif_ping)
    {
        config.interface = esp_netif_get_netif_impl_index (netif_ping);
    }
    
    // parse IP address
    ip_addr_t target_addr;
    struct addrinfo hint;
    struct addrinfo *res = NULL;
    memset(&hint, 0, sizeof(hint));
    memset(&target_addr, 0, sizeof(target_addr));

    /* convert domain name to IP address */
    if (getaddrinfo(EXAMPLE_PING_IP, NULL, &hint, &res) != 0) {
        printf("ping: unknown host %s\n", EXAMPLE_PING_IP);
        return 1;
    }
    if (res->ai_family == AF_INET) {
        struct in_addr addr4 = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
        inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
    } else {
        struct in6_addr addr6 = ((struct sockaddr_in6 *) (res->ai_addr))->sin6_addr;
        inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &addr6);
    }
    freeaddrinfo(res);
    config.target_addr = target_addr;

    /* set callback functions */
    esp_ping_callbacks_t cbs = {
        .on_ping_success = cmd_ping_on_ping_success,
        .on_ping_timeout = cmd_ping_on_ping_timeout,
        .on_ping_end = cmd_ping_on_ping_end,
        .cb_args = NULL
    };

    if (netif_ping == wifi_netif)
    {
        esp_ping_new_session(&config, &cbs, &ping_wifi);
        esp_ping_start(ping_wifi);
    }
    else
    {
        esp_ping_new_session(&config, &cbs, &ping);
        esp_ping_start(ping);

    }

    return 0;
}


void wifi_connect_manager(void)
{
    
    static wifi_state_t state = WIFI_BEGIN;
    static uint8_t wifi_wait_time = 0;
    EventBits_t bits = 0;
    if (connect_wifi_again)
    {
        connect_wifi_again = false;
        // type_of_process_data = get_type_of_data (WIFI_NAME);
        err = read_config_data_from_flash (&config_infor_now, get_type_of_data (WIFI_NAME), WIFI_NAME);
        err = read_config_data_from_flash (&config_infor_now, get_type_of_data (WIFI_PASS), WIFI_PASS);
        // read_wifi_info_from_flash (wifi_name_temp, sizeof (wifi_name_temp), wifi_pass_temp, sizeof (wifi_pass_temp));
        // if (memcmp (wifi_name_temp, wifi_name, sizeof (wifi_name)) 
        //     || memcmp (wifi_pass_temp, wifi_pass, sizeof (wifi_pass)))
        // {
        //     write_wifi_info_to_flash (wifi_name, wifi_pass);
        // }
        state = WIFI_BEGIN;
    }
    switch (state)
    {
    case WIFI_BEGIN:
        ESP_LOGW (TAG, "WIFI BEGIN");
        
        // sprintf (config_infor_now.wifiname, CONFIG_ESP_WIFI_SSID);
        // sprintf (config_infor_now.wifipass, CONFIG_ESP_WIFI_PASSWORD);
        sprintf (wifi_name, "%s", config_infor_now.wifiname);
        sprintf (wifi_pass, "%s", config_infor_now.wifipass);
        state = WIFI_WAIT_RESPONE;
        if ((strlen (wifi_name) == 0) || (strlen (wifi_pass) == 0))
        {
          state = WIFI_WAIT_SET_UP;
        }
        app_wifi_connect (wifi_name, wifi_pass);

        wifi_started = false;
        break;
    case WIFI_WAIT_SET_UP:
        esp_wifi_disconnect();
        if (strlen (wifi_name) || strlen (wifi_pass))
        {
          state = WIFI_BEGIN;
        }

        break;
    case WIFI_WAIT_RESPONE:
        ESP_LOGW (TAG, "WIFI WAIT");
        bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdTRUE,
                                           pdFALSE,
                                           20);
        if ((bits & WIFI_CONNECTED_BIT) == WIFI_CONNECTED_BIT)
        {
            ESP_LOGI (TAG, "WIFI CONNECT");
            ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                    wifi_name, wifi_pass);
            do_ping_cmd(wifi_netif);
            wifi_started = true;
            state = WIFI_CONNECTED;
        }
        else if ((bits & WIFI_FAIL_BIT) == WIFI_FAIL_BIT)
        {
            ESP_LOGE(TAG, "Failed to connect to SSID:%s,     password:%s",
                    wifi_name, wifi_pass);
            wifi_started = false;
            state = WIFI_DISCONNECTED;
        }
        if (wifi_wait_time++ > 15)
        {
            esp_ping_delete_session(ping_wifi);
            esp_ping_stop(ping_wifi);
            wifi_wait_time = 0;
            wifi_started = false;
            state = WIFI_DISCONNECTED;
        }
        break;
    case WIFI_CONNECTED:
        
        bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_PING_TIMEOUT | WIFI_PING_SUCESS, pdTRUE, pdFALSE, 10);
        ESP_LOGW (TAG, "WIFI CONNECTED: %d", bits);
        // co the wait ping luon trong day
        if ((bits & WIFI_PING_SUCESS) == WIFI_PING_SUCESS)
        {
            ESP_LOGI (TAG, "WIFI PING OK");
            state = WIFI_WAIT_PING_OK;
        }
        else if ((bits & WIFI_PING_TIMEOUT) == WIFI_PING_TIMEOUT)
        {
            wifi_started = false;
            ESP_LOGW (TAG, "WIFI PING TIMEOUT");
            state = WIFI_WAIT_PING_TIME_OUT;
        }

        if (wifi_wait_time++ > 20)
        {
            ESP_LOGW (TAG, "CONNNECT BUT CAN'T PING!!! RECONNECT");
            wifi_wait_time = 0;
            wifi_started = false;
            state = WIFI_DISCONNECTED;
        }
        state = WIFI_WAIT_PING_OK;
        break;
    case WIFI_WAIT_PING_OK:
        ESP_LOGW (TAG, "WIFI PING OK");
        bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_FAIL_BIT, pdTRUE, pdFALSE, 0);
        if ((bits & WIFI_FAIL_BIT) == WIFI_FAIL_BIT)
        {
            state = WIFI_DISCONNECTED;
        }

        break;
    case WIFI_WAIT_PING_TIME_OUT:

        state = WIFI_DISCONNECTED;
        break;
    case WIFI_DISCONNECTED:
        ESP_LOGW (TAG, "WIFI DISCONNECTED");
        // esp_wifi_stop();
        wifi_started = false;
        state = WIFI_BEGIN;
        break;
    default:
        break;
    }

    
}
