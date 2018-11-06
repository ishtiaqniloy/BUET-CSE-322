package SMTP_Client;

import java.io.*;
import java.net.*;
import java.util.Scanner;
import java.util.concurrent.TimeoutException;


public class SMTP_FileInput1 {

    private static final String mailServer = "webmail.buet.ac.bd";
    private static final int portNum = 25;
    private static final String inputFile = "input2.txt";
    private static final int timeout = 20000;
    private static String state = "CLOSED";


    public static void main(String[] args) {

        try {
            String fileStr = "";
            String replyFromServer = "";

            InetAddress mailHost = InetAddress.getByName(mailServer);
            InetAddress localHost = InetAddress.getLocalHost();

            System.out.println("State : " + state);

            Socket smtpSocket = null;
            BufferedReader in = null;
            PrintWriter pr = null;

            while (state.equalsIgnoreCase("CLOSED")){
                System.out.println("Attempting to connect to mail server :" + mailServer);

                smtpSocket = new Socket();
                smtpSocket.connect(new InetSocketAddress(mailHost.getHostAddress(), portNum), timeout);
                //smtpSocket = new Socket(mailHost,portNum);

                in =  new BufferedReader(new InputStreamReader(smtpSocket.getInputStream()));
                pr = new PrintWriter(smtpSocket.getOutputStream(),true);

                replyFromServer = readWithTimeout(in, timeout);
                replyFromServer = replyFromServer.trim();
                System.out.println("Connection Reply : " + replyFromServer);

                if(replyFromServer.charAt(0)=='2'){
                    state = "BEGIN";
                }

                System.out.println("State : " + state);
            }

            //replyFromServer = readWithTimeout(in, timeout);//for checking timeout

            Scanner sc = new Scanner(new File(".\\" + inputFile));

            while(sc.hasNextLine()){
                System.out.println();
                fileStr = sc.nextLine();
                fileStr = fileStr.trim();
                System.out.println("Input from file : " + fileStr);

                pr.println(fileStr);
                pr.flush();
                replyFromServer="";
                if(!state.equalsIgnoreCase("WRITING MAIL") || fileStr.equalsIgnoreCase(".")){
                    replyFromServer = readWithTimeout(in, timeout);
                    replyFromServer = replyFromServer.trim();

                }




                if(replyFromServer!=null){
                    if(!state.equalsIgnoreCase("WRITING MAIL") || fileStr.equalsIgnoreCase(".")){
                        System.out.println("Server Reply : " + replyFromServer);
                        ///APPROPRIATE MSG

                        if(fileStr.equalsIgnoreCase("QUIT")){
                            state = "CLOSED";
                            smtpSocket.close();
                            return;
                        }

                    }

                    if(state.equalsIgnoreCase("CLOSED")){
                        System.out.println("Not connected to mail server");
                    }
                    else if(state.equalsIgnoreCase("BEGIN")){
                        if (fileStr.startsWith("HELO ")){
                            state= "WAIT";
                        }
                    }
                    else if(state.equalsIgnoreCase("WAIT")){
                        if (fileStr.startsWith("MAIL FROM:") && replyFromServer.charAt(0)=='2'){
                            state= "ENVELOPE CREATED, NO RECIPIENTS";
                        }
                    }
                    else if(state.equalsIgnoreCase("ENVELOPE CREATED, NO RECIPIENTS")){
                        if (fileStr.startsWith("RCPT TO:") && replyFromServer.charAt(0)=='2'){
                            state= "RECIPIENTS SET";
                        }
                        else if (fileStr.equalsIgnoreCase("RSET")){
                            state= "WAIT";
                        }
                    }
                    else if(state.equalsIgnoreCase("RECIPIENTS SET")){
                        if (fileStr.equalsIgnoreCase("DATA") ){ //&& replyFromServer.charAt(0)=='2'
                            state= "WRITING MAIL";
                        }
                        else if (fileStr.startsWith("RSET")){
                            state= "WAIT";
                        }
                    }
                    else if(state.equalsIgnoreCase("WRITING MAIL")){
                        if (fileStr.equalsIgnoreCase(".")){
                            state= "READY TO DELIVER";
                            System.out.println("State : " + state);

                            if(!(replyFromServer.charAt(0)=='2' )){
                                state = "ATTEMPT TO DELIVER";
                            }
                            else{
                                state= "WAIT";
                            }
                        }
                    }
                    else if(state.equalsIgnoreCase("READY TO DELIVER")){
                        if (fileStr.startsWith("MAIL FROM:") && replyFromServer.charAt(0)=='2'){
                            state= "ENVELOPE CREATED, NO RECIPIENTS";
                        }
                    }
                    else if(state.equalsIgnoreCase("ATTEMPT TO DELIVER")){
                        System.out.println("ERROR OCCURRED");
                        //?
                    }


                }
                else {
                    System.out.println("Server Reply : " + "NULL");
                }


                System.out.println("State : " + state);




            }
        }
        catch (SocketTimeoutException ste){
            System.out.println("Socket Connection Timeout  of " + timeout + "milliseconds occurred");
        }
        catch (TimeoutException te){
            System.out.println("Timeout of " + timeout + "milliseconds occurred");
        }
        catch (Exception e){
            e.printStackTrace();
        }


    }

    public static String readWithTimeout(BufferedReader bufferedReader, int timeoutInMili) throws TimeoutException, IOException {
        long maxTimeInMili =   System.currentTimeMillis() + timeoutInMili;
        while (System.currentTimeMillis() < maxTimeInMili){
            if(bufferedReader.ready()){
                String str = bufferedReader.readLine();
                return str;
            }
        }
        throw  new TimeoutException();

    }
}
