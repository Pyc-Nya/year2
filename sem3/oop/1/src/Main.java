import java.util.Random;
import java.util.Arrays;

/**
 * @author Sharpinskiy Denis 3311
 * */

public class Main {

    /**
     * Main method of the application. Generates an array of 10 random integers,
     * sorts them using the bubble sort algorithm, and displays them in both ascending and descending order.
     *
     * @param args arguments from the command line
     */
    public static void main(String[] args) {
        int[] Arr = new int[10];
        Random rand = new Random();

        for (int i = 0; i < Arr.length; i++) {
            Arr[i] = rand.nextInt(100);
        }

        System.out.println("Original Array: " + Arrays.toString(Arr));

        bubbleSort(Arr, true);
        System.out.println("Sorted Array (ascending): " + Arrays.toString(Arr));

        bubbleSort(Arr, false);
        System.out.println("Sorted Array (descending): " + Arrays.toString(Arr));
    }

    /**
     * Sorts an array of integers using the bubble sort algorithm.
     * The method compares adjacent elements and swaps them if they are in the wrong order.
     * The order is determined by the 'ascending' parameter.
     *
     * @param array the array of integers to be sorted
     * @param ascending if true, sorts the array in ascending order, otherwise sorts in descending order
     */
    public static void bubbleSort(int[] array, boolean ascending) {
        int n = array.length;
        boolean swapped;
        for (int i = 0; i < n - 1; i++) {
            swapped = false;
            for (int j = 0; j < n - 1 - i; j++) {
                if ((array[j] > array[j + 1] && ascending) || (array[j] < array[j + 1] && !ascending)) {
                    int temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                    swapped = true;
                }
            }
            if (!swapped) {
                break;
            }
        }
    }
}
