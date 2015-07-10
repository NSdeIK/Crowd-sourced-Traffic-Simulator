/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package hu.traffic_analyser.tcpserver;

import hu.traffic_analyser.TrafficAnalyticsClass;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author misi
 */
public class TcpServer {

    public List<Client> clients = new ArrayList<>();
    private final Thread serverThread;

    public TcpServer(int port) throws IOException {
        serverThread = new Thread(new ServerSocketRubbable(port));
        serverThread.start();
    }

    public void closeServerThread() {
        serverThread.interrupt();
    }

    private class ServerSocketRubbable implements Runnable {

        private final ServerSocket serverSocket;

        public ServerSocketRubbable(int port) throws IOException {
            this.serverSocket = new ServerSocket(port);
        }

        @Override
        public void run() {
            while (!Thread.currentThread().isInterrupted()) {
                Socket socket = null;
                try {
                    socket = serverSocket.accept();
                } catch (IOException ex) {
                    Logger.getLogger(TcpServer.class.getName()).log(Level.SEVERE, null, ex);
                }
                if (socket != null) {

                    System.out.println("New Client: " + socket.getInetAddress().getHostAddress());

                    List<Client> del = new ArrayList<>();

                    for (Client c : clients)
                        if (c.socket.getInetAddress().equals(socket.getInetAddress()))
                            del.add(c);

                    del.stream().forEach((d) -> {
                        clients.remove(d);
                        System.out.println("Delete client: " + d.socket.getInetAddress());
                    });

                    clients.add(new Client(socket));
                }
            }
        }
    }

    private class Client {

        public Socket socket;
        private Thread clientThred;
        private PrintWriter out;

        public Client(Socket socket) {
            try {
                this.socket = socket;
                clientThred = new Thread(new clientRunnable(socket.getInputStream()));
                clientThred.start();
                out = new PrintWriter(socket.getOutputStream());
            } catch (IOException ex) {
                Logger.getLogger(TcpServer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        public void SendMess(String text) {
            out.println(text);
        }

        private class clientRunnable implements Runnable {

            DataInputStream din;

            public clientRunnable(InputStream in) {
                this.din =  new DataInputStream(in);
            }

            @Override
            public void run() {

                while (!Thread.currentThread().isInterrupted())
                    try {
                       byte[] temp = new byte[din.available()];
                        din.read(temp);                        
                            if (temp.length > 0) {  
                            TrafficAnalyticsClass.TrafficAnalytics analytics = TrafficAnalyticsClass.TrafficAnalytics.parseFrom(temp);
                            System.out.println("Car id: "+analytics.getCarId());
                            System.out.println("Timestamp: "+analytics.getTimestamp());
                            System.out.println("Latitude: "+analytics.getLatitude()+"'"+analytics.getLatitudens().toString());
                            System.out.println("Longitude: "+analytics.getLongitude()+"'"+analytics.getLongitudeew().toString());
                            System.out.println("Density: "+analytics.getDensity());
                            System.out.println("Speed GPS: "+analytics.getVehicleSpeedGps());
                                                    }                            
                    } catch (IOException ex) {
                        Logger.getLogger(TcpServer.class.getName()).log(Level.SEVERE, null, ex);
                    }
            }
        }
    }
}
