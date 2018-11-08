package SMTP_Client;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Base64;

public class Base64Implementation {
    File originalFile = new File("signature.jpg");
    String encodedBase64 = null;
    try

    {
        FileInputStream fileInputStreamReader = new FileInputStream(originalFile);
        byte[] bytes = new byte[(int)originalFile.length()];
        fileInputStreamReader.read(bytes);
        encodedBase64 = new String(Base64.encodeBase64(bytes));
    }catch(FileNotFoundException e){
        e.printStackTrace();
    }catch(IOException etc){
        etc.printStackTrace();
    }
}
