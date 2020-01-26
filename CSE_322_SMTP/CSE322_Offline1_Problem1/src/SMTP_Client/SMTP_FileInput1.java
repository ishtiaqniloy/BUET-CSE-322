package SMTP_Client;

import java.io.*;
import java.net.*;
import java.util.Scanner;
import java.util.concurrent.TimeoutException;


public class SMTP_FileInput1 {

    private static final String mailServer = "webmail.buet.ac.bd";
    private static final int portNum = 25;
    private static final String inputFile = "message.txt";
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
                System.out.println("Message from server : " + getAppropriateMessage(replyFromServer));

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
                        System.out.println("Message from server : " + getAppropriateMessage(replyFromServer));

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
        catch (UnknownHostException uhe){
            System.out.println("Unknown Host Exception Occurred. Cannot find mail server.\nTerminating...");
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

    static String getAppropriateMessage(String replyFromServer){
        String codeMsg;
        if(replyFromServer.startsWith("211")){
            codeMsg = "211 System status, or system help reply.";
        }
        else if(replyFromServer.startsWith("214")){
            codeMsg = "214 Help message.";
        }
        else if(replyFromServer.startsWith("220")){
            codeMsg = "220 " + mailServer + " Service ready.\n";
        }
        else if(replyFromServer.startsWith("221")){
            codeMsg = "221 " + mailServer + " Service closing transmission channel.\n";
        }
        else if(replyFromServer.startsWith("250")){
            codeMsg = "250 Requested mail action okay, completed.";
        }
        else if(replyFromServer.startsWith("251")){
            codeMsg = "251 User not local; will forward to <forward-path>.";
        }
        else if(replyFromServer.startsWith("354")){
            codeMsg = "354 Start mail input; end with <CRLF>.<CRLF>.";
        }
        else if(replyFromServer.startsWith("421")){
            codeMsg = "421" + mailServer + " Service not available, closing transmission channel. [This may be a reply to any command if the service knows it must shut down].";
        }
        else if(replyFromServer.startsWith("450")){
            codeMsg = "450 Requested mail action not taken: mailbox unavailable.";
        }
        else if(replyFromServer.startsWith("451")){
            codeMsg = "451 Requested action aborted: local error in processing.";
        }
        else if(replyFromServer.startsWith("452")){
            codeMsg = "452 Requested action not taken: insufficient system storage.";
        }
        else if(replyFromServer.startsWith("500")){
            codeMsg = "500 Syntax error, command unrecognized.  [This may include errors such as command line too long]";
        }
        else if(replyFromServer.startsWith("501")){
            codeMsg = "501 Syntax error in parameters or arguments.";
        }
        else if(replyFromServer.startsWith("502")){
            codeMsg = "502 Command not implemented.";
        }
        else if(replyFromServer.startsWith("503")){
            codeMsg = "503 Bad sequence of commands.";
        }
        else if(replyFromServer.startsWith("504")){
            codeMsg = "504 Command parameter not implemented.";
        }
        else if(replyFromServer.startsWith("550")){
            codeMsg = "550 Requested action not taken: mailbox unavailable.";
        }
        else if(replyFromServer.startsWith("551")){
            codeMsg = "551 User not local; please try <forward-path>.";
        }
        else if(replyFromServer.startsWith("552")){
            codeMsg = "552 Requested mail action aborted: exceeded storage allocation.";
        }
        else if(replyFromServer.startsWith("553")){
            codeMsg = "553 Requested action not taken: mailbox name not allowed. [E.g., mailbox syntax incorrect]";
        }
        else if(replyFromServer.startsWith("554")){
            codeMsg = "554 Transaction failed.";
        }
        else {
            codeMsg = "UNKNOWN REPLY FROM SERVER";
        }

        return codeMsg;
    }
}
//Attachment needs to be handled tomorrow morning.
