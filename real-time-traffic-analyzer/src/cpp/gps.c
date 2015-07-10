#include "gps.h"

int init_gps() {

    if ((gps_fd = open("/dev/ttyUL1", O_RDONLY | O_NOCTTY)) == -1) {
        perror("Open GPS serial: Failed to Open!");
        return -1;
    }
    return 0;
}

int parse_nmea(char * data) {
    char *p = strtok(data, ",");
    char temp[10];

    if (strcmp(p, "GPGGA") == 0) {
        //time
        if ((p = strtok(NULL, ",")) != NULL) {
            //hour
            memcpy(temp, p, 2);
            temp[2] = '\0';
            gpgaa.hour = atoi(temp);
            //min
            memcpy(temp, &p[2], 2);
            temp[2] = '\0';
            gpgaa.min = atoi(temp);
            //sec
            memcpy(temp, &p[4], 2);
            temp[2] = '\0';
            gpgaa.sec = atoi(temp);
        }

        //lat
        if ((p = strtok(NULL, ",")) != NULL)
            gpgaa.latitude = atof(p);
        if ((p = strtok(NULL, ",")) != NULL)
            gpgaa.lns = (!strcmp(p, "N")) ? NORTH : SOUTH;

        //long
        if ((p = strtok(NULL, ",")) != NULL)
            gpgaa.longitude = atof(p);
        if ((p = strtok(NULL, ",")) != NULL)
            gpgaa.lew = (!strcmp(p, "E")) ? EAST : WEST;

        //quality
        if ((p = strtok(NULL, ",")) != NULL)
            gpgaa.gps_fix = atoi(p);

        //sat num
        if ((p = strtok(NULL, ",")) != NULL)
            gpgaa.sat_num = atoi(p);
        return NMEA_TYPE_GPGGA;

    } else if (strcmp(p, "GPRMC") == 0) {
        //time
        if ((p = strtok(NULL, ",")) != NULL) {
            //hour
            memcpy(temp, p, 2);
            temp[2] = '\0';
            gprmc.hour = atoi(temp);
            //min
            memcpy(temp, &p[2], 2);
            temp[2] = '\0';
            gprmc.min = atoi(temp);
            //sec
            memcpy(temp, &p[4], 2);
            temp[2] = '\0';
            gprmc.sec = atoi(temp);
        }

        // Status
        if ((p = strtok(NULL, ",")) != NULL)
            gprmc.status = (!strcmp(p, "A")) ? ACTIVE : VOID;

        //lat
        if ((p = strtok(NULL, ",")) != NULL)
            gprmc.latitude = atof(p);
        if ((p = strtok(NULL, ",")) != NULL)
            gprmc.lns = (!strcmp(p, "N")) ? NORTH : SOUTH;

        //long
        if ((p = strtok(NULL, ",")) != NULL)
            gprmc.longitude = atof(p);
        if ((p = strtok(NULL, ",")) != NULL)
            gprmc.lew = (!strcmp(p, "E")) ? EAST : WEST;

        // speed knots -> km/h
        if ((p = strtok(NULL, ",")) != NULL)
            gprmc.speed = atof(p)*1.85200;

        // speed knots -> km/h
        if ((p = strtok(NULL, ",")) != NULL)
            gprmc.track_angle = atof(p);

        //date
        if ((p = strtok(NULL, ",")) != NULL) {
            // day
            memcpy(temp, p, 2);
            temp[2] = '\0';
            gprmc.day = atoi(temp);
            //month
            memcpy(temp, &p[2], 2);
            temp[2] = '\0';
            gprmc.month = atoi(temp);
            //year
            memcpy(temp, &p[4], 2);
            temp[2] = '\0';
            gprmc.year = atoi(temp) + 2000;
        }
        return NMEA_TYPE_GPRMC;
    }
    return 0;
}

void print_GPGAA() {
    printf("%02d:%02d:%02d\r\n", gpgaa.hour, gpgaa.min, gpgaa.sec);
    printf("Lat: %lf, %s\r\n", gpgaa.latitude, NSN[gpgaa.lns]);
    printf("Long: %lf, %s\r\n", gpgaa.longitude, EWN[gpgaa.lew]);
    printf("Quality: %d\r\nSatelites: %d\r\n\n", gpgaa.gps_fix, gpgaa.sat_num);
}

void print_GPRMC() {
    printf("%02d. %02d. %02d. %02d:%02d:%02d\r\n", gprmc.year, gprmc.month, gprmc.day, gprmc.hour, gprmc.min, gprmc.sec);
    printf("Lat: %lf, %s\r\n", gprmc.latitude, NSN[gprmc.lns]);
    printf("Long: %lf, %s\r\n", gprmc.longitude, EWN[gprmc.lew]);
    printf("Track angle: %.2lf\r\n", gprmc.track_angle);
    printf("Speed (Km/h): %.1lf\r\n", gprmc.speed);
}

char read_GPS() {
    char in;
    if (read(gps_fd, &in, sizeof in) < 0) {
        perror("Failed to read GPS serial");
        exit(1);
    }
    return in;
}

char* read_GPS_frame() {
    while (read_GPS() != '$');

    nmea_index = 0;

    while ((nmea_data[nmea_index++] = read_GPS()) != '*');

    nmea_data[nmea_index] = '\0';
    
    return nmea_data;
}