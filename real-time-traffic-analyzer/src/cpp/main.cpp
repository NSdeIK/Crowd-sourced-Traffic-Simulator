#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#define DEBUG

#include "gps.h"
#include "gsm.h"
#include "video_processing.hpp"
#include "TrafficAnalytics.pb.h"

using namespace std;
using namespace TrafficAnalytics;

struct TAData {
    std::mutex mutex;
    TrafficAnalytics::TrafficAnalytics ta;

    void init(string car_id) {
        mutex.lock();
        ta.set_car_id(car_id);
        mutex.unlock();
    }

    void set_timestamp(const char* timestamp) {
        mutex.lock();
        ta.set_timestamp(timestamp);
        mutex.unlock();
    }

    void set_latitude(double latitude, TrafficAnalytics_LatitudeNS lns) {
        mutex.lock();
        ta.set_latitudens(lns);
        ta.set_latitude(latitude);
        mutex.unlock();
    }

    void set_longitude(double longitude, TrafficAnalytics_longitudeEW lew) {
        mutex.lock();
        ta.set_longitudeew(lew);
        ta.set_longitude(longitude);
        mutex.unlock();
    }

    void set_density(double density) {
        mutex.lock();
        ta.set_density(density);
        mutex.unlock();
    }

    void set_vehicle_speed_gps(uint32_t speed) {
        mutex.lock();
        ta.set_vehicle_speed_gps(speed);
        mutex.unlock();
    }

    void set_vehicle_speed_can(uint32_t speed) {
        mutex.lock();
        ta.set_vehicle_speed_can(speed);
        mutex.unlock();
    }

    void set_fuel_level(uint32_t level) {
        mutex.lock();
        ta.set_fuel_level(level);
        mutex.unlock();
    }

    string get_data() {

        string out;

        mutex.lock();
#ifdef DEBUG
        ta.PrintDebugString();
#endif
        ta.SerializeToString(&out);
        mutex.unlock();

        return out;
    }
};

TAData TrafficData;

void gps_thread() {
    char temp[100];
    for (;;) {
        switch (parse_nmea(read_GPS_frame())) {
            case NMEA_TYPE_GPGGA:
#ifdef DEBUG
                print_GPGAA();
#endif
                TrafficData.set_latitude(gpgaa.latitude, (gpgaa.lns == NORTH) ? TrafficAnalytics_LatitudeNS_NORTH : TrafficAnalytics_LatitudeNS_SOUTH);
                TrafficData.set_longitude(gpgaa.longitude, (gpgaa.lew == EAST) ? TrafficAnalytics_longitudeEW_EAST : TrafficAnalytics_longitudeEW_WEST);
                break;
            case NMEA_TYPE_GPRMC:
#ifdef DEBUG
                print_GPRMC();
#endif
                sprintf(temp, "%d.%d.%d-%d:%d:%d", gprmc.year, gprmc.month, gprmc.day, gprmc.hour, gprmc.min, gprmc.sec);
                TrafficData.set_timestamp(temp);
                TrafficData.set_vehicle_speed_gps(gprmc.speed);
                break;
            default:;
        }
        usleep(100);
    }
}

void gsm_thread() {
    char *temp;
    for (;;) {
        sleep(5);        
	temp = (char*) TrafficData.get_data().data();
        if (gsmSendData(temp) != 0)
            while (gsm_init());
    }
}

void opencv_thread() {
    for (;;) {
        if (capture_frame() == 0) {
            detect(frame);
            TrafficData.set_density(density);
        }
    }
}

int main(int argc, char** argv) {

    if(argc != 2){
        perror("Load cascade file! ./TrafficAnalytics cascade.xml");
    }
       
    //init GPS
    init_gps();

    //init GSM
    gsm_serial_init();

    // init video
    init_video(argv[1]);

    //threads
    thread t_gps(gps_thread);
    thread t_gsm(gsm_thread);
    thread t_opencv(opencv_thread);

    t_gps.join();
    t_gsm.join();
    t_opencv.join();	

    return 0;
}

