package SMTP_Client;

import java.io.*;
import java.net.*;
import java.util.Scanner;
import java.util.concurrent.TimeoutException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Base64;

public class Base64Implementation {
    File originalFile = new File("signature.jpg");
    String encodedBase64 = null;

    String encode() {
        try {
            FileInputStream fileInputStreamReader = new FileInputStream(originalFile);
            byte[] bytes = new byte[(int) originalFile.length()];
            fileInputStreamReader.read(bytes);
            encodedBase64 = Base64.getEncoder().encodeToString(bytes);
            return encodedBase64;

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException etc) {
            etc.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

}





