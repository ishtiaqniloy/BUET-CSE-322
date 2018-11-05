package SMTP_Client;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;


public class SMTP_Ideal {

    public static void main(String[] args) throws UnknownHostException, IOException {
        String mailServer = "webmail.buet.ac.bd";
        //String mailServer = "localhost";

        InetAddress mailHost = InetAddress.getByName(mailServer);
        InetAddress localHost = InetAddress.getLocalHost();

        Socket smtpSocket = new Socket(mailHost,25);

        BufferedReader in =  new BufferedReader(new InputStreamReader(smtpSocket.getInputStream()));
        PrintWriter pr = new PrintWriter(smtpSocket.getOutputStream(),true);

        String initialID = in.readLine();
        System.out.println(initialID);

        pr.println("HELO "+localHost.getHostName());
        //System.out.println("HELO "+localHost.getHostName());
        pr.flush();
        String welcome = in.readLine();
        System.out.println(welcome);

        pr.println("MAIL FROM:ishtiaq_niloy@yahoo.com");
        pr.flush();
        welcome = in.readLine();
        System.out.println(welcome);


        pr.println("RCPT TO:ishtiaq.niloy@gmail.com");
        pr.flush();
        welcome = in.readLine();
        System.out.println(welcome);


        pr.println("DATA");
        pr.println("Subject: Sample");
        pr.println("From: ishtiaq_niloy@yahoo.com");
        pr.println("To: ishtiaq.niloy@gmail.com");
        pr.println("");

        pr.println("Greetings,");
        pr.println("Bye");

        pr.println(".");
        pr.flush();

        welcome = in.readLine();
        System.out.println(welcome);

        pr.println("QUIT");
        pr.flush();

        welcome = in.readLine();
        System.out.println(welcome);

        // TODO code application logic here
    }
}
