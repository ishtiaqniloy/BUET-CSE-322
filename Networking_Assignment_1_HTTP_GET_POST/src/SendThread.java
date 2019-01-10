import java.io.*;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Scanner;

public class SendThread extends Thread {
    private  static int bufferSize = 200;

    private Socket clientSocket;
    private int id;

    SendThread(Socket sck, int n){
        clientSocket = sck;
        id = n;
        System.out.println("Thread id = " + id);

        start();

    }

    @Override
    public void run() {
        while (true){
            try {
                BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                PrintWriter pr = new PrintWriter(clientSocket.getOutputStream());
                //OutputStreamWriter osw = new OutputStreamWriter(s.getOutputStream(), "UTF-8");
                String filename = "";
                String input = in.readLine();
                String httpVersion ="";
                String contentType = "";
                System.out.println("Here Input : "+input);
            //            pr.println("nice");
            //            pr.flush();
                if(input == null){
                    System.out.println("input is null"); // this case is not handled
                    //filename = "index.html";
                    return;
                }
                else{
                    input =  input.trim();
                    String []arrOfinput =  input.split("/");
                    String []tokens = input.split(" ");
                    httpVersion = tokens[2];
                    String []secondLine = arrOfinput[1].split(" ");
                    filename = secondLine[0];
                    if(secondLine[0].equals("")){
                        filename = "index.html";
                    }

                }
                filename = filename.replaceAll("%20", " ");
                System.out.println(filename);

                File targetFile = new File(".\\" + filename);

                String []tokens = filename.split("\\.");
            //            for(String st:tokens){
            //                System.out.println("Working");
            //                System.out.println(st);
            //            }
                int targetNum = tokens.length-1;
                System.out.println(targetNum);
                if(tokens[targetNum].equalsIgnoreCase("html") )
                    contentType = "text/html";
                else if(tokens[targetNum].equalsIgnoreCase("pdf") )
                    contentType = "application/pdf";
                else if(tokens[targetNum].equalsIgnoreCase("jpg") )
                    contentType = "image/jpg";
                else if(tokens[targetNum].equalsIgnoreCase("jpeg") )
                    contentType = "image/jpeg";
                else if(tokens[targetNum].equalsIgnoreCase("ttf") )
                    contentType = "image/ttf";
                else if(tokens[targetNum].equalsIgnoreCase("png") )
                    contentType = "image/png";
                else if(tokens[targetNum].equalsIgnoreCase("gif") )
                    contentType = "image/gif";

                //String lineSeparator = System.getProperty("line.separator");
                if(targetFile.exists()){
                    //Scanner scanner = new Scanner(targetFile);
                    System.out.println("Yahoo!!");
                    SimpleDateFormat sdf = new SimpleDateFormat("E, dd MMM yyyy HH:mm:ss z");
                    String date =  sdf.format(targetFile.lastModified());
                    System.out.println(date);

                    String headers = httpVersion +  " 200 OK\n" +
                            "MIME-Version: 1.0\n" +
                            "Date: " + date + "\n" +
                            "Server: HahaSofolota\n" +
                            "Content-Type: " +contentType + "\n" +
                            "Content-Length: " +targetFile.length() + "\n";
                    if(contentType.compareToIgnoreCase("text/html") < 0){
                        //System.out.println("Working from here");
                        headers += "Content-disposition: inline; filename=" + filename + "\r\n";
                    }
                    System.out.println(headers);
                    pr.println(headers);
                    pr.flush();

                    BufferedReader inFromFile = new BufferedReader(new InputStreamReader(new FileInputStream(targetFile)));

                    BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(clientSocket.getOutputStream(), bufferSize);
                    DataOutputStream dataOutputStream = new DataOutputStream(bufferedOutputStream);

                    byte[] byteArr = new byte[bufferSize+10];

                    FileInputStream fileInputStream = new FileInputStream(targetFile);

                    for (int i=0; i*bufferSize<targetFile.length();i++){

                        int done = fileInputStream.read(byteArr, 0, bufferSize);
                        System.out.println(i + " " + done);
                        if(done>=0)dataOutputStream.write(byteArr, 0, done);
                        dataOutputStream.flush();
                    }
                    System.out.println("File Sent");





            //                while(scanner.hasNextLine()){
            //
            //
            //                    String str = scanner.nextLine();
            //                    System.out.println(str);
            //                    pr.println(str);
            //                    pr.flush();
            //                }

            //                System.out.println("Connection closed by foreign host.");
            //                pr.println("Connection closed by foreign host.");
            //                pr.flush();


                    //clientSocket.close();
                }
                else{
                    String headers = httpVersion + " 404 Not Found" +
                            "MIME-Version: 1.0\n" +
                            "Server: HahaSofolota\n" +
                            "Keep-Alive: timeout=15, max=100\n" +
                            "Connection: Keep-Alive\n"+
                            "Content-Type: text/html\n"+
                            "\r\n";

                    System.out.println(headers);
                    String errorMSG = "<html> <head> <title> 404: NOT FOUND </title> </head>"+
                            "<body> <h1> <center> 404 : File Not Found  </center> </h1>  </body> </html> \n";

                    pr.println(headers);
                    pr.println(errorMSG);
                    pr.flush();

                }
        }
        catch (Exception e){
            e.printStackTrace();
        }
}

    }
}
