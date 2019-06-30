#include <cstdlib>
#include <xc.h>
#include <sys/attribs.h>
#include <proc/p32mz1024eff064.h>
#include "ESP8266.h"

//using namespace std;

 
//**Function to send one byte of date to UART**//
void _esp8266_putch(char bt)  
{
    U3STAbits.UTXEN = 1;                // Make sure transmitter is enabled
                    
    while(U3STAbits.UTXBF);             // Wait while buffer is full
    U3TXREG = bt;                     // Transmit character
      
}
//_____________End of function________________//
 
//**Function to get one byte of date from UART**//

char _esp8266_getch()   
{
  //  if(OERR) // check for Error 
    //{
      //  CREN = 0; //If error -> Reset 
        //CREN = 1; //If error -> Reset 
    //}
    
    while(!U3STAbits.URXDA);  // hold the program till RX buffer is free
    
    return U3RXREG; //receive the value and send it to main function
}
//_____________End of function________________//
 

//**Function to convert string to byte**//
void ESP8266_send_string(char* st_pt)
{
    while(*st_pt) //if there is a char
        _esp8266_putch(*st_pt++); //process it as a byte data
}
//___________End of function______________//








/**
 * Check if the module is started
 *
 * This sends the `AT` command to the ESP and waits until it gets a response.
 *
 * @return true if the module is started, false if something went wrong
 */
bool esp8266_isStarted(void) {
    _esp8266_print("AT\r\n");
    return (_esp8266_waitResponse() == ESP8266_OK);
}

/**
 * Restart the module
 *
 * This sends the `AT+RST` command to the ESP and waits until there is a
 * response.
 *
 * @return true iff the module restarted properly
 */
bool esp8266_restart(void) {
    _esp8266_print("AT+RST\r\n");
    if (_esp8266_waitResponse() != ESP8266_OK) {
        return false;
    }
    return (_esp8266_waitResponse() == ESP8266_READY);
}

/**
 * Enable / disable command echoing.
 *
 * Enabling this is useful for debugging: one could sniff the TX line from the
 * ESP8266 with his computer and thus receive both commands and responses.
 *
 * This sends the ATE command to the ESP module.
 *
 * @param echo whether to enable command echoing or not
 */
void esp8266_echoCmds(bool echo) {
    _esp8266_print("ATE");
    if (echo) {
        _esp8266_putch('1');
    } else {
        _esp8266_putch('0');
    }
    _esp8266_print("\r\n");
    _esp8266_waitFor("OK");
}

/**
 * Set the WiFi mode.
 *
 * ESP8266_STATION : Station mode
 * ESP8266_SOFTAP : Access point mode
 *
 * This sends the AT+CWMODE command to the ESP module.
 *
 * @param mode an ORed bitmask of ESP8266_STATION and ESP8266_SOFTAP
 */
void esp8266_mode(char mode) {
    _esp8266_print("AT+CWMODE=");
    _esp8266_putch(mode + '0');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
}

/**
 * Connect to an access point.
 *
 * This sends the AT+CWJAP command to the ESP module.
 *
 * @param ssid The SSID to connect to
 * @param pass The password of the network
 * @return an ESP status code, normally either ESP8266_OK or ESP8266_FAIL
 */
 char esp8266_connect( char* ssid, char* pass) {
    _esp8266_print("AT+CWJAP=\"");
    _esp8266_print(ssid);
    _esp8266_print("\",\"");
    _esp8266_print(pass);
    _esp8266_print("\"\r\n");
    return _esp8266_waitResponse();
}

/**
 * Disconnect from the access point.
 *
 * This sends the AT+CWQAP command to the ESP module.
 */
void esp8266_disconnect(void) {
    _esp8266_print("AT+CWQAP\r\n");
    _esp8266_waitFor("OK");
}

/**
 * Store the current local IPv4 address.
 *
 * This sends the AT+CIFSR command to the ESP module.
 *
 * The result will not be stored as a string but byte by byte. For example, for
 * the IP 192.168.0.1, the value of store_in will be: {0xc0, 0xa8, 0x00, 0x01}.
 *
 * @param store_in a pointer to an array of the type unsigned char[4]; this
 * array will be filled with the local IP.
 */
void esp8266_ip( char* store_in) {
    _esp8266_print("AT+CIFSR\r\n");
     char received;
    do {
        received = _esp8266_getch();
    } while (received < '0' || received > '9');
    for ( char i = 0; i < 4; i++) {
        store_in[i] = 0;
        do {
            store_in[i] = 10 * store_in[i] + received - '0';
            received = _esp8266_getch();
        } while (received >= '0' && received <= '9');
        received = _esp8266_getch();
    }
    _esp8266_waitFor("OK");
}

/**
 * Open a TCP or UDP connection.
 *
 * This sends the AT+CIPSTART command to the ESP module.
 *
 * @param protocol Either ESP8266_TCP or ESP8266_UDP
 * @param ip The IP or hostname to connect to; as a string
 * @param port The port to connect to
 *
 * @return true iff the connection is opened after this.
 */
