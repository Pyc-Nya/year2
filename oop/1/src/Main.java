import java.util.Random;
import java.util.Arrays;

/**
 * @author Sharpinskiy Denis 3311
 * */

public class Main {
    /**
     * Main method of the application. Generates an array of 10 random integers,
     * sorts them and displays then in ascending order.
     *
     * @param args an arguments of command line
     * */
    public static void main(String[] args) {
        int[] Arr = new int[10];
        Random rand = new Random();

        for (int i = 0; i < Arr.length; i++) {
            Arr[i] = rand.nextInt(100);
        }

        System.out.println("Original Array: " + Arrays.toString(Arr));
        Arrays.sort(Arr);
        System.out.println("Sorted Array: " + Arrays.toString(Arr));
    }
}
