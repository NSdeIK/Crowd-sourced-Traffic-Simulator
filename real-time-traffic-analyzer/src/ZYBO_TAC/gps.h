#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <math.h>

#ifndef GPS_H
#define	GPS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define NMEA_TYPE_GPGGA 1
#define NMEA_TYPE_GPRMC 2
    
// gps file descriptor
int gps_fd;

// buffer data
static char nmea_data[100];

// buffer index
int nmea_index;

// get variable name ;)
#define VName(x) #x

// GPS pos NORTH, SOUTH
enum NS {
    NORTH, SOUTH
};

// GPS pos EAST, WEST
enum EW {
    EAST, WEST
};

// GPS pos stat ACTIVE, VOID
enum RMCStatus {
    ACTIVE, VOID
};

// enum variable name start
static const char* NSN [] = {
    VName(NORTH),
    VName(SOUTH),
};

static const char* EWN [] = {
    VName(EAST),
    VName(WEST),
};

static const char* RMCS [] = {
    VName(ACTIVE),
    VName(VOID)
};
// enum variable name end

// GPGAA struct
typedef struct GPGAA_ {
    unsigned char hour, min, sec;
    enum NS lns;
    double latitude;
    enum EW lew;
    double longitude;
    unsigned char gps_fix, sat_num;
} GPGAA;

// GPGAA variable
GPGAA gpgaa;

// GPRMC struct
typedef struct GPRMC_ {
    unsigned char month, day, hour, min, sec;
    int year;
    enum RMCStatus status;
    enum NS lns;
    double latitude;
    enum EW lew;
    double longitude;
    double speed;
    double track_angle;
} GPRMC;

// GPRMC variable
GPRMC gprmc;

// inicialize GPS
int init_gps();

// parse NMEA string
int parse_nmea(char * data);

// print GPGAA data
void print_GPGAA();

// print GPRMC data
void print_GPRMC();

// read GPS serial char by char
char read_GPS();

// read GPS full frame (GPGAA/GPRMC/...)
char* read_GPS_frame();

#ifdef	__cplusplus
}
#endif

#endif	/* GPS_H */

