/* 
 * File:   gsm.h
 * Author: misi
 *
 * Created on June 23, 2015, 12:48 PM
 */

#ifndef GSM_H
#define	GSM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>

//
#define BUFFER_SIZE 150

// config
#define PIN         3103
#define APN         "internet" //T-Mobile H
#define SERVER      "myserver.hu"
#define PORT        2015

// AT command defines
#define OK          "OK"
#define ERROR       "ERROR"
#define INITIAL     "INITIAL"
#define START       "START"
#define CPIN        "AT+CPIN=%d"
#define CREG        "AT+CREG?"
#define CIPMUX      "AT+CIPMUX=0"
#define CIPSTATUS   "AT+CIPSTATUS"
#define CSTT        "AT+CSTT=\"%s\",\"\",\"\""
#define CIICR       "AT+CIICR"
#define CIFSR       "AT+CIFSR"
#define CIPSTART    "AT+CIPSTART=\"TCP\",\"%s\",\"%d\""
#define CIPSEND     "AT+CIPSEND=%d"

int gsm_fd;

int available();
char readSerial();
int writeSerial(char *text);
int writeLnSerial(char *text);
unsigned long getMilisec();
int sendATComm(char * command, char * answer1, char * answer2, unsigned long timeOut);
int gsm_serial_init();
int gsm_init();
int gsmSendData(char *message);


#ifdef	__cplusplus
}
#endif

#endif	/* GSM_H */

