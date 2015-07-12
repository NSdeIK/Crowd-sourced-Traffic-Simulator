/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package hu.deik.rtta.rttaosmmqttcli;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

/**
 *
 * @author misi
 */
class OSMGetData {

    private String postcode;
    private String city;
    private String road;

    public OSMGetData() {
    }

    String getAddrJson(double lat, double lon) throws IOException {

        String baseUrl = "http://nominatim.openstreetmap.org/reverse?format=json&lat=" + String.valueOf(lat) + "&lon=" + String.valueOf(lon) + "&zoom=18&addressdetails=1";
        HttpURLConnection httpURLConnection = null;
        try {
            httpURLConnection = (HttpURLConnection) new URL(baseUrl).openConnection();
        } catch (MalformedURLException ex) {
            Logger.getLogger(OSMGetData.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(OSMGetData.class.getName()).log(Level.SEVERE, null, ex);
        }
        try {
            httpURLConnection.setRequestMethod("POST");
        } catch (ProtocolException ex) {
            Logger.getLogger(OSMGetData.class.getName()).log(Level.SEVERE, null, ex);
        }
        httpURLConnection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
        BufferedReader bufferedReader = null;
        try {
            bufferedReader = new BufferedReader(new InputStreamReader(httpURLConnection.getInputStream()));
        } catch (IOException ex) {
            Logger.getLogger(OSMGetData.class.getName()).log(Level.SEVERE, null, ex);
        }

        return bufferedReader.readLine();
    }

    String getAddr(String jsonStr) {
        JSONParser jsonParser = new JSONParser();
        JSONObject root = null;
        try {
            root = (JSONObject) jsonParser.parse(jsonStr);
        } catch (ParseException ex) {
            Logger.getLogger(OSMGetData.class.getName()).log(Level.SEVERE, null, ex);
        }
        try {
            JSONObject address = (JSONObject) root.get("address");

            postcode = address.get("postcode").toString();
            city = address.get("city").toString();
            road = address.get("road").toString();

            return road;
        } catch (Exception ex) {
           // Logger.getLogger(OSMGetData.class.getName()).log(Level.SEVERE, null, ex);
        }
        return null;
    }

    public String getCity() {
        return city;
    }

    public String getPostcode() {
        return postcode;
    }

    public String getRoad() {
        return road;
    }

}
