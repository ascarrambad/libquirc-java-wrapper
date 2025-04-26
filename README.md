# libquirc‑java‑wrapper

> A lightweight **JNI** wrapper around Daniel Beer’s [quirc](https://github.com/dlbeer/quirc) C library that lets you **decode QR codes in pure Java** (no external processes) while shipping the native code you need.

---

[![Build with Maven](https://img.shields.io/badge/build-maven-blue.svg)](https://maven.apache.org/)  [![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

## Why use this project?

* **Zero runtime dependencies** – the native `libquirc` is bundled and extracted at startup.
* **Simple API** – pass a `BufferedImage` (or raw RGB data) and get all decoded strings back.
* **Fast** – quirc is one of the quickest open‑source QR scanners.
* **Permissive licence** – MIT for Java/JNI code, original quirc is ISC.

## Getting started

### 1. Add the dependency

```xml
<dependency>
    <groupId>org.libquirc</groupId>
    <artifactId>libquirc-wrapper</artifactId>
    <version>1.2-a9c52e79283d6872728e68fec7892dc7c276c827</version>
</dependency>
```

> **Note**: pre‑built binaries are included **for Linux x86_64**.  For other platforms you’ll need to [build from source](#build-from-source).

### 2. Decode your first QR code

```java
import org.libquirc.*;
import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;

public class Demo {
    public static void main(String[] args) throws Exception {
        // 1. Load the native library once
        QuircLoader.loadLibrary();

        // 2. Read an image and copy pixel data
        BufferedImage img = ImageIO.read(new File("qr.png"));
        int[] pixels = img.getRGB(0, 0, img.getWidth(), img.getHeight(), null, 0, img.getWidth());

        // 3. Wrap and decode
        QuircImage qimg = new QuircImage(img.getWidth(), img.getHeight(), pixels);
        String[] results = qimg.decode();

        for (String s : results) System.out.println("Decoded: " + s);
    }
}
```

## Build from source

*Requires JDK 8+ and a recent GCC/Clang toolchain on Linux.*  The project uses Maven; the steps below are **required** to compile C++ objects and JNI.

### Required manual build step

1. Open shell
2. Execute `cd projroot/src/main/resources/jni/linux`
3. Compile JNI
   - `g++ -c -fPIC -I/path/to/jvm/include/ -I/path/to/jvm/include/linux org_libquirc_QuircImage.cpp -o ../../../c/org_libquirc_QuircImage.o -I../../native/linux`
   - `g++ -shared -fPIC -o libquirc.so ../../../c/org_libquirc_QuircImage.o ../../native/linux/libquirc.a -lc`
4. Execute Maven `clean, compile, install` (or `deploy`)

### Automated build

From the project root:

```bash
mvn clean package
```

This compiles the Java sources, builds the JNI objects, and assembles the JAR with `libquirc.so` inside `target/`.

## Project structure

```
libquirc-java-wrapper/
 ├── src/main/java           # Java wrapper classes
 │   └── org/libquirc/*
 ├── src/main/c              # JNI glue code
 ├── src/main/resources/
 │   └── jni/linux           # Pre‑built shared library (.so)
 ├── pom.xml                 # Maven build file
 ├── LICENSE                 # MIT licence (wrapper code)
 ├── NOTICE                  # Full copyright notice
 └── README.md               # You are here
```

## Contributing

Issues and pull requests are welcome!

## Credits

* **Daniel Beer** – author of the original [quirc](https://github.com/dlbeer/quirc) library.
* **Matteo Riva** – Java/JNI wrapper.

See `NOTICE` for full attribution details.

## Licence

This wrapper code is licensed under the **MIT License**; the bundled quirc C library remains under the **ISC License**.  See [LICENSE](LICENSE) for details.

---
