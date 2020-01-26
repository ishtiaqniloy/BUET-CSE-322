
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;


public class HTTP_TRY {

    static final int PORT = 6789;
    private static int threadNo = 0;

    public static void main(String[] args) throws IOException {

        ServerSocket serverConnect = new ServerSocket(PORT);
        System.out.println("Server started.\nListening for connections on port : " + PORT + " ...\n");
        while(true)
        {
            Socket s=serverConnect.accept();
            SendThread newThread = new SendThread(s, threadNo);
            threadNo++;


        }

    }
}
