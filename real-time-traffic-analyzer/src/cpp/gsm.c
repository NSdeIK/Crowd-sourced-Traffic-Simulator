#include "gsm.h"

int gsm_serial_init() {
    if ((gsm_fd = open("/dev/ttyUL2", O_RDWR | O_NOCTTY)) == -1) {
        perror("Open GSM serial: Failed to Open!");
        return -1;
    }
    return 0;
}

int gsm_init() {
    char temp[BUFFER_SIZE];

    sprintf(temp, CPIN, PIN);

    if (sendATComm(temp, OK, ERROR, 3000) == 1) {

        while (sendATComm(CREG, "+CREG: 0,1", "+CREG: 0,5", 1000) == 0);

        if (sendATComm(CIPMUX, OK, ERROR, 1000) == 1) {//single connection

            while (sendATComm(CIPSTATUS, INITIAL, "", 500) == 0);
            sleep(5);

            sprintf(temp, CSTT, APN);
            if (sendATComm(temp, OK, ERROR, 30000) == 1) {

                while (sendATComm(CIPSTATUS, START, "", 500) == 0);
                sleep(5);

                if (sendATComm(CIICR, OK, ERROR, 30000) == 1) {

                    while (sendATComm(CIPSTATUS, START, "", 500) == 0);
                    sleep(5);

                    if (sendATComm(CIFSR, ".", ERROR, 10000) == 1) {

                        while (sendATComm(CIPSTATUS, START, "", 500) == 0);
                        sleep(5);

                        sprintf(temp, CIPSTART, SERVER, PORT);
                        if (sendATComm(temp, "CONNECT OK", "CONNECT FAIL", 30000) == 1) {
                            printf("Connected\n");
                            return 1;
                        }

                    }
                }
            }
        }
    }
    return 0;
}

int gsmSendData(char *message) {

    char temp[BUFFER_SIZE];

    sprintf(temp, CIPSEND, strlen(message));

    if (sendATComm(temp, ">", ERROR, 10000) == 1)
        return sendATComm(message, "SEND OK", ERROR, 10000);

    return 0;
}

int sendATComm(char * command, char * answer1, char * answer2, unsigned long timeOut) {

    unsigned long start; // start time

    char buffer[BUFFER_SIZE]; // buffer

    int buffer_index = 0;

    memset(buffer, '\0', BUFFER_SIZE); //clear buffer

    //clear input stream
    while (available() > 0) readSerial();

    writeLnSerial(command);
#ifdef DEBUG
    printf("%s\n", command);
#endif
    start = getMilisec(); // get start time

    // read while time limit expires
    while ((getMilisec() - start) < timeOut) {
        if (available() > 0) {

            buffer[buffer_index++] = readSerial();

#ifdef DEBUG
            printf("%c", buffer[buffer_index - 1]);
#endif
            if (buffer_index > BUFFER_SIZE)
                buffer_index = 0;

            if (strstr(buffer, answer1) != NULL)
                return 1;
            else if (strstr(buffer, answer2) != NULL)
                return 2;
        }
    }
    return 0;

}

unsigned long getMilisec() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (t.tv_sec + (t.tv_usec / 1000000))*1000;
}

int writeSerial(char *text) {
    return write(gsm_fd, text, strlen(text)); // send 
}

int writeLnSerial(char *text) {
    char rn[] = "\r\n"; // CR+LF
    char temp[strlen(text) + strlen(rn) + 1]; // text+CR+LF
    strcpy(temp, text);
    strcat(temp, rn);
    return write(gsm_fd, temp, strlen(temp)); // send 
}

char readSerial() {
    char c;
    read(gsm_fd, &c, sizeof c);
    return c;
}

int available() {
    int n = 0;
    if (ioctl(gsm_fd, FIONREAD, &n) < 0) {
        perror("Failed to get byte count on serial.\n");
        exit(-1);
    }
    return n;
}
