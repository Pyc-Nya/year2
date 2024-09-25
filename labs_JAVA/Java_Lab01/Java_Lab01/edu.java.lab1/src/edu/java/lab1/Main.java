package edu.java.lab1;

import java.util.Random;
import java.util.Arrays;

/**
 * Main class for the application.
 *
 * @author Kostenko Aleksey 3311
 * @version 1.0
 */
public class Main {

    /**
     * The main method of the application. Generates an array of 10 random numbers,
     * sorts them and prints them in ascending order.
     *
     * @param args command line arguments
     */
    public static void main(String[] args) {
        // Create an array of 10 integers
        int[] array = new int[10];

        // Create a Random object to generate random numbers
        Random rand = new Random();

        // Fill the array with random numbers between -1000 and 1000
        for (int i = 0; i < array.length; i++) {
            array[i] = rand.nextInt(-1000, 1000);
        }

        // Print the original array
        System.out.println("Original Array: " + Arrays.toString(array));

        // Sort the array in ascending order
        sort(array, true);

        // Print the sorted array in ascending order
        System.out.println("Sorted Array (ascending): " + Arrays.toString(array));

        // Sort the array in descending order
        sort(array, false);

        // Print the sorted array in descending order
        System.out.println("Sorted Array (descending): " + Arrays.toString(array));
    }

    /**
     * Sorts the given array in ascending or descending order based on the flag.
     *
     * @param array the array to be sorted
     * @param flag  true for ascending order, false for descending order
     */
    public static void sort(int[] array, boolean flag) {
        // Get the length of the array
        int len = array.length;

        // Flag to check if any swaps were made in the inner loop
        boolean n;

        // Outer loop to iterate over the array
        for (int i = 0; i < len - 1; i++) {
            // Initialize the flag to false
            n = false;

            // Inner loop to compare adjacent elements
            for (int j = 0; j < len - i - 1; j++) {
                // Check if the elements need to be swapped based on the flag
                if ((array[j] > array[j + 1] && flag) || (array[j] < array[j + 1] && !flag)) {
                    // Swap the elements
                    int temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;

                    // Set the flag to true
                    n = true;
                }
            }

            // If no swaps were made in the inner loop, the array is already sorted
            if (!n) break;
        }
    }
}