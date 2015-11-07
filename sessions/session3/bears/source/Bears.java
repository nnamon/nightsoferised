import java.util.*;

public class Bears {

    public boolean hotdog(int isdead) {
        if (isdead % 42 == 0) {
            return true;
        }
        return false;
    }

     public boolean partypat(int lasers) {
        int meow = 0;
        for (int i = 0; i < 3; i++) {
            meow = meow*i + lasers;
        }
        if (meow == 0x14b4a) {
            return true;
        }
        return false;
    }

    public boolean treetrunks(int bubblegum) {
        long val = (bubblegum*bubblegum)*2 + bubblegum + 0x1337;
        long bignum = 13999614;
        if (val == bignum) {
            return true;
        }
        return false;
    }

    public static void main(String[] argv) {
        Scanner sc = new Scanner(System.in);
        int first = sc.nextInt();
        int second = sc.nextInt();
        int third = sc.nextInt();
        Bears bear = new Bears();
        if (bear.hotdog(first)) {
            System.out.println("You have passed the first test of DOOM!");
            if (bear.partypat(second)) {
                System.out.println("You have passed the second test of POTATOES!");
                if (bear.treetrunks(third)) {
                    System.out.println("You have passed the third test of EPICAL!");
                    System.out.println("Mathematical! You win!");
                }
            }
        }
        else {
            System.out.println("Aww, you suck! Cheque please!");
        }
    }
}
