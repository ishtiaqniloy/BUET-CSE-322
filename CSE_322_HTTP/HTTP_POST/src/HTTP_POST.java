import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Date;

public class HTTP_POST {
    static final int PORT = 6789;
    private static int threadNo = 0;
    public static PrintWriter logWriter = null;

    public static void main(String[] args) throws IOException {

        ServerSocket serverConnect = new ServerSocket(PORT);
        System.out.println("Server started.\nListening for connections on port : " + PORT + " ...\n");
        File logFile = new File(".\\logfile.txt");
        if(!logFile.exists()){
            logFile.createNewFile();
        }

        logWriter = new PrintWriter(new FileOutputStream(logFile, true), true);

        SimpleDateFormat sdf = new SimpleDateFormat("E, dd MMM yyyy HH:mm:ss z");
        String date = sdf.format(new Date());
        logWriter.println("***New Session started at " + date + "***");
        logWriter.flush();

        while(true)
        {
            Socket s=serverConnect.accept();
            SendThread newThread = new SendThread(s, threadNo);
            threadNo++;


        }

    }
}
