/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package hu.deik.rtta.rttaosmmqttcli;

import com.google.protobuf.InvalidProtocolBufferException;
import hu.traffic_analyser.TrafficAnalyticsClass;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

/**
 *
 * @author misi
 */
class RTTAMQTT {

    //MQTT conf
    private final String topic = "traffic/Debrecen";
    private final int qos = 1;
    private final String broker = "tcp://test.mosquitto.org:1883";
    private final String clientId = "RTTAJavaClient";
    private final MemoryPersistence persistence = new MemoryPersistence();
    private MqttClient mqttClient;

    //OSM+TRACK
    private final OSMGetData oSMGetData = new OSMGetData();
    private String lastRoad;
    private int actDensity;

    //output
    private final String outputFileName = "rtta.txt";
    private BufferedWriter out;

    public RTTAMQTT() {

        lastRoad = "";
        actDensity = 0;

        try {
            out = new BufferedWriter(new FileWriter(new File(outputFileName)));
        } catch (IOException ex) {
            Logger.getLogger(RTTAMQTT.class.getName()).log(Level.SEVERE, null, ex);
        }

        try {
            mqttClient = new MqttClient(broker, clientId, persistence);

            MqttConnectOptions connOpts = new MqttConnectOptions();

            connOpts.setCleanSession(true);

            mqttClient.connect(connOpts);

            System.out.println("MQTT Connected.");

            mqttClient.setCallback(mqttCallback);

            mqttClient.subscribe(topic, qos);

        } catch (MqttException ex) {
            Logger.getLogger(RTTAMQTT.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    private MqttCallback mqttCallback = new MqttCallback() {

        @Override
        public void connectionLost(Throwable thrwbl) {
        }

        @Override
        public void messageArrived(String string, MqttMessage mm) throws Exception {

            TrafficAnalyticsClass.TrafficAnalytics analytics = null;
            try {
                analytics = TrafficAnalyticsClass.TrafficAnalytics.parseFrom(mm.getPayload());
                /* debug
                 System.out.println("Car id: " + analytics.getCarId());
                 System.out.println("Timestamp: " + analytics.getTimestamp());
                 System.out.println("Latitude: " + analytics.getLatitude() + "'" + analytics.getLatitudens().toString());
                 System.out.println("Longitude: " + analytics.getLongitude() + "'" + analytics.getLongitudeew().toString());
                 System.out.println("Density: " + analytics.getDensity());
                 System.out.println("Speed GPS: " + analytics.getVehicleSpeedGps());*/
            } catch (InvalidProtocolBufferException ex) {
                Logger.getLogger(RTTAMQTT.class.getName()).log(Level.SEVERE, null, ex);
            }

            if (analytics != null) {

                String actRoad = oSMGetData.getAddr(oSMGetData.getAddrJson(analytics.getLatitude(), analytics.getLongitude()));
                
                if (actRoad != null) {
                    if (lastRoad.isEmpty()) {// first road
                        lastRoad = actRoad;
                        actDensity =  (int) analytics.getDensity();
                       
                    } else if (lastRoad.equals(actRoad)) {// act road same last road 
                        actDensity += (int) analytics.getDensity();// count all car in the act road
                      //  System.out.println(lastRoad + " " + actDensity);                        
                    } else {// new road

                        System.out.println(lastRoad + " " + actDensity);                       

                        out.write(lastRoad + " " + actDensity);
                        out.newLine();
                        out.flush();

                        actDensity = (int) analytics.getDensity();
                        lastRoad = actRoad; // change road
                    }
                }
            }
        }

        @Override
        public void deliveryComplete(IMqttDeliveryToken imdt) {
        }
    };
}