bool esp8266_start( char protocol, char* ip, char port) {
    _esp8266_print("AT+CIPSTART=\"");
    if (protocol == ESP8266_TCP) {
        _esp8266_print("TCP");
    } else {
        _esp8266_print("UDP");
    }
    _esp8266_print("\",\"");
    _esp8266_print(ip);
    _esp8266_print("\",");
    char port_str[5] = "\0\0\0\0";
    //sprintf(port_str, "%u", port);
    _esp8266_print(port_str);
    _esp8266_print("\r\n");
    if (_esp8266_waitResponse() != ESP8266_OK) {
        return 0;
    }
    if (_esp8266_waitResponse() != ESP8266_LINKED) {
        return 0;
    }
    return 1;
}

// Send data (AT+CIPSEND)
/**
 * Send data over a connection.
 *
 * This sends the AT+CIPSEND command to the ESP module.
 *
 * @param data The data to send
 *
 * @return true iff the data was sent correctly.
 */
bool esp8266_send( char* data) {
     char length_str[6] = "\0\0\0\0\0";
    //sprintf(length_str, "%u", strlen(data));
    _esp8266_print("AT+CIPSEND=");
    _esp8266_print(length_str);
    _esp8266_print("\r\n");
    while (_esp8266_getch() != '>');
    _esp8266_print(data);
    if (_esp8266_waitResponse() == ESP8266_OK) {
        return 1;
    }
    return 0;
}

/**
 * Read a string of data that is sent to the ESP8266.
 *
 * This waits for a +IPD line from the module. If more bytes than the maximum
 * are received, the remaining bytes will be discarded.
 *
 * @param store_in a pointer to a character array to store the data in
 * @param max_length maximum amount of bytes to read in
 * @param discard_headers if set to true, we will skip until the first \r\n\r\n,
 * for HTTP this means skipping the headers.
 */
void esp8266_receive( char* store_in, uint16_t max_length, bool discard_headers) {
    _esp8266_waitFor("+IPD,");
    uint16_t length = 0;
     char received = _esp8266_getch();
    do {
        length = length * 10 + received - '0';
        received = _esp8266_getch();
    } while (received >= '0' && received <= '9');

    if (discard_headers) {
        length -= _esp8266_waitFor("\r\n\r\n");
    }

    if (length < max_length) {
        max_length = length;
    }

    /*sprintf(store_in, "%u,%u:%c%c", length, max_length, _esp8266_getch(), _esp8266_getch());
    return;*/

    uint16_t i;
    for (i = 0; i < max_length; i++) {
        store_in[i] = _esp8266_getch();
    }
    store_in[i] = 0;
    for (; i < length; i++) {
        _esp8266_getch();
    }
    _esp8266_waitFor("OK");
}

/**
 * Output a string to the ESP module.
 *
 * This is a function for internal use only.
 *
 * @param ptr A pointer to the string to send.
 */
void _esp8266_print(  const char *ptr) {
    while (*ptr != 0) {
        _esp8266_putch(*ptr++);
    }
}

/**
 * Wait until we found a string on the input.
 *
 * Careful: this will read everything until that string (even if it's never
 * found). You may lose important data.
 *
 * @param string
 *
 * @return the number of characters read
 */
inline uint16_t _esp8266_waitFor(char *s) {
    char so_far = 0;
    char received;
    uint16_t counter = 0;
    do {
        received = _esp8266_getch();
        counter++;
        if (received == s[so_far]) {
            so_far++;
        } else {
            so_far = 0;
        }
    } while (s[so_far] != 0);
    return counter;
}

/**
 * Wait until we received the ESP is done and sends its response.
 *
 * This is a function for internal use only.
 *
 * Currently the following responses are implemented:
 *  * OK
 *  * ready
 *  * FAIL
 *  * no change
 *  * Linked
 *  * Unlink
 *
 * Not implemented yet:
 *  * DNS fail (or something like that)
 *
 * @return a constant from esp8266.h describing the status response.
 */
inline  char _esp8266_waitResponse(void) {
    char so_far[6] = {0,0,0,0,0,0};
    const char lengths[6] = {2,5,4,9,6,6};
     const char* strings[6] = {"OK", "ready", "FAIL", "no change", "Linked", "Unlink"};
    const char responses[6] = {ESP8266_OK, ESP8266_READY, ESP8266_FAIL, ESP8266_NOCHANGE, ESP8266_LINKED, ESP8266_UNLINK};
    char received;
    char response;
    bool continue_loop = true;
    while (continue_loop) {
        received = _esp8266_getch();
        for ( char i = 0; i < 6; i++) {
            if (strings[i][so_far[i]] == received) {
                so_far[i]++;
                if (so_far[i] == lengths[i]) {
                    response = responses[i];
                    continue_loop = false;
                }
            } else {
                so_far[i] = 0;
            }
        }
    }
    return response;
}

