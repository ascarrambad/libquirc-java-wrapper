package org.libquirc;


import java.awt.image.BufferedImage;

public class QuircImage {
    private final int[] data;

    private final int width;
    private final int height;

    public QuircImage(int width, int height, int[] data) {
        this.width = width;
        this.height = height;
        this.data = data;
    }

    public QuircImage(BufferedImage image) {
        if (image.getType() != BufferedImage.TYPE_BYTE_GRAY) {
            throw new RuntimeException("Only TYPE_BYTE_GRAY type is supported.");
        }

        this.width = image.getWidth();
        this.height = image.getHeight();
        this.data = image.getRGB(0, 0, this.width, this.height, null, 0, width);
    }

    public int getHeight() {
        return this.height;
    }

    public int getWidth() {
        return this.width;
    }

    public int[] getData() {
        return this.data;
    }

    /**
     * Decodes an image using the quirc library.
     * @return The number of QR codes detected (or a negative value on error).
     */
    public native String[] decode();
}
