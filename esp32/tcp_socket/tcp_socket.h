#ifndef TCP_CONSOLE_H
#define TCP_CONSOLE_H

#include "stdint.h"

/**
 * @brief           Start TCP console
 */
void tcp_console_start(void);

/**
 * @brief           Put data to tcp console
 * @param[in]       TCP console data
 */
int32_t tcp_console_puts(char *msg);

/**
 * @brief           Close tcp console sock
 */
void tcp_console_close(void);

void send_tcp_msg_to_server (uint8_t* buffer, uint32_t size);
#endif /* TCP_CONSOLE_H */