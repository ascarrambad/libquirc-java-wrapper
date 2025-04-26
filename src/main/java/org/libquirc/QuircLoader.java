package org.libquirc;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;

public class QuircLoader {

    public static void loadLibrary() throws RuntimeException {
        try {
            // Get OS and Architecture details
            String os = System.getProperty("os.name").toLowerCase();

            // Define resource path based on OS
            String jniLibrary = "/jni/";

            if (os.contains("linux")) {
                jniLibrary += "linux/libquirc.so";
            } else {
                throw new UnsupportedOperationException("Unsupported OS: " + os);
            }

            List<File> tempFiles = new ArrayList<>();
            for (String library : List.of(jniLibrary)) {
                // Load the library as a resource
                InputStream in = QuircLoader.class.getResourceAsStream(library);
                if (in == null) {
                    throw new RuntimeException("Native library not found: " + library);
                }

                // Create a temporary file to extract the native library
                File tempFile = Files.createTempFile(null, null).toFile();
                tempFile.deleteOnExit();

                // Write the resource to the temporary file
                try (FileOutputStream out = new FileOutputStream(tempFile)) {
                    byte[] buffer = new byte[4096];
                    int bytesRead;
                    while ((bytesRead = in.read(buffer)) != -1) {
                        out.write(buffer, 0, bytesRead);
                    }
                }
                tempFiles.add(tempFile);
            }

            // Load the library from the extracted file
            tempFiles.forEach(tmpFile -> System.load(tmpFile.getAbsolutePath()));


        } catch (Exception e) {
            throw new RuntimeException("Failed to load native library", e);
        }
    }

}

