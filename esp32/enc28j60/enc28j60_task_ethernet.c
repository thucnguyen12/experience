static void ethernet_init (void)
{

      //eth netif config
    esp_netif_inherent_config_t netif_eth_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    //netif_eth_config.lost_ip_event = IP_EVENT_ETH_LOST_IP;
    netif_eth_config.route_prio = 1;
    netif_eth_config.if_desc = "netif_eth";
    esp_netif_config_t cfg = {
        .base = &netif_eth_config,                 // use specific behaviour configuration
        .driver = NULL,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH, // use default WIFI-like network stack configuration
    };
    eth_netif = esp_netif_new(&cfg);

     /* Register event handler */
    ESP_ERROR_CHECK(esp_eth_set_default_handlers(eth_netif));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL)); //  FOR ETH
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL)); // FOR IP
#if USE_ETH_IP101
    // Init MAC and PHY configs to default
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    // reinit gpio
    phy_config.reset_gpio_num = CONFIG_ETH_PHY_RST_GPIO;
    mac_config.smi_mdc_gpio_num = CONFIG_ETH_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = CONFIG_ETH_MDIO_GPIO;
#warning "Ethernet chua dc init"

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
  
  if (esp_eth_driver_install(&config, &eth_handle) == ESP_OK)
    {
        /* attach Ethernet driver to TCP/IP stack */
        ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

        esp_eth_start (eth_handle);
        ESP_LOGI (TAG, "netif attach done");
    }
    else
    {
        ESP_LOGE (TAG, "INSTALL ETH DRIVER FAIL\r\n");
        // we need do sth here!!!!!!!!!!!!!!!!!!
    }
    /* start Ethernet driver state machine */
#elif USE_ETH_ENC28J60
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
        spi_bus_config_t buscfg = {
        .miso_io_num = CONFIG_EXAMPLE_ENC28J60_MISO_GPIO,
        .mosi_io_num = CONFIG_EXAMPLE_ENC28J60_MOSI_GPIO,
        .sclk_io_num = CONFIG_EXAMPLE_ENC28J60_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(1, &buscfg, 1));
    /* ENC28J60 ethernet driver is based on spi driver */
    spi_device_interface_config_t devcfg = {
        .command_bits = 3,
        .address_bits = 5,
        .mode = 0,
        .clock_speed_hz = 6 * 1000 * 1000,
        .spics_io_num = CONFIG_EXAMPLE_ENC28J60_CS_GPIO,
        .queue_size = 20
    };
    spi_device_handle_t spi_handle = NULL;
    ESP_ERROR_CHECK(spi_bus_add_device(1, &devcfg, &spi_handle));

    eth_enc28j60_config_t enc28j60_config = ETH_ENC28J60_DEFAULT_CONFIG(spi_handle);
    enc28j60_config.int_gpio_num = 4; //chan INT chua ddc noi di dau ca nene chon 1 chana chuwa dc noi la 21 de ko anh hg 

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    mac_config.smi_mdc_gpio_num = -1;  // ENC28J60 doesn't have SMI interface
    mac_config.smi_mdio_gpio_num = -1;
    esp_eth_mac_t *mac = esp_eth_mac_new_enc28j60(&enc28j60_config, &mac_config);

    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.autonego_timeout_ms = 0; // ENC28J60 doesn't support auto-negotiation
    phy_config.reset_gpio_num = -1; // ENC28J60 doesn't have a pin to reset internal PHY
    esp_eth_phy_t *phy = esp_eth_phy_new_enc28j60(&phy_config);
        /* ENC28J60 doesn't burn any factory MAC address, we need to set it manually.
       02:00:00 is a Locally Administered OUI range so should not be used except when testing on a LAN under your control.
    */

 
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);

    if (esp_eth_driver_install(&config, &eth_handle) == ESP_OK)
    {
        mac->set_addr(mac, (uint8_t[]) {
        0x02, 0x00, 0x00, 0x12, 0x34, 0x56
        });
        /* attach Ethernet driver to TCP/IP stack */
        ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

        esp_eth_start (eth_handle);
        ESP_LOGI (TAG, "netif attach done");
    }
    else
    {
        ESP_LOGE (TAG, "INSTALL ETH DRIVER FAIL\r\n");
        // we need do sth here!!!!!!!!!!!!!!!!!!
    }
#endif


    // /* attach Ethernet driver to TCP/IP stack */
    // ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));
    // /* start Ethernet driver state machine */
    // ESP_ERROR_CHECK(esp_eth_start(eth_handle));

}