package SMTP_Client;

import java.io.*;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;


public class SMTP_FileInput2 {

    private static final String mailServer = "webmail.buet.ac.bd";
    private static final int portNum = 25;
    private static final String inputFile = "input1.txt";
    private static String state = "BEGIN";

    public static void main(String[] args) {

        try {
            String fileStr = "";
            String replyFromServer = "";

            InetAddress mailHost = InetAddress.getByName(mailServer);
            InetAddress localHost = InetAddress.getLocalHost();

            System.out.println("State : " + state);

            Socket smtpSocket = new Socket(mailHost,portNum);

            BufferedReader in =  new BufferedReader(new InputStreamReader(smtpSocket.getInputStream()));
            PrintWriter pr = new PrintWriter(smtpSocket.getOutputStream(),true);

            replyFromServer = in.readLine();
            System.out.println("Connection Reply : " + replyFromServer);

            state = "WAIT";
            System.out.println("State : " + state);

            Scanner sc = new Scanner(new File(".\\" + inputFile));

            while(sc.hasNextLine()){
                System.out.println();
                fileStr = sc.nextLine();
                System.out.println("Input from file : " + fileStr);

                pr.println(fileStr);
                pr.flush();

                replyFromServer = in.readLine();


                if(replyFromServer!=null){
                    System.out.println("Server Reply : " + replyFromServer);
                    switch (replyFromServer.charAt(0)){
                        case '1' :
                        case '3' :  state = "ERROR";
                            break;
                        case '2' :  state = "SUCCESS";
                            break;
                        case '4' :
                        case '5' :  state = "FAILURE";
                            break;
                        default :   state = "UNKNOWN";
                    }
                }
                else {
                    System.out.println("Server Reply : " + "NULL");
                    state = "UNKNOWN";
                }


                System.out.println("State : " + state);




            }
        }
        catch (Exception e){
            e.printStackTrace();
        }


    }
}
