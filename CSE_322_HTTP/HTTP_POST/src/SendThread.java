import java.io.*;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Scanner;

public class SendThread extends Thread {
    private static int bufferSize = 200;

    private Socket clientSocket;
    private int id;
    private String logLine;
    private String userAgent;

    SendThread(Socket sck, int n) {
        clientSocket = sck;
        id = n;
        System.out.println("Thread id = " + id);

        start();

    }

    @Override
    public void run() {
        while (true) {
            logLine = "";
            try {
                InputStream inputStream = clientSocket.getInputStream();
                //Scanner scanner = new Scanner(inputStream);
                InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
                BufferedReader bufferedReader = new BufferedReader(inputStreamReader);

                PrintWriter pr = new PrintWriter(clientSocket.getOutputStream());
                //OutputStreamWriter osw = new OutputStreamWriter(s.getOutputStream(), "UTF-8");

                logLine +=  clientSocket.getRemoteSocketAddress();
                logLine += " -- ";

//                HTTP_POST.logWriter.println(logLine);
//                HTTP_POST.logWriter.flush();

                String filename = "";
                int value = 0;
                String input = bufferedReader.readLine();
                String httpVersion = "";
                String contentType = "";
                System.out.println("Here Input : " + input);
                //            pr.println("nice");
                //            pr.flush();

                String[] tokens = null;
                if (input == null) {
                    System.out.println("input is null"); // this case is not handled
                    //filename = "index.html";
                    return;
                } else {

                    input = input.trim();
                    String[] arrOfinput = input.split("/");
                    tokens = input.split(" ");
                    httpVersion = tokens[2];
                    String[] secondLine = arrOfinput[1].split(" ");
                    filename = secondLine[0];
                    if (secondLine[0].equals("")) {
                        filename = "index.html";
                    }
                }
                filename = filename.replaceAll("%20", " ");
                System.out.println(filename);


                if (tokens[0].equalsIgnoreCase("GET")) {     ///implementing get method

                    String st = bufferedReader.readLine();
                    while (!st.equalsIgnoreCase("")) {
                        if(st.startsWith("User-Agent: ")){
                            userAgent = st.replaceAll("User-Agent: ", "");

                        }
                        System.out.println(st);
                        st = bufferedReader.readLine();
                    }
                    System.out.println("OUT OF LOOP");

                    File targetFile = new File(".\\" + filename);

                    String[] tokens2 = filename.split("\\.");
                    int targetNum = tokens2.length - 1;
                    System.out.println(targetNum);
                    if (tokens2[targetNum].equalsIgnoreCase("html"))
                        contentType = "text/html";
                    else if (tokens2[targetNum].equalsIgnoreCase("pdf"))
                        contentType = "application/pdf";
                    else if (tokens2[targetNum].equalsIgnoreCase("jpg"))
                        contentType = "image/jpg";
                    else if (tokens2[targetNum].equalsIgnoreCase("jpeg"))
                        contentType = "image/jpeg";
                    else if (tokens2[targetNum].equalsIgnoreCase("ttf"))
                        contentType = "image/ttf";
                    else if (tokens2[targetNum].equalsIgnoreCase("png"))
                        contentType = "image/png";
                    else if (tokens2[targetNum].equalsIgnoreCase("gif"))
                        contentType = "image/gif";

                    //String lineSeparator = System.getProperty("line.separator");

                    SimpleDateFormat sdf = new SimpleDateFormat("E, dd MMM yyyy HH:mm:ss z");
                    String date = sdf.format(new Date());
                    System.out.println(date);

                    logLine +=date;
                    logLine += " ";

                    logLine += input;
                    logLine += " ";

                    if (targetFile.exists()) {
                        //Scanner scanner = new Scanner(targetFile);
                        //System.out.println("Yahoo!!");

                        logLine += "200";
                        logLine += " ";

                        logLine += targetFile.length();
                        logLine +=" ";

                        logLine += userAgent;
                        logLine += " ";

                                String headers = httpVersion + " 200 OK\n" +
                                "MIME-Version: 1.0\n" +
                                "Date: " + date + "\n" +
                                "Server: HahaSofolota\n" +
                                "Content-Type: " + contentType + "\n" +
                                "Content-Length: " + targetFile.length() + "\n";
                        if (contentType.compareToIgnoreCase("text/html") < 0) {
                            //System.out.println("Working from here");
                            headers += "Content-disposition: inline; filename=" + filename + "\r\n";
                        }
                        System.out.println(headers);
                        pr.println(headers);
                        pr.flush();



                        BufferedReader inFromFile = new BufferedReader(new InputStreamReader(new FileInputStream(targetFile)));

                        BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(clientSocket.getOutputStream(), bufferSize);
                        DataOutputStream dataOutputStream = new DataOutputStream(bufferedOutputStream);

                        byte[] byteArr = new byte[bufferSize + 10];

                        FileInputStream fileInputStream = new FileInputStream(targetFile);

                        for (int i = 0; i * bufferSize < targetFile.length(); i++) {

                            int done = fileInputStream.read(byteArr, 0, bufferSize);
                            //System.out.println(i + " " + done);
                            if (done >= 0) dataOutputStream.write(byteArr, 0, done);
                            dataOutputStream.flush();
                        }
                        System.out.println("File Sent");

                    } else {

                        logLine += "404";
                        logLine += " ";

                        logLine += userAgent;
                        logLine += " ";

                        String headers = httpVersion + " 404 Not Found" +
                                "MIME-Version: 1.0\n" +
                                "Server: HahaSofolota\n" +
                                "Keep-Alive: timeout=15, max=100\n" +
                                "Connection: Keep-Alive\n" +
                                "Content-Type: text/html\n" +
                                "\r\n";

                        System.out.println(headers);
                        String errorMSG = "<html> <head> <title> 404: NOT FOUND </title> </head>" +
                                "<body> <h1> <center> 404 : File Not Found  </center> </h1>  </body> </html> \n";

                        pr.println(headers);
                        pr.println(errorMSG);
                        pr.flush();

                    }
                } else if (tokens[0].equalsIgnoreCase("POST")) { ///implementing POST method



                    //char []msg = new char[10000];
                    String msg = "";
                    String temp = "";
                    char[] tempbuff = new char[1000];

                    int charsize = bufferedReader.read(tempbuff);

                    msg = new String(tempbuff);
                    System.out.println("");

                    //System.out.println(msg);
                    String []lines = msg.split("\n");



                    String intended_line = lines[lines.length-1];
                    System.out.println(intended_line);
                    File targetFile = new File(".\\" + filename);

                    SimpleDateFormat sdf = new SimpleDateFormat("E, dd MMM yyyy HH:mm:ss z");
                    String date = sdf.format(new Date());
                    System.out.println(date);

                    logLine +=date;
                    logLine += " ";

                    logLine += input;
                    logLine += " ";

                    logLine += "200";
                    logLine += " ";

                    if(targetFile.exists()){
                        Scanner scanner = new Scanner(targetFile);


                        long length = targetFile.length() + 200;

                        logLine += targetFile.length();
                        logLine += " ";

                        for(String str : lines){
                            if(str.startsWith("User-Agent: ")){
                                userAgent = str.replaceAll("User-Agent: ", "");

                            }
                        }

                        logLine += userAgent;
                        logLine += " ";

                        String headers = httpVersion + " 200 OK\n" +
                                "MIME-Version: 1.0\n" +
                                "Date: " + date + "\n" +
                                "Server: HahaSofolota\n" +
                                "Content-Type: " + contentType + "\n" +
                                "Content-Length: " + length + "\n";


                        String Post_File = "";


                        while(scanner.hasNextLine()){

                            String str = scanner.nextLine();

                            if(str.contains("HTTP REQUEST TYPE->")){
                                //System.out.println("OK Work-1");
                               //str += "POST";
                               str = str.replaceAll("<h2> HTTP REQUEST TYPE-> </h2>","<h2> HTTP REQUEST TYPE-> POST </h2>");
                            }
                            if(str.contains("Post->")){
                                //str += intended_line;

                                intended_line = intended_line.trim();
                                intended_line = intended_line.replace("user=", "");

                                str = str.replaceAll("<h2> Post-> </h2>", "<h2> Post-> " + intended_line + " </h2>" );

                            }
                            str += "\n";
                            Post_File += str;
                            //pr.println(str);
                            //pr.flush();
                            //Post_File += "\n";
                            //System.out.println(str);



                        }
                        //the form portion isn't getting displayed for unknown reason...

                        pr.println(headers);
                        System.out.println(Post_File);
                        pr.println(Post_File);
                        pr.flush();
                    }
                    else {
                        System.out.println("File not found? :/ ");
                    }





                }

                HTTP_POST.logWriter.println(logLine);
                HTTP_POST.logWriter.flush();

            } catch (Exception e) {
                e.printStackTrace();
            }

        }
    }
}
